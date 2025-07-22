#include "bankid/bankid.h"
#include <iostream>
#include <chrono>
#include <thread>

namespace BankID
{
  // BankIDConfig constructor implementations

  // Main constructor - only endUserIp is required
  BankIDConfig::BankIDConfig(const std::string &endUserIp, const SSLConfig &sslConfig)
      : m_endUserIp(endUserIp), m_sslConfig(sslConfig)
  {
    std::cout << "BankID Config: Basic configuration created" << std::endl;
    std::cout << "  End User IP: " << m_endUserIp << std::endl;
    std::cout << "  SSL Environment: " << (m_sslConfig.environment == Environment::TEST ? "TEST" : "PRODUCTION") << std::endl;
    std::cout << "  CA File: " << m_sslConfig.ca_file_path << std::endl;
  }

  // Static factory methods
  BankIDConfig BankIDConfig::createSimple(const std::string &endUserIp, const SSLConfig &sslConfig)
  {
    std::cout << "BankID Config: Creating simple configuration" << std::endl;
    return BankIDConfig(endUserIp, sslConfig);
  }

  BankIDConfig BankIDConfig::createApp(const std::string &endUserIp, const AppConfig &appConfig, const SSLConfig &sslConfig)
  {
    std::cout << "BankID Config: Creating app-based configuration" << std::endl;
    BankIDConfig config(endUserIp, sslConfig);
    config.setAppConfig(appConfig);

    std::cout << "  App Config:" << std::endl;
    std::cout << "    App Identifier: " << appConfig.appIdentifier << std::endl;
    std::cout << "    Device OS: " << appConfig.deviceOS << std::endl;
    std::cout << "    Device Model: " << appConfig.deviceModelName << std::endl;
    std::cout << "    Device ID: " << appConfig.deviceIdentifier << std::endl;

    return config;
  }

  BankIDConfig BankIDConfig::createWeb(const std::string &endUserIp, const WebConfig &webConfig, const SSLConfig &sslConfig)
  {
    std::cout << "BankID Config: Creating web-based configuration" << std::endl;
    BankIDConfig config(endUserIp, sslConfig);
    config.setWebConfig(webConfig);

    std::cout << "  Web Config:" << std::endl;
    std::cout << "    Device ID: " << webConfig.deviceIdentifier << std::endl;
    std::cout << "    Referring Domain: " << webConfig.referringDomain << std::endl;
    std::cout << "    User Agent: " << webConfig.userAgent << std::endl;

    return config;
  }

  // Session class implementation
  Session::Session(const BankIDConfig &config)
      : m_config(config), m_initialized(false), m_current_token("")
  {
    std::cout << "BankID Session: Creating session with config for End User IP: " << m_config.getEndUserIp() << std::endl;
    m_initialized = initialize();
  }

  Session::~Session()
  {
    std::cout << "BankID Session: Destroying session" << std::endl;
  }

  bool Session::initialize()
  {
    std::cout << "BankID Session: Initializing session for " << m_config.getEndUserIp() << std::endl;
    // Here you would typically initialize SSL context, validate certificates, etc.
    if (!m_config.getSSLConfig().validate())
    {
      std::cout << "BankID Session: SSL configuration validation failed" << std::endl;
      return false;
    }
    std::cout << "BankID Session: Session initialized successfully" << std::endl;
    return true;
  }

  /**
   * Start authentication process and return a token.
   * This simulates the process of starting an authentication session.
   * @return A string token representing the authentication session.
   * If the session is not initialized, it returns an empty string.
   */
  std::string Session::startAuthentication()
  {
    if (!m_initialized)
    {
      std::cout << "BankID Session: Session not initialized" << std::endl;
      return "";
    }
    std::cout << "BankID Session: Starting authentication for " << m_config.getEndUserIp() << std::endl;

    httplib::SSLClient cli("appapi2.test.bankid.com", 443,
                           m_config.getSSLConfig().pem_cert_path.c_str(),
                           m_config.getSSLConfig().pem_key_path.c_str());

    // Set CA certificate for server verification
    cli.set_ca_cert_path(m_config.getSSLConfig().ca_file_path.c_str());

    cli.enable_server_certificate_verification(true);
    cli.enable_server_hostname_verification(m_config.getSSLConfig().environment == Environment::PRODUCTION);

    // Set connection timeout (optional)
    cli.set_connection_timeout(30); // 30 seconds
    cli.set_read_timeout(30);       // 30 seconds

    auto res = cli.Post("/rp/v6.0/auth",
                        R"({"endUserIp":")" + m_config.getEndUserIp() + R"("})", 
                        "application/json");

    // Log the response
    if (res)
    {
      if (res->status == 200)
      {
        std::cout << "Response: " << res->body << std::endl;
      }
      else
      {
        std::cout << "HTTP Error: " << res->status << std::endl;
      }
    }
    else
    {
      auto err = res.error();
      std::cout << "Request failed: " << httplib::to_string(err) << std::endl;
    }

    return m_current_token;
  }

  std::string Session::checkStatus()
  {
    return checkStatus(m_current_token);
  }

  std::string Session::checkStatus(const std::string &token)
  {
    if (!m_initialized)
    {
      std::cout << "BankID Session: Session not initialized" << std::endl;
      return "ERROR";
    }

    if (token.empty())
    {
      std::cout << "BankID Session: No token provided" << std::endl;
      return "ERROR";
    }

    std::cout << "BankID Session: Checking authentication status for token: " << token
              << " with End User IP: " << m_config.getEndUserIp() << std::endl;

    // Simulate authentication status check
    return "COMPLETED";
  }

}
