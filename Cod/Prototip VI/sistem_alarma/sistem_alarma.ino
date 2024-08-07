#include <NTPClient.h>

#include <ESP8266WiFi.h>

#include <WiFiClient.h>

#include <ESP8266WebServer.h>

#include <WiFiUdp.h>

#include <LiquidCrystal_I2C.h>

#include "index.h"

#define BUTTON_PIN D5
#define LED_conectare D3
#define LED D6


LiquidCrystal_I2C lcd(0x27, 16, 2);
ESP8266WebServer server(80);

byte lastButtonState = LOW;
byte ledState = LOW;

unsigned long debounceDuration = 50; // millis
unsigned long lastTimeButtonStateChanged = 0;

bool contor_wifi = false;
bool contor_lcd = false;
bool contor_saptamana = false;

int ora_start=8;
int ora_sfarsit=19;

const char * ssid = "";
const char * password = "";

const long utcOffsetInSeconds = 7200;
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "europe.pool.ntp.org", utcOffsetInSeconds);//pool.ntp.org  ro.pool.ntp.org


String daysOfTheWeek[7]={"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};


void handleRoot() {
  String s = MAIN_page; //Read HTML contents
  server.send(200, "text/html", s); //Send web page
}


void ldc_afisare() {
    if (timeClient.getHours() >= ora_start && timeClient.getHours() <= ora_sfarsit) { //daca este in intervalul de ore 08:00-19:50
      lcd.backlight();
      lcd.setCursor(0, 0);
      if (timeClient.getMinutes() < 50) {
        if (contor_lcd == false) {
          contor_lcd = true;
          lcd.clear();
        }
        lcd.print(timeClient.getHours());
        lcd.print(":00->");
        lcd.print(timeClient.getHours());
        lcd.print(":50");
      }
      if (timeClient.getMinutes() >= 50) {
        if (contor_lcd == true) {
          contor_lcd = false;
          lcd.clear();
        }
        lcd.print(timeClient.getHours());
        lcd.print(":50->");
        lcd.print(timeClient.getHours() + 1);
        lcd.print(":00");
      }
      
    }
    else
      lcd.noBacklight();
}

void sonerie() {
  digitalWrite(LED, HIGH);
  delay(1000);
  digitalWrite(LED, LOW);
  delay(1000);
}

void button() {
  byte buttonState = digitalRead(BUTTON_PIN);
  if (buttonState != lastButtonState) {
    lastButtonState = buttonState;
    if (buttonState == LOW) {
      ledState = (ledState == HIGH) ? LOW : HIGH;
      digitalWrite(LED, ledState);
    }
  }
}

void pauza(int ore, int minute, int sec) {
  for (int i = ora_start; i <= ora_sfarsit; i++) {
    if ((ore == i && minute == 0 && sec == 0) || (ore == i && minute == 50 && sec == 0)) {
      sonerie();
      Serial.write("Sonerie");
    }
  }
  
}

void led_control() {
  String state = "OFF";
  String act_state = server.arg("state");
  if (act_state == "1") {
    digitalWrite(LED, HIGH); //LED ON
    state = "ON";
  } else {
    digitalWrite(LED, LOW); //LED OFF
    state = "OFF";
  }
  server.send(200, "text/plane", state);
}

void setup() {
  Serial.begin(115200);
  pinMode(LED, OUTPUT);
  pinMode(LED_conectare, OUTPUT);
  pinMode(BUTTON_PIN, INPUT);
  WiFi.begin(ssid, password);
  timeClient.begin(); 

  lcd.begin();
  lcd.backlight();

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
    lcd.setCursor(0, 0);
    lcd.print("Se asteapta WiFi");
    digitalWrite(LED_conectare, HIGH);
  }

  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP()); //IP address assigned to your ESP

  server.on("/", handleRoot); //Which routine to handle at root location. This is display page
  server.on("/led_set", led_control);
  server.begin(); //Start server
  Serial.println("HTTP server started");
  lcd.clear();
}

void loop() {
  
  timeClient.update();
  if (WiFi.status() != WL_CONNECTED) {
    lcd.backlight();
    digitalWrite(LED_conectare, HIGH);
    lcd.setCursor(0, 0);
    lcd.print("Se asteapta WiFi");
    contor_wifi = true;
  } 
  else {
    if (contor_wifi == true) {
      contor_wifi = false;
      lcd.clear();
      delay(200);
    }
    digitalWrite(LED_conectare, LOW);  
    button();
    if(timeClient.getDay()>=1 && timeClient.getDay()<=5){
      ldc_afisare();
      pauza(timeClient.getHours(), timeClient.getMinutes(), timeClient.getSeconds());
    }
    else
      lcd.noBacklight(); 
      
    server.handleClient();
  }

}
