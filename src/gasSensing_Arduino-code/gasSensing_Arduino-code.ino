/***************************************************************************
    @author Le Nguyen Tan Kiet
    @version 19:15, 11th July, 2024
 ***************************************************************************/

/***************************************************************************
    @brief Define libraries
 ***************************************************************************/
#include <Arduino.h>

#include <Wire.h>
#include <SPI.h>

#include "Adafruit_CCS811.h"
#include <RTClib.h>
#include <SD.h>

/***************************************************************************
    @brief Define constants
 ***************************************************************************/
#define SERIAL_BAUDRATE             115200

/**************************************************************************/
/*!
    @brief  Choose time setting mode.
    @param  SET_COMPUTER_TIME Set time base on real-time on your PC
    @param  SET_MANUAL_TIME   Set time by your own wanted time
    @param  FINISH_TIME_SETTING  Use this definition to upload the firmware to your MCU after successfully set the time.
*/
/**************************************************************************/
#define FINISH_TIME_SETTING

#define SD_CHIP_SELECT_PIN          5
#define NO2_PIN                     33
#define CH4_PIN                     34

/***************************************************************************
    @brief  Set sampling rates in seconds
 ***************************************************************************/
#define SAMPLING_RATE_1             10
#define SAMPLING_RATE_2             15
#define SAMPLING_RATE_3             30

/***************************************************************************
    @brief  LED definitions to indicate if system is still working
 ***************************************************************************/
#define LED_BUILTIN                 2
#define LED_STATE_ON                (1u)
#define LED_STATE_OFF               (0u)
#define LED_INIT_STATE              LED_STATE_OFF
uint8_t LED_State = LED_INIT_STATE;

/***************************************************************************
    @brief Define objects
 ***************************************************************************/ 
Adafruit_CCS811 ccs;
RTC_DS3231 rtc;  // rtc module
File dataFile, datafile_1, datafile_2, datafile_3;  // sd module

/***************************************************************************
    @brief Define parameters
 ***************************************************************************/
unsigned long timer_1 = 0;

char file_name[32];
char file_name_1[32];
char file_name_2[32];
char file_name_3[32];

uint16_t counter_1 = 0;
uint16_t counter_2 = 0;
uint16_t counter_3 = 0;

struct
{
  int day;
  int month;
  int year;
  int hour;
  int minute;
  int second;
} rtc_data;

struct
{
  float co2_data;
  float tvoc_data;
  float no2_data;
  float ch4_data;
} sensors_data;

// int no2_pin = 33;
// int ch4_pin = 34;
// float no2_value, ch4_value;

/***************************************************************************
    @brief Functions declarations
 ***************************************************************************/
void rtc_init();
void rtc_set_time();
void get_file_name();

void sd_init();
void sd_make_file();
void sd_log_data();
void sd_log_data_1();
void sd_log_data_2();
void sd_log_data_3();

void get_sensors_data();
void print_sensors_data();

void counter_increment();
void LED_init();
void LED_toggle();

/***************************************************************************
    @brief Applications
 ***************************************************************************/ 
void setup() {
  Serial.begin(SERIAL_BAUDRATE);
  timer_1 = millis();
 
  if(!ccs.begin()){
    Serial.println("Failed to start sensor! Please check your wiring.");
    while(1);
  }  
  while(!ccs.available());  // Wait for the sensor to be ready
  rtc_init();
  sd_init();
  LED_init();
}
 
void loop() {
  if ( (unsigned long) (millis() - timer_1) > 1000)
  {
    counter_increment();    
    rtc_set_time();
    DateTime now = rtc.now();
    rtc_data.day = now.day();
    rtc_data.month = now.month();
    rtc_data.year = now.year();
    rtc_data.hour = now.hour();
    rtc_data.minute = now.minute();
    rtc_data.second = now.second();    
    get_file_name();
    sd_make_file();    
    get_sensors_data();
    if(counter_1 >= SAMPLING_RATE_1)
    {
      sd_log_data_1();
      counter_1 = 0;
      LED_toggle();
    }
    if(counter_2 >= SAMPLING_RATE_2)
    {
      sd_log_data_2();
      counter_2 = 0;
    }
    if(counter_3 >= SAMPLING_RATE_3)
    {
      sd_log_data_3();
      counter_3 = 0;
    }
    if(ccs.available()) {
      if(!ccs.readData())
        print_sensors_data();
      else {
        Serial.println("ERROR!");
        //while(1);
      }
    }
    print_sensors_data();
    timer_1 = millis();
  }
}

/***************************************************************************
    @brief  RTC functions definitions
 ***************************************************************************/
void rtc_init()
{
  rtc.begin();
  if (!rtc.begin())
    Serial.println("RTC not found!");
  else
    Serial.println("RTC initialized!");
}
void rtc_set_time()
{
  #ifdef SET_COMPUTER_TIME
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
  #endif
  #ifdef SET_MANUAL_TIME
    // Set time with the format (yyyy, mm, dd, hh, mm, ss)
    // For example to set January 21, 2014 at 3am:
    rtc.adjust(DateTime(2014, 1, 21, 3, 0, 0));
  #endif
  #ifdef FINISH_TIME_SETTING
    // do nothing
  #endif
  //Serial.println("Time updated!");
}

/***************************************************************************
    @brief  SD functions definitions
 ***************************************************************************/
