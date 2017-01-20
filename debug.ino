
unsigned long coger_medida(){
   static unsigned long lecturas=0;
   static byte contador_lecturas=0;
   unsigned long media=0;

   contador_lecturas++;
   lecturas+=get_dist();

   if (contador_lecturas>=NUM_MEDIDAS){
      media=lecturas/contador_lecturas;
      contador_lecturas=0;
      lecturas=0;
      return media;
   }
   return 0;
}


long int get_dist(){
   unsigned long int mm;
   mm = pow(3027.4 / analogRead(SENSOR_DIST), 1.2134) * 1000;
   //Serial.print(" cm -> ");
   //Serial.println(cm);
   return mm;
}
