/****************************************************************************************************************************
  AsyncWebAuthentication_STM32.cpp - Dead simple AsyncWebServer for STM32 LAN8720 or built-in LAN8742A Ethernet

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

#include "AsyncWebAuthentication_STM32.h"
#include <libb64/cencode.h>

// For STM32
#include "Crypto/md5.h"
#include "Crypto/bearssl_hash.h"
#include "Crypto/Hash.h"

/////////////////////////////////////////////////

// Basic Auth hash = base64("username:password")

bool checkBasicAuthentication(const char * hash, const char * username, const char * password)
{
  if (username == NULL || password == NULL || hash == NULL)
  {
    LOGDEBUG("checkBasicAuthentication: Fail: NULL username/password/hash");
    return false;
  }

  size_t toencodeLen = strlen(username) + strlen(password) + 1;
  size_t encodedLen = base64_encode_expected_len(toencodeLen);

  if (strlen(hash) != encodedLen)
  {
    LOGDEBUG3("checkBasicAuthentication: Fail: strlen(hash) = ", strlen(hash), " != encodedLen = ", encodedLen );

    return false;
  }

  char *toencode = new char[toencodeLen + 1];

  if (toencode == NULL)
  {
    LOGDEBUG("checkBasicAuthentication: NULL toencode");

    return false;
  }

  char *encoded = new char[base64_encode_expected_len(toencodeLen) + 1];

  if (encoded == NULL)
  {
    LOGDEBUG("checkBasicAuthentication: NULL encoded");

    delete[] toencode;

    return false;
  }

  sprintf(toencode, "%s:%s", username, password);

  if (base64_encode_chars(toencode, toencodeLen, encoded) > 0 && memcmp(hash, encoded, encodedLen) == 0)
  {
    LOGDEBUG("checkBasicAuthentication: OK");

    delete[] toencode;
    delete[] encoded;

    return true;
  }

  LOGDEBUG("checkBasicAuthentication: Failed");

  delete[] toencode;
  delete[] encoded;

  return false;
}

/////////////////////////////////////////////////

static bool getMD5(uint8_t * data, uint16_t len, char * output)
{
  //33 bytes or more

  // For Teensy41
  md5_context _ctx;

  uint8_t i;
  uint8_t * _buf = (uint8_t*) malloc(16);

  if (_buf == NULL)
  {
    LOGDEBUG("getMD5: Can malloc _buf");

    return false;
  }

  memset(_buf, 0x00, 16);

  // For Teensy41
  md5_starts(&_ctx);
  md5_update(&_ctx, data, len);
  md5_finish(&_ctx, _buf);

  for (i = 0; i < 16; i++)
  {
    sprintf(output + (i * 2), "%02x", _buf[i]);
  }

  free(_buf);

  LOGDEBUG("getMD5: Success");

  return true;
}

/////////////////////////////////////////////////

static String genRandomMD5()
{
  // For Teensy41
  uint32_t r = rand();

  char * out = (char*) malloc(33);

  if (out == NULL || !getMD5((uint8_t*)(&r), 4, out))
    return "";

  String res = String(out);
  free(out);

  LOGDEBUG1("genRandomMD5: res = ", res);

  return res;
}

/////////////////////////////////////////////////

static String stringMD5(const String& in)
{
  char * out = (char*) malloc(33);

  if (out == NULL || !getMD5((uint8_t*)(in.c_str()), in.length(), out))
    return "";

  String res = String(out);
  free(out);

  LOGDEBUG1("stringMD5: res = ", res);

  return res;
}

/////////////////////////////////////////////////

String generateDigestHash(const char * username, const char * password, const char * realm)
{
  if (username == NULL || password == NULL || realm == NULL)
  {
    return "";
  }

  char * out = (char*) malloc(33);
  String res = String(username);

  res.concat(":");
  res.concat(realm);
  res.concat(":");

  String in = res;

  in.concat(password);

  if (out == NULL || !getMD5((uint8_t*)(in.c_str()), in.length(), out))
    return "";

  res.concat(out);
  free(out);

  LOGDEBUG1("generateDigestHash: res = ", res);

  return res;
}

/////////////////////////////////////////////////

String requestDigestAuthentication(const char * realm)
{
  String header = "realm=\"";

  if (realm == NULL)
    header.concat("asyncesp");
  else
    header.concat(realm);

  header.concat( "\", qop=\"auth\", nonce=\"");
  header.concat(genRandomMD5());
  header.concat("\", opaque=\"");
  header.concat(genRandomMD5());
  header.concat("\"");

  LOGDEBUG1("requestDigestAuthentication: header = ", header);

  return header;
}

/////////////////////////////////////////////////

bool checkDigestAuthentication(const char * header, const char * method, const char * username, const char * password,
                               const char * realm, bool passwordIsHash, const char * nonce, const char * opaque, const char * uri)
{
  if (username == NULL || password == NULL || header == NULL || method == NULL)
  {
    LOGDEBUG("AUTH FAIL: missing required fields");

    return false;
  }

  String myHeader = String(header);
  int nextBreak = myHeader.indexOf(",");

  if (nextBreak < 0)
  {
    LOGDEBUG("AUTH FAIL: no variables");

    return false;
  }

  String myUsername = String();
  String myRealm    = String();
  String myNonce    = String();
  String myUri      = String();
  String myResponse = String();
  String myQop      = String();
  String myNc       = String();
  String myCnonce   = String();

  myHeader += ", ";

  do
  {
    String avLine = myHeader.substring(0, nextBreak);

    avLine.trim();
    myHeader = myHeader.substring(nextBreak + 1);
    nextBreak = myHeader.indexOf(",");

    int eqSign = avLine.indexOf("=");

    if (eqSign < 0)
    {
      LOGDEBUG("AUTH FAIL: no = sign");

      return false;
    }

    String varName = avLine.substring(0, eqSign);
    avLine = avLine.substring(eqSign + 1);

    if (avLine.startsWith("\""))
    {
      avLine = avLine.substring(1, avLine.length() - 1);
    }

    if (varName.equals("username"))
    {
      if (!avLine.equals(username))
      {
        LOGDEBUG("AUTH FAIL: username");

        return false;
      }

      myUsername = avLine;
    }
    else if (varName.equals("realm"))
    {
      if (realm != NULL && !avLine.equals(realm))
      {
        LOGDEBUG("AUTH FAIL: realm");

        return false;
      }

      myRealm = avLine;
    }
    else if (varName.equals("nonce"))
    {
      if (nonce != NULL && !avLine.equals(nonce))
      {
        LOGDEBUG("AUTH FAIL: nonce");

        return false;
      }

      myNonce = avLine;
    }
    else if (varName.equals("opaque"))
    {
      if (opaque != NULL && !avLine.equals(opaque))
      {
        LOGDEBUG("AUTH FAIL: opaque");

        return false;
      }
    }
    else if (varName.equals("uri"))
    {
      if (uri != NULL && !avLine.equals(uri))
      {
        LOGDEBUG("AUTH FAIL: uri");

        return false;
      }

      myUri = avLine;
    }
    else if (varName.equals("response"))
    {
      myResponse = avLine;
    }
    else if (varName.equals("qop"))
    {
      myQop = avLine;
    }
    else if (varName.equals("nc"))
    {
      myNc = avLine;
    }
    else if (varName.equals("cnonce"))
    {
      myCnonce = avLine;
    }
  } while (nextBreak > 0);

  String ha1 = (passwordIsHash) ? String(password) : stringMD5(myUsername + ":" + myRealm + ":" + String(password));
  String ha2 = String(method) + ":" + myUri;
  String response = ha1 + ":" + myNonce + ":" + myNc + ":" + myCnonce + ":" + myQop + ":" + stringMD5(ha2);

  if (myResponse.equals(stringMD5(response)))
  {
    LOGDEBUG("AUTH SUCCESS");

    return true;
  }

  LOGDEBUG("AUTH FAIL: password");

  return false;
}
