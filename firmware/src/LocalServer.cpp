#include "Particle.h"
#include "LocalServer.h"
#include "RestAPIEndpoints.h"
#include "RdJson.h"
#include <functional>

LocalServer::LocalServer(Display* display, Setting* setting): _display(display), _setting(setting) {
    _lastStatus = IDLE;
}

void LocalServer::setup() {
    using namespace std::placeholders;

    while (1) {
        if (WiFi.ready()) {
            break;
        }
        Particle.process();
    }

    // Add endpoint
    _restAPIEndpoints.addEndpoint("postLogin", RestAPIEndpointDef::ENDPOINT_CALLBACK, std::bind(&LocalServer::restAPI_PostLogin, this, _1, _2), "");
    _restAPIEndpoints.addEndpoint("postChangePassword", RestAPIEndpointDef::ENDPOINT_CALLBACK, std::bind(&LocalServer::restAPI_PostChangePassword, this, _1, _2), "");
    _restAPIEndpoints.addEndpoint("postSettings", RestAPIEndpointDef::ENDPOINT_CALLBACK, std::bind(&LocalServer::restAPI_PostSettings, this, _1, _2), "");
    _restAPIEndpoints.addEndpoint("getSettings", RestAPIEndpointDef::ENDPOINT_CALLBACK, std::bind(&LocalServer::restAPI_GetSettings, this, _1, _2), "");
    _restAPIEndpoints.addEndpoint("getOpenOperation", RestAPIEndpointDef::ENDPOINT_CALLBACK, std::bind(&LocalServer::restAPI_GetOpenOperation, this, _1, _2), "");

    // Construct server
    _webServer = new RdWebServer();

    // Configure web server
    if (_webServer) {
        // Add resources to web server
        _webServer->addStaticResources(genResources, genResourcesCount);

        // Endpoints
        _webServer->addRestAPIEndpoints(&_restAPIEndpoints);

        // Start the web server
        _webServer->start(80);
    }
}

bool LocalServer::service(ServerStatus* status) {
    if (_webServer) {
        _webServer->service();
    }

    if (_lastStatus == IDLE) {
        return false;
    }

    *status = _lastStatus;

    _lastStatus = IDLE;

    return true;
}

void LocalServer::restAPI_PostLogin(RestAPIEndpointMsg& apiMsg, String& retStr) {
    String postStr = (const char *)apiMsg._pMsgContent;
    String passwordString = RdJson::getString("password", "", postStr.c_str());
    String localPassword = _setting->getPassword();
    Serial.print("passwordString: ");Serial.println(passwordString);
    Serial.print("localPassword: ");Serial.println(localPassword);
    if (passwordString == localPassword) {
        // authorized
        _token = _generateToken();
        _tokenTime = millis();

        retStr = "{\"status\":\"ok\", \"token\":\"" + _token + "\"}";
    } else {
        retStr = "{\"status\":\"unauthorized\"}";
    }
}

void LocalServer::restAPI_PostChangePassword(RestAPIEndpointMsg& apiMsg, String& retStr) {
    if (!_isTokenExistedAndValid((char*)apiMsg._pMsgHeader)) {
        retStr = "{\"status\":\"unauthorized\"}";
        return;
    }

    String postStr = (const char *)apiMsg._pMsgContent;

    String oldPasswordString = RdJson::getString("oldPassword", "", postStr.c_str());
    String newPasswordString = RdJson::getString("newPassword", "", postStr.c_str());
    String localPassword = _setting->getPassword();
    Serial.print("oldPasswordString: ");Serial.println(oldPasswordString);
    Serial.print("newPasswordString: ");Serial.println(newPasswordString);
    Serial.print("localPassword: ");Serial.println(localPassword);
    if (oldPasswordString == localPassword) {
        _setting->setPassword(newPasswordString);
        _tokenTime = millis();

        retStr = "{\"status\":\"ok\"}";
    } else {
        retStr = "{\"status\":\"unauthorized\"}";
    }
}

void LocalServer::restAPI_PostSettings(RestAPIEndpointMsg& apiMsg, String& retStr) {
    if (!_isTokenExistedAndValid((char*)apiMsg._pMsgHeader)) {
        retStr = "{\"status\":\"unauthorized\"}";
        return;
    }

    String configStr = (const char *)apiMsg._pMsgContent;

    String openTimeString = RdJson::getString("openTime", "", configStr.c_str());
    String reminderTimeString = RdJson::getString("reminderTime", "", configStr.c_str());

    ConfigurationTime openTime;
    if (ConfigurationTime::fromString(openTimeString, &openTime)) {
        _setting->setOpenTime(openTime);
    }

    ConfigurationTime reminderTime;
    if (ConfigurationTime::fromString(reminderTimeString, &reminderTime)) {
        _setting->setReminderTime(reminderTime);
    }

    // Result
    retStr = "{\"status\":\"ok\"}";

    _lastStatus = SETTING_SAVED;
}

void LocalServer::restAPI_GetSettings(RestAPIEndpointMsg& apiMsg, String& retStr) {
    if (!_isTokenExistedAndValid((char*)apiMsg._pMsgHeader)) {
        retStr = "{\"status\":\"unauthorized\"}";
        return;
    }

    ConfigurationTime reminderTime = _setting->getReminderTime();
    String reminderTimeString = String(reminderTime.hour) + ":" + String(reminderTime.minute);
    ConfigurationTime openTime = _setting->getOpenTime();
    String openTimeString = String(openTime.hour) + ":" + String(openTime.minute);

    String configStr = "{\"openTime\":\"" + openTimeString +"\", \"reminderTime\":\"" + reminderTimeString + "\"}";
    retStr = configStr;
}

void LocalServer::restAPI_GetOpenOperation(RestAPIEndpointMsg& apiMsg, String& retStr) {
    if (!_isTokenExistedAndValid((char*)apiMsg._pMsgHeader)) {
        retStr = "{\"status\":\"unauthorized\"}";
        return;
    }

    retStr = "{\"status\":\"ok\"}";
    _lastStatus = OPEN_REQUESTED;
}

String LocalServer::_generateToken() {
    String token = "";
    for (unsigned int i=0; i<32; i++) {
        token += String(random(0, 10));
    }

    return token;
}

bool LocalServer::_isTokenValid(String token) {
    return (token == _token) && !_isTokenExpired();
}

bool LocalServer::_isTokenExpired() {
    return millis() - _tokenTime > 1000 * 60 * 5; // 5 minutes
}

String LocalServer::_getTokenFromHeader(char* headerString) {
    char* line = strtok(headerString, "\n");
    while (line != 0) {
        char* separator = strchr(line, ':');
        if (separator != 0) {
            *separator = 0;

            if (strcmp(line, "X-Token") == 0) {
                ++separator;
                return String(separator).trim();
            }
        }
        line = strtok(0, "\n");
    }

    return "";
}

bool LocalServer::_isTokenExistedAndValid(char* headerString) {
    String token = _getTokenFromHeader(headerString);
    return _isTokenValid(token);
}
