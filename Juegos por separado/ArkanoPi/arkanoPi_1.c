#include "arkanoPi_1.h"

static volatile tipo_juego juego;

volatile int flags = 0;
tmr_t* tmr;
tmr_t* tmr_pelota;
static int gpio_cols[4]={GPIO_COL_0,GPIO_COL_1,GPIO_COL_2,GPIO_COL_3};  // array de columnas
unsigned volatile int columna = 0;
int debounceTime;
fsm_t* inicio_fsm;
tipo_pantalla mensaje_inicial;
int cnt_rebotes = 1;
int v_pelota = TIMEOUT_PELOTA;
int vidas = 3;
int nivel = 1;
tipo_pantalla ladrillos_nv2;
tipo_pantalla ladrillos_nv3;
tipo_pantalla mensaje_victoria;
tipo_pantalla mensaje_derrota;
//------------------------------------------------------
// FUNCIONES DE ACCION
//------------------------------------------------------

// espera hasta la próxima activación del reloj
void delay_until (unsigned int next) {
	unsigned int now = millis();
	if (next > now) {
		delay (next - now);
    }
}

//------------------------------------------------------
// FUNCIONES DE LA MAQUINA DE ESTADOS
//------------------------------------------------------

// FUNCIONES DE ENTRADA O COMPROBACIÓN DE LA MAQUINA DE ESTADOS

/**
* @brief función donde se comprueba la interrupción de la pulsación del boton_tecla
* @param this
* @return devuelve 1 si la iterrupción ha sido causada por esta pulsación; 0 en caso contrario
*/
int compruebaTeclaPulsada (fsm_t* this) {
	int result;

	piLock (FLAGS_KEY);
	result = (flags & FLAG_TECLA);
	piUnlock (FLAGS_KEY);

	return result;
}
/*int compruebaTeclaPelota (fsm_t* this) {
	int result;

	piLock (FLAGS_KEY);
	result = (flags & FLAG_PELOTA);
	piUnlock (FLAGS_KEY);

	return result;
}*/
/**
* @brief función donde se comprueba la interrupción de la pulsación del boton_derecho
* @param this
* @return devuelve 1 si la iterrupción ha sido causada por esta pulsación; 0 en caso contrario
*/
int compruebaTeclaRaquetaDerecha (fsm_t* this) {
	int result;

	piLock (FLAGS_KEY);
	result = (flags & FLAG_RAQUETA_DERECHA);
	piUnlock (FLAGS_KEY);

	return result;
}
/**
* @brief función donde se comprueba la interrupción de la pulsación del boton_izquierdo
* @param this
* @return devuelve 1 si la iterrupción ha sido causada por esta pulsación; 0 en caso contrario
*/
int compruebaTeclaRaquetaIzquierda (fsm_t* this) {
	int result;

	piLock (FLAGS_KEY);
	result = (flags & FLAG_RAQUETA_IZQUIERDA);
	piUnlock (FLAGS_KEY);

	return result;
}
/**
* @brief función donde se comprueba la interrupción de la pulsación simultánea del boton_derecho y boron_izquierdo
* @param this
* @return devuelve 1 si la iterrupción ha sido causada por estas pulsaciones; 0 en caso contrario
*/
int compruebaPausePlay (fsm_t* this) {
	int result;

	piLock (FLAGS_KEY);
	result = (flags & (FLAG_RAQUETA_DERECHA || FLAG_RAQUETA_IZQUIERDA));
	piUnlock (FLAGS_KEY);

	return result;
}
/**
* @brief función donde se comprueba la interrupción del final de juego
* @param this
* @return devuelve 1 si la iterrupción ha sido causada por el fin del juego; 0 en caso contrario
*/
int compruebaFinalJuego (fsm_t* this) {
	int result;

	piLock (FLAGS_KEY);
	result = (flags & FLAG_FINAL_JUEGO);
	piUnlock (FLAGS_KEY);

	return result;
}

// FUNCIONES DE SALIDA O ACCION DE LA MAQUINA DE ESTADOS

