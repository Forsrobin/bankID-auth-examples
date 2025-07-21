#pragma once

#include <string>
#include <optional>

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

namespace BankID
{
  // Configuration structures
  struct BANKID_API AppConfig
  {
    std::string appIdentifier;
    std::string deviceOS;
    std::string deviceModelName;
    std::string deviceIdentifier;
  };

  struct BANKID_API WebConfig
  {
    std::string referringDomain;
    std::string userAgent;
    std::string deviceIdentifier;
  };

  struct BANKID_API Requirement
  {
    std::optional<std::string> personalNumber;
  };

  // SSL/Certificate configuration
  enum class BANKID_API Environment
  {
    TEST,
    PRODUCTION
  };

  struct BANKID_API SSLConfig
  {
    Environment environment = Environment::TEST;
    std::string p12_file_path = "certs/FPTestcert5_20240610.p12";
    std::string p12_passphrase = "qwerty123";
    std::string ca_file_path = "certs/test.ca"; // Will be set based on environment
    std::string pem_cert_path = "certs/bankid_cert.pem"; // PEM certificate for Crow
    std::string pem_key_path = "certs/bankid_key.pem";   // PEM private key for Crow
    std::string pem_combined_path = "certs/bankid_combined.pem"; // Combined PEM file
    
    // Constructor to automatically set CA path based on environment
    SSLConfig(Environment env = Environment::TEST, 
              const std::string& p12_path = "certs/FPTestcert5_20240610.p12",
              const std::string& passphrase = "qwerty123")
      : environment(env), p12_file_path(p12_path), p12_passphrase(passphrase)
    {
      ca_file_path = (env == Environment::TEST) ? "certs/test.ca" : "certs/prod.ca";
      pem_cert_path = "certs/bankid_cert.pem";
      pem_key_path = "certs/bankid_key.pem";
      pem_combined_path = "certs/bankid_combined.pem";
    }
  };

  // Main BankID configuration class
  class BANKID_API BankIDConfig
  {
  private:
    std::string m_endUserIp;
    std::string m_returnUrl;
    std::optional<std::string> m_userVisibleData;
    bool m_returnRisk;
    std::optional<AppConfig> m_appConfig;
    std::optional<WebConfig> m_webConfig;
    std::optional<Requirement> m_requirement;
    SSLConfig m_sslConfig;

  public:
    // Constructor overloads
    // Simple authentication (basic)
    BankIDConfig(const std::string &endUserIp, const std::string &returnUrl, 
                 const SSLConfig& sslConfig = SSLConfig());

    // Authentication with user visible data and risk
    BankIDConfig(const std::string &endUserIp, const std::string &returnUrl,
                 const std::string &userVisibleData, bool returnRisk = true,
                 const SSLConfig& sslConfig = SSLConfig());

    // App-based authentication
    BankIDConfig(const std::string &endUserIp, const std::string &returnUrl,
                 const std::string &userVisibleData, const AppConfig &appConfig,
                 bool returnRisk = true, const SSLConfig& sslConfig = SSLConfig());

    // Web-based authentication
    BankIDConfig(const std::string &endUserIp, const std::string &returnUrl,
                 const std::string &userVisibleData, const WebConfig &webConfig,
                 bool returnRisk = true, const SSLConfig& sslConfig = SSLConfig());

    // Getters
    const std::string &getEndUserIp() const { return m_endUserIp; }
    const std::string &getReturnUrl() const { return m_returnUrl; }
    const std::optional<std::string> &getUserVisibleData() const { return m_userVisibleData; }
    bool getReturnRisk() const { return m_returnRisk; }
    const std::optional<AppConfig> &getAppConfig() const { return m_appConfig; }
    const std::optional<WebConfig> &getWebConfig() const { return m_webConfig; }
    const std::optional<Requirement> &getRequirement() const { return m_requirement; }
    const SSLConfig &getSSLConfig() const { return m_sslConfig; }

    // Setter for requirement (can be called after construction)
    void setRequirement(const Requirement &requirement) { m_requirement = requirement; }
    void setSSLConfig(const SSLConfig &sslConfig) { m_sslConfig = sslConfig; }
  };

  // New functions using BankIDConfig
  BANKID_API bool Initialize(const BankIDConfig &config);
  BANKID_API std::string StartAuthentication(const BankIDConfig &config);
  BANKID_API std::string CheckAuthenticationStatus(const std::string &token, const BankIDConfig &config);
  BANKID_API void Shutdown();
}

#ifdef _WIN32
#pragma warning(pop)
#endif
