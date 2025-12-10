#include <WiFi.h>
#include <WiFiManager.h>
#include <WiFiClientSecure.h>
#include <ArduinoHttpClient.h>
#include <TFT_eSPI.h>   
#include <SPI.h>

// Color Define
#define RED     0xf904
//#define RED 0x001F  // Configuration for TZT ESP32 LVGL screen
#define YELLOW  0xffe0
//#define YELLOW 0x07FF  // Configuration for TZT ESP32 LVGL screen
#define GREEN   0x07e0
#define WHITE   0xFFFF
#define BACKGROUND 0x0000


TFT_eSPI tft = TFT_eSPI();

// LED
#define LED 22
#define RESET_PIN 32

// define element position on the screen
#define column 145
#define row 25
#define row_offset 23
#define s_column 140
#define s_column_offset 15
#define s_row 20
#define s_row_offset 144

// Refresh interval
#define interval 60 // interval in minutes for getting data from the server

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
int propagation[8] = {3,3,3,3,3,3,3,3};

// VHF conditions - string table
String vhf_phenomena[5];
String vhf_locations[5];
String vhf_conditions[5];

// function declaration
void connectToWiFi();
bool fetchXML(int retries = 3);
void GetXMLData();
String getXMLValue(const String& xml, const String& tag);
int toConditionValue(String val);
String getAttr(const String& tag, const String& attrName);
void Display();
bool isGreaterThanM5(String val);


void setup() {
  Serial.begin(115200);
  pinMode(LED, OUTPUT);
  pinMode(RESET_PIN, INPUT_PULLUP);
  tft.init();
  tft.setRotation(3);
  tft.fillScreen(BACKGROUND);
  tft.setFreeFont(&FreeSansBold9pt7b);

  if (digitalRead(RESET_PIN) == LOW) {
    Serial.println("Reset WiFi settings...");
    tft.setTextColor(RED);
    tft.drawString("Resetting WiFiSettings", 30, 160);
    WiFiManager wifiManager;
    wifiManager.resetSettings();
    delay(2000);
    ESP.restart();
  }

  tft.drawRoundRect(10, 10, 300, 105, 10, WHITE);
  tft.setTextColor(WHITE);
  tft.drawString("HF Propagation report", 65, 26);
  tft.drawString("canis_lupus - SQ9ZAQ", 65, 44);
  tft.drawString("Data source: HAMQSL.com", 48, 62);
  tft.drawString("Serial debug: 115200 baud", 50, 80);

  digitalWrite(LED, 1);

  connectToWiFi();

  tft.setFreeFont(&FreeSansBold12pt7b);

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
    tft.drawString("WiFi config mode", 20, 130);
    tft.drawString("Please connect to HamQSL WiFi", 20, 150);
    tft.drawString("and open 192.168.4.1", 20, 170);
  });

  // Try to connect automatically or show captive portal
  if (!wm.autoConnect("HamQSL")) {
    Serial.println("! Nie połączono z Wi-Fi w ciągu 5 minut. Restartuję ESP32...");
    delay(2000);
    digitalWrite(LED, 0);
    ESP.restart();
  }

  tft.setTextColor(GREEN);
  tft.drawString("WiFi connected", 95, 150);
  Serial.println("Połączono z Wi-Fi!");
  Serial.print("Adres IP: ");
  Serial.println(WiFi.localIP());
  delay(300);
}


bool fetchXML(int retries) {
  for (int i=0; i<retries; i++) {
    Serial.println("Pobieranie danych XML...");
    http.get(url);
    if (http.responseStatusCode() == 200) return true;
    delay(1000);
  }
  return false;
}



