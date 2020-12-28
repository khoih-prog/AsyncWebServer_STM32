/****************************************************************************************************************************
  AsyncWebServer_STM32.cpp - Dead simple AsyncWebServer for STM32 built-in LAN8742A Ethernet
  
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

    c->setRxTimeout(3);
    AsyncWebServerRequest *r = new AsyncWebServerRequest((AsyncWebServer*)s, c);

    if (r == NULL)
    {
      c->close(true);
      c->free();
      delete c;
    }
  }, this);
}

AsyncWebServer::~AsyncWebServer()
{
  reset();
  end();

  if (_catchAllHandler)
    delete _catchAllHandler;
}

AsyncWebRewrite& AsyncWebServer::addRewrite(AsyncWebRewrite* rewrite)
{
  _rewrites.add(rewrite);

  return *rewrite;
}

bool AsyncWebServer::removeRewrite(AsyncWebRewrite *rewrite)
{
  return _rewrites.remove(rewrite);
}

AsyncWebRewrite& AsyncWebServer::rewrite(const char* from, const char* to)
{
  return addRewrite(new AsyncWebRewrite(from, to));
}

AsyncWebHandler& AsyncWebServer::addHandler(AsyncWebHandler* handler)
{
  _handlers.add(handler);
  return *handler;
}

bool AsyncWebServer::removeHandler(AsyncWebHandler *handler)
{
  return _handlers.remove(handler);
}

void AsyncWebServer::begin()
{
  _server.setNoDelay(true);
  _server.begin();
}

void AsyncWebServer::end()
{
  _server.end();
}

#if ASYNC_TCP_SSL_ENABLED
void AsyncWebServer::onSslFileRequest(AcSSlFileHandler cb, void* arg)
{
  _server.onSslFileRequest(cb, arg);
}

void AsyncWebServer::beginSecure(const char *cert, const char *key, const char *password)
{
  _server.beginSecure(cert, key, password);
}
#endif

void AsyncWebServer::_handleDisconnect(AsyncWebServerRequest *request)
{
  delete request;
}

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


AsyncCallbackWebHandler& AsyncWebServer::on(const char* uri, WebRequestMethodComposite method, ArRequestHandlerFunction onRequest, 
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

AsyncCallbackWebHandler& AsyncWebServer::on(const char* uri, WebRequestMethodComposite method, ArRequestHandlerFunction onRequest, ArUploadHandlerFunction onUpload) 
{
  AsyncCallbackWebHandler* handler = new AsyncCallbackWebHandler();
  handler->setUri(uri);
  handler->setMethod(method);
  handler->onRequest(onRequest);
  handler->onUpload(onUpload);
  addHandler(handler);
  
  return *handler;
}

AsyncCallbackWebHandler& AsyncWebServer::on(const char* uri, WebRequestMethodComposite method, ArRequestHandlerFunction onRequest) 
{
  AsyncCallbackWebHandler* handler = new AsyncCallbackWebHandler();
  handler->setUri(uri);
  handler->setMethod(method);
  handler->onRequest(onRequest);
  addHandler(handler);
  
  return *handler;
}

AsyncCallbackWebHandler& AsyncWebServer::on(const char* uri, ArRequestHandlerFunction onRequest) 
{
  AsyncCallbackWebHandler* handler = new AsyncCallbackWebHandler();
  handler->setUri(uri);
  handler->onRequest(onRequest);
  addHandler(handler);
  
  return *handler;
}

void AsyncWebServer::onNotFound(ArRequestHandlerFunction fn) 
{
  _catchAllHandler->onRequest(fn);
}

void AsyncWebServer::onRequestBody(ArBodyHandlerFunction fn) 
{
  _catchAllHandler->onBody(fn);
}

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

