#ifndef SENSOR_H
#define SENSOR_H

#include "SensorBaseMqtt.h"
#include <ArduinoJson.h>

#define TRIG_PIN 12
#define ECHO_PIN 14

#define JANELA_FILTRO 10         // Número de amostras do filtro para realizar a média

class Sensor: public SensorBaseMqtt {
  private:
  StaticJsonDocument<100> readSensor();

  public:
  void metodoPublisher();
  void init();

};

#endif