#include <Arduino.h>
#include <TimerOne.h>

#define PRETO 1
#define BRANCO 0

#define motorE1 5  // motor direito para frente
#define motorE2 6  // motor direito para tras
#define motorD1 11 // motor esquero para frente
#define motorD2 3  // motor esquerdo para tras

// sensores dianteiros
#define pin_sensor1 2 // 1o da direita
#define pin_sensor2 12
#define pin_sensor3 4
#define pin_sensor4 9
#define pin_sensor5 10 // mais a esquerda

#define VELOCIDADE_MAXIMA 150

float kp = 850;  // PRIMEIRO A REAGIR => quanto maior maior a primeira reacao
float ki = 1.1;  // ACELERA AO LONGO DO TEMPO
float kd = 30;   // SUAVIZACAO

volatile bool sensor1 = 0, sensor2 = 0, sensor3 = 0, sensor4 = 0, sensor5 = 0;

volatile float erro = 0;
volatile float ERRO = 0, PID = 0, proporcional = 0, integral = 0, derivativo = 0, U_ERRO = 0;
int PARADA = 0;

// Inicialize as velocidades com VELOCIDADE_MAXIMA
volatile int velocidade_direita = VELOCIDADE_MAXIMA;
volatile int velocidade_esquerda = VELOCIDADE_MAXIMA;

void setup() {
  Serial.begin(115200);
  // MOTORES
  pinMode(motorD1, OUTPUT);
  pinMode(motorD2, OUTPUT);
  pinMode(motorE1, OUTPUT);
  pinMode(motorE2, OUTPUT);

  // Inicializa TimerOne
  Timer1.initialize(1000); // intervalo de 10ms
  Timer1.attachInterrupt(timerIsr); // liga a interrupção do timer
}

void loop() {
  if(PARADA == 1) {
    analogWrite(motorE1, 0);
    analogWrite(motorE2, 0);
    analogWrite(motorD1, 0);
    analogWrite(motorD2, 0);
    delay(15000);
    PARADA = 0;
  } else {
    controle_motor();
  }
}

void timerIsr() {
  if(PARADA == 0) {
    lerEstadoSensores();
    calculaErro();
    calculaPID();
  }
}

void lerEstadoSensores() {
  sensor1 = digitalRead(pin_sensor1);
  sensor2 = digitalRead(pin_sensor2);
  sensor3 = digitalRead(pin_sensor3);
  sensor4 = digitalRead(pin_sensor4);
  sensor5 = digitalRead(pin_sensor5);
}

void calculaErro() {
  if (sensor1 == PRETO && sensor2 == PRETO && sensor3 == PRETO && sensor4 == PRETO && sensor5 == PRETO) {
    PARADA = 1;
  } else if (sensor1 == PRETO) {
    if(ERRO > 0) zeraErro();
    erro -= 0.0002;
  } else if (sensor5 == PRETO) {
    if(ERRO < 0) zeraErro();
    erro += 0.0002;
  } else if (sensor4 == PRETO) {
    if(ERRO < 0) zeraErro();
    erro += 0.00001;
  } else if (sensor2 == PRETO) {
    if(ERRO > 0) zeraErro();
    erro -= 0.00001;
  } else if (sensor3 == PRETO) {
    zeraErro();
  }
  erro = constrain(erro, -2000000, 2000000);
  ERRO += erro;
}

void zeraErro() {
  ERRO = 0;
  erro = 0;
}

void calculaPID() {
  if (ERRO == 0) {
    integral = 0;
  }
  
  proporcional = constrain(ERRO, -2000000, 2000000);
  integral = constrain(integral + ERRO, -2000000, 2000000);
  derivativo = constrain(ERRO - U_ERRO, -2000000, 2000000);
  PID = integral = constrain(((kp * proporcional) + (ki * integral) + (kd * derivativo)), -2000000, 2000000);
  U_ERRO = ERRO;
}

void controle_motor() {
  long ajuste = PID / 100;

  if (ajuste >= 0) {
    velocidade_esquerda = VELOCIDADE_MAXIMA - ajuste;
    velocidade_direita = VELOCIDADE_MAXIMA;
  } else {
    velocidade_esquerda = VELOCIDADE_MAXIMA;
    velocidade_direita = VELOCIDADE_MAXIMA + ajuste;
  }

  velocidade_esquerda = constrain(velocidade_esquerda, -VELOCIDADE_MAXIMA, VELOCIDADE_MAXIMA);
  velocidade_direita = constrain(velocidade_direita, -VELOCIDADE_MAXIMA, VELOCIDADE_MAXIMA);

  // Realiza curvas fechadas
  if (velocidade_esquerda < 0 && velocidade_direita > 0) {
    // motor da esquerda para tras
    analogWrite(motorE1, 0);
    analogWrite(motorE2, constrain((velocidade_esquerda * -1), 0, VELOCIDADE_MAXIMA / 2.5));
    analogWrite(motorD1, velocidade_direita);
    analogWrite(motorD2, 0);
  } else if (velocidade_esquerda > 0 && velocidade_direita < 0) {
    // motor da direita para tras
    analogWrite(motorE1, velocidade_esquerda);
    analogWrite(motorE2, 0);
    analogWrite(motorD1, 0);
    analogWrite(motorD2, constrain((velocidade_direita * -1), 0, VELOCIDADE_MAXIMA / 2.5));
  } else {
    analogWrite(motorE1, constrain(velocidade_esquerda, 0, VELOCIDADE_MAXIMA));
    analogWrite(motorE2, 0);
    analogWrite(motorD1, constrain(velocidade_direita, 0, VELOCIDADE_MAXIMA));
    analogWrite(motorD2, 0);
  }
}
