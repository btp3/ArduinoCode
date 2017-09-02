#include <SoftwareSerial.h>

#include <TinyGPS.h>

/* This sample code demonstrates the normal use of a TinyGPS object.
   It requires the use of SoftwareSerial, and assumes that you have a
   4800-baud serial GPS device hooked up on pins 4(rx) and 3(tx).
*/

TinyGPS gps;
SoftwareSerial ss(3, 2);
SoftwareSerial serialSIM800(8,7);
String incString;
void setup()
{
  Serial.begin(115200);
  serialSIM800.begin(9600);
  setupModemForPost();
  ss.begin(9600);
  Serial.println();
}
void ShowSerialData()
{
  while(serialSIM800.available()!=0)  /* If data is available on serial port */
  Serial.write(char (serialSIM800.read())); /* Print character received on to the serial monitor */
}
void readFromGSM(){
        while(true){
          //Read SIM800 output (if available) and print it in Arduino IDE Serial Monitor
            while(serialSIM800.available()){
            Serial.write(serialSIM800.read());
            }  
 
         //Read Arduino IDE Serial Monitor inputs (if available) and send them to SIM800
         if(Serial.available()){
          
               incString = Serial.readString();
               if(incString.equals("/032\r\n")){
               serialSIM800.write((char)26);
               delay(1000);
               }
               else{
               serialSIM800.write(incString.c_str());
               }
         }   
      }
}

void setupModemForPost(){
    Serial.println(F("HTTP post method :"));
    Serial.print(F("AT\\r\\n"));
    serialSIM800.println("AT"); /* Check Communication */
    delay(5000);
    ShowSerialData();  /* Print response on the serial monitor */
    delay(5000);
    /* Configure bearer profile 1 */
    Serial.print(F("AT+SAPBR=3,1,\"CONTYPE\",\"GPRS\"\\r\\n"));
    serialSIM800.println("AT+SAPBR=3,1,\"CONTYPE\",\"GPRS\"");  /* Connection type GPRS */
    delay(5000);
    ShowSerialData();
    delay(5000);
    Serial.print(F("AT+SAPBR=3,1,\"APN\",\"internet\"\\r\\n"));
    serialSIM800.println("AT+SAPBR=3,1,\"APN\",\"internet\"");  /* APN of the provider */
    delay(5000);
    ShowSerialData();
    delay(5000);
    Serial.print(F("AT+SAPBR=1,1\\r\\n"));
    serialSIM800.println("AT+SAPBR=1,1"); /* Open GPRS context */
    delay(8000);
    ShowSerialData();
    delay(1000);
    Serial.print(F("AT+SAPBR=2,1\\r\\n"));
    serialSIM800.println("AT+SAPBR=2,1"); /* Query the GPRS context */
    delay(8000);
    ShowSerialData();
    delay(1000);
    Serial.print(F("AT+HTTPINIT\\r\\n"));
    serialSIM800.println("AT+HTTPINIT");  /* Initialize HTTP service */
    delay(5000); 
    ShowSerialData();
    delay(5000);
    Serial.print(F("AT+HTTPPARA=\"CID\",1\\r\\n"));
    serialSIM800.println("AT+HTTPPARA=\"CID\",1");  /* Set parameters for HTTP session */
    delay(5000);
    ShowSerialData();
    delay(5000);
    Serial.print(F("AT+HTTPPARA=\"REDIR\",1\\r\\n"));
    serialSIM800.println("AT+HTTPPARA=\"REDIR\",1");  /* Set parameters for HTTP session */
    delay(5000);
    ShowSerialData();
    delay(5000);
    Serial.print(F("AT+HTTPSSL=1\\r\\n"));
    serialSIM800.println("AT+HTTPSSL=1");  /* Enable HTTP SSL for HTTPS POST */
    delay(5000);
    ShowSerialData();
    delay(5000);
  
    Serial.print(F("AT+HTTPPARA=\"CONTENT\",\"application/json\"\\r\\n"));
    serialSIM800.println("AT+HTTPPARA=\"CONTENT\",\"application/json\"");  /* Setting content type */
    delay(8000);
    ShowSerialData();
    delay(5000);
  
    Serial.println(F("AT+HTTPPARA=\"USERDATA\",\"Authorization: Basic YW1hcjpiYWlrdW50aA==\"\\r\\n"));
    serialSIM800.println("AT+HTTPPARA=\"USERDATA\",\"Authorization: Basic YW1hcjpiYWlrdW50aA== \"");                     /* Setting AUTHORIZATION HEADER */
    delay(7000);
    ShowSerialData();
    delay(7000);
  
    Serial.print(F("AT+HTTPPARA=\"URL\",\"https://btpmain.herokuapp.com/api/location/create/\"\\r\\n"));     /*    https://btpmain.herokuapp.com/api/location/create/  */
    serialSIM800.println("AT+HTTPPARA=\"URL\",\"https://btpmain.herokuapp.com/api/location/create/\"");  /* Set parameters for HTTP session */
    delay(10000);
    ShowSerialData();
    delay(5000);
}

