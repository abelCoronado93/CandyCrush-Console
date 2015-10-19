//Práctica 3 FP. Color Crush. 1ºA - Abel Coronado
#include <iostream>
#include <ctime>
#include <Windows.h> 
#include <iomanip>
#include <fstream>
using namespace std;
#include <string>

const bool DEBUG = false; //Modo debug

const int NUMCOLOR = 4; //Numero de colores que usará el programa
const int DIM = 8; //Dimension del tablero
const int INTENTOS = 15; //Numero de intentos
const int SLEEP = 400; 

//Types
typedef enum {magenta, amarillo, cyan, verde, negro} tFicha; //Colores de ficha
typedef tFicha tTablero[DIM][DIM]; //Array bidimensional tFicha de dimension DIMxDIM
typedef struct { //Struct con el tablero, los intentos restantes y los puntos llevados hasta el momento
	tTablero tablero;
	int intentosRestantes;
	int puntos;
} tJuego;
typedef enum {arriba, abajo, derecha, izquierda, error} tDireccion; //Direccion del movimiento (A, B, D, I)
typedef struct { //Struct con las coordenadas de la ficha a desplazar y el sentido
	int fila;
	int columna;
	tDireccion direccion;
} tMovimiento;
typedef enum { //Enum que usa Windows para dar color a las fichas
black, dark_blue, dark_green, dark_cyan, dark_red,
dark_magenta, dark_yellow, light_gray, dark_gray,
light_blue, light_green, light_cyan, light_red,
light_magenta, light_yellow, white
} tColor;
typedef bool tAciertos[DIM][DIM]; //Array bidimensional que nos dice si existen grupos de fichas

//Prototipos
void generarTablero(tTablero tablero); /*Rellena el array bidimensional con enumerados tFicha
									   Entrada: array tablero vacío / Salida: array tablero relleno */
void mostrarParteArribaTablero(tJuego juego); /*Muestra la primera linea de tablero
											  Entrada: struct juego / Salida: cout */
void mostrarParteCentralTablero(tJuego juego); /*Muestra el resto del tablero excepto la ultima linea
											   Entrada: struct juego / Salida: cout */
void mostrarParteAbajoTablero(tJuego juego); /*Muestra la ultima linea del tablero
											 Entrada: struct juego / Salida: cout */
void mostrarTablero(tJuego juego); /*Usa las 3 funciones anteriores para dibujar el tablero completo
								   Entrada: struct juego / Salida: cout */
void setColor(tColor color); /*Funcion que usa Windows para colorear nuestras fichas
							 Entrada: enum tColor / Salida: ficha coloreada */
tColor cambioEnum(tFicha ficha); /*Pasa nuestro enum tFicha a tColor (enum predefinido que usa la funcion setColor)
								 Entrada: enum tFicha / Salida: enum tColor */
void procesarFilas(tJuego &juego, tAciertos aciertos, bool &haCambiado); /*Mira los cjts. de fichas en las filas
																		 Entrada: struct juego, array bool aciertos, bool haCambiado / Salida: juego y bool modificados*/
void procesarCols(tJuego &juego, tAciertos aciertos, bool &haCambiado); /*Mira los cjts. de fichas en las columnas
																		Entrada: struct juego, array bool aciertos, bool haCambiado / Salida: juego y bool modificados*/
void procesarTablero(tJuego &juego, tAciertos aciertos, bool &haCambiado); /*Mira los aciertos en las filas y cols, rellenando el array de bools 
																		   Entrada: struct juego, array bool aciertos, bool haCambiado / Salida: juego y bool modificados*/
void colorearNegro(tJuego &juego, const tAciertos aciertos); /*'Destruye' los grupos de fichas repetidos
															 Entrada: juego, array bool / Salida: juego modificado*/
void contarPuntos(tJuego &juego, int cont); /*Cuenta los puntos que ha conseguido el usser en esa jugada
											Entrada: juego, contador / Salida: juego modificado*/
