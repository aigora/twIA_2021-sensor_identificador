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

  //VARIABLES
    
    int estado=0;
    float temperatura_visual=0.0;
    bool usuario_existente=false;
    int repes=0;
    //comunicacion con visual:
    String Usuario_Enc="USUARIO ENCONTRADO ";
    String Usuario_N_Enc="USUARIO NO ENCONTRADO ";
    String Activ_puerta="ACTIVAR PUERTA ";
    String Tiempo_Exced="TIEMPO EXCEDIDO ";
    String Mensaje_recibido;
    String Mensaje_enviado;
    char cadena[11];
    byte Lectura_vector[4];         // crea un vector para guardar el ID leido
    
   // Ajusta la dirección de la LCD a 0x27 para una pantalla de 16 caracteres y 2 lineas
    LiquidCrystal_I2C lcd(0x27, 16, 2);


//-----------------------------------------------------------------------------------------------------------------------
//                                                     | VOID SETUP |
//-----------------------------------------------------------------------------------------------------------------------
  void setup() {
    estado=0;
    Serial.begin(9600);     // inicializa la comunicacion a traves del monitor serie a 9600 bps
    SPI.begin();        // inicializa bus SPI
    mfrc522.PCD_Init();     // inicio modulo lector
    //Serial.println("Listo"); //REVISAR (lo retiraremos al final del proyecto)
    pinMode(40, OUTPUT); // Pin ZUMBADOR
    pinMode(35, OUTPUT); // Pin activador cerradura
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
      //Serial.println("Arduino iniciado");

  }//FIN SETUP

//-----------------------------------------------------------------------------------------------------------------------
//                                                     | VOID LOOP |
//-----------------------------------------------------------------------------------------------------------------------

  void loop() {
    digitalWrite(35,LOW); //la cerradura está desactivada
    
   switch (estado){

    
     case 0://codigo lectura tarjeta

       lcd.clear();
       lcd.setCursor(0, 0);
       lcd.print("ACERQUE TARJETA");
    
      //                                                     SECCION NFC

      if (mfrc522.PICC_IsNewCardPresent()){ //sale del bucle si no hay tarjeta           
        if (mfrc522.PICC_ReadCardSerial()){ //sale del bucle si no puede leer correctamente      
          for (byte i = 0; i < mfrc522.uid.size; i++) { // bucle recorre de a un byte por vez el UID 
             
             if (mfrc522.uid.uidByte[i] < 0x10){   // si el byte leido es menor a 0x10
               //Serial.print("0");       // imprime espacio en blanco y numero cero
             }
             else{ 
               //Serial.print(" ");        // imprime un espacio en blanco
             }
             
             //Guardando el ID en un vector
              Lectura_vector[i]=mfrc522.uid.uidByte[i];     // almacena en VECTOR DE BYTES el byte del ID leido  
              sprintf(cadena,"%02X",mfrc522.uid.uidByte[i]);   //guarda en una cadena 2 elementos hexadecimales del vector 
              Mensaje_enviado=Mensaje_enviado+cadena+" "; // añade a un string los dos elementos hexadecimales y un espacio (el formato establecido)
              
           }//fin for
             Mensaje_enviado=Mensaje_enviado.substring(0,11); //Borramos el último espacio para no tener problemas luego
             mfrc522.PICC_HaltA();     // detiene comunicacion con tarjeta 
             
           //Enviamos DATOS Y ESPERAMOS RESPUESTA DE VISUAL      
             Serial.print(Mensaje_enviado); // Envío por el puerto serie (VISUAL) el codigo de la tarjeta NFC
             Mensaje_enviado=""; //borramos el mensaje enviado tras haberlo enviado
             estado=1; //Una vez enviado el codigo NFC pasamos a esperar respuesta de visual
             
          }//fin if
       }//FIN PRIMER IF  y fin de la sección lectura NFC
       delay(1000);//corrección desfase comunicación visual
       break;//Fin case 0

     case 1: //                                         COMUNICACION VISUAL
        repes=0;
      //Esperamos la respuesta del procesamiento de los datos de visual
        while(Serial.available()==0 && repes<=19){ //4 SEGUNDOS COMO MÁXIMO HASTA QUE VISUAL MANDE LA RESPUESTA
          if (Serial.available()>0){ break;}
          delay(200);
          repes+=1;
          }
          repes=0;
        /* Este último while está hecho de esta manera para que medianamente estén coordinados visual y arduino, y
         * que esto sea compatible con la funcion imprimir por pantalla los registros del visual, de esta manera se
         * salta el imprimir por pantalla LCD que la tarjeta activadora no se encuentra en la base de datos. */
         
       //Una vez hay comunicación:
         if( Serial.available()> 0) {
            String Mensaje_recibido = Serial.readStringUntil('\n'); // Lectura de la cadena que almacena mensajes recibidos
            
            //COMPARADOR Mensaje recibido con "comandos establecidos"
            if (Mensaje_recibido.equals(Usuario_Enc)) {  //SE HA ENCONTRADO EL USUARIO EN LA BASE DE DATOS
                lcd.clear();
                lcd.print(" BIENVENIDO");delay(2000);
                usuario_existente=true;
                estado=2; break;
                }
            if (Mensaje_recibido==Usuario_N_Enc) {  //NO SE HA ENCONTRADO EL USUARIO EN LA BASE DE DATOS 
                lcd.clear();
                lcd.setCursor(0, 0);
                lcd.print("    USUARIO");
                lcd.setCursor(0, 1);
                lcd.print(" NO ENCONTRADO");
                Alarma_zumbador(); //NO AÑADO DELAY PORQUE ESTA FUNCION INCLUYE UNA
                lcd.clear();
                usuario_existente=false;
                estado=0; break; //reinicio el switch case para cambiar de estado
                }
            if (Mensaje_recibido.equals(Tiempo_Exced)){
                lcd.clear();
                lcd.setCursor(0, 0);
                lcd.print("    TIEMPO");
                lcd.setCursor(0, 1);
                lcd.print("   EXCEDIDO");
                Alarma_zumbador(); //NO AÑADO DELAY PORQUE ESTA FUNCION INCLUYE UNA
                lcd.clear();
                estado=0; break; // REINICIAR LOOP y vuelvo a leer tarjetas
                }
            if (Mensaje_recibido.equals(Activ_puerta)){
              
                //FUNCION ACTIVAR PUERTA
                lcd.clear();
                lcd.setCursor(0, 0);
                lcd.print("  abriendo");
                lcd.setCursor(0, 1);
                lcd.print("    Puerta");
                digitalWrite(35,HIGH);
                Alarma_zumbador(); //NO AÑADO DELAY PORQUE ESTA FUNCION INCLUYE UNA
                lcd.clear();
                digitalWrite(35,LOW);
                estado=0; break; // REINICIAR LOOP
                }
         
            }// FIN IF SERIAL.AVAILABLE

          if (estado==1){ //si no ha recibido nada en 4 segundos y tampoco respuesta de visual
                estado=0; break;
                error_Conexion(); 
                
             }



      case 2://                                         SECCION MEDICION TEMPERATURA
        lcd.clear();
        if (usuario_existente==true){
          
          usuario_existente=false;//ya hemos entrado en el bucle
          lcd.setCursor(0, 0);
          lcd.print(" Acerquese al");
          lcd.setCursor(0, 1);
          lcd.print("sensor de temperatura");
          delay(3000);
          temperatura_visual=Medir_temperatura(); //llamamos a la funcion MEDIR TEMPERATURA
          Serial.print(temperatura_visual);
          if (temperatura_visual!=0.00){ //si es 0.00 es que ha habido un problema al leer, aún así se lo mandamos a visual para que lo notifique
            lcd.clear();
            lcd.setCursor(0, 0);
            lcd.print("Su temperatura:");
            lcd.setCursor(0, 1);
            lcd.print(temperatura_visual); lcd.print("C");
            delay(3000);
          }else{
            Error_temperatura();
            Alarma_zumbador();
            }
          estado=1;
        }
           break; //                            FIN SECCION TEMPERATURA
      
    }//fin switch ESTADO          
  }//llave fin loop

