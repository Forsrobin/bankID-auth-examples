#include "bankid/bankid.h"
#include "helper.h"
#include <crow.h>
#include <memory>
#include <string>

#include <nlohmann/json.hpp>
using json = nlohmann::json;

int main()
{
  // SSL Configuration for BankID
  BankID::SSLConfig test_ssl_config(
      BankID::Environment::TEST,
      "certs/bankid_cert.pem",
      "certs/bankid_key.pem");

  if (!test_ssl_config.validate())
  {
    return 1;
  }

  // // Create BankID configuration for your project using the simple factory method
  // BankID::BankIDConfig simple_config = BankID::BankIDConfig::createSimple(
  //     "172.0.0.1",    // endUserIp
  //     test_ssl_config // SSL configuration
  // );

  // // Set optional fields using fluent interface
  // simple_config.setReturnUrl("https://yourapp.example.com/auth/callback")
  //     .setReturnRisk(true)
  //     .setUserVisibleData("VEhJUyBJUyBBIFRFU1Q=");

  BankID::AppConfig app_config{
      "com.yourcompany.yourapp",                                         // appIdentifier
      "iOS 17.1",                                                        // deviceOS
      "iPhone15,2",                                                      // deviceModelName
      "a1b2c3d4e5f6789012345678901234567890abcdef1234567890abcdef123456" // deviceIdentifier
  };

  BankID::BankIDConfig app_config_bankid = BankID::BankIDConfig::createApp(
      "172.0.0.1",
      app_config,
      test_ssl_config);

  app_config_bankid.setReturnUrl("https://yourapp.example.com/auth/callback")
      .setReturnRisk(true)
      .setUserVisibleData("VEhJUyBJUyBBIFRFU1Q=");


  // Create your BankID session instance
  BankID::Session bankid_session(app_config_bankid);

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
        auto response = bankid_session.auth();


        if (!response)
        {
          const auto &error = response.error();
          return crow::response(error.status, error.details);
        }

        // Simple JSON response using nlohmann::json
        json json_response;
        json_response["status"] = "success";
        json_response["orderRef"] = response->orderRef;
        json_response["autoStartToken"] = response->autoStartToken;


        crow::response resp(200, json_response.dump());
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


        // Simple JSON response without external JSON library
        std::string json_response = "{"
                                    "\"status\":\"success\","
                                    "\"token\":\"" + current_token + "\","
                                    "}";

        crow::response resp(200, json_response);
        resp.add_header("Content-Type", "application/json");
        return resp; });

  log_starting_server();
  app.port(8080).multithreaded().run();

  return 0;
}
