#include <NTPClient.h>
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <WiFiUdp.h>
#include "index.h"
#define LED D2
const int buttonPin = D1;
int buttonState = 0; 
const char *ssid = "";
const char *password = "";
const long utcOffsetInSeconds = 10800;
char daysOfTheWeek[7][12] = { "Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday" };
String alarma_state;
ESP8266WebServer server(80);
// Define NTP Client to get time
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org", utcOffsetInSeconds);

void handleRoot()
{
  String s = MAIN_page; //Read HTML contents
  server.send(200, "text/html", s); //Send web page
}

void handleADC()
{
  int sec = timeClient.getSeconds();
  int min = timeClient.getMinutes();
  int ore = timeClient.getHours();
  String sec_c = String(sec);
  String min_c = String(min);
  String ore_c = String(ore);
  String total = ore_c + ":" + min_c + ":" + sec_c;
  server.send(200, "text/plane", total);  //Send ADC value only to client ajax request

}

void handleLED()
{
  String ledState = "OFF";
  String t_state = server.arg("LEDstate");  //Refer  xhttp.open("GET", "setLED?LEDstate="+led, true);
  Serial.println(t_state);
  if (t_state == "1")
  {
    digitalWrite(LED, LOW); //LED ON
    ledState = "ON";  //Feedback parameter
  }
  else
  {
    digitalWrite(LED, HIGH);  //LED OFF
    ledState = "OFF"; //Feedback parameter  
  }

  server.send(200, "text/plane", ledState); //Send web page
}

//NOU

void alarma()
{
  alarma_state = "OFF";
  String t_state = server.arg("alarma_state");  //Refer  xhttp.open("GET", "setLED?LEDstate="+led, true);
  Serial.println(t_state);
  if (t_state == "1")
  {
    alarma_state = "ON";  //Feedback parameter
  }
  else
  {
    alarma_state = "OFF"; //Feedback parameter  
  }

  server.send(200, "text/plane", alarma_state); //Send web page
}






void sonerie()
{
  digitalWrite(LED, HIGH);
  delay(1000);
  digitalWrite(LED, LOW);
  delay(1000);
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

void setup()
{
  Serial.begin(115200);
  pinMode(LED, OUTPUT);
  pinMode(buttonPin, INPUT);
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
  server.on("/setLED", handleLED);
  server.on("/readADC", handleADC);
  server.on("/alarma", alarma);

  server.begin(); //Start server
  Serial.println("HTTP server started");

}

void loop()
{
  timeClient.update();
  if (buttonState == HIGH) {
    alarma_state = "ON";
    Serial.write("aici");
  } else {
    alarma_state = "OFF";
  }
  buttonState = digitalRead(buttonPin);
  pauza(timeClient.getHours(), timeClient.getMinutes(), timeClient.getSeconds());

  delay(1000);
  if(alarma_state == "ON"){
    sonerie();
  }
  server.handleClient();
}
