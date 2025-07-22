#include "bankid/bankid.h"
#include <iostream>
#include <chrono>
#include <thread>

namespace BankID
{
  // BankIDConfig constructor implementations

  // 1. Simple authentication constructor
  BankIDConfig::BankIDConfig(const std::string &endUserIp, const std::string &returnUrl,
                             const SSLConfig &sslConfig)
      : m_endUserIp(endUserIp), m_returnUrl(returnUrl), m_returnRisk(false), m_sslConfig(sslConfig)
  {
    std::cout << "BankID Config: Simple authentication configuration created" << std::endl;
    std::cout << "  End User IP: " << m_endUserIp << std::endl;
    std::cout << "  Return URL: " << m_returnUrl << std::endl;
    std::cout << "  SSL Environment: " << (m_sslConfig.environment == Environment::TEST ? "TEST" : "PRODUCTION") << std::endl;
    std::cout << "  P12 File: " << m_sslConfig.p12_file_path << std::endl;
    std::cout << "  CA File: " << m_sslConfig.ca_file_path << std::endl;
  }

  // 2. Authentication with user visible data and risk
  BankIDConfig::BankIDConfig(const std::string &endUserIp, const std::string &returnUrl,
                             const std::string &userVisibleData, bool returnRisk,
                             const SSLConfig &sslConfig)
      : m_endUserIp(endUserIp), m_returnUrl(returnUrl),
        m_userVisibleData(userVisibleData), m_returnRisk(returnRisk), m_sslConfig(sslConfig)
  {
    std::cout << "BankID Config: Enhanced authentication configuration created" << std::endl;
    std::cout << "  End User IP: " << m_endUserIp << std::endl;
    std::cout << "  Return URL: " << m_returnUrl << std::endl;
    std::cout << "  User Visible Data: " << m_userVisibleData.value() << std::endl;
    std::cout << "  Return Risk: " << (m_returnRisk ? "true" : "false") << std::endl;
    std::cout << "  SSL Environment: " << (m_sslConfig.environment == Environment::TEST ? "TEST" : "PRODUCTION") << std::endl;
    std::cout << "  P12 File: " << m_sslConfig.p12_file_path << std::endl;
    std::cout << "  CA File: " << m_sslConfig.ca_file_path << std::endl;
  }

  // 3. App-based authentication constructor
  BankIDConfig::BankIDConfig(const std::string &endUserIp, const std::string &returnUrl,
                             const std::string &userVisibleData, const AppConfig &appConfig,
                             bool returnRisk, const SSLConfig &sslConfig)
      : m_endUserIp(endUserIp), m_returnUrl(returnUrl),
        m_userVisibleData(userVisibleData), m_returnRisk(returnRisk), m_appConfig(appConfig), m_sslConfig(sslConfig)
  {
    std::cout << "BankID Config: App-based authentication configuration created" << std::endl;
    std::cout << "  End User IP: " << m_endUserIp << std::endl;
    std::cout << "  Return URL: " << m_returnUrl << std::endl;
    std::cout << "  User Visible Data: " << m_userVisibleData.value() << std::endl;
    std::cout << "  Return Risk: " << (m_returnRisk ? "true" : "false") << std::endl;
    std::cout << "  App Config:" << std::endl;
    std::cout << "    App Identifier: " << m_appConfig->appIdentifier << std::endl;
    std::cout << "    Device OS: " << m_appConfig->deviceOS << std::endl;
    std::cout << "    Device Model: " << m_appConfig->deviceModelName << std::endl;
    std::cout << "    Device ID: " << m_appConfig->deviceIdentifier << std::endl;
    std::cout << "  SSL Environment: " << (m_sslConfig.environment == Environment::TEST ? "TEST" : "PRODUCTION") << std::endl;
    std::cout << "  P12 File: " << m_sslConfig.p12_file_path << std::endl;
    std::cout << "  CA File: " << m_sslConfig.ca_file_path << std::endl;
  }

  // 4. Web-based authentication constructor
  BankIDConfig::BankIDConfig(const std::string &endUserIp, const std::string &returnUrl,
                             const std::string &userVisibleData, const WebConfig &webConfig,
                             bool returnRisk, const SSLConfig &sslConfig)
      : m_endUserIp(endUserIp), m_returnUrl(returnUrl),
        m_userVisibleData(userVisibleData), m_returnRisk(returnRisk), m_webConfig(webConfig), m_sslConfig(sslConfig)
  {
    std::cout << "BankID Config: Web-based authentication configuration created" << std::endl;
    std::cout << "  End User IP: " << m_endUserIp << std::endl;
    std::cout << "  Return URL: " << m_returnUrl << std::endl;
    std::cout << "  User Visible Data: " << m_userVisibleData.value() << std::endl;
    std::cout << "  Return Risk: " << (m_returnRisk ? "true" : "false") << std::endl;
    std::cout << "  Web Config:" << std::endl;
    std::cout << "    Referring Domain: " << m_webConfig->referringDomain << std::endl;
    std::cout << "    User Agent: " << m_webConfig->userAgent << std::endl;
    std::cout << "    Device ID: " << m_webConfig->deviceIdentifier << std::endl;
    std::cout << "  SSL Environment: " << (m_sslConfig.environment == Environment::TEST ? "TEST" : "PRODUCTION") << std::endl;
    std::cout << "  P12 File: " << m_sslConfig.p12_file_path << std::endl;
    std::cout << "  CA File: " << m_sslConfig.ca_file_path << std::endl;
  }

  // New functions using BankIDConfig
  bool Initialize(const BankIDConfig &config)
  {
    std::cout << "BankID Library: Initializing" << config.getEndUserIp() << std::endl;
    return true;
  }

  std::string StartAuthentication(const BankIDConfig &config)
  {
    std::cout << "BankID Library: Starting authentication..." << config.getEndUserIp() << std::endl;
    return "config_auth_token_";
  }

  std::string CheckAuthenticationStatus(const std::string &token, const BankIDConfig &config)
  {
    std::cout << "BankID Library: Checking authentication status for token: " << token << " with End User IP: " << config.getEndUserIp() << std::endl;
    return "COMPLETED";
  }
}
