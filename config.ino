// 8x32 ws2812 5050 RGB wifi banner NodeMcu 1.0 esp8266 esp-12e
// HTML goes in data folder >> ctrl+shift+p >> upload littleFS to **
// LCD goes to RX pin

//opensource version

#include <NeoPixelBusLg.h>  // https://github.com/Makuna/NeoPixelBus
#include <ESP8266WiFi.h>
#include <ESPAsyncTCP.h>        //https://github.com/me-no-dev/ESPAsyncTCP
#include <ESPAsyncWebServer.h>  //https://github.com/me-no-dev/ESPAsyncWebServer/blob/master/examples/ESP_AsyncFSBrowser/ESP_AsyncFSBrowser.ino
#include <WiFiClientSecure.h>   //https://github.com/esp8266/Arduino/blob/master/libraries/ESP8266WiFi/examples/HTTPSRequest/HTTPSRequest.ino
#include "FS.h"
#include <LittleFS.h>  // https://github.com/espressif/arduino-esp32/blob/master/libraries/LittleFS/examples/LITTLEFS_test/LITTLEFS_test.ino
#include <time.h>

#define se ";"                                              //info separator
#define kyst "(key stored)"                                 // key stored text
#define WFT 50                                              // Wifi tries
#define ARSZ(array) ((sizeof(array)) / (sizeof(array[0])))  //array size
#define Width 32                                            //const uint8_t Width = 16;
#define Height 8                                            //const uint8_t Height = 16;
#define LEDnum 256                                          //const int LEDnum = (Width * Height);
#define flip 1
#define mirr 1
//webserver
#define tp "text/plain"
#define th "text/html"
#define jn "application/json"
#define ct "Content-Type"
#define bm "<p><a href='/'>Back to menu</a>"
//files
#define xMyTZ "/MyTZ"
#define xMyTZsi "/MyTZsi"
#define xNTP1 "/NTP1"
#define xNTP2 "/NTP2"
#define xHideAP "/HideAP"
#define xAPSSID "/APSSID"
#define xAPPSK "/APPSK"
#define xSTASSID "/STASSID"
#define xSTAPSK "/STAPSK"
#define xglow "/aglow"
#define xfoco "/afoco"
#define xmess "/mess"

const char* HideAP PROGMEM = "2";
const char* MyTZ PROGMEM = "EST5EDT,M3.2.0,M11.1.0";
const char* APSSID PROGMEM = "WiFi Banner";
const char* APPSK PROGMEM = "ChangeMe";
const char* NTP1 PROGMEM = "ca.pool.ntp.org";
const char* NTP2 PROGMEM = "pool.ntp.org";
const char* smspost PROGMEM = R"=({ "MessageBody":"Wifi Banner, !d!" })=";
const char* tabo PROGMEM = "<table id='chbt'>";
const char* td1 PROGMEM = "<tr><td><a href='/edit?f=/";
const char* td2 PROGMEM = "</a></td><td> ";
const char* td3 PROGMEM = "<a href='/delt?f=/";
const char* td4 PROGMEM = "'>Del</a></td></tr>";
const char* tabc PROGMEM = "</table>";
const char* nanw PROGMEM = "Number of available networks:";
const char* btnz PROGMEM = "(Buttons below set SSID in Main tab)<p>";
const char* bt1 PROGMEM = "<button onclick='upd(";
const char* bt2 PROGMEM = ")' type='button' id='wf";
const char* plzw PROGMEM = "<meta http-equiv='refresh' content='8; url=/' /><h1>Please Wait... You may have to reconnect to WiFi.</h1>";
const char* btfs PROGMEM = "Poof! <a href='/files.htm'>back to files</a>";
const char* ed1 PROGMEM = "<form action='/sfile' method='post'><textarea name=data rows='25' cols='25'>";
const char* ed2 PROGMEM = "</textarea><br><input name='f' value='";
const char* ed3 PROGMEM = "'><button>Save</button></form>";

// For Esp8266, the Pin is omitted and it uses GPIO3 / RX due to DMA hardware use.
NeoPixelBusLg<NeoGrbFeature, NeoEsp8266DmaWs2812xMethod> strip(LEDnum);