//-----------------------------------------------------------------------------------------------------------------------
//                                                     | FIN LOOP |
//-----------------------------------------------------------------------------------------------------------------------





//                                                      FUNCIONES


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
    
      while(Dist<=DistMin){
        Error=0;
        Dist=sr04.Distance(); //distancia a la que estamos
         //medimos 4 veces (para calcular la media)
          for (M4=0;M4<=3;M4++){
            medida_temp=mlx.readObjectTempC();//empleamos una variable, en vez de invocar a la funcion
            
              if (medida_temp>=33 && medida_temp<=44){ //barajamos un posible error
                Suma_temp=Suma_temp+medida_temp;//sumamos temperatura
                }else{ Error=1;} //ERROR
            medida_temp=0;
          }//fin for
  
          //ya tenemos las 4 medidas, calculamos la medida final (media)
          Media_temp=Suma_temp/4; Suma_temp=0;   
        }//fin while
         
       if (Media_temp==0) {Error=1;}  //nos aseguramos de que tengamos una medida que ofrecer
       if (Error==1) {Error_temperatura();} //Funcion error por LCD
       return Media_temp;   
     }//fin funcion temperatura


//FUNCION ERROR (MEDICION TEMPERATURA)    <<<LCD>>>
  void Error_temperatura(void){ 
    //MOSTRAR ERROR POR LCD

    for(int i=0; i<1;i++){ //combina mensajes LCD y soniditos del zumbador
      lcd.clear();
      lcd.print(" ERROR MEDICION ");
      tone(40, 30); 
      delay(400);
      delay(300);
      lcd.clear();
      noTone(40);     
      delay(80);
      noTone(40);     
      delay(80);
    }
      lcd.clear();
      delay(300);
      lcd.print(" ERROR MEDICION ");
      delay(2000);
      lcd.clear();
    }
    
//FUNCION ALARMA
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

//FUNCION ERROR CONEXION VISUAL
   void error_Conexion(void){
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("    ERROR");
      lcd.setCursor(0, 1);
      lcd.print("CONEXION VISUAL");
      Alarma_zumbador(); //NO AÑADO DELAY PORQUE ESTA FUNCION INCLUYE UNA
      lcd.clear();
    
   }
 



//                                                      FIN FUNCIONES
//-----------------------------------------------------------------------------------------------------------------------




//                                                        NOTAS
/*


                                - Añadir sonidito despues de ingresar tarjeta
                                - Menu visual, terminal visual y PENSAR COMO CREAR SISTEMAS PARALELOS (MENU-SENSORES)
                                - Cerradura
                                - Pasar hardware a presentacion más presentable
                                - Electronica parte cerradura (relé y demas)
                                - Descripcion/desarroyo del proyecto (pato y natalia)
                                - DEFINIR PINES (ENTRADAS) Y CREAR UN FRIZING (MAPA ELECTRONICO) 


*/
