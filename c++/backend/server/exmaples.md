// Example 2: Enhanced authentication with user visible data and SSL
// BankID::BankIDConfig enhanced_config(
// "192.168.1.100", // endUserIp
// "https://yourapp.example.com/auth/callback", // returnUrl
// "VGhpcyBpcyBhIHNhbXBsZSB0ZXh0IHRvIGJlIHNpZ25lZA==", // base64 encoded user visible data
// true, // returnRisk
// test_ssl_config // SSL configuration
// );

// Example 3: App-based authentication with SSL
// BankID::AppConfig app_config{
// "com.yourcompany.yourapp", // appIdentifier
// "iOS 17.1", // deviceOS
// "iPhone15,2", // deviceModelName
// "a1b2c3d4e5f6789012345678901234567890abcdef1234567890abcdef123456" // deviceIdentifier
// };
// BankID::BankIDConfig app_config_bankid(
// "192.168.1.100",
// "https://yourapp.example.com/auth/callback",
// "VGhpcyBpcyBhIHNhbXBsZSB0ZXh0IHRvIGJlIHNpZ25lZA==",
// app_config,
// true,
// test_ssl_config
// );

// Example 4: Web-based authentication with SSL
// BankID::WebConfig web_config{
// "yourapp.example.com", // referringDomain
// "Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36", // userAgent
// "w1x2y3z4a5b6c7d8e9f0123456789012345678901234567890abcdef123456" // deviceIdentifier
// };
// BankID::BankIDConfig web_config_bankid(
// "192.168.1.100",
// "https://yourapp.example.com/auth/callback",
// "VGhpcyBpcyBhIHNhbXBsZSB0ZXh0IHRvIGJlIHNpZ25lZA==",
// web_config,
// true,
// test_ssl_config
// );
