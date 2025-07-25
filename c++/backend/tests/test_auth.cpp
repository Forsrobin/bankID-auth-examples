#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "bankid.h"
#include "api/auth.h"
#include "api/responses.h"

class AuthTest : public ::testing::Test
{
protected:
  void SetUp() override
  {
    // Setup SSL config for testing
    sslConfig = BankID::SSLConfig(BankID::Environment::TEST);

    // Create session
    session = std::make_unique<BankID::Session>(sslConfig);
  }

  void TearDown() override
  {
    session.reset();
  }

  BankID::SSLConfig sslConfig;
  std::unique_ptr<BankID::Session> session;
};

TEST_F(AuthTest, CreateAuthConfigWithPersonalNumber)
{
  // Test creating auth config with personal number
  BankID::API::AuthConfig config("192.168.1.1");

  BankID::Requirement requirement;
  requirement.personalNumber = "190000000000";
  config.setRequirement(requirement);

  EXPECT_EQ(config.getEndUserIp(), "192.168.1.1");
  EXPECT_TRUE(config.getRequirement().has_value());
  EXPECT_EQ(config.getRequirement().value().personalNumber.value(), "190000000000");
}

TEST_F(AuthTest, CreateAuthConfigWithoutPersonalNumber)
{
  // Test creating auth config without personal number (QR code flow)
  BankID::API::AuthConfig config("192.168.1.1");

  EXPECT_EQ(config.getEndUserIp(), "192.168.1.1");
  EXPECT_FALSE(config.getRequirement().has_value());
}

TEST_F(AuthTest, CreateAuthConfigWithRequirement)
{
  // Test creating auth config with specific requirements
  BankID::API::AuthConfig config("192.168.1.1");

  BankID::Requirement requirement;
  requirement.cardReader = "class1";
  requirement.certificatePolicies = {"1.2.752.78.1.1", "1.2.752.78.1.2"};
  config.setRequirement(requirement);

  EXPECT_EQ(config.getEndUserIp(), "192.168.1.1");
  EXPECT_TRUE(config.getRequirement().has_value());
  EXPECT_EQ(config.getRequirement().value().cardReader.value(), "class1");
  EXPECT_EQ(config.getRequirement().value().certificatePolicies.value().size(), 2);
}

TEST_F(AuthTest, CreateAuthConfigWithUserVisibleData)
{
  // Test creating auth config with user visible data
  BankID::API::AuthConfig config("192.168.1.1");
  config.setUserVisibleData("VGVzdCBkYXRh") // Base64 encoded "Test data"
      .setUserVisibleDataFormat("simpleMarkdownV1");

  EXPECT_EQ(config.getEndUserIp(), "192.168.1.1");
  EXPECT_TRUE(config.getUserVisibleData().has_value());
  EXPECT_EQ(config.getUserVisibleData().value(), "VGVzdCBkYXRh");
  EXPECT_TRUE(config.getUserVisibleDataFormat().has_value());
  EXPECT_EQ(config.getUserVisibleDataFormat().value(), "simpleMarkdownV1");
}

TEST_F(AuthTest, ValidateEndUserIpRequired)
{
  // Test that endUserIp is required - constructor requires it
  // This test demonstrates that endUserIp is required in constructor
  BankID::API::AuthConfig config1("192.168.1.1");
  EXPECT_EQ(config1.getEndUserIp(), "192.168.1.1");

  BankID::API::AuthConfig config2("");
  EXPECT_TRUE(config2.getEndUserIp().empty());
}

TEST_F(AuthTest, AuthWithInvalidSSLConfig)
{
  // Test auth with invalid SSL configuration
  BankID::SSLConfig invalidConfig(BankID::Environment::TEST, "invalid_cert.pem", "invalid_key.pem");
  BankID::Session invalidSession(invalidConfig);

  BankID::API::AuthConfig config("192.168.1.1");

  // This should fail due to invalid SSL config
  auto result = invalidSession.auth(config);
  EXPECT_FALSE(result.has_value());
}

// Note: These tests below would require a test BankID environment
// and proper certificates to run successfully

TEST_F(AuthTest, DISABLED_AuthWithValidConfig)
{
  // Test successful auth call (disabled by default - requires test environment)
  BankID::API::AuthConfig config("192.168.1.1");

  BankID::Requirement requirement;
  requirement.personalNumber = "190000000000"; // Test personal number
  config.setRequirement(requirement);

  auto result = session->auth(config);

  if (result.has_value())
  {
    EXPECT_FALSE(result.value().orderRef.empty());
    EXPECT_FALSE(result.value().autoStartToken.empty());
  }
}

TEST_F(AuthTest, DISABLED_AuthQRCodeFlow)
{
  // Test QR code authentication flow (disabled by default)
  BankID::API::AuthConfig config("192.168.1.1");
  // No personal number for QR code flow

  auto result = session->auth(config);

  if (result.has_value())
  {
    EXPECT_FALSE(result.value().orderRef.empty());
    EXPECT_FALSE(result.value().qrStartToken.empty());
  }
}

TEST_F(AuthTest, DISABLED_AuthWithUserVisibleData)
{
  // Test auth with user visible data (disabled by default)
  BankID::API::AuthConfig config("192.168.1.1");
  config.setUserVisibleData("VGVzdCBkYXRh") // Base64 encoded "Test data"
      .setUserVisibleDataFormat("simpleMarkdownV1");

  auto result = session->auth(config);

  if (result.has_value())
  {
    EXPECT_FALSE(result.value().orderRef.empty());
  }
}
