/****************************************************************************************************************************
  AsyncWebServer_Debug_STM32.h - Dead simple AsyncWebServer for STM32 LAN8720 or built-in LAN8742A Ethernet

  For STM32 with LAN8720 (STM32F4/F7) or built-in LAN8742A Ethernet (Nucleo-144, DISCOVERY, etc)

  AsyncWebServer_STM32 is a library for the STM32 with LAN8720 or built-in LAN8742A Ethernet WebServer

  Based on and modified from ESPAsyncWebServer (https://github.com/me-no-dev/ESPAsyncWebServer)
  Built by Khoi Hoang https://github.com/khoih-prog/AsyncWebServer_STM32

  Copyright (c) 2016 Hristo Gochkov. All rights reserved.
  This file is part of the esp8266 core for Arduino environment.
  This program is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License
  as published bythe Free Software Foundation, either version 3 of the License, or (at your option) any later version.
  This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more details.
  You should have received a copy of the GNU General Public License along with this program.
  If not, see <https://www.gnu.org/licenses/>

  Version: 1.6.1

  Version Modified By   Date      Comments
  ------- -----------  ---------- -----------
  1.2.3   K Hoang      02/09/2020 Initial coding for STM32 for built-in Ethernet (Nucleo-144, DISCOVERY, etc).
                                  Bump up version to v1.2.3 to sync with ESPAsyncWebServer v1.2.3
  1.2.4   K Hoang      05/09/2020 Add back MD5/SHA1 authentication feature.
  1.2.5   K Hoang      28/12/2020 Suppress all possible compiler warnings. Add examples.
  1.2.6   K Hoang      22/03/2021 Fix dependency on STM32AsyncTCP Library
  1.3.0   K Hoang      14/04/2021 Add support to LAN8720 using STM32F4 or STM32F7
  1.3.1   K Hoang      09/10/2021 Update `platform.ini` and `library.json`
  1.4.0   K Hoang      14/12/2021 Fix base64 encoding of websocket client key and add WebServer progmem support
  1.4.1   K Hoang      12/01/2022 Fix authenticate issue caused by libb64
  1.5.0   K Hoang      22/06/2022 Update for STM32 core v2.3.0
  1.6.0   K Hoang      06/10/2022 Option to use non-destroyed cString instead of String to save Heap
  1.6.1   K Hoang      11/11/2022 Add examples to demo how to use beginChunkedResponse() to send in chunks
 *****************************************************************************************************************************/

#pragma once

#ifndef AsyncWebServer_Debug_STM32_H
#define AsyncWebServer_Debug_STM32_H

#ifdef ASYNCWEBSERVER_STM32_DEBUG_PORT
  #define DBG_PORT_AWS      ASYNCWEBSERVER_STM32_DEBUG_PORT
#else
  #define DBG_PORT_AWS      Serial
#endif

// Change _ASYNCWEBSERVER_STM32_LOGLEVEL_ to set tracing and logging verbosity
// 0: DISABLED: no logging
// 1: ERROR: errors
// 2: WARN: errors and warnings
// 3: INFO: errors, warnings and informational (default)
// 4: DEBUG: errors, warnings, informational and debug

#ifndef _ASYNCWEBSERVER_STM32_LOGLEVEL_
  #define _ASYNCWEBSERVER_STM32_LOGLEVEL_       0
#endif

/////////////////////////////////////////////////////////

#define AWS_PRINT_MARK      AWS_PRINT("[AWS] ")
#define AWS_PRINT_SP        DBG_PORT_AWS.print(" ")

#define AWS_PRINT           DBG_PORT_AWS.print
#define AWS_PRINTLN         DBG_PORT_AWS.println

/////////////////////////////////////////////////////////

#define LOGERROR(x)         if(_ASYNCWEBSERVER_STM32_LOGLEVEL_>0) { AWS_PRINT_MARK; AWS_PRINTLN(x); }
#define LOGERROR0(x)        if(_ASYNCWEBSERVER_STM32_LOGLEVEL_>0) { AWS_PRINT(x); }
#define LOGERROR1(x,y)      if(_ASYNCWEBSERVER_STM32_LOGLEVEL_>0) { AWS_PRINT_MARK; AWS_PRINT(x); AWS_PRINT_SP; AWS_PRINTLN(y); }
#define LOGERROR2(x,y,z)    if(_ASYNCWEBSERVER_STM32_LOGLEVEL_>0) { AWS_PRINT_MARK; AWS_PRINT(x); AWS_PRINT_SP; AWS_PRINT(y); AWS_PRINT_SP; AWS_PRINTLN(z); }
#define LOGERROR3(x,y,z,w)  if(_ASYNCWEBSERVER_STM32_LOGLEVEL_>0) { AWS_PRINT_MARK; AWS_PRINT(x); AWS_PRINT_SP; AWS_PRINT(y); AWS_PRINT_SP; AWS_PRINT(z); AWS_PRINT_SP; AWS_PRINTLN(w); }

