#include <LiquidCrystal.h>
#include <Wire.h>
#include <hd44780.h>
#include <hd44780ioClass/hd44780_I2Cexp.h>
#include <time.h>

hd44780_I2Cexp lcd;

#define TEXT_EX1 "2022-11-19 10:43"
#define TEXT_EX2 "TZ | CEST GMT+1"
#define TEXT_EX3 "NXT TODAY  18:00"

static unsigned long TMS = 1668905289;

typedef struct MyDateTime {
  unsigned int year;
  unsigned char month;
  unsigned char day;
  unsigned char hour;
  unsigned char minute;
  unsigned char second;
} MyDateTime;

void printUnsignedLong(unsigned long value){
  char buff[16] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
  ltoa(value, buff, 10);
}

void getDayAndMonthFromPastdays(unsigned int pastdays, unsigned char * month, unsigned char * day){
  unsigned char monthsDays [12] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
  unsigned char i = 0;
  pastdays += 1;
  while (i < 12 && pastdays > monthsDays[i]){
    pastdays -= monthsDays[i];
    i++;
  }
  *month = i + 1;
  *day = pastdays;
}

void timestampToDateTime(unsigned long tsorigin, MyDateTime * datetime){
  unsigned long timestamp = tsorigin;
  datetime->second = timestamp % 60;
  timestamp -= datetime->second;
  timestamp /= 60;
  datetime->minute = timestamp % 60;
  timestamp -= datetime->minute;
  timestamp /= 60;
  datetime->hour = timestamp % 24;
  timestamp -= datetime->hour;
  timestamp /= 24;
  datetime->year =  1970 + ((timestamp / (3 * 365 + 366)) * 4);
  unsigned int yearPast = datetime->year - 1970;
  unsigned int quartYear = yearPast / 4;
  unsigned int quartYearMod = yearPast % 4;
  unsigned long daysPast = (quartYear * (365 * 3 + 366)) + (quartYearMod * 365);
  unsigned long daysPastInYear = timestamp - daysPast;
  getDayAndMonthFromPastdays(daysPastInYear, &datetime->month, &datetime->day);
}

void datetimeToString(MyDateTime datetime, char * strToFill, char len){
  snprintf(strToFill, len, "%04d-%02d-%02d %02d:%02d", datetime.year, datetime.month, datetime.day, datetime.hour, datetime.minute);
}

void updateDateTimeDisplay(){
  lcd.setCursor(0, 0);
  MyDateTime datetime;
  timestampToDateTime(TMS, &datetime);
  char strDateTime[17] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
  datetimeToString(datetime, strDateTime, 17);
  lcd.print(strDateTime);
}

void updateTimestampDisplay(){
  char buff[16] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
  lcd.setCursor(0, 1);
  ltoa(TMS, buff, 10);
  lcd.print(buff);
}

void updateTimeDisplay(){
  updateDateTimeDisplay();
  updateTimestampDisplay();
}

void setup() {
  // put your setup code here, to run once:
  lcd.begin(16, 2);
  lcd.clear();
  lcd.home();
  updateDateTimeDisplay();
  updateTimestampDisplay();
  Serial.begin(115200);
}

unsigned long lastmls = 0;

void loop() {

  unsigned long mls = millis();

  if (mls % 1000 == 0 && mls != lastmls){
    lastmls = mls;
    TMS++;
    updateTimeDisplay();
  }

}


