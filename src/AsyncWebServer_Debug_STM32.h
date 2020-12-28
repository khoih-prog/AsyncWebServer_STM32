/****************************************************************************************************************************
  AsyncWebServer_Debug_STM32.h - Dead simple AsyncWebServer for STM32 built-in LAN8742A Ethernet
  
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

#pragma once

#ifndef AsyncWebServer_Debug_STM32_H
#define AsyncWebServer_Debug_STM32_H

#ifdef ASYNCWEBSERVER_STM32_DEBUG_PORT
  #define DBG_PORT      ASYNCWEBSERVER_STM32_DEBUG_PORT
#else
  #define DBG_PORT      Serial
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

#define LOGERROR(x)         if(_ASYNCWEBSERVER_STM32_LOGLEVEL_>0) { DBG_PORT.print("[AWS] "); DBG_PORT.println(x); }
#define LOGERROR0(x)        if(_ASYNCWEBSERVER_STM32_LOGLEVEL_>0) { DBG_PORT.print(x); }
#define LOGERROR1(x,y)      if(_ASYNCWEBSERVER_STM32_LOGLEVEL_>0) { DBG_PORT.print("[AWS] "); DBG_PORT.print(x); DBG_PORT.print(" "); DBG_PORT.println(y); }
#define LOGERROR2(x,y,z)    if(_ASYNCWEBSERVER_STM32_LOGLEVEL_>0) { DBG_PORT.print("[AWS] "); DBG_PORT.print(x); DBG_PORT.print(" "); DBG_PORT.print(y); DBG_PORT.print(" "); DBG_PORT.println(z); }
#define LOGERROR3(x,y,z,w)  if(_ASYNCWEBSERVER_STM32_LOGLEVEL_>0) { DBG_PORT.print("[AWS] "); DBG_PORT.print(x); DBG_PORT.print(" "); DBG_PORT.print(y); DBG_PORT.print(" "); DBG_PORT.print(z); DBG_PORT.print(" "); DBG_PORT.println(w); }

#define LOGWARN(x)          if(_ASYNCWEBSERVER_STM32_LOGLEVEL_>1) { DBG_PORT.print("[AWS] "); DBG_PORT.println(x); }
#define LOGWARN0(x)         if(_ASYNCWEBSERVER_STM32_LOGLEVEL_>1) { DBG_PORT.print(x); }
#define LOGWARN1(x,y)       if(_ASYNCWEBSERVER_STM32_LOGLEVEL_>1) { DBG_PORT.print("[AWS] "); DBG_PORT.print(x); DBG_PORT.print(" "); DBG_PORT.println(y); }
#define LOGWARN2(x,y,z)     if(_ASYNCWEBSERVER_STM32_LOGLEVEL_>1) { DBG_PORT.print("[AWS] "); DBG_PORT.print(x); DBG_PORT.print(" "); DBG_PORT.print(y); DBG_PORT.print(" "); DBG_PORT.println(z); }
#define LOGWARN3(x,y,z,w)   if(_ASYNCWEBSERVER_STM32_LOGLEVEL_>1) { DBG_PORT.print("[AWS] "); DBG_PORT.print(x); DBG_PORT.print(" "); DBG_PORT.print(y); DBG_PORT.print(" "); DBG_PORT.print(z); DBG_PORT.print(" "); DBG_PORT.println(w); }

#define LOGINFO(x)          if(_ASYNCWEBSERVER_STM32_LOGLEVEL_>2) { DBG_PORT.print("[AWS] "); DBG_PORT.println(x); }
#define LOGINFO0(x)         if(_ASYNCWEBSERVER_STM32_LOGLEVEL_>2) { DBG_PORT.print(x); }
#define LOGINFO1(x,y)       if(_ASYNCWEBSERVER_STM32_LOGLEVEL_>2) { DBG_PORT.print("[AWS] "); DBG_PORT.print(x); DBG_PORT.print(" "); DBG_PORT.println(y); }
#define LOGINFO2(x,y,z)     if(_ASYNCWEBSERVER_STM32_LOGLEVEL_>2) { DBG_PORT.print("[AWS] "); DBG_PORT.print(x); DBG_PORT.print(" "); DBG_PORT.print(y); DBG_PORT.print(" "); DBG_PORT.println(z); }
#define LOGINFO3(x,y,z,w)   if(_ASYNCWEBSERVER_STM32_LOGLEVEL_>2) { DBG_PORT.print("[AWS] "); DBG_PORT.print(x); DBG_PORT.print(" "); DBG_PORT.print(y); DBG_PORT.print(" "); DBG_PORT.print(z); DBG_PORT.print(" "); DBG_PORT.println(w); }

#define LOGDEBUG(x)         if(_ASYNCWEBSERVER_STM32_LOGLEVEL_>3) { DBG_PORT.print("[AWS] "); DBG_PORT.println(x); }
#define LOGDEBUG0(x)        if(_ASYNCWEBSERVER_STM32_LOGLEVEL_>3) { DBG_PORT.print(x); }
#define LOGDEBUG1(x,y)      if(_ASYNCWEBSERVER_STM32_LOGLEVEL_>3) { DBG_PORT.print("[AWS] "); DBG_PORT.print(x); DBG_PORT.print(" "); DBG_PORT.println(y); }
#define LOGDEBUG2(x,y,z)    if(_ASYNCWEBSERVER_STM32_LOGLEVEL_>3) { DBG_PORT.print("[AWS] "); DBG_PORT.print(x); DBG_PORT.print(" "); DBG_PORT.print(y); DBG_PORT.print(" "); DBG_PORT.println(z); }
#define LOGDEBUG3(x,y,z,w)  if(_ASYNCWEBSERVER_STM32_LOGLEVEL_>3) { DBG_PORT.print("[AWS] "); DBG_PORT.print(x); DBG_PORT.print(" "); DBG_PORT.print(y); DBG_PORT.print(" "); DBG_PORT.print(z); DBG_PORT.print(" "); DBG_PORT.println(w); }

#endif    //AsyncWebServer_Debug_STM32_H
