/****************************************************************************************************************************
  AsyncWebHandlerImpl_STM32.h - Dead simple AsyncWebServer for STM32 LAN8720 or built-in LAN8742A Ethernet

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

#ifndef ASYNCWEBSERVERHANDLERIMPL_STM32_H_
#define ASYNCWEBSERVERHANDLERIMPL_STM32_H_

#include <string>

#ifdef ASYNCWEBSERVER_REGEX
  #include <regex>
#endif

#include "stddef.h"
#include <time.h>

/////////////////////////////////////////////////

class AsyncStaticWebHandler: public AsyncWebHandler
{
  private:
    uint8_t _countBits(const uint8_t value) const;

  protected:
    String _uri;
    String _path;
    String _cache_control;
    String _last_modified;
    AwsTemplateProcessor _callback;
    bool _isDir;
    bool _gzipFirst;
    uint8_t _gzipStats;

  public:
    AsyncStaticWebHandler(const char* uri, const char* path, const char* cache_control);
    virtual bool canHandle(AsyncWebServerRequest *request) override final;
    virtual void handleRequest(AsyncWebServerRequest *request) override final;
    AsyncStaticWebHandler& setIsDir(bool isDir);
    AsyncStaticWebHandler& setCacheControl(const char* cache_control);
    AsyncStaticWebHandler& setLastModified(const char* last_modified);
    AsyncStaticWebHandler& setLastModified(struct tm* last_modified);

    AsyncStaticWebHandler& setLastModified(time_t last_modified);
    AsyncStaticWebHandler& setLastModified(); //sets to current time. Make sure sntp is runing and time is updated

    /////////////////////////////////////////////////

    AsyncStaticWebHandler& setTemplateProcessor(AwsTemplateProcessor newCallback)
    {
      _callback = newCallback;
      return *this;
    }
};

class AsyncCallbackWebHandler: public AsyncWebHandler
{
  private:
  protected:
    String _uri;
    WebRequestMethodComposite _method;
    ArRequestHandlerFunction _onRequest;
    ArUploadHandlerFunction _onUpload;
    ArBodyHandlerFunction _onBody;
    bool _isRegex;

  public:
    AsyncCallbackWebHandler() : _uri(), _method(HTTP_ANY), _onRequest(NULL), _onUpload(NULL), _onBody(NULL),
      _isRegex(false) {}

    /////////////////////////////////////////////////

    inline void setUri(const String& uri)
    {
      _uri = uri;
      _isRegex = uri.startsWith("^") && uri.endsWith("$");
    }

    /////////////////////////////////////////////////

    inline void setMethod(WebRequestMethodComposite method)
    {
      _method = method;
    }

    /////////////////////////////////////////////////

    inline void onRequest(ArRequestHandlerFunction fn)
    {
      _onRequest = fn;
    }

    /////////////////////////////////////////////////

    inline void onUpload(ArUploadHandlerFunction fn)
    {
      _onUpload = fn;
    }

    /////////////////////////////////////////////////

    inline void onBody(ArBodyHandlerFunction fn)
    {
      _onBody = fn;
    }

    /////////////////////////////////////////////////

    virtual bool canHandle(AsyncWebServerRequest *request) override final
    {
      if (!_onRequest)
        return false;

      if (!(_method & request->method()))
        return false;

#ifdef ASYNCWEBSERVER_REGEX

      if (_isRegex)
      {
        std::regex pattern(_uri.c_str());
        std::smatch matches;
        std::string s(request->url().c_str());

        if (std::regex_search(s, matches, pattern))
        {
          for (size_t i = 1; i < matches.size(); ++i)
          {
            // start from 1
            request->_addPathParam(matches[i].str().c_str());
          }
        }
        else
        {
          return false;
        }
      }
      else
#endif
        if (_uri.length() && _uri.endsWith("*"))
        {
          String uriTemplate = String(_uri);
          uriTemplate = uriTemplate.substring(0, uriTemplate.length() - 1);

          if (!request->url().startsWith(uriTemplate))
            return false;
        }
        else if (_uri.length() && (_uri != request->url() && !request->url().startsWith(_uri + "/")))
          return false;

      request->addInterestingHeader("ANY");

      return true;
    }

    /////////////////////////////////////////////////

    virtual void handleRequest(AsyncWebServerRequest *request) override final
    {
      if (_onRequest)
        _onRequest(request);
      else
        request->send(500);
    }

    /////////////////////////////////////////////////

    virtual void handleBody(AsyncWebServerRequest *request, uint8_t *data, size_t len, size_t index,
                            size_t total) override final
    {
      if (_onBody)
        _onBody(request, data, len, index, total);
    }

    /////////////////////////////////////////////////

    virtual bool isRequestHandlerTrivial() override final
    {
      return _onRequest ? false : true;
    }
};

#endif /* ASYNCWEBSERVERHANDLERIMPL_STM32_H_ */
