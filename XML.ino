#include <WiFi.h>
#include <WiFiManager.h>
#include <WiFiClientSecure.h>
#include <ArduinoHttpClient.h>

// HTTPS config
const char* host = "www.hamqsl.com";
const int httpsPort = 443;
const String url = "/solarxml.php";

WiFiClientSecure secureClient;
HttpClient http(secureClient, host, httpsPort);

// Globalne zmienne na warunki (Poor=0, Fair=1, Good=2)
int band_80m_40m_day;
int band_30m_20m_day;
int band_17m_15m_day;
int band_12m_10m_day;
int band_80m_40m_night;
int band_30m_20m_night;
int band_17m_15m_night;
int band_12m_10m_night;
int interval=5; // <-- Interwał w minutach co jaki czas są poierane dane z serwera

void setup() {
  Serial.begin(115200);
  delay(1000);

  WiFiManager wm;
  if (!wm.autoConnect("HF Ppopagation Config")) {
    Serial.println("Nie udało się połączyć z WiFi");
    ESP.restart();
  }

  Serial.println("Połączono z WiFi.");
  secureClient.setInsecure();  
}

void loop() {
  GetXMLData();  // <-- Pobranie i przetworzenie danych

  Serial.println("=== Warunki dzienne (0=Poor, 1=Fair, 2=Good) ===");
  Serial.printf("80m-40m: %d\n", band_80m_40m_day);
  Serial.printf("30m-20m: %d\n", band_30m_20m_day);
  Serial.printf("17m-15m: %d\n", band_17m_15m_day);
  Serial.printf("12m-10m: %d\n", band_12m_10m_day);

  Serial.println("=== Warunki nocne ===");
  Serial.printf("80m-40m: %d\n", band_80m_40m_night);
  Serial.printf("30m-20m: %d\n", band_30m_20m_night);
  Serial.printf("17m-15m: %d\n", band_17m_15m_night);
  Serial.printf("12m-10m: %d\n", band_12m_10m_night);
  delay(interval*60*1000);
}

// === Główna funkcja pobierająca dane i zapisująca do zmiennych ===
void GetXMLData() {
  Serial.println("Pobieranie danych XML...");
  http.get(url);
  int statusCode = http.responseStatusCode();
  String response = http.responseBody();

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
    String time = getAttr(tag, "time");  // "day" lub "night"
    String value = response.substring(bandClose + 1, bandEnd);
    value.trim();

    int numeric = toConditionValue(value);

    // Przypisanie do odpowiedniej zmiennej
    if (name == "80m-40m" && time == "day") band_80m_40m_day = numeric;
    if (name == "30m-20m" && time == "day") band_30m_20m_day = numeric;
    if (name == "17m-15m" && time == "day") band_17m_15m_day = numeric;
    if (name == "12m-10m" && time == "day") band_12m_10m_day = numeric;

    if (name == "80m-40m" && time == "night") band_80m_40m_night = numeric;
    if (name == "30m-20m" && time == "night") band_30m_20m_night = numeric;
    if (name == "17m-15m" && time == "night") band_17m_15m_night = numeric;
    if (name == "12m-10m" && time == "night") band_12m_10m_night = numeric;

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
