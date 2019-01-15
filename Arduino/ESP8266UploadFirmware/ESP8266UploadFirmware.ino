#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <Arduino.h>
#include <WiFiClient.h>
#include <WiFiServer.h>
#include <ESP8266WebServer.h>
#include <DNSServer.h>
#include <WiFiUdp.h>
#include <StreamString.h>
#include "version.h"
#define emptyString String()

////////////////////// Change Paramter to your choosing /////////////////////////
/////////////// Do not change any parameters above this line ////////////////////

#define HOSTNAME "UpdateESP8266"    // AP name will be HOSTNAME-XXXXXX, where XXXXXX are CHIP_ID
#define HTTP_USER ""                // Set any username for update authentication, default empty
#define HTTP_PASSWORD ""            // Set any password for update authentication, default empty
IPAddress apIP(192, 168, 4, 1);     // AP IP: http://192.168.4.1/update
bool SERIAL_OUTPUT = true;          // If Serial output is desired

//Enter values for SSID and corresponing password
String WIFI_SSID = ""; //Enter SSID
String WIFI_PASS = ""; //Enter SSID
/////////////// Do not change any parameters below this line ////////////////////
/////////////////////////////////////////////////////////////////////////////////

#define HTTP_PORT 80
#define HTTP_UPDATE_PATH "/update"
const byte DNS_PORT = 53;
bool authenticated = false;
String updaterError;
String username = HTTP_USER;
String password = HTTP_PASSWORD;

/* //uncompressed size: 3879
static const char update_htm[] PROGMEM = R"=====(
<!DOCTYPE html><html><meta http-equiv="Content-Type" content="text/html; charset=utf-8"><meta name="viewport" content="width=device-width,initial-scale=0.5"><head><style>#fileLabelText,#uploadStatus{font-family:Arial,Helvetica,sans-serif}body{margin:0;height:100vh;display:flex;flex-direction:column;justify-content:center;background-color:#95b195}#dropFileForm{margin:auto 100px;text-align:center;border-radius:8px;overflow:hidden;transition:.5s}#dropFileForm #fileLabel{background-color:rgba(0,255,0,.5);display:block;padding:24px;position:relative;cursor:pointer}#dropFileForm #fileInput{display:none}#dropFileForm #fileLabel:after,#dropFileForm #fileLabel:before{position:absolute;content:"";top:0;bottom:0;left:0;right:0;background-color:#fff;z-index:-2;border-radius:8px 8px 0 0}#dropFileForm #fileLabel:before{z-index:-1;background:repeating-linear-gradient(45deg,transparent,transparent 5%,#000 5%,#000 10%);opacity:0;transition:.5s}#dropFileForm.fileHover #fileLabel:before{opacity:.25}#dropFileForm .uploadButton{border:0;outline:0;width:100%;padding:8px;font-size:50px;background-color:#32cd32;color:#fff;cursor:pointer}#dropFileForm.fileHover{box-shadow:0 0 16px #32cd32}#fileLabelText{color:#000;font-size:40px}#uploadStatus{color:#700000;font-size:30px}</style></head><body><form id="dropFileForm" action="/update" method="post" onsubmit="uploadFiles(event)"><input type="file" name="files[]" id="fileInput" onchange="addFiles(event)" accept=".bin"><label for="fileInput" id="fileLabel" ondragover="overrideDefault(event),fileHover()" ondragenter="overrideDefault(event),fileHover()" ondragleave="overrideDefault(event),fileHoverEnd()" ondrop="overrideDefault(event),fileHoverEnd(),addFiles(event)"><svg width="350" height="350" viewBox="0 0 2048 1792" xmlns="http://www.w3.org/2000/svg"><path d="M1344 864q0-14-9-23l-352-352q-9-9-23-9t-23 9l-351 351q-10 12-10 24 0 14 9 23t23 9h224v352q0 13 9.5 22.5t22.5 9.5h192q13 0 22.5-9.5t9.5-22.5v-352h224q13 0 22.5-9.5t9.5-22.5zm640 288q0 159-112.5 271.5t-271.5 112.5h-1088q-185 0-316.5-131.5t-131.5-316.5q0-130 70-240t188-165q-2-30-2-43 0-212 150-362t362-150q156 0 285.5 87t188.5 231q71-62 166-62 106 0 181 75t75 181q0 76-41 138 130 31 213.5 135.5t83.5 238.5z"/></svg><br><span id="fileLabelText">Choose firmware.bin or drag it here</span><br><span id="uploadStatus"></span></label><input type="submit" value="Update Firmware" class="uploadButton"></form><script>function overrideDefault(e){e.preventDefault(),e.stopPropagation()}function fileHover(){dropFileForm.classList.add("fileHover")}function fileHoverEnd(){dropFileForm.classList.remove("fileHover")}function addFiles(e){droppedFiles=e.target.files||e.dataTransfer.files,showFiles(droppedFiles)}function showFiles(e){"bin"!=e[0].name.split(".").pop()?(fileLabelText.innerText="STOP: Only choose .bin file !!!",alert("STOP: Only choose .bin file !!!"),fileInput.value="",dropFileForm.reset()):fileLabelText.innerText=e[0].name}function uploadFiles(e){if(e.preventDefault(),0==fileInput.files.length)fileLabelText.innerText="Select at least 1 file!";else{changeStatus("Uploading...");var t=new FormData;t.append(fileInput.name,droppedFiles[0],droppedFiles[0].name),fetch(dropFileForm.action,{method:dropFileForm.method,body:t}).then(function(e){if(e.ok)return e.text();throw new Error("Network response was not ok.")}).then(function(e){changeStatus(e+", reload in 30s!"),console.log(e),setTimeout(function(){location.reload()},3e4)}).catch(function(e){changeStatus("Error: "+e.message),console.log("Error: ",e.message),setTimeout(function(){location.reload()},5e3)})}}function changeStatus(e){uploadStatus.innerText=e}var dropFileForm=document.getElementById("dropFileForm"),fileLabelText=document.getElementById("fileLabelText"),uploadStatus=document.getElementById("uploadStatus"),fileInput=document.getElementById("fileInput"),droppedFiles</script></body></html>)=====";
*/

