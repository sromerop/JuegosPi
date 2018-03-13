/** File name	: snakePiLib.h
 *  Description	: Archivo de cabecera que recoge las funcionalidades
 *  			  referidas a la matriz, la serpiente y las frutas
 */

#ifndef _SNAKEPILIB_H_
#define _SNAKEPILIB_H_

/*Includes---------------------------------------*/
#include <stdio.h>
#include <wiringPi.h>

/*Defines---------------------------------------*/
// CONSTANTES DEL JUEGO
#define MATRIZ_ANCHO 	10
#define MATRIZ_ALTO 	7
#define SERPIENTE_ANCHO 1
#define SERPIENTE_ALTO 	3

typedef struct {
	// Posicion
	int x[70];
	int y[70];
	int x_antigua;
	int y_antigua;
	int x_antigua1;
	int y_antigua1;
	// Forma
	int ancho;
	int alto;
	// Trayectoria
	int xv;
	int yv;
} tipo_serpiente;

typedef struct {
	// Posicion
	int x;
	int y;
} tipo_fruta;

typedef struct {
	// Matriz de ocupación de las distintas posiciones que conforman el display
	// (correspondiente al estado encendido/apagado de cada uno de los leds)
	int matriz[MATRIZ_ANCHO][MATRIZ_ALTO];
} tipo_pantalla;

typedef struct {
  tipo_pantalla pantalla;
  tipo_serpiente serpiente;
  tipo_fruta fruta;
} tipo_snakePi;

extern tipo_pantalla pantalla_inicial;

//------------------------------------------------------
// FUNCIONES DE INICIALIZACION / RESET
//------------------------------------------------------
void ReseteaMatriz(tipo_pantalla *p_pantalla);
void ReseteaSerpiente(tipo_serpiente *p_serpiente);
void ReseteaFruta(tipo_fruta *p_fruta);
void ReseteaFrutaAleatorio(tipo_fruta *p_fruta);
//------------------------------------------------------
// FUNCIONES DE VISUALIZACION (ACTUALIZACION DEL OBJETO PANTALLA QUE LUEGO USARA EL DISPLAY)
//------------------------------------------------------
void PintaMensajeInicialPantalla (tipo_pantalla *p_pantalla, tipo_pantalla *p_pantalla_inicial);
void PintaPantallaPorTerminal (tipo_pantalla *p_pantalla);
void PintaSerpiente(tipo_serpiente *p_serpiente, tipo_pantalla *p_pantalla);
void PintaFruta(tipo_fruta *p_fruta, tipo_pantalla *p_pantalla);
void ActualizaPantalla(tipo_snakePi* p_snakePi);

void InicializaSnakePi(tipo_snakePi *p_snakePi);

#endif /* _SNAKEPILIB_H_ */
