/*

Codigo modificado por GUILLE


 __________________________________________________________________
           *** {==[=======> (MANEJO DEL I2C CON PANTALLA LCD) <=======]==}  ***
           ___________________________________________________________________
*/
/*
 Board I2C / TWI pins
Uno       =   A4 (SDA), A5 (SCL)
Mega2560  =   20 (SDA), 21 (SCL)
Leonardo  =   2 (SDA), 3 (SCL)
Due       =   20 (SDA), 21 (SCL), SDA1, SCL1
esta configuracion de estos pines se encuentran dentro de la librería "wire" mas info: https://www.arduino.cc/en/Reference/Wire
 */
#include <LiquidCrystal_I2C.h> //DESCARGAR LIBRERÍA:https://github.com/ELECTROALL/Codigos-arduino/blob/master/LiquidCrystal_I2C.zip
LiquidCrystal_I2C lcd(0x27,16,2); // si no te sale con esta direccion  puedes usar (0x3f,16,2) || (0x27,16,2)  ||(0x20,16,2) 
void setup() {
  lcd.init();
  lcd.backlight();
  lcd.clear();
 lcd.setCursor(0,0);
  lcd.print("  Acerquese al"); 
  lcd.setCursor (0,1);
  lcd.print("sensor de Temp.");
}
void loop() { 
  lcd.display();
  delay(5000);
  lcd.noDisplay();
  delay(500);
}