//generated
int awifi = 0;  // wifi bars
int wfc[] = { 0, 0 };  //bootup stuff: [0]=wifi connected, or turn on AP & [1]=temp/humd loaded

//set variables
int afoco = 1;             //Font Colour 0=manual
int amspd[] = { 600, 0 };  // marquee speed, counter
int yax = 1;               //y axis

// toggle modes
int rst = 0;  //reset
time_t now;
struct tm tm;
boolean flag = true;
int tik = 0;                                                                                                                                                              //tick count
const int tikr = 12;                                                                                                                                                      // max ticks between update
int c1 = 0, c2 = 0;                                                                                                                                                       // center prev
int dn[8] = { 1, 5, 3, 4, -1, -1, -1 };                                                                                                                                   // Digits now
int row[5][3] = { { 255, 0, 0 }, { 255, 128, 0 }, { 255, 255, 0 }, { 0, 255, 0 }, { 0, 0, 255 } };                                                                        //rainbow

String marquee = "         !hoot!      ";  //message
int mpos[] = { 1, 0 };                     //mac position ( pos 0-3, string index )

AsyncWebServer server(80);

//https client
//WiFiClientSecure client; HTTPClient https;
String hty[] = { "", "" };  //client
//int httpCode = 0;

const char* diga PROGMEM = "0123456789AbBcCDdEFGHhIiJjKLlnOoPprSTtUuVXxY:^+-*%/[]=_?!. ";  // ^=^c and *=deg
const short digz[] = {
 0x7B6F,  // 0b111101101101111 0
 0x7493,  // 0b111010010010011 1
 0x73E7,  // 0b111001111100111 2
 0x79E7,  // 0b111100111100111 3
 0x49ED,  // 0b100100111101101 4
 0x79CF,  // 0b111100111001111 5
 0x7BCF,  // 0b111101111001111 6
 0x4927,  // 0b100100100100111 7
 0x7BEF,  // 0b111101111101111 8
 0x79EF,  // 0b111100111101111 9
 0x5BEA,  // 0b101101111101010 A 10
 0x7BC9,  // 0b111101111001001 b
 0x3AEB,  // 0b011101011101011 B
 0x73C0,  // 0b111001111000000 c
 0x624E,  // 0b110001001001110 C
 0x3B6B,  // 0b011101101101011 D 15
 0x7BE4,  // 0b111101111100100 d
 0x73CF,  // 0b111001111001111 E
 0x13CF,  // 000b1001111001111 F
 0x7B4F,  // 0b111101101001111 G
 0x5BED,  // 0b101101111101101 H 20
 0x5BC9,  // 0b101101111001001 h
 0x7497,  // 0b111010010010111 I
 0x2482,  // 0b010010010000010 i
 0x7B27,  // 0b111101100100111 J
 0x7B04,  // 0b111101100000100 j 25
 0x5AED,  // 0b101101011101101 K
 0x7249,  // 0b111001001001001 L
 0x2492,  // 0b010010010010010 l
 0x5AC0,  // 0b101101011000000 n
 0x2B6A,  // 0b010101101101010 O 30
 0x2A80,  // 0b010101010000000 o
 0x12EB,  // 0b001001011101011 P
 0x1758,  // 0b001011101011000 p
 0x1BC0,  // 0b001101111000000 r
 0x39CE,  // 0b011100111001110 S 35
 0x2497,  // 0b010010010010111 T
 0x25D0,  // 0b010010111010000 t
 0x7B6D,  // 0b111101101101101 U
 0x6B40,  // 0b110101101000000 u
 0x2B6D,  // 0b010101101101101 V 40
 0x5AAD,  // 0b101101010101101 X
 0x5540,  // 0b101010101000000 x
 0x24AD,  // 0b010010010101101 Y
 0x0410,  // 0b000010000010000 :
 0x01CF,  // 0b000000111001111 ^c 45
 0x05D0,  // 0b000010111010000 +
 0x01C0,  // 0b000000111000000 -
 0x01EF,  // 0b000000111101111 *deg
 0x42A1,  // 0b100001010100001 %
 0x02A0,  // 0b000001010100000 / 50
 0x324B,  // 0b011001001001011 [
 0x6926,  // 0B110100100100110 ]
 0x0E38,  // 0b000111000111000 =
 0x7000,  // 0b111000000000000 _
 0x21A7,  // 0b010000110100111 ? 55
 0x2092,  // 0b010000010010010 !
 0x2000,  // 0b010000000000000 .
 0x0000   // 0b000000000000000 null
};
const int ldigz = ARSZ(digz) - 1;  //length of digz array

