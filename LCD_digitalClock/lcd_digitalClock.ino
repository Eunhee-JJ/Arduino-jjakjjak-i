#include <LiquidCrystal.h>
#include <Wire.h>
#include "RTClib.h"

DateTime now;
char day[7][12] = {"Sun","Mon","Tue","Wed","Thu","Fri","Sat"};

RTC_DS3231 rtc;

void showDate();
void showTime();
void showDay();

char buf1[10];
char buf2[10];
char buf3[10];

//int timer = 0 ; //Start the time at 0
LiquidCrystal lcd (2, 3, 4, 5, 6, 7); //Initialize the library with the numbers of the interface pins

void setup()
{
  lcd.begin(16 , 2 ); //Set up the LCD's number of columns and
  //lcd.print("Hello world!"); //Print a message to the
  Serial.begin(9600);
  Wire.begin();

  if (! rtc.begin())
  {
    Serial.println("Couldn't find RTC");
    while(1);
  }
  
  if (rtc.lostPower())
  {
    Serial.println("lost power");
    rtc.adjust(DateTime(F(__DATE__),F(__TIME__)));
  }

  rtc.adjust(DateTime(F(__DATE__),F(__TIME__)));
}

void loop()
{
  now = rtc.now();
  showDate();
  showDay();
  showTime();
  int time = 0;
  String AP="";
  if (now.hour()>12){
    time = now.hour()-12;
    AP = "PM";
  }
  else{
    AP = "AM";
  }
  sprintf(buf1,"%02d",time);
  sprintf(buf2,"%02d",now.minute());
  sprintf(buf3,"%02d",now.second());
  lcd.setCursor(1,0);
  lcd.print(now.year());
  lcd.print("/");
  lcd.print(now.month());
  lcd.print("/");
  lcd.print(now.day());
  lcd.print("/");
  lcd.print(day[now.dayOfTheWeek()]);
  lcd.setCursor(2,1);
  lcd.print(AP);
  lcd.print("  ");
  lcd.print(buf1);
  lcd.print(":");
  lcd.print(buf2);
  lcd.print(":");
  lcd.print(buf3);
  //Serial.println();
  delay(1000);
  lcd.clear();
}

void showDate()
{
 Serial.print(now.year());
 Serial.print("/");
 Serial.print(now.month());
 Serial.print("/");
 Serial.print(now.day());
 Serial.print("  "); 
}

void showDay()
{
  Serial.print(day[now.dayOfTheWeek()]);
  Serial.print(" ");
}

void showTime()
{
  Serial.print("Time: ");
  Serial.print(now.hour());
  Serial.print(":");
  Serial.print(now.minute());
  Serial.print(":");
  Serial.print(now.second());
  Serial.println();
}
