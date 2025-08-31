#include <WiFi.h>
#include <WiFiManager.h>
#include <WiFiClientSecure.h>
#include <ArduinoHttpClient.h>
#include "SPI.h"
#include "Adafruit_GFX.h"
#include "Adafruit_ILI9341.h"
#include "Fonts/FreeSansBold9pt7b.h"
#include "Fonts/FreeSans9pt7b.h"
#include "Fonts/FreeSansBold12pt7b.h"

// Color Define
//#define RED 0xf882
//#define RED 0x001F  // Configuration for TZT ESP32 LVGL screen
#define RED 0xf904
//#define YELLOW 0xff80
//#define YELLOW 0x07FF  // Configuration for TZT ESP32 LVGL screen
#define YELLOW 0xffe0
#define GREEN 0x07e0
#define WHITE 0xFFFF
#define BACKGROUND 0x0000

// define element position on the screen
#define column 145
#define row 25
#define row_offset 20
#define s_column 140
#define s_column_offset 15
#define s_row 20
#define s_row_offset 155

// System configuration
#define interval 60 // interval in minutes for getting data from the server 
#define RESET_PIN 32

// Screen configuration
//#define TFT_DC 2  // Configuration for TZT ESP32 LVGL screen
//#define TFT_MISO 12  // Configuration for TZT ESP32 LVGL screen
//#define TFT_MOSI 13  // Configuration for TZT ESP32 LVGL screen
//#define TFT_SCLK 14  // Configuration for TZT ESP32 LVGL screen
//#define TFT_CS 15  // Configuration for TZT ESP32 LVGL screen
//#define TFT_RST -1  // Configuration for TZT ESP32 LVGL screen
//#define LED 27  // Configuration for TZT ESP32 LVGL screen

#define TFT_CS   12
#define TFT_DC   14
#define TFT_MOSI 27
#define TFT_CLK  25
#define TFT_RST  26
#define TFT_MISO 33
#define LED 22

// Screen initialization
Adafruit_ILI9341 tft = Adafruit_ILI9341(TFT_CS, TFT_DC, TFT_MOSI, TFT_CLK, TFT_RST, TFT_MISO); // for TZT ESP32 LVGL screen change TFT_CLK to TFT_SCLK

// HTTPS Configuration
const char* host = "www.hamqsl.com";
const int httpsPort = 443;
const String url = "/solarxml.php";

WiFiClientSecure secureClient;
HttpClient http(secureClient, host, httpsPort);

// converting data from XML
String updated = "";
int solarflux = 0;
int aindex = 0;
int kindex = 0;
String kindexnt = "";
String xray = "";
int sunspots = 0;
float heliumline = 0.0;
int protonflux = 0;
int electronflux = 0;
int aurora = 0;
float normalization = 0.0;
float latdegree = 0.0;
float solarwind = 0.0;
float magneticfield = 0.0;
String geomagfield = "";
String signalnoise = "";
String muf = "";

// propagation conditions (Poor=0, Fair=1, Good=2)
int propagation[8] = {3, 3, 3, 3, 3, 3, 3, 3};

// VHF conditions - string table
String vhf_phenomena[5] = {"", "", "", "", ""};
String vhf_locations[5] = {"", "", "", "", ""};
String vhf_conditions[5] = {"", "", "", "", ""};

// function declaration
void connectToWiFi();
bool fetchXML(int retries = 3);
void GetXMLData();
String getXMLValue(const String& xml, const String& tag);
int toConditionValue(String val);
String getAttr(const String& tag, const String& attrName);
void Display();

void setup() {
  Serial.begin(115200);
  pinMode(LED, OUTPUT);
  pinMode(RESET_PIN, INPUT_PULLUP);
  tft.begin();
  tft.setRotation(3);
  //tft.setRotation(2);  // Configuration for TZT ESP32 LVGL screen
  tft.setTextSize(1);
  tft.setFont(&FreeSansBold9pt7b);
  tft.fillScreen(BACKGROUND);
  
  if (digitalRead(RESET_PIN) == LOW) {
    Serial.println("Reset WiFi settings...");
    tft.setCursor(30, 160);
    tft.setTextColor(RED);
    tft.println("Resetting WiFiSettings");
    WiFiManager wifiManager;
    wifiManager.resetSettings();
    delay(2000);
    ESP.restart();
  }

  tft.drawRoundRect(10, 10, 300, 105, 10, WHITE);
  tft.setCursor(65, 37);
  tft.setTextColor(0xFFFF);
  tft.println("HF Propagation report");
  tft.setCursor(65, 55);
  tft.println("canis_lupus - SQ9ZAQ");
  tft.setCursor(48, 73);
  tft.println("Data source: HAMQSL.com");
  tft.setCursor(50, 91);
  tft.println("Serial debug: 115200 baud");
  digitalWrite(LED, 1);

  connectToWiFi();

  tft.setFont(&FreeSansBold12pt7b);
  secureClient.setInsecure();
}