//functions
uint16_t XYz(uint8_t y, uint8_t x) {
 uint16_t i, rX;
 if (y & 0x01) {  //odd //serpentine
  rX = flip ? x : (Height - 1) - x;
 } else {  //even
  rX = flip ? (Height - 1) - x : x;
 }
 i = (y * Height) + rX;
 if (mirr == 1) { i = LEDnum - 1 - i; }
 return i;
}


int WifiSig() {
 int draw = 0;
 if (WiFi.status() == WL_CONNECTED) {
  int bars = WiFi.RSSI();
  if (bars >= -60) { draw = 6; }
  if (bars <= -61 && bars > -70) { draw = 5; }
  if (bars <= -71 && bars > -80) { draw = 4; }
  if (bars <= -81 && bars > -90) { draw = 3; }
  if (bars <= -91 && bars > -100) { draw = 2; }
  if (bars <= -101 && bars > -110) { draw = 1; }
  if (bars <= -111) { draw = 0; }
 }
 return draw;
}

String listFiles(AsyncWebServerRequest* request) {
 String out = tabo;
 Dir d = LittleFS.openDir("/");
 while (d.next()) {
  if (d.isFile() && d.fileName().indexOf(".") == -1) {
   out += td1;
   out += d.fileName();
   out += "'>";
   out += d.fileName();
   out += td2;
   //out+=dir.size(); //out+=" bytes";
   out += td3;
   out += d.fileName();
   out += td4;
  }
 }
 out += tabc;
 return out;
}

String readFile(const char* path, int rd = 1) {  //rd=0 check file exist, 1=regular, 2=var mode
 String out;
 //Serial.print(F("PT: ")); Serial.print(rd); Serial.print(" ");  Serial.println(path);
 File f = LittleFS.open(path, "r");
 //Serial.println(f);
 if (!f || f.isDirectory()) {
  //Serial.println(smsurl);
  //strcpy(out,"!Err Open ");
  //strcat(out,path);
  if (rd != 2) {
   out = "! " + String(path);
   //out.concat(path);
   //Serial.println(F("!Err Open read ")+ String(path));
  } else {
   if (path == xMyTZ) { out = MyTZ; }
   //if (String(path) == "/tempadj") { out = tempadj; }
   //if (String(path) == "/afoco") { out = afoco; }
   //if (String(path) == "/aglow") { out = aglow; }

   if (String(path) == xAPSSID) { out = APSSID; }
   if (String(path) == xAPPSK) { out = APPSK; }
   if (String(path) == xNTP1) { out = NTP1; }
   if (String(path) == xNTP2) { out = NTP2; }
   if (String(path) == xHideAP) { out = HideAP; }
   //if (String(path) == "/afc") { out = 0; }
   //Serial.print("DF: ");  Serial.println(out);
  }
 }
 if (rd > 0) {
  while (f.available()) {
   char x = f.read();
   //strcat(out,x);
   out.concat(x);
  }
  //Serial.print("RD: "); Serial.println(out);
 }

 f.close();
 return out;
}

String writeFile(const char* path, const char* message) {
 if (message == kyst) { return "x"; }
 //String writeFile(fs::FS &fs, const char * path, const char * message){
 //path= '/' + path;
 //char* out = "Unchanged";
 //char* tmp = readFile(path,0);
 String out = "Unchanged";
 String tmp = String(readFile(path, 9));
 if (tmp != message) {
  //Serial.printf("Writing file: %s\r\n", path);
  File f = LittleFS.open(String(path), "w");
  if (!f) {
   Serial.println(F("!Err open write ") + String(path));
   out = "Fail";
  } else {
   if (f.print(message)) {
    Serial.println(F("wrote ") + String(path));
    out = "Saved";
   } else {
    Serial.println(F("!fail ") + String(path));
    out = "Failed";
   }
  }
  f.close();
 }
 return out;
}

