/****************************************************************************************************************************
  AsyncWebHandlers_STM32.cpp - Dead simple AsyncWebServer for STM32 LAN8720 or built-in LAN8742A Ethernet

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

#if !defined(_ASYNCWEBSERVER_STM32_LOGLEVEL_)
  #define _ASYNCWEBSERVER_STM32_LOGLEVEL_     1
#endif

#include "AsyncWebServer_Debug_STM32.h"

#include "AsyncWebServer_STM32.h"
#include "AsyncWebHandlerImpl_STM32.h"

/////////////////////////////////////////////////

AsyncStaticWebHandler::AsyncStaticWebHandler(const char* uri, /*FS& fs,*/ const char* path, const char* cache_control)
  : _uri(uri), _path(path), _cache_control(cache_control), _last_modified(""), _callback(nullptr)
{
  // Ensure leading '/'
  if (_uri.length() == 0 || _uri[0] != '/')
    _uri = "/" + _uri;

  if (_path.length() == 0 || _path[0] != '/')
    _path = "/" + _path;

  // If path ends with '/' we assume a hint that this is a directory to improve performance.
  // However - if it does not end with '/' we, can't assume a file, path can still be a directory.
  _isDir = _path[_path.length() - 1] == '/';

  // Remove the trailing '/' so we can handle default file
  // Notice that root will be "" not "/"
  if (_uri[_uri.length() - 1] == '/')
    _uri = _uri.substring(0, _uri.length() - 1);

  if (_path[_path.length() - 1] == '/')
    _path = _path.substring(0, _path.length() - 1);

  // Reset stats
  _gzipFirst = false;
  _gzipStats = 0xF8;
}

/////////////////////////////////////////////////

AsyncStaticWebHandler& AsyncStaticWebHandler::setIsDir(bool isDir)
{
  _isDir = isDir;
  return *this;
}

/////////////////////////////////////////////////

AsyncStaticWebHandler& AsyncStaticWebHandler::setCacheControl(const char* cache_control)
{
  _cache_control = String(cache_control);

  return *this;
}

/////////////////////////////////////////////////

AsyncStaticWebHandler& AsyncStaticWebHandler::setLastModified(const char* last_modified)
{
  _last_modified = String(last_modified);

  return *this;
}

/////////////////////////////////////////////////

AsyncStaticWebHandler& AsyncStaticWebHandler::setLastModified(struct tm* last_modified)
{
  char result[30];

  strftime (result, 30, "%a, %d %b %Y %H:%M:%S %Z", last_modified);

  return setLastModified((const char *)result);
}

/////////////////////////////////////////////////

// For Teensy41
AsyncStaticWebHandler& AsyncStaticWebHandler::setLastModified(time_t last_modified)
{
  return setLastModified((struct tm *)gmtime(&last_modified));
}

/////////////////////////////////////////////////

AsyncStaticWebHandler& AsyncStaticWebHandler::setLastModified()
{
  time_t last_modified;

  if (time(&last_modified) == 0) //time is not yet set
    return *this;

  return setLastModified(last_modified);
}

/////////////////////////////////////////////////

bool AsyncStaticWebHandler::canHandle(AsyncWebServerRequest *request)
{
  if (request->method() != HTTP_GET
      || !request->url().startsWith(_uri)
      || !request->isExpectedRequestedConnType(RCT_DEFAULT, RCT_HTTP)
     )
  {
    return false;
  }

  return false;
}

/////////////////////////////////////////////////

// For STM32
#define FILE_IS_REAL(f) (f == true)

/////////////////////////////////////////////////

uint8_t AsyncStaticWebHandler::_countBits(const uint8_t value) const
{
  uint8_t w = value;
  uint8_t n;

  for (n = 0; w != 0; n++)
    w &= w - 1;

  return n;
}
