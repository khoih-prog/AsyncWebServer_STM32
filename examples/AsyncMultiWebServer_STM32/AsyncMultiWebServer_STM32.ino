/****************************************************************************************************************************
  AsyncMultiWebServer_STM32.h - Dead simple AsyncWebServer for STM32 built-in LAN8742A Ethernet
  
  For STM32 with built-in LAN8742A Ethernet (Nucleo-144, DISCOVERY, etc)
  
  AsyncWebServer_STM32 is a library for the STM32 run built-in Ethernet WebServer
  
  Based on and modified from ESPAsyncWebServer (https://github.com/me-no-dev/ESPAsyncWebServer)
  Built by Khoi Hoang https://github.com/khoih-prog/AsyncWebServer_STM32
  Licensed under MIT license
 
  Version: 1.2.5
  
  Version Modified By   Date      Comments
  ------- -----------  ---------- -----------
  1.2.3   K Hoang      02/09/2020 Initial coding for STM32 for built-in Ethernet (Nucleo-144, DISCOVERY, etc).
                                  Bump up version to v1.2.3 to sync with ESPAsyncWebServer v1.2.3
  1.2.4   K Hoang      05/09/2020 Add back MD5/SHA1 authentication feature.
  1.2.5   K Hoang      28/12/2020 Suppress all possible compiler warnings. Add examples.
 *****************************************************************************************************************************/
/*
   Currently support
   1) STM32 boards with built-in Ethernet (to use USE_BUILTIN_ETHERNET = true) such as :
      - Nucleo-144 (F429ZI, F767ZI)
      - Discovery (STM32F746G-DISCOVERY)
      - STM32 boards (STM32F/L/H/G/WB/MP1) with 32K+ Flash, with Built-in Ethernet,
      - See How To Use Built-in Ethernet at (https://github.com/khoih-prog/EthernetWebServer_STM32/issues/1)
*/

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

#define SHIELD_TYPE     "LAN8742A built-in Ethernet"

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

unsigned int    analogReadPin []  = { 12, 13, 14 };

#define BUFFER_SIZE       500

#define HTTP_PORT1        8080
#define HTTP_PORT2        8081
#define HTTP_PORT3        8082

AsyncWebServer* server1;
AsyncWebServer* server2;
AsyncWebServer* server3;

AsyncWebServer*  multiServer  []  = { server1, server2, server3 };
uint16_t        http_port     []  = { HTTP_PORT1, HTTP_PORT2, HTTP_PORT3 };

#define NUM_SERVERS     ( sizeof(multiServer) / sizeof(AsyncWebServer*) )

unsigned int serverIndex;

String createBuffer()
{
  char temp[BUFFER_SIZE];

  memset(temp, 0, sizeof(temp));

  int sec = millis() / 1000;
  int min = sec / 60;
  int hr = min / 60;
  int day = hr / 24;

  snprintf(temp, BUFFER_SIZE - 1,
           "<html>\
<head>\
<meta http-equiv='refresh' content='5'/>\
<title>%s</title>\
<style>\
body { background-color: #cccccc; font-family: Arial, Helvetica, Sans-Serif; Color: #000088; }\
</style>\
</head>\
<body>\
<h1>Hello from %s</h1>\
<h2>running AsyncWebServer_STM32</h2>\
<h3>on %s</h3>\
<h3>Uptime: %d d %02d:%02d:%02d</h3>\
</body>\
</html>", BOARD_NAME, BOARD_NAME, "Built-in LAN8742A", day, hr, min % 60, sec % 60);

  return temp;
}


void handleRoot(AsyncWebServerRequest * request)
{
  String message = createBuffer();
  request->send(200, F("text/html"), message);
}

String createNotFoundBuffer(AsyncWebServerRequest * request)
{
  String message;

  message.reserve(500);

  message = F("File Not Found\n\n");

  message += F("URI: ");
  message += request->url();
  message += F("\nMethod: ");
  message += (request->method() == HTTP_GET) ? F("GET") : F("POST");
  message += F("\nArguments: ");
  message += request->args();
  message += F("\n");

  for (uint8_t i = 0; i < request->args(); i++)
  {
    message += " " + request->argName(i) + ": " + request->arg(i) + "\n";
  }

  return message;
}

void handleNotFound(AsyncWebServerRequest * request)
{
  String message = createNotFoundBuffer(request);
  request->send(404, F("text/plain"), message);
}

void setup()
{
  Serial.begin(115200);
  while (!Serial);

  delay(200);

  Serial.printf("\nStarting AsyncMultiWebServer_STM32 on %s with %s\n", BOARD_NAME, SHIELD_TYPE);
  Serial.println(ASYNC_WEBSERVER_STM32_VERSION);

  // start the ethernet connection and the server
  // Use random mac
  uint16_t index = millis() % NUMBER_OF_MAC;

  // Use Static IP
  //Ethernet.begin(mac[index], ip);
  // Use DHCP dynamic IP and random mac
  Ethernet.begin(mac[index]);

  Serial.print("\nConnected to network. IP = ");
  Serial.println(Ethernet.localIP());

  for (serverIndex = 0; serverIndex < NUM_SERVERS; serverIndex++)
  {
    multiServer[serverIndex] = new AsyncWebServer(http_port[serverIndex]);  

    if (multiServer[serverIndex])
    {
      Serial.printf("Initialize multiServer OK, serverIndex = %d, port = %d\n", serverIndex, http_port[serverIndex]);
    }
    else
    {
      Serial.printf("Error initialize multiServer, serverIndex = %d\n", serverIndex);

      while(1);
    }

    multiServer[serverIndex]->on("/", HTTP_GET, [](AsyncWebServerRequest * request)
    {
      handleRoot(request);
    });

    multiServer[serverIndex]->on("/hello", HTTP_GET, [](AsyncWebServerRequest * request) 
    {     
      String message = F("Hello from AsyncWebServer using built-in LAN8742A Ethernet, running on ");
      message       += BOARD_NAME;
      
      request->send(200, "text/plain", message);
    });
      
    multiServer[serverIndex]->onNotFound([](AsyncWebServerRequest * request) 
    {
      handleNotFound(request);
    });

    multiServer[serverIndex]->begin();

    Serial.printf("HTTP server started at ports %d\n", http_port[serverIndex]);
  }
}

void loop()
{
}
