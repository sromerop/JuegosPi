/*
 * juegosPiLib.h
 *
 *  Created on: May 11, 2017
 *      Author: javier
 */

#ifndef JUEGOSPILIB_H_
#define JUEGOSPILIB_H_

/*Includes---------------------------------------*/
#include <stdio.h>
#include <stdlib.h>
#include <wiringPi.h>

/*Defines---------------------------------------*/
// CONSTANTES DEL JUEGO
#define MATRIZ_ANCHO 	10
#define MATRIZ_ALTO 	7
#define LADRILLOS_ANCHO 10
#define LADRILLOS_ALTO 	2
#define RAQUETA_ANCHO 	3
#define RAQUETA_ALTO 	1
#define SERPIENTE_ANCHO 1
#define SERPIENTE_ALTO 	3

typedef struct {
	// Posicion
	int x;
	int y;
	// Forma
	int ancho;
	int alto;
} tipo_raqueta;

typedef struct {
	// Posicion
	int x;
	int y;
	// Trayectoria
	int xv;
	int yv;
} tipo_pelota;

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
  tipo_pantalla ladrillos;
  tipo_pantalla pantalla;
  tipo_raqueta raqueta;
  tipo_pelota pelota;
} tipo_arkanoPi;

typedef struct {
  tipo_pantalla pantalla;
  tipo_raqueta raqueta1;
  tipo_raqueta raqueta2;
  tipo_pelota pelota;
} tipo_pongPi;

typedef struct {
  tipo_pantalla pantalla;
  tipo_serpiente serpiente;
  tipo_fruta fruta;
} tipo_snakePi;

/*typedef struct {
  tipo_pantalla ladrillos;
  tipo_pantalla pantalla;
  tipo_raqueta raqueta;
  tipo_raqueta raqueta1;
  tipo_raqueta raqueta2;
  tipo_pelota pelota;
  tipo_serpiente serpiente;
  tipo_fruta fruta;
} tipo_juegosPi;*/

extern tipo_pantalla pantalla_inicial;

//------------------------------------------------------
// FUNCIONES DE INICIALIZACION / RESET
//------------------------------------------------------
void ReseteaMatriz(tipo_pantalla *p_pantalla);
void ReseteaLadrillos(tipo_pantalla *p_ladrillos);
void ReseteaPelota(tipo_pelota *p_pelota);
void ReseteaRaqueta(tipo_raqueta *p_raqueta);
void ReseteaPelotaPong(tipo_pelota *p_pelota);
void ReseteaRaquetas(tipo_raqueta *p_raqueta_1, tipo_raqueta *p_raqueta_2);
void ReseteaSerpiente(tipo_serpiente *p_serpiente);
void ReseteaFruta(tipo_fruta *p_fruta);
//void ReseteaFrutaAleatorio(tipo_fruta *p_fruta);
void ReseteaFrutaAleatorio(tipo_fruta *p_fruta, tipo_pantalla *p_pantalla);

//------------------------------------------------------
// FUNCIONES DE VISUALIZACION (ACTUALIZACION DEL OBJETO PANTALLA QUE LUEGO USARA EL DISPLAY)
//------------------------------------------------------
void PintaMensajeInicialPantalla (tipo_pantalla *p_pantalla, tipo_pantalla *p_pantalla_inicial);
void PintaPantallaPorTerminal (tipo_pantalla *p_pantalla);
void PintaLadrillos(tipo_pantalla *p_ladrillos, tipo_pantalla *p_pantalla);
void PintaRaqueta(tipo_raqueta *p_raqueta, tipo_pantalla *p_pantalla);
void PintaRaquetas(tipo_raqueta *p_raqueta_1, tipo_raqueta *p_raqueta_2, tipo_pantalla *p_pantalla);
void PintaPelota(tipo_pelota *p_pelota, tipo_pantalla *p_pantalla);
void PintaPelotaPong(tipo_pelota *p_pelota, tipo_pantalla *p_pantalla);
void PintaSerpiente(tipo_serpiente *p_serpiente, tipo_pantalla *p_pantalla);
void PintaFruta(tipo_fruta *p_fruta, tipo_pantalla *p_pantalla);
void ActualizaPantallaArkano(tipo_arkanoPi* p_arkanoPi);
void ActualizaPantallaPong(tipo_pongPi *p_pongPi);
void ActualizaPantallaSnake(tipo_snakePi* p_snakePi);

void InicializaArkanoPi(tipo_arkanoPi *p_arkanoPi);
void InicializaPongPi(tipo_pongPi *p_pongPi);
void InicializaSnakePi(tipo_snakePi *p_snakePi);
int CalculaLadrillosRestantes(tipo_pantalla *p_ladrillos);

#endif /* _JUEGOSPILIB_H_ */
