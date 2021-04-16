// Librerías utilizadas
#include <Wire.h> //LIBRERIA COMUNICACION 
#include <LiquidCrystal_I2C.h> //LIBRERIA LCD
#include <Adafruit_MLX90614.h> //LIBRERIA TERMOMETRO INFRARROJO

// Se declara el objeto
Adafruit_MLX90614 mlx = Adafruit_MLX90614();

// Ajusta la dirección de la LCD a 0x27 para una pantalla de 16 caracteres y 2 lineas
LiquidCrystal_I2C lcd(0x27, 16, 2);

void setup() {

// Inicializa la LCD y el MLX90614
  lcd.init();
  mlx.begin();

  // Enciende la retroiluminación.
  lcd.backlight();

  //bucle inicializacion (DETALLE)
  lcd.setCursor(0, 0);
  for (int z1=1;z1<=3;z1++){
    lcd.print("Iniciando");
    for (int z2=1;z2<=3;z2++){
      delay(400);
      lcd.print(".");
      }
      delay(400);
      lcd.clear();
    }
    
}

  void loop() {

    //Imprimir temperatuta
  lcd.setCursor(0, 0);
  lcd.print("Ambient: "); lcd.print(mlx.readAmbientTempC());  lcd.print("C");
  lcd.setCursor(0, 1);
  lcd.print("Object:  "); lcd.print(mlx.readObjectTempC());   lcd.print("C");
  delay(250);
  
}






  
