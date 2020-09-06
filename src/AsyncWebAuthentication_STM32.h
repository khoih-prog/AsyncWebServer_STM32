/****************************************************************************************************************************
  AsyncWebAuthentication_STM32.h - Dead simple AsyncWebServer for STM32 built-in LAN8742A Ethernet
  
  For STM32 with built-in LAN8742A Ethernet (Nucleo-144, DISCOVERY, etc)
  
  AsyncWebServer_STM32 is a library for the STM32 run built-in Ethernet WebServer
  
  Based on and modified from ESPAsyncWebServer (https://github.com/me-no-dev/ESPAsyncWebServer)
  Built by Khoi Hoang https://github.com/khoih-prog/AsyncWebServer_STM32
  Licensed under MIT license
 
  Version: 1.2.3a
  
  Version Modified By   Date      Comments
  ------- -----------  ---------- -----------
  1.2.3   K Hoang      02/09/2020 Initial coding for STM32 for built-in Ethernet (Nucleo-144, DISCOVERY, etc).
                                  Bump up version to v1.2.3 to sync with ESPAsyncWebServer v1.2.3
  1.2.3a  K Hoang      05/09/2020 Add back MD5/SHA1 authentication feature.
 *****************************************************************************************************************************/
 
#ifndef ASYNCWEB_AUTHENTICATION_STM32_H_
#define ASYNCWEB_AUTHENTICATION_STM32_H_

#include "Arduino.h"
#include "AsyncWebServer_Debug_STM32.h"

bool checkBasicAuthentication(const char * header, const char * username, const char * password);
String requestDigestAuthentication(const char * realm);
bool checkDigestAuthentication(const char * header, const char * method, const char * username, const char * password, const char * realm, bool passwordIsHash, const char * nonce, const char * opaque, const char * uri);

//for storing hashed versions on the device that can be authenticated against
String generateDigestHash(const char * username, const char * password, const char * realm);

#endif    // ASYNCWEB_AUTHENTICATION_STM32_H_