void loop() {
  memset(propagation, 3, sizeof(propagation));
  GetXMLData();
  Display();
  delay(interval * 60 * 1000);
}

void connectToWiFi() {
  WiFi.mode(WIFI_STA); 
  WiFiManager wm;
  //wm.resetSettings();  //uncomment for WiFi Manager reset
  wm.setWiFiAPChannel(6);
  wm.setConfigPortalTimeout(300); // timeout in seconds (5 minut)

  wm.setAPCallback([](WiFiManager* wm) {
    Serial.println("-> Uruchamiam WiFiManager captive portal...");
    tft.setTextColor(RED);
    tft.setCursor(20, 140);
    tft.println("WiFi config mode");
    tft.setCursor(20, 160);
    tft.println("Connect to the Wi-Fi HamQSL");
    tft.setCursor(20, 180);
    tft.println("to configure the connection");
    tft.setCursor(20, 200);
    tft.println("and open 192.168.4.1");
  });

  // Try to connect automatically or show captive portal
  if (!wm.autoConnect("HamQSL")) {
    Serial.println("! Nie połączono z Wi-Fi w ciągu 5 minut. Restartuję ESP32...");
    delay(2000);
    digitalWrite(LED, 0);
    ESP.restart();
  }

  // connection acquired
  Serial.println("✔ Połączono z Wi-Fi!");
  tft.setCursor(95, 160);
  tft.setTextColor(GREEN);
  tft.println("WiFi connected");
  Serial.print("Adres IP: ");
  Serial.println(WiFi.localIP());
  delay(300);
}

bool fetchXML(int retries) {
  for (int i = 0; i < retries; ++i) {
    Serial.println("Pobieranie danych XML...");
    http.get(url);
    int statusCode = http.responseStatusCode();
    if (statusCode == 200) {
      return true;
    }
    delay(1000); // wait before trying to download the data again
  }
  return false;
}

