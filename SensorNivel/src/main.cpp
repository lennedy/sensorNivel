#include <Arduino.h>

#include "SensorBaseMqtt.h"
#include "Sensor.h"

Sensor sensor;

void setup() {
  Serial.begin(115200);
  sensor.init();
}

void onConnectionEstablished()
{ 
  sensor.availableSignal();
}

void loop()
{
  sensor.loop();
}