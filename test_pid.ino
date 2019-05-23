//TODO: Hacer que si lleva más de 10 segundos en la posición central le pegue un meneo y pite

#include <LiquidCrystal.h>


#include "config.h"
#include <Servo.h>


#define SENSOR_DIST A0
#define SERVO 9
#define LED_EN_POSICION 8

LiquidCrystal lcd(LCD_RS, LCD_E, LCD_D4, LCD_D5, LCD_D6, LCD_D7);

Servo servo;

void setup() {
  Serial.begin(115200);
  pinMode(BUZZER, OUTPUT);
  servo.attach(SERVO);
  pinMode(LED_EN_POSICION, OUTPUT);
  servo.write(SERVO_POS_CENTRAL + 10);
  servo.write(SERVO_POS_CENTRAL);
  /*while(1){
    int med=coger_medida();
    if (med) Serial.println(med);
  }*/
  lcd.begin(16, 2);
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("  Control PID   ");
  lcd.setCursor(0, 1);
  lcd.print(" Sindormir.net  ");
}

void loop() {
  static unsigned long last_loop_time = millis();
  unsigned long medida;

  if (millis() > (last_loop_time + LOOP_TIME)) {
    last_loop_time = millis();
    medida = coger_medida();
    //medida = get_dist();

    if (medida) {
      //Serial.println(medida);
      //control_p(medida);
      control_pid(medida);
      //todo_nada(medida);
    }

    //
    //control_p_no_calibrado(medida);
    //mide_dist(medida);
  }
}

int control_pid(long medida) {
  static unsigned long last_medida = 0;
  static unsigned long tiempo_en_centro = 0;
  static unsigned long ultimo_tiempo_en_centro = 0;
  long termino_p;
  static long termino_i = 0;
  long termino_d;

  long cambio;
  long error;
  long correccion;
  long angulo;
  long uSecsParaServo;
  
  error = medida - PELOTA_PUNTO_CENTRAL_MM;
  cambio = medida - last_medida;
  last_medida = medida;

  termino_p = error / 1000;  //Lo pasamos a cm

  termino_i +=  error / 1000 ;
  //if (termino_i >= MAX_I_TERM_VALUE) termino_i = MAX_I_TERM_VALUE;
  //if (termino_i <= -MAX_I_TERM_VALUE) termino_i = -MAX_I_TERM_VALUE;
  termino_i = constrain(termino_i, -MAX_I_TERM_VALUE , MAX_I_TERM_VALUE);

  termino_d = (cambio / LOOP_TIME); //Velocidad de cambio

  correccion = termino_p * Kp + termino_i * Ki + termino_d * Kd;
  if (error < 1000 && error > -1000) { //Era 750
    correccion = 0;
    digitalWrite(LED_EN_POSICION, HIGH);
    ultimo_tiempo_en_centro = millis();
    tiempo_en_centro++;

  }
  else {
    digitalWrite(LED_EN_POSICION, LOW);
    tiempo_en_centro = 0;
  }

  if (millis() - ultimo_tiempo_en_centro < 10) {
    if (tiempo_en_centro > 150) {
      //Serial.print(millis());
      tone(BUZZER, 1000, 1000);
      //Serial.println(" En centro!!!");
      tiempo_en_centro = 0;
      servo.write(45);
      delay(1000);
      noTone(BUZZER);
    }
  }

  uSecsParaServo=SERVO_US_HORIZ - correccion * 10; //Era 10 el factor de multiplicación
  

  /*Serial.print(error/1000);
  Serial.print(" ");
  Serial.print(termino_p * Kp);
  Serial.print(" ");
  Serial.print(termino_i * Ki);
  Serial.print(" ");
  Serial.print(termino_d * Kd);
  Serial.print(" ");
  Serial.println(correccion);*/
  Serial.print(0); //Referencia
  Serial.print(" ");
  Serial.print(uSecsParaServo);
  Serial.print(" ");
  Serial.println(error);

  //servo.writeMicroseconds(constrain(uSecsParaServo, 1166, 1900)); 
  servo.writeMicroseconds(uSecsParaServo); 
}


void control_p(int medida) {
  long correccion;
  long angulo;

  angulo = map(medida, 6600, 26700, SERVO_US_MAX_ANGLE, -SERVO_US_MAX_ANGLE);
  servo.writeMicroseconds(SERVO_US_HORIZ + angulo * Kp);
}


void control_p_old(int medida) {
  long correccion;
  long angulo;

  correccion = map(medida, 26700, 6600, -SERVO_MAX_ANGLE, SERVO_MAX_ANGLE);
  Serial.print("medida es: ");
  Serial.print(medida);
  Serial.print(" Correccion es: ");
  Serial.print(correccion);
  Serial.print(" angulo: ");
  angulo = SERVO_POS_CENTRAL + constrain(correccion, -SERVO_MAX_ANGLE, +SERVO_MAX_ANGLE) * Kp;
  servo.write(angulo);
  Serial.println(angulo);
}

void control_p_no_calibrado(int medida) {
  int angulo;
  angulo = map(medida, 308, 958, SERVO_POS_CENTRAL - SERVO_MAX_ANGLE, SERVO_POS_CENTRAL + SERVO_MAX_ANGLE);
  servo.write(angulo);
}


void control_p_no_calibrado_zona_muerta(int medida) {
  int angulo;
  if (medida < PELOTA_PUNTO_CENTRAL_MM - 45 || medida > PELOTA_PUNTO_CENTRAL_MM + 45) {
    angulo = map(medida, 308, 958, SERVO_POS_CENTRAL - SERVO_MAX_ANGLE, SERVO_POS_CENTRAL + SERVO_MAX_ANGLE);
  }
  else {
    angulo = SERVO_POS_CENTRAL;
  }
  servo.write(angulo);
}

void todo_nada(int lectura) {
  if (lectura > PELOTA_PUNTO_CENTRAL_MM ) {
    servo.write(SERVO_POS_CENTRAL - SERVO_MAX_ANGLE);
  }
  else {
    servo.write(SERVO_POS_CENTRAL + SERVO_MAX_ANGLE);
  }
}
