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

#define RED 0xf882
#define YELLOW 0xff80
#define GREEN 0x07e0
#define WHITE 0xFFFF
#define BACKGROUND 0x0000
#define column 145
#define row 25
#define row_offset 20
#define s_column 140
#define s_column_offset 15
#define s_row 20
#define s_row_offset 155
#define interval 60 // Interwał w minutach co jaki czas są pobierane dane z serwera
#define LED 22

Adafruit_ILI9341 tft = Adafruit_ILI9341(12, 14, 27, 25, 26, 33);

// HTTPS config
const char* host = "www.hamqsl.com";
const int httpsPort = 443;
const String url = "/solarxml.php";

WiFiClientSecure secureClient;
HttpClient http(secureClient, host, httpsPort);

// Zmienne na wszystkie dane z XML
String updated = "";
int solarflux = 0;
int aindex = 0;
int kindex = 0;
String kindexnt = "";
String xray = "";
int sunspots = 0;
float heliumline = 0.0;
int protonflux = 0;
int electonflux = 0;
int aurora = 0;
float normalization = 0.0;
float latdegree = 0.0;
float solarwind = 0.0;
float magneticfield = 0.0;
String geomagfield = "";
String signalnoise = "";
String muf = "";

// Warunki propagacji (Poor=0, Fair=1, Good=2)
int propagation[8] = {3, 3, 3, 3, 3, 3, 3, 3};

// Warunki VHF - tablice stringów
String vhf_phenomena[5] = {"", "", "", "", ""};
String vhf_locations[5] = {"", "", "", "", ""};
String vhf_conditions[5] = {"", "", "", "", ""};

void setup() {
  Serial.begin(115200);
  pinMode(LED, OUTPUT);
  delay(1000);
  tft.begin();
  tft.setRotation(3);
  tft.setTextSize(1);
  tft.setFont(&FreeSansBold9pt7b);
  tft.fillScreen(BACKGROUND);
  tft.drawRoundRect(10,30,310,160,10,WHITE);
  tft.setCursor(65, 67);
  tft.setTextColor(0xFFFF);
  tft.println("HF Propagation report");
  tft.setCursor(65, 85);
  tft.println("canis_lupus - SQ9ZAQ");
  tft.setCursor(48, 103);
  tft.println("Data source: HAMQSL.com");
  tft.setCursor(50, 121);
  tft.println("Serial debug: 115200 baud");
  digitalWrite(LED, 1);

  bool wm_nonblocking = true;
  WiFiManager wm;
  wm.setConnectTimeout(20);
  bool res;
  res = wm.autoConnect("WiFi Config Portal");
  if(!res) {
    Serial.println("Failed to connect");
  } 
  else {
    Serial.println("connected...yeey :)");
    tft.setCursor(50, 162);
    tft.println("WiFi Connected...");
  }
  tft.setFont(&FreeSansBold12pt7b);
  secureClient.setInsecure();  
}

void loop() {
  memset(propagation, 3, sizeof(propagation));
  GetXMLData();
  Display();
  delay(interval*60*1000);
}

