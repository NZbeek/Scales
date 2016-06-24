#include <GSM.h>
#include <Wire.h>
#include <RTClib.h>

// initialize the library instance
GPRS gprsAccess;  // GPRS access
GSM gsmAccess;    // GSM access: include a 'true' parameter for debug enabled
GSMClient client;  // Client service for TCP connection

GSM3ShieldV1DirectModemProvider modemAccess;

RTC_DS1307 RTC;

// PIN Number
#define PINNUMBER "xxxx"

// APN info
char apn[] = {"xxx"};
char login[] = {""};
char password[] = {""};

void setup(){
  Serial.begin(9600);
  Wire.begin();
  RTC.begin();
  
  connectGSM();
  
  //show old datetime from RTC
  showDate("Old datetime: ",RTC.now());
  
  syncRTCwithGSM(getGSMTimestamp(),RTC);
  
  //show new datetime from RTC, synced with GSM module
  showDate("New datetime: ",RTC.now());
}
void loop(){  
}



////GSM Functions, these use GSM Lib but I use SoftwareSerial///
void connectGSM(){
  boolean notConnected=true;
  Serial.println("Connecting to GSM network...");
  while(notConnected){
    if(gsmAccess.begin(PINNUMBER) == GSM_READY)
      notConnected = false;
    else delay(1000);
  }
  Serial.println("Connected to GSM, now GPRS...");

  // 1000ms bug
  delay(1000);
  
  while(gprsAccess.attachGPRS(apn, login, password)!=GPRS_READY){
    delay(1000);
  }
  Serial.println("GSM shield fully connected.");
}

String getGSMTimestamp(){
  boolean notConnected = true;
  
  return send_AT_Command("AT+CCLK?",1000);
}

void setGSMRTCSyncServer(){
  // set server
  send_AT_Command("AT+QNTP=\"time1.google.com\"",2000);

  // execute sync
  send_AT_Command("AT+QNTP",1000);  
}

String getGSMRTCSyncServer(){
  // get server
  return send_AT_Command("AT+QNTP?",1000);
}

void GSMRTCSync(){
  send_AT_Command("AT+QNTP",1000);   
}

String send_AT_Command(String AT_COMMAND, int tDelay){
  /*Serial.print("COMMAND: "+AT_COMMAND);
  Serial.println(modemAccess.writeModemCommand(AT_COMMAND,tDelay));
  Serial.println("-----------------------------------------------");*/
  
  return modemAccess.writeModemCommand(AT_COMMAND,tDelay);
}



/////RTC Functions, should remain same////

void syncRTCwithGSM(String CCLK_Return, RTC_DS1307 myRTC){
  DateTime datetime;
  
  char datetime_year[3];
  char datetime_month[3];
  char datetime_day[3];
  char datetime_hour[3];
  char datetime_minute[3];
  char datetime_second[3];
  
  String datetimeGSM = CCLK_Return.substring(CCLK_Return.indexOf("\"")+1,CCLK_Return.lastIndexOf("\"")-3);
  
  datetimeGSM.substring(0,2).toCharArray(datetime_year,sizeof(datetime_year));
  datetimeGSM.substring(3,5).toCharArray(datetime_month,sizeof(datetime_month));
  datetimeGSM.substring(6,8).toCharArray(datetime_day,sizeof(datetime_day));
  datetimeGSM.substring(9,11).toCharArray(datetime_hour,sizeof(datetime_hour));
  datetimeGSM.substring(12,14).toCharArray(datetime_minute,sizeof(datetime_minute));
  datetimeGSM.substring(15,17).toCharArray(datetime_second,sizeof(datetime_second));
  
  uint8_t datetime_year_int = atoi(datetime_year);
  uint8_t datetime_month_int = atoi(datetime_month);
  uint8_t datetime_day_int = atoi(datetime_day);
  uint8_t datetime_hour_int = atoi(datetime_hour);
  uint8_t datetime_minute_int = atoi(datetime_minute);
  uint8_t datetime_second_int = atoi(datetime_second);
  
  // substract one second in order to take into account the 1s delay from the GSM module
  myRTC.adjust(DateTime((2000+(uint16_t)datetime_year_int),datetime_month_int,datetime_day_int,datetime_hour_int,datetime_minute_int,((int)datetime_second_int-1)));
  
  datetimeGSM="";
}

void showDate(const char* txt, const DateTime& dt) {
    Serial.print(txt);
    Serial.print(' ');
    Serial.print(dt.year(), DEC);
    Serial.print('/');
    Serial.print(dt.month(), DEC);
    Serial.print('/');
    Serial.print(dt.day(), DEC);
    Serial.print(' ');
    Serial.print(dt.hour(), DEC);
    Serial.print(':');
    Serial.print(dt.minute(), DEC);
    Serial.print(':');
    Serial.print(dt.second(), DEC);
    Serial.println();
}