/////////////////////////////////////////////////////////

#define LOGWARN(x)          if(_ASYNCWEBSERVER_STM32_LOGLEVEL_>1) { AWS_PRINT_MARK; AWS_PRINTLN(x); }
#define LOGWARN0(x)         if(_ASYNCWEBSERVER_STM32_LOGLEVEL_>1) { AWS_PRINT(x); }
#define LOGWARN1(x,y)       if(_ASYNCWEBSERVER_STM32_LOGLEVEL_>1) { AWS_PRINT_MARK; AWS_PRINT(x); AWS_PRINT_SP; AWS_PRINTLN(y); }
#define LOGWARN2(x,y,z)     if(_ASYNCWEBSERVER_STM32_LOGLEVEL_>1) { AWS_PRINT_MARK; AWS_PRINT(x); AWS_PRINT_SP; AWS_PRINT(y); AWS_PRINT_SP; AWS_PRINTLN(z); }
#define LOGWARN3(x,y,z,w)   if(_ASYNCWEBSERVER_STM32_LOGLEVEL_>1) { AWS_PRINT_MARK; AWS_PRINT(x); AWS_PRINT_SP; AWS_PRINT(y); AWS_PRINT_SP; AWS_PRINT(z); AWS_PRINT_SP; AWS_PRINTLN(w); }

/////////////////////////////////////////////////////////

#define LOGINFO(x)          if(_ASYNCWEBSERVER_STM32_LOGLEVEL_>2) { AWS_PRINT_MARK; AWS_PRINTLN(x); }
#define LOGINFO0(x)         if(_ASYNCWEBSERVER_STM32_LOGLEVEL_>2) { AWS_PRINT(x); }
#define LOGINFO1(x,y)       if(_ASYNCWEBSERVER_STM32_LOGLEVEL_>2) { AWS_PRINT_MARK; AWS_PRINT(x); AWS_PRINT_SP; AWS_PRINTLN(y); }
#define LOGINFO2(x,y,z)     if(_ASYNCWEBSERVER_STM32_LOGLEVEL_>2) { AWS_PRINT_MARK; AWS_PRINT(x); AWS_PRINT_SP; AWS_PRINT(y); AWS_PRINT_SP; AWS_PRINTLN(z); }
#define LOGINFO3(x,y,z,w)   if(_ASYNCWEBSERVER_STM32_LOGLEVEL_>2) { AWS_PRINT_MARK; AWS_PRINT(x); AWS_PRINT_SP; AWS_PRINT(y); AWS_PRINT_SP; AWS_PRINT(z); AWS_PRINT_SP; AWS_PRINTLN(w); }

/////////////////////////////////////////////////////////

#define LOGDEBUG(x)         if(_ASYNCWEBSERVER_STM32_LOGLEVEL_>3) { AWS_PRINT_MARK; AWS_PRINTLN(x); }
#define LOGDEBUG0(x)        if(_ASYNCWEBSERVER_STM32_LOGLEVEL_>3) { AWS_PRINT(x); }
#define LOGDEBUG1(x,y)      if(_ASYNCWEBSERVER_STM32_LOGLEVEL_>3) { AWS_PRINT_MARK; AWS_PRINT(x); AWS_PRINT_SP; AWS_PRINTLN(y); }
#define LOGDEBUG2(x,y,z)    if(_ASYNCWEBSERVER_STM32_LOGLEVEL_>3) { AWS_PRINT_MARK; AWS_PRINT(x); AWS_PRINT_SP; AWS_PRINT(y); AWS_PRINT_SP; AWS_PRINTLN(z); }
#define LOGDEBUG3(x,y,z,w)  if(_ASYNCWEBSERVER_STM32_LOGLEVEL_>3) { AWS_PRINT_MARK; AWS_PRINT(x); AWS_PRINT_SP; AWS_PRINT(y); AWS_PRINT_SP; AWS_PRINT(z); AWS_PRINT_SP; AWS_PRINTLN(w); }

/////////////////////////////////////////////////////////

#endif    //AsyncWebServer_Debug_STM32_H
