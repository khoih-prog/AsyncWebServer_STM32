/****************************************************************************************************************************
  WebClient.h - Dead simple AsyncWebServer for STM32 built-in LAN8742A Ethernet
  
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

#include "defines.h"

char server[] = "arduino.cc";

// Initialize the Web client object
EthernetClient client;

void setup()
{
  // Open serial communications and wait for port to open:
  Serial.begin(115200);
  while (!Serial);

  Serial.printf("\nStarting WebClient on %s with %s\n", BOARD_NAME, SHIELD_TYPE);
  Serial.println(ASYNC_WEBSERVER_STM32_VERSION);

  // start the ethernet connection and the server
  // Use random mac
  uint16_t index = millis() % NUMBER_OF_MAC;

  // Use Static IP
  //Ethernet.begin(mac[index], ip);
  // Use DHCP dynamic IP and random mac
  Ethernet.begin(mac[index]);

  // you're connected now, so print out the data
  Serial.print(F("You're connected to the network, IP = "));
  Serial.println(Ethernet.localIP());

  Serial.println();
  Serial.println(F("Starting connection to server..."));

  // if you get a connection, report back via serial
  if (client.connect(server, 80))
  {
    Serial.println(F("Connected to server"));
    // Make a HTTP request
    client.println(F("GET /asciilogo.txt HTTP/1.1"));
    client.println(F("Host: arduino.cc"));
    client.println(F("Connection: close"));
    client.println();
  }
}

void printoutData(void)
{
  // if there are incoming bytes available
  // from the server, read them and print them
  while (client.available())
  {
    char c = client.read();
    Serial.write(c);
  }
}

void loop()
{
  printoutData();

  // if the server's disconnected, stop the client
  if (!client.connected())
  {
    Serial.println();
    Serial.println(F("Disconnecting from server..."));
    client.stop();

    // do nothing forevermore
    while (true);
  }
}
