#include <HCSR04.h>

#include <Blynk.h>
#include <DallasTemperature.h>
#include <OneWire.h>
#include <DHT.h>
#include <dht.h>
#define BLYNK_PRINT Serial
#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>
#define ONE_WIRE_BUS D2
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);

char auth[] = "Y9F5UITMXUpAUgCWWSIPIDHEdYkPNpom";// Auth token
char ssid[] = "chikkitha";// WIFI name
char pass[] = "987654321";// WIFI password

const int trigPin = D6;
const int echoPin = D7;

#define SOUND_VELOCITY 0.034
#define CM_TO_INCH 0.393701

long duration;
float distanceCm;
float distanceInch;

#define sensorPin D3 //DHT
int sensorState = 0;
int lastState = 0;                   
int pinValue;                    
#define rainPin D5
int rainState = 0;
int lastRainState = 0;
#define DHTPIN 2    
#define DHTTYPE DHT11     
DHT dht(DHTPIN, DHTTYPE);



BlynkTimer timer;

BLYNK_WRITE(V0)
{
 pinValue = param.asInt();    
} 

void sendSensor()
{
  float h = dht.readHumidity();
  float t = dht.readTemperature();

  if (isnan(h) || isnan(t)) {
    Serial.println("Failed to read from DHT sensor!");
    return;
  }
 
  Blynk.virtualWrite(V5, h);  
  Blynk.virtualWrite(V6, t);  
}

void moisture() {
  int value = analogRead(A0);
  value = map(value, 0, 1023, 0, 100);
  Blynk.virtualWrite(V0, value);
  Serial.println(value);
}

void setup() {
  Serial.begin(9600);
  Blynk.begin(auth , ssid, pass);
  pinMode(sensorPin, INPUT);
  pinMode(rainPin, INPUT);
  dht.begin();

  timer.setInterval(1000L, sendSensor);
  sensors.begin();
   
  pinMode(D4, OUTPUT);
  digitalWrite(D4, HIGH);
  timer.setInterval(100L, moisture);

  pinMode(trigPin, INPUT); // Sets the trigPin as an input
  pinMode(echoPin, OUTPUT);

}

int sensor=0;
void sendTemps()
{
sensor=analogRead(A0);
sensors.requestTemperatures();
float temp = sensors.getTempCByIndex(0); 
Serial.println(temp);
Serial.println(sensor);
Blynk.virtualWrite(V1, temp);
Blynk.virtualWrite(V2,sensor);
delay(1000);
}

void loop() {
  Blynk.run();
  timer.run();

  sendTemps();
  sensorState = digitalRead(sensorPin);
Serial.println(sensorState);

if (sensorState == 1 && lastState == 0) {
  Serial.println("needs water, send notification");
  Blynk.notify("Water your plants");
  lastState = 1;
  delay(1000);
//send notification
    
  } 
  else if (sensorState == 1 && lastState == 1) {
    //do nothing, has not been watered yet
  Serial.println("has not been watered yet");
  delay(1000);
  }
  else {
    //st
    Serial.println("does not need water");
    lastState = 0;
    delay(1000);
  }
  
  delay(100);

  rainState = digitalRead(rainPin);
Serial.println(rainState);

  if (rainState == 0 && lastRainState == 0) {
  Serial.println("Its Raining!");
  Blynk.notify("Its Raining!");
  lastRainState = 1;
  delay(1000);
//send notification
    
  } 
  else if (rainState == 0 && lastRainState == 1) {
  delay(1000);
  }
  else {
    Serial.println("No Rains");
    lastRainState = 0;
    delay(1000);
  }// Clears the trigPin
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);// Sets trigPin on HIGH for 10 micro seconds
  digitalWrite(trigPin, LOW);
  duration = pulseIn(echoPin, HIGH);// Reads the echoPin
  //returns the sound wave travel time in microseconds
  distanceCm = duration * SOUND_VELOCITY/2;// Calculate the distance
  distanceInch = distanceCm * CM_TO_INCH;// Convert to inches
  // Prints the distance on the Serial Monitor
  Serial.print("Distance (cm): ");
  Serial.println(distanceCm);
  Blynk.notify("Object is appearinig near to garden");
  Serial.print("Distance (inch): ");
  Serial.println(distanceInch);
  delay(1000);
}
