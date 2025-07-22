#include "bankid/bankid.h"
#include "helper.h"
#include <crow.h>
#include <memory>
#include <string>
#include <iostream>
#include <filesystem>

// Helper function to check if file exists
bool file_exists(const std::string &path)
{
  return std::filesystem::exists(path);
}

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
    if (!initialized)
    {
      std::cerr << "Failed to initialize BankID library with configuration" << std::endl;
    }
    else
    {
      std::cout << "BankID library initialized successfully with configuration" << std::endl;
    }
  }

  ~BankIDServer()
  {
    if (initialized)
    {
      BankID::Shutdown();
    }
  }

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
  // Test environment (default)
  BankID::SSLConfig test_ssl_config(
      BankID::Environment::TEST,
      "certs/FPTestcert5_20240610.p12", // P12 certificate file
      "qwerty123"                       // P12 passphrase
  );

  // Verify certificate files exist
  if (!file_exists(test_ssl_config.p12_file_path))
  {
    std::cerr << "Warning: P12 certificate file not found: " << test_ssl_config.p12_file_path << std::endl;
    std::cerr << "Please ensure the BankID certificate is placed in the certs/ directory" << std::endl;
    return 0;
  }

  if (!file_exists(test_ssl_config.ca_file_path))
  {
    std::cerr << "Warning: CA certificate file not found: " << test_ssl_config.ca_file_path << std::endl;
    std::cerr << "Please ensure the CA certificate is placed in the certs/ directory" << std::endl;
    return 0;
  }

  // Create BankID configuration for your project
  BankID::BankIDConfig simple_config(
      "172.0.0.1",                                 // endUserIp
      "https://yourapp.example.com/auth/callback", // returnUrl
      test_ssl_config                              // SSL configuration
  );

  // Example 2: Enhanced authentication with user visible data and SSL
  // BankID::BankIDConfig enhanced_config(
  //   "192.168.1.100", // endUserIp
  //   "https://yourapp.example.com/auth/callback", // returnUrl
  //   "VGhpcyBpcyBhIHNhbXBsZSB0ZXh0IHRvIGJlIHNpZ25lZA==", // base64 encoded user visible data
  //   true, // returnRisk
  //   test_ssl_config // SSL configuration
  // );

  // Example 3: App-based authentication with SSL
  // BankID::AppConfig app_config{
  //   "com.yourcompany.yourapp", // appIdentifier
  //   "iOS 17.1", // deviceOS
  //   "iPhone15,2", // deviceModelName
  //   "a1b2c3d4e5f6789012345678901234567890abcdef1234567890abcdef123456" // deviceIdentifier
  // };
  // BankID::BankIDConfig app_config_bankid(
  //   "192.168.1.100",
  //   "https://yourapp.example.com/auth/callback",
  //   "VGhpcyBpcyBhIHNhbXBsZSB0ZXh0IHRvIGJlIHNpZ25lZA==",
  //   app_config,
  //   true,
  //   test_ssl_config
  // );

  // Example 4: Web-based authentication with SSL
  // BankID::WebConfig web_config{
  //   "yourapp.example.com", // referringDomain
  //   "Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36", // userAgent
  //   "w1x2y3z4a5b6c7d8e9f0123456789012345678901234567890abcdef123456" // deviceIdentifier
  // };
  // BankID::BankIDConfig web_config_bankid(
  //   "192.168.1.100",
  //   "https://yourapp.example.com/auth/callback",
  //   "VGhpcyBpcyBhIHNhbXBsZSB0ZXh0IHRvIGJlIHNpZ25lZA==",
  //   web_config,
  //   true,
  //   test_ssl_config
  // );

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
