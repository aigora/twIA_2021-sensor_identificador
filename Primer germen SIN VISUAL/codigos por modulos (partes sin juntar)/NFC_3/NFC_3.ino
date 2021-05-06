
#include <SPI.h>      // incluye libreria bus SPI
#include <MFRC522.h>      // incluye libreria especifica para MFRC522

#define RST_PIN  49      // constante para referenciar pin de reset
#define SS_PIN  53      // constante para referenciar pin de slave select
MFRC522 mfrc522(SS_PIN, RST_PIN); // crea objeto mfrc522 enviando pines de slave select y reset

byte Lectura_vector[4];         // crea un vector para almacenar el UID leido
byte Usuario1[4]= {0xEA, 0x15, 0x4E, 0x19} ;    // ID de tarjeta del usuario 1
byte Usuario2[4]= {0x06, 0x76, 0x25, 0xD9} ;    // ID de tarjeta del usuario 2

void setup() {
  Serial.begin(9600);     // inicializa la comunicacion a traves del monitor serie a 9600 bps
  SPI.begin();        // inicializa bus SPI
  mfrc522.PCD_Init();     // inicio modulo lector
  Serial.println("Listo");    
}

void loop() {
  
  if ( ! mfrc522.PICC_IsNewCardPresent()) {return;}//sale del bucle si no hay tarjeta           
  if ( ! mfrc522.PICC_ReadCardSerial()) {return;}//sale del bucle si no puede leer correctamente
  
    Serial.print("UID:");       
    
    for (byte i = 0; i < mfrc522.uid.size; i++) { // bucle recorre de a un byte por vez el UID
      if (mfrc522.uid.uidByte[i] < 0x10){   // si el byte leido es menor a 0x10
        Serial.print(" 0"); }      // imprime espacio en blanco y numero cero
        
        else{ Serial.print(" "); }       // imprime un espacio en blanco
          
          Serial.print(mfrc522.uid.uidByte[i], HEX);    // imprime el byte del UID leido en hexadecimal
          Lectura_vector[i]=mfrc522.uid.uidByte[i];     // almacena en array el byte del UID leido      
          }
          
          Serial.print("\t");         // imprime un espacio de tabulacion             




                    
          if(comparacion_vector(Lectura_vector, Usuario1))    // llama a funcion comparaUID con Usuario1
            Serial.println("Bienvenido Usuario 1"); // si retorna verdadero muestra texto bienvenida
          else if(comparacion_vector(Lectura_vector, Usuario2)) // llama a funcion comparaUID con Usuario2
            Serial.println("Bienvenido Usuario 2"); // si retorna verdadero muestra texto bienvenida
           else           // si retorna falso
            Serial.println("No te conozco");    // muestra texto equivalente a acceso denegado          
                  
                  mfrc522.PICC_HaltA();     // detiene comunicacion con tarjeta                
}

boolean comparacion_vector(byte lectura[],byte usuario[]) // funcion comparacion por bytes
{
  for (byte i=0; i < mfrc522.uid.size; i++){    // bucle recorre de a un byte por vez el UID
  if(lectura[i] != usuario[i])        // si byte de UID leido es distinto a usuario
    return(false);          // devuelve falso
  }
  return(true);           // si los 4 bytes coinciden devuelve verdadero
}
