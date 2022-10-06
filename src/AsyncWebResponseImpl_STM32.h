/****************************************************************************************************************************
  AsyncWebResponseImpl_STM32.h - Dead simple AsyncWebServer for STM32 LAN8720 or built-in LAN8742A Ethernet

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

  Version: 1.6.0

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
 *****************************************************************************************************************************/

#pragma once

#ifndef ASYNCWEBSERVERRESPONSEIMPL_STM32_H_
#define ASYNCWEBSERVERRESPONSEIMPL_STM32_H_

/////////////////////////////////////////////////

#ifdef Arduino_h
  // arduino is not compatible with std::vector
  #undef min
  #undef max
#endif

/////////////////////////////////////////////////

#include <vector>

// It is possible to restore these defines, but one can use _min and _max instead. Or std::min, std::max.

/////////////////////////////////////////////////

class AsyncBasicResponse: public AsyncWebServerResponse
{
  private:
    String _content;
    
    char *_contentCstr;      // RSMOD
    String _partialHeader;

  public:
    AsyncBasicResponse(int code, const String& contentType = String(), const String& content = String());
    
    AsyncBasicResponse(int code, const String& contentType, const char *content = nullptr);     // RSMOD
    
    void _respond(AsyncWebServerRequest *request);

    size_t _ack(AsyncWebServerRequest *request, size_t len, uint32_t time);

    /////////////////////////////////////////////////
    
    inline bool _sourceValid() const 
    {
      return true;
    }

    /////////////////////////////////////////////////
};

/////////////////////////////////////////////////

class AsyncAbstractResponse: public AsyncWebServerResponse
{
  private:
    String _head;
    // Data is inserted into cache at begin().
    // This is inefficient with vector, but if we use some other container,
    // we won't be able to access it as contiguous array of bytes when reading from it,
    // so by gaining performance in one place, we'll lose it in another.
    std::vector<uint8_t> _cache;
    size_t _readDataFromCacheOrContent(uint8_t* data, const size_t len);
    size_t _fillBufferAndProcessTemplates(uint8_t* buf, size_t maxLen);

  protected:
    AwsTemplateProcessor _callback;

  public:
    AsyncAbstractResponse(AwsTemplateProcessor callback = nullptr);
    void _respond(AsyncWebServerRequest *request);
    size_t _ack(AsyncWebServerRequest *request, size_t len, uint32_t time);

    /////////////////////////////////////////////////
    
    inline bool _sourceValid() const 
    {
      return false;
    }

    /////////////////////////////////////////////////
    
    virtual size_t _fillBuffer(uint8_t *buf __attribute__((unused)), size_t maxLen __attribute__((unused))) 
    {
      return 0;
    }

    /////////////////////////////////////////////////
};

/////////////////////////////////////////////////

#ifndef TEMPLATE_PLACEHOLDER
  #define TEMPLATE_PLACEHOLDER '%'
#endif

#define TEMPLATE_PARAM_NAME_LENGTH 32

/////////////////////////////////////////////////

class AsyncStreamResponse: public AsyncAbstractResponse
{
  private:
    Stream *_content;

  public:
    AsyncStreamResponse(Stream &stream, const String& contentType, size_t len, AwsTemplateProcessor callback = nullptr);

    /////////////////////////////////////////////////
    
    inline bool _sourceValid() const 
    {
      return !!(_content);
    }

    /////////////////////////////////////////////////

    virtual size_t _fillBuffer(uint8_t *buf, size_t maxLen) override;
};

/////////////////////////////////////////////////

class AsyncCallbackResponse: public AsyncAbstractResponse
{
  private:
    AwsResponseFiller _content;
    size_t _filledLength;

  public:
    AsyncCallbackResponse(const String& contentType, size_t len, AwsResponseFiller callback, AwsTemplateProcessor templateCallback = nullptr);

    /////////////////////////////////////////////////
    
    inline bool _sourceValid() const 
    {
      return !!(_content);
    }

    /////////////////////////////////////////////////

    virtual size_t _fillBuffer(uint8_t *buf, size_t maxLen) override;
};

/////////////////////////////////////////////////

class AsyncChunkedResponse: public AsyncAbstractResponse
{
  private:
    AwsResponseFiller _content;
    size_t _filledLength;

  public:
    AsyncChunkedResponse(const String& contentType, AwsResponseFiller callback, AwsTemplateProcessor templateCallback = nullptr);

    /////////////////////////////////////////////////
    
    inline bool _sourceValid() const 
    {
      return !!(_content);
    }

    /////////////////////////////////////////////////

    virtual size_t _fillBuffer(uint8_t *buf, size_t maxLen) override;
};

/////////////////////////////////////////////////

class cbuf;

/////////////////////////////////////////////////

// KH, to replace Progmem
class AsyncMemResponse: public AsyncAbstractResponse 
{
  private:
    const uint8_t * _content;
    size_t _readLength;
    
  public:
    AsyncMemResponse(int code, const String& contentType, const uint8_t * content, size_t len, 
                     AwsTemplateProcessor callback=nullptr);
    
    /////////////////////////////////////////////////
    
    inline bool _sourceValid() const 
    {
      return true;
    }

    /////////////////////////////////////////////////
    
    virtual size_t _fillBuffer(uint8_t *buf, size_t maxLen) override;
};

/////////////////////////////////////////////////

class AsyncProgmemResponse: public AsyncAbstractResponse 
{
  private:
    const uint8_t * _content;
    size_t _readLength;
    
  public:
    AsyncProgmemResponse(int code, const String& contentType, const uint8_t * content, size_t len, AwsTemplateProcessor callback=nullptr);
    
    /////////////////////////////////////////////////
    
    inline bool _sourceValid() const 
    {
      return true;
    }

    /////////////////////////////////////////////////
    
    virtual size_t _fillBuffer(uint8_t *buf, size_t maxLen) override;
};

/////////////////////////////////////////////////

class AsyncResponseStream: public AsyncAbstractResponse, public Print
{
  private:
    cbuf *_content;

  public:
    AsyncResponseStream(const String& contentType, size_t bufferSize);
    ~AsyncResponseStream();

    /////////////////////////////////////////////////
    
    inline bool _sourceValid() const 
    {
      return (_state < RESPONSE_END);
    }

    /////////////////////////////////////////////////

    virtual size_t _fillBuffer(uint8_t *buf, size_t maxLen) override;
    size_t write(const uint8_t *data, size_t len);
    size_t write(uint8_t data);
    using Print::write;
};

/////////////////////////////////////////////////

#endif /* ASYNCWEBSERVERRESPONSEIMPL_STM32_H_ */
