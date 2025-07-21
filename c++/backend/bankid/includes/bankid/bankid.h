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

namespace BankID {
    /// <summary>
    /// Initialize the BankID authentication system
    /// </summary>
    /// <returns>True if initialization was successful, false otherwise</returns>
    BANKID_API bool Initialize();
    
    /// <summary>
    /// Start an authentication process
    /// </summary>
    /// <param name="personalNumber">Personal number for authentication</param>
    /// <returns>Authentication token or empty string if failed</returns>
    BANKID_API std::string StartAuthentication(const std::string& personalNumber);
    
    /// <summary>
    /// Check the status of an ongoing authentication
    /// </summary>
    /// <param name="token">Authentication token from StartAuthentication</param>
    /// <returns>Authentication status</returns>
    BANKID_API std::string CheckAuthenticationStatus(const std::string& token);
    
    /// <summary>
    /// Cleanup and shutdown the BankID system
    /// </summary>
    BANKID_API void Shutdown();
}
