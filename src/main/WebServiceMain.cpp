#include "Poco/Net/HTTPServer.h"
#include "Poco/Net/HTTPRequestHandler.h"
#include "Poco/Net/HTTPRequestHandlerFactory.h"
#include "Poco/Net/HTTPServerRequest.h"
#include "Poco/Net/HTTPServerResponse.h"
#include "Poco/Net/ServerSocket.h"
#include "Poco/Util/ServerApplication.h"
#include <iostream>

using namespace Poco;
using namespace Poco::Net;
using namespace Poco::Util;


class HelloRequestHandler : public HTTPRequestHandler {
    void handleRequest(HTTPServerRequest &request, HTTPServerResponse &response) override {
        Application &app = Application::instance();
        app.logger().information("Request from %s", request.clientAddress().toString());

        response.setChunkedTransferEncoding(true);
        response.setContentType("text/html");

        response.send()
                << "<html>"
                << "<head><title>Hello</title></head>"
                << "<body><h1>Hello from the POCO Web Server</h1></body>"
                << "</html>";
    }
};

class HelloRequestHandlerFactory : public HTTPRequestHandlerFactory {
    HTTPRequestHandler *createRequestHandler(const HTTPServerRequest &) override {
        return new HelloRequestHandler;
    }
};

class WebServerApp : public ServerApplication {
    void initialize(Application &self) override {
        loadConfiguration();
        ServerApplication::initialize(self);
    }

    int main(const std::vector<std::string> &) override {
        UInt16 port = static_cast<UInt16>(config().getUInt("port", 8080));

        HTTPServer srv(new HelloRequestHandlerFactory, port);
        srv.start();
        logger().information("HTTP Server started on port %hu.", port);
        waitForTerminationRequest();
        logger().information("Stopping HTTP Server...");
        srv.stop();

        return Application::EXIT_OK;
    }
};

POCO_SERVER_MAIN(WebServerApp)