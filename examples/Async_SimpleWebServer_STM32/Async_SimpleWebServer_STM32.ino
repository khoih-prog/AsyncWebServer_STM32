/****************************************************************************************************************************
  Async_HelloServerr.h - Dead simple AsyncWebServer for STM32 built-in LAN8742A Ethernet
  
  For STM32 with built-in LAN8742A Ethernet (Nucleo-144, DISCOVERY, etc)
  
  AsyncWebServer_STM32 is a library for the STM32 run built-in Ethernet WebServer
  
  Based on and modified from ESPAsyncWebServer (https://github.com/me-no-dev/ESPAsyncWebServer)
  Built by Khoi Hoang https://github.com/khoih-prog/AsyncWebServer_STM32
  Licensed under MIT license
 
  Version: 1.0.0
  
  Version Modified By   Date      Comments
  ------- -----------  ---------- -----------
  1.0.0   K Hoang      02/09/2020 Initial coding for STM32 for built-in Ethernet (Nucleo-144, DISCOVERY, etc)
 *****************************************************************************************************************************/
/*
   Currently support
   1) STM32 boards with built-in Ethernet (to use USE_BUILTIN_ETHERNET = true) such as :
      - Nucleo-144 (F429ZI, F767ZI)
      - Discovery (STM32F746G-DISCOVERY)
      - STM32 boards (STM32F/L/H/G/WB/MP1) with 32K+ Flash, with Built-in Ethernet,
      - See How To Use Built-in Ethernet at (https://github.com/khoih-prog/EthernetWebServer_STM32/issues/1)
*/
//
// A simple server implementation showing how to:
//  * serve static messages
//  * read GET and POST parameters
//  * handle missing pages / 404s
//

#if !( defined(STM32F0) || defined(STM32F1) || defined(STM32F2) || defined(STM32F3)  ||defined(STM32F4) || defined(STM32F7) || \
       defined(STM32L0) || defined(STM32L1) || defined(STM32L4) || defined(STM32H7)  ||defined(STM32G0) || defined(STM32G4) || \
       defined(STM32WB) || defined(STM32MP1) )
  #error This code is designed to run on STM32F/L/H/G/WB/MP1 platform! Please check your Tools->Board setting.
#endif

#if defined(STM32F0)
  #warning STM32F0 board selected
  #define BOARD_TYPE  "STM32F0"
#elif defined(STM32F1)
  #warning STM32F1 board selected
  #define BOARD_TYPE  "STM32F1"
#elif defined(STM32F2)
  #warning STM32F2 board selected
  #define BOARD_TYPE  "STM32F2"
#elif defined(STM32F3)
  #warning STM32F3 board selected
  #define BOARD_TYPE  "STM32F3"
#elif defined(STM32F4)
  #warning STM32F4 board selected
  #define BOARD_TYPE  "STM32F4"
#elif defined(STM32F7)
  #warning STM32F7 board selected
  #define BOARD_TYPE  "STM32F7"
#elif defined(STM32L0)
  #warning STM32L0 board selected
  #define BOARD_TYPE  "STM32L0"
#elif defined(STM32L1)
  #warning STM32L1 board selected
  #define BOARD_TYPE  "STM32L1"
#elif defined(STM32L4)
  #warning STM32L4 board selected
  #define BOARD_TYPE  "STM32L4"
#elif defined(STM32H7)
  #warning STM32H7 board selected
  #define BOARD_TYPE  "STM32H7"
#elif defined(STM32G0)
  #warning STM32G0 board selected
  #define BOARD_TYPE  "STM32G0"
#elif defined(STM32G4)
  #warning STM32G4 board selected
  #define BOARD_TYPE  "STM32G4"
#elif defined(STM32WB)
  #warning STM32WB board selected
  #define BOARD_TYPE  "STM32WB"
#elif defined(STM32MP1)
  #warning STM32MP1 board selected
  #define BOARD_TYPE  "STM32MP1"
#else
  #warning STM32 unknown board selected
  #define BOARD_TYPE  "STM32 Unknown"
#endif

#ifndef BOARD_NAME
  #define BOARD_NAME    BOARD_TYPE
#endif

