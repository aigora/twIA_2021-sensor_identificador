#include "SerialClass/SerialClass.h" // Biblioteca para acceder al puerto serie
#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <time.h>


#define FIEBRE 38 //máximo aceptado por covid
#define LimiteUsuarios 15

typedef struct {
	char UID[15]; //El UID que corresponde a cada trabajador y el cual dará lugar a la representacion del trabajador al que le corresponde ese UID
	char nombre[50];
	int telefono;
	char sexo;
	int edad;
}USUARIO;

typedef struct {
	char hora[10];
	char fecha[10];
	char UID[15];
	float temperatura;
}registros;



//funciones prototipo
int leer_fichero_usuarios(USUARIO usuario[], int* pnumero_users, int longitud);
int buscar_usuario(char UID[], USUARIO usuario[], int numero);
void registrotemp(char UID[], float temperatura);
//prototipos para la funcion leer registros
void ImprimirRegistrosUsuario(registros* registro);
int cantidadderegistros(FILE*);


int main() {

	//variables y demas que son datos del usuario extraidos de Arduino
	char UID[200];
	bool Protocolo_Temperatura = false; //una vez se ha leido al usuario, iniciamos el proceso de leer la temperatura o no

	//variables y demas de la parte temperatura
	float tempRecibida;
	bool Tiempo_excedido=false; //booleana para esperar a que arduino responda

	//variables y demas de la parte  visual-arduino
	Serial* Arduino;
	char puerto[] = "COM3"; // Puerto serie en el que está conectado Arduino
	char BufferSalida[200];
	char BufferEntrada[200];
	int bytesRecibidos;
	Arduino = new Serial((char*)puerto); // Crea conexión lógica con Arduino

	//variables y estructuras relacionadas con la "base de datos" que se encuentra en el fichero DATOS TRABAJADORES
	USUARIO usuario[LimiteUsuarios];
	int numero_usuarios=0, Prob_Fichero; //Numero de usuarios reales que hay (OJO QUE SE CAMBIA CON UN PUNTERO) y problemas que hay con el fichero
	int user_i=-1;//usuario con el que estamos trabajando para no tener que leer el fichero entero otra vez (esto hace de (i) de un bucle)

	//comprobar fichero base de datos
	Prob_Fichero = leer_fichero_usuarios(usuario, &numero_usuarios, LimiteUsuarios); //si me devuelve -1 hay error, si no me devuelve 0
	if (Prob_Fichero == -1) {
		printf("El fichero Datos trabajadores está vacío o no ha podido leerse\n");
	}
	
	//variables y declaraciones que tienen relación con la función IMPRIMIR REGISTROS DE USUARIOS
	registros* registro;
	char TarjActivadora[] = "04 43 36 92"; //codigo de la tarjeta
	printf("Sistema Iniciado sin errores\n");

	//COMUNICACION ARDUINO-VISUAL

	while (Arduino->IsConnected()) //si hay conexion con arduino
	{
		Sleep(2000); // Tiempos de espera para la primera trama (evita pérdidas)

		//LEER UID
		
		bytesRecibidos = Arduino->ReadData(BufferEntrada, sizeof(char) * 199); //copiamos en buffer entrada lo que hay en arduino
		if (bytesRecibidos != -1) // Lectura correcta de mensaje DEL UID DEL USUARIO desde el puerto 
		{
			BufferEntrada[bytesRecibidos] = '\0'; // Le añadimos un \0 al final 
			

			strcpy_s(UID, 199, BufferEntrada); //LO GUARDAMOS EN UID PARA LA FUNCION REGISTRO 
			//y seleccionamos el numero de caracteres de buffer entrada que queremos meter en UID
			memset(BufferEntrada,0,11);
			UID[11] = '\0'; //nos aseguramos de que el uid sea solo de un usuario, si ha detectado mas de uno los borra
		

			printf("Acabo de leer: %s. \n", UID);

			if (strcmp(UID,TarjActivadora)==0) { //si es la tarjeta activadora se salta todo el procedimiento de comprobar si está en la base de datos
				
				FILE* pf_registro;
				errno_t err3;
				err3 = fopen_s(&pf_registro, "registro.txt", "r");
				if (pf_registro == NULL) {
					printf("El fichero Registro está vacío o no ha podido leerse");
				}
				registro = (registros*)malloc(cantidadderegistros(pf_registro) * sizeof(registros*));
				ImprimirRegistrosUsuario(registro);

			}
			else { //si no es la tarjeta activadora continuamos con el trascurso normal del programa

				user_i = buscar_usuario(UID, usuario, numero_usuarios); //meto el ID de NFC escaneado, el vector de estructuras de usuarios y hasta donde tiene q contar
				if (user_i == -1) {

					//NO SE HA ENCONTRADO EL USUARIO
					printf("Se ha introducido un UID que no existe \n");
					strcpy_s(BufferSalida, "USUARIO NO ENCONTRADO ");
					Protocolo_Temperatura = false; //no procedemos a leer temperatura
				}
				else {
					printf("Se ha encontrado al usuario en la base de datos \n");
					printf("se llama %s \n", usuario[user_i].nombre);
					strcpy_s(BufferSalida, "USUARIO ENCONTRADO ");
					Protocolo_Temperatura = true; //procedemos a leer temperatura

				}
				Arduino->WriteData(BufferSalida, strlen(BufferSalida)); // Envía mensaje USUARIO ENCONTRADO O USUARIO NO ENCONTRADO
				Sleep(2000); // Tiempos de espera para la primera trama (evita pérdidas)

			}
		} //Acaba funcion leer uid



		//PROTOCOLO DE LECTURA TEMPERATURA
		
		if (Protocolo_Temperatura == true) {

			//Proceso de esperar a que se mida la temperatura
			bytesRecibidos = -1;
			int repes=0;
			printf("Esperando temperatura...\n");
			Tiempo_excedido = false; //todavia no se ha excedido el tiempo
			// un usuario suele tardar entre 5 y 10 segundos de media (esperamos a 15 como límite)
			while (bytesRecibidos == -1 && repes != 30) {

				//BORRAR (APAÑO PARA NO USAR ARDUINO TODAVIA)                                                                   ((((BORRAR))))
				//bytesRecibidos = 6;
				//gets_s(BufferEntrada);


				bytesRecibidos=Arduino->ReadData(BufferEntrada, sizeof(char) * 199); //copiamos en buffer entrada lo que hay en arduino

				Sleep(500); //esperamos 0.5 segundos hasta volver a repetir la lectura
				repes += 1;
				
			}
			if (repes >= 30) {
				printf("Se ha excedido el tiempo maximo de lectura de temperatura\n");
				repes = 0;
				strcpy_s(BufferSalida, "TIEMPO EXCEDIDO ");
				Arduino->WriteData(BufferSalida, strlen(BufferSalida)); // Envía mensaje ERROR DE LECTURA a arduino por el serial
				Tiempo_excedido = true; 
			}

			if (Tiempo_excedido == false) {  //solo ejecutamos el procesado de la temperatura si no se ha excedido el tiempo
				
				BufferEntrada[bytesRecibidos] = '\0'; // Le añadimos un \0 al final
				tempRecibida = atof(BufferEntrada); //mandamos la temperatura de la cadena buffer a una variable float
				memset(BufferEntrada, 0, 199);

				//COMPROBAMOS QUE LA TEMPERATURA ESTÉ DENTRO DEL MARGEN COVID
				if (tempRecibida != 0.00) {
					if (tempRecibida >= FIEBRE) {
						printf("LA TEMPERATURA SUPERA LOS %d GRADOS\n", FIEBRE);
					}
					else {
						printf("Temperatura correcta, %.2fºC\n", tempRecibida);

						//FUNCION ACTIVAR PUERTA (se le manda a arduino)
						strcpy_s(BufferSalida, "ACTIVAR PUERTA ");
						Arduino->WriteData(BufferSalida, strlen(BufferSalida)); // Envía mensaje Activar la puerta a arduino por el serial
						printf("acabo de enviar por el serial: ACTIVAR PUERTA\n");//														  					((((BORRAR)))))
					}
					registrotemp(UID, tempRecibida); //INDEPENDIENTEMENTE DE SI TIENE COVID O NO GUARDAMOS LA TEMPERATURA EN UN REGISTRO
					printf("Se ha registrado la temperatura %.2f\n", tempRecibida);
				}
				else {
					printf("ERROR TEMPERATURA\n");
				}
			}

			Protocolo_Temperatura = false; //para evitar que se vuelva a ejecutar sin pasar por el registro
		}//acaba el protocolo temperatura

		//limpiamos datos para evitar problemas en el siguiente bucle
		memset(BufferSalida, 0, 199);
		bytesRecibidos = -1;
		tempRecibida = 0;
		user_i = -1;
		memset(UID, 0, 11); //borramos la cadena UID

	} //acaba WHILE ARDUINO IS CONECTED
	printf("\n\nCONEXIÓN PERDIDA ARDUINO-VISUAL\n\n");
} //acaba int main













	//FUNCIONES




	int leer_fichero_usuarios(USUARIO usuario[], int* pnumero_users, int longitud){

		FILE* pf_BaseDatos;
		errno_t err1;
		int i;


		//abrir ficheros y evitar errores
		err1 = fopen_s(&pf_BaseDatos, "Datos trabajadores.txt", "r");
		if (pf_BaseDatos == NULL)
		{
			printf("Error en la apertura del fichero Datos trabajadores.\n");
			*pnumero_users = 0;
			return -1;
		}


		//Sacamos datos de los usuarios del fichero y los metemos en nuestra estructura usuario
		i = 0;
		while (!feof(pf_BaseDatos))
		{

			fgets(usuario[i].UID, 15, pf_BaseDatos); //guarda el UID
			usuario[i].UID[strlen(usuario[i].UID) - 1] = '\0';

			fgets(usuario[i].nombre, 50, pf_BaseDatos); //guarda el nombre
			usuario[i].nombre[strlen(usuario[i].nombre) - 1] = '\0';

			fscanf_s(pf_BaseDatos, "%d", &usuario[i].telefono); //guarda el telefono
			fgetc(pf_BaseDatos);//salta de linea
			usuario[i].sexo = fgetc(pf_BaseDatos); //guarda el sexo
			fscanf_s(pf_BaseDatos, "%d", &usuario[i].edad);//guarda la edad
			fgetc(pf_BaseDatos); //salta de linea
			fgetc(pf_BaseDatos); //salta de linea

			*pnumero_users = i;
			i++;
		}
		fclose(pf_BaseDatos);
		
		return 0;
	}







	int buscar_usuario(char UID[], USUARIO usuario[], int numero) {
		int resultado = -1;

		if (strlen (UID) < 11) {
			return -1;
		}

		for (int j = 0; j < numero; j++)
		{
			if (strcmp(UID, usuario[j].UID) == 0)
			{
				resultado = j;
				
			}
			
		}
		return resultado;
	}



	//													FUNCIONES

	

	void registrotemp(char UID[], float temperatura) {

		/*  Esta función se encarga de registrar todas las temperaturas de sus respectivos usuarios almacenandolos en ficheros
			de 2 formas distintas, la primera es en un fichero tipo herramienta (los datos están separados por espacios) y la
			otra forma es un poco más cuidada y más presentable.
			Con esto queremos tratar de imitar una especie de base de datos (forma1) que tiene una utilidad para otra función 
			que usa nuestro programa más adelante y la otra forma imita una especie de historial de registros. Que se puede 
			leer por una persona, pero no está destinada a tener ningún tipo de utilidad para la máquina.  */

		FILE* pf_registro, * pf_registroS;  //fichero herramienta y fichero historial
		errno_t err1, err2;
		char hora[10];
		char fecha[10];
		
		_strtime_s(hora, 10); //Toma la hora del sistema
		_strdate_s(fecha, 10); //Toma la fecha del sistema

		err1 = fopen_s(&pf_registro, "registro.txt", "a+");
		if (pf_registro == NULL) {
			printf("Error en la apertura del fichero registro.");
			exit(-1);
		}
		err2 = fopen_s(&pf_registroS, "registroS.txt", "a+");
		if (pf_registroS == NULL) {
			printf("Error en la apertura del fichero registroS.");
			exit(-1);
		}

		if (temperatura < FIEBRE) //Temperatura < 38º C
		{
			fprintf(pf_registroS, "Registro de UID: %s temperatura: %.2fºC a las %s del %s. No se han detectado sintomas de COVID-19\n", UID, temperatura, hora, fecha ); //Y permite la entrada del trabajador
		}
		else //Temperatura > 38º C
		{
			fprintf(pf_registroS, "Registro de UID: %s temperatura: %.2fºC a las %s del %s. Posibles Síntomas de COVID-19\n", UID, temperatura , hora, fecha); //Y no permite la entrada del trabajador
		}

		fprintf(pf_registro, "%s %.2f %s %s\n", UID, temperatura, hora, fecha );

		fclose(pf_registro);
		fclose(pf_registroS);
	}
	












	



	void ImprimirRegistrosUsuario(registros* registro) {

		/*	Esta función extrae los datos del fichero (herramienta) y los imprime por pantalla en función 
			del UID que escriba el sujeto que esté utilizando el terminal, éste UID se emplea para buscar
			en el fichero <registro> TODOS los registros que ha tenido el usuario correspondiente a ese UID. */
		FILE* pf_registro;
		errno_t err3;
		int i;
		char UID[15];
		

		printf("Introduzca el UID del trabajador del que quiere sus registros: ");
		gets_s(UID);
		printf("\n\nRegistros del UID: %s\n\n", UID);


		err3 = fopen_s(&pf_registro, "registro.txt", "r");
		if (pf_registro == NULL) {
			printf("El fichero Registro está vacío o no ha podido leerse");
		}


		i = 0;
		while (!feof(pf_registro)) {
			fgets(registro[i].UID, 12, pf_registro);

			fgetc(pf_registro);

			fscanf_s(pf_registro, "%f", &registro[i].temperatura);

			fgetc(pf_registro);

			fgets(registro[i].hora, 9, pf_registro);
			fgetc(pf_registro);
			fgets(registro[i].fecha, 9, pf_registro);

			fgetc(pf_registro);


			if (strcmp(UID, registro[i].UID) == 0) {
				printf("Dia %s a las %s  ->  Temperatura: %.2f ºC\n", registro[i].fecha, registro[i].hora, registro[i].temperatura);
			}

			i++;
			}


			fclose(pf_registro);
		}

	int cantidadderegistros(FILE* pf_registro) {

		//Esta función es una herramiente de la función ImprimirRegistrosUsuario

		char datos[35];
		errno_t err1;
		err1 = fopen_s(&pf_registro, "registro.txt", "r");
		int cantidad;

		cantidad = 0;
		while (!feof(pf_registro)) {

			fgets(datos, 35, pf_registro);
			fgetc(pf_registro);

			memset(datos, 0, 35);
			cantidad += 35;
		}


		return cantidad;
	}