void GetXMLData() {
  if (!fetchXML()) {
    Serial.println("Nie udało się pobrać danych po kilku próbach.");
    delay(2000);
    ESP.restart();
  }

  String response = http.responseBody();
  //Serial.println(response); // <- Enable to view downloaded XML in serial console

  // Parsing basic data
  updated = getXMLValue(response,"updated");
  solarflux = getXMLValue(response,"solarflux").toInt();
  aindex    = getXMLValue(response,"aindex").toInt();
  kindex    = getXMLValue(response,"kindex").toInt();
  kindexnt  = getXMLValue(response,"kindexnt");
  xray      = getXMLValue(response,"xray");
  sunspots  = getXMLValue(response,"sunspots").toInt();
  heliumline= getXMLValue(response,"heliumline").toFloat();
  protonflux= getXMLValue(response,"protonflux").toInt();
  electronflux=getXMLValue(response,"electronflux").toInt();
  aurora    = getXMLValue(response,"aurora").toInt();
  normalization=getXMLValue(response,"normalization").toFloat();
  latdegree = getXMLValue(response,"latdegree").toFloat();
  solarwind = getXMLValue(response,"solarwind").toFloat();
  magneticfield=getXMLValue(response,"magneticfield").toFloat();
  geomagfield = getXMLValue(response,"geomagfield");
  signalnoise = getXMLValue(response,"signalnoise");
  muf = getXMLValue(response,"muf");

  // Parsing HF propagation conditions
  int pos = 0;
  while (true) {
    int s = response.indexOf("<band ", pos);
    if (s == -1) break;
    int c = response.indexOf(">", s);
    int e = response.indexOf("</band>", c);

    String tag = response.substring(s,c+1);
    String name = getAttr(tag,"name");
    String time = getAttr(tag,"time");

    String val = response.substring(c+1, e);
    val.trim();

    int numeric = toConditionValue(val);

    if (name=="80m-40m" && time=="day") propagation[0]=numeric;
    if (name=="30m-20m" && time=="day") propagation[1]=numeric;
    if (name=="17m-15m" && time=="day") propagation[2]=numeric;
    if (name=="12m-10m" && time=="day") propagation[3]=numeric;

    if (name=="80m-40m" && time=="night") propagation[4]=numeric;
    if (name=="30m-20m" && time=="night") propagation[5]=numeric;
    if (name=="17m-15m" && time=="night") propagation[6]=numeric;
    if (name=="12m-10m" && time=="night") propagation[7]=numeric;

    pos = e + 7;
  }

  // Parsing VHF Conditions
  pos = 0;
  int vhfIndex = 0;
  while (vhfIndex < 5) {
    int phenStart = response.indexOf("<phenomenon ", pos);
    if (phenStart == -1) break;

    int phenClose = response.indexOf(">", phenStart);
    if (phenClose == -1) break;

    int phenEnd = response.indexOf("</phenomenon>", phenClose);
    if (phenEnd == -1) break;

    String tag = response.substring(phenStart,phenClose+1);

    vhf_phenomena[vhfIndex] = getAttr(tag,"name");
    vhf_locations[vhfIndex] = getAttr(tag,"location");
    vhf_conditions[vhfIndex] = response.substring(phenClose+1,phenEnd);
    vhf_conditions[vhfIndex].trim();

    pos = phenEnd+13;
    vhfIndex++;
  }
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
  String o = "<"+tag+">";
  String c = "</"+tag+">";
  int s = xml.indexOf(o);
  if (s==-1) return "";
  s+=o.length();
  int e = xml.indexOf(c,s);
  if (e==-1) return "";
  String v = xml.substring(s,e);
  v.trim();
  return v;
}

int toConditionValue(String v){
  v.toLowerCase();
  if (v=="good") return 2;
  if (v=="fair") return 1;
  if (v=="poor") return 0;
  return -1;
}

String getAttr(const String& tag, const String& attrName){
  String s = attrName+"=\"";
  int a = tag.indexOf(s);
  if (a==-1) return "";
  a+=s.length();
  int b = tag.indexOf("\"", a);
  return tag.substring(a,b);
}

bool isGreaterThanM5(String v){
  if (v.length() < 2) return false;
  char L = v.charAt(0);
  float num = v.substring(1).toFloat();
  return (L>='M' && num>=5.0);
}


  //DISPLAY
