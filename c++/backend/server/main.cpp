#include <iostream>
#include "bankid/bankid.h"

int main() {
    std::cout << "Example project using BankID library" << std::endl;
    
    if (!BankID::Initialize()) {
        std::cerr << "Failed to initialize BankID" << std::endl;
        return 1;
    }
    
    std::string token = BankID::StartAuthentication("199001011234");
    std::cout << "Authentication token: " << token << std::endl;
    
    std::string status = BankID::CheckAuthenticationStatus(token);
    std::cout << "Status: " << status << std::endl;
    
    BankID::Shutdown();
    return 0;
}
