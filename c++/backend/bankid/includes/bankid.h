#pragma once

#define CPPHTTPLIB_OPENSSL_SUPPORT
#include "httplib.h"
#include <string>
#include <optional>
#include <filesystem>
#include <vector>
#include <expected>
#include <iostream>
#include <chrono>
#include <thread>
#include <unordered_map>

#include <nlohmann/json.hpp>
using json = nlohmann::json;

#ifdef _WIN32
#ifdef BANKID_STATIC
#define BANKID_API
#elif defined(BANKID_EXPORTS)
#define BANKID_API __declspec(dllexport)
#else
#define BANKID_API __declspec(dllimport)
#endif
// Suppress C4251 warnings for STL types in DLL interface
#pragma warning(push)
#pragma warning(disable : 4251)
#else
#define BANKID_API
#endif

bool file_exists(const std::string &path)
{
  return std::filesystem::exists(path);
}

namespace BankID
{
  struct BANKID_API AppConfig
  {
    std::string appIdentifier;
    std::string deviceOS;
    std::string deviceIdentifier;
    std::string deviceModelName;
  };

  struct BANKID_API WebConfig
  {
    std::string deviceIdentifier;
    std::string referringDomain;
    std::string userAgent;
  };

  struct BANKID_API Requirement
  {
    std::optional<std::string> cardReader;
    std::optional<std::vector<std::string>> certificatePolicies;
    std::optional<bool> mrtd;
    std::optional<std::string> personalNumber;
    std::optional<bool> pinCode;
  };

  struct DefaultResponse
  {
    int status;
  };

  inline void from_json(const nlohmann::json &j, DefaultResponse &r)
  {
    // You can optionally include shared response fields if they exist in JSON
    if (j.contains("status"))
      j.at("status").get_to(r.status);
  }

  /**
   * Authentication response structure
   * This structure contains the response from the BankID authentication API.
   * It includes the order reference, auto-start token, QR start token, and QR start
   * secret. Look at, https://developers.bankid.com/api-references/auth--sign/auth
   * for more details.
   */
  struct BANKID_API AuthResponse : public DefaultResponse
  {
    std::string orderRef;
    std::string autoStartToken;
    std::string qrStartToken;
    std::string qrStartSecret;
  };

  inline void from_json(const nlohmann::json &j, AuthResponse &r)
  {
    // Populate base struct fields (optional if `status` is added manually)
    from_json(j, static_cast<DefaultResponse &>(r));

    j.at("orderRef").get_to(r.orderRef);
    j.at("autoStartToken").get_to(r.autoStartToken);
    j.at("qrStartToken").get_to(r.qrStartToken);
    j.at("qrStartSecret").get_to(r.qrStartSecret);
  }
  enum class BANKID_API Environment
  {
    TEST,
    PRODUCTION
  };

  enum class BANKID_API BankIdErrorCode
  {
    NOT_INITIALIZED,
    ALREADY_IN_PROGRESS,
    INVALID_PARAMETERS,
    UNAUTHORIZED,
    NOT_FOUND,
    METHOD_NOT_ALLOWED,
    REQUEST_TIMEOUT,
    UNSUPPORTED_MEDIA_TYPE,
    INTERNAL_ERROR,
    MAINTENANCE,
  };

  struct BANKID_API AuthError : public DefaultResponse
  {
    BankID::BankIdErrorCode errorCode;
    std::string details;
  };

  /** SSL configuration structure
   * This structure contains the SSL configuration for the BankID API.
   * It includes the environment, CA file path, PEM certificate path, and PEM key path.
   * It is used to configure the SSL client for secure communication with the BankID API.
   * To generate the PEM certificate and key, you can use OpenSSL to convert a PKCS#12 file (.p12) to PEM format.
   *
   * For example:
   *    openssl pkcs12 -in bankid_cert.p12 -out bankid_key.pem -nocerts -nodes
   *    openssl pkcs12 -in bankid_cert.p12 -out bankid_cert.pem -clcerts -nokeys
   */
  struct BANKID_API SSLConfig
  {
    Environment environment = Environment::TEST;
    std::string ca_file_path = "certs/test.ca";          // Will be set based on environment
    std::string pem_cert_path = "certs/bankid_cert.pem"; // PEM certificate for Crow
    std::string pem_key_path = "certs/bankid_key.pem";   // PEM private key for Crow