/**
* @brief rutina donde se inicia el juego, sacando por pantalla y dibujando ladrillos_basico
* @param fsm
*/
void InicializaJuegoFSM(fsm_t* fsm) {
	piLock (FLAGS_KEY);
	flags &= ~FLAG_TECLA;
	piUnlock (FLAGS_KEY);

	InicializaArkanoPi((tipo_arkanoPi*)(&juego.arkanoPi));
	tmr_startms(tmr_pelota, TIMEOUT_PELOTA);

	piLock (STD_IO_BUFFER_KEY);
	ActualizaPantalla((tipo_arkanoPi*)(&juego.arkanoPi));
	PintaPantallaPorTerminal((tipo_pantalla*)(&juego.arkanoPi.pantalla));
	piUnlock (STD_IO_BUFFER_KEY);

}
/**
* @brief rutina donde se modifica la pelota en una posición siguiendo su trayectoria, pudiendo avanzar de nivel, ganar o perder el juego.
* @param fsm
*/
void MovimientoPelotaFSM(fsm_t* fsm) {
	//piLock (FLAGS_KEY);
	//flags &= ~FLAG_PELOTA;
	//piUnlock (FLAGS_KEY);

		int pelota_x = juego.arkanoPi.pelota.x;
		int pelota_y = juego.arkanoPi.pelota.y;
		int pelota_xv = juego.arkanoPi.pelota.xv;
		int pelota_yv = juego.arkanoPi.pelota.yv;
		int pelota_sig_x = pelota_x + pelota_xv;
		int pelota_sig_y = pelota_y + pelota_yv;

		int raqueta_x = juego.arkanoPi.raqueta.x;
		int raqueta_y = juego.arkanoPi.raqueta.y;
		
		softPwmWrite(GPIO_PWM, 0);
		
		if (pelota_x == 0 || pelota_x == 9 || pelota_y == 0 || pelota_y == 5){
			if (pelota_x == 0){
				if (pelota_xv < 0){
					juego.arkanoPi.pelota.xv = 1;
					pelota_xv = juego.arkanoPi.pelota.xv;
					pelota_sig_x = pelota_x + pelota_xv;
					cnt_rebotes++;
					softPwmWrite(GPIO_PWM, 75);
				}
			}
			if (pelota_x == 9){
				if (pelota_xv > 0){
					juego.arkanoPi.pelota.xv = -1;
					pelota_xv = juego.arkanoPi.pelota.xv;
					pelota_sig_x = pelota_x + pelota_xv;
					cnt_rebotes++;
					softPwmWrite(GPIO_PWM, 75);
				}
			}
			if (pelota_y == 0){
				if (pelota_yv < 0){
					juego.arkanoPi.pelota.yv = 1;
					pelota_yv = juego.arkanoPi.pelota.yv;
					pelota_sig_y = pelota_y + pelota_yv;
					cnt_rebotes++;
					softPwmWrite(GPIO_PWM, 75);
				}
			}
			if (pelota_y == 5){
				if (pelota_yv > 0){
					if ((pelota_sig_x == (raqueta_x + 2)) && (pelota_sig_y == raqueta_y)) { // derecha
						if (pelota_x == 9){
							juego.arkanoPi.pelota.xv = -1;
							juego.arkanoPi.pelota.yv = -1;
							pelota_xv = juego.arkanoPi.pelota.xv;
							pelota_yv = juego.arkanoPi.pelota.yv;
							pelota_sig_x = pelota_x + pelota_xv;
							pelota_sig_y = pelota_y + pelota_yv;
						}else{
							juego.arkanoPi.pelota.xv = 1;
							juego.arkanoPi.pelota.yv = -1;
							pelota_xv = juego.arkanoPi.pelota.xv;
							pelota_yv = juego.arkanoPi.pelota.yv;
							pelota_sig_x = pelota_x + pelota_xv;
							pelota_sig_y = pelota_y + pelota_yv;
						}
						cnt_rebotes++;
						softPwmWrite(GPIO_PWM, 50);
					}else if ((pelota_sig_x == (raqueta_x + 1)) && (pelota_sig_y == raqueta_y)) { // centro
							juego.arkanoPi.pelota.xv = 0;
							juego.arkanoPi.pelota.yv = -1;
							pelota_xv = juego.arkanoPi.pelota.xv;
							pelota_yv = juego.arkanoPi.pelota.yv;
							pelota_sig_x = pelota_x + pelota_xv;
							pelota_sig_y = pelota_y + pelota_yv;
							cnt_rebotes++;
							softPwmWrite(GPIO_PWM, 50);
					}else if ((pelota_sig_x == (raqueta_x)) && (pelota_sig_y == raqueta_y)) { // izquierda
						if (pelota_x == 0){
							juego.arkanoPi.pelota.xv = 1;
							juego.arkanoPi.pelota.yv = -1;
							pelota_xv = juego.arkanoPi.pelota.xv;
							pelota_yv = juego.arkanoPi.pelota.yv;
							pelota_sig_x = pelota_x + pelota_xv;
							pelota_sig_y = pelota_y + pelota_yv;
						}else{
							juego.arkanoPi.pelota.xv = -1;
							juego.arkanoPi.pelota.yv = -1;
							pelota_xv = juego.arkanoPi.pelota.xv;
							pelota_yv = juego.arkanoPi.pelota.yv;
							pelota_sig_x = pelota_x + pelota_xv;
							pelota_sig_y = pelota_y + pelota_yv;
						}
						cnt_rebotes++;
						softPwmWrite(GPIO_PWM, 50);
					}else{
						juego.arkanoPi.pelota.y = 6;
						//juego.arkanoPi.pelota.x += juego.arkanoPi.pelota.xv;
						vidas--;
						if (vidas == 0) {
							piLock (FLAGS_KEY);
							flags |= FLAG_FINAL_JUEGO;
							piUnlock (FLAGS_KEY);
						} else {
							ReseteaPelota((tipo_pelota*)(&juego.arkanoPi.pelota));
							ReseteaRaqueta((tipo_raqueta*)(&juego.arkanoPi.raqueta));

							cnt_rebotes = 1;
							v_pelota = TIMEOUT_PELOTA;
						}
					}
				}
			}
		}

		if (juego.arkanoPi.ladrillos.matriz [pelota_sig_x][pelota_sig_y] >= 1){ // ladrillo
			juego.arkanoPi.ladrillos.matriz [pelota_sig_x][pelota_sig_y] -= 1;
			cnt_rebotes++;
			softPwmWrite(GPIO_PWM, 25);
			if (CalculaLadrillosRestantes((tipo_pantalla*)(&juego.arkanoPi.ladrillos.matriz)) == 0) { // no quedan m�s ladrillos
					nivel++;
					if (nivel == 2){
						ReseteaMatriz((tipo_pantalla*)(&juego.arkanoPi.pantalla.matriz));
						juego.arkanoPi.ladrillos = ladrillos_nv2;
						PintaLadrillos((tipo_pantalla*)(&ladrillos_nv2), (tipo_pantalla*)(&juego.arkanoPi.pantalla));
						ReseteaPelota((tipo_pelota*)(&juego.arkanoPi.pelota));
						ReseteaRaqueta((tipo_raqueta*)(&juego.arkanoPi.raqueta));
						cnt_rebotes = 1;
						v_pelota = TIMEOUT_PELOTA - 200;
						vidas++;
					}else if (nivel == 3){
						ReseteaMatriz((tipo_pantalla*)(&juego.arkanoPi.pantalla.matriz));
						juego.arkanoPi.ladrillos = ladrillos_nv3;
						PintaLadrillos((tipo_pantalla*)(&ladrillos_nv3), (tipo_pantalla*)(&juego.arkanoPi.pantalla));
						ReseteaPelota((tipo_pelota*)(&juego.arkanoPi.pelota));
						ReseteaRaqueta((tipo_raqueta*)(&juego.arkanoPi.raqueta));
						cnt_rebotes = 1;
						v_pelota = TIMEOUT_PELOTA - 400;
						vidas++;
					}else if (nivel == 4){
						piLock (FLAGS_KEY);
						flags |= FLAG_FINAL_JUEGO;
						piUnlock (FLAGS_KEY);
					}
			}
			if (pelota_y == 0){
				juego.arkanoPi.pelota.xv = - pelota_xv;
				pelota_xv = juego.arkanoPi.pelota.xv;
				pelota_sig_x = pelota_x + pelota_xv;
			} else {
			juego.arkanoPi.pelota.yv = - pelota_yv;
			pelota_yv = juego.arkanoPi.pelota.yv;
			pelota_sig_y = pelota_y + pelota_yv;
			}

		}
		if (juego.arkanoPi.pelota.y != 6){
		juego.arkanoPi.pelota.x += juego.arkanoPi.pelota.xv;
		juego.arkanoPi.pelota.y += juego.arkanoPi.pelota.yv;
		}
	piLock (STD_IO_BUFFER_KEY);
		ActualizaPantalla((tipo_arkanoPi*)(&juego.arkanoPi));
		PintaPantallaPorTerminal((tipo_pantalla*)(&juego.arkanoPi.pantalla));
	piUnlock (STD_IO_BUFFER_KEY);
}
/**
* @brief método asignado al temporizador que hace mover la pelota automáticamente
*/
void pelota_auto(union sigval value) {
	if ((cnt_rebotes % 5) == 0) {
		v_pelota -= 10;
		if (v_pelota <= 200){
			v_pelota = 200;
		}
	}
	MovimientoPelotaFSM(inicio_fsm);
	tmr_startms(tmr_pelota, v_pelota);
}
/**
* @brief rutina donde se modifica la raqueta en una posición hacia la derecha
* @param fsm
*/
void MueveRaquetaDerechaFSM(fsm_t *fsm) {
	piLock (FLAGS_KEY);
	flags &= ~FLAG_RAQUETA_DERECHA;
	piUnlock (FLAGS_KEY);

	if(juego.arkanoPi.raqueta.x < (MATRIZ_ANCHO - RAQUETA_ANCHO)+1){
		juego.arkanoPi.raqueta.x = juego.arkanoPi.raqueta.x+1;
	} else {
		juego.arkanoPi.raqueta.x = (MATRIZ_ANCHO - RAQUETA_ANCHO)+1;
	}
	piLock (STD_IO_BUFFER_KEY);
	ActualizaPantalla((tipo_arkanoPi*)(&juego.arkanoPi));
	PintaPantallaPorTerminal((tipo_pantalla*)(&juego.arkanoPi.pantalla));
	piUnlock (STD_IO_BUFFER_KEY);
}
/**
* @brief método asignado a la inturrupción por la pulsación del boton asignado
*/
void boton_derecho(void) {
	// Pin event (key / button event) debouncing procedure
		if (millis () < debounceTime)
		{
			debounceTime = millis () + DEBOUNCE_TIME ;
			return;
		}

		piLock (FLAGS_KEY);
		flags |= FLAG_RAQUETA_DERECHA;
		piUnlock (FLAGS_KEY);

			// Wait for key to be released
		while (digitalRead (GPIO_BOTON_DER) == HIGH) {
			delay (1) ;
			if (digitalRead (GPIO_BOTON_IZQ) == HIGH) {
				piLock (FLAGS_KEY);
				flags |= (FLAG_RAQUETA_DERECHA || FLAG_RAQUETA_IZQUIERDA);
				piUnlock (FLAGS_KEY);
			}
		}

		debounceTime = millis () + DEBOUNCE_TIME ;
}
/**
* @brief rutina donde se modifica la raqueta en una posición hacia la izquierda
* @param fsm
*/
void MueveRaquetaIzquierdaFSM(fsm_t *fsm) {
	piLock (FLAGS_KEY);
	flags &= ~FLAG_RAQUETA_IZQUIERDA;
	piUnlock (FLAGS_KEY);

	if(juego.arkanoPi.raqueta.x > -1){
			juego.arkanoPi.raqueta.x = juego.arkanoPi.raqueta.x-1;
		} else {
			juego.arkanoPi.raqueta.x = -1;
		}
	piLock (STD_IO_BUFFER_KEY);
	ActualizaPantalla((tipo_arkanoPi*)(&juego.arkanoPi));
	PintaPantallaPorTerminal((tipo_pantalla*)(&juego.arkanoPi.pantalla));
	piUnlock (STD_IO_BUFFER_KEY);
}
/**
* @brief método asignado a la inturrupción por la pulsación del boton asignado
*/
void boton_izquierdo(void) {
	// Pin event (key / button event) debouncing procedure
			if (millis () < debounceTime)
			{
				debounceTime = millis () + DEBOUNCE_TIME ;
				return;
			}

			piLock (FLAGS_KEY);
			flags |= FLAG_RAQUETA_IZQUIERDA;
			piUnlock (FLAGS_KEY);

				// Wait for key to be released
			while (digitalRead (GPIO_BOTON_IZQ) == HIGH) {
				delay (1) ;
				if (digitalRead (GPIO_BOTON_DER) == HIGH) {
					piLock (FLAGS_KEY);
					flags |= (FLAG_RAQUETA_DERECHA || FLAG_RAQUETA_IZQUIERDA);
					piUnlock (FLAGS_KEY);
				}
			}

			debounceTime = millis () + DEBOUNCE_TIME ;
}
/**
* @brief rutina donde se detiene el juego
* @param fsm
*/
void PauseIt (fsm_t *fsm) {
	piLock (FLAGS_KEY);
	flags &= ~(FLAG_RAQUETA_DERECHA || FLAG_RAQUETA_IZQUIERDA);
	//flags &= ~FLAG_RAQUETA_IZQUIERDA;
	piUnlock (FLAGS_KEY);

	piLock (STD_IO_BUFFER_KEY);
	printf("Press the buttons again to continue: ... ");
	ActualizaPantalla((tipo_arkanoPi*)(&juego.arkanoPi));
	PintaPantallaPorTerminal((tipo_pantalla*)(&juego.arkanoPi.pantalla));
	piUnlock (STD_IO_BUFFER_KEY);

	tmr_destroy(tmr_pelota);
}
/**
* @brief rutina donde se reanuda el juego
* @param fsm
*/
void PlayIt (fsm_t *fsm) {
	piLock (FLAGS_KEY);
	flags &= ~(FLAG_RAQUETA_DERECHA || FLAG_RAQUETA_IZQUIERDA);
	//flags &= ~FLAG_RAQUETA_IZQUIERDA;
	piUnlock (FLAGS_KEY);

	piLock (STD_IO_BUFFER_KEY);
	printf("Continue... ");
	ActualizaPantalla((tipo_arkanoPi*)(&juego.arkanoPi));
	PintaPantallaPorTerminal((tipo_pantalla*)(&juego.arkanoPi.pantalla));
	piUnlock (STD_IO_BUFFER_KEY);

	tmr_pelota = tmr_new(pelota_auto);
	tmr_startms(tmr_pelota, v_pelota);
}
/**
* @brief método asignado a la inturrupción por la pulsación del boton asignado
*/
void boton_tecla(void) {
	// Pin event (key / button event) debouncing procedure
		if (millis () < debounceTime)
		{
			debounceTime = millis () + DEBOUNCE_TIME ;
			return;
		}

		piLock (FLAGS_KEY);
		flags |= FLAG_TECLA;
		piUnlock (FLAGS_KEY);

			// Wait for key to be released
		while (digitalRead (GPIO_BOTON_TECLA) == HIGH) {
			delay (1) ;
		}

		debounceTime = millis () + DEBOUNCE_TIME ;
}
/*void boton_salida(void) {
	// Pin event (key / button event) debouncing procedure
		if (millis () < debounceTime)
		{
			debounceTime = millis () + DEBOUNCE_TIME ;
			return;
		}
		exit(0);

			// Wait for key to be released
		while (digitalRead (GPIO_BOTON_SALIDA) == HIGH) {
			delay (1) ;
		}

		debounceTime = millis () + DEBOUNCE_TIME ;
}*/
/**
* @brief rutina donde se finaliza el juego y se muestra un mensaje por pantalla dependiendo de si se ha ganado o se ha perdido
* @param fsm
*/
void FinalJuegoFSM(fsm_t* fsm) {
	piLock (FLAGS_KEY);
	flags &= ~FLAG_FINAL_JUEGO;
	piUnlock (FLAGS_KEY);

	piLock (STD_IO_BUFFER_KEY);
	if (nivel == 4) {// suponiendo 3 el numero niveles m�ximo
			printf("\n");
			printf("You win!");
			printf("\n");
			//juego.arkanoPi.ladrillos = mensaje_victoria;
			PintaMensajeInicialPantalla((tipo_pantalla*)(&juego.arkanoPi.pantalla),(tipo_pantalla*)(&mensaje_victoria));
			PintaPantallaPorTerminal((tipo_pantalla*)(&juego.arkanoPi.pantalla));
		} else {
			printf("\n");
			printf("You lose in the %d level, %d bricks remaining!", nivel, CalculaLadrillosRestantes((tipo_pantalla*)(&juego.arkanoPi.ladrillos.matriz)));
			printf("\n");
			//juego.arkanoPi.ladrillos = mensaje_derrota;
			PintaMensajeInicialPantalla((tipo_pantalla*)(&juego.arkanoPi.pantalla),(tipo_pantalla*)(&mensaje_derrota));
			PintaPantallaPorTerminal((tipo_pantalla*)(&juego.arkanoPi.pantalla));
		}
	piUnlock (STD_IO_BUFFER_KEY);

	tmr_destroy(tmr_pelota);
}
/**
* @brief rutina donde se reinicia el juego y los elementos vuelven a sus estados iniciales
* @param fsm
*/
void ReseteaJuegoFSM(fsm_t* fsm) {
	piLock (FLAGS_KEY);
	flags &= ~FLAG_TECLA;
	piUnlock (FLAGS_KEY);

	ReseteaMatriz((tipo_pantalla*)(&juego.arkanoPi.pantalla.matriz));

	ReseteaLadrillos((tipo_pantalla*)(&juego.arkanoPi.ladrillos.matriz));
	ReseteaPelota((tipo_pelota*)(&juego.arkanoPi.pelota));
	ReseteaRaqueta((tipo_raqueta*)(&juego.arkanoPi.raqueta));

	tmr_pelota = tmr_new(pelota_auto);
	cnt_rebotes = 1;
	v_pelota = TIMEOUT_PELOTA;
	vidas = 3;
	nivel = 1;

	piLock (STD_IO_BUFFER_KEY);
	ActualizaPantalla((tipo_arkanoPi*)(&juego.arkanoPi));
	PintaPantallaPorTerminal((tipo_pantalla*)(&juego.arkanoPi.pantalla));
	piUnlock (STD_IO_BUFFER_KEY);
}
/**
* @brief método asignado al temporizador que excita las columnas una a una y, en cada una, explora las flas de la matriz
*/
void excitaColumna(union sigval value){

	switch (columna) {

	case 0:
		digitalWrite(gpio_cols[0], 0);
		digitalWrite(gpio_cols[1], 0);
		digitalWrite(gpio_cols[2], 0);
		digitalWrite(gpio_cols[3], 0);
		break;

	case 1:
		digitalWrite(gpio_cols[0], 1);
		digitalWrite(gpio_cols[1], 0);
		digitalWrite(gpio_cols[2], 0);
		digitalWrite(gpio_cols[3], 0);
		break;

	case 2:
		digitalWrite(gpio_cols[0], 0);
		digitalWrite(gpio_cols[1], 1);
		digitalWrite(gpio_cols[2], 0);
		digitalWrite(gpio_cols[3], 0);
		break;

	case 3:
		digitalWrite(gpio_cols[0], 1);
		digitalWrite(gpio_cols[1], 1);
		digitalWrite(gpio_cols[2], 0);
		digitalWrite(gpio_cols[3], 0);
		break;

	case 4:
		digitalWrite(gpio_cols[0], 0);
		digitalWrite(gpio_cols[1], 0);
		digitalWrite(gpio_cols[2], 1);
		digitalWrite(gpio_cols[3], 0);
		break;

	case 5:
		digitalWrite(gpio_cols[0], 1);
		digitalWrite(gpio_cols[1], 0);
		digitalWrite(gpio_cols[2], 1);
		digitalWrite(gpio_cols[3], 0);
		break;

	case 6:
		digitalWrite(gpio_cols[0], 0);
		digitalWrite(gpio_cols[1], 1);
		digitalWrite(gpio_cols[2], 1);
		digitalWrite(gpio_cols[3], 0);
		break;

	case 7:
		digitalWrite(gpio_cols[0], 1);
		digitalWrite(gpio_cols[1], 1);
		digitalWrite(gpio_cols[2], 1);
		digitalWrite(gpio_cols[3], 0);
		break;

	case 8:
		digitalWrite(gpio_cols[0], 0);
		digitalWrite(gpio_cols[1], 0);
		digitalWrite(gpio_cols[2], 0);
		digitalWrite(gpio_cols[3], 1);
		break;

	case 9:
		digitalWrite(gpio_cols[0], 1);
		digitalWrite(gpio_cols[1], 0);
		digitalWrite(gpio_cols[2], 0);
		digitalWrite(gpio_cols[3], 1);
		break;
	}

	digitalWrite(GPIO_FILA_0, !juego.arkanoPi.pantalla.matriz[columna][0]);
	digitalWrite(GPIO_FILA_1, !juego.arkanoPi.pantalla.matriz[columna][1]);
	digitalWrite(GPIO_FILA_2, !juego.arkanoPi.pantalla.matriz[columna][2]);
	digitalWrite(GPIO_FILA_3, !juego.arkanoPi.pantalla.matriz[columna][3]);
	digitalWrite(GPIO_FILA_4, !juego.arkanoPi.pantalla.matriz[columna][4]);
	digitalWrite(GPIO_FILA_5, !juego.arkanoPi.pantalla.matriz[columna][5]);
	digitalWrite(GPIO_FILA_6, !juego.arkanoPi.pantalla.matriz[columna][6]);

	columna++;
	if (columna == 10) {
		columna = 0;
	}
	tmr_startms(tmr, TIMEOUT);
}

