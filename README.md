# home_automation_ESP8266
This repository serves code for Automation for electrical devices using ESP8266 running as a Webserver and also in Station Mode, WiFi credentials are passed by connecting to it in AP mode.

Flow:
1. If it is running for the first time, that means nothing is in EEPROM so it starts itself in Access Point Mode.
2. Get connected to it in AP mode, using Phone or Laptop with the given credentials.
3. Use the wifisave call to send the WiFi credentials to the ESP8266, it saves that in EEPROM.
4. It gets connected to the Local Wifi, if credentials are correct and then starts its MDNS server.
5. Because of MDNS, the device can be reached directly without knowing the IP address of the device in local network by using its mdns name. ex- smarthome.local


For Testing using Postman Use the following exported json:
***


For testing with mobile end, I am using HTTP Shortcuts
https://play.google.com/store/apps/details?id=ch.rmy.android.http_shortcuts
and here goes the shortcuts file to get it working instantly.
****
