/****************************************************************************************************************************
  AsyncWebServer_STM32.cpp - Dead simple AsyncWebServer for STM32 LAN8720 or built-in LAN8742A Ethernet

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

AsyncWebServer::AsyncWebServer(uint16_t port)
  : _server(port), _rewrites(LinkedList<AsyncWebRewrite * >([](AsyncWebRewrite * r)
{
  delete r;
}))
, _handlers(LinkedList<AsyncWebHandler*>([](AsyncWebHandler* h)
{
  delete h;
}))
{
  _catchAllHandler = new AsyncCallbackWebHandler();

  if (_catchAllHandler == NULL)
    return;

  _server.onClient([](void *s, AsyncClient * c)
  {
    if (c == NULL)
      return;

    // KH set no RxTimeout for slower Firefox / network
    //c->setRxTimeout(3);
    c->setRxTimeout(0);
    //////

    AsyncWebServerRequest *r = new AsyncWebServerRequest((AsyncWebServer*)s, c);

    if (r == NULL)
    {
      c->close(true);
      c->free();
      delete c;
    }
  }, this);
}

/////////////////////////////////////////////////

AsyncWebServer::~AsyncWebServer()
{
  reset();
  end();

  if (_catchAllHandler)
    delete _catchAllHandler;
}

/////////////////////////////////////////////////

AsyncWebRewrite& AsyncWebServer::addRewrite(AsyncWebRewrite* rewrite)
{
  _rewrites.add(rewrite);

  return *rewrite;
}

/////////////////////////////////////////////////

bool AsyncWebServer::removeRewrite(AsyncWebRewrite *rewrite)
{
  return _rewrites.remove(rewrite);
}

/////////////////////////////////////////////////

AsyncWebRewrite& AsyncWebServer::rewrite(const char* from, const char* to)
{
  return addRewrite(new AsyncWebRewrite(from, to));
}

/////////////////////////////////////////////////

AsyncWebHandler& AsyncWebServer::addHandler(AsyncWebHandler* handler)
{
  _handlers.add(handler);
  return *handler;
}

/////////////////////////////////////////////////

bool AsyncWebServer::removeHandler(AsyncWebHandler *handler)
{
  return _handlers.remove(handler);
}

/////////////////////////////////////////////////

void AsyncWebServer::begin()
{
  _server.setNoDelay(true);
  _server.begin();
}

/////////////////////////////////////////////////

void AsyncWebServer::end()
{
  _server.end();
}

/////////////////////////////////////////////////

#if ASYNC_TCP_SSL_ENABLED
void AsyncWebServer::onSslFileRequest(AcSSlFileHandler cb, void* arg)
{
  _server.onSslFileRequest(cb, arg);
}

/////////////////////////////////////////////////

void AsyncWebServer::beginSecure(const char *cert, const char *key, const char *password)
{
  _server.beginSecure(cert, key, password);
}
#endif

/////////////////////////////////////////////////

void AsyncWebServer::_handleDisconnect(AsyncWebServerRequest *request)
{
  delete request;
}

/////////////////////////////////////////////////

void AsyncWebServer::_rewriteRequest(AsyncWebServerRequest *request)
{
  for (const auto& r : _rewrites)
  {
    if (r->match(request))
    {
      request->_url = r->toUrl();
      request->_addGetParams(r->params());
    }
  }
}

/////////////////////////////////////////////////

void AsyncWebServer::_attachHandler(AsyncWebServerRequest *request)
{
  for (const auto& h : _handlers)
  {
    if (h->filter(request) && h->canHandle(request))
    {
      request->setHandler(h);
      return;
    }
  }

  request->addInterestingHeader("ANY");
  request->setHandler(_catchAllHandler);
}

/////////////////////////////////////////////////

AsyncCallbackWebHandler& AsyncWebServer::on(const char* uri, WebRequestMethodComposite method,
                                            ArRequestHandlerFunction onRequest,
                                            ArUploadHandlerFunction onUpload, ArBodyHandlerFunction onBody)
{
  AsyncCallbackWebHandler* handler = new AsyncCallbackWebHandler();

  handler->setUri(uri);
  handler->setMethod(method);
  handler->onRequest(onRequest);
  handler->onUpload(onUpload);
  handler->onBody(onBody);
  addHandler(handler);

  return *handler;
}

/////////////////////////////////////////////////

AsyncCallbackWebHandler& AsyncWebServer::on(const char* uri, WebRequestMethodComposite method,
                                            ArRequestHandlerFunction onRequest,
                                            ArUploadHandlerFunction onUpload)
{
  AsyncCallbackWebHandler* handler = new AsyncCallbackWebHandler();
  handler->setUri(uri);
  handler->setMethod(method);
  handler->onRequest(onRequest);
  handler->onUpload(onUpload);
  addHandler(handler);

  return *handler;
}

/////////////////////////////////////////////////

AsyncCallbackWebHandler& AsyncWebServer::on(const char* uri, WebRequestMethodComposite method,
                                            ArRequestHandlerFunction onRequest)
{
  AsyncCallbackWebHandler* handler = new AsyncCallbackWebHandler();
  handler->setUri(uri);
  handler->setMethod(method);
  handler->onRequest(onRequest);
  addHandler(handler);

  return *handler;
}

/////////////////////////////////////////////////

AsyncCallbackWebHandler& AsyncWebServer::on(const char* uri, ArRequestHandlerFunction onRequest)
{
  AsyncCallbackWebHandler* handler = new AsyncCallbackWebHandler();
  handler->setUri(uri);
  handler->onRequest(onRequest);
  addHandler(handler);

  return *handler;
}

/////////////////////////////////////////////////

void AsyncWebServer::onNotFound(ArRequestHandlerFunction fn)
{
  _catchAllHandler->onRequest(fn);
}

/////////////////////////////////////////////////

void AsyncWebServer::onRequestBody(ArBodyHandlerFunction fn)
{
  _catchAllHandler->onBody(fn);
}

/////////////////////////////////////////////////

void AsyncWebServer::reset()
{
  _rewrites.free();
  _handlers.free();

  if (_catchAllHandler != NULL)
  {
    _catchAllHandler->onRequest(NULL);
    _catchAllHandler->onUpload(NULL);
    _catchAllHandler->onBody(NULL);
  }
}

/////////////////////////////////////////////////
