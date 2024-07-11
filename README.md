## Wiring
|CCS811|ESP32|
|:---:|:---:|
|VCC|3V3|
|GND|GND|
|SCL|GPIO22|
|SDA|GPIO21|
|WAK|GND|
> **_NOTE_:**
> Remember to connect the WAK pin of the CCS811 to GND to be able to interface with the CCS811 module!

|DFROBOT NO2|ESP32|
|:---:|:---:|
|VCC|3V3|
|GND|GND|
|A|GPIO33|

|DFROBOT CH4|ESP32|
|:---:|:---:|
|VCC|3V3|
|GND|GND|
|A|GPIO34|

|RTC module|ESP32|
|:---:|:---:|
|GND|GND|
|VCC|3V3|
|SDA|GPIO21|
|SCL|GPIO22|
|SQW|X|
|32K|X|

|SD module|ESP32|
|:---:|:---:|
|GND|GND|
|VCC|5V|
|MISO|GPIO19|
|MOSI|GPIO23|
|SCK|GPIO18|
|CS|GPIO5|