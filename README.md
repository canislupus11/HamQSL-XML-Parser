License: CC-BY-NC-SA

# Polska wersja poniżej!

![FOT_2235](https://github.com/user-attachments/assets/5233885b-4ea6-49ee-989c-db660b8dd178)

## How It Works
The device connects to a Wi-Fi network and retrieves propagation data from HAMQSL.com in XML format. It displays information like Solar Flux, sunspot number, K and A indices, and propagation conditions for HF, 6m, 4m, and VHF (2m) bands on a color TFT display. All XML data is parsed into variables in the code, so you can easily configure which information should be displayed or how it should be presented.

## First Start
1. Power on the device.
2. If this is the first boot or Wi-Fi was reset, configuration mode will start.
3. Connect to the Wi-Fi network named `HamQSL`.
4. Open your web browser and go to `192.168.4.1` to configure the Wi-Fi connection.
5. After saving, the device will connect automatically.

## Resetting Wi-Fi Settings
- Hold the RESET button (GPIO 32 to GND) during startup to reset Wi-Fi settings.

## Displayed Data

| Section           | Description                                  |
|-------------------|----------------------------------------------|
| HF Band DAY/NIGHT | Propagation conditions for day/night         |
| K-Index           | Geomagnetic disturbance index                |
| A-Index           | Geomagnetic activity index                   |
| Solar Flux        | Solar radiation flux index                   |
| Sunspots          | Number of sunspots                           |
| VHF 6m/4m/2m      | 6m, 4m, and 2m band conditions (if available)|

### HF Status:
- RED – poor conditions
- YELLOW – fair conditions
- GREEN – good conditions

### VHF:
If any of the bands (6m, 4m, 2m) is open (e.g., via tropo or aurora), its label appears in green.

## Data Refresh
- Data is automatically refreshed every **60 minutes**.
- This interval can be changed by editing the `interval` constant in the source code.
- **Do not set the refresh interval below 60 minutes!**

## Tips
- Serial port runs at 115200 baud – useful for debugging.
- If Wi-Fi connection fails within 5 minutes, the device will restart automatically.

## Technical Requirements
- Microcontroller: ESP32
- Display: TFT 2.8" ILI9341 (SPI)

- Arduino Libraries:
  - WiFiManager
  - WiFiClientSecure
  - ArduinoHttpClient
  - Adafruit_ILI9341
  - Adafruit_GFX





## Opis działania
Urządzenie łączy się z siecią Wi-Fi i pobiera dane propagacyjne z serwera HAMQSL.com w formacie XML. Informacje takie jak Solar Flux, liczba plam słonecznych, indeksy K i A oraz warunki propagacyjne w pasmach HF, 6m, 4m i VHF (2m) są wyświetlane na kolorowym wyświetlaczu TFT. Wszystkie dane z XML są wyciągane do odpowiednich zmiennych w kodzie – dzięki temu można łatwo dostosować, które informacje mają być wyświetlane lub w jakiej formie.

## Pierwsze uruchomienie
1. Włącz urządzenie.
2. Jeśli to pierwsze uruchomienie lub sieć Wi-Fi została zresetowana, uruchomi się tryb konfiguracji.
3. Połącz się z siecią Wi-Fi o nazwie `HamQSL`.
4. Otwórz przeglądarkę i wpisz `192.168.4.1`, aby skonfigurować połączenie Wi-Fi.
5. Po zapisaniu danych urządzenie połączy się automatycznie.

## Resetowanie ustawień Wi-Fi
- Przytrzymaj przycisk RESET (GPIO 32 do masy) podczas uruchamiania, aby zresetować ustawienia Wi-Fi.

## Dane wyświetlane na ekranie
| Pozycja          | Opis                                         |
|------------------|----------------------------------------------|
| HF Band DAY/NIGHT| Warunki propagacyjne w dzień/noc             |
| K-Index          | Wskaźnik zaburzeń geomagnetycznych           |
| A-Index          | Wskaźnik aktywności geomagnetycznej          |
| Solar Flux       | Strumień słoneczny (aktywność słońca)        |
| Sunspots         | Liczba plam słonecznych                      |
| VHF 6m/4m/2m     | Warunki w pasmach 6m, 4m i 2m (jeśli dostępne)|

### Statusy HF:
- Czerwony – słabe warunki
- Żółty – umiarkowane warunki
- Zielony – dobre warunki

### VHF:
Jeśli któreś z pasm (6m, 4m, 2m) jest otwarte (np. przez tropo, aurorę), informacja o tym pojawia się w kolorze zielonym.


## Odświeżanie danych
- Dane są odświeżane automatycznie co **60 minut**.
- Czas ten można zmienić, edytując wartość stałej `interval` w kodzie źródłowym.
- **Nie ustawiaj krótszego czasu odświeżania niż 60 minut!**

## Wskazówki
- Port szeregowy działa z prędkością 115200 baud – można użyć go do diagnostyki.
- Jeśli połączenie z Wi-Fi nie powiedzie się w ciągu 5 minut, urządzenie uruchomi się ponownie.

## Wymagania techniczne
- Mikrokontroler: ESP32
- Wyświetlacz: TFT 2.8" ILI9341 (SPI)
- Biblioteki Arduino:
  - WiFiManager
  - WiFiClientSecure
  - ArduinoHttpClient
  - Adafruit_ILI9341
  - Adafruit_GFX