void Display() {
  //digitalWrite(LED, 0);
  tft.fillScreen(BACKGROUND);
  //digitalWrite(LED, 1);

  tft.setFreeFont(&FreeSansBold9pt7b);
  tft.setTextColor(WHITE);
  tft.drawString("DAY", 66, 10);
  tft.drawString("NIGHT", 201, 10);

  tft.setFreeFont(&FreeSansBold12pt7b);
  tft.drawRoundRect(5, 3, 310, 230, 10, WHITE);
  tft.drawLine(15, 135, 305, 135, WHITE);

  // HF
for (int i = 0; i < 8; i++) {

    // Kolor tekstu – TFT_eSPI używa identycznych funkcji
    switch (propagation[i]) {
      case 0:
        tft.setTextColor(RED, TFT_BLACK);   // drugi parametr = kolor tła (opcjonalnie)
        break;
      case 1:
        tft.setTextColor(YELLOW, TFT_BLACK);
        break;
      case 2:
        tft.setTextColor(GREEN, TFT_BLACK);
        break;
    }

    switch (i) {
      case 0:
        tft.setCursor(0 * column + 34, 1 * row + row_offset);
        tft.print("80m-40m");
        Serial.printf("Day 80m-40m: %d\n", propagation[i]);
        break;

      case 1:
        tft.setCursor(0 * column + 34, 2 * row + row_offset);
        tft.print("30m-20m");
        Serial.printf("Day 30m-20m: %d\n", propagation[i]);
        break;

      case 2:
        tft.setCursor(0 * column + 32, 3 * row + row_offset);
        tft.print("17m-15m");
        Serial.printf("Day 17m-15m: %d\n", propagation[i]);
        break;

      case 3:
        tft.setCursor(0 * column + 32, 4 * row + row_offset);
        tft.print("12m-10m");
        Serial.printf("Day 12m-10m: %d\n", propagation[i]);
        break;

      case 4:
        tft.setCursor(1 * column + 34, 1 * row + row_offset);
        tft.print("80m-40m");
        Serial.printf("Night 80m-40m: %d\n", propagation[i]);
        break;

      case 5:
        tft.setCursor(1 * column + 34, 2 * row + row_offset);
        tft.print("30m-20m");
        Serial.printf("Night 30m-20m: %d\n", propagation[i]);
        break;

      case 6:
        tft.setCursor(1 * column + 32, 3 * row + row_offset);
        tft.print("17m-15m");
        Serial.printf("Night 17m-15m: %d\n", propagation[i]);
        break;

      case 7:
        tft.setCursor(1 * column + 32, 4 * row + row_offset);
        tft.print("12m-10m");
        Serial.printf("Night 12m-10m: %d\n", propagation[i]);
        break;
    }
}

 // Solar data
  tft.setFreeFont(&FreeSans9pt7b);

  tft.setTextColor(WHITE);
  tft.drawString("K-Index:", 17, s_row_offset);
  if      (kindex<=2) tft.setTextColor(GREEN);
  else if (kindex<=5) tft.setTextColor(YELLOW);
  else                tft.setTextColor(RED);
  tft.drawString(String(kindex), 112, s_row_offset);

  tft.setTextColor(WHITE);
  tft.drawString("A-Index:", 17, 1 * s_row + s_row_offset);
  if      (aindex<=10) tft.setTextColor(GREEN);
  else if (aindex<=50) tft.setTextColor(YELLOW);
  else                 tft.setTextColor(RED);
  tft.drawString(String(aindex), 112, 1 * s_row + s_row_offset);

  tft.setTextColor(WHITE);
  tft.drawString("Solar Flux:", 17, 2 * s_row + s_row_offset);
  if (solarflux > 150) tft.setTextColor(GREEN);
  else tft.setTextColor(WHITE);
  tft.drawString(String(solarflux), 112, 2 * s_row + s_row_offset);

  tft.setTextColor(WHITE);
  tft.drawString("Sunspots:", 17, 3 * s_row + s_row_offset);
  if (sunspots > 105) tft.setTextColor(GREEN);
  else tft.setTextColor(WHITE);
  tft.drawString(String(sunspots), 112, 3 * s_row + s_row_offset);


  // VHF
  tft.setTextColor(WHITE);
  tft.drawString("2m:", 160, s_row_offset);
  tft.setTextColor(vhf_conditions[1]!="Band Closed"?GREEN:WHITE);
  tft.drawString(vhf_conditions[1], 200, s_row_offset);

  tft.setTextColor(WHITE);
  tft.drawString("6m:", 160, 1 * s_row + s_row_offset);
  tft.setTextColor(vhf_conditions[3]!="Band Closed"?GREEN:WHITE);
  tft.drawString(vhf_conditions[3], 200, 1 * s_row + s_row_offset);

  tft.setTextColor(WHITE);
  tft.drawString("4m:", 160, 2 * s_row + s_row_offset);
  tft.setTextColor(vhf_conditions[4]!="Band Closed"?GREEN:WHITE);
  tft.drawString(vhf_conditions[4], 200, 2 * s_row + s_row_offset);

  tft.setTextColor(WHITE);
  tft.drawString("X-Ray:", 160, 3 * s_row + s_row_offset);
  tft.setTextColor(isGreaterThanM5(xray)?RED:WHITE);
  tft.drawString(xray, 220, 3 * s_row + s_row_offset);
}
