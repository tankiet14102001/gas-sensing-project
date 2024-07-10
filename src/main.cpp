/**************************************************************************/
/*!
    @brief  Setups the I2C interface and hardware and checks for communication.
    @param  addr Optional I2C address the sensor can be found on. Default is
   0x5A
    @param theWire Optional pointer to I2C interface, &Wire is used by default
    @returns True if device is set up, false on any failure
*/
/**************************************************************************/

/***************************************************************************
    @brief Define libraries
 ***************************************************************************/

#include <Arduino.h>

#include <Wire.h>
#include <SPI.h>

#include "Adafruit_CCS811.h"
#include <RTClib.h>
#include <sd.h>

/***************************************************************************
    @brief Define constants
 ***************************************************************************/

#define sd_CS 5
#define sd_chip_select 5

/***************************************************************************
    @brief Define objects
 ***************************************************************************/
 
Adafruit_CCS811 ccs;
RTC_DS3231 rtc;  // rtc module
File dataFile, dataFile1, dataFile2, dataFile3;  // sd module

/***************************************************************************
    @brief Define parameters
 ***************************************************************************/

unsigned long timer1;

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
} now_data;

struct
{
  float co2_data;
  float tvoc_data;
  float no2_data;
  float ch4_data;
} sensors_data;

int no2Pin = 33;
int ch4Pin = 34;
float no2Value, ch4Value;

/***************************************************************************
    @brief Functions declarations
 ***************************************************************************/

void rtc_init();
void sd_init();
void get_file_name();
void sd_make_file();

void sd_log_data();
void sd_log_data_1();
void sd_log_data_2();
void sd_log_data_3();

void get_sensors_data();

/***************************************************************************
    @brief Applications
 ***************************************************************************/
 
void setup() {
  Serial.begin(115200);
  timer1 = millis();
 
  Serial.println("CCS811 test");
 
  if(!ccs.begin()){
    Serial.println("Failed to start sensor! Please check your wiring.");
    while(1);
  }
 
  // Wait for the sensor to be ready
  while(!ccs.available());
  rtc_init();
  sd_init();
  //get_file_name();
  //sd_make_file();
}
 
void loop() {
  if ( (unsigned long) (millis() - timer1) > 1000)
  {
    counter_1++;
    counter_2++;
    counter_3++;
    DateTime now = rtc.now();
    now_data.day = now.day();
    now_data.month = now.month();
    now_data.year = now.year();
    now_data.hour = now.hour();
    now_data.minute = now.minute();
    now_data.second = now.second();

    sd_make_file();

    if(counter_1 >= 10)
    {
      sd_log_data_1();
      counter_1 = 0;
    }
    if(counter_2 >= 15)
    {
      sd_log_data_2();
      counter_2 = 0;
    }
    if(counter_3 >= 30)
    {
      sd_log_data_3();
      counter_3 = 0;
    }
    get_file_name();
    get_sensors_data();
    if(ccs.available()){
      if(!ccs.readData()){
        Serial.print("CO2: ");
        Serial.print(sensors_data.co2_data);
        Serial.print("ppm, TVOC: ");
        Serial.print(sensors_data.tvoc_data);
        Serial.print("\tNO2: ");
        Serial.print(sensors_data.no2_data);
        Serial.print("\tCH4: ");
        Serial.println(sensors_data.ch4_data);
      }
      else{
        Serial.println("ERROR!");
        //while(1);
      }
    }
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
    //sd_log_data();
    timer1 = millis();
  }
}

void rtc_init()
{
  //DateTime now = rtc.now();
  rtc.begin();
  if (!rtc.begin())
  {
    Serial.println("RTC not found!");
  }
  else
  {
    Serial.println("RTC initialized!");
  }
  //DateTime now = rtc.now();
  //rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
  //Serial.println("Time updated!");
}
void rtc_set_time()
{
  //DateTime now = rtc.now();
  rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));  
  Serial.println("Time updated!");
}
void sd_init()
{
  if (!sd.begin(sd_CS))
  {
    Serial.println("sd card not found.");
    //return;
  }
  Serial.println("sd init successful.");
  //sd_make_file();
}

void get_file_name()
{
  //rtc_get_time();
  //sprintf(file_name, "/%02d%02d.txt", now_data.hour, now_data.minute);
  sprintf(file_name, "/%04d%02d%02d.csv", now_data.year, now_data.month, now_data.day);
  sprintf(file_name_1, "/%04d%02d%02d_1.csv", now_data.year, now_data.month, now_data.day);
  sprintf(file_name_2, "/%04d%02d%02d_2.csv", now_data.year, now_data.month, now_data.day);
  sprintf(file_name_3, "/%04d%02d%02d_3.csv", now_data.year, now_data.month, now_data.day);
  Serial.print("file name: ");
  Serial.println(file_name);
}

