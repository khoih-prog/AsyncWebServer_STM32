/****************************************************************************************************************************
  Async_AdvancedWebServer_SendChunked.ino

  For STM32 with LAN8720 (STM32F4/F7) or built-in LAN8742A Ethernet (Nucleo-144, DISCOVERY, etc)

  AsyncWebServer_STM32 is a library for the STM32 with LAN8720 or built-in LAN8742A Ethernet WebServer

  Based on and modified from ESPAsyncWebServer (https://github.com/me-no-dev/ESPAsyncWebServer)
  Built by Khoi Hoang https://github.com/khoih-prog/AsyncWebServer_STM32
  Licensed under GPLv3 license

  Copyright (c) 2015, Majenko Technologies
  All rights reserved.

  Redistribution and use in source and binary forms, with or without modification,
  are permitted provided that the following conditions are met:

  Redistributions of source code must retain the above copyright notice, this
  list of conditions and the following disclaimer.

  Redistributions in binary form must reproduce the above copyright notice, this
  list of conditions and the following disclaimer in the documentation and/or
  other materials provided with the distribution.

  Neither the name of Majenko Technologies nor the names of its
  contributors may be used to endorse or promote products derived from
  this software without specific prior written permission.

  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
  ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
  WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
  DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
  ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
  (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
  LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
  ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
  SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *****************************************************************************************************************************/

/*
   Currently support
   1) STM32 boards with built-in Ethernet (to use USE_BUILTIN_ETHERNET = true) such as :
      - Nucleo-144 (F429ZI, F767ZI)
      - Discovery (STM32F746G-DISCOVERY)
      - STM32 boards (STM32F/L/H/G/WB/MP1) with 32K+ Flash, with Built-in Ethernet,
      - See How To Use Built-in Ethernet at (https://github.com/khoih-prog/EthernetWebServer_STM32/issues/1)
   2) STM32F/L/H/G/WB/MP1 boards (with 64+K Flash) running ENC28J60 shields (to use USE_BUILTIN_ETHERNET = false)
   3) STM32F/L/H/G/WB/MP1 boards (with 64+K Flash) running W5x00 shields
   4) STM32F4 and STM32F7 boards (with 64+K Flash) running LAN8720 shields
*/

#include "defines.h"

// In bytes
#define STRING_SIZE                    40000

AsyncWebServer    server(80);

int reqCount = 0;                // number of requests received

const int led = 13;

#define BUFFER_SIZE         512
char temp[BUFFER_SIZE];

void handleRoot(AsyncWebServerRequest *request)
{
  digitalWrite(led, 1);
  
  int sec = millis() / 1000;
  int min = sec / 60;
  int hr = min / 60;
  int day = hr / 24;

  snprintf(temp, BUFFER_SIZE - 1,
           "<html>\
<head>\
<meta http-equiv='refresh' content='5'/>\
<title>AsyncWebServer-%s</title>\
<style>\
body { background-color: #cccccc; font-family: Arial, Helvetica, Sans-Serif; Color: #000088; }\
</style>\
</head>\
<body>\
<h2>Async_AdvancedWebServer_SendChunked!</h2>\
<h3>running on %s</h3>\
<h3>with %s</h3>\
<p>Uptime: %d d %02d:%02d:%02d</p>\
<img src=\"/test.svg\" />\
</body>\
</html>", BOARD_NAME, BOARD_NAME, SHIELD_TYPE, day, hr % 24, min % 60, sec % 60);

  request->send(200, "text/html", temp);

  digitalWrite(led, 0);
}

void handleNotFound(AsyncWebServerRequest *request)
{
  digitalWrite(led, 1);
  
  String message = "File Not Found\n\n";

  message += "URI: ";
  message += request->url();
  message += "\nMethod: ";
  message += (request->method() == HTTP_GET) ? "GET" : "POST";
  message += "\nArguments: ";
  message += request->args();
  message += "\n";

  for (uint8_t i = 0; i < request->args(); i++)
  {
    message += " " + request->argName(i) + ": " + request->arg(i) + "\n";
  }

  request->send(404, "text/plain", message);

  digitalWrite(led, 0);
}

