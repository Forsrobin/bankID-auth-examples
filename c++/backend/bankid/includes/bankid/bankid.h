#pragma once

#define CPPHTTPLIB_OPENSSL_SUPPORT
#include "bankid/httplib.h"

#include <string>
#include <optional>
#include <filesystem>
#include <vector>

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
  // Configuration structures matching BankID API
  struct BANKID_API AppConfig
  {
    std::string appIdentifier; // Android package or iOS bundle ID
    std::string deviceOS;
    std::string deviceIdentifier; // Unique, persistent device ID
    std::string deviceModelName;
  };

  struct BANKID_API WebConfig
  {
    std::string deviceIdentifier; // Unique browser/device identifier
    std::string referringDomain;  // Punycode if IDN
    std::string userAgent;
  };

  struct BANKID_API Requirement
  {
    std::optional<std::string> cardReader;                       // "class1" or "class2"
    std::optional<std::vector<std::string>> certificatePolicies; // e.g. "1.2.752.78.1.1"
    std::optional<bool> mrtd;
    std::optional<std::string> personalNumber;
    std::optional<bool> pinCode;
  };

  // SSL/Certificate configuration
  enum class BANKID_API Environment
  {
    TEST,
    PRODUCTION
  };

  enum class BankIdErrorCode
  {
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

  struct BANKID_API SSLConfig
  {
    Environment environment = Environment::TEST;
    std::string ca_file_path = "certs/test.ca";          // Will be set based on environment
    std::string pem_cert_path = "certs/bankid_cert.pem"; // PEM certificate for Crow
    std::string pem_key_path = "certs/bankid_key.pem";   // PEM private key for Crow

    SSLConfig(Environment env = Environment::TEST,
              const std::string &pem_cert_path = "certs/bankid_cert.pem",
              const std::string &pem_key_path = "certs/bankid_key.pem")
        : environment(env), pem_cert_path(pem_cert_path), pem_key_path(pem_key_path)
    {
      // Set the CA file path based on the environment
      if (environment == Environment::TEST)
      {
        ca_file_path = "certs/test.ca";
      }
      else
      {
        ca_file_path = "certs/production.ca";
      }
    }

    bool validate() const
    {
      // Verify certificate files exist
      if (!file_exists(this->pem_cert_path))
      {
        return false;
      }

      if (!file_exists(this->ca_file_path))
      {
        return false;
      }

      return true;
    }
  };

  // Main BankID configuration class
  class BANKID_API BankIDConfig
  {
  private:
    // Required field
    std::string m_endUserIp;

    // Optional fields
    std::optional<bool> m_returnRisk;
    std::optional<std::string> m_returnUrl;
    std::optional<std::string> m_userNonVisibleData;    // base64-encoded
    std::optional<std::string> m_userVisibleData;       // base64-encoded UTF-8
    std::optional<std::string> m_userVisibleDataFormat; // "plaintext" or "simpleMarkdownV1"
    std::optional<AppConfig> m_appConfig;
    std::optional<WebConfig> m_webConfig;
    std::optional<Requirement> m_requirement;
    SSLConfig m_sslConfig;

  public:
    // Constructor - only endUserIp is required, everything else is optional
    BankIDConfig(const std::string &endUserIp, const SSLConfig &sslConfig = SSLConfig());

    // Static factory methods for different configuration types
    // Simple configuration (just endUserIp and basic options)
    static BankIDConfig createSimple(const std::string &endUserIp,
                                     const SSLConfig &sslConfig = SSLConfig());

    // App-based configuration
    static BankIDConfig createApp(const std::string &endUserIp,
                                  const AppConfig &appConfig,
                                  const SSLConfig &sslConfig = SSLConfig());

    // Web-based configuration
    static BankIDConfig createWeb(const std::string &endUserIp,
                                  const WebConfig &webConfig,
                                  const SSLConfig &sslConfig = SSLConfig());

    // Getters for required field
    const std::string &getEndUserIp() const { return m_endUserIp; }

    // Getters for optional fields
    const std::optional<bool> &getReturnRisk() const { return m_returnRisk; }
    const std::optional<std::string> &getReturnUrl() const { return m_returnUrl; }
    const std::optional<std::string> &getUserNonVisibleData() const { return m_userNonVisibleData; }
    const std::optional<std::string> &getUserVisibleData() const { return m_userVisibleData; }
    const std::optional<std::string> &getUserVisibleDataFormat() const { return m_userVisibleDataFormat; }
    const std::optional<AppConfig> &getAppConfig() const { return m_appConfig; }
    const std::optional<WebConfig> &getWebConfig() const { return m_webConfig; }
    const std::optional<Requirement> &getRequirement() const { return m_requirement; }
    const SSLConfig &getSSLConfig() const { return m_sslConfig; }

    // Setters for optional fields (fluent interface)
    BankIDConfig &setReturnRisk(bool returnRisk)
    {
      m_returnRisk = returnRisk;
      return *this;
    }
    BankIDConfig &setReturnUrl(const std::string &returnUrl)
    {
      m_returnUrl = returnUrl;
      return *this;
    }
    BankIDConfig &setUserNonVisibleData(const std::string &data)
    {
      m_userNonVisibleData = data;
      return *this;
    }
    BankIDConfig &setUserVisibleData(const std::string &data)
    {
      m_userVisibleData = data;
      return *this;
    }
    BankIDConfig &setUserVisibleDataFormat(const std::string &format)
    {
      m_userVisibleDataFormat = format;
      return *this;
    }
    BankIDConfig &setAppConfig(const AppConfig &appConfig)
    {
      m_appConfig = appConfig;
      return *this;
    }
    BankIDConfig &setWebConfig(const WebConfig &webConfig)
    {
      m_webConfig = webConfig;
      return *this;
    }
    BankIDConfig &setRequirement(const Requirement &requirement)
    {
      m_requirement = requirement;
      return *this;
    }
    BankIDConfig &setSSLConfig(const SSLConfig &sslConfig)
    {
      m_sslConfig = sslConfig;
      return *this;
    }
  };

  // BankID Session class for managing authentication sessions
  class BANKID_API Session
  {
  private:
    BankIDConfig m_config;
    bool m_initialized;
    std::string m_current_token;

  public:
    // Constructor
    Session(const BankIDConfig &config);

    // Destructor
    ~Session();

    // Initialize the session
    bool initialize();

    // Start authentication and return token
    std::string startAuthentication();

    // Check authentication status using stored token
    std::string checkStatus();
    std::string checkStatus(const std::string &token);

    // Convenience method for authentication
    std::string auth() { return startAuthentication(); }

    // Get current token
    const std::string &getCurrentToken() const { return m_current_token; }
    // Check if session is initialized
    bool isInitialized() const { return m_initialized; }
    // Get configuration
    const BankIDConfig &getConfig() const { return m_config; }
  };
}

#ifdef _WIN32
#pragma warning(pop)
#endif
