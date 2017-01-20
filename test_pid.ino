#include "config.h"
#include <Servo.h>

#define SENSOR_DIST A0
#define SERVO 9
#define LED_EN_POSICION 8

Servo servo;

void setup(){
   Serial.begin(115200);
   servo.attach(SERVO);
   pinMode(LED_EN_POSICION, OUTPUT);
   servo.write(SERVO_POS_CENTRAL+10);
}

void loop(){
   static unsigned long last_loop_time=millis();
   unsigned long medida;

   if (millis() > (last_loop_time + LOOP_TIME)){
      last_loop_time=millis();
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

int control_pid(long medida){
   static unsigned long last_medida=0;
   long termino_p;
   static long termino_i = 0;
   long termino_d;

   long cambio;
   long error;
   long correccion;
   long angulo;

   error = medida - PELOTA_PUNTO_CENTRAL_MM;
   cambio = medida - last_medida;
   last_medida=medida;

   termino_p = error / 1000;  //Lo pasamos a cm

   termino_i +=  error / 1000 ;
   if (termino_i >= MAX_I_TERM_VALUE) termino_i=MAX_I_TERM_VALUE;
   if (termino_i <= -MAX_I_TERM_VALUE) termino_i=-MAX_I_TERM_VALUE;

   termino_d = (cambio / LOOP_TIME); //Velocidad de cambio

   correccion=termino_p*Kp+termino_i*Ki+termino_d*Kd;
   if (error < 750 && error > -750) {
    correccion=0;
    digitalWrite(LED_EN_POSICION, HIGH);
   }
   else{
    digitalWrite(LED_EN_POSICION, LOW);
   }

   //Serial.print(error);
   //Serial.print(" ");
   Serial.print(termino_p*Kp);
   Serial.print(" ");
   Serial.print(termino_i*Ki);
   Serial.print(" ");
   Serial.print(termino_d*Kd);
   Serial.print(" ");
   Serial.println(correccion);

   servo.writeMicroseconds(SERVO_US_HORIZ-correccion*10);
}


void control_p(int medida){
   long correccion;
   long angulo;

   angulo = map(medida, 6600, 26700, SERVO_US_MAX_ANGLE, -SERVO_US_MAX_ANGLE);
   servo.writeMicroseconds(SERVO_US_HORIZ+angulo*Kp);
}


void control_p_old(int medida){
   long correccion;
   long angulo;

   correccion = map(medida, 26700, 6600, -SERVO_MAX_ANGLE, SERVO_MAX_ANGLE);
   Serial.print("medida es: ");
   Serial.print(medida);
   Serial.print(" Correccion es: ");
   Serial.print(correccion);
   Serial.print(" angulo: ");
   angulo = SERVO_POS_CENTRAL + constrain(correccion, -SERVO_MAX_ANGLE, +SERVO_MAX_ANGLE)*Kp;
   servo.write(angulo);
   Serial.println(angulo);
}

void control_p_no_calibrado(int medida){
   int angulo;
   angulo = map(medida, 308, 958, SERVO_POS_CENTRAL-SERVO_MAX_ANGLE, SERVO_POS_CENTRAL+SERVO_MAX_ANGLE);
   servo.write(angulo);
}


void control_p_no_calibrado_zona_muerta(int medida){
   int angulo;
   if (medida < PELOTA_PUNTO_CENTRAL_MM-45 || medida > PELOTA_PUNTO_CENTRAL_MM+45){
      angulo = map(medida, 308, 958, SERVO_POS_CENTRAL-SERVO_MAX_ANGLE, SERVO_POS_CENTRAL+SERVO_MAX_ANGLE);
   }
   else{
      angulo = SERVO_POS_CENTRAL;
   }
   servo.write(angulo);
}

void todo_nada(int lectura){
   if (lectura > PELOTA_PUNTO_CENTRAL_MM ){
      servo.write(SERVO_POS_CENTRAL-SERVO_MAX_ANGLE);
   }
   else{
      servo.write(SERVO_POS_CENTRAL+SERVO_MAX_ANGLE);
   }
}