void doPOSTwithData(float lan , float lon , float spp){
    Serial.println(F("IN DOPOST :: ::"));
    
    Serial.println(F("AT+HTTPDATA=92,10000\\r\\n"));
    serialSIM800.println("AT+HTTPDATA=92,10000"); /* POST data of size given Bytes with maximum latency time of 10 seconds for inputting the data*/ 
      delay(4000);
      ShowSerialData();
      Serial.print(F("{\"latitude\": \""));
      Serial.print(lan);
      Serial.print(F("\",\"longitude\": \""));
      Serial.print(lon);
      Serial.print(F("\",\"speed\": \""));
      Serial.print(spp);
      Serial.print(F("\",\"loc_type\": \"user"));
      Serial.println(F("\",\"type_id\": \"1\"}"));
      serialSIM800.print("{\"latitude\": \"");
      serialSIM800.print(lan);
      serialSIM800.print("\",\"longitude\": \"");
      serialSIM800.print(lon);
      serialSIM800.print("\",\"speed\": \"");
      serialSIM800.print(spp);
      serialSIM800.print("\",\"loc_type\": \"user");
      serialSIM800.println("\",\"type_id\": \"1\"}");
      delay(3000);
      ShowSerialData();
      Serial.println(F("AT+HTTPACTION=1\\r\\n"));
      serialSIM800.println("AT+HTTPACTION=1");  /* Start POST session */
      delay(3000);
      ShowSerialData();
    Serial.println("AT+HTTPREAD\\r\\n");  
    serialSIM800.println("AT+HTTPREAD"); delay(3000);
    ShowSerialData();
    delay(3000);delay(3000);
    ShowSerialData();
    delay(3000); /* Read Response */
   // readFromGSM();
}

void loop()
{
  bool newData = false;
  unsigned long chars;
  unsigned short sentences, failed;

  // For one second we parse GPS data and report some key values
  for (unsigned long start = millis(); millis() - start < 5000;)
  {
    while (ss.available())
    {
      char c = ss.read();
      // Serial.write(c); // uncomment this line if you want to see the GPS data flowing
      if (gps.encode(c)) // Did a new valid sentence come in?
        newData = true;
    }
  }

  if (newData)
  {
    float flat, flon , spp;
    unsigned long age;
    gps.f_get_position(&flat, &flon, &age);
    spp = gps.speed();
    Serial.print("LAT=");
    Serial.print(flat == TinyGPS::GPS_INVALID_F_ANGLE ? 0.0 : flat, 4);
    Serial.print(" LON=");
    Serial.print(flon == TinyGPS::GPS_INVALID_F_ANGLE ? 0.0 : flon, 4);
    Serial.print(" SPEED=");
    Serial.println(spp);
    doPOSTwithData(flat,flon,spp);
    
  }
  
}
