#pragma once

#include <string>

#ifdef _WIN32
#ifdef BANKID_STATIC
#define BANKID_API
#elif defined(BANKID_EXPORTS)
#define BANKID_API __declspec(dllexport)
#else
#define BANKID_API __declspec(dllimport)
#endif
#else
#define BANKID_API
#endif

namespace BankID
{
  BANKID_API bool Initialize();

  BANKID_API std::string StartAuthentication(const std::string &personalNumber);

  BANKID_API std::string CheckAuthenticationStatus(const std::string &token);

  BANKID_API void Shutdown();
}