String out;

void drawGraph(AsyncWebServerRequest *request)
{
  //out.reserve(STRING_SIZE);
  char temp[70];

  out = String();

  out += "<svg xmlns=\"http://www.w3.org/2000/svg\" version=\"1.1\" width=\"1810\" height=\"150\">\n";
  out += "<rect width=\"1810\" height=\"150\" fill=\"rgb(250, 230, 210)\" stroke-width=\"2\" stroke=\"rgb(0, 0, 0)\" />\n";
  out += "<g stroke=\"blue\">\n";
  int y = rand() % 130;

  for (int x = 10; x < 5000; x += 10)
  {
    int y2 = rand() % 130;
    sprintf(temp, "<line x1=\"%d\" y1=\"%d\" x2=\"%d\" y2=\"%d\" stroke-width=\"2\" />\n", x, 140 - y, x + 10, 140 - y2);
    out += temp;
    y = y2;
  }

  out += "</g>\n</svg>\n";

  LOGDEBUG1("Total length to send in chunks =", out.length());

  AsyncWebServerResponse *response = request->beginChunkedResponse("image/svg+xml", [](uint8_t *buffer, size_t maxLen, size_t filledLength) -> size_t
  {
    size_t len = min(maxLen, out.length() - filledLength);
    memcpy(buffer, out.c_str() + filledLength, len);

    LOGDEBUG1("Bytes sent in chunk =", len);
    
    return len;
  });

  request->send(response);
}

void setup()
{
  out.reserve(STRING_SIZE);
  
  pinMode(led, OUTPUT);
  digitalWrite(led, 0);
  
  Serial.begin(115200);

  while (!Serial && millis() < 5000);

  delay(200);

  Serial.print("\nStart Async_AdvancedWebServer_SendChunked on ");
  Serial.print(BOARD_NAME);
  Serial.print(" with ");
  Serial.println(SHIELD_TYPE);
  Serial.println(ASYNC_WEBSERVER_STM32_VERSION);

#if (_ASYNCWEBSERVER_STM32_LOGLEVEL_ > 2)
  Serial.print("STM32 Core version v");
  Serial.print(STM32_CORE_VERSION_MAJOR);
  Serial.print(".");
  Serial.print(STM32_CORE_VERSION_MINOR);
  Serial.print(".");
  Serial.println(STM32_CORE_VERSION_PATCH);
#endif

  // start the ethernet connection and the server
  // Use random mac
  uint16_t index = millis() % NUMBER_OF_MAC;

  // Use Static IP
  //Ethernet.begin(mac[index], ip);
  // Use DHCP dynamic IP and random mac
  Ethernet.begin(mac[index]);

  ///////////////////////////////////

  server.on("/", HTTP_GET, [](AsyncWebServerRequest * request)
  {
    handleRoot(request);
  });

  server.on("/test.svg", HTTP_GET, [](AsyncWebServerRequest * request)
  {
    drawGraph(request);
  });

  server.on("/inline", [](AsyncWebServerRequest * request)
  {
    request->send(200, "text/plain", "This works as well");
  });

  server.onNotFound(handleNotFound);

  server.begin();

  Serial.print(F("AsyncWebServer is @ IP : "));
  Serial.println(Ethernet.localIP());
}

void heartBeatPrint()
{
  static int num = 1;

  Serial.print(F("."));

  if (num == 80)
  {
    Serial.println();
    num = 1;
  }
  else if (num++ % 10 == 0)
  {
    Serial.print(F(" "));
  }
}

void check_status()
{
  static unsigned long checkstatus_timeout = 0;

#define STATUS_CHECK_INTERVAL     10000L

  // Send status report every STATUS_REPORT_INTERVAL (60) seconds: we don't need to send updates frequently if there is no status change.
  if ((millis() > checkstatus_timeout) || (checkstatus_timeout == 0))
  {
    heartBeatPrint();
    checkstatus_timeout = millis() + STATUS_CHECK_INTERVAL;
  }
}

void loop()
{
  check_status();
}