String wifilist(AsyncWebServerRequest* request) {
 String pg = "";
 AsyncWebParameter* p = request->getParam(0);
 if (p->value() == "1") {
  //call scan in bg
  pg = "scanning";
  int ns = WiFi.scanNetworks(true);
 } else {
  //get data
  int ns = WiFi.scanComplete();
  if (ns >= 0) {
   pg = nanw + String(ns) + "<br>";
   pg += btnz;
   for (int sd = 0; sd < ns; sd++) {
    String nam = WiFi.SSID(sd);
    pg += bt1 + String(sd) + bt2;
    pg += String(sd) + "' name='" + nam + "'>" + nam + "</button>  ";
    pg += String(WiFi.RSSI(sd)) + " dBm<br>";
    //ESP.wdtFeed();
   }
   WiFi.scanDelete();
  } else {
   pg = ".";
  }
 }
 return pg;
}

String restartx(AsyncWebServerRequest* request) {
 rst = 666;  //reset
 return plzw;
}


String sfile(AsyncWebServerRequest* request) {
 AsyncWebParameter* p = request->getParam(1);  //name
 String tmp1 = String(p->value());
 p = request->getParam(0);  //data
 String tmp2 = String(p->value());
 String data = writeFile(tmp1.c_str(), tmp2.c_str());
 data += bm;  //back to menu
 return data;
}
String infox(AsyncWebServerRequest* request) {
 AsyncWebParameter* p = request->getParam(0);
 int full = p->value().toInt();
 String data;

 if (full < 2) {
  data += String(WifiSig());
  data += se + String(ESP.getFreeHeap());  //uptime secs / free ram
  String tmp = readFile("/mess");
  data += se + tmp;
 }
 if (full == 1) {
  String tmp = readFile("/aglow");
  if (tmp.charAt(0) == '!') { tmp = "50"; }
  data += se + tmp + se + String(amspd[0]) + se + readFile(xSTASSID, 2) + se + String(afoco) + se;
  data += readFile(xSTAPSK, 2) != "" ? kyst : "";
  data += se;
  data += readFile(xAPPSK, 2) != "" ? kyst : "";
  data += se + readFile(xMyTZ, 2) + se + readFile(xMyTZsi, 2);
  data += se + WiFi.localIP().toString() + se + WiFi.softAPIP().toString();
  data += se + readFile(xAPSSID, 2) + se + readFile(xHideAP, 2);  // AP
  data += se + readFile(xNTP1, 2) + se + readFile(xNTP2, 2);      // NTP

  char bf[6];
  for (int ii = 0; ii < 5; ii++) {  //5 font colours
   data += se;
   data += "#";
   String r = String(row[ii][0], HEX);
   String g = String(row[ii][1], HEX);
   String b = String(row[ii][2], HEX);
   sprintf(bf, "%02s", r.c_str());
   data += bf;  //add leading 0
   sprintf(bf, "%02s", g.c_str());
   data += bf;
   sprintf(bf, "%02s", b.c_str());
   data += bf;
  }
  data += se;
 }
 return data;
}


String dpxs(AsyncWebServerRequest* request) {  //draw line
 int qr, qb, qg, qx, qy;
 //server.send(200, "text/html", "ok" );
 for (int ii = 0; ii < request->params(); ii++) {
  AsyncWebParameter* p = request->getParam(ii);
  if (p->name() == "r") {
   qr = strtol(p->value().c_str(), NULL, 16);
  } else if (p->name() == "g") {
   qg = strtol(p->value().c_str(), NULL, 16);
  } else if (p->name() == "b") {
   qb = strtol(p->value().c_str(), NULL, 16);
  } else {
   if (p->name()[0] == 'v') {
    int pch = p->value().indexOf("x");
    if (pch != -1) {
     qx = p->value().substring(0, pch).toInt();
     qy = p->value().substring(pch + 1).toInt();
     strip.SetPixelColor(XYz(qx, qy), RgbColor(qr, qg, qb));
    }
   }
  }
 }
 strip.Show();
 return "ok";
}

