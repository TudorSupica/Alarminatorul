  #include <NTPClient.h>
  #include <ESP8266WiFi.h>
  #include <WiFiClient.h>
  #include <ESP8266WebServer.h>
  #include <WiFiUdp.h>
  #include <LiquidCrystal_I2C.h>
  #include <WiFiManager.h>
  #include <DNSServer.h>
  #include <WiFiManager.h>
  #include "index.h"

  #define BUTTON_PIN D5
  #define LED_conectare D3
  #define LED D6

  LiquidCrystal_I2C lcd(0x27, 16, 2);
  ESP8266WebServer server(80);

  byte lastButtonState = LOW; byte ledState = LOW;

  const unsigned long CONNECT_TIMEOUT = 10; // Wait 3 minutes to connect to the real AP before trying to boot the local AP
  const unsigned long AP_TIMEOUT = 180; // Wait 3 minutes in the config portal before trying again the original WiFi creds

  unsigned long debounceDuration = 50; 
  unsigned long lastTimeButtonStateChanged = 0;

  bool contor_wifi=false; bool contor_lcd = false;  bool contor_saptamana = false;

  int ora_start = 8;  int ora_sfarsit = 19;


  const long utcOffsetInSeconds = 7200;
  WiFiUDP ntpUDP;
  NTPClient timeClient(ntpUDP, "europe.pool.ntp.org", utcOffsetInSeconds); //pool.ntp.org  ro.pool.ntp.org

  String daysOfTheWeek[7] = {
    "Sunday",
    "Monday",
    "Tuesday",
    "Wednesday",
    "Thursday",
    "Friday",
    "Saturday"
  };

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

    } else{ 
      lcd.noBacklight();
      lcd.clear();
    }
  }


  void ap_mode(){
      lcd.clear();
      lcd.backlight();
      lcd.setCursor(0, 0);  lcd.print("WiFi alarma");
      lcd.setCursor(0, 1);  lcd.print("Ip 192.168.4.1");
  } 
  
  void panel_credintials(){
    lcd.backlight();
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Panel-site:");
    lcd.setCursor(0, 1);
    lcd.print(WiFi.localIP());
    delay(5000);
    lcd.clear();
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
    WiFi.mode(WIFI_STA);
    Serial.begin(115200);

    pinMode(LED, OUTPUT);
    pinMode(LED_conectare, OUTPUT);
    pinMode(BUTTON_PIN, INPUT);
    timeClient.begin();
    Wire.begin(D2, D1);
    lcd.begin(16,2);
    lcd.backlight();
  
    
    WiFiManager wifiManager;
    //wifiManager.resetSettings();
    while (WiFi.status() != WL_CONNECTED) {
      if(WiFi.SSID().isEmpty()){ 
        ap_mode();
        wifiManager.autoConnect("alarma");
      }
    else{ 

      digitalWrite(LED_conectare, HIGH); 
      
      wifiManager.setConnectTimeout(CONNECT_TIMEOUT);
      wifiManager.setTimeout(AP_TIMEOUT);     
      ap_mode();
      wifiManager.autoConnect("alarma");
    }
    }
   
    
    panel_credintials();
    server.on("/", handleRoot); //Which routine to handle at root location. This is display page
    server.on("/led_set", led_control);
    server.begin(); //Start server
    Serial.println("HTTP server started");
    lcd.clear();
  }
  

  void loop() {
    WiFiManager wifiManager;
    timeClient.update();
    if (WiFi.status() != WL_CONNECTED ) {
        ESP.restart();
    } 
    else {
      digitalWrite(LED_conectare, LOW);
      button();
      if (timeClient.getDay() >= 1 && timeClient.getDay() <= 5) {
        ldc_afisare();
        pauza(timeClient.getHours(), timeClient.getMinutes(), timeClient.getSeconds());
      } 
      else{
        lcd.noBacklight();
        lcd.clear();
      }
      server.handleClient();
    }

  }