    /**
     * Constructor for SSLConfig
     * @param env Environment type (TEST or PRODUCTION)
     * @param pem_cert_path Path to the PEM certificate file
     * @param pem_key_path Path to the PEM private key file
     * @brief Initializes the SSL configuration with the specified environment and certificate paths.
     * If the environment is TEST, it sets the CA file path to "certs/test.ca".
     * If the environment is PRODUCTION, it sets the CA file path to "certs/production.ca".
     *
     * The .ca file is used for server verification and are provided by BankID so no need to generate it.
     */
    SSLConfig(Environment env = Environment::TEST,
              const std::string &pem_cert_path = "certs/bankid_cert.pem",
              const std::string &pem_key_path = "certs/bankid_key.pem")
        : environment(env), pem_cert_path(pem_cert_path), pem_key_path(pem_key_path)
    {
      std::cout << "BankID SSLConfig: Creating SSL configuration" << std::endl;
      if (environment == Environment::TEST)
      {
        ca_file_path = "certs/test.ca";
      }
      else
      {
        ca_file_path = "certs/production.ca";
      }
    }

    /**
     * Validate the SSL configuration
     * @return true if the configuration is valid, false otherwise
     * @brief Checks if the certificate and key files exist and are accessible.
     * If any of the files do not exist, it returns false and prints an error message.
     */
    bool validate() const
    {
      // Verify certificate files exist
      if (!file_exists(this->pem_cert_path))
      {
        std::cerr << "Certificate file does not exist: " << this->pem_cert_path << std::endl;
        return false;
      }

      if (!file_exists(this->pem_key_path))
      {
        std::cerr << "Key file does not exist: " << this->pem_key_path << std::endl;
        return false;
      }
      if (!file_exists(this->ca_file_path))
      {
        std::cerr << "CA file does not exist: " << this->ca_file_path << std::endl;
        return false;
      }

      return true;
    }
  };

}

// Include API endpoint configurations
#include "api/auth.h"
#include "api/sign.h"
#include "api/payment.h"
#include "api/phone.auth.h"
#include "api/phone.sign.h"
#include "api/other.payment.h"
#include "api/collect.h"
#include "api/cancel.h"

namespace BankID
{

  // BankID Session class for managing authentication sessions
  class BANKID_API Session
  {
  private:
    SSLConfig m_sslConfig;
    bool m_initialized;
    std::string m_current_token;
    httplib::SSLClient *cli;

  public:
    Session(const SSLConfig &sslConfig);
    ~Session();

    /**
     * Start authentication process using AuthConfig
     * @param authConfig Configuration for the /auth endpoint
     * @return AuthResponse or AuthError
     */
    const std::expected<BankID::AuthResponse, BankID::AuthError> auth(const BankID::API::AuthConfig &authConfig);

    /**
     * Start signing process using SignConfig
     * @param signConfig Configuration for the /sign endpoint
     * @return AuthResponse or AuthError
     */
    const std::expected<BankID::AuthResponse, BankID::AuthError> sign(const BankID::API::SignConfig &signConfig);

    /**
     * Start payment process using PaymentConfig
     * @param paymentConfig Configuration for the /payment endpoint
     * @return AuthResponse or AuthError
     */
    const std::expected<BankID::AuthResponse, BankID::AuthError> payment(const BankID::API::PaymentConfig &paymentConfig);

    /**
     * Start phone authentication using PhoneAuthConfig
     * @param phoneAuthConfig Configuration for the /phone/auth endpoint
     * @return Limited response (orderRef only) or AuthError
     */
    const std::expected<BankID::AuthResponse, BankID::AuthError> phoneAuth(const BankID::API::PhoneAuthConfig &phoneAuthConfig);

    /**
     * Start phone signing using PhoneSignConfig
     * @param phoneSignConfig Configuration for the /phone/sign endpoint
     * @return Limited response (orderRef only) or AuthError
     */
    const std::expected<BankID::AuthResponse, BankID::AuthError> phoneSign(const BankID::API::PhoneSignConfig &phoneSignConfig);

    /**
     * Start other payment using OtherPaymentConfig
     * @param otherPaymentConfig Configuration for the /other/payment endpoint
     * @return Limited response (orderRef only) or AuthError
     */
    const std::expected<BankID::AuthResponse, BankID::AuthError> otherPayment(const BankID::API::OtherPaymentConfig &otherPaymentConfig);

