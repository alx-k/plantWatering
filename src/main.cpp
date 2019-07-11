#include <Arduino.h>
#include "Nextion.h"




#define CALMIN 680
#define CALMAX 280
#define POTCOUNT 1
#define HUMIDITY_THRESHOLD 40
#define WATERING_DURATION 10000
#define RECENT_THRESHOLD 60000
#define DELAYTIME 10000

int analogPins[] = {54,55,56,57};
int pumpPWMPins[] = {2,3,4,5};
int pumpDirPins[] = {22,24,26,28};
unsigned long watered_at[POTCOUNT];
char textBuffer[100] = {0};

NexGauge humid_gauges[POTCOUNT] = {NexGauge(0,1, "z0")/*,NexGauge(0,3, "z1"),NexGauge(0,4, "z2"),NexGauge(0,6, "z3")*/};
NexText ago_texts[POTCOUNT] = {NexText(0,2,"t0")/*,NexText(0,8,"t1"),NexText(0,5,"t2"),NexText(0,7,"t3")*/};

void setup() {
  Serial.begin(9600);
  nexInit();
  
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
  //Serial.println(analogRead(analogPins[potNumber]));
  return percent;
}

int calcGaugeVal(int percent){
  if (percent>100)
  {
    percent=100;
  }else if (percent<0)
  {
    percent=0;
  }
  int gaugeVal = map(percent,0,100,0,270);
  if (gaugeVal<=45)
  {
    gaugeVal += 315;
  }else
  {
    gaugeVal -= 45;
  }
  return gaugeVal;
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

void updateDisplay(int gaugeVals[], int humidities[]){
  for (int i = 0; i < POTCOUNT; i++)
  {
    humid_gauges[i].setValue(gaugeVals[i]);
    uint16_t numString = humidities[i];
    memset(textBuffer, 0, sizeof(textBuffer));
    itoa(numString, textBuffer, 10);
    ago_texts[i].setText(textBuffer);
  }
}

void loop() {
  while (true)
  {
    int humidities[POTCOUNT];
    int gaugeVals[POTCOUNT];
    for (int i = 0; i < POTCOUNT; i++)
    {
      humidities[i] = getHumidity(i);
      gaugeVals[i] = calcGaugeVal(humidities[i]);
      if(humidities[i] < HUMIDITY_THRESHOLD && !recentlyWatered(i)){
        pumpWater(i, WATERING_DURATION);
        watered_at[i] = millis();
      }
    }
    updateDisplay(gaugeVals,humidities);
    delay(DELAYTIME);
  }

  int percent = getHumidity(0);
  int humidities[POTCOUNT];
  int gaugeVals[POTCOUNT];
  for (int i = 0; i < POTCOUNT; i++)
  {
    humidities[i] = percent;
    gaugeVals[i] = calcGaugeVal(humidities[i]);
  }
  updateDisplay(gaugeVals,humidities);

  pumpWater(0, 10000);
  delay(10000);
  
}