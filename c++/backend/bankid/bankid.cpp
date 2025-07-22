#include "bankid/bankid.h"

namespace BankID
{
  BankIDConfig::BankIDConfig(const std::string &endUserIp, const SSLConfig &sslConfig)
      : m_endUserIp(endUserIp), m_sslConfig(sslConfig)
  {
    std::cout << "BankID Config: Basic configuration created" << std::endl;
    std::cout << "  End User IP: " << m_endUserIp << std::endl;
    std::cout << "  SSL Environment: " << (m_sslConfig.environment == Environment::TEST ? "TEST" : "PRODUCTION") << std::endl;
    std::cout << "  CA File: " << m_sslConfig.ca_file_path << std::endl;
  }

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

  Session::Session(const BankIDConfig &config)
      : m_config(config), m_initialized(false), m_current_token("")
  {
    std::cout << "BankID Session: Creating session with config for End User IP: " << m_config.getEndUserIp() << std::endl;
    m_initialized = initialize();
  }

  Session::~Session()
  {
    delete cli;
    std::cout << "BankID Session: Destroying session" << std::endl;
  }

  const bool Session::initialize()
  {
    std::cout << "BankID Session: Initializing session for " << m_config.getEndUserIp() << std::endl;
    if (!m_config.getSSLConfig().validate())
    {
      std::cerr << "BankID Session: SSL configuration validation failed" << std::endl;
      return false;
    }

    this->cli = new httplib::SSLClient("appapi2.test.bankid.com", 443,
                                       m_config.getSSLConfig().pem_cert_path.c_str(),
                                       m_config.getSSLConfig().pem_key_path.c_str());

    // Set CA certificate for server verification
    this->cli->set_ca_cert_path(m_config.getSSLConfig().ca_file_path.c_str());

    this->cli->enable_server_certificate_verification(true);
    this->cli->enable_server_hostname_verification(m_config.getSSLConfig().environment == Environment::PRODUCTION);

    // Set connection timeout (optional)
    this->cli->set_connection_timeout(30); // 30 seconds
    this->cli->set_read_timeout(30);       // 30 seconds

    std::cout << "BankID Session: Session initialized successfully" << std::endl;
    return true;
  }

  const std::expected<BankID::AuthResponse, BankID::AuthError> Session::auth()
  {
    if (!m_initialized)
    {
      std::cout << "BankID Session: Session not initialized" << std::endl;
      return std::unexpected(BankID::AuthError{
          500, // Internal error status
          BankID::BankIdErrorCode::NOT_INITIALIZED, "Session not initialized"});
    }

    json payload;
    payload["endUserIp"] = m_config.getEndUserIp();

    // App/Web config
    if (auto app = m_config.getAppConfig())
    {
      payload["app"] = {
          {"appIdentifier", app->appIdentifier},
          {"deviceOS", app->deviceOS},
          {"deviceModelName", app->deviceModelName},
          {"deviceIdentifier", app->deviceIdentifier}};
    }
    else if (auto web = m_config.getWebConfig())
    {
      payload["web"] = {
          {"deviceIdentifier", web->deviceIdentifier},
          {"referringDomain", web->referringDomain},
          {"userAgent", web->userAgent}};
    }

    auto set_optional = [&](const std::string &key, const auto &opt)
    {
      if (opt)
        payload[key] = *opt;
    };

    set_optional("returnRisk", m_config.getReturnRisk());
    set_optional("userVisibleData", m_config.getUserVisibleData());
    set_optional("userVisibleDataFormat", m_config.getUserVisibleDataFormat());
    set_optional("userNonVisibleData", m_config.getUserNonVisibleData());
    set_optional("returnUrl", m_config.getReturnUrl());

    // Requirement
    if (auto req = m_config.getRequirement())
    {
      payload["requirement"] = {
          {"personalNumber", req->personalNumber},
          {"cardReader", req->cardReader},
          {"certificatePolicies", req->certificatePolicies},
          {"mrtd", req->mrtd},
          {"pinCode", req->pinCode}};
    }

    std::cout << "BankID Session: Sending authentication request" << std::endl;
    std::cout << "Payload: " << payload.dump(2) << std::endl;

    auto res = cli->Post("/rp/v6.0/auth",
                         payload.dump(),
                         "application/json");

    return validateStatusAndParse<BankID::AuthResponse>(res);
  }
}