#include <LwIP.h>
#include <STM32Ethernet.h>
#include <AsyncWebServer_STM32.h>

// Enter a MAC address and IP address for your controller below.
#define NUMBER_OF_MAC      20

byte mac[][NUMBER_OF_MAC] =
{
  { 0xDE, 0xAD, 0xBE, 0xEF, 0x32, 0x01 },
  { 0xDE, 0xAD, 0xBE, 0xEF, 0x32, 0x02 },
  { 0xDE, 0xAD, 0xBE, 0xEF, 0x32, 0x03 },
  { 0xDE, 0xAD, 0xBE, 0xEF, 0x32, 0x04 },
  { 0xDE, 0xAD, 0xBE, 0xEF, 0x32, 0x05 },
  { 0xDE, 0xAD, 0xBE, 0xEF, 0x32, 0x06 },
  { 0xDE, 0xAD, 0xBE, 0xEF, 0x32, 0x07 },
  { 0xDE, 0xAD, 0xBE, 0xEF, 0x32, 0x08 },
  { 0xDE, 0xAD, 0xBE, 0xEF, 0x32, 0x09 },
  { 0xDE, 0xAD, 0xBE, 0xEF, 0x32, 0x0A },
  { 0xDE, 0xAD, 0xBE, 0xEF, 0x32, 0x0B },
  { 0xDE, 0xAD, 0xBE, 0xEF, 0x32, 0x0C },
  { 0xDE, 0xAD, 0xBE, 0xEF, 0x32, 0x0D },
  { 0xDE, 0xAD, 0xBE, 0xEF, 0x32, 0x0E },
  { 0xDE, 0xAD, 0xBE, 0xEF, 0x32, 0x0F },
  { 0xDE, 0xAD, 0xBE, 0xEF, 0x32, 0x10 },
  { 0xDE, 0xAD, 0xBE, 0xEF, 0x32, 0x11 },
  { 0xDE, 0xAD, 0xBE, 0xEF, 0x32, 0x12 },
  { 0xDE, 0xAD, 0xBE, 0xEF, 0x32, 0x13 },
  { 0xDE, 0xAD, 0xBE, 0xEF, 0x32, 0x14 },
};
// Select the IP address according to your local network
IPAddress ip(192, 168, 2, 232);

AsyncWebServer    server(80);

const char* PARAM_MESSAGE = "message";

void notFound(AsyncWebServerRequest *request)
{
  request->send(404, "text/plain", "Not found");
}

void setup() 
{
  Serial.begin(115200);
  while (!Serial);

  Serial.println("\nStarting Async_SimpleWebServer_STM32 on " + String(BOARD_NAME));

  // start the ethernet connection and the server
  // Use random mac
  uint16_t index = millis() % NUMBER_OF_MAC;

  // Use Static IP
  //Ethernet.begin(mac[index], ip);
  // Use DHCP dynamic IP and random mac
  Ethernet.begin(mac[index]);

  server.on("/", HTTP_GET, [](AsyncWebServerRequest * request) 
  {
    request->send(200, "text/plain", "Hello, world");
  });

  // Send a GET request to <IP>/get?message=<message>
  server.on("/get", HTTP_GET, [] (AsyncWebServerRequest * request) 
  {
    String message;
    
    if (request->hasParam(PARAM_MESSAGE)) 
    {
      message = request->getParam(PARAM_MESSAGE)->value();
    } 
    else 
    {
      message = "No message sent";
    }
    
    request->send(200, "text/plain", "Hello, GET: " + message);
  });

  // Send a POST request to <IP>/post with a form field message set to <message>
  server.on("/post", HTTP_POST, [](AsyncWebServerRequest * request) 
  {
    String message;
    
    if (request->hasParam(PARAM_MESSAGE, true)) 
    {
      message = request->getParam(PARAM_MESSAGE, true)->value();
    } 
    else 
    {
      message = "No message sent";
    }
    
    request->send(200, "text/plain", "Hello, POST: " + message);
  });

  server.onNotFound(notFound);

  server.begin();

  Serial.print("Server started @ ");
  Serial.println(Ethernet.localIP());
}

void loop()
{
}
