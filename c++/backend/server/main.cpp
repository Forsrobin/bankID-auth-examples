#include "bankid.h"
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

  // Create your BankID session instance with just SSL config
  BankID::Session bankid_session(test_ssl_config);

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
        
        // Create auth config on-demand for this specific request
        auto authConfig = BankID::API::AuthConfig::createSimple("172.0.0.1")
            .setReturnUrl("https://yourapp.example.com/auth/callback")
            .setReturnRisk(true)
            .setUserVisibleData("VEhJUyBJUyBBIFRFU1Q=");
        
        // Optionally add app-specific config
        BankID::AppConfig app_config{
            "com.yourcompany.yourapp",
            "iOS 17.1", 
            "a1b2c3d4e5f6789012345678901234567890abcdef1234567890abcdef123456",
            "iPhone15,2"
        };
        authConfig.setAppConfig(app_config);

        auto response = bankid_session.auth(authConfig);


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

  // GET /payment endpoint - Example of payment API usage
  CROW_ROUTE(app, "/payment")
  ([&bankid_session](const crow::request &)
   {
        std::cout << "GET /payment - Starting payment flow" << std::endl;
        
        // Create payment config on-demand for this specific request
        auto paymentConfig = BankID::API::PaymentConfig::createCardPayment(
            "172.0.0.1",          // endUserIp
            "Test Merchant Inc.", // recipient name
            "100,00",            // amount
            "SEK"                // currency
        )
        .setReturnUrl("https://yourapp.example.com/payment/callback")
        .setReturnRisk(true)
        .setUserVisibleData("UGF5bWVudCBmb3IgdGVzdCBwdXJjaGFzZQ==") // base64 encoded
        .setRiskFlags({"largeAmount", "newCustomer"});

        auto response = bankid_session.payment(paymentConfig);

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

  log_starting_server();
  app.port(8080).multithreaded().run();

  return 0;
}
