/****************************************************************************************************************************
  AsyncEventSource_STM32.h - Dead simple AsyncWebServer for STM32 LAN8720 or built-in LAN8742A Ethernet

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

#ifndef ASYNCEVENTSOURCE_STM32_H_
#define ASYNCEVENTSOURCE_STM32_H_

#include <Arduino.h>

#include <STM32AsyncTCP.h>
#include <AsyncWebServer_STM32.h>
#include "AsyncWebSynchronization_STM32.h"

// STM32
#include <Crypto/Hash.h>

/////////////////////////////////////////////////////////

#define SSE_MAX_QUEUED_MESSAGES 32
//#define SSE_MAX_QUEUED_MESSAGES 8

#define DEFAULT_MAX_SSE_CLIENTS 8
//#define DEFAULT_MAX_SSE_CLIENTS 4

/////////////////////////////////////////////////////////

class AsyncEventSource;
class AsyncEventSourceResponse;
class AsyncEventSourceClient;
typedef std::function<void(AsyncEventSourceClient *client)> ArEventHandlerFunction;

/////////////////////////////////////////////////////////

class AsyncEventSourceMessage
{
  private:
    uint8_t * _data;
    size_t _len;
    size_t _sent;
    //size_t _ack;
    size_t _acked;

  public:
    AsyncEventSourceMessage(const char * data, size_t len);
    ~AsyncEventSourceMessage();
    size_t ack(size_t len, uint32_t time __attribute__((unused)));
    size_t send(AsyncClient *client);

    /////////////////////////////////////////////////

    inline bool finished()
    {
      return _acked == _len;
    }

    /////////////////////////////////////////////////

    inline bool sent()
    {
      return _sent == _len;
    }
};

/////////////////////////////////////////////////
/////////////////////////////////////////////////

class AsyncEventSourceClient
{
  private:
    AsyncClient *_client;
    AsyncEventSource *_server;
    uint32_t _lastId;
    LinkedList<AsyncEventSourceMessage *> _messageQueue;
    void _queueMessage(AsyncEventSourceMessage *dataMessage);
    void _runQueue();

  public:

    AsyncEventSourceClient(AsyncWebServerRequest *request, AsyncEventSource *server);
    ~AsyncEventSourceClient();

    /////////////////////////////////////////////////

    inline AsyncClient* client()
    {
      return _client;
    }

    /////////////////////////////////////////////////

    void close();
    void write(const char * message, size_t len);
    void send(const char *message, const char *event = NULL, uint32_t id = 0, uint32_t reconnect = 0);

    /////////////////////////////////////////////////

    inline bool connected() const
    {
      return (_client != NULL) && _client->connected();
    }

    /////////////////////////////////////////////////

    inline uint32_t lastId() const
    {
      return _lastId;
    }

    /////////////////////////////////////////////////

    inline size_t  packetsWaiting() const
    {
      return _messageQueue.length();
    }

    /////////////////////////////////////////////////

    //system callbacks (do not call)
    void _onAck(size_t len, uint32_t time);
    void _onPoll();
    void _onTimeout(uint32_t time);
    void _onDisconnect();
};

/////////////////////////////////////////////////
/////////////////////////////////////////////////

class AsyncEventSource: public AsyncWebHandler
{
  private:
    String _url;
    LinkedList<AsyncEventSourceClient *> _clients;
    ArEventHandlerFunction _connectcb;

  public:
    AsyncEventSource(const String& url);
    ~AsyncEventSource();

    /////////////////////////////////////////////////

    inline const char * url() const
    {
      return _url.c_str();
    }

    /////////////////////////////////////////////////

    void close();
    void onConnect(ArEventHandlerFunction cb);
    void send(const char *message, const char *event = NULL, uint32_t id = 0, uint32_t reconnect = 0);
    size_t count() const; //number clinets connected
    size_t  avgPacketsWaiting() const;

    //system callbacks (do not call)
    void _addClient(AsyncEventSourceClient * client);
    void _handleDisconnect(AsyncEventSourceClient * client);
    virtual bool canHandle(AsyncWebServerRequest *request) override final;
    virtual void handleRequest(AsyncWebServerRequest *request) override final;
};

/////////////////////////////////////////////////
/////////////////////////////////////////////////

class AsyncEventSourceResponse: public AsyncWebServerResponse
{
  private:
    String _content;
    AsyncEventSource *_server;

  public:
    AsyncEventSourceResponse(AsyncEventSource *server);
    void _respond(AsyncWebServerRequest *request);
    size_t _ack(AsyncWebServerRequest *request, size_t len, uint32_t time);

    /////////////////////////////////////////////////

    inline bool _sourceValid() const
    {
      return true;
    }
};

#endif /* ASYNCEVENTSOURCE__STM32_H_ */
