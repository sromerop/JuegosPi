/** File name	: arkanoPiLib.h
 *  Description	: Archivo de cabecera que recoge las funcionalidades referidas
 *  			  a la matriz, los ladrillos, la raqueta y la pelota
 */

#ifndef _PONGPILIB_H_
#define _PONGPILIB_H_

/*Includes---------------------------------------*/
#include <stdio.h>
#include <wiringPi.h>

/*Defines---------------------------------------*/
// CONSTANTES DEL JUEGO
#define MATRIZ_ANCHO 	10
#define MATRIZ_ALTO 	7
#define RAQUETA_ANCHO 	3
#define RAQUETA_ALTO 	1

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
	// Matriz de ocupación de las distintas posiciones que conforman el display
	// (correspondiente al estado encendido/apagado de cada uno de los leds)
	int matriz[MATRIZ_ANCHO][MATRIZ_ALTO];
} tipo_pantalla;

typedef struct {
  tipo_pantalla pantalla;
  tipo_raqueta raqueta1;
  tipo_raqueta raqueta2;
  tipo_pelota pelota;
} tipo_pongPi;

extern tipo_pantalla pantalla_inicial;

//------------------------------------------------------
// FUNCIONES DE INICIALIZACION / RESET
//------------------------------------------------------
void ReseteaMatriz(tipo_pantalla *p_pantalla);
void ReseteaPelota(tipo_pelota *p_pelota);
void ReseteaRaquetas(tipo_raqueta *p_raqueta_1, tipo_raqueta *p_raqueta_2);

//------------------------------------------------------
// FUNCIONES DE VISUALIZACION (ACTUALIZACION DEL OBJETO PANTALLA QUE LUEGO USARA EL DISPLAY)
//------------------------------------------------------
void PintaMensajeInicialPantalla (tipo_pantalla *p_pantalla, tipo_pantalla *p_pantalla_inicial);
void PintaPantallaPorTerminal (tipo_pantalla *p_pantalla);
void PintaRaquetas(tipo_raqueta *p_raqueta_1, tipo_raqueta *p_raqueta_2, tipo_pantalla *p_pantalla);
void PintaPelota(tipo_pelota *p_pelota, tipo_pantalla *p_pantalla);
void ActualizaPantalla(tipo_pongPi* p_pongPi);

void InicializaPongPi(tipo_pongPi *p_pongPi);
int CalculaLadrillosRestantes(tipo_pantalla *p_ladrillos);

#endif /* _PONGPILIB_H_ */
