#include <NTPClient.h>
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <WiFiUdp.h>
#include <LiquidCrystal_I2C.h>
#include "index.h"
#define LED D6
#define LED_conectare D3

LiquidCrystal_I2C lcd(0x27, 16, 2);

#define BUTTON_PIN D5
byte lastButtonState = LOW;
byte ledState = LOW;
unsigned long debounceDuration = 50; // millis
unsigned long lastTimeButtonStateChanged = 0;



const char *ssid = "SM";
const char *password = "olimpiada";
const long utcOffsetInSeconds = 7200  ;
char daysOfTheWeek[7][12] = { "Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday" };
ESP8266WebServer server(80);
// Define NTP Client to get time
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org", utcOffsetInSeconds);

void handleRoot()
{
  String s = MAIN_page; //Read HTML contents
  server.send(200, "text/html", s); //Send web page
}

bool contor_lcd=false;
void ldc_afisare(){
  
  if(timeClient.getHours()>=8 && timeClient.getHours()<=23){
    lcd.setCursor(0, 0);
      if(timeClient.getMinutes()<50){
        if(contor_lcd==false){ 
            contor_lcd=true;
            lcd.clear();
          }    
          lcd.print(timeClient.getHours()); lcd.print(":00->");
          lcd.print(timeClient.getHours()); lcd.print(":50");      
      }
      if(timeClient.getMinutes()>=50){
          if(contor_lcd==true){ 
            contor_lcd=false;
            lcd.clear();
          }
          lcd.print(timeClient.getHours()); lcd.print(":50->");
          lcd.print(timeClient.getHours()+1); lcd.print(":00");
      }
    }
           
}

void sonerie()
{
  digitalWrite(LED, HIGH);
  delay(1000);
  digitalWrite(LED, LOW);
  delay(1000);
}
void button(){
byte buttonState = digitalRead(BUTTON_PIN);
  if (buttonState != lastButtonState) {
    lastButtonState = buttonState;
    if (buttonState == LOW) {
      ledState = (ledState == HIGH) ? LOW: HIGH;
      digitalWrite(LED, ledState);
    }
  }
}

void pauza(int ore, int minute, int sec)
{
  for (int i = 8; i <= 23; i++)
  {
    if ((ore == i && minute == 0 && sec == 0) || (ore == i && minute == 50 && sec == 0))
    {
      sonerie();
      Serial.write("brrrrrr");
    }
  }
}

void led_control() 
{
 String state = "OFF";
 String act_state = server.arg("state");
 if(act_state == "1")
 {
  digitalWrite(LED,HIGH); //LED ON
  state = "ON";
 }
 else
 {
  digitalWrite(LED,LOW); //LED OFF
  state = "OFF";
 } 
 server.send(200, "text/plane", state);
}

void setup()
{
  Serial.begin(115200);
  pinMode(LED, OUTPUT);
  pinMode(LED_conectare, OUTPUT);
  pinMode(BUTTON_PIN, INPUT);
  WiFi.begin(ssid, password);

  lcd.begin();
  lcd.backlight();
  
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
    
  }

  timeClient.begin();

  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP()); //IP address assigned to your ESP

  server.on("/", handleRoot); //Which routine to handle at root location. This is display page
  server.on("/led_set", led_control);
  server.begin(); //Start server
  Serial.println("HTTP server started");

}
bool contor_wifi=false;
void loop()
{
  
  
  if (WiFi.status() != WL_CONNECTED){
    digitalWrite(LED_conectare, HIGH);
    lcd.setCursor(0,0);
    lcd.print("Se asteapta WiFi");
    contor_wifi=true;
  }
  else{ 
    if(contor_wifi==true){
      contor_wifi=false;
      lcd.clear();
    }
  digitalWrite(LED_conectare, LOW);
  timeClient.update();
  button();
  ldc_afisare();
  pauza(timeClient.getHours(), timeClient.getMinutes(), timeClient.getSeconds());
  server.handleClient();
  }
  //delay(500);
  //lcd.clear();
}