void caidaFichas(tJuego &juego, bool &hanCaido); /*Hace que caigan las fichas una vez y genera nuevas
												 Entrada: struct juego, bool hanCaido / Salida: juego modificado, bool modificado*/
void gestionCaida(tJuego &juego); /*Repite la funcion anterior las veces que sea necesario
								  Entrada: struct juego / Salida: juego modificado*/
void procesarSuperTablero(tJuego &juego, tAciertos aciertos, bool &haCambiado); /*Procesa toda la caida
																				Entrada: struct juego, array bool aciertos, bool haCambiado / Salida: juego y bool modificados*/
void jugar(tJuego juego); /*Lleva el desarrollo del juego
						  Entrada: juego / Salida: - */
void chartDireccion(char dir, tMovimiento &movimiento); /*Convierte el char direccion en enum tMovimiento (A, B, D, I, ERROR)
														Entrada: char de direccion, struct movimiento / Salida: struct movimiento modificado*/
bool esValido(string mov, tMovimiento movimiento); /*Comprueba que las filas y las cols son validas y que el char es correcto
												   Entrada: cin mov, struct movimiento / Salida: bool*/
void intercambiarFicha(tJuego &juego, tMovimiento movimiento); /*Genera el movimiento del jugador
															   Entrada: struct juego, movimiento del usser / Salida: juego modificado*/
int menu(); /*Devuelve la opción que elige el usser (1 o 2)
			Entrada: - / Salida: Opcion del usser*/
bool cargaTablero(tJuego &juego, string nombreArchivo); /*Carga un tablero definido en archivo
														Entrada: struct juego, string nombre del archivo / Salida: juego modificado*/
