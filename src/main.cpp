#include <Arduino.h>
#include <SPI.h>
#include "Adafruit_CCS811.h"
#include <Wire.h>

#include <RTClib.h>
#include <SD.h>

#define SD_CS 5
 
Adafruit_CCS811 ccs;
RTC_DS3231 rtc;  // rtc module
File dataFile;  // SD module
unsigned long timer1;

char file_name[32];
float dataPM1_0, dataPM2_5, dataPM10, datatemp;  // current data

struct
{
  int day;
  int month;
  int year;
  int hour;
  int minute;
  int second;
} now_data;

int no2Pin = 33;
int ch4Pin = 34;
int no2Value, ch4Value;

void rtc_init();
void sd_init();
void get_file_name();
void SD_make_file();

void SD_log_data();
 
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
  //SD_make_file();
}
 
void loop() {
  DateTime now = rtc.now();
  now_data.day = now.day();
  now_data.month = now.month();
  now_data.year = now.year();
  now_data.hour = now.hour();
  now_data.minute = now.minute();
  now_data.second = now.second();
  //sprintf(file_name, "/%04d%02d%02d.txt", now_data.year, now_data.month, now_data.day);

  //Serial.print("now_data info gathered. File name: "); Serial.println(file_name);


  Serial.print(now_data.day);
  Serial.print(now_data.month);
  Serial.print(now_data.year);
  Serial.print("\n");
  Serial.print(now_data.hour);
  Serial.print(now_data.minute);
  Serial.print(now_data.second);
  Serial.print("\n");
  get_file_name();
  if(ccs.available()){
    if(!ccs.readData()){
      Serial.print("CO2: ");
      Serial.print(ccs.geteCO2());
      Serial.print("ppm, TVOC: ");
      Serial.print(ccs.getTVOC());
      Serial.print("\tNO2: ");
      Serial.print(analogRead(no2Pin));
      Serial.print("\tCH4: ");
      Serial.println(analogRead(ch4Pin));
    }
    else{
      Serial.println("ERROR!");
      while(1);
    }
  }
  SD_make_file();
  SD_log_data();
  delay(5000);
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
  if (!SD.begin(SD_CS))
  {
    Serial.println("SD card not found.");
    //return;
  }
  Serial.println("SD init successful.");
  //SD_make_file();
}

void get_file_name()
{
  //rtc_get_time();
  //sprintf(file_name, "/%02d%02d.txt", now_data.hour, now_data.minute);
  sprintf(file_name, "/%04d%02d%02d.csv", now_data.year, now_data.month, now_data.day);
  Serial.print("file name: ");
  Serial.println(file_name);
}

void SD_make_file()
{
  //get_file_name();
  if (! SD.exists(file_name))
  {
    Serial.println("no file for today yet.");
    File dataFile = SD.open(file_name, FILE_WRITE);
    dataFile.print("CO2,TVOC,NO2,CH4\n");
    dataFile.close();
    Serial.println("New file created!");
  }
  else
  {
    Serial.println("file for today existed!");
    File dataFile = SD.open(file_name, FILE_APPEND);
    dataFile.close();
    Serial.println("file check completed!");
  }
}

void SD_log_data()
{
  File dataFile = SD.open(file_name, FILE_APPEND);
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
    dataFile.print(ccs.geteCO2());
    dataFile.print(",");
    dataFile.print(ccs.getTVOC());
    dataFile.print(",");
    dataFile.print(analogRead(no2Pin));
    dataFile.print(",");
    dataFile.println(analogRead(ch4Pin));

    dataFile.close();
  }
  else
  {
    Serial.println("writing failed!!");
  }
  //return;
}