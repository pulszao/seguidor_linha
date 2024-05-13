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
#define VELOCIDADE_CURVA 250
#define VELOCIDADE_RETA 150
#define VELOCIDADE_MEDIA 80
#define VELOCIDADE_BAIXA 0

// Coeficientes PID ajustados
#define KP 0.4  // Reduzido para diminuir a sensibilidade às pequenas variações
#define KI 0.1  // Ajustado para acumular menos erro integral
#define KD 0.3  // Reduzido para diminuir a resposta a mudanças rápidas de erro

// Variáveis PID
float erroPID = 0;
float erroAnterior = 0;
float somaErro = 0;
float deltaErro = 0;
unsigned long tempoAnterior = 0;

// Definir um valor de referência para a posição central
#define POSICAO_REFERENCIA 3  // Supondo que o sensor3 é a posição central

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
  } else if (sensor5 == PRETO) {
    // Curva fechada para a esquerda
    velocidadeMotorE = VELOCIDADE_BAIXA;
    velocidadeMotorD = VELOCIDADE_CURVA;
  } else if (sensor4 == PRETO) {
    // Curva leve para a direita
    velocidadeMotorE = VELOCIDADE_MEDIA;
    velocidadeMotorD = VELOCIDADE_RETA;
  }  else if (sensor2 == PRETO) {
    // Curva leve para a esquerda
    velocidadeMotorE = VELOCIDADE_RETA;
    velocidadeMotorD = VELOCIDADE_MEDIA;
  } else if (sensor3 == PRETO) {
    // Motores andam reto
    velocidadeMotorE = VELOCIDADE_RETA;
    velocidadeMotorD = VELOCIDADE_RETA;
  } else {
    calcularPID();
  }

  analogWrite(motorE1, velocidadeMotorE);
  analogWrite(motorD1, velocidadeMotorD);

  delay(5);
}

void calcularPID() {
  int posicaoAtual = lerPosicaoAtual();

  erroPID = POSICAO_REFERENCIA - posicaoAtual;
  somaErro += erroPID;
  unsigned long tempoAtual = millis();
  
  if (tempoAtual - tempoAnterior >= 10) { // Atualização a cada 10 ms
    deltaErro = (erroPID - erroAnterior) / (tempoAtual - tempoAnterior);
    float ajuste = KP * erroPID + KI * somaErro + KD * deltaErro;

    // escreve a velocidade ajustada para as variaveis globais da veloc dos motores
    velocidadeMotorE = constrain(120 + ajuste, 0, 255);
    velocidadeMotorD = constrain(120 - ajuste, 0, 255);

    tempoAnterior = tempoAtual;
    erroAnterior = erroPID;
  }
}


int lerPosicaoAtual() {
  if (sensor1 == PRETO) return 1;
  if (sensor2 == PRETO) return 2;
  if (sensor3 == PRETO) return 3;
  if (sensor4 == PRETO) return 4;
  if (sensor5 == PRETO) return 5;
  return 3; // Se nenhum sensor detectar preto, assumir centro
}

void frequenciaMotor(int ldrPin, int& lastSensorValue, unsigned long& lastTransitionTime, int& frequency) {
  int sensorValue = digitalRead(ldrPin);

  if (sensorValue == HIGH && lastSensorValue == LOW) {
    unsigned long currentTime = millis();
    unsigned long timeSinceLastTransition = currentTime - lastTransitionTime;
    lastTransitionTime = currentTime;

    // Calcular frequência (transições por segundo)
    frequency = 1000 / timeSinceLastTransition;
  }

  lastSensorValue = sensorValue;
}