String dpx(AsyncWebServerRequest* request) {  //draw pixel on screen
 AsyncWebParameter* qx = request->getParam(0);
 AsyncWebParameter* qy = request->getParam(1);
 AsyncWebParameter* p = request->getParam(2);
 int qr = strtol(p->value().c_str(), NULL, 16);
 p = request->getParam(3);
 int qg = strtol(p->value().c_str(), NULL, 16);
 p = request->getParam(4);
 int qb = strtol(p->value().c_str(), NULL, 16);

 strip.SetPixelColor(XYz(qx->value().toInt(), qy->value().toInt()), RgbColor(qr, qg, qb));
 strip.Show();
 return "ok";
}

String dttm(AsyncWebServerRequest* request) {  //set date and time
 AsyncWebParameter* p = request->getParam(0);
 time_t rtc = p->value().toInt();
 timeval tv = { rtc, 0 };
 settimeofday(&tv, nullptr);
 return "ok";
}

String togl(AsyncWebServerRequest* request) {  // toggle checks
 return "ok";
}

String saveForm(AsyncWebServerRequest* request) {
 char buf[15];
 AsyncWebParameter* p = request->getParam(0);
 String typ = p->value();
 for (int ii = 1; ii < request->params(); ii++) {
  AsyncWebParameter* p = request->getParam(ii);
  strcpy(buf, "/");
  strcat(buf, p->name().c_str());
  if (p->value() == "-1") {  //delete file
   LittleFS.remove(buf);
  } else {
   if (p->name() != "") { writeFile(buf, p->value().c_str()); }
  }
  if (p->name() != "") {
   if (typ == "data") {  //set data vars
    if (p->name() == "amspd") { amspd[0] = p->value().toInt(); }
   }
  }
 }
 return "Settings saved!";
}
String sped(AsyncWebServerRequest* request) {  //marq speed
 AsyncWebParameter* p = request->getParam(0);
 amspd[0] = p->value().toInt();
 return "ok";
}
String glow(AsyncWebServerRequest* request) {  //brightness aglow
 AsyncWebParameter* p = request->getParam(0);
 strip.SetLuminance(p->value().toInt());
 return "ok";
}

String foco(AsyncWebServerRequest* request) {  //font colour
 //server.send(200, "text/plain", "ok");
 AsyncWebParameter* p = request->getParam(0);
 afoco = p->value().toInt();
 for (int i = 0; i < 5; i++) {

  AsyncWebParameter* p = request->getParam(i + 1);
  String qr = p->value().substring(0, 2);
  String qg = p->value().substring(2, 4);
  String qb = p->value().substring(4, 6);
  int tr = strtol(qr.c_str(), NULL, 16);
  int tg = strtol(qg.c_str(), NULL, 16);
  int tb = strtol(qb.c_str(), NULL, 16);
  row[i][0] = tr;
  row[i][1] = tg;
  row[i][2] = tb;
 }
 return "ok";
}

String newf(AsyncWebServerRequest* request) {
 AsyncWebParameter* p = request->getParam(0);
 writeFile(("/" + p->value()).c_str(), "");
 return "File created, refresh!";
}

void notFound(AsyncWebServerRequest* request) {
 char out[50] = "Not found!";
 strcat(out, bm);
 request->send(404, th, out);
}

String marq(AsyncWebServerRequest* request) {
 AsyncWebParameter* p = request->getParam(0);
 marquee = "         " + p->value() + "     ";
 mpos[1] = 0;
 writeFile(xmess, p->value().c_str());
 return "ok";
}

