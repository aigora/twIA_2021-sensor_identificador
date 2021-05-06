
-------------------------------------------------------------

#include <SR04.h> // Libreria ultrasonico 

// Definicion de pines
#define Echo 11 //Echo del Ultrasonico
#define Trig 10 // Trig del Ultrasonico


float Medir_temperatura(void){

  int Espera=3000; // Tiempo de espera para verificar al usuario
  int Dist; // Distancia del Ultrasonico
  int DistMin=10; // Distancia minima para detectar al usuario (mm)
  unsigned long Tiempo=0; // Tiempo que lleva detectado (millis)

  int Error=0;//NO HAY ERROR
  float Suma_temp=0;
  float Media_temp=0;
  int Numero_medidas=0;  //REVISAR
  float medida_temp=0;

  SR04 sr04=SR04(Echo,Trig); //inicialización del sensor ultrasonido
  Tiempo=millis();//guardamos el origen del contador
  
   while(millis()-Tiempo <= Espera){   //"mientras estemos en el tiempo de espera"
     Dist=sr04.Distance(); //distancia a la que estamos
      if(Dist<=DistMin){

        //medimos 4 veces (para calcular la media)
          for (int 4M=0,4M<=3,4M++){
            if (medida_temp>33 && medida_temp<44){ //barajamos un posible error
              Suma_temp=Suma_temp+medida_temp;//sumamos temperatura
            }else{ Error=1;//ERROR
            }
            medida_temp=0;
          }
          Media_temp=Suma_temp/4;
          Suma_temp=0;
          
    }else{ 
      //ERROR DE LECTURA
      Error=1;//ERROR
      }

      //AVISAR DE QUE YA HEMOS ACABADO LA MEDICION
  }
  
  if (error!=1){
  return Media_temp;
  }
  Media_temp=0;
    
}
---------------------------------------------------------
