<pre>
 Notice:  I pulled this code base from another project of mine, this code works, although I didn't optimize it, there could be cleanup of old code to do yet.
 
 Okay, you want to create a marquee banner! Here is my code for a Wifi router like config page for your custom banner.

Step 1.
Buy the parts.
 I used an ESP8266 NodeMCU 12E and a 8x32 WS2812 5050 LED panel.
 You'll also need wires, either some jumpers or some to solder on.
 Two USB 2.0 cables, one USBA to micro to program the ESP. The other can be an old broken cable, as long as the USBA end is fine.

Step 2.
Wiring it up is easy. On the 8x32 panel there is a wire (3 wires) labeled DIN, GND and 5V.  These can connect via a female-female jumper wires to the ESP board.
 DIN goes to RX
 5V to VIN
 GND connects to G

Step 3.
You need a USB2 cable, remove the micro/mini connector and keep the USBA part. The red and black wires connect to the red and black ones on the middle of the panel.
Since there is no connector here, I recommend solder. Make sure to isolate the other two unused wires so they don't short on anything, tape or hot glue. This cable
will power the panel, the ESP board runs on 3.3v and will not power the 5050 panel.

Step 4.
Next you'll need to program the ESP. This requires the arduino IDE from arduino.cc, get the latest version.
To use my IDE, you'll also need these libraries installed:
 ESP8266WiFi
 ESPAsyncTCP
 ESPAsyncWebServer
 NeoPixelBus by Makuna
This plugin:
 LittleFS uploader - https://github.com/earlephilhower/arduino-littlefs-upload

Step 5.
Program the ino sketch to the ESP board. Then dump the HTM files via the litteFS uploader.

Finish.
If all went well, you should be able to find a wifi hotspot called "WiFi Banner". The default password is "ChangeMe". Connect to the hotspot, then in your browser visit http://192.168.4.1

</pre>
