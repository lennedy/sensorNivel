#include "Sensor.h"

void Sensor::init(){
  pinMode(TRIG_PIN, OUTPUT); // Sets the trigPin as an Output
  pinMode(ECHO_PIN, INPUT); // Sets the echoPin as an Input
  SensorBaseMqtt::init();
}

StaticJsonDocument<100> Sensor::readSensor(){
  StaticJsonDocument<100> jsonDoc;
  
  const float SOUND_VELOCITY = 0.034;
  const float CM_TO_INCH     = 0.393701;

  long duration;
  float distanceCm;

  // Clears the trigPin
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);
  // Sets the trigPin on HIGH state for 10 micro seconds
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);

  // Reads the echoPin, returns the sound wave travel time in microseconds
  duration = pulseIn(ECHO_PIN, HIGH);
  
  // Calculate the distance
  distanceCm = duration * SOUND_VELOCITY/2;

  jsonDoc["value"] = distanceCm;
  if(distanceCm > 210 || distanceCm == 0){
    jsonDoc["erro"] = true;
  }
  else{
    jsonDoc["erro"] = false;
  }
  
  return jsonDoc;
}

void Sensor::metodoPublisher(){
  static unsigned int amostrasTotais = 0;  //variável para realizar o filtro de média
  static unsigned int amostrasValidas = 0; //variável para realizar o filtro de média
  static float acumulador = 0;       //variável para acumular a média
  
  StaticJsonDocument<100> jsonSensor;
  
  jsonSensor = readSensor();

  //Realização de tratamento de erros
  if( jsonSensor["erro"] == false ){
    float temp  = jsonSensor["value"];
    acumulador += temp; //somente os valores sem erro serão utilizados na média
    amostrasValidas++;  //incremento das amostras onde não foram encontradas erros
  }
  amostrasTotais++;  //incremento de amostras total

  //realização de média
  if (amostrasTotais >= JANELA_FILTRO){
    StaticJsonDocument<300> jsonDoc;

    jsonDoc["RSSI"]     = WiFi.RSSI();
    jsonDoc["nivel"]    = 0;
    jsonDoc["erro"]     = false;
    jsonDoc["invalido"] = false;

    //se amostras válidas forem diferentes das amostras totais houve erro em alguma aquisição de dados
    if(amostrasTotais != amostrasValidas){ 
      if(amostrasValidas > 0){
        jsonDoc["nivel"] = acumulador/amostrasValidas;
      }
      else{ //Se amostras válidas for igual a 0 o valor será zero e o conjunto de dados inválidos
        jsonDoc["nivel"] = 0;
        jsonDoc["invalido"] = true;
      }
      
      jsonDoc["erro"] = true;
    }
    else{ //todos os dados são válidos
      jsonDoc["nivel"] = acumulador/JANELA_FILTRO;
      jsonDoc["erro"]  = false;
    }
  
    String payload = "";
    serializeJson(jsonDoc, payload);

    client.publish(topic_name, payload); 
    amostrasTotais = 0; 
    amostrasValidas = 0;
    acumulador = 0;
  }
    
}