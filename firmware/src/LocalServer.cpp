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

// Post settings information via API
void LocalServer::restAPI_PostSettings(RestAPIEndpointMsg& apiMsg, String& retStr) {
    if (apiMsg._pMsgHeader) {
        Log.trace("RestAPI PostSettings header len %d", strlen(apiMsg._pMsgHeader));
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
    retStr = "{\"ok\"}";

    _lastStatus = SETTING_SAVED;
}

// Get settings information via API
void LocalServer::restAPI_GetSettings(RestAPIEndpointMsg& apiMsg, String& retStr) {
    ConfigurationTime reminderTime = _setting->getReminderTime();
    String reminderTimeString = String(reminderTime.hour) + ":" + String(reminderTime.minute);
    ConfigurationTime openTime = _setting->getOpenTime();
    String openTimeString = String(openTime.hour) + ":" + String(openTime.minute);

    String configStr = "{\"openTime\":\"" + openTimeString +"\", \"reminderTime\":\"" + reminderTimeString + "\"}";
    retStr = configStr;
}

void LocalServer::restAPI_GetOpenOperation(RestAPIEndpointMsg& apiMsg, String& retStr) {
    String configStr = "{\"status\":\"ok\"}";
    retStr = configStr;
    _lastStatus = OPEN_REQUESTED;
}