    /**
     * Collect order status and result
     * @param collectConfig Configuration for the /collect endpoint
     * @return Collect response or AuthError
     */
    const std::expected<BankID::AuthResponse, BankID::AuthError> collect(const BankID::API::CollectConfig &collectConfig);

    /**
     * Cancel an ongoing order
     * @param cancelConfig Configuration for the /cancel endpoint
     * @return Empty response or AuthError
     */
    const std::expected<BankID::AuthResponse, BankID::AuthError> cancel(const BankID::API::CancelConfig &cancelConfig);

    // Get current token
    const std::string &getCurrentToken() const { return m_current_token; }
    const bool initialize();
    bool isInitialized() const { return m_initialized; }
    const SSLConfig &getSSLConfig() const { return m_sslConfig; }

    template <typename T>
    std::expected<T, BankID::AuthError> validateStatusAndParse(
        const httplib::Result &res,
        const std::unordered_map<int, std::string> &customErrors = {})
    {
      if (!res)
      {
        return std::unexpected(AuthError{
            0,
            BankIdErrorCode::INTERNAL_ERROR,
            "No response from server"});
      }

      // Success case
      if (res->status == 200)
      {
        try
        {
          nlohmann::json j = nlohmann::json::parse(res->body);
          T parsed = j.get<T>();
          if constexpr (std::is_base_of_v<DefaultResponse, T>)
          {
            parsed.status = res->status;
          }
          return parsed;
        }
        catch (const std::exception &e)
        {
          return std::unexpected(AuthError{
              res->status,
              BankIdErrorCode::INVALID_PARAMETERS,
              std::string("Failed to parse response: ") + e.what()});
        }
      }

      // Custom error overrides
      auto it = customErrors.find(res->status);
      if (it != customErrors.end())
      {
        return std::unexpected(AuthError{
            res->status,
            BankIdErrorCode::INVALID_PARAMETERS, // Or let caller map their own code
            it->second});
      }

      // Special case: status 400 with body (errorCode + details)
      if (res->status == 400)
      {
        try
        {
          auto jsonBody = nlohmann::json::parse(res->body);
          std::string details = jsonBody.at("details").get<std::string>();
          std::string errorCode = jsonBody.at("errorCode").get<std::string>();
          return std::unexpected(AuthError{
              400,
              BankIdErrorCode::INVALID_PARAMETERS,
              errorCode + ": " + details});
        }
        catch (const std::exception &e)
        {
          return std::unexpected(AuthError{
              400,
              BankIdErrorCode::INVALID_PARAMETERS,
              std::string("Failed to parse error response: ") + e.what()});
        }
      }

      // Status code → BankIdErrorCode and message mapping
      const static std::unordered_map<int, std::pair<BankIdErrorCode, std::string>> defaultErrors = {
          {401, {BankIdErrorCode::UNAUTHORIZED, "You do not have access to the service."}},
          {403, {BankIdErrorCode::UNAUTHORIZED, "You do not have access to the service."}},
          {404, {BankIdErrorCode::NOT_FOUND, "An invalid URL path was used."}},
          {405, {BankIdErrorCode::METHOD_NOT_ALLOWED, "Only HTTP method POST is allowed."}},
          {408, {BankIdErrorCode::REQUEST_TIMEOUT, "Timeout while transmitting the request."}},
          {415, {BankIdErrorCode::UNSUPPORTED_MEDIA_TYPE, "The type is missing or invalid."}},
          {500, {BankIdErrorCode::INTERNAL_ERROR, "Internal technical error in the BankID system."}},
          {503, {BankIdErrorCode::MAINTENANCE, "The service is temporarily unavailable."}},
      };

      auto defIt = defaultErrors.find(res->status);
      if (defIt != defaultErrors.end())
      {
        return std::unexpected(AuthError{
            res->status,
            defIt->second.first,
            defIt->second.second});
      }

      // Fallback for unknown codes
      return std::unexpected(AuthError{
          res->status,
          BankIdErrorCode::INTERNAL_ERROR,
          "Unhandled HTTP error"});
    }

  private:
    /**
     * Generic method to make API calls
     * @param endpoint The API endpoint (e.g., "/auth", "/sign", etc.)
     * @param config Any config object that has a toJson() method
     * @return Expected response or error
     */
    template <typename ConfigType>
    const std::expected<BankID::AuthResponse, BankID::AuthError> makeApiCall(
        const std::string &endpoint,
        const ConfigType &config);
  };

}

#ifdef _WIN32
#pragma warning(pop)
#endif
