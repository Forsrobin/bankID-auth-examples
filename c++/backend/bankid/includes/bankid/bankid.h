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

  public:
    // Constructor overloads
    // Simple authentication (basic)
    BankIDConfig(const std::string &endUserIp, const std::string &returnUrl);

    // Authentication with user visible data and risk
    BankIDConfig(const std::string &endUserIp, const std::string &returnUrl,
                 const std::string &userVisibleData, bool returnRisk = true);

    // App-based authentication
    BankIDConfig(const std::string &endUserIp, const std::string &returnUrl,
                 const std::string &userVisibleData, const AppConfig &appConfig,
                 bool returnRisk = true);

    // Web-based authentication
    BankIDConfig(const std::string &endUserIp, const std::string &returnUrl,
                 const std::string &userVisibleData, const WebConfig &webConfig,
                 bool returnRisk = true);

    // Getters
    const std::string &getEndUserIp() const { return m_endUserIp; }
    const std::string &getReturnUrl() const { return m_returnUrl; }
    const std::optional<std::string> &getUserVisibleData() const { return m_userVisibleData; }
    bool getReturnRisk() const { return m_returnRisk; }
    const std::optional<AppConfig> &getAppConfig() const { return m_appConfig; }
    const std::optional<WebConfig> &getWebConfig() const { return m_webConfig; }
    const std::optional<Requirement> &getRequirement() const { return m_requirement; }

    // Setter for requirement (can be called after construction)
    void setRequirement(const Requirement &requirement) { m_requirement = requirement; }
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
