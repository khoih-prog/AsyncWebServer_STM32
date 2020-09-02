//
// A simple server implementation with regex routes:
//  * serve static messages
//  * read GET and POST parameters
//  * handle missing pages / 404s
//

// Add buildflag ASYNCWEBSERVER_REGEX to enable the regex support

// For platformio: platformio.ini:
//  build_flags =
//      -DASYNCWEBSERVER_REGEX

// For arduino IDE: create/update platform.local.txt
// Windows: C:\Users\(username)\AppData\Local\Arduino15\packages\espxxxx\hardware\espxxxx\{version}\platform.local.txt
// Linux: ~/.arduino15/packages/espxxxx/hardware/espxxxx/{version}/platform.local.txt
//
// compiler.cpp.extra_flags=-DASYNCWEBSERVER_REGEX=1

#define ASYNCWEBSERVER_REGEX      true

#include <Arduino.h>

#include <LwIP.h>
#include <STM32Ethernet.h>

#include <STM32AsyncTCP.h>
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

AsyncWebServer server(80);

const char* PARAM_MESSAGE = "message";

void notFound(AsyncWebServerRequest *request)
{
  request->send(404, "text/plain", "Not found");
}

void setup() 
{
  Serial.begin(115200);
  while (!Serial);

  Serial.println("\nStarting Async_RegexPatterns_STM32 on " + String(BOARD_NAME));

  // start the ethernet connection and the server
  // Use random mac
  uint16_t index = millis() % NUMBER_OF_MAC;

  // Use Static IP
  //Ethernet.begin(mac[index], ip);
  // Use DHCP dynamic IP and random mac
  Ethernet.begin(mac[index]);

  server.on("/", HTTP_GET, [](AsyncWebServerRequest * request) 
  {
    request->send(200, "text/plain", "Hello, world from Async_RegexPatterns_STM32 on " + String(BOARD_NAME));
  });

  // Send a GET request to <IP>/sensor/<number>
  server.on("^\\/sensor\\/([0-9]+)$", HTTP_GET, [] (AsyncWebServerRequest * request) 
  {
    String sensorNumber = request->pathArg(0);
    request->send(200, "text/plain", "Hello, sensor: " + sensorNumber);
  });

  // Send a GET request to <IP>/sensor/<number>/action/<action>
  server.on("^\\/sensor\\/([0-9]+)\\/action\\/([a-zA-Z0-9]+)$", HTTP_GET, [] (AsyncWebServerRequest * request) 
  {
    String sensorNumber = request->pathArg(0);
    String action = request->pathArg(1);
    request->send(200, "text/plain", "Hello, sensor: " + sensorNumber + ", with action: " + action);
  });

  server.onNotFound(notFound);

  server.begin();

  Serial.print("Server started @ ");
  Serial.println(Ethernet.localIP());
}

void loop() 
{
}