void setup() {
 Serial.begin(115200);
 while (!Serial)
  ;  // wait for serial attach

 strip.Begin();  // INITIALIZE NeoPixel strip object (REQUIRED)
 strip.Show();   //clear

 if (!LittleFS.begin()) {
  Serial.println(F("LittleFS Mount Failed"));
 } else {  // load variables on boot
  String tmp;
  //display
  tmp = readFile("/amspd");
  amspd[0] = tmp.charAt(0) != '!' ? tmp.toInt() : amspd[0];

  tmp = readFile(xmess);
  marquee = "         " + tmp + "     ";

  tmp = readFile(xglow);
  int aglow = tmp.charAt(0) != '!' ? tmp.toInt() : 50;
  strip.SetLuminance(aglow);

  tmp = readFile(xfoco);
  afoco = tmp.charAt(0) != '!' ? tmp.toInt() : afoco;
  for (int ii = 1; ii < 6; ii++) {  //font1
   tmp = "/f" + String(ii);
   tmp = readFile(tmp.c_str());
   if (tmp.charAt(0) != '!') {
    //convert hex to decimal
    String qr = tmp.substring(1, 3);
    String qg = tmp.substring(3, 5);
    String qb = tmp.substring(5, 7);
    //Serial.printf("x(%s %s %s)",qr,qg,qb);
    int tr = strtol(qr.c_str(), NULL, 16);
    int tg = strtol(qg.c_str(), NULL, 16);
    int tb = strtol(qb.c_str(), NULL, 16);
    row[ii - 1][0] = tr;
    row[ii - 1][1] = tg;
    row[ii - 1][2] = tb;
    //Serial.printf("y[%d %d %d]",tr,tg,tb);
   }
  }
 }

 yield();
 String SID = readFile(xSTASSID, 2);
 String PSK = readFile(xSTAPSK, 2);
 String RDF0, RDF1, RDF2;
 RDF2 = readFile(xHideAP, 2);
 if (SID == "" || PSK == "") { RDF2 = 2; }  // Turns on AP if WiFi not config
 if (RDF2 == 0) {                           //0=disable, 1=hide, 2=show
  WiFi.mode(WIFI_STA);
  Serial.println(F("STA Only"));
 } else {
  WiFi.mode(WIFI_AP_STA);
  int hide = RDF2 == "1" ? 1 : 0;
  /// void softAP(const char* ssid, const char* passphrase, int channel = 1, int ssid_hidden = 0);
  RDF0 = readFile(xAPSSID, 2);
  RDF1 = readFile(xAPPSK, 2);
  String tmp = WiFi.softAP(RDF0, RDF1, 8, hide) ? "Ready" : "Failed";
  Serial.println(F("AP: ") + tmp);
  if (tmp == "Ready") {
   Serial.println(F("Clock Wifi AP:"));
   Serial.println(F("APSSID: ") + RDF0);
   Serial.println(F("APPSK: ") + RDF1);
   Serial.print(F("APIP: "));
   Serial.println(WiFi.softAPIP());
  }
 }

 WiFi.setAutoReconnect(true);
 WiFi.persistent(true);

 if (SID != "" && PSK != "") {
  WiFi.begin(SID, PSK);
  delay(100);
  Serial.println(F("Local Wifi:"));
  Serial.println(F("SSID: ") + SID);
  Serial.println(F("PSK: ") + PSK);
  wfc[0] = 0;
  WiFi.status();
 }
 server.serveStatic("/", LittleFS, "/").setDefaultFile("index.htm");
 server.on("/restart", HTTP_GET, [](AsyncWebServerRequest* request) {
  request->send(200, th, restartx(request));
 });
 server.on("/sfile", HTTP_POST, [](AsyncWebServerRequest* request) {
  request->send(200, th, sfile(request));
 });
 server.on("/wifisig", HTTP_POST, [](AsyncWebServerRequest* request) {
  request->send(200, tp, String(WifiSig()));
 });
 server.on("/wifilist", HTTP_POST, [](AsyncWebServerRequest* request) {
  request->send(200, th, wifilist(request));
 });
 server.on("/togl", HTTP_POST, [](AsyncWebServerRequest* request) {
  request->send(200, tp, togl(request));
 });
 server.on("/dpxs", HTTP_POST, [](AsyncWebServerRequest* request) {
  request->send(200, tp, dpxs(request));
 });
 server.on("/dpx", HTTP_POST, [](AsyncWebServerRequest* request) {
  request->send(200, tp, dpx(request));
 });
 server.on("/info", HTTP_POST, [](AsyncWebServerRequest* request) {
  request->send(200, tp, infox(request));
 });
 server.on("/dttm", HTTP_POST, [](AsyncWebServerRequest* request) {
  request->send(200, tp, dttm(request));
 });
 server.on("/marq", HTTP_POST, [](AsyncWebServerRequest* request) {
  request->send(200, tp, marq(request));
 });
 server.on("/glow", HTTP_POST, [](AsyncWebServerRequest* request) {
  request->send(200, tp, glow(request));
 });
 server.on("/sped", HTTP_POST, [](AsyncWebServerRequest* request) {
  request->send(200, tp, sped(request));
 });
 server.on("/foco", HTTP_POST, [](AsyncWebServerRequest* request) {
  request->send(200, tp, foco(request));
 });
 server.on("/saveForm", HTTP_POST, [](AsyncWebServerRequest* request) {
  request->send(200, tp, saveForm(request));
 });

 server.onNotFound(notFound);
 server.begin();
}

