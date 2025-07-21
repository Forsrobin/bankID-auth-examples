# BankID SSL Certificate Setup

This document explains how to set up SSL certificates for your BankID authentication server.

## Overview

The BankID server supports both HTTP and HTTPS. For production use, HTTPS is strongly recommended. The server can automatically detect and use SSL certificates when properly configured.

## Certificate Files

The server supports the following certificate configurations:

### BankID Certificates
- **P12 Certificate**: `certs/FPTestcert5_20240610.p12` (from BankID)
- **P12 Passphrase**: `qwerty123` (default for test environment)
- **CA Certificate**: `certs/test.ca` (for test) or `certs/prod.ca` (for production)

### Converted PEM Certificates (for Crow framework)
- **Combined PEM**: `certs/bankid_combined.pem` (certificate + private key)
- **Certificate PEM**: `certs/bankid_cert.pem` (certificate only)
- **Private Key PEM**: `certs/bankid_key.pem` (private key only)

## Setup Instructions

### 1. Download BankID Certificates

Download the test certificate from BankID:
- Place `FPTestcert5_20240610.p12` in the `certs/` directory
- Place `test.ca` in the `certs/` directory

### 2. Convert P12 to PEM Format

The Crow HTTP framework requires PEM format certificates. Use one of the conversion methods below:

#### Option A: Use the Conversion Scripts

**Windows:**
```cmd
cd scripts
convert_certificates.bat
```

**Linux/macOS:**
```bash
cd scripts
./convert_certificates.sh
```

#### Option B: Manual Conversion with OpenSSL

```bash
# Convert P12 to combined PEM file (recommended)
openssl pkcs12 -in certs/FPTestcert5_20240610.p12 -out certs/bankid_combined.pem -nodes
# Enter password: qwerty123

# Or convert to separate certificate and key files
openssl pkcs12 -in certs/FPTestcert5_20240610.p12 -clcerts -nokeys -out certs/bankid_cert.pem
openssl pkcs12 -in certs/FPTestcert5_20240610.p12 -nocerts -nodes -out certs/bankid_key.pem
```

### 3. Directory Structure

After setup, your `certs/` directory should look like:

```
certs/
├── FPTestcert5_20240610.p12    # Original P12 certificate
├── test.ca                      # Test CA certificate
├── bankid_combined.pem          # Converted combined certificate (preferred)
├── bankid_cert.pem             # Converted certificate only
└── bankid_key.pem              # Converted private key only
```

## Server Behavior

The server will automatically detect and use certificates in this priority order:

1. **Combined PEM file** (`bankid_combined.pem`) - HTTPS on port 8443
2. **Separate PEM files** (`bankid_cert.pem` + `bankid_key.pem`) - HTTPS on port 8443
3. **P12 file only** - HTTP on port 8080 with conversion instructions
4. **No certificates** - HTTP on port 8080 with setup instructions

## Configuration in Code

### Basic Configuration
```cpp
// Test environment (default)
BankID::SSLConfig test_ssl_config(
    BankID::Environment::TEST,
    "certs/FPTestcert5_20240610.p12",
    "qwerty123"
);

BankID::BankIDConfig config(
    "172.0.0.1",
    "https://yourapp.example.com/auth/callback",
    test_ssl_config
);
```

### Production Configuration
```cpp
// Production environment
BankID::SSLConfig prod_ssl_config(
    BankID::Environment::PRODUCTION,
    "certs/your_production_cert.p12",
    "your_production_passphrase"
);

BankID::BankIDConfig config(
    "172.0.0.1",
    "https://yourapp.example.com/auth/callback",
    prod_ssl_config
);
```

## Troubleshooting

### Common Issues

1. **"SSL certificates not found"**
   - Ensure certificate files are in the correct `certs/` directory
   - Check file names match the configuration

2. **"SSL setup failed"**
   - Verify PEM files are properly formatted
   - Check that certificate and key match
   - Ensure port 8443 is available

3. **"Certificate verify failed"**
   - Ensure CA certificate is present and valid
   - For self-signed certificates, add them to your system's trust store

### Testing SSL Setup

Test your HTTPS endpoints:

```bash
# Test with curl (ignoring certificate verification for self-signed)
curl -k https://localhost:8443/init

# Test certificate details
openssl x509 -in certs/bankid_cert.pem -text -noout
```

## Security Notes

- **Never commit certificates to version control**
- Use environment-specific certificates (test vs production)
- Regularly update certificates before expiration
- Keep private keys secure and never share them
- For production, consider using certificates from a trusted CA

## Environment Variables

You can override default paths using environment variables:

```bash
export BANKID_P12_PATH="path/to/your/cert.p12"
export BANKID_P12_PASSPHRASE="your_passphrase"
export BANKID_CA_PATH="path/to/your/ca.pem"
```

## Production Deployment

For production deployment:

1. Obtain production certificates from your bank
2. Update SSL configuration to use production environment
3. Ensure certificates are properly secured on the server
4. Configure firewall to allow HTTPS traffic on port 8443
5. Consider using a reverse proxy (nginx, Apache) for additional security
