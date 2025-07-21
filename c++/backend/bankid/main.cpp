#include <iostream>
#include "bankid/bankid.h"

int main()
{
    std::cout << "BankID Example Application" << std::endl;
    
    // Initialize the BankID library
    if (!BankID::Initialize()) {
        std::cerr << "Failed to initialize BankID library" << std::endl;
        return 1;
    }
    
    // Start authentication
    std::string token = BankID::StartAuthentication("199001011234");
    if (token.empty()) {
        std::cerr << "Failed to start authentication" << std::endl;
        BankID::Shutdown();
        return 1;
    }
    
    // Check authentication status
    std::string status = BankID::CheckAuthenticationStatus(token);
    std::cout << "Authentication status: " << status << std::endl;
    
    // Cleanup
    BankID::Shutdown();
    
    return 0;
}