void Drawr(int xx, int yy, int data, int modx = 0) {  // puts 5x3 letter/num into buffer
 if (data < 0 || data > ldigz) {
  return;
 }
 int ii = 1;
 for (uint8_t y = 0; y < 5; y++) {
  for (uint8_t x = 0; x < 3; x++) {
   uint8_t tx = x;
   tx = 3 - x;        //invert
   int cx = xx - tx;  //Width-1-xx;
   int cy = yy + y;

   if (cx < Width && cy < Height && cx >= 0 && cy >= 0) {  //trims off OoB
    if (digz[data] & ii) {
     strip.SetPixelColor(XYz(cx, cy), RgbColor(row[y][0], row[y][1], row[y][2]));
    } else {
     if (!modx || (x == 1)) {
      strip.SetPixelColor(XYz(cx, cy), RgbColor(0, 0, 0));
     }
    }
   }
   ii = ii + ii;
  }
 }
 return;
}


void Dmac(int num) {  // marquee mode, draw line num
 if (num == -1) {     //clear
  for (int x = 0; x < Width; x++) {
   for (int y = 0; y < Height; y++) {
    strip.SetPixelColor(XYz(x, y), RgbColor(0, 0, 0));
   }
  }
  mpos[0] = mpos[0] < 0 ? 3 : mpos[0] - 1;
  mpos[1] = mpos[0] == 3 ? mpos[1] + 1 : mpos[1];
  if (mpos[1] > marquee.length() - 5) {
   mpos[1] = 0;
   yax = random(0, 4);
  }
 } else {
  char ma = marquee.charAt(num + mpos[1]);
  char* tx = strchr(diga, ma);
  int tmp = (tx - diga);
  if (tmp != ldigz) {
   Drawr(mpos[0] + ((num)*4), yax, tmp);
  }
 }
}


void loop() {
 if (rst > 1) {
  rst--;  //web reset
  if (rst < 2) { ESP.restart(); }
 }
 tik++;
 if (tik >= tikr) {
  tik = -1;
  amspd[1]++;  //marquee speed, skips cycles
  if (amspd[1] > amspd[0]) {
   amspd[1] = 0;
  }
 }
 if (tik < 9) {                      // marquee mode
  if (amspd[1] == 0) { Dmac(tik); }  //check speed
 }
 if (tik == 10) {
  if (wfc[0] != -1) {  // Test wifi
   if (WiFi.status() != WL_CONNECTED) {
    Serial.print(F("w"));
    wfc[0]++;
   } else {
    Serial.println(F(" Connected, IP address: "));
    yield();
    Serial.println(WiFi.localIP());
    time(&now);
    localtime_r(&now, &tm);
    wfc[0] = -1;
    String RDF0, RDF1, RDF2;
    RDF0 = readFile(xMyTZ, 2);
    RDF1 = readFile(xNTP1, 2);
    RDF2 = readFile(xNTP2, 2);
    configTime(RDF0.c_str(), RDF1.c_str(), RDF2.c_str());
    now = time(nullptr);
    while (now < 8 * 3600 * 2) {
     delay(400);
     Serial.print(F("t"));
     now = time(nullptr);
    }
   }
  }
 }

 if (tik == 11) {
  if (amspd[1] == 0) {
   strip.Show();
  }
 }
}
