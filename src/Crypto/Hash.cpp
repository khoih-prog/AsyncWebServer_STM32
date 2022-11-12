/**
   @file Hash.cpp
   @date 20.05.2015
   @author Markus Sattler

   Copyright (c) 2015 Markus Sattler. All rights reserved.
   This file is part of the esp8266 core for Arduino environment.

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Lesser General Public
   License as published by the Free Software Foundation; either
   version 2.1 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Lesser General Public License for more details.

   You should have received a copy of the GNU Lesser General Public
   License along with this library; if not, write to the Free Software
   Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA

*/

#include <Arduino.h>
#include "bearssl_hash.h"

#include "Hash.h"

/**
   create a sha1 hash from data
   @param data uint8_t
   @param size uint32_t
   @param hash uint8_t[HASH_BUFFER_SIZE]
*/
void sha1(const uint8_t* data, uint32_t size, uint8_t hash[HASH_BUFFER_SIZE])
{
  br_sha1_context ctx;

  LOGDEBUG0("DATA:");

  for (uint16_t i = 0; i < size; i++)
  {
    LOGDEBUG0(data[i]);
  }

  LOGDEBUG0("\nDATA:");

  for (uint16_t i = 0; i < size; i++)
  {
    LOGDEBUG0((char) data[i]);

  }

  LOGDEBUG0("\n");

  br_sha1_init(&ctx);
  br_sha1_update(&ctx, data, size);
  br_sha1_out(&ctx, hash);

  LOGDEBUG0("SHA1:");

  for (uint16_t i = 0; i < HASH_BUFFER_SIZE; i++)
  {
    LOGDEBUG0(hash[i]);
  }

  LOGDEBUG0("\n");
}

void sha1(const char* data, uint32_t size, uint8_t hash[HASH_BUFFER_SIZE])
{
  sha1((const uint8_t *) data, size, hash);
}

void sha1(const String& data, uint8_t hash[HASH_BUFFER_SIZE])
{
  sha1(data.c_str(), data.length(), hash);
}

String sha1(const uint8_t* data, uint32_t size)
{
  uint8_t hash[HASH_BUFFER_SIZE];

  String hashStr((const char*)nullptr);
  hashStr.reserve(HASH_BUFFER_SIZE * 2 + 1);

  sha1(&data[0], size, &hash[0]);

  for (uint16_t i = 0; i < HASH_BUFFER_SIZE; i++)
  {
    char hex[3];
    snprintf(hex, sizeof(hex), "%02x", hash[i]);
    hashStr += hex;
  }

  return hashStr;
}

String sha1(const char* data, uint32_t size)
{
  return sha1((const uint8_t*) data, size);
}

String sha1(const String& data)
{
  return sha1(data.c_str(), data.length());
}