//gz size: 1721
#define update_htm_gz_len 1738
static const char update_htm_gz[] PROGMEM = {
  0x1f, 0x8b, 0x08, 0x08, 0xba, 0x80, 0x3d, 0x5c, 0x04, 0x00, 0x69, 0x6e,
  0x64, 0x65, 0x78, 0x2e, 0x6d, 0x69, 0x6e, 0x2e, 0x68, 0x74, 0x6d, 0x00,
  0x95, 0x57, 0x59, 0x13, 0x93, 0x30, 0x10, 0xfe, 0x2b, 0x11, 0xc7, 0x99,
  0x76, 0x04, 0xca, 0x51, 0x6a, 0x0b, 0x56, 0xc7, 0x73, 0x74, 0xc6, 0x6b,
  0x46, 0x7d, 0x70, 0x1c, 0x1f, 0x52, 0x58, 0x4a, 0x94, 0x12, 0x1a, 0x42,
  0x6b, 0xad, 0xfd, 0xef, 0xee, 0x42, 0x0f, 0x50, 0xeb, 0xf1, 0x50, 0x48,
  0xc2, 0xee, 0xb7, 0x9b, 0xdd, 0x6f, 0x37, 0xe9, 0xdd, 0x1b, 0x8f, 0x5f,
  0x3f, 0x7a, 0xf7, 0xe1, 0xcd, 0x13, 0x96, 0xe9, 0x55, 0x7e, 0xef, 0x6e,
  0xfb, 0x5c, 0x81, 0xe6, 0xb8, 0xa0, 0x4b, 0x0b, 0xd6, 0xb5, 0xd8, 0xcc,
  0x8d, 0x47, 0xb2, 0xd0, 0x50, 0x68, 0xeb, 0xdd, 0xae, 0x04, 0x83, 0xc5,
  0xed, 0x6c, 0x6e, 0x68, 0xf8, 0xaa, 0x47, 0xa4, 0x12, 0xb1, 0x38, 0xe3,
  0xaa, 0x02, 0x3d, 0xaf, 0x75, 0x6a, 0x4d, 0x8d, 0x23, 0x44, 0xc1, 0x57,
  0x30, 0x37, 0x36, 0x02, 0xb6, 0xa5, 0x54, 0xba, 0xa3, 0xb8, 0x15, 0x89,
  0xce, 0xe6, 0x09, 0x6c, 0x44, 0x0c, 0x56, 0x33, 0x31, 0x45, 0x21, 0xb4,
  0xe0, 0xb9, 0x55, 0xc5, 0x3c, 0x87, 0xb9, 0x63, 0x07, 0x08, 0x92, 0x01,
  0x4f, 0xee, 0xdd, 0xad, 0xf4, 0x2e, 0x87, 0x7b, 0x37, 0x53, 0x91, 0xc3,
  0x0b, 0xbe, 0x80, 0xfc, 0x1d, 0x5a, 0x35, 0x6f, 0xd6, 0x65, 0x2e, 0x79,
  0xf2, 0x56, 0x73, 0x5d, 0x57, 0xfb, 0x14, 0x81, 0xad, 0x94, 0xaf, 0x44,
  0xbe, 0x0b, 0x1f, 0x28, 0x84, 0x31, 0x9f, 0x41, 0xbe, 0x01, 0x2d, 0x62,
  0x6e, 0x56, 0xbc, 0xa8, 0xac, 0x0a, 0x94, 0x48, 0x0f, 0x0b, 0x99, 0xec,
  0xf6, 0x2b, 0xae, 0x96, 0xa2, 0x08, 0x9d, 0x28, 0x03, 0xb1, 0xcc, 0x74,
  0xe8, 0x3a, 0xce, 0x26, 0x8b, 0x12, 0x51, 0x95, 0x39, 0xdf, 0x85, 0x69,
  0x0e, 0x5f, 0x23, 0x7a, 0x58, 0x89, 0x50, 0x10, 0x6b, 0x21, 0x8b, 0x30,
  0x96, 0x79, 0xbd, 0x2a, 0xa2, 0xcf, 0x75, 0xa5, 0x45, 0xba, 0xb3, 0x8e,
  0xbb, 0x08, 0x63, 0xc0, 0xb7, 0x8a, 0x16, 0x3c, 0xfe, 0xb2, 0x54, 0xb2,
  0x2e, 0x12, 0xfc, 0x92, 0x4b, 0x15, 0xde, 0x9c, 0x05, 0x0b, 0x77, 0x16,
  0x1c, 0x6e, 0x26, 0x4a, 0x96, 0x4f, 0xd1, 0xeb, 0xa7, 0x52, 0xad, 0x4e,
  0x66, 0x79, 0xad, 0x25, 0x43, 0x93, 0xe5, 0xd7, 0x88, 0xa2, 0x67, 0xf1,
  0x5c, 0x2c, 0x8b, 0x33, 0x94, 0x54, 0x09, 0x28, 0x4b, 0xf1, 0x44, 0xd4,
  0x55, 0x38, 0x45, 0x19, 0xb9, 0x01, 0x95, 0xe6, 0x72, 0x1b, 0x66, 0x22,
  0x49, 0xa0, 0x88, 0xb4, 0xc2, 0xdd, 0x88, 0xc6, 0x2b, 0x3b, 0xa8, 0xfa,
  0x26, 0xd8, 0x25, 0x44, 0xfb, 0x5f, 0x9c, 0x52, 0xcb, 0x05, 0x1f, 0x38,
  0xa6, 0x17, 0x04, 0xa6, 0x63, 0xda, 0xc1, 0xf0, 0xbc, 0xe1, 0x45, 0x2e,
  0xe3, 0x2f, 0x51, 0xc9, 0x93, 0x44, 0x14, 0xcb, 0xd0, 0x1b, 0xa3, 0xd1,
  0x52, 0x1e, 0x4d, 0x28, 0xc8, 0xb9, 0x16, 0x1b, 0x88, 0xe2, 0x5a, 0x55,
  0x08, 0x52, 0x4a, 0x41, 0x7e, 0xfe, 0xce, 0xec, 0xf3, 0xa2, 0xac, 0xf5,
  0xfe, 0x04, 0x5a, 0xc8, 0x02, 0xae, 0x3a, 0x17, 0xf2, 0x14, 0x41, 0xcc,
  0xab, 0x9f, 0x17, 0x90, 0x4a, 0x05, 0xfb, 0xb3, 0x17, 0x7c, 0x51, 0x61,
  0x02, 0x34, 0x44, 0xa7, 0xc0, 0x1b, 0x46, 0xa4, 0x65, 0x19, 0x3a, 0x18,
  0x2f, 0xad, 0xe5, 0x0a, 0x07, 0x39, 0xa4, 0x1a, 0x5f, 0xaa, 0x49, 0xa8,
  0xf3, 0x9b, 0x94, 0xa4, 0x69, 0x1a, 0x7d, 0xb3, 0x44, 0x91, 0xc0, 0xd7,
  0xd0, 0xf2, 0x7e, 0x0d, 0x34, 0xa3, 0x9f, 0xc3, 0x9c, 0xc3, 0xdf, 0xbc,
  0x3a, 0x83, 0xb8, 0x1d, 0x2b, 0x18, 0xa8, 0x12, 0xb8, 0xc6, 0x00, 0x5a,
  0xb9, 0x28, 0x80, 0x2b, 0x6b, 0x49, 0xd0, 0xe8, 0xeb, 0x60, 0x1c, 0x24,
  0xb0, 0x34, 0x9b, 0xb4, 0x95, 0x5c, 0xe1, 0x4a, 0x77, 0xcc, 0x82, 0x5b,
  0xe6, 0x4d, 0xc7, 0x71, 0xce, 0x6f, 0xd7, 0xb9, 0x35, 0x8c, 0x64, 0xc9,
  0x63, 0xa1, 0x77, 0xa1, 0xf3, 0xc7, 0x6c, 0xdb, 0xe4, 0xda, 0x33, 0xe2,
  0xc7, 0x6f, 0xbc, 0x3c, 0x41, 0xd8, 0x5e, 0xf0, 0xd3, 0x8e, 0xec, 0xb6,
  0x6e, 0x1e, 0xd6, 0x18, 0xb9, 0x62, 0xdf, 0xc6, 0x01, 0x2d, 0xc9, 0x5a,
  0x93, 0xe7, 0x38, 0x6a, 0xaa, 0x91, 0x8a, 0xe2, 0xd6, 0x89, 0x14, 0x0d,
  0x11, 0x9b, 0x0a, 0xab, 0xc4, 0x37, 0x08, 0x03, 0xe2, 0xee, 0xaf, 0x11,
  0xf6, 0xbd, 0x38, 0xf1, 0xbd, 0xe8, 0x12, 0xef, 0x3f, 0x92, 0xe6, 0xe2,
  0x3d, 0xfa, 0xf0, 0xd5, 0xaa, 0x32, 0x9e, 0x20, 0xc9, 0x1d, 0x86, 0x21,
  0x98, 0x60, 0x26, 0x8e, 0x68, 0x87, 0x7e, 0xd1, 0xef, 0x5b, 0x70, 0x8a,
  0x54, 0xc7, 0x9f, 0x31, 0xfa, 0x73, 0xe8, 0xb5, 0x83, 0x93, 0xdc, 0x1d,
  0x14, 0xec, 0x89, 0xfa, 0x24, 0x7a, 0x77, 0xd4, 0x36, 0x94, 0xbb, 0xa3,
  0xb6, 0xbd, 0x50, 0x53, 0xb8, 0x77, 0x37, 0xa5, 0xe0, 0x88, 0x64, 0x6e,
  0x74, 0xbd, 0x34, 0x18, 0x6f, 0x3a, 0xc0, 0xdc, 0x18, 0xd5, 0x65, 0xc2,
  0x35, 0xf6, 0x3e, 0xec, 0x6b, 0x99, 0x44, 0x31, 0x64, 0x27, 0x36, 0x34,
  0x59, 0x54, 0xf5, 0x62, 0x25, 0xb0, 0xa3, 0xb5, 0xf6, 0x49, 0xb1, 0x1a,
  0xc0, 0x06, 0x73, 0x3b, 0xc4, 0xfe, 0x25, 0xa8, 0x28, 0x98, 0xde, 0x95,
  0xd8, 0x05, 0x69, 0x27, 0x06, 0x75, 0xc4, 0xe3, 0xb8, 0xfa, 0xf8, 0xc9,
  0x68, 0x0c, 0x9e, 0xaa, 0xa7, 0x81, 0xc3, 0x36, 0x5a, 0x2c, 0x51, 0x82,
  0x27, 0x3d, 0x2c, 0x72, 0x24, 0x86, 0x12, 0xed, 0xd8, 0x0b, 0x51, 0x20,
  0x72, 0x4e, 0x31, 0x61, 0xe8, 0x74, 0x47, 0xbf, 0x85, 0x3b, 0x47, 0x8c,
  0xe0, 0x12, 0xc5, 0x97, 0x18, 0x64, 0x94, 0xa2, 0xa7, 0x12, 0x09, 0x3c,
  0x86, 0x94, 0xd7, 0xb9, 0x3e, 0xe2, 0x9a, 0xe7, 0x34, 0x0c, 0x86, 0x27,
  0x79, 0x5c, 0xff, 0x3f, 0x85, 0x1c, 0xf8, 0x06, 0xfe, 0xae, 0xf0, 0xa4,
  0x48, 0x4e, 0x3a, 0xb2, 0xfc, 0x47, 0x71, 0xf3, 0xe7, 0x38, 0xe0, 0x69,
  0xb0, 0x59, 0xb2, 0xf6, 0xf8, 0x30, 0xfc, 0xc0, 0x31, 0x58, 0xdb, 0xc5,
  0x8f, 0x13, 0x3a, 0x6a, 0x1e, 0xca, 0xaf, 0x73, 0x83, 0x98, 0xe4, 0x39,
  0xe3, 0x29, 0x73, 0xef, 0xcc, 0x3c, 0x83, 0x7d, 0x5d, 0xe5, 0x45, 0x35,
  0x37, 0xe8, 0x48, 0x0b, 0x47, 0xa3, 0xed, 0x76, 0x6b, 0x6f, 0x7d, 0x5b,
  0xaa, 0xe5, 0xc8, 0x43, 0x82, 0x8c, 0x10, 0x12, 0x81, 0x4b, 0xae, 0x33,
  0x86, 0x01, 0x7c, 0xe9, 0xfa, 0xe3, 0x31, 0x9b, 0x4e, 0xc6, 0x6b, 0xc7,
  0x72, 0xc7, 0xd6, 0xcc, 0xf2, 0xfc, 0xdc, 0xf2, 0x03, 0x8f, 0x7e, 0x6b,
  0x9c, 0xd2, 0x82, 0x35, 0xd3, 0xf8, 0x64, 0x33, 0xfa, 0xe0, 0x32, 0xfc,
  0xad, 0x2d, 0x17, 0xa9, 0xeb, 0xd1, 0xd3, 0x1b, 0x13, 0x8b, 0xc7, 0x6c,
  0xc6, 0x3c, 0x5f, 0x93, 0x50, 0xe6, 0x79, 0xe3, 0x0d, 0x29, 0xe3, 0x32,
  0x4e, 0xed, 0x80, 0x79, 0x9e, 0x1d, 0x68, 0x7a, 0xd0, 0x2c, 0x73, 0x67,
  0xde, 0x1a, 0x3f, 0x38, 0xcd, 0xb2, 0x35, 0xc3, 0x4f, 0xf8, 0xb3, 0x68,
  0xb2, 0x21, 0x9b, 0xa4, 0x7e, 0xe5, 0xfb, 0xb7, 0xd5, 0x64, 0x8c, 0xcb,
  0xd3, 0x29, 0x41, 0x07, 0x33, 0xcb, 0x75, 0x3d, 0x42, 0xbf, 0xe3, 0xa2,
  0x8c, 0xd5, 0xbc, 0x58, 0xb3, 0x94, 0xa1, 0x5f, 0x28, 0x64, 0xb9, 0xd3,
  0x80, 0x39, 0x96, 0xef, 0x4e, 0x50, 0xdf, 0xf5, 0x49, 0xaa, 0x7d, 0xb5,
  0x4b, 0xb4, 0x5f, 0xdf, 0x61, 0x77, 0x1c, 0xcb, 0x1b, 0x3b, 0xda, 0x9d,
  0x4e, 0x2d, 0x77, 0x12, 0xac, 0x2d, 0xdc, 0xb8, 0x83, 0x8f, 0xb1, 0xcf,
  0xf0, 0xe5, 0x7a, 0x68, 0x08, 0x21, 0x26, 0x9e, 0xc6, 0x9f, 0x85, 0xe3,
  0xb5, 0x1b, 0x4c, 0x18, 0x39, 0x11, 0xa0, 0xb5, 0xe9, 0x1d, 0xd2, 0x23,
  0x1f, 0x7c, 0x77, 0x7d, 0xc7, 0xb5, 0x26, 0x28, 0x3e, 0x99, 0x34, 0x2f,
  0x87, 0xa4, 0xdc, 0xa9, 0xcb, 0xee, 0x04, 0xfa, 0x4e, 0x40, 0xa3, 0x35,
  0xda, 0x9a, 0x58, 0x63, 0x17, 0xa3, 0x32, 0x65, 0x64, 0xd9, 0x77, 0x99,
  0xe7, 0xfa, 0xe4, 0xb4, 0x8f, 0x60, 0x7a, 0xea, 0x37, 0x40, 0x08, 0xf7,
  0xcd, 0x18, 0xdd, 0xbb, 0x4b, 0x69, 0xc2, 0x6a, 0x55, 0x48, 0x81, 0x92,
  0x17, 0x27, 0xae, 0x5f, 0xba, 0x83, 0x71, 0xef, 0x51, 0x26, 0x65, 0x05,
  0x2c, 0x15, 0x6a, 0xb5, 0xc5, 0xfe, 0x4a, 0xa5, 0xc2, 0xa4, 0x62, 0xc4,
  0x51, 0x26, 0x34, 0x32, 0x45, 0x01, 0xa2, 0xa0, 0xf2, 0x4f, 0x30, 0xdd,
  0xde, 0x61, 0xdc, 0x3b, 0x89, 0x8c, 0x9a, 0x1a, 0xeb, 0x17, 0x71, 0x5b,
  0xed, 0xc8, 0x33, 0x9e, 0xd7, 0x38, 0x7d, 0xdf, 0x34, 0x05, 0xf6, 0xb4,
  0x35, 0x48, 0x37, 0xa3, 0x9c, 0x57, 0xc8, 0xb3, 0x6e, 0x8f, 0x25, 0x40,
  0x6a, 0x2e, 0x68, 0x2f, 0x56, 0xa2, 0xd4, 0xf7, 0xd2, 0xba, 0x68, 0x9a,
  0x0a, 0xfb, 0xa5, 0x02, 0x86, 0x7b, 0xb0, 0x4b, 0xd5, 0x30, 0xfd, 0xb4,
  0x36, 0x34, 0xc1, 0xae, 0xf0, 0xb0, 0x7b, 0x83, 0x35, 0xc3, 0x97, 0x9c,
  0xf4, 0x06, 0xc3, 0xc3, 0x19, 0xa2, 0x53, 0x8d, 0xfb, 0x5e, 0x7b, 0x6d,
  0x1c, 0x79, 0x21, 0x2a, 0x6d, 0x63, 0x05, 0x0d, 0x8c, 0xb3, 0x9c, 0xd1,
  0x53, 0xbe, 0x94, 0xda, 0x75, 0x7d, 0x05, 0x2b, 0x94, 0xb9, 0x02, 0x71,
  0x29, 0xcf, 0x56, 0xbf, 0x84, 0x76, 0x3e, 0x07, 0x5b, 0xe3, 0x7d, 0x07,
  0xb4, 0xdd, 0xb4, 0xba, 0xef, 0xdf, 0xc1, 0xc6, 0x48, 0xf1, 0x77, 0x74,
  0x9e, 0xa5, 0xa0, 0xda, 0x55, 0xb3, 0xca, 0xe4, 0xb6, 0x55, 0xef, 0xea,
  0x76, 0xe0, 0x49, 0xe0, 0x8c, 0x6f, 0x50, 0xe7, 0xbb, 0x31, 0x87, 0x8f,
  0xce, 0x27, 0x9b, 0xda, 0xa8, 0x8d, 0x57, 0x0d, 0xa1, 0x07, 0x86, 0x6d,
  0x0c, 0xed, 0x52, 0x96, 0x83, 0xe1, 0xfd, 0x41, 0x8f, 0x0f, 0xb6, 0x28,
  0x0a, 0x50, 0x34, 0x9a, 0x1b, 0x6f, 0xdf, 0xbd, 0x7e, 0x13, 0xb2, 0xd7,
  0x45, 0xbe, 0x63, 0x71, 0x4b, 0x92, 0x86, 0x1c, 0x24, 0xcf, 0x6e, 0xdc,
  0xb8, 0x61, 0x98, 0x78, 0xe7, 0x54, 0x7a, 0xf0, 0x57, 0xb9, 0xa1, 0x79,
  0xee, 0xb6, 0xf6, 0x91, 0x03, 0x86, 0xd9, 0x0b, 0x9c, 0x82, 0x0a, 0x30,
  0x6d, 0xc3, 0xf0, 0x8a, 0x2f, 0x17, 0xff, 0x2f, 0xdb, 0x6c, 0xe9, 0x72,
  0xde, 0xa8, 0x48, 0x07, 0xbf, 0xe1, 0x81, 0x33, 0x9f, 0x9f, 0x6c, 0x1f,
  0xa3, 0x6a, 0xe7, 0x50, 0x2c, 0x75, 0x36, 0xbc, 0xbe, 0x6b, 0xc8, 0xf1,
  0x06, 0xcb, 0xb8, 0x66, 0xd8, 0xa2, 0x2b, 0xcd, 0xdc, 0x66, 0x23, 0x37,
  0x8c, 0x08, 0xf2, 0x0a, 0xf6, 0xed, 0x51, 0xd3, 0xb2, 0x7e, 0x60, 0xbc,
  0x27, 0x1f, 0xe8, 0xb4, 0xb7, 0x6d, 0x0c, 0x68, 0xb4, 0xe1, 0x8a, 0xe9,
  0x79, 0x01, 0x5b, 0x46, 0xbb, 0x7a, 0x8c, 0xb9, 0x8b, 0x90, 0x4a, 0x98,
  0x23, 0xa4, 0xca, 0xd9, 0x8d, 0x66, 0x1b, 0x66, 0x37, 0x77, 0xb8, 0xb7,
  0x9f, 0xe7, 0x8d, 0x10, 0xc6, 0x0d, 0x74, 0x9c, 0x0d, 0x7a, 0xa1, 0x6a,
  0xcf, 0x56, 0x73, 0xdf, 0x1e, 0xa9, 0x61, 0xef, 0x5b, 0xbb, 0x66, 0xd2,
  0xd9, 0x1c, 0xea, 0xc3, 0xd0, 0xd6, 0x19, 0x14, 0x83, 0x53, 0xc0, 0xce,
  0x41, 0x92, 0x5f, 0x86, 0x0a, 0x74, 0xad, 0x0a, 0x86, 0x7c, 0xc3, 0x2d,
  0x0f, 0x86, 0x91, 0xce, 0x94, 0xdc, 0x32, 0x72, 0xfc, 0x89, 0x52, 0x52,
  0x0d, 0x8c, 0x57, 0xa0, 0xb7, 0x52, 0x7d, 0x61, 0x98, 0x98, 0x52, 0x16,
  0x98, 0xd1, 0x2d, 0xaf, 0x58, 0x21, 0x35, 0x93, 0x5f, 0x70, 0xa3, 0xbf,
  0x81, 0xee, 0x07, 0x06, 0x6e, 0x1b, 0x26, 0xea, 0x52, 0x70, 0x18, 0x32,
  0xc1, 0x77, 0x2a, 0x62, 0x41, 0x8c, 0x48, 0x32, 0x07, 0x3b, 0x97, 0x4b,
  0xd4, 0x30, 0x31, 0xe5, 0xef, 0xc4, 0x0a, 0xf0, 0x12, 0x75, 0x01, 0x1a,
  0xee, 0xf1, 0x62, 0xdd, 0x14, 0xac, 0xdd, 0xaa, 0x63, 0xdd, 0x9a, 0x3e,
  0x8c, 0xc9, 0x24, 0xae, 0xc7, 0xd9, 0x75, 0x9b, 0x46, 0xe3, 0x7a, 0xc8,
  0x8c, 0xdb, 0x80, 0x81, 0xa8, 0x2a, 0x3c, 0x93, 0x7b, 0x26, 0x2f, 0x02,
  0xe6, 0x49, 0xe0, 0x7f, 0x9c, 0x08, 0xc0, 0x47, 0x27, 0x0e, 0x17, 0xfe,
  0x75, 0x8d, 0x93, 0x33, 0xdd, 0x7e, 0xd8, 0x65, 0xee, 0x81, 0x58, 0xd1,
  0x4d, 0xd3, 0x3c, 0x91, 0x71, 0xbd, 0x42, 0x9a, 0xda, 0x58, 0xea, 0x4f,
  0x72, 0xa0, 0xe1, 0xc3, 0xdd, 0x73, 0x6c, 0x38, 0x5d, 0x29, 0x8c, 0x57,
  0x8f, 0xa2, 0xd7, 0xb5, 0x7a, 0x62, 0xa8, 0xd6, 0x75, 0xe4, 0xba, 0x56,
  0x57, 0xaa, 0x5b, 0xa1, 0xd7, 0x35, 0x2e, 0x57, 0xa6, 0x61, 0x8f, 0xae,
  0x77, 0x47, 0xc7, 0x1e, 0x7d, 0x77, 0xd4, 0xde, 0x0a, 0x47, 0xcd, 0xff,
  0xe1, 0x1f, 0x78, 0x62, 0xe0, 0x81, 0x25, 0x0f, 0x00, 0x00 
};

