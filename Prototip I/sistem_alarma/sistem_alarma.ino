#include <NTPClient.h>
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <WiFiUdp.h>
#include "index.h"
#define LED D1

#define BUTTON_PIN D2
byte lastButtonState = LOW;
byte ledState = LOW;
unsigned long debounceDuration = 50; // millis
unsigned long lastTimeButtonStateChanged = 0;


const char *ssid = "";
const char *password = "";
const long utcOffsetInSeconds = 10800;
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
  for (int i = 18; i <= 20; i++)
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
  pinMode(BUTTON_PIN, INPUT);
  WiFi.begin(ssid, password);

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

void loop()
{
  timeClient.update();
  button();
  pauza(timeClient.getHours(), timeClient.getMinutes(), timeClient.getSeconds());
  server.handleClient();
}
