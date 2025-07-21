#include "bankid/bankid.h"
#include <memory>
#include <string>
#include <iostream>

#include "crow.h"

class BankIDServer {
private:
    std::string current_token;
    bool initialized = false;

public:
    BankIDServer() {
        initialized = BankID::Initialize();
        if (!initialized) {
            std::cerr << "Failed to initialize BankID library" << std::endl;
        }
    }

    ~BankIDServer() {
        if (initialized) {
            BankID::Shutdown();
        }
    }

    crow::response init_authentication(const std::string& personal_number = "199001011234") {
        if (!initialized) {
            return crow::response(500, "BankID library not initialized");
        }

        current_token = BankID::StartAuthentication(personal_number);
        
        if (current_token.empty()) {
            return crow::response(500, "Failed to start authentication");
        }

        // Simple JSON response without external JSON library
        std::string json_response = "{"
            "\"status\":\"success\","
            "\"token\":\"" + current_token + "\","
            "\"message\":\"Authentication initiated\""
            "}";
        
        crow::response resp(200, json_response);
        resp.add_header("Content-Type", "application/json");
        return resp;
    }

    crow::response poll_authentication() {
        if (!initialized) {
            return crow::response(500, "BankID library not initialized");
        }

        if (current_token.empty()) {
            return crow::response(400, "No active authentication session");
        }

        std::string status = BankID::CheckAuthenticationStatus(current_token);

        // Simple JSON response without external JSON library
        std::string json_response = "{"
            "\"status\":\"success\","
            "\"token\":\"" + current_token + "\","
            "\"auth_status\":\"" + status + "\""
            "}";

        crow::response resp(200, json_response);
        resp.add_header("Content-Type", "application/json");
        return resp;
    }
};

int main() {
    crow::SimpleApp app;

    // Create BankID server instance
    auto bankid_server = std::make_shared<BankIDServer>();

    // GET /init endpoint
    CROW_ROUTE(app, "/init")
    ([bankid_server](const crow::request&) {
        std::cout << "GET /init - Starting authentication" << std::endl;
        return bankid_server->init_authentication();
    });

    // GET /poll endpoint
    CROW_ROUTE(app, "/poll")
    ([bankid_server](const crow::request&) {
        std::cout << "GET /poll - Checking authentication status" << std::endl;
        return bankid_server->poll_authentication();
    });

    // Start server
    std::cout << "Starting BankID REST API server on port 8080..." << std::endl;
    app.port(8080).multithreaded().run();

    return 0;
}
