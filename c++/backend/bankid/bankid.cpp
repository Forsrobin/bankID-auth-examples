#include "bankid/bankid.h"

namespace BankID
{
  // Keep the old BankIDConfig for backward compatibility, but mark as deprecated
  BankIDConfig::BankIDConfig(const std::string &endUserIp, const SSLConfig &sslConfig)
      : m_endUserIp(endUserIp), m_sslConfig(sslConfig)
  {
    std::cout << "BankID Config: Basic configuration created (DEPRECATED - use endpoint-specific configs)" << std::endl;
    std::cout << "  End User IP: " << m_endUserIp << std::endl;
    std::cout << "  SSL Environment: " << (m_sslConfig.environment == Environment::TEST ? "TEST" : "PRODUCTION") << std::endl;
    std::cout << "  CA File: " << m_sslConfig.ca_file_path << std::endl;
  }

  BankIDConfig BankIDConfig::createSimple(const std::string &endUserIp, const SSLConfig &sslConfig)
  {
    std::cout << "BankID Config: Creating simple configuration (DEPRECATED)" << std::endl;
    return BankIDConfig(endUserIp, sslConfig);
  }

  BankIDConfig BankIDConfig::createApp(const std::string &endUserIp, const AppConfig &appConfig, const SSLConfig &sslConfig)
  {
    std::cout << "BankID Config: Creating app-based configuration (DEPRECATED)" << std::endl;
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
    std::cout << "BankID Config: Creating web-based configuration (DEPRECATED)" << std::endl;
    BankIDConfig config(endUserIp, sslConfig);
    config.setWebConfig(webConfig);

    std::cout << "  Web Config:" << std::endl;
    std::cout << "    Device ID: " << webConfig.deviceIdentifier << std::endl;
    std::cout << "    Referring Domain: " << webConfig.referringDomain << std::endl;
    std::cout << "    User Agent: " << webConfig.userAgent << std::endl;

    return config;
  }

  // New Session implementation using SSLConfig only
  Session::Session(const SSLConfig &sslConfig)
      : m_sslConfig(sslConfig), m_initialized(false), m_current_token("")
  {
    std::cout << "BankID Session: Creating session with SSL config" << std::endl;
    m_initialized = initialize();
  }

  Session::~Session()
  {
    delete cli;
    std::cout << "BankID Session: Destroying session" << std::endl;
  }

  const bool Session::initialize()
  {
    std::cout << "BankID Session: Initializing session" << std::endl;
    if (!m_sslConfig.validate())
    {
      std::cerr << "BankID Session: SSL configuration validation failed" << std::endl;
      return false;
    }

    this->cli = new httplib::SSLClient("appapi2.test.bankid.com", 443,
                                       m_sslConfig.pem_cert_path.c_str(),
                                       m_sslConfig.pem_key_path.c_str());

    // Set CA certificate for server verification
    this->cli->set_ca_cert_path(m_sslConfig.ca_file_path.c_str());

    this->cli->enable_server_certificate_verification(true);
    this->cli->enable_server_hostname_verification(m_sslConfig.environment == Environment::PRODUCTION);

    // Set connection timeout (optional)
    this->cli->set_connection_timeout(30); // 30 seconds
    this->cli->set_read_timeout(30);       // 30 seconds

    std::cout << "BankID Session: Session initialized successfully" << std::endl;
    return true;
  }

  // Generic template implementation for making API calls
  template<typename ConfigType>
  const std::expected<BankID::AuthResponse, BankID::AuthError> Session::makeApiCall(
      const std::string& endpoint, 
      const ConfigType& config)
  {
    if (!m_initialized)
    {
      std::cout << "BankID Session: Session not initialized" << std::endl;
      return std::unexpected(BankID::AuthError{
          500, // Internal error status
          BankID::BankIdErrorCode::NOT_INITIALIZED, "Session not initialized"});
    }

    auto payload = config.toJson();
    std::cout << "BankID Session: Sending request to " << endpoint << std::endl;
    std::cout << "Payload: " << payload.dump(2) << std::endl;

    auto res = cli->Post(("/rp/v6.0" + endpoint).c_str(),
                         payload.dump(),
                         "application/json");

    return validateStatusAndParse<BankID::AuthResponse>(res);
  }

  // Endpoint-specific implementations
  const std::expected<BankID::AuthResponse, BankID::AuthError> Session::auth(const BankID::API::AuthConfig& authConfig)
  {
    return makeApiCall("/auth", authConfig);
  }

  const std::expected<BankID::AuthResponse, BankID::AuthError> Session::sign(const BankID::API::SignConfig& signConfig)
  {
    return makeApiCall("/sign", signConfig);
  }

  const std::expected<BankID::AuthResponse, BankID::AuthError> Session::payment(const BankID::API::PaymentConfig& paymentConfig)
  {
    return makeApiCall("/payment", paymentConfig);
  }

  const std::expected<BankID::AuthResponse, BankID::AuthError> Session::phoneAuth(const BankID::API::PhoneAuthConfig& phoneAuthConfig)
  {
    return makeApiCall("/phone/auth", phoneAuthConfig);
  }

  const std::expected<BankID::AuthResponse, BankID::AuthError> Session::phoneSign(const BankID::API::PhoneSignConfig& phoneSignConfig)
  {
    return makeApiCall("/phone/sign", phoneSignConfig);
  }

  const std::expected<BankID::AuthResponse, BankID::AuthError> Session::otherPayment(const BankID::API::OtherPaymentConfig& otherPaymentConfig)
  {
    return makeApiCall("/other/payment", otherPaymentConfig);
  }

  const std::expected<BankID::AuthResponse, BankID::AuthError> Session::collect(const BankID::API::CollectConfig& collectConfig)
  {
    return makeApiCall("/collect", collectConfig);
  }

  const std::expected<BankID::AuthResponse, BankID::AuthError> Session::cancel(const BankID::API::CancelConfig& cancelConfig)
  {
    return makeApiCall("/cancel", cancelConfig);
  }
}
