License: CC-BY-NC-SA


# Polska wersja poniżej!

![Screenshot_20250711_104856](https://github.com/user-attachments/assets/b4e92ac1-e99a-44d1-8a6b-6833c615d033)

![FOT_2235](https://github.com/user-attachments/assets/5233885b-4ea6-49ee-989c-db660b8dd178)

![FOT_3730](https://github.com/user-attachments/assets/5856e151-d021-445c-963f-bf02a169ebea)

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

## Environment Setup and Compilation

#### Installing ESP32 Board Support

1. Open Arduino IDE
2. Go to **Tools → Board → Board Manager**
3. Search for `esp32`
4. Install **esp32 by Espressif Systems**
5. Select **ESP32 Dev Module** as your board

#### Required Libraries

Install via **Sketch → Include Library → Manage Libraries**:

- WiFiManager by tzapu
- WiFiClientSecure
- ArduinoHttpClient by Arduino
- Adafruit ILI9341
- Adafruit GFX

#### Build and Upload

1. Download the project from GitHub:  
   https://github.com/canislupus11/HamQSL-XML-Parser/
2. Open the `.ino` file in Arduino IDE
3. Select the correct COM port and board
4. Upload the code to your ESP32

## Enclosure

Proposal of 3d printed case:
- https://www.thingiverse.com/thing:4827372
- https://www.thingiverse.com/thing:6918515

## Troubleshooting Display Issues (ILI9341 / ILI9342)

If your TFT display shows a distorted or shifted image (e.g., noise bars, incorrect orientation, or strange colors), it's possible that your screen uses an ILI9342 controller instead of the expected ILI9341. Although Adafruit does not officially support ILI9342, the following workaround might help.
1. Shifted or distorted image

Some displays using the ILI9342 controller have a different default screen orientation. To fix this, manually modify the header file of the Adafruit ILI9341 library.

Open Adafruit_ILI9341.h and locate:
```cpp
#define ILI9341_TFTWIDTH  240
#define ILI9341_TFTHEIGHT 320
```
Replace them with:
```cpp
#define ILI9341_TFTWIDTH  320
#define ILI9341_TFTHEIGHT 240
```

Recompile and upload the project. The image should now appear correctly.

2. Incorrect colors (e.g., red appears blue, yellow appears cyan)

If your display shows swapped or incorrect colors, it may be operating in BGR mode instead of RGB.

Rather than attempting to change the display controller mode, you can manually adjust the color definitions in your code:
```cpp
#define RED     0x001F  // was 0xF800
#define YELLOW  0x07FF  // was 0xFFE0
```
This simple fix allows the display to show correct colors without modifying controller registers.


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
  - 

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
- Adafruit ILI9341
- Adafruit GFX

#### Kompilacja i wgrywanie

1. Pobierz projekt z GitHuba:  
   https://github.com/canislupus11/HamQSL-XML-Parser/
2. Otwórz plik `.ino` w Arduino IDE
3. Wybierz odpowiedni port COM i płytkę ESP32 Dev Module
4. Wgraj kod do płytki

## Obudowa

Propozycje drukowanej obudowy: 
- https://www.thingiverse.com/thing:4827372
- https://www.thingiverse.com/thing:6918515

## Rozwiązywanie problemów z wyświetlaczem TFT (ILI9341 / ILI9342)

Jeśli Twój wyświetlacz TFT pokazuje zniekształcony obraz (np. przesunięcie, pasy szumu, nieprawidłowe kolory), możliwe, że trafił Ci się wariant sterownika ILI9342 zamiast ILI9341. Choć Adafruit oficjalnie nie wspiera ILI9342, istnieje proste obejście, które może rozwiązać problem.
1. Obraz przesunięty, zniekształcony, odwrócony

W przypadku wyświetlacza o innej orientacji (np. ILI9342), należy zmodyfikować plik nagłówkowy biblioteki Adafruit_ILI9341.h.

Odszukaj linie:
```cpp
#define ILI9341_TFTWIDTH  240
#define ILI9341_TFTHEIGHT 320
```
i zamień je na:
```cpp
#define ILI9341_TFTWIDTH  320
#define ILI9341_TFTHEIGHT 240
```
Następnie skompiluj i wgraj projekt ponownie. Obraz powinien wyświetlać się prawidłowo.

2. Nieprawidłowe kolory (np. czerwony = niebieski, żółty = błękitny)

Jeśli kolory na wyświetlaczu są pozamieniane, to prawdopodobnie ekran działa w trybie BGR zamiast RGB.

Zamiast próbować przełączać tryb kontrolera, możesz ręcznie dostosować definicje kolorów w kodzie:
```cpp
#define RED     0x001F  // was 0xF800
#define YELLOW  0x07FF  // was 0xFFE0
```

To szybkie i skuteczne rozwiązanie, które pozwala dostosować kolory bez modyfikacji rejestrów sterownika.
