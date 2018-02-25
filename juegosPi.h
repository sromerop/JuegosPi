/** File name	: juegosPi.h
 *  Description	: Archivo de cabecera del programa principal
 */

#ifndef JUEGOSPI_H_
#define JUEGOSPI_H_


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

#include "juegosPiLib.h"
#include "kbhit.h" 	// para poder detectar teclas pulsadas sin bloqueo y leer las teclas pulsadas
#include "fsm.h" 	// para poder crear y ejecutar la máquina de estados
#include "tmr.h" 	// para poder crear y ejecutar el temporizador

/*Defines---------------------------------------*/
#define CLK_MS 10 	// PERIODO DE ACTUALIZACION DE LA MAQUINA ESTADOS
#define TIMEOUT 1	// tiempo para llamar al temporizador
#define TIMEOUT_PELOTA			1000
#define TIMEOUT_PELOTA_PONG		500
#define TIMEOUT_SERPIENTE		500
#define DEBOUNCE_TIME			200

// FLAGS DEL SISTEMA
#define FLAG_TECLA 				0x01
//#define FLAG_PELOTA 			0x02 	//equivale también a FLAG_PELOTA (Pong) y FLAG_SERPIENTE
#define FLAG_RAQUETA_DERECHA 	0x04 	//equivale también a FLAG_RAQUETA_1_UP (Pong) y FLAG_SERPIENTE_RIGHT 
#define FLAG_RAQUETA_IZQUIERDA 	0x08 	//equivale también a FLAG_RAQUETA_1_DOWN y FLAG_SERPIENTE_LEFT 
#define FLAG_FINAL_JUEGO 		0x10
#define FLAG_RAQUETA_UP		    0x40	//equivale también a FLAG_SERPIENTE_UP
#define FLAG_RAQUETA_DOWN		0x80	//equivale también a FLAG_SERPIENTE_DOWN


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
#define GPIO_BOTON_DER		16	//equivale también a GPIO_BOTON_1_UP (Pong) y GPIO_BOTON_RIGHT (Snake)
#define GPIO_BOTON_IZQ		15	//equivale también a GPIO_BOTON_1_DOWN (Pong)y GPIO_BOTON_LEFT (Snake)
#define GPIO_BOTON_TECLA	21
#define GPIO_BOTON_2_UP		20	//equivale también a GPIO_BOTON_UP (Snake)
#define GPIO_BOTON_2_DOWN	19	//equivale también a GPIO_BOTON_DOWN(Snake)
#define GPIO_BOTON_SALIDA	9

void delay_until (unsigned int next);
void fsm_setup(fsm_t* inicio_fsm);

//------------------------------------------------------
// FUNCIONES DE ENTRADA O DE TRANSICION DE LA MAQUINA DE ESTADOS
//------------------------------------------------------
int compruebaTeclaPulsada (fsm_t* this);
int compruebaTeclaRaquetaDerecha (fsm_t* this);
int compruebaTeclaRaquetaIzquierda (fsm_t* this);
//int compruebaTeclaPelota (fsm_t* this);
int compruebaPausePlay (fsm_t* this);
int compruebaPausePlay2 (fsm_t* this);
int compruebaTeclaRaqueta2Up (fsm_t* this);
int compruebaTeclaRaqueta2Down (fsm_t* this);
int compruebaFinalJuego (fsm_t* this);

//------------------------------------------------------
// FUNCIONES DE SALIDA O DE ACCION DE LA MAQUINA DE ESTADOS
//------------------------------------------------------
void InicializaJuegoArkanoFSM(fsm_t* fsm);
void InicializaJuegoPongFSM(fsm_t* fsm);
void InicializaJuegoSnakeFSM(fsm_t* fsm);
void MovimientoPelotaFSM(fsm_t* fsm);
void MueveRaquetaDerechaFSM(fsm_t* fsm);
void MueveRaquetaIzquierdaFSM(fsm_t* fsm);
void PauseIt(fsm_t* fsm);
void PlayIt(fsm_t* fsm);
void PauseIt1(fsm_t* fsm);
void PlayIt1(fsm_t* fsm);
void PauseIt2(fsm_t* fsm);
void PlayIt2(fsm_t* fsm);
void MovimientoPelotaPongFSM(fsm_t* fsm);
void MueveRaqueta1UpFSM(fsm_t* fsm);
void MueveRaqueta1DownFSM(fsm_t* fsm);
void MueveRaqueta2UpFSM(fsm_t* fsm);
void MueveRaqueta2DownFSM(fsm_t* fsm);
void PauseIt(fsm_t* fsm);
void MovimientoSerpienteFSM(fsm_t* fsm);
void PauseUpDown(fsm_t* fsm);
void PlayItRightLeft(fsm_t* fsm);
void MueveSerpienteUpFSM(fsm_t* fsm);
void MueveSerpienteDownFSM(fsm_t* fsm);
void MueveSerpienteRightFSM(fsm_t* fsm);
void MueveSerpienteLeftFSM(fsm_t* fsm);
void FinalJuegoFSM(fsm_t* fsm);
void ReseteaJuegoFSM(fsm_t* fsm);

//------------------------------------------------------
// SUBRUTINAS DE ATENCION A LAS INTERRUPCIONES
//------------------------------------------------------
PI_THREAD (thread_explora_teclado);

typedef enum {
	WAIT_START,
	WAIT_PUSH_ARKANO,
	WAIT_PUSH_PONG,
	WAIT_PUSH_SNAKE,
	WAIT_PAUSE_ARKANO,
	WAIT_PAUSE_1,
	WAIT_PAUSE_2,
	WAIT_PAUSE_RL,
	WAIT_PAUSE_UD,
	WAIT_END} tipo_estados_juego;

	typedef struct {
		tipo_arkanoPi arkanoPi;
		tipo_pongPi pongPi;
		tipo_snakePi snakePi;
		//tipo_juegosPi juegosPi;
		tipo_pantalla pantalla;
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