void sd_init()
{
  if (!SD.begin(SD_CHIP_SELECT_PIN))
  {
    Serial.println("SD card not found.");
    while(1);
  }
  Serial.println("SD init successful.");
}
void get_file_name()
{
  // sprintf(file_name, "/%04d%02d%02d.csv", rtc_data.year, rtc_data.month, rtc_data.day);
  sprintf(file_name_1, "/%04d%02d%02d_sampling_rate_1.csv", rtc_data.year, rtc_data.month, rtc_data.day);
  sprintf(file_name_2, "/%04d%02d%02d_sampling_rate_2.csv", rtc_data.year, rtc_data.month, rtc_data.day);
  sprintf(file_name_3, "/%04d%02d%02d_sampling_rate_3.csv", rtc_data.year, rtc_data.month, rtc_data.day);
}
void sd_make_file()
{
  if (! SD.exists(file_name))
  {
    File dataFile = SD.open(file_name, FILE_WRITE);
    dataFile.print("CO2,TVOC,NO2,CH4\n");
    dataFile.close();
  }
  else if (! SD.exists(file_name_1))
  {
    File datafile_1 = SD.open(file_name_1, FILE_WRITE);
    datafile_1.print("CO2,TVOC,NO2,CH4\n");
    datafile_1.close();
  }
  else if (! SD.exists(file_name_2))
  {
    File datafile_2 = SD.open(file_name_2, FILE_WRITE);
    datafile_2.print("CO2,TVOC,NO2,CH4\n");
    datafile_2.close();
  }
  else if (! SD.exists(file_name_3))
  {
    File datafile_3 = SD.open(file_name_3, FILE_WRITE);
    datafile_3.print("CO2,TVOC,NO2,CH4\n");
    datafile_3.close();
  }
  else
  {
    //Serial.println("file for today existed!");
  }
}
void sd_log_data()
{
  File dataFile = SD.open(file_name, FILE_APPEND);
  if (dataFile)
  {
    dataFile.print(sensors_data.co2_data);
    dataFile.print(",");
    dataFile.print(sensors_data.tvoc_data);
    dataFile.print(",");
    dataFile.print(sensors_data.no2_data);
    dataFile.print(",");
    dataFile.println(sensors_data.ch4_data);

    dataFile.close();
    Serial.println("print!");
  }
  else
  {
    Serial.println("writing failed!!");
  }
}
void sd_log_data_1()
{
  File datafile_1 = SD.open(file_name_1, FILE_APPEND);
  if (datafile_1)
  {
    datafile_1.print(sensors_data.co2_data);
    datafile_1.print(",");
    datafile_1.print(sensors_data.tvoc_data);
    datafile_1.print(",");
    datafile_1.print(sensors_data.no2_data);
    datafile_1.print(",");
    datafile_1.println(sensors_data.ch4_data);

    datafile_1.close();
    Serial.println("print 1!");
  }
  else
  {
    Serial.println("writing failed!!");
  }
}
void sd_log_data_2()
{
  File datafile_2 = SD.open(file_name_2, FILE_APPEND);
  if (datafile_2)
  {
    datafile_2.print(sensors_data.co2_data);
    datafile_2.print(",");
    datafile_2.print(sensors_data.tvoc_data);
    datafile_2.print(",");
    datafile_2.print(sensors_data.no2_data);
    datafile_2.print(",");
    datafile_2.println(sensors_data.ch4_data);

    datafile_2.close();
    Serial.println("print 2!");
  }
  else
  {
    Serial.println("writing failed!!");
  }
}
void sd_log_data_3()
{
  File datafile_3 = SD.open(file_name_3, FILE_APPEND);
  if (datafile_3)
  {
    datafile_3.print(sensors_data.co2_data);
    datafile_3.print(",");
    datafile_3.print(sensors_data.tvoc_data);
    datafile_3.print(",");
    datafile_3.print(sensors_data.no2_data);
    datafile_3.print(",");
    datafile_3.println(sensors_data.ch4_data);

    datafile_3.close();
    Serial.println("print 3!");
  }
  else
  {
    Serial.println("writing failed!!");
  }
}

/***************************************************************************
    @brief  Supporting functions definitions
 ***************************************************************************/
void counter_increment()
{
  counter_1++;
  counter_2++;
  counter_3++;
}
void get_sensors_data()
{
  sensors_data.co2_data = ccs.geteCO2();
  sensors_data.tvoc_data = ccs.getTVOC();
  sensors_data.no2_data = analogRead(NO2_PIN);
  sensors_data.ch4_data = analogRead(CH4_PIN);
}
void print_sensors_data()
{
  Serial.print("CO2: ");
  Serial.print(sensors_data.co2_data);
  Serial.print("ppm, TVOC: ");
  Serial.print(sensors_data.tvoc_data);
  Serial.print("\tNO2: ");
  Serial.print(sensors_data.no2_data);
  Serial.print("\tCH4: ");
  Serial.println(sensors_data.ch4_data);
  Serial.println(counter_1);
  Serial.println(counter_2);
  Serial.println(counter_3);
}
void LED_init() {
  pinMode(LED_BUILTIN, OUTPUT);
}
void LED_toggle() {
  if ( (LED_State == LED_STATE_OFF) ) {
    digitalWrite(LED_BUILTIN, HIGH);
    LED_State = LED_STATE_ON;
  }
  else {
    digitalWrite(LED_BUILTIN, LOW);
    LED_State = LED_STATE_OFF;
  }
  Serial.println("LED toggled!");
}