#include <Arduino.h>

#define PRETO 1
#define BRANCO 0

#define motorE1 9  // motor direito para frente
#define motorE2 10 // motor direito para tras
#define motorD1 11 // motor esquero para frente
#define motorD2 3  // motor esquerdo para tras

// sensores dianteiros
#define pin_sensor1 2 // 1o da direita
#define pin_sensor2 12
#define pin_sensor3 4
#define pin_sensor4 5
#define pin_sensor5 6 // mais a esquerda

#define VELOCIDADE_MAXIMA 150

bool sensor1 = 0, sensor2 = 0, sensor3 = 0, sensor4 = 0, sensor5 = 0;

int velocidadeMotorD = 0, velocidadeMotorE = 0, velocidadeMotorD2 = 0, velocidadeMotorE2 = 0;

float ERRO = 0, PID = 0, proporcional = 0, integral = 0, derivativo = 0, U_ERRO = 0;
float kp = 485;  // PRIMEIRO A REAGIR => quanto maior maior a primeira reacao
float ki = 0.35; // ACELERA AO LONGO DO TEMPO
float kd = 300;  // SUAVIZACAO

// Inicialize as velocidades com VELOCIDADE_MAXIMA
int velocidade_direita = VELOCIDADE_MAXIMA;
int velocidade_esquerda = VELOCIDADE_MAXIMA;


void setup() {
  Serial.begin(115200);
  // MOTORES
  pinMode(motorD1, OUTPUT);
  pinMode(motorD2, OUTPUT);
  pinMode(motorE1, OUTPUT);
  pinMode(motorE2, OUTPUT);
}

void loop() {
  lerEstadoSensores();
  calculaErro();
  calculaPID();
  controle_motor();
}

void lerEstadoSensores() {
  sensor1 = digitalRead(pin_sensor1);
  sensor2 = digitalRead(pin_sensor2);
  sensor3 = digitalRead(pin_sensor3);
  sensor4 = digitalRead(pin_sensor4);
  sensor5 = digitalRead(pin_sensor5);
}

void calculaErro() {
  float erro = 0;
  if (sensor1 == PRETO) {
    erro -= 0.265;
  } else if (sensor5 == PRETO) {
    erro += 0.265;
  } else if (sensor4 == PRETO) {
    erro += 0.0365;
  } else if (sensor2 == PRETO) {
    erro -= 0.0365;
  } else if (sensor3 == PRETO) {
    ERRO = 0;
  }
  ERRO += erro;
}

void calculaPID() {
  if (ERRO == 0) {
    integral = 0;
  }
  
  proporcional = ERRO;
  integral = constrain(integral + ERRO, -2000000, 2000000);
  derivativo = ERRO - U_ERRO;
  PID = ((kp * proporcional) + (ki * integral) + (kd * derivativo));
  U_ERRO = ERRO;
}

void controle_motor() {
  long ajuste = PID/100;

  if (ajuste >= 0) {
    velocidade_esquerda = VELOCIDADE_MAXIMA - ajuste;
    velocidade_direita = VELOCIDADE_MAXIMA ;
  } else {
    velocidade_esquerda = VELOCIDADE_MAXIMA;
    velocidade_direita = VELOCIDADE_MAXIMA + ajuste;
  }

  velocidade_esquerda = constrain(velocidade_esquerda, -VELOCIDADE_MAXIMA, VELOCIDADE_MAXIMA);
  velocidade_direita = constrain(velocidade_direita, -VELOCIDADE_MAXIMA, VELOCIDADE_MAXIMA);

  // Realiza curvas fechadas
  if (velocidade_esquerda < 0 && velocidade_direita > 0) {
    // motor da esquerda para tras
    velocidadeMotorE = 0;
    velocidadeMotorD = velocidade_direita;
    velocidadeMotorE2 = constrain((velocidade_esquerda * -1), 0, VELOCIDADE_MAXIMA/2);
    velocidadeMotorD2 = 0;
  } else if (velocidade_esquerda > 0 && velocidade_direita < 0) {
    // motor da direita para tras
    velocidadeMotorE = velocidade_esquerda;
    velocidadeMotorD = 0;
    velocidadeMotorE2 = 0;
    velocidadeMotorD2 = constrain((velocidade_direita * -1), 0, VELOCIDADE_MAXIMA/2);
  } else {
    velocidadeMotorE = constrain(velocidade_esquerda, 0, VELOCIDADE_MAXIMA);
    velocidadeMotorD = constrain(velocidade_direita, 0, VELOCIDADE_MAXIMA);
    velocidadeMotorE2 = 0;
    velocidadeMotorD2 = 0;
  }

  analogWrite(motorE1, velocidadeMotorE);
  analogWrite(motorE2, velocidadeMotorE2);
  analogWrite(motorD1, velocidadeMotorD);
  analogWrite(motorD2, velocidadeMotorD2);
}
