# AsyncWebServer_STM32

[![arduino-library-badge](https://www.ardu-badge.com/badge/AsyncWebServer_STM32.svg?)](https://www.ardu-badge.com/AsyncWebServer_STM32)
[![GitHub release](https://img.shields.io/github/release/khoih-prog/AsyncWebServer_STM32.svg)](https://github.com/khoih-prog/AsyncWebServer_STM32/releases)
[![GitHub](https://img.shields.io/github/license/mashape/apistatus.svg)](https://github.com/khoih-prog/AsyncWebServer_STM32/blob/master/LICENSE)
[![contributions welcome](https://img.shields.io/badge/contributions-welcome-brightgreen.svg?style=flat)](#Contributing)
[![GitHub issues](https://img.shields.io/github/issues/khoih-prog/AsyncWebServer_STM32.svg)](http://github.com/khoih-prog/AsyncWebServer_STM32/issues)

---
---

## Table of contents

* [Table of contents](#table-of-contents)
* [Changelog](#changelog)
  * [Releases v1.4.0](#releases-v140)
  * [Releases v1.3.1](#releases-v131)
  * [Releases v1.3.0](#releases-v130)
  * [Releases v1.2.6](#releases-v126)
  * [Releases v1.2.5](#releases-v125)
  * [Releases v1.2.4](#releases-v124)
  * [Releases v1.2.3](#releases-v123)

---
---

## Changelog

### Releases v1.4.0

1. Fix base64 encoding of websocket client key and add WebServer progmem support. Check PR [Fix base64 encoding of websocket client key and progmem support for webserver #7](https://github.com/khoih-prog/AsyncWebServer_STM32/pull/7)


### Releases v1.3.1

1. Update `platform.ini` and `library.json` to use original `khoih-prog` instead of `khoih.prog` after PIO fix
2. Update `Packages' Patches`

### Releases v1.3.0

1. Add support to **LAN8720** Ethernet for many **STM32F4** (F407xx, NUCLEO_F429ZI) and **STM32F7** (DISCO_F746NG, NUCLEO_F746ZG, NUCLEO_F756ZG) boards.
2. Add LAN8720 examples
3. Add Packages' Patches for STM32 to use LAN8720 with STM32Ethernet and LwIP libraries
4. Reduce compiled code size.

### Releases v1.2.6

1. Fix dependency on unpublished [**STM32AsyncTCP Library**](https://github.com/philbowles/STM32AsyncTCP). Check [Compilation broken due to error in STM32AsyncTCP dependency](https://github.com/khoih-prog/AsyncWebServer_STM32/issues/4) and [how to run one of the examples?](https://github.com/khoih-prog/AsyncWebServer_STM32/issues/2).

### Releases v1.2.5

1. Clean-up all compiler warnings possible.
2. Update Table of Contents
3. Add examples
4. Add Version String 

### Releases v1.2.4

1. Add back MD5/SHA1 authentication feature.
2. Add example, update README.md, clean up.

#### Releases v1.2.3

1. Initial coding to port [ESPAsyncWebServer](https://github.com/me-no-dev/ESPAsyncWebServer) to STM32 boards using builtin LAN8742A Ethernet. More supports will be added gradually later, such as AsyncUDP, other Ethernet / WiFi shields.
2. Add more examples.
3. Add debugging features.
4. Bump up to v1.2.3 to sync with [ESPAsyncWebServer v1.2.3](https://github.com/me-no-dev/ESPAsyncWebServer).




