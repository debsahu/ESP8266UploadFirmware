# ESP8266UploadFirmware

A simple Arduino IDE compatible code for updating firmware

[![Build Status](https://travis-ci.com/debsahu/ESP8266UploadFirmware.svg?branch=master)](https://travis-ci.com/debsahu/ESP8266UploadFirmware) [![License: MIT](https://img.shields.io/github/license/debsahu/ESP8266UploadFirmware.svg)](https://opensource.org/licenses/MIT) [![version](https://img.shields.io/github/release/debsahu/ESP8266UploadFirmware.svg)](https://github.com/debsahu/ESP8266UploadFirmware/releases/tag/1.0.0) [![LastCommit](https://img.shields.io/github/last-commit/debsahu/ESP8266UploadFirmware.svg?style=social)](https://github.com/debsahu/ESP8266UploadFirmware/commits/master)

----------------
## Youtube
[![ESP8266UploadFirmware](https://img.youtube.com/vi/xxxxxxxxxx/0.jpg)](https://www.youtube.com/watch?v=xxxxxxxxxxxx)

## Software instructions

### Arduino IDE
* [Download](https://github.com/debsahu/ESP8266UploadFirmware/archive/master.zip) or `git clone  https://github.com/debsahu/ESP8266UploadFirmware` this repo
* Install [latest ESP8266 Arduino Core](https://github.com/esp8266/Arduino)
* Compile for your board and upload to ESP8266

### On PC/Mac
* Scan for WiFi for **`UpdateESP8266`** and connect to it
* If computer's IP is not set automatically, manually set 
```
IP: 192.168.4.100
SubNet Prefix: 24
Gateway: 192.168.4.1 
SubNet: 255.255.255.0
```
* Open web-browser and go to: `http://192.168.4.1/update`

### On Mobile
* Scan for WiFi for **`UpdateESP8266`** and connect to it
* Most mobile phones should direct you to upload page or manually go to: `http://192.168.4.1/update`

## Firmware Update
- Lot of projects provide **firmware.bin** file to upload to ESP8266.
- Drag and drop or select precompiled .bin files to update ESP8266's firmware
-  `http://192.168.4.1/update` will yield:

![Update HTML](https://github.com/debsahu/ESP8266UploadFirmware/blob/master/doc/update.png)

