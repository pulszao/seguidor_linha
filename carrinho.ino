#include <Arduino.h>

#define PRETO 1
#define BRANCO 0

#define motorE1 9  // motor direito para frente
#define motorE2 10 // motor direito para tras
#define motorD1 11 // motor esquero para frente
#define motorD2 12 // motor esquerdo para tras

// sensores dianteiros
#define pin_sensor1 2 // 1o da direita
#define pin_sensor2 3
#define pin_sensor3 4
#define pin_sensor4 5
#define pin_sensor5 6 // mais a esquerda

// sensores rodas
#define pin_sensorRD 7
#define pin_sensorRE 8

// velocidades
#define VELOCIDADE_CURVA 255
#define VELOCIDADE_RETA 230
#define VELOCIDADE_MEDIA 190
#define VELOCIDADE_BAIXA 0

bool sensor1 = 0;
bool sensor2 = 0;
bool sensor3 = 0;
bool sensor4 = 0;
bool sensor5 = 0;
bool sensorRE = 0;
bool sensorRD = 0;

int ultimoSensorRD = 0;
int ultimoSensorRE = 0;
unsigned long ultimoTempoTransicaoRD = 0;
unsigned long ultimoTempoTransicaoRE = 0;
int frequenciaRD = 0;
int frequenciaRE = 0;

int velocidadeMotorD = 0;
int velocidadeMotorE = 0;
int velocidadeMotorD2 = 0;
int velocidadeMotorE2 = 0;

void setup() {
  Serial.begin(115200);
  // MOTORES
  pinMode(motorD1, OUTPUT);
  pinMode(motorD2, OUTPUT);
  pinMode(motorE1, OUTPUT);
  pinMode(motorE2, OUTPUT);
}

void loop() {
  sensor1 = digitalRead(pin_sensor1);
  sensor2 = digitalRead(pin_sensor2);
  sensor3 = digitalRead(pin_sensor3);
  sensor4 = digitalRead(pin_sensor4);
  sensor5 = digitalRead(pin_sensor5);

  if (sensor1 == PRETO) {
    // Curva fechada para a direita
    velocidadeMotorE = VELOCIDADE_CURVA;
    velocidadeMotorD = VELOCIDADE_BAIXA;
    velocidadeMotorE2 = VELOCIDADE_BAIXA;
    velocidadeMotorD2 = VELOCIDADE_RETA;
  } else if (sensor5 == PRETO) {
    // Curva fechada para a esquerda
    velocidadeMotorE = VELOCIDADE_BAIXA;
    velocidadeMotorD = VELOCIDADE_CURVA;
    velocidadeMotorE2 = VELOCIDADE_RETA;
    velocidadeMotorD2 = VELOCIDADE_BAIXA;
  } else if (sensor4 == PRETO) {
    // Curva leve para a direita
    velocidadeMotorE = VELOCIDADE_MEDIA;
    velocidadeMotorD = VELOCIDADE_RETA;
    velocidadeMotorE2 = VELOCIDADE_BAIXA;
    velocidadeMotorD2 = VELOCIDADE_BAIXA;
  }  else if (sensor2 == PRETO) {
    // Curva leve para a esquerda
    velocidadeMotorE = VELOCIDADE_RETA;
    velocidadeMotorD = VELOCIDADE_MEDIA;
    velocidadeMotorE2 = VELOCIDADE_BAIXA;
    velocidadeMotorD2 = VELOCIDADE_BAIXA;
  } else if (sensor3 == PRETO) {
    // Motores andam reto
    velocidadeMotorE = VELOCIDADE_RETA;
    velocidadeMotorD = VELOCIDADE_RETA;
    velocidadeMotorE2 = VELOCIDADE_BAIXA;
    velocidadeMotorD2 = VELOCIDADE_BAIXA;
  }

  analogWrite(motorE1, velocidadeMotorE);
  analogWrite(motorE2, velocidadeMotorE2);
  analogWrite(motorD1, velocidadeMotorD);
  analogWrite(motorD2, velocidadeMotorD2);
}