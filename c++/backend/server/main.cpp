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
      "certs/FPTestcert5_20240610.p12", // P12 certificate file
      "qwerty123"                       // P12 passphrase
  );

  if (!test_ssl_config.validate())
  {
    std::cout << "SSL configuration validation failed. Exiting." << std::endl;
    return 1;
  }

  // Create BankID configuration for your project
  BankID::BankIDConfig simple_config(
      "172.0.0.1",                                 // endUserIp
      "https://yourapp.example.com/auth/callback", // returnUrl
      test_ssl_config                              // SSL configuration
  );

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
        
        std::string token = bankid_session.auth("12345678901");
        
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

  // Configure SSL if certificates are available
  const auto &ssl_config = simple_config.getSSLConfig();

  // Check for PEM certificates first (preferred for Crow)
  if (file_exists(ssl_config.pem_combined_path) && file_exists(ssl_config.p12_file_path))
  {
    log_certificates_found(ssl_config);
    log_starting_server();

    app.port(8080).multithreaded().run();
  }
  else
  {
    log_error_no_ssl_found(ssl_config);
    return 0;
  }

  return 0;
}
