License: CC-BY-NC-SA


# Polska wersja poniżej!

![Screenshot_20250711_104856](https://github.com/user-attachments/assets/b4e92ac1-e99a-44d1-8a6b-6833c615d033)

![FOT_2235](https://github.com/user-attachments/assets/5233885b-4ea6-49ee-989c-db660b8dd178)

![FOT_3730](https://github.com/user-attachments/assets/5856e151-d021-445c-963f-bf02a169ebea)

Oto tłumaczenie na angielski z **niezmienionym formatowaniem Markdown**:

---

## How it works

The device connects to a Wi-Fi network and downloads propagation data from the HAMQSL.com server in XML format. Information such as Solar Flux, sunspot number, K and A indices, and propagation conditions for HF, 6m, 4m, and VHF (2m) bands is displayed on a color TFT screen. All XML data is extracted into corresponding variables in the code — this makes it easy to adjust which information should be displayed and in what form.

## First startup

1. Power on the device.
2. If this is the first startup or the Wi-Fi network has been reset, the configuration mode will start.
3. Connect to the Wi-Fi network named `HamQSL`.
4. Open a browser and enter `192.168.4.1` to configure the Wi-Fi connection.
5. After saving the data, the device will connect automatically.

## Resetting Wi-Fi settings

* Hold the RESET button (GPIO 32 to ground) during startup to reset Wi-Fi settings.

## Data displayed on the screen

| Item              | Description                                       |
| ----------------- | ------------------------------------------------- |
| HF Band DAY/NIGHT | Propagation conditions during day/night           |
| K-Index           | Geomagnetic disturbance indicator                 |
| A-Index           | Geomagnetic activity indicator                    |
| Solar Flux        | Solar flux (sun activity)                         |
| Sunspots          | Number of sunspots                                |
| VHF 6m/4m/2m      | Conditions on 6m, 4m, and 2m bands (if available) |

### HF Status:

* Red – poor conditions
* Yellow – moderate conditions
* Green – good conditions

### VHF:

If any of the bands (6m, 4m, 2m) are open (e.g., due to tropo or aurora), the information will appear in green.

## Data refresh

* Data is refreshed automatically every **60 minutes**.
* This interval can be changed by modifying the `interval` constant in the source code.
* **Do not set the refresh interval shorter than 60 minutes!**

## Tips

* The serial port operates at 115200 baud — it can be used for diagnostics.
* If the Wi-Fi connection fails within 5 minutes, the device will restart.

## Technical requirements

* Microcontroller: ESP32
* Display: TFT 2.8" ILI9341 (SPI)
* Arduino libraries:

  * WiFiManager
  * WiFiClientSecure
  * ArduinoHttpClient
  * TFT_eSPI

## Environment setup and compilation

#### Installing the ESP32 board

1. Open Arduino IDE
2. Go to **Tools → Board → Boards Manager**
3. Search for `esp32`
4. Install **esp32 by Espressif Systems**
5. Select the **ESP32 Dev Module** board

#### Required libraries

Install via **Sketch → Include Library → Manage Libraries**:

* WiFiManager by tzapu
* WiFiClientSecure
* ArduinoHttpClient by Arduino
* TFT_eSPI

#### Compilation and uploading

1. Download the project from GitHub:
   [https://github.com/canislupus11/HamQSL-XML-Parser/](https://github.com/canislupus11/HamQSL-XML-Parser/)
2. Place the User_Setup.h file into the TFT_eSPI library directory (likely Arduino/libraries/TFT_eSPI)
3. Open the `.ino` file in Arduino IDE
4. Select the appropriate COM port and the ESP32 Dev Module board
5. Upload the code to the board

## Enclosure

Suggested 3D-printed enclosures:

* [https://www.thingiverse.com/thing:4827372](https://www.thingiverse.com/thing:4827372)
* [https://www.thingiverse.com/thing:6918515](https://www.thingiverse.com/thing:6918515)

Oto dodatkowa sekcja w **angielskiej wersji**, również w niezmienionym formacie Markdown:


## Troubleshooting TFT display issues (ILI9341 / ILI9342)

If you are using the TZT ESP32 LVGL screen – [https://a.aliexpress.com/_EwsuCq6](https://a.aliexpress.com/_EwsuCq6) – there are commented lines in the code marked with the display model. You must comment out the original ones and uncomment the lines corresponding to this specific display.

---


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
  - TFT_eSPI

## Instalacja środowiska i kompilacja

#### Instalacja płytki ESP32

1. Otwórz Arduino IDE
2. Przejdź do **Narzędzia → Płytka → Menedżer płytek**
3. Wyszukaj `esp32`
4. Zainstaluj **esp32 by Espressif Systems**
5. Wybierz płytkę **ESP32 Dev Module**

#### Wymagane biblioteki

Zainstaluj przez **Szkic → Dołącz bibliotekę → Zarządzaj bibliotekami**:

- WiFiManager by tzapu
- WiFiClientSecure
- ArduinoHttpClient by Arduino
- TFT_eSPI

#### Kompilacja i wgrywanie

1. Pobierz projekt z GitHuba:  
   https://github.com/canislupus11/HamQSL-XML-Parser/
2. Plik User_Setup.h wrzuć do katalogu z biblioteką TFT_eSPI (Prawdopodobnie Adruino/libraries/TFT_eSPI)
3. Otwórz plik `.ino` w Arduino IDE
4. Wybierz odpowiedni port COM i płytkę ESP32 Dev Module
5. Wgraj kod do płytki

## Obudowa

Propozycje drukowanej obudowy: 
- https://www.thingiverse.com/thing:4827372
- https://www.thingiverse.com/thing:6918515

## Rozwiązywanie problemów z wyświetlaczem TFT (ILI9341 / ILI9342)

Jeśli używasz TZT ESP32 LVGL screen - https://a.aliexpress.com/_EwsuCq6 w kodzie istnieją zakomentowane linie opisane modelem wyświetlacza, należy zakomentować orginalne i odkomentować te odpowiedzialne za ten wyświetlacz.
