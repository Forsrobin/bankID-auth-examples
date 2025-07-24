#include "bankid.h"
#include "helper.h"
#include <crow.h>
#include <memory>
#include <string>
#include <csignal>
#include <crow/middlewares/cors.h>

#include <nlohmann/json.hpp>
using json = nlohmann::json;

void handleShutdown(int signal)
{
  std::cout << "Received shutdown signal (" << signal << "). Cleaning up..." << std::endl;
  BankID::QRGeneratorCache::instance().shutdown();
  std::_Exit(0);
}

int main()
{
  // Enable CORS for the application
  crow::App<crow::CORSHandler> app;
  auto &cors = app.get_middleware<crow::CORSHandler>();

  // clang-format off
    cors
      .global()
        .headers("X-Custom-Header", "Upgrade-Insecure-Requests")
        .methods("POST"_method, "GET"_method)
      .prefix("/cors")
        .origin("localhost:3000")
      .prefix("/nocors")
        .ignore();


  // SSL Configuration for BankID
  BankID::SSLConfig test_ssl_config(
      BankID::Environment::TEST,
      "certs/bankid_cert.pem",
      "certs/bankid_key.pem");

  if (!test_ssl_config.validate())
  {
    return 1;
  }

  // Create your BankID session instance with just SSL config
  BankID::Session bankid_session(test_ssl_config);

  if (!bankid_session.isInitialized())
  {
    std::cout << "BankID session initialization failed. Exiting." << std::endl;
    return 1;
  }


  // GET /api/auth/init endpoint
  CROW_ROUTE(app, "/api/auth/init")
  ([&bankid_session]()
   {
        std::cout << "GET /init - Starting authentication" << std::endl;
        
        // Create auth config on-demand for this specific request
        auto authConfig = BankID::API::AuthConfig("127.0.0.1")
            .setUserVisibleData("TG9nZ2luIHDDpSBjKysgdGVzdCBwb3J0YWxlbg==");

        auto response = bankid_session.auth(authConfig);

        if (!response)
        {
          const auto &error = response.error();
          return crow::response(error.status, error.details);
        }
        // Simple JSON response using nlohmann::json
        json json_response;
        json_response["orderRef"] = response->orderRef;
        json_response["autoStartToken"] = response->autoStartToken;
        json_response["authCountdown"] = 300;


        crow::response resp(200, json_response.dump());
        resp.add_header("Content-Type", "application/json");
        return resp; });

  // GET /api/auth/poll endpoint
  CROW_ROUTE(app, "/api/auth/poll/<string>")
  ([&bankid_session](std::string orderRef)
   {
     std::cout << "GET /api/auth/poll - Checking authentication status" << std::endl;
     std::cout << "Order Reference: " << orderRef << std::endl;

     auto generator = BankID::QRGeneratorCache::instance().get(orderRef);
     if (generator)
     {
      const auto isExpired = generator->isExpired();
      std::cout << "QR Code generator is expired: " << (isExpired ? "Yes" : "No") << std::endl;
      
      std::string qrCode = generator->getNextQRCode();

       json json_response;
       json_response["status"] = "qrCode";
       json_response["qrCode"] = qrCode;
       json_response["orderRef"] = orderRef;
       json_response["token"] = nullptr;
       json_response["user"] = nullptr;

       crow::response resp(200, json_response.dump());
       resp.add_header("Content-Type", "application/json");
       return resp;
     }
     else
     {
       crow::response resp(404, "QR Code not found");
       resp.add_header("Content-Type", "application/json");
       return resp;
     } });

  CROW_ROUTE(app, "/api/auth/cancel/<string>")
  ([&bankid_session](std::string orderRef)
   {
        auto cancelConfig = BankID::API::CancelConfig::create(
            orderRef          // orderRef
        );

        auto response = bankid_session.cancel(cancelConfig);

        if (!response)
        {
          const auto &error = response.error();
          return crow::response(error.status, error.details);
        }


        // Convert the response to JSON
        json json_response;
        json_response["message"] = "Order cancelled successfully";

        crow::response resp(200, json_response.dump());
        resp.add_header("Content-Type", "application/json");
        return resp; });

  log_starting_server();

  // Register cleanup handler to ensure graceful shutdown
  std::atexit([]
              {
  std::cout << "Application is shutting down. Cleaning up QRGeneratorCache..." << std::endl;
  BankID::QRGeneratorCache::instance().shutdown(); });

  std::signal(SIGINT, handleShutdown);  // Ctrl+C
  std::signal(SIGTERM, handleShutdown); // kill signal

  app.port(8080).multithreaded().run();

  return 0;
}
