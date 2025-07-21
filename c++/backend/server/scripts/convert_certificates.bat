@echo off
REM BankID Certificate Conversion Script
REM This script converts P12 certificates to PEM format for use with Crow HTTP server

echo BankID Certificate Conversion Utility
echo ====================================

REM Check if OpenSSL is available
where openssl >nul 2>nul
if %ERRORLEVEL% NEQ 0 (
    echo Error: OpenSSL is not installed or not in PATH
    echo Please install OpenSSL first. You can download it from:
    echo https://slproweb.com/products/Win32OpenSSL.html
    pause
    exit /b 1
)

REM Set default paths
set P12_FILE=..\certs\FPTestcert5_20240610.p12
set CERT_DIR=..\certs
set PEM_CERT=%CERT_DIR%\bankid_cert.pem
set PEM_KEY=%CERT_DIR%\bankid_key.pem
set PEM_COMBINED=%CERT_DIR%\bankid_combined.pem
set PASSWORD=qwerty123

echo.
echo Converting BankID P12 certificate to PEM format...
echo Source P12 file: %P12_FILE%
echo Target directory: %CERT_DIR%
echo.

REM Check if P12 file exists
if not exist "%P12_FILE%" (
    echo Error: P12 certificate file not found: %P12_FILE%
    echo Please ensure the BankID certificate is placed in the certs directory
    pause
    exit /b 1
)

REM Create certs directory if it doesn't exist
if not exist "%CERT_DIR%" (
    mkdir "%CERT_DIR%"
)

echo Converting certificate...
REM Extract certificate
openssl pkcs12 -in "%P12_FILE%" -clcerts -nokeys -out "%PEM_CERT%" -passin pass:%PASSWORD%
if %ERRORLEVEL% NEQ 0 (
    echo Error: Failed to extract certificate
    pause
    exit /b 1
)

echo Converting private key...
REM Extract private key (without password protection)
openssl pkcs12 -in "%P12_FILE%" -nocerts -nodes -out "%PEM_KEY%" -passin pass:%PASSWORD%
if %ERRORLEVEL% NEQ 0 (
    echo Error: Failed to extract private key
    pause
    exit /b 1
)

echo Creating combined certificate file...
REM Create combined certificate file (certificate + key)
type "%PEM_CERT%" > "%PEM_COMBINED%"
echo. >> "%PEM_COMBINED%"
type "%PEM_KEY%" >> "%PEM_COMBINED%"

echo.
echo Conversion completed successfully!
echo Generated files:
echo   Certificate only: %PEM_CERT%
echo   Private key only: %PEM_KEY%
echo   Combined file:    %PEM_COMBINED%
echo.
echo You can now use these PEM files with your Crow HTTPS server.
echo.

REM Display certificate information
echo Certificate Information:
echo ========================
openssl x509 -in "%PEM_CERT%" -text -noout | findstr "Subject:\|Issuer:\|Not Before:\|Not After:"

echo.
pause