void GetXMLData() {
  if (!fetchXML()) {
    Serial.println("Nie udało się pobrać danych po kilku próbach.");
    delay(2000);
    ESP.restart();
    return;
  }

  String response = http.responseBody();
  //Serial.println(response); // <- Enable to view downloaded XML in serial console

  // Parsing basic data
  updated = getXMLValue(response, "updated");
  solarflux = getXMLValue(response, "solarflux").toInt();
  aindex = getXMLValue(response, "aindex").toInt();
  kindex = getXMLValue(response, "kindex").toInt();
  kindexnt = getXMLValue(response, "kindexnt");
  xray = getXMLValue(response, "xray");
  sunspots = getXMLValue(response, "sunspots").toInt();
  heliumline = getXMLValue(response, "heliumline").toFloat();
  protonflux = getXMLValue(response, "protonflux").toInt();
  electronflux = getXMLValue(response, "electronflux").toInt();
  aurora = getXMLValue(response, "aurora").toInt();
  normalization = getXMLValue(response, "normalization").toFloat();
  latdegree = getXMLValue(response, "latdegree").toFloat();
  solarwind = getXMLValue(response, "solarwind").toFloat();
  magneticfield = getXMLValue(response, "magneticfield").toFloat();
  geomagfield = getXMLValue(response, "geomagfield");
  signalnoise = getXMLValue(response, "signalnoise");
  muf = getXMLValue(response, "muf");

  // Parsing propagation conditions
  int pos = 0;
  while (true) {
    int bandStart = response.indexOf("<band ", pos);
    if (bandStart == -1) break;

    int bandClose = response.indexOf(">", bandStart);
    if (bandClose == -1) break;

    int bandEnd = response.indexOf("</band>", bandClose);
    if (bandEnd == -1) break;

    String tag = response.substring(bandStart, bandClose + 1);
    String name = getAttr(tag, "name");
    String time = getAttr(tag, "time");
    String value = response.substring(bandClose + 1, bandEnd);
    value.trim();

    int numeric = toConditionValue(value);

    if (name == "80m-40m" && time == "day") propagation[0] = numeric;
    if (name == "30m-20m" && time == "day") propagation[1] = numeric;
    if (name == "17m-15m" && time == "day") propagation[2] = numeric;
    if (name == "12m-10m" && time == "day") propagation[3] = numeric;
    if (name == "80m-40m" && time == "night") propagation[4] = numeric;
    if (name == "30m-20m" && time == "night") propagation[5] = numeric;
    if (name == "17m-15m" && time == "night") propagation[6] = numeric;
    if (name == "12m-10m" && time == "night") propagation[7] = numeric;

    pos = bandEnd + 7;
  }

  // Parsing VHF Conditions
  pos = 0;
  int vhfIndex = 0;
  while (true && vhfIndex < 5) {
    int phenStart = response.indexOf("<phenomenon ", pos);
    if (phenStart == -1) break;

    int phenClose = response.indexOf(">", phenStart);
    if (phenClose == -1) break;

    int phenEnd = response.indexOf("</phenomenon>", phenClose);
    if (phenEnd == -1) break;

    String tag = response.substring(phenStart, phenClose + 1);
    vhf_phenomena[vhfIndex] = getAttr(tag, "name");
    vhf_locations[vhfIndex] = getAttr(tag, "location");
    vhf_conditions[vhfIndex] = response.substring(phenClose + 1, phenEnd);
    vhf_conditions[vhfIndex].trim();

    pos = phenEnd + 13;
    vhfIndex++;
  }

  // Displaying all data in Serial Monitor
  Serial.println("=== DANE SOLARNE ===");
  Serial.println("Updated: " + updated);
  Serial.println("Solar Flux: " + String(solarflux));
  Serial.println("A-Index: " + String(aindex));
  Serial.println("K-Index: " + String(kindex));
  Serial.println("K-Index NT: " + kindexnt);
  Serial.println("X-Ray: " + xray);
  Serial.println("Sunspots: " + String(sunspots));
  Serial.println("Helium Line: " + String(heliumline));
  Serial.println("Proton Flux: " + String(protonflux));
  Serial.println("Electron Flux: " + String(electronflux));
  Serial.println("Aurora: " + String(aurora));
  Serial.println("Normalization: " + String(normalization));
  Serial.println("Lat Degree: " + String(latdegree));
  Serial.println("Solar Wind: " + String(solarwind));
  Serial.println("Magnetic Field: " + String(magneticfield));
  Serial.println("Geomagnetic Field: " + geomagfield);
  Serial.println("Signal Noise: " + signalnoise);
  Serial.println("MUF: " + muf);

  Serial.println("=== WARUNKI VHF ===");
  for (int i = 0; i < 5; i++) {
    if (vhf_phenomena[i] != "") {
      Serial.println(vhf_phenomena[i] + " (" + vhf_locations[i] + "): " + vhf_conditions[i]);
    }
  }
}

String getXMLValue(const String& xml, const String& tag) {
  String openTag = "<" + tag + ">";
  String closeTag = "</" + tag + ">";

  int start = xml.indexOf(openTag);
  if (start == -1) return "";

  start += openTag.length();
  int end = xml.indexOf(closeTag, start);
  if (end == -1) return "";

  String value = xml.substring(start, end);
  value.trim();
  return value;
}

int toConditionValue(String val) {
  val.toLowerCase();
  if (val == "good") return 2;
  if (val == "fair") return 1;
  if (val == "poor") return 0;
  return -1;
}

String getAttr(const String& tag, const String& attrName) {
  String search = attrName + "=\"";
  int attrStart = tag.indexOf(search);
  if (attrStart == -1) return "";
  attrStart += search.length();
  int attrEnd = tag.indexOf("\"", attrStart);
  if (attrEnd == -1) return "";
  return tag.substring(attrStart, attrEnd);
}

bool isGreaterThanM5(String val) {
  if (val.length() < 2) return false;

  char letter = val.charAt(0);
  float number = val.substring(1).toFloat();

  return (letter >= 'M' && number >= 5.0);
}