void GetXMLData() {
  Serial.println("Pobieranie danych XML...");
  http.get(url);
  int statusCode = http.responseStatusCode();
  String response = http.responseBody();
  Serial.println(response);
  
  if (statusCode != 200) {
    Serial.println("Błąd podczas pobierania danych.");
    return;
  }

  // Parsowanie podstawowych danych
  updated = getXMLValue(response, "updated");
  solarflux = getXMLValue(response, "solarflux").toInt();
  aindex = getXMLValue(response, "aindex").toInt();
  kindex = getXMLValue(response, "kindex").toInt();
  kindexnt = getXMLValue(response, "kindexnt");
  xray = getXMLValue(response, "xray");
  sunspots = getXMLValue(response, "sunspots").toInt();
  heliumline = getXMLValue(response, "heliumline").toFloat();
  protonflux = getXMLValue(response, "protonflux").toInt();
  electonflux = getXMLValue(response, "electonflux").toInt();
  aurora = getXMLValue(response, "aurora").toInt();
  normalization = getXMLValue(response, "normalization").toFloat();
  latdegree = getXMLValue(response, "latdegree").toFloat();
  solarwind = getXMLValue(response, "solarwind").toFloat();
  magneticfield = getXMLValue(response, "magneticfield").toFloat();
  geomagfield = getXMLValue(response, "geomagfield");
  signalnoise = getXMLValue(response, "signalnoise");
  muf = getXMLValue(response, "muf");

  // Parsowanie warunków propagacji
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

  // Parsowanie warunków VHF
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

  // Wyświetlanie wszystkich danych w Serial Monitor
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
  Serial.println("Electron Flux: " + String(electonflux));
  Serial.println("Aurora: " + String(aurora));
  Serial.println("Normalization: " + String(normalization));
  Serial.println("Lat Degree: " + String(latdegree));
  Serial.println("Solar Wind: " + String(solarwind));
  Serial.println("Magnetic Field: " + String(magneticfield));
  Serial.println("Geomagnetic Field: " + geomagfield);
  Serial.println("Signal Noise: " + signalnoise);
  Serial.println("MUF: " + muf);
  
  Serial.println("=== WARUNKI VHF ===");
  for(int i = 0; i < 5; i++) {
    if(vhf_phenomena[i] != "") {
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

void Display() {
  tft.setFont(&FreeSansBold9pt7b);
  digitalWrite(LED, 0);
  tft.fillScreen(BACKGROUND);
  digitalWrite(LED, 1);
  tft.setTextColor(WHITE);
  tft.setCursor(62, 23);
  tft.println("DAY");
  tft.setCursor(197, 23);
  tft.println("NIGHT");
  tft.setFont(&FreeSansBold12pt7b);
  tft.drawRoundRect(5,3,310,230,10, WHITE);
  tft.drawLine(15, 132, 305, 132, WHITE);

  for (int i = 0; i < 8; i++) {
    switch(propagation[i]) {
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
    
    switch(i) {
      case 0:
        tft.setCursor(0*column+32, 1*row+row_offset);
        tft.printf("80m-40m");
        Serial.printf("Day 80m-40m: %d\n", propagation[i]);
        break;
      case 1:
        tft.setCursor(0*column+32, 2*row+row_offset);
        tft.printf("30m-20m");
        Serial.printf("Day 30m-20m: %d\n", propagation[i]);
        break;
      case 2:
        tft.setCursor(0*column+32, 3*row+row_offset);
        tft.printf("17m-15m");
        Serial.printf("Day 17m-15m: %d\n", propagation[i]);
        break;
      case 3:
        tft.setCursor(0*column+32, 4*row+row_offset);
        tft.printf("12m-10m");
        Serial.printf("Day 12m-10m: %d\n", propagation[i]);
        break;
      case 4:
        tft.setCursor(1*column+32, 1*row+row_offset);
        tft.printf("80m-40m");
        Serial.printf("Night 80m-40m: %d\n", propagation[i]);
        break;
      case 5:
        tft.setCursor(1*column+32, 2*row+row_offset);
        tft.printf("30m-20m");
        Serial.printf("Night 30m-20m: %d\n", propagation[i]);
        break;
      case 6:
        tft.setCursor(1*column+32, 3*row+row_offset);
        tft.printf("17m-15m");
        Serial.printf("Night 17m-15m: %d\n", propagation[i]);
        break;
      case 7:
        tft.setCursor(1*column+32, 4*row+row_offset);
        tft.printf("12m-10m");
        Serial.printf("Night 12m-10m: %d\n", propagation[i]);
        break;
    }
  }
  tft.setFont(&FreeSans9pt7b);
  tft.setTextColor(WHITE);
  tft.setCursor(17, 155);
  tft.println("Solar Flux: " + String(solarflux));
  tft.setCursor(17, 1*s_row+s_row_offset);
  tft.println("A-Index:     " + String(aindex));
  tft.setCursor(17, 2*s_row+s_row_offset);
  tft.println("K-Index:     " + String(kindex));
  tft.setCursor(17, 3*s_row+s_row_offset);
  tft.println("Sunspots:   " + String(sunspots));
  
  tft.setCursor(160, 155);
  if (vhf_conditions[1] != "Band Closed"){
    tft.setTextColor(GREEN);}
    else {tft.setTextColor(WHITE);}
  tft.println("2m: " + vhf_conditions[1]);
  tft.setCursor(160, 1*s_row+s_row_offset);
    if (vhf_conditions[3] != "Band Closed"){
    tft.setTextColor(GREEN);}
    else {tft.setTextColor(WHITE);}
  tft.println("6m: " + vhf_conditions[3]);
  tft.setCursor(160, 2*s_row+s_row_offset);
    if (vhf_conditions[4] != "Band Closed"){
    tft.setTextColor(GREEN);}
    else {tft.setTextColor(WHITE);}
  tft.println("4m: " + vhf_conditions[4]);
  tft.setCursor(160, 3*s_row+s_row_offset);
  tft.setTextColor(WHITE);
  tft.println("X-Ray:    " + xray);
}
