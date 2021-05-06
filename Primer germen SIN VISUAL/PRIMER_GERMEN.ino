//LIBRERIAS
#include <SPI.h>      // incluye libreria bus SPI
#include <MFRC522.h>      // incluye libreria especifica para MFRC522
#include <Wire.h> //LIBRERIA COMUNICACION 
#include <LiquidCrystal_I2C.h> //LIBRERIA LCD
#include <Adafruit_MLX90614.h> //LIBRERIA TERMOMETRO INFRARROJO
#include <SR04.h> // Libreria ultrasonico 


//CONSTANTES
// Definicion de pines NFC
#define RST_PIN  49      // constante para referenciar pin de reset
#define SS_PIN  53      // constante para referenciar pin de slave select
// Definicion de pines ultrasonido
#define Echo 30 
#define Trig 32


//Objetos necesarios para los sensores:
MFRC522 mfrc522(SS_PIN, RST_PIN); // crea objeto mfrc522 enviando pines de slave select y reset
Adafruit_MLX90614 mlx = Adafruit_MLX90614(); //objeto para el LCD

//usuarios y "base de datos de usuarios" (esto se trasladará a visual por ficheros en una versión posterior)
byte Lectura_vector[4];         // crea un vector para guardar el ID leido
byte Usuario1[4]= {0xEA, 0x15, 0x4E, 0x19} ;    // ID de tarjeta del usuario 1
byte Usuario2[4]= {0x06, 0x76, 0x25, 0xD9} ;    // ID de tarjeta del usuario 2

// Ajusta la dirección de la LCD a 0x27 para una pantalla de 16 caracteres y 2 lineas
LiquidCrystal_I2C lcd(0x27, 16, 2);



//-----------------------------------------------------------------------------------------------------------------------
//                                                     | VOID SETUP |
//-----------------------------------------------------------------------------------------------------------------------
void setup() {
  
  Serial.begin(9600);     // inicializa la comunicacion a traves del monitor serie a 9600 bps
  SPI.begin();        // inicializa bus SPI
  mfrc522.PCD_Init();     // inicio modulo lector
  Serial.println("Listo"); //REVISAR (lo retiraremos al final del proyecto)
  pinMode(40, OUTPUT); // Pin ZUMBADOR
  bool usuario_existente = false;



  // Inicializa la LCD y el MLX90614
  lcd.init();
  mlx.begin(); 
  lcd.backlight();// Enciende la retroiluminación.

  //FUNCION INICIO LCD
    //FUNCION INICIO
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
    Serial.println("Arduino iniciado");

    

      
}//FIN SETUP

//-----------------------------------------------------------------------------------------------------------------------
//                                                     | VOID LOOP |
//-----------------------------------------------------------------------------------------------------------------------

