#include <RTClib.h>
#include <Wire.h>
#include <SoftwareSerial.h>


SoftwareSerial GPRS(7,8);  //SoftwareSerial mySerial(7, 8); //set pins to communicate with GSM sheild


RTC_DS3231 RTC;


void setup()
{
  GPRS.begin(19200);
  Serial.begin(19200);
  RTC.begin();
  Wire.begin();
  ConnectNetwork();
  GetDateTime();

  
  //show old datetime from RTC
  showDate("Old datetime: ",RTC.now());

  syncRTCwithGSM(getGSMTimestamp(),RTC);
  
  //show new datetime from RTC, synced with GSM module
  showDate("New datetime: ",RTC.now());
}


void loop()
{
  
 
}



/////return current Date/Time and attach to string getGSMTimestamp///
String getGSMTimestamp(){
  boolean notConnected = true;
  
  return send_AT_Command("AT+CCLK?",1000);
}

String send_AT_Command(String AT_COMMAND, int tDelay){
  
  return //////this return needs to return the date/time result which is printed to the serial monitor from ("AT+CCLK")/// (AT_COMMAND,tDelay);
}



/////connect to GSM network////
void ConnectNetwork()
{
  boolean notConnected=true;
  GPRS.println("AT+CGATT=1"); // Attach from GPRS serivce
  Response();
  delay(1000);
  GPRS.println("AT+CSTT=\"internet\""); // Start task and set APN
  delay(3000);
  Response();
  GPRS.println("AT+CIICR"); // Bring up wireless connection with GPRS
  delay(2000);
  Response();
  GPRS.println("AT+CIFSR"); // Get local IP address
  Response();
  GPRS.println("AT+CIPSPRT=1"); // Set its prompt echo '>'and shows "send ok" when sending is successful
  Response();
  GPRS.println("AT+CIPQSEND=0"); // Set normal data transmitting mode
  Response();
}



////close connection function///
void CloseConnection()
{
  GPRS.println("AT+CIPCLOSE"); // Close TCP connection
  Response();
  delay(3000);
}



////Get time function////
void GetDateTime()
{
  
  delay(500);
  GPRS.println("AT+CLTS=1"); //get current date & time from network
  delay(500);
  Response();
  GPRS.println("AT+CCLK?"); //display timestamp
  Response();
  delay(500);

}




////use Date/Time returned from network to update RTC date/time///
void syncRTCwithGSM(String CCLK_Return, RTC_DS3231 myRTC){
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


////Response function to show result from network prompt////
void Response()
{
  delay(100);
  while(!GPRS.available());
  if(GPRS.available()){
    while(GPRS.available()){
      Serial.write(GPRS.read());
    }
  }
}