int main(){
	srand(time(NULL));
	int op;
	tJuego juego;
	string nombreArchivo;

	juego.intentosRestantes = INTENTOS; //Inicializacion de intentos
	juego.puntos = 0; //Inicializacion de los puntos

	op = menu();
	switch (op){
		case 1: { generarTablero(juego.tablero); } break;
		case 2: {
			cout << endl;  cout << "Introduce el nombre del archivo: ";
			cin >> nombreArchivo;
			while (!cargaTablero(juego, nombreArchivo)){
				cout << endl; cout << "Error! Archivo no encontrado" << endl; cout << endl;
				cout << "Introduce el nombre del archivo: ";
				cin >> nombreArchivo;
			}
		} break;
	}
	jugar(juego);
return 0;
}
bool cargaTablero(tJuego &juego, string nombreArchivo){
	bool ok;
	ifstream archivo;
	int color;
	archivo.open(nombreArchivo.c_str());
	if (archivo.is_open()){
		ok = true;
		for (int i = 0; i < DIM; i++){
			for (int j = 0; j < DIM; j++){
				archivo >> color;
				juego.tablero[i][j] = tFicha(color);
			}
		}
	}
	else { ok = false; }
	archivo.close();
return ok;
}
int menu(){
	int op;
	cout << "---------------------------------------";
	cout << endl; cout << "1. Tablero aleatorio" << endl;
	cout << "2. Cargar tablero desde archivo ( .txt)" << endl;
	cout << "---------------------------------------";
	cout << endl; cout << "Introduce una opcion: ";
	cin >> op;
	while ((op != 1) && (op != 2)){
		cout << endl; cout << "Opcion no valida" << endl; cout << endl;
		cout << endl; cout << "Introduce una opcion: ";
		cin >> op; cout << endl;
	}
return op;
}
void generarTablero(tTablero tablero){
	for(int fila = 0; fila < DIM; fila++){
		for(int col = 0; col < DIM; col++){
			tablero[fila][col] = tFicha(rand() % NUMCOLOR);
		}
	}
}
void mostrarParteArribaTablero(tJuego juego){
	cout << "       Intentos: " << juego.intentosRestantes << "    Puntos: " << juego.puntos << endl;
	setColor(white);
	cout << "    ";
	cout << char(218);
	for(int i = 0; i < DIM - 1; i++){
		cout << char(196) << char(196) << char(196) << char(194);
	}
	cout << char(196) << char(196) << char(196) << char(191) << endl;
}
void mostrarParteCentralTablero(tJuego juego){
	//Vamos a colocar las filas inversamente para procesarlas más fácilmente en un futuro
	for(int i = DIM-1; i > 0; i--){ 
		cout << " ";
		cout << i + 1;
		cout << "  ";
		for(int k = 0; k < DIM; k++){
			cout << char(179);
			setColor(cambioEnum(juego.tablero[i][k]));
			cout << char(219) << char(219) << char(219);
			setColor(white);
		}
		cout << char(179) << endl;
		cout << "    "; 
		cout << char(195);
			for(int m = 0; m < DIM - 1; m++){
				cout << char(196) << char(196) << char(196) << char(197);
			}
			cout << char(196) << char(196) << char(196) << char(180) << endl;
	}
	cout << " 1  ";
	for(int i = 0; i < DIM; i++){
		cout << char(179);
		//Mostramos la primera línea en último lugar (de ahí el indice [0][i])
		setColor(cambioEnum(juego.tablero[0][i]));
		cout << char(219) << char(219) << char(219);
		setColor(white);
	}
	cout << char(179) << endl;
}
void mostrarParteAbajoTablero(tJuego juego){
	cout << "    ";
	cout << char(192);
	for(int i = 0; i < DIM - 1; i++){
		cout << char(196) << char(196) << char(196) << char(193);
	}
	cout << char(196) << char(196) << char(196) << char(217) << endl;
	cout << "      ";
	for(int i = 1; i < DIM+1; i++){
		cout << i << "   ";
	}
	cout << endl;
}
void mostrarTablero(tJuego juego){
	if (DEBUG == false){
		system ("cls");
		mostrarParteArribaTablero(juego);
		mostrarParteCentralTablero(juego);
		mostrarParteAbajoTablero(juego);
		Sleep(SLEEP);
		}
	else {
		mostrarParteArribaTablero(juego);
		mostrarParteCentralTablero(juego);
		mostrarParteAbajoTablero(juego);
		system("pause");
	}
}
void setColor(tColor color){
	HANDLE handle = GetStdHandle(STD_OUTPUT_HANDLE);
	SetConsoleTextAttribute(handle, color);
}
tColor cambioEnum(tFicha ficha){
	tColor color;
	switch(ficha){
		case magenta: {
			color = light_magenta;
		}
		break;
		case amarillo: {
			color = light_yellow;
		}
		break;
		case cyan: {
			color = light_cyan;
		}
		break;
		case verde: {
			color = light_green;
		}
		break;
		case negro: {
			color = black;
		}
		break;
	}
return color;
}
void procesarTablero(tJuego &juego, tAciertos aciertos, bool &haCambiado){ //Procesar una vuelta
	for(int i = 0; i < DIM; i++){
		for(int j = 0; j < DIM; j++){
			aciertos[i][j] = false;
		}
	}
	haCambiado = false;
//---------Comprobaremos las filas-------------------------
	procesarFilas(juego, aciertos, haCambiado);
//---------Comprobaremos las columnas----------------------
	procesarCols(juego, aciertos, haCambiado);
//---------------------------------------------------------
}
void procesarFilas(tJuego &juego, tAciertos aciertos, bool &haCambiado){ //Procesar una vuelta por fila
	tFicha colorActual;
	int cont = 1;
	for(int fila = 0; fila < DIM; fila++){
		colorActual = negro;
		for(int col = 0; col < DIM; col++){
			if (colorActual == juego.tablero[fila][col]){
				cont++;
				//Caso especial para columna 8
				if ((cont >= 3) && (col == DIM-1)){ 
					for(int i = col; i > col-cont; i--){
						aciertos[fila][i] = true;
						haCambiado = true;
					}
					contarPuntos(juego, cont);
					cont = 1;
				}
			}
			else {
				if (cont >= 3){
					for(int i = col-1; i > col-1-cont; i--){
						aciertos[fila][i] = true;
						haCambiado = true;
					}
					contarPuntos(juego, cont);
				}
				cont = 1;
			}
			colorActual = juego.tablero[fila][col];	
		}
	}
}
void procesarCols(tJuego &juego, tAciertos aciertos, bool &haCambiado){ //Procesar una vuelta por columnas
	tFicha colorActual;
	int cont = 1;
	for(int col = 0; col < DIM; col++){
		colorActual = negro;
		for(int fila = 0; fila < DIM; fila++){
			if (colorActual == juego.tablero[fila][col]){
				cont++;
				//Caso especial para fila 8
				if ((cont >= 3) && (fila == DIM-1)){ 
					for(int i = fila; i > fila-cont; i--){
						aciertos[i][col] = true;
						haCambiado = true;
					}
					contarPuntos(juego, cont);
					cont = 1;
				}
			}
			else {
				if (cont >= 3){
					for(int i = fila-1; i > fila-1-cont; i--){
						aciertos[i][col] = true;
						haCambiado = true;
					}
					contarPuntos(juego, cont);
				}
				cont = 1;
			}
			colorActual = juego.tablero[fila][col];
		}
	}
}
void colorearNegro(tJuego &juego, const tAciertos aciertos){
	for(int i = 0; i < DIM; i++){
		for(int j = 0; j < DIM; j++){
			if (aciertos[i][j]){
				juego.tablero[i][j] = negro;
			}
		}
	}
}
void contarPuntos(tJuego &juego, int cont){
	if (cont == 3){
		juego.puntos = juego.puntos + 3;
	}
	else if (cont == 4){
		juego.puntos = juego.puntos + 8;
	}
	else if (cont >= 5){
		juego.puntos = juego.puntos + 15;
	}
}
void caidaFichas(tJuego &juego, bool &hanCaido){
	hanCaido = false;
	for(int col = 0; col < DIM ; col++){
		for(int fila = 0; fila < DIM - 1; fila++){
			if (juego.tablero[fila][col] == negro){
				juego.tablero[fila][col] = juego.tablero[fila + 1][col];
				juego.tablero[fila + 1][col] = negro;
				hanCaido = true;
			}
		}
		if (juego.tablero[DIM - 1][col] == negro){
			juego.tablero[DIM - 1][col] = tFicha(rand() % NUMCOLOR);
			hanCaido = true;
		}
	}
}
void gestionCaida(tJuego &juego){
	bool hanCaido = true;
	while (hanCaido){
		caidaFichas(juego, hanCaido);
		if (hanCaido){
			mostrarTablero(juego);
		}
	}
}
void procesarSuperTablero(tJuego &juego, tAciertos aciertos, bool &haCambiado){
	int contCambios = 0;
	while (haCambiado) {
		//Busca y destruye los grupos de fichas (notifica si hay cambios)
		procesarTablero(juego, aciertos, haCambiado);
		colorearNegro(juego, aciertos);
		if (haCambiado) {
			//Muestra el tablero
			mostrarTablero(juego);
			//Caída de fichas para ocupar las celdas vacías
			gestionCaida(juego);
			contCambios++;
		}
	}
	if(contCambios != 0) { haCambiado = true; }
}
void jugar(tJuego juego){
	bool fin = false, haCambiado = true;
	char dir;
	string mov;
	tAciertos aciertos;
	tMovimiento movimiento;
	tJuego juegoAux; //Para volver atrás
	
	mostrarTablero(juego);
	while (!fin){
	procesarSuperTablero(juego, aciertos, haCambiado);
		if (juego.intentosRestantes == 0) { fin = true; }
		else {
			mov = "";
			while(mov.length() == 0){
				cin.sync(); //Limpiamos
				cout << "Fila, columna y direccion (A, B, I, D; 0 para salir): ";
				getline(cin, mov);
				if(mov.length() == 0){
					cout << endl; cout << "Error, movimiento vacio" << endl; cout << endl;
				}
			}
			movimiento.fila = mov[0] - '0';
			if(mov.length() == 5){ //Fila Columna Direccion (5 chars)
				movimiento.fila--;
				movimiento.columna = mov[2] - '0';
				movimiento.columna--;
				dir = mov[4];
				chartDireccion(dir, movimiento);
			}
			if (movimiento.fila == 0 && mov.length() == 1) { fin = true; }
			else {
				if (esValido(mov, movimiento)){
					juegoAux = juego; //Hacemos una copia del tablero por si el movimiento es erróneo
					juego.intentosRestantes--;
					intercambiarFicha(juego, movimiento);
					haCambiado = true;
					procesarSuperTablero(juego, aciertos, haCambiado);
					if (!haCambiado){
						juego = juegoAux; //Volvemos atrás
						cout << endl; cout << "Error! No se han generado grupos de fichas" << endl; cout << endl;
					}
				}
				else {
					cout << endl; cout << "Error! Instrucciones no validas" << endl; cout << endl;
				}
			}
		}
	}
	cout << endl; cout << "Hasta pronto! " << endl;
}
bool esValido(string mov, tMovimiento movimiento){
	bool ok;
	/*Comprobaremos que la fila y la columna están entre los valores de la DIM, que el char de movimiento es correcto y
		que el string introducido tiene 3 caracteres (fila, columna y direccion) */
	if((movimiento.fila >= 0) && (movimiento.fila < DIM) && (movimiento.columna >= 0) && (movimiento.columna < DIM)
		&& (movimiento.direccion != error)){
		ok = true;
		/*Ahora comprobaremos que el movimiento no se saldrá del tablero */
		if((movimiento.fila == DIM - 1) && (movimiento.direccion == arriba)) { ok = false; }
		if((movimiento.columna == DIM - 1) && (movimiento.direccion == derecha)) { ok = false; }
		if((movimiento.fila == 0) && (movimiento.direccion == abajo)) { ok = false; }
		if((movimiento.columna == 0) && (movimiento.direccion == izquierda)) { ok = false; }
	}
	else { ok = false; }
return ok;
}
void chartDireccion(char dir, tMovimiento &movimiento){
	switch(dir) {
		case 'A': { movimiento.direccion = arriba; } break;
		case 'B': { movimiento.direccion = abajo; } break;
		case 'D': { movimiento.direccion = derecha; } break;
		case 'I': { movimiento.direccion = izquierda; } break;
		default: { movimiento.direccion = error; } break;
	}
}
void intercambiarFicha(tJuego &juego, tMovimiento movimiento){
	tFicha ficha;
	switch(movimiento.direccion) {
		case arriba: {
			ficha = juego.tablero[movimiento.fila+1][movimiento.columna];
			juego.tablero[movimiento.fila+1][movimiento.columna] = juego.tablero[movimiento.fila][movimiento.columna];
			juego.tablero[movimiento.fila][movimiento.columna] = ficha;
		} break;
		case abajo: {
			ficha = juego.tablero[movimiento.fila-1][movimiento.columna];
			juego.tablero[movimiento.fila-1][movimiento.columna] = juego.tablero[movimiento.fila][movimiento.columna];
			juego.tablero[movimiento.fila][movimiento.columna] = ficha;	 
		} break;
		case derecha: {
			ficha = juego.tablero[movimiento.fila][movimiento.columna+1];
			juego.tablero[movimiento.fila][movimiento.columna+1] = juego.tablero[movimiento.fila][movimiento.columna];
			juego.tablero[movimiento.fila][movimiento.columna] = ficha;	  
		} break;
		case izquierda: {
			ficha = juego.tablero[movimiento.fila][movimiento.columna-1];
			juego.tablero[movimiento.fila][movimiento.columna-1] = juego.tablero[movimiento.fila][movimiento.columna];
			juego.tablero[movimiento.fila][movimiento.columna] = ficha;
		} break;
	}
}
