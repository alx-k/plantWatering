#include <Arduino.h>
#include "Nextion.h"




#define CALMIN 660
#define CALMAX 285
#define POTCOUNT 1
#define HUMIDITY_THRESHOLD 40
#define WATERING_DURATION 10000
#define RECENT_THRESHOLD 60000

int analogPins[] = {54,55,56,57};
int pumpPWMPins[] = {2,3,4,5};
int pumpDirPins[] = {22,24,26,28};
unsigned long watered_at[POTCOUNT];

NexGauge humid_gauges[POTCOUNT];
NexText ago_hours[POTCOUNT];
NexText ago_mins[POTCOUNT];

void setup() {
  Serial.begin(9600);
  //nexInit();
  
  for (int i = 0; i < POTCOUNT; i++)
  {
    pinMode(pumpPWMPins[i], OUTPUT);
    pinMode(pumpDirPins[i], OUTPUT);
    pinMode(pumpDirPins[i]+1, OUTPUT);
    digitalWrite(pumpPWMPins[0], LOW);
    digitalWrite(pumpDirPins[0], LOW);
    digitalWrite(pumpDirPins[0]+1, LOW);
  }
  
}

void pumpWater(int potNumber, int duration){
  analogWrite(pumpPWMPins[potNumber], 255);
  digitalWrite(pumpDirPins[potNumber], HIGH);
  delay(duration);
  analogWrite(pumpPWMPins[potNumber], LOW);
  digitalWrite(pumpDirPins[potNumber], LOW);

}

int getHumidity(int potNumber){
  int percent = map(analogRead(analogPins[potNumber]),CALMIN,CALMAX,0,100);
  return percent;
}

bool recentlyWatered(int potNumber){
  if (isnan(watered_at[potNumber]))
  {
    return false;
  }
  unsigned long ago = millis() - watered_at[potNumber];
  if (ago > RECENT_THRESHOLD)
  {
    return false;
  }
  return true;
}

void loop() {
  int percent = getHumidity(0);
  Serial.println(percent);
  Serial.println(millis());
  //pumpWater(0, 10000);
  delay(5000);
  while (false)
  {
    int humidities[POTCOUNT];
    for (int i = 0; i < POTCOUNT; i++)
    {
      humidities[i] = getHumidity(i);
      if(humidities[i] < HUMIDITY_THRESHOLD && !recentlyWatered(i)){
        pumpWater(i, WATERING_DURATION);
        watered_at[i] = millis();
      }
    }
    //TODO: Variablen ans Display senden
  }
  
}