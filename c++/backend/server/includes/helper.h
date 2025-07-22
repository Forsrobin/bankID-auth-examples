#include <iostream>

void log_error_no_ssl_found(const BankID::SSLConfig &ssl_config)
{
  std::cout << "No SSL certificates found, starting HTTP server..." << std::endl;
  std::cout << "To enable HTTPS, place your BankID certificates in the certs/ directory:" << std::endl;
  std::cout << "  - " << ssl_config.p12_file_path << " (P12 certificate)" << std::endl;
  std::cout << "  - " << ssl_config.ca_file_path << " (CA certificate)" << std::endl;
  std::cout << "Then run the certificate conversion script." << std::endl;
  std::cout << std::endl;
}

void log_certificates_found(const BankID::SSLConfig &ssl_config)
{
  std::cout << "PEM combined certificate found, setting up HTTPS server..." << std::endl;
  std::cout << "Using SSL configuration:" << std::endl;
  std::cout << "  Environment: " << (ssl_config.environment == BankID::Environment::TEST ? "TEST" : "PRODUCTION") << std::endl;
  std::cout << "  PEM Combined Certificate: " << ssl_config.pem_combined_path << std::endl;
  std::cout << "  CA Certificate: " << ssl_config.ca_file_path << std::endl;
}

void log_starting_server()
{
  std::cout << "Starting BankID REST API server on port 8443 (HTTPS)..." << std::endl;
  std::cout << "Available endpoints:" << std::endl;
  std::cout << "  GET https://localhost:8443/init - Start authentication" << std::endl;
  std::cout << "  GET https://localhost:8443/poll - Check authentication status" << std::endl;
}