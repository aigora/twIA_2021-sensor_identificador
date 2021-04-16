/*
          CODIGO MODIFICADO POR DANIEL


*/

//Aquí empieza el código para el NFC
#include <SPI.h>
#include <MFRC522.h>
#define RST_PIN  9    //Pin 9 para el reset del RC522
#define SS_PIN  10   //Pin 10 para el SS (SDA) del RC522
MFRC522 mfrc522(SS_PIN, RST_PIN); //Creamos el objeto para el RC522

void setup() {
  Serial.begin(9600); //Se inicia la comunicación en serie
  SPI.begin();        //Se inicia el Bus SPI
  mfrc522.PCD_Init(); //Se inicia el MFRC522
  Serial.println("Lectura del UID");
}

void loop() {
  //Revisión por alguna posible nueva tarjeta
  if (mfrc522.PICC_IsNewCardPresent()) 
        {  
      //Se selecciona una tarjeta
            if (mfrc522.PICC_ReadCardSerial()) 
            {
                  //Se envía su UID en forma ede serie
                  Serial.print("Card UID:");
                  for (byte i = 0; i < mfrc522.uid.size; i++) {
                          Serial.print(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " ");
                          Serial.print(mfrc522.uid.uidByte[i], HEX);   
                  } 
                  Serial.println();
                  //Se termina la lectura de la tarjeta en uso actual
                  mfrc522.PICC_HaltA();         
            }      
  } 
}