//------------------------------------------------------
// FUNCIONES DE INICIALIZACION
//------------------------------------------------------

/**
* @brief procedimiento de configuracion del sistema:
* configurar el uso de posibles librerías
* configurar las interrupciones externas asociadas a los pines GPIO
* configurar las interrupciones periódicas y sus correspondientes temporizadores
* crear, si fuese necesario, los threads adicionales que pueda requerir el sistema
* @return1, si el resultado es satisfactorio; -1, en caso contrario
*/
int systemSetup (void) {
	// A completar por el alumno...
	//int x = 0;

		piLock (STD_IO_BUFFER_KEY);

		// sets up the wiringPi library
		if (wiringPiSetupGpio () < 0) {
			printf ("Unable to setup wiringPi\n");
			piUnlock (STD_IO_BUFFER_KEY);
			return -1;
	    }

		// Lanzamos thread para exploracion del teclado convencional del PC
		/*x = piThreadCreate (thread_explora_teclado);

		if (x != 0) {
			printf ("it didn't start!!!\n");
			piUnlock (STD_IO_BUFFER_KEY);
			return -1;
	    }*/

		piUnlock (STD_IO_BUFFER_KEY);

		return 1;
}
/**
* @brief función donde se inicia la máquina de estados, además se incluye un mensaje por pantalla
* @param inicio_fsm
*/
void fsm_setup(fsm_t* inicio_fsm) {
	piLock (FLAGS_KEY);
	flags = 0;
	piUnlock (FLAGS_KEY);

	piLock (STD_IO_BUFFER_KEY);
	printf("\nINICIO DEL JUEGO\n");
	PintaMensajeInicialPantalla((tipo_pantalla*)(&juego.arkanoPi.pantalla),(tipo_pantalla*)(&mensaje_inicial));
	PintaPantallaPorTerminal((tipo_pantalla*)(&juego.arkanoPi.pantalla));
	piUnlock (STD_IO_BUFFER_KEY);
}

