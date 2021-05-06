void setup () {
     Serial.begin(9600);
     while(!Serial) { ; }
}
void loop () {
     if( Serial.available()> 0) {
          
        String str = Serial.readStringUntil('\n'); // Lectura de una cadena
        str.toUpperCase(); // Conversión a mayúsculas
        Serial.println(str); // Envío por el puerto serie
      }
     delay(20);
}
