// Librerías utilizadas
#include <Wire.h> //LIBRERIA COMUNICACION 
#include <LiquidCrystal_I2C.h> //LIBRERIA LCD
#include <Adafruit_MLX90614.h> //LIBRERIA TERMOMETRO INFRARROJO
#include <SR04.h> // Libreria ultrasonico 

// Definicion de pines ultrasonido
#define Echo 30 
#define Trig 32 



// Se declara el objeto
Adafruit_MLX90614 mlx = Adafruit_MLX90614();


// Ajusta la dirección de la LCD a 0x27 para una pantalla de 16 caracteres y 2 lineas
LiquidCrystal_I2C lcd(0x27, 16, 2);

void setup() {
  pinMode(40, OUTPUT); // Pin ZUMBADOR


// Inicializa la LCD y el MLX90614
  lcd.init();
  mlx.begin();

  // Enciende la retroiluminación.
  lcd.backlight();


/*
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
    }*/
    
}

  void loop() {


    //Imprimir temperatuta
  lcd.setCursor(0, 0);
  lcd.print("Ambient: "); lcd.print(mlx.readAmbientTempC());  lcd.print("C");
  lcd.setCursor(0, 1);
  lcd.print("Object:  "); lcd.print(mlx.readObjectTempC());   lcd.print("C");
  delay(250);


    //Imprimir temperatuta
  lcd.setCursor(0, 0);
  lcd.print("MEDIA: "); lcd.print(Medir_temperatura());  lcd.print("C");


Error_temperatura();
  delay(2500);





/*
//ALARMA COVID
  if (mlx.readObjectTempC()>=30){
      int o1=0;
      for (int o1=1;o1<=3;o1++){
          tone(52, 30); 
          delay(400);
          noTone(52);     
          delay(80);
          tone(52, 15); 
          delay(330);
          noTone(52);     
          delay(80);
        }
    }
*/
/*    //PITIDO DE AVISO
          tone(52, 30); 
          delay(270);
          noTone(52);     
          delay(100);
*/



  
}



















































//-----------------------------------------------------------------------------------------------------------------------
//FUNCION MEDIR TEMPERATURA
  float Medir_temperatura(void){ //FUNCION MEDIR TEMPERATURA

  int Espera=3000; // Tiempo de espera para verificar al usuario
  int Dist; // Distancia del Ultrasonico
  int DistMin=10; // Distancia minima para detectar al usuario (mm)
  unsigned long Tiempo=0; // Tiempo que lleva detectado (millis)
  int M4=0; //para el for
  
  int Error=0;//NO HAY ERROR
  float Suma_temp=0; //variable necesaria para calcular la media y reducir errores
  float Media_temp=0; //variable necesaria para reducir errores en la medicion
  float medida_temp=0; //medida final que manda la funcion

    SR04 sr04=SR04(Echo,Trig); //inicialización del sensor ultrasonido
    Dist=sr04.Distance(); //distancia a la que estamos
    
    while(Dist>=DistMin){
      Error=0;
      Dist=sr04.Distance(); //distancia a la que estamos
        //medimos 4 veces (para calcular la media)
          for (M4=0;M4<=3;M4++){
            medida_temp=mlx.readObjectTempC();//empleamos una variable, en vez de invocar a la funcion
            
              if (medida_temp>=33 && medida_temp<=44){ //barajamos un posible error
                Suma_temp=Suma_temp+medida_temp;//sumamos temperatura
                }else{ Error=1;} //ERROR
            medida_temp=0;
          }
               
          //ya tenemos las 4 medidas, calculamos la medida final (media)
          Media_temp=Suma_temp/4; Suma_temp=0;   
         }
         
         
          if (Media_temp==0){Error=1;}  //nos aseguramos de que tengamos una medida que ofrecer
          if (Error==1){Error_temperatura();} //Funcion error por LCD
          return Media_temp;   
       }
//-----------------------------------------------------------------------------------------------------------------------
  void Error_temperatura(void){ 
    //MOSTRAR ERROR POR LCD
    lcd.clear();
    lcd.print(" ERROR MEDICION ");
    delay(300);
    lcd.clear();
    delay(300);
    lcd.print(" ERROR MEDICION ");
    delay(300);
    lcd.clear();
    delay(300);
    lcd.print(" ERROR MEDICION ");
    delay(2000);
    lcd.clear();
    }





//AVISAR DE QUE YA HEMOS ACABADO LA MEDICION
      //PITIDO DE AVISO
          /*tone(52, 30); 
          delay(270);
          noTone(52);     
          delay(100);
*/