void Display() {
  tft.setFont(&FreeSansBold9pt7b);
  digitalWrite(LED, 0);
  tft.fillScreen(BACKGROUND);
  digitalWrite(LED, 1);
  tft.setTextColor(WHITE);
  tft.setCursor(66, 22);
  tft.println("DAY");
  tft.setCursor(201, 22);
  tft.println("NIGHT");
  tft.setFont(&FreeSansBold12pt7b);
  tft.drawRoundRect(5, 3, 310, 230, 10, WHITE);
  tft.drawLine(15, 132, 305, 132, WHITE);

  for (int i = 0; i < 8; i++) {
    switch (propagation[i]) {
      case 0:
        tft.setTextColor(RED);
        break;
      case 1:
        tft.setTextColor(YELLOW);
        break;
      case 2:
        tft.setTextColor(GREEN);
        break;
    }

    switch (i) {
      case 0:
        tft.setCursor(0 * column + 34, 1 * row + row_offset);
        tft.printf("80m-40m");
        Serial.printf("Day 80m-40m: %d\n", propagation[i]);
        break;
      case 1:
        tft.setCursor(0 * column + 34, 2 * row + row_offset);
        tft.printf("30m-20m");
        Serial.printf("Day 30m-20m: %d\n", propagation[i]);
        break;
      case 2:
        tft.setCursor(0 * column + 32, 3 * row + row_offset);
        tft.printf("17m-15m");
        Serial.printf("Day 17m-15m: %d\n", propagation[i]);
        break;
      case 3:
        tft.setCursor(0 * column + 32, 4 * row + row_offset);
        tft.printf("12m-10m");
        Serial.printf("Day 12m-10m: %d\n", propagation[i]);
        break;
      case 4:
        tft.setCursor(1 * column + 34, 1 * row + row_offset);
        tft.printf("80m-40m");
        Serial.printf("Night 80m-40m: %d\n", propagation[i]);
        break;
      case 5:
        tft.setCursor(1 * column + 34, 2 * row + row_offset);
        tft.printf("30m-20m");
        Serial.printf("Night 30m-20m: %d\n", propagation[i]);
        break;
      case 6:
        tft.setCursor(1 * column + 32, 3 * row + row_offset);
        tft.printf("17m-15m");
        Serial.printf("Night 17m-15m: %d\n", propagation[i]);
        break;
      case 7:
        tft.setCursor(1 * column + 32, 4 * row + row_offset);
        tft.printf("12m-10m");
        Serial.printf("Night 12m-10m: %d\n", propagation[i]);
        break;
    }
  }
  
  tft.setFont(&FreeSans9pt7b);
  tft.setTextColor(WHITE);
  tft.setCursor(17, 155);
  tft.print("K-Index:     ");
  if (kindex >= 0 && kindex <= 2) {
    tft.setTextColor(GREEN);
  } else if (kindex >= 3 && kindex <= 5) {
    tft.setTextColor(YELLOW);
  } else if (kindex >= 6 && kindex <= 9) {
    tft.setTextColor(RED);
  }
  tft.println(String(kindex));

  tft.setTextColor(WHITE);
  tft.setCursor(17, 1 * s_row + s_row_offset);
  tft.print("A-Index:     ");
    if (aindex >= 0 && aindex <= 10) {
    tft.setTextColor(GREEN);
  } else if (aindex >= 11 && aindex <= 50) {
    tft.setTextColor(YELLOW);
  } else if (aindex >= 51 && aindex <= 900) {
    tft.setTextColor(RED);
  }
  tft.println(String(aindex));
  
  tft.setCursor(17, 2 * s_row + s_row_offset);
  tft.setTextColor(WHITE);
  tft.print("Solar Flux: ");
  if (solarflux > 150) {
    tft.setTextColor(GREEN);
  } else {tft.setTextColor(WHITE);}
  tft.println(String(solarflux));
  
  tft.setCursor(17, 3 * s_row + s_row_offset);
  tft.setTextColor(WHITE);
  tft.print("Sunspots:  ");
  if (sunspots > 105) {
    tft.setTextColor(GREEN);
  } else {tft.setTextColor(WHITE);}
  tft.println(String(sunspots));

  tft.setCursor(160, 155);
  tft.setTextColor(WHITE);
  tft.print("2m: " );
  if (vhf_conditions[1] != "Band Closed") {
    tft.setTextColor(GREEN);
  } else {
    tft.setTextColor(WHITE);
  }
  tft.print(vhf_conditions[1]);
  
  tft.setCursor(160, 1 * s_row + s_row_offset);
  tft.setTextColor(WHITE);
  tft.print("6m: ");
  if (vhf_conditions[3] != "Band Closed") {
    tft.setTextColor(GREEN);
  } else {
    tft.setTextColor(WHITE);
  }
  tft.print(vhf_conditions[3]);
  
  tft.setCursor(160, 2 * s_row + s_row_offset);
  tft.setTextColor(WHITE);
  tft.print("4m: ");
  if (vhf_conditions[4] != "Band Closed") {
    tft.setTextColor(GREEN);
  } else {
    tft.setTextColor(WHITE);
  }
  tft.println(vhf_conditions[4]);
  
  tft.setCursor(160, 3 * s_row + s_row_offset);
  tft.setTextColor(WHITE);
  tft.print("X-Ray:       ");
  if (isGreaterThanM5(xray)) {
    tft.setTextColor(RED);
  } else {tft.setTextColor(WHITE);}
  tft.println(xray);
}
