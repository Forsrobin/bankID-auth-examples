#!/bin/bash

# BankID Certificate Conversion Script
# This script converts P12 certificates to PEM format for use with Crow HTTP server

echo "BankID Certificate Conversion Utility"
echo "===================================="

# Check if OpenSSL is available
if ! command -v openssl &> /dev/null; then
    echo "Error: OpenSSL is not installed or not in PATH"
    echo "Please install OpenSSL first:"
    echo "  Ubuntu/Debian: sudo apt-get install openssl"
    echo "  macOS: brew install openssl"
    echo "  CentOS/RHEL: sudo yum install openssl"
    exit 1
fi

# Set default paths
P12_FILE="../certs/FPTestcert5_20240610.p12"
CERT_DIR="../certs"
PEM_CERT="$CERT_DIR/bankid_cert.pem"
PEM_KEY="$CERT_DIR/bankid_key.pem"
PEM_COMBINED="$CERT_DIR/bankid_combined.pem"
PASSWORD="qwerty123"

echo
echo "Converting BankID P12 certificate to PEM format..."
echo "Source P12 file: $P12_FILE"
echo "Target directory: $CERT_DIR"
echo

# Check if P12 file exists
if [ ! -f "$P12_FILE" ]; then
    echo "Error: P12 certificate file not found: $P12_FILE"
    echo "Please ensure the BankID certificate is placed in the certs directory"
    exit 1
fi

# Create certs directory if it doesn't exist
mkdir -p "$CERT_DIR"

echo "Converting certificate..."
# Extract certificate
if ! openssl pkcs12 -in "$P12_FILE" -clcerts -nokeys -out "$PEM_CERT" -passin pass:$PASSWORD; then
    echo "Error: Failed to extract certificate"
    exit 1
fi

echo "Converting private key..."
# Extract private key (without password protection)
if ! openssl pkcs12 -in "$P12_FILE" -nocerts -nodes -out "$PEM_KEY" -passin pass:$PASSWORD; then
    echo "Error: Failed to extract private key"
    exit 1
fi

echo "Creating combined certificate file..."
# Create combined certificate file (certificate + key)
cat "$PEM_CERT" > "$PEM_COMBINED"
echo "" >> "$PEM_COMBINED"
cat "$PEM_KEY" >> "$PEM_COMBINED"

echo
echo "Conversion completed successfully!"
echo "Generated files:"
echo "  Certificate only: $PEM_CERT"
echo "  Private key only: $PEM_KEY"
echo "  Combined file:    $PEM_COMBINED"
echo
echo "You can now use these PEM files with your Crow HTTPS server."
echo

# Display certificate information
echo "Certificate Information:"
echo "========================"
openssl x509 -in "$PEM_CERT" -text -noout | grep -E "Subject:|Issuer:|Not Before:|Not After:"

echo
echo "Press any key to continue..."
read -n 1 -s
