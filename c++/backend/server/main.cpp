#include "bankid/bankid.h"
#include "helper.h"
#include <crow.h>
#include <memory>
#include <string>

int main()
{
  // SSL Configuration for BankID
  BankID::SSLConfig test_ssl_config(
      BankID::Environment::TEST,
      "certs/bankid_cert.pem",
      "certs/bankid_key.pem");

  if (!test_ssl_config.validate())
  {
    std::cout << "SSL configuration validation failed. Exiting." << std::endl;
    return 1;
  }

  // Create BankID configuration for your project using the simple factory method
  BankID::BankIDConfig simple_config = BankID::BankIDConfig::createSimple(
      "172.0.0.1",    // endUserIp
      test_ssl_config // SSL configuration
  );

  // Set optional fields using fluent interface
  simple_config.setReturnUrl("https://yourapp.example.com/auth/callback")
      .setReturnRisk(true)
      .setUserVisibleData("VEhJUyBJUyBBIFRFU1Q=");

  // Create your BankID session instance
  BankID::Session bankid_session(simple_config);

  if (!bankid_session.isInitialized())
  {
    std::cout << "BankID session initialization failed. Exiting." << std::endl;
    return 1;
  }

  crow::SimpleApp app;

  // GET /init endpoint
  CROW_ROUTE(app, "/init")
  ([&bankid_session](const crow::request &)
   {
        std::cout << "GET /init - Starting authentication" << std::endl;
        std::string token = bankid_session.auth();

        if (token.empty())
        {
          return crow::response(500, "Failed to start authentication");
        }

        std::string json_response = "{"
                                    "\"status\":\"success\","
                                    "\"token\":\"" + token + "\","
                                    "\"message\":\"Authentication initiated\""
                                    "}";

        crow::response resp(200, json_response);
        resp.add_header("Content-Type", "application/json");
        return resp; });

  // GET /poll endpoint
  CROW_ROUTE(app, "/poll")
  ([&bankid_session](const crow::request &)
   {
        std::cout << "GET /poll - Checking authentication status" << std::endl;
        
        std::string current_token = bankid_session.getCurrentToken();
        if (current_token.empty())
        {
          return crow::response(400, "No active authentication session");
        }

        std::string status = bankid_session.checkStatus();

        // Simple JSON response without external JSON library
        std::string json_response = "{"
                                    "\"status\":\"success\","
                                    "\"token\":\"" + current_token + "\","
                                    "\"auth_status\":\"" + status + "\""
                                    "}";

        crow::response resp(200, json_response);
        resp.add_header("Content-Type", "application/json");
        return resp; });

  log_starting_server();
  app.port(8080).multithreaded().run();

  return 0;
}