ESP8266WebServer server(HTTP_PORT);
DNSServer dns;

void setUpdaterError()
{
    if (SERIAL_OUTPUT)
        Update.printError(Serial);
    StreamString str;
    Update.printError(str);
    updaterError = str.c_str();
}

void handleRedirect()
{ 
    server.sendHeader("Location", "/update", true);
    server.send(302, "text/plain", ""); 
}

void setup(void)
{
    char NameChipId[64] = {0};
    snprintf(NameChipId, sizeof(NameChipId), "%s-%06x", HOSTNAME, ESP.getChipId());
    if (SERIAL_OUTPUT)
    {
        Serial.begin(115200);
        Serial.println();
        Serial.println("Booting Sketch...");
    }
    WiFi.hostname(NameChipId);
    if (!WIFI_SSID.length()){
      WiFi.mode(WIFI_AP);
    }else{
      WiFi.mode(WIFI_AP_STA);
      WiFi.begin(WIFI_SSID.c_str(), WIFI_PASS.c_str());
      if (SERIAL_OUTPUT) Serial.printf("Connecting to %s ", WIFI_SSID.c_str());
      while (WiFi.status() != WL_CONNECTED)
      {
        if (SERIAL_OUTPUT) Serial.print(".");
        delay(1000);
      }
      if (SERIAL_OUTPUT){
        Serial.println(" connected!");
        Serial.print("Router IP: ");
        Serial.println(WiFi.localIP());
      }
    }
    WiFi.softAPConfig(apIP, apIP, IPAddress(255, 255, 255, 0));
    WiFi.softAP(NameChipId);
    if (SERIAL_OUTPUT)
    {
        Serial.print("HotSpt IP: ");
        Serial.println(WiFi.softAPIP());
    }
    dns.start(DNS_PORT, "*", WiFi.softAPIP());

    server.on("/", handleRedirect);
    server.on("/version", []() {
        server.send(200, "text/plain", SKETCH_VERSION);
    });
    // handler for the /update form page
    server.on(HTTP_UPDATE_PATH, HTTP_GET, [&]() {
        if (username != emptyString && password != emptyString && !server.authenticate(username.c_str(), password.c_str()))
            return server.requestAuthentication();
        // server.send_P(200, PSTR("text/html"), update_htm);
        server.sendHeader("Content-Encoding", "gzip", true);
        server.send_P(200, PSTR("text/html"), update_htm_gz, update_htm_gz_len);
    });

    // handler for the /update form POST (once file upload finishes)
    server.on(HTTP_UPDATE_PATH, HTTP_POST, [&]() {
      if(!authenticated)
        return server.requestAuthentication();
      if (Update.hasError()) {
        server.send(200, F("text/plain"), String(F("Update error: ")) + updaterError);
      } else {
        server.client().setNoDelay(true);
        server.send(200, PSTR("text/plain"), String(F("Update OK")));
        delay(100);
        server.client().stop();
        ESP.restart();
      } }, [&]() {
      // handler for the file upload, get's the sketch bytes, and writes
      // them through the Update object
      HTTPUpload& upload = server.upload();

      if(upload.status == UPLOAD_FILE_START){
        updaterError = String();
        if (SERIAL_OUTPUT)
          Serial.setDebugOutput(true);

        authenticated = (username == emptyString || password == emptyString || server.authenticate(username.c_str(), password.c_str()));
        if(!authenticated){
          if (SERIAL_OUTPUT)
            Serial.printf("Unauthenticated Update\n");
          return;
        }

        WiFiUDP::stopAll();
        if (SERIAL_OUTPUT)
          Serial.printf("Update: %s\n", upload.filename.c_str());
        uint32_t maxSketchSpace = (ESP.getFreeSketchSpace() - 0x1000) & 0xFFFFF000;
        if(!Update.begin(maxSketchSpace)){//start with max available size
          setUpdaterError();
        }
      } else if(authenticated && upload.status == UPLOAD_FILE_WRITE && !updaterError.length()){
        if (SERIAL_OUTPUT) Serial.printf(".");
        if(Update.write(upload.buf, upload.currentSize) != upload.currentSize){
          setUpdaterError();
        }
      } else if(authenticated && upload.status == UPLOAD_FILE_END && !updaterError.length()){
        if(Update.end(true)){ //true to set the size to the current progress
          if (SERIAL_OUTPUT) Serial.printf("\nUpdate Success: %u\nRebooting...\n", upload.totalSize);
        } else {
          setUpdaterError();
        }
        if (SERIAL_OUTPUT) Serial.setDebugOutput(false);
      } else if(authenticated && upload.status == UPLOAD_FILE_ABORTED){
        Update.end();
        if (SERIAL_OUTPUT) Serial.println("Update was aborted");
      }
      delay(0); });
    server.onNotFound(handleRedirect);
    server.begin();
}

void loop(void)
{
    server.handleClient();
    dns.processNextRequest();
}