void sd_make_file()
{
  //get_file_name();
  if (! sd.exists(file_name))
  {
    File dataFile = sd.open(file_name, FILE_WRITE);
    dataFile.print("CO2,TVOC,NO2,CH4\n");
    dataFile.close();
  }
  else if (! sd.exists(file_name_1))
  {
    File dataFile1 = sd.open(file_name_1, FILE_WRITE);
    dataFile1.print("CO2,TVOC,NO2,CH4\n");
    dataFile1.close();
  }
  else if (! sd.exists(file_name_2))
  {
    File dataFile2 = sd.open(file_name_2, FILE_WRITE);
    dataFile2.print("CO2,TVOC,NO2,CH4\n");
    dataFile2.close();
  }
  else if (! sd.exists(file_name_3))
  {
    File dataFile3 = sd.open(file_name_3, FILE_WRITE);
    dataFile3.print("CO2,TVOC,NO2,CH4\n");
    dataFile3.close();
  }
  else
  {
    Serial.println("file for today existed!");
  }
}

void sd_log_data()
{
  File dataFile = sd.open(file_name, FILE_APPEND);
  if (dataFile)
  {
    // char buf[32];
    // sprintf(buf, "%2d/%2d/%4d  %2d:%2d:%2d: ", now_data.day, now_data.month, now_data.year, now_data.hour, now_data.minute, now_data.second);

    // dataFile.print(buf);
    // dataFile.print("\t");

    // dataFile.print(dataPM1_0);
    // dataFile.print("\t");
    // dataFile.print(dataPM2_5);
    // dataFile.print("\t");
    // dataFile.print(dataPM10);
    // dataFile.print("\n");
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
  //return;
}

void sd_log_data_1()
{
  File dataFile1 = sd.open(file_name_1, FILE_APPEND);
  if (dataFile1)
  {
    // char buf[32];
    // sprintf(buf, "%2d/%2d/%4d  %2d:%2d:%2d: ", now_data.day, now_data.month, now_data.year, now_data.hour, now_data.minute, now_data.second);

    // dataFile.print(buf);
    // dataFile.print("\t");

    // dataFile.print(dataPM1_0);
    // dataFile.print("\t");
    // dataFile.print(dataPM2_5);
    // dataFile.print("\t");
    // dataFile.print(dataPM10);
    // dataFile.print("\n");
    dataFile1.print(sensors_data.co2_data);
    dataFile1.print(",");
    dataFile1.print(sensors_data.tvoc_data);
    dataFile1.print(",");
    dataFile1.print(sensors_data.no2_data);
    dataFile1.print(",");
    dataFile1.println(sensors_data.ch4_data);

    dataFile1.close();
    Serial.println("print 1!");
  }
  else
  {
    Serial.println("writing failed!!");
  }
  //return;
}

void sd_log_data_2()
{
  File dataFile2 = sd.open(file_name_2, FILE_APPEND);
  if (dataFile2)
  {
    // char buf[32];
    // sprintf(buf, "%2d/%2d/%4d  %2d:%2d:%2d: ", now_data.day, now_data.month, now_data.year, now_data.hour, now_data.minute, now_data.second);

    // dataFile.print(buf);
    // dataFile.print("\t");

    // dataFile.print(dataPM1_0);
    // dataFile.print("\t");
    // dataFile.print(dataPM2_5);
    // dataFile.print("\t");
    // dataFile.print(dataPM10);
    // dataFile.print("\n");
    dataFile2.print(sensors_data.co2_data);
    dataFile2.print(",");
    dataFile2.print(sensors_data.tvoc_data);
    dataFile2.print(",");
    dataFile2.print(sensors_data.no2_data);
    dataFile2.print(",");
    dataFile2.println(sensors_data.ch4_data);

    dataFile2.close();
    Serial.println("print 2!");
  }
  else
  {
    Serial.println("writing failed!!");
  }
  //return;
}

void sd_log_data_3()
{
  File dataFile3 = sd.open(file_name_3, FILE_APPEND);
  if (dataFile3)
  {
    // char buf[32];
    // sprintf(buf, "%2d/%2d/%4d  %2d:%2d:%2d: ", now_data.day, now_data.month, now_data.year, now_data.hour, now_data.minute, now_data.second);

    // dataFile.print(buf);
    // dataFile.print("\t");

    // dataFile.print(dataPM1_0);
    // dataFile.print("\t");
    // dataFile.print(dataPM2_5);
    // dataFile.print("\t");
    // dataFile.print(dataPM10);
    // dataFile.print("\n");
    dataFile3.print(sensors_data.co2_data);
    dataFile3.print(",");
    dataFile3.print(sensors_data.tvoc_data);
    dataFile3.print(",");
    dataFile3.print(sensors_data.no2_data);
    dataFile3.print(",");
    dataFile3.println(sensors_data.ch4_data);

    dataFile3.close();
    Serial.println("print 3!");
  }
  else
  {
    Serial.println("writing failed!!");
  }
  //return;
}

void get_sensors_data()
{
  sensors_data.co2_data = ccs.geteCO2();
  sensors_data.tvoc_data = ccs.getTVOC();
  sensors_data.no2_data = analogRead(no2Pin);
  sensors_data.ch4_data = analogRead(ch4Pin);
}