#pragma once

#include "RdWebServer.h"
#include "GenResources.h"
#include "Display.h"
#include "Setting.h"

class LocalServer {

    public:

        enum ServerStatus {
            IDLE = 0,
            OPEN_REQUESTED,
            SETTING_SAVED
        };

        LocalServer(Display* display, Setting* setting);

        void setup();
        bool service(ServerStatus* status);

    private:
        RdWebServer* _webServer;
        RestAPIEndpoints _restAPIEndpoints;
        Display* _display;
        Setting* _setting;

        ServerStatus _lastStatus;

        void restAPI_PostSettings(RestAPIEndpointMsg& apiMsg, String& retStr);
        void restAPI_GetSettings(RestAPIEndpointMsg& apiMsg, String& retStr);
        void restAPI_GetOpenOperation(RestAPIEndpointMsg& apiMsg, String& retStr);

        void restAPI_PostLogin(RestAPIEndpointMsg& apiMsg, String& retStr);
        void restAPI_PostChangePassword(RestAPIEndpointMsg& apiMsg, String& retStr);

        String _token;
        unsigned long _tokenTime;
        String _generateToken();
        bool _isTokenValid(String token);
        bool _isTokenExpired();
        String _getTokenFromHeader(char* headerString);
        bool _isTokenExistedAndValid(char* headerString);
};
