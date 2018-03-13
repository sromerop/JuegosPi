/** File name	: arkanoPi_1.h
 *  Description	: Archivo de cabecera del programa principal
 */

#ifndef _ARKANOPI_H_
#define _ARKANOPI_H_

/*Includes---------------------------------------*/
#include <time.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <sys/time.h>
#include <wiringPi.h>
#include <softPwm.h>

#include "arkanoPiLib.h"
#include "kbhit.h" 	// para poder detectar teclas pulsadas sin bloqueo y leer las teclas pulsadas
#include "fsm.h" 	// para poder crear y ejecutar la m�quina de estados
#include "tmr.h" 	// para poder crear y ejecutar el temporizador

/*Defines---------------------------------------*/
#define CLK_MS 10 	// PERIODO DE ACTUALIZACION DE LA MAQUINA ESTADOS
#define TIMEOUT 1	// tiempo para llamar al temporizador
#define TIMEOUT_PELOTA	1000
#define DEBOUNCE_TIME	200

// FLAGS DEL SISTEMA
#define FLAG_TECLA 				0x01
//#define FLAG_PELOTA 			0x02
#define FLAG_RAQUETA_DERECHA 	0x04
#define FLAG_RAQUETA_IZQUIERDA 	0x08
#define FLAG_FINAL_JUEGO 		0x10

// A 'key' which we can lock and unlock - values are 0 through 3
//	This is interpreted internally as a pthread_mutex by wiringPi
//	which is hiding some of that to make life simple.
#define	FLAGS_KEY	1
#define	STD_IO_BUFFER_KEY	2

//	Salidas del HW
#define GPIO_FILA_0	0
#define GPIO_FILA_1	1
#define GPIO_FILA_2	2
#define GPIO_FILA_3	3
#define GPIO_FILA_4	4
#define GPIO_FILA_5	7
#define GPIO_FILA_6	23
#define GPIO_COL_0	14
#define GPIO_COL_1	17
#define GPIO_COL_2	18
#define GPIO_COL_3	22
#define GPIO_PWM	24

//	Entradas del HW
#define GPIO_BOTON_DER		16
#define GPIO_BOTON_IZQ		15
#define GPIO_BOTON_TECLA	21

void delay_until (unsigned int next);
void fsm_setup(fsm_t* inicio_fsm);

//------------------------------------------------------
// FUNCIONES DE ENTRADA O DE TRANSICION DE LA MAQUINA DE ESTADOS
//------------------------------------------------------
int compruebaTeclaPulsada (fsm_t* this);
//int compruebaTeclaPelota (fsm_t* this);
int compruebaTeclaRaquetaDerecha (fsm_t* this);
int compruebaTeclaRaquetaIzquierda (fsm_t* this);
int compruebaFinalJuego (fsm_t* this);

//------------------------------------------------------
// FUNCIONES DE SALIDA O DE ACCION DE LA MAQUINA DE ESTADOS
//------------------------------------------------------
void InicializaJuegoFSM(fsm_t* fsm);
void MovimientoPelotaFSM(fsm_t* fsm);
void MueveRaquetaDerechaFSM(fsm_t* fsm);
void MueveRaquetaIzquierdaFSM(fsm_t* fsm);
void FinalJuegoFSM(fsm_t* fsm);
void ReseteaJuegoFSM(fsm_t* fsm);

//------------------------------------------------------
// SUBRUTINAS DE ATENCION A LAS INTERRUPCIONES
//------------------------------------------------------
PI_THREAD (thread_explora_teclado);

typedef enum {
	WAIT_START,
	WAIT_PUSH,
	WAIT_PAUSE,
	WAIT_END} tipo_estados_juego;

typedef struct {
	tipo_arkanoPi arkanoPi;
	//tipo_estados_juego estado;
	char teclaPulsada;
} tipo_juego;

//------------------------------------------------------
// FUNCIONES DE ACCION
//------------------------------------------------------

void InicializaJuego (void);
void MueveRaquetaIzquierda (void);
void MueveRaquetaDerecha (void);
void MovimientoPelota (void);
void FinalJuego (void);
void ReseteaJuego (void);

//------------------------------------------------------
// FUNCIONES DE INICIALIZACION
//------------------------------------------------------
int systemSetup (void);

#endif /* ARKANOPI_H_ */