/*PI_THREAD (thread_explora_teclado) {
	int teclaPulsada;

	while(1) {
		delay(10); // Wiring Pi function: pauses program execution for at least 10 ms

		piLock (STD_IO_BUFFER_KEY);

		if(kbhit()) {
			teclaPulsada = kbread();
			//printf("Tecla %c\n", teclaPulsada);

			switch(teclaPulsada) {
				case 'p':
					piLock (FLAGS_KEY);
					flags |= FLAG_PELOTA;
					piUnlock (FLAGS_KEY);
					break;

				case 'i':
					piLock (FLAGS_KEY);
					flags |= FLAG_RAQUETA_IZQUIERDA;
					piUnlock (FLAGS_KEY);
					break;

				case 'o':
					piLock (FLAGS_KEY);
					flags |= FLAG_RAQUETA_DERECHA;
					piUnlock (FLAGS_KEY);
					break;

				case 'q':
					exit(0);
					break;

				case 'm':
					piLock (FLAGS_KEY);
					flags |= (FLAG_RAQUETA_DERECHA || FLAG_RAQUETA_IZQUIERDA);
					piUnlock (FLAGS_KEY);
					break;

				default:
					piLock (FLAGS_KEY);
					flags |= FLAG_TECLA;
					piUnlock (FLAGS_KEY);
					break;
			}
		}

		piUnlock (STD_IO_BUFFER_KEY);
	}
}*/
/**
* @brief función donde se ejecuta el flujo principal del programa
*/
int main (){

		// Maquina de estados: lista de transiciones
		// {EstadoOrigen,FunciónDeEntrada,EstadoDestino,FunciónDeSalida}
		fsm_trans_t fsm_tabla[] = {
				{ WAIT_START, compruebaTeclaPulsada,  WAIT_PUSH, InicializaJuegoFSM },
				//{ WAIT_PUSH, compruebaTeclaPelota, WAIT_PUSH,   MovimientoPelotaFSM },
				{ WAIT_PUSH, compruebaPausePlay, WAIT_PAUSE,   PauseIt },
				{ WAIT_PUSH, compruebaTeclaRaquetaDerecha, WAIT_PUSH,   MueveRaquetaDerechaFSM },
				{ WAIT_PUSH, compruebaTeclaRaquetaIzquierda, WAIT_PUSH,   MueveRaquetaIzquierdaFSM },
				{ WAIT_PAUSE, compruebaPausePlay, WAIT_PUSH,   PlayIt },
				{ WAIT_PUSH, compruebaFinalJuego, WAIT_END,   FinalJuegoFSM },
				{ WAIT_END, compruebaTeclaPulsada, WAIT_START,   ReseteaJuegoFSM },
				{ -1, NULL, -1, NULL },
		};
		

		/*Inicialización del Hardware*/
		wiringPiSetupGpio();

		/* Asociación de entradas del Hardware */
		pinMode(GPIO_BOTON_DER,INPUT);
		pinMode(GPIO_BOTON_IZQ,INPUT);
		pullUpDnControl(GPIO_BOTON_IZQ, 0);
		pullUpDnControl(GPIO_BOTON_DER, 0);

		wiringPiISR(GPIO_BOTON_DER, INT_EDGE_FALLING, boton_derecho);
		wiringPiISR(GPIO_BOTON_IZQ, INT_EDGE_FALLING, boton_izquierdo);

		pinMode(GPIO_BOTON_TECLA,INPUT);
		pullUpDnControl(GPIO_BOTON_TECLA, 0);

		wiringPiISR(GPIO_BOTON_TECLA, INT_EDGE_FALLING, boton_tecla);

		/*Local variable*/
		unsigned int next;

		pinMode(GPIO_FILA_0,OUTPUT);
		pinMode(GPIO_FILA_1,OUTPUT);
		pinMode(GPIO_FILA_2,OUTPUT);
		pinMode(GPIO_FILA_3,OUTPUT);
		pinMode(GPIO_FILA_4,OUTPUT);
		pinMode(GPIO_FILA_5,OUTPUT);
		pinMode(GPIO_FILA_6,OUTPUT);

		pinMode(GPIO_COL_0,OUTPUT);
		pinMode(GPIO_COL_1,OUTPUT);
		pinMode(GPIO_COL_2,OUTPUT);
		pinMode(GPIO_COL_3,OUTPUT);
		
		pinMode(GPIO_PWM,OUTPUT);
		softPwmCreate(GPIO_PWM, 0, 100);

		fsm_t* inicio_fsm = fsm_new (WAIT_START, fsm_tabla, NULL);

		// Configuracion e inicializacion del sistema
		systemSetup();
		fsm_setup (inicio_fsm);

		/* Creación y activación del timer------- */
		tmr = tmr_new(excitaColumna);
		tmr_startms(tmr, TIMEOUT);
		tmr_pelota = tmr_new(pelota_auto);
		//tmr_startms(tmr_pelota, TIMEOUT_PELOTA);

		next = millis();
		while (1) {
			fsm_fire (inicio_fsm);
			next += CLK_MS;
			delay_until (next);
		}

	    /*Destrucción de la máquina de estados*/
		fsm_destroy (inicio_fsm);

		/*Destrucción del timer*/
		tmr_destroy(tmr);
		tmr_destroy(tmr_pelota);
}
