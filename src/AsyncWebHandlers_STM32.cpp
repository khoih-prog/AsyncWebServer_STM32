/****************************************************************************************************************************
  AsyncWebHandlers_STM32.cpp - Dead simple AsyncWebServer for STM32 built-in LAN8742A Ethernet
  
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

#define _ASYNCWEBSERVER_STM32_LOGLEVEL_     1

#include "AsyncWebServer_Debug_STM32.h"

#include "AsyncWebServer_STM32.h"
#include "AsyncWebHandlerImpl_STM32.h"

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

AsyncStaticWebHandler& AsyncStaticWebHandler::setIsDir(bool isDir)
{
  _isDir = isDir;
  return *this;
}

AsyncStaticWebHandler& AsyncStaticWebHandler::setCacheControl(const char* cache_control)
{
  _cache_control = String(cache_control);

  return *this;
}

AsyncStaticWebHandler& AsyncStaticWebHandler::setLastModified(const char* last_modified)
{
  _last_modified = String(last_modified);

  return *this;
}

AsyncStaticWebHandler& AsyncStaticWebHandler::setLastModified(struct tm* last_modified)
{
  char result[30];

  strftime (result, 30, "%a, %d %b %Y %H:%M:%S %Z", last_modified);

  return setLastModified((const char *)result);
}

// For STM32
AsyncStaticWebHandler& AsyncStaticWebHandler::setLastModified(time_t last_modified)
{
  return setLastModified((struct tm *)gmtime(&last_modified));
}

AsyncStaticWebHandler& AsyncStaticWebHandler::setLastModified()
{
  time_t last_modified;

  if (time(&last_modified) == 0) //time is not yet set
    return *this;

  return setLastModified(last_modified);
}

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

// For STM32
#define FILE_IS_REAL(f) (f == true)

uint8_t AsyncStaticWebHandler::_countBits(const uint8_t value) const
{
  uint8_t w = value;
  uint8_t n;

  for (n = 0; w != 0; n++)
    w &= w - 1;

  return n;
}