void loop() {
  float temperatura_visual=0.0;
    bool usuario_existente=false;
    lcd.clear();
    lcd.setCursor(0, 0);
      lcd.print("ACERQUE TARJETA");
      
  
//-----------------------------------------------------------------------------------------------------------------------
//                                                     SECCION NFC

  if ( ! mfrc522.PICC_IsNewCardPresent()) {return;}//sale del bucle si no hay tarjeta           
  if ( ! mfrc522.PICC_ReadCardSerial()) {return;}//sale del bucle si no puede leer correctamente
  
    Serial.print("UID:"); //REVISAR.  SIRVE para imprimir por pantalla (RETIRAR EN EL CODIGO FINAL)      
    
    for (byte i = 0; i < mfrc522.uid.size; i++) { // bucle recorre de a un byte por vez el UID

      //imprime por pantalla
      if (mfrc522.uid.uidByte[i] < 0x10){   // si el byte leido es menor a 0x10
        Serial.print(" 0"); }      // imprime espacio en blanco y numero cero
        else{ Serial.print(" "); }       // imprime un espacio en blanco
          Serial.print(mfrc522.uid.uidByte[i], HEX);    // imprime el byte del UID leido en hexadecimal

          
          //Guardando el ID en un vector
          Lectura_vector[i]=mfrc522.uid.uidByte[i];     // almacena en array el byte del ID leido      
          }
          
          Serial.print("\t");         // imprime un espacio de tabulacion             




              //REVISAR comparacion con "base de datos de arduino" (RETIRAR EN LA VERSION VISUAL)     
         if(comparacion_vector(Lectura_vector, Usuario1)){    // llama a funcion comparaUID con Usuario1 // si retorna verdadero muestra texto bienvenida
            usuario_existente=true;
            Serial.println("Bienvenido Usuario 1");
            lcd.clear();
            lcd.print(" BIENVENIDO");delay(2000);
         }
            else if(comparacion_vector(Lectura_vector, Usuario2)){ // llama a funcion comparaUID con Usuario2 // si retorna verdadero muestra texto bienvenida
              usuario_existente=true;
              Serial.println("Bienvenido Usuario 2");
              lcd.clear();
              lcd.print(" BIENVENIDO");delay(2000);
              }
              else{           // si retorna falso
                 Serial.println("No te conozco");
                 // muestra texto equivalente a acceso denegado 
                 }         

                  //DETENIENDO COMUNICACION CON TARJETA NFC
                  mfrc522.PICC_HaltA();     // detiene comunicacion con tarjeta  

//                                                   FIN SECCION NFC
//-----------------------------------------------------------------------------------------------------------------------






//                                                 SECCION //VISUAL// DATOS ARDUINO-VISUAL (COMPROBAR NFC REGISTRADO) luego visual comprueba en ficheros
//                                                 SECCION //VISUAL// DATOS VISUAL-ARDUINO (ACCESO O NO ACCESO)

                 
//-----------------------------------------------------------------------------------------------------------------------
//                                              SECCION MEDICION TEMPERATURA
   lcd.clear();
   if (usuario_existente==true){
      usuario_existente=false;//ya hemos entrado en el bucle
        lcd.setCursor(0, 0);
        lcd.print(" Acerquese al");
        lcd.setCursor(0, 1);
        lcd.print("sensor de temperatura");
        delay(3000);
        temperatura_visual=Medir_temperatura();
        Serial.print(temperatura_visual);
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("Su temperatura:");
        lcd.setCursor(0, 1);
        lcd.print(temperatura_visual); lcd.print("C");
        delay(3000);
    
   }else{ //tarjeta incorrecta
    lcd.setCursor(0, 0);
    lcd.print("ERROR");
    lcd.setCursor(0, 1);
    lcd.print("TARJETA INCORRECTA");
    Alarma_zumbador();
    lcd.clear();
    }

//                                                   FIN SECCION TEMPERATURA
//-----------------------------------------------------------------------------------------------------------------------





//                                                 SECCION //VISUAL// DATOS ARDUINO-VISUAL (TEMPERATURA) luego visual lo guarda
//                                                 SECCION //VISUAL// valorar (ACCESO O NO ACCESO) por temperatura


//                                                 SECCION CERRADURA

               /*
              lcd.clear();
              lcd.setCursor(0, 0);
              lcd.print("Fin bucle");*/
              
}  //llave fin loop

//-----------------------------------------------------------------------------------------------------------------------
//                                                     | FIN LOOP |
//-----------------------------------------------------------------------------------------------------------------------


//                                                      FUNCIONES



//FUNCION COMPARACION VECTORES con usuarios registrados
  boolean comparacion_vector(byte lectura[],byte usuario[]){ // funcion comparacion por bytes

    for (byte i=0; i < mfrc522.uid.size; i++){    // bucle recorre de a un byte por vez el UID
      if(lectura[i] != usuario[i])        // si byte de UID leido es distinto a usuario
        return(false);          // devuelve falso
        }
      return(true);           // si los 4 bytes coinciden devuelve verdadero
    }


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


//FUNCION ERROR (MEDICION TEMPERATURA)    <<<LCD>>>
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

  void Alarma_zumbador(void){
      int o1=0;
      for (int o1=1;o1<=3;o1++){
          tone(40, 30); 
          delay(400);
          noTone(40);     
          delay(80);
          tone(40, 15); 
          delay(330);
          noTone(40);     
          delay(80);
        }
  }



//                                                      FIN FUNCIONES
//-----------------------------------------------------------------------------------------------------------------------
