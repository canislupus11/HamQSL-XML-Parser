#include <WiFi.h>
#include <WiFiManager.h>
#include <WiFiClientSecure.h>
#include <ArduinoHttpClient.h>

#include "SPI.h"
#include "Adafruit_GFX.h"
#include "Adafruit_ILI9341.h"
#include "Fonts/FreeSansBold9pt7b.h"
#include "Fonts/FreeSansBold12pt7b.h"

#define RED 0xf882
#define YELLOW 0xff80
#define GREEN 0x07e0
#define WHITE 0xFFFF
#define column 145
#define row 30
#define interval 60 // <-- Interwał w minutach co jaki czas są poierane dane z serwera
#define LED 22

Adafruit_ILI9341 tft = Adafruit_ILI9341(12, 14, 27, 25, 26, 33);
  

// HTTPS config
const char* host = "www.hamqsl.com";
const int httpsPort = 443;
const String url = "/solarxml.php";

WiFiClientSecure secureClient;
HttpClient http(secureClient, host, httpsPort);

// Globalne zmienne na warunki (Poor=0, Fair=1, Good=2)

int propagation[8] = {3, 3, 3, 3, 3, 3, 3, 3};


void setup() {
  Serial.begin(115200);
  pinMode(LED, OUTPUT);
  delay(1000);
  tft.begin();
  tft.setRotation(3);
  tft.setTextSize(1);
  tft.setFont(&FreeSansBold9pt7b);
  tft.fillScreen(ILI9341_BLACK);
  digitalWrite(LED,1);
  tft.setCursor(60, 67);
  tft.setTextColor(0xFFFF);
  tft.println("HF Propagation report");
  tft.setCursor(60, 85);
  tft.println("canis_lupus - SQ9ZAQ");
  tft.setCursor(40, 115);
  tft.println("Serial debug: 115200 baud");

 

  bool wm_nonblocking = true;
  WiFiManager wm;
  wm.setConnectTimeout(20);
    bool res;
    res = wm.autoConnect("AutoConnectAP"); // password protected ap
    if(!res) {
        Serial.println("Failed to connect");
    } 
    else {
        //if you get here you have connected to the WiFi    
        Serial.println("connected...yeey :)");
        tft.println("WiFi Connected");
  }
tft.setFont(&FreeSansBold12pt7b);
 


  secureClient.setInsecure();  

}

void loop() {
  GetXMLData();  // <-- Pobranie i przetworzenie danych
  Display();
  delay(interval*60*1000);
}

// === Główna funkcja pobierająca dane i zapisująca do zmiennych ===
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

  int pos = 0;
  while (true) {
    int bandStart = response.indexOf("<band ", pos);
    if (bandStart == -1) break;

    int bandClose = response.indexOf(">", bandStart);
    if (bandClose == -1) break;

    int bandEnd = response.indexOf("</band>", bandClose);
    if (bandEnd == -1) break;

    String tag = response.substring(bandStart, bandClose + 1);
    String name = getAttr(tag, "name");  // np. "80m-40m"
    String time = getAttr(tag, "time");  // "day" lub "night"  digitalWrite(22,1);
    String value = response.substring(bandClose + 1, bandEnd);
    value.trim();

    int numeric = toConditionValue(value);
    // Przypisanie do odpowiedniej zmiennej
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
}

// === Pomocnicze: konwertuje "Good"/"Fair"/"Poor" => 2/1/0 ===
int toConditionValue(String val) {
  val.toLowerCase();
  if (val == "good") return 2;
  if (val == "fair") return 1;
  if (val == "poor") return 0;
  return -1;  // Błąd
}

// === Pomocnicze: pobiera wartość atrybutu z tagu XML ===
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
  digitalWrite(LED,0);
  tft.fillScreen(ILI9341_BLACK);
  tft.setTextColor(WHITE);
  tft.setCursor(58, 20);
  tft.println("DAY");
  tft.setCursor(193, 20);
  tft.println("NIGHT");

  for ( int i=0 ; i<8 ; i++) {
    switch(propagation[i]){
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
    switch(i){
      case 0:
        tft.setCursor(0*column+32, 1*row+25);
        tft.printf("80m-40m");
        Serial.printf("Day 80m-40m: %d\n", propagation[i]);
        break;
      case 1:
        tft.setCursor(0*column+32, 2*row+25);
        tft.printf("30m-20m");
        Serial.printf("Day 30m-20m: %d\n", propagation[i]);
        break;
      case 2:
        tft.setCursor(0*column+32, 3*row+25);
        tft.printf("17m-15m");
        Serial.printf("Day 17m-15m: %d\n", propagation[i]);
        break;
      case 3:
        tft.setCursor(0*column+32, 4*row+25);
        tft.printf("12m-10m");
        Serial.printf("Day 12m-10m: %d\n", propagation[i]);
        break;
      case 4:
        tft.setCursor(1*column+32, 1*row+25);
        tft.printf("80m-40m");
        Serial.printf("Night 80m-40m: %d\n", propagation[i]);
        break;
      case 5:
        tft.setCursor(1*column+32, 2*row+25);
        tft.printf("30m-20m");
        Serial.printf("Night 30m-20m: %d\n", propagation[i]);
        break;
      case 6:
        tft.setCursor(1*column+32, 3*row+25);
        tft.printf("17m-15m");
        Serial.printf("Night 17m-15m: %d\n", propagation[i]);
        break;
      case 7:
        tft.setCursor(1*column+32, 4*row+25);
        tft.printf("12m-10m");
        Serial.printf("Night 12m-10m: %d\n", propagation[i]);
        break;
    }
  }; 
 digitalWrite(LED,1);

}
