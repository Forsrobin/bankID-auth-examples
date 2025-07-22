#include "bankid/bankid.h"
#include "helper.h"
#include <crow.h>
#include <memory>
#include <string>

class BankIDServer
{
private:
  std::string current_token;
  bool initialized = false;
  BankID::BankIDConfig config;

public:
  BankIDServer(const BankID::BankIDConfig &bankid_config)
      : config(bankid_config)
  {
    initialized = BankID::Initialize(config);
  }

  ~BankIDServer() {}

  crow::response init_authentication(const std::string &personal_number)
  {
    if (!initialized)
    {
      return crow::response(500, "BankID library not initialized");
    }

    BankID::BankIDConfig auth_config = config;

    if (!personal_number.empty())
    {
      BankID::Requirement req;
      req.personalNumber = personal_number;
      auth_config.setRequirement(req);
    }

    current_token = BankID::StartAuthentication(auth_config);

    if (current_token.empty())
    {
      return crow::response(500, "Failed to start authentication");
    }

    std::string json_response = "{"
                                "\"status\":\"success\","
                                "\"token\":\"" +
                                current_token + "\","
                                                "\"message\":\"Authentication initiated\""
                                                "}";

    crow::response resp(200, json_response);
    resp.add_header("Content-Type", "application/json");
    return resp;
  }

  crow::response poll_authentication()
  {
    if (!initialized)
    {
      return crow::response(500, "BankID library not initialized");
    }

    if (current_token.empty())
    {
      return crow::response(400, "No active authentication session");
    }

    // Use the config-based status checking
    std::string status = BankID::CheckAuthenticationStatus(current_token, config);

    // Simple JSON response without external JSON library
    std::string json_response = "{"
                                "\"status\":\"success\","
                                "\"token\":\"" +
                                current_token + "\","
                                                "\"auth_status\":\"" +
                                status + "\""
                                         "}";

    crow::response resp(200, json_response);
    resp.add_header("Content-Type", "application/json");
    return resp;
  }
};

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

  // Create your BankID server instance
  BankIDServer bankid_server(simple_config);

  crow::SimpleApp app;

  // GET /init endpoint
  CROW_ROUTE(app, "/init")
  ([&bankid_server](const crow::request &)
   {
        std::cout << "GET /init - Starting authentication" << std::endl;
        return bankid_server.init_authentication("12345678901"); });

  // GET /poll endpoint
  CROW_ROUTE(app, "/poll")
  ([&bankid_server](const crow::request &)
   {
        std::cout << "GET /poll - Checking authentication status" << std::endl;
        return bankid_server.poll_authentication(); });

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
