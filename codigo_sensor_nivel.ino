#include <LiquidCrystal.h>
LiquidCrystal lcd(2, 8, 4, 5, 6, 7);

#define out555 3
#define trigger 9
#define botao 10
#define botaoConfirma 11

volatile unsigned long pulseBegin = micros();
volatile unsigned long pulseEnd = micros();
volatile bool pulseTimeAvailable = false;
volatile unsigned long tempoatual = 0;
unsigned long pulseDuration;
float m = 0;
float b = 0;
int estadoBotao;
int estadoBotaoConfirma;
bool modoCalibracao = false;
unsigned long tempoPressionado = 0;
unsigned long tempoPressionado2 = 0;
int state;
float capacitancia_5cm;
float capacitancia_20cm;

void buttonPinInterrupt() {
  if (digitalRead(out555) == HIGH) {
    tempoatual = micros();
    pulseBegin = micros() - tempoatual;
  } else {
    pulseEnd = micros() - tempoatual;
    pulseTimeAvailable = true;
  }
}

void setup() {
  lcd.begin(16, 2);
  Serial.begin(9600);
  pinMode(out555, INPUT);
  pinMode(trigger, OUTPUT);
  pinMode(botao, INPUT);
  pinMode(botaoConfirma, INPUT);

  attachInterrupt(digitalPinToInterrupt(out555),
                  buttonPinInterrupt,
                  CHANGE);
}

void loop() {

  if (pulseTimeAvailable) {
    pulseTimeAvailable = false;
    pulseDuration = pulseEnd - pulseBegin;
    Serial.print("duracao: ");
    Serial.println(pulseDuration);
    nivel(medicao());
  }
  digitalWrite(trigger, HIGH);
  delay(200);
  digitalWrite(trigger, LOW);

  estadoBotao = digitalRead(botao);
  if (estadoBotao == HIGH) {
    tempoPressionado = millis();  // Salva o tempo atual
    while (digitalRead(botao) == HIGH) {
    }                                          // Aguarda até o botão ser liberado
    if (millis() - tempoPressionado >= 200) {  // Botão pressionado por 0,2 segundos
      modoCalibracao = true;
      state = 0;
    }
  }

  estadoBotaoConfirma = digitalRead(botaoConfirma);
  if (estadoBotaoConfirma == HIGH) {
    tempoPressionado2 = millis();  // Salva o tempo atual
    while (digitalRead(botaoConfirma) == HIGH) {
    }                                           // Aguarda até o botão ser liberado
    if (millis() - tempoPressionado2 >= 200) {  // Botão pressionado por 0,2 segundos
      state++;
    }
  }


  if (modoCalibracao) {
    switch (state) {
      case 0:
        lcd.clear();
        lcd.print("Coloque 5cm de");
        lcd.setCursor(0, 1);
        lcd.print("agua e confirme");
        capacitancia_5cm = medicao();
        Serial.print("Capacitancia: ");
        Serial.println(capacitancia_5cm);

        break;
      case 1:
        lcd.clear();
        lcd.print("Coloque 20cm de");
        lcd.setCursor(0, 1);
        lcd.print("agua e confirme");
        capacitancia_20cm = medicao();
        Serial.print("Capacitancia: ");
        Serial.println(capacitancia_20cm);
        break;
      case 2:
        lcd.clear();
        lcd.print("Cap 5 cm: ");
        lcd.print(capacitancia_5cm);
        lcd.setCursor(0, 1);
        lcd.print("Cap 20 cm: ");
        lcd.print(capacitancia_20cm);
        break;
      case 3:
        //finalizando calibração
        m = (20 - 5) / (capacitancia_20cm - capacitancia_5cm);
        b = 5 - (m * capacitancia_5cm);
        modoCalibracao = false;
        break;
      default:
        break;
    }
  }
}

float medicao() {
  float capacitancia = (pulseDuration / (1.1 * 1000));
  return capacitancia;
}

void nivel(float capacitancia) {
  if (!modoCalibracao) {
    lcd.clear();
    lcd.print("Nivel:");
    lcd.setCursor(0, 1);
    Serial.print("Capacitancia: ");
    Serial.println(capacitancia);
    float nivel = (m * capacitancia) + b;
    lcd.print(nivel);
    lcd.print(" cm");
    if (nivel == 0) {
      lcd.clear();
      lcd.print("Calibre o sensor");
    }
  }
}