#include "bankid.h"

namespace BankID
{
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

    this->cli = new httplib::SSLClient(m_sslConfig.environment == Environment::PRODUCTION ? "appapi2.bankid.com" : "appapi2.test.bankid.com",
                                       443,
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
  template <typename ConfigType>
  const std::expected<BankID::AuthResponse, BankID::AuthError> Session::makeApiCall(
      const std::string &endpoint,
      const ConfigType &config)
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
  const std::expected<BankID::AuthResponse, BankID::AuthError> Session::auth(const BankID::API::AuthConfig &authConfig)
  {
    return makeApiCall("/auth", authConfig);
  }

  const std::expected<BankID::AuthResponse, BankID::AuthError> Session::sign(const BankID::API::SignConfig &signConfig)
  {
    return makeApiCall("/sign", signConfig);
  }

  const std::expected<BankID::AuthResponse, BankID::AuthError> Session::payment(const BankID::API::PaymentConfig &paymentConfig)
  {
    return makeApiCall("/payment", paymentConfig);
  }

  const std::expected<BankID::AuthResponse, BankID::AuthError> Session::phoneAuth(const BankID::API::PhoneAuthConfig &phoneAuthConfig)
  {
    return makeApiCall("/phone/auth", phoneAuthConfig);
  }

  const std::expected<BankID::AuthResponse, BankID::AuthError> Session::phoneSign(const BankID::API::PhoneSignConfig &phoneSignConfig)
  {
    return makeApiCall("/phone/sign", phoneSignConfig);
  }

  const std::expected<BankID::AuthResponse, BankID::AuthError> Session::otherPayment(const BankID::API::OtherPaymentConfig &otherPaymentConfig)
  {
    return makeApiCall("/other/payment", otherPaymentConfig);
  }

  const std::expected<BankID::AuthResponse, BankID::AuthError> Session::collect(const BankID::API::CollectConfig &collectConfig)
  {
    return makeApiCall("/collect", collectConfig);
  }

  const std::expected<BankID::AuthResponse, BankID::AuthError> Session::cancel(const BankID::API::CancelConfig &cancelConfig)
  {
    return makeApiCall("/cancel", cancelConfig);
  }
}
