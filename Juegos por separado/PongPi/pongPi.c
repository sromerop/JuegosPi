#include "pongPi.h"

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
* @brief función donde se comprueba la interrupción de la pulsación del boton_1_up
* @param this
* @return devuelve 1 si la iterrupción ha sido causada por esta pulsación; 0 en caso contrario
*/
int compruebaTeclaRaqueta1Up (fsm_t* this) {
	int result;

	piLock (FLAGS_KEY);
	result = (flags & FLAG_RAQUETA_1_UP);
	piUnlock (FLAGS_KEY);

	return result;
}
/**
* @brief función donde se comprueba la interrupción de la pulsación del boton_1_down
* @param this
* @return devuelve 1 si la iterrupción ha sido causada por esta pulsación; 0 en caso contrario
*/
int compruebaTeclaRaqueta1Down (fsm_t* this) {
	int result;

	piLock (FLAGS_KEY);
	result = (flags & FLAG_RAQUETA_1_DOWN);
	piUnlock (FLAGS_KEY);

	return result;
}
/**
* @brief función donde se comprueba la interrupción de la pulsación del boton_2_up
* @param this
* @return devuelve 1 si la iterrupción ha sido causada por esta pulsación; 0 en caso contrario
*/
int compruebaTeclaRaqueta2Up (fsm_t* this) {
	int result;

	piLock (FLAGS_KEY);
	result = (flags & FLAG_RAQUETA_2_UP);
	piUnlock (FLAGS_KEY);

	return result;
}
/**
* @brief función donde se comprueba la interrupción de la pulsación del boton_2_down
* @param this
* @return devuelve 1 si la iterrupción ha sido causada por esta pulsación; 0 en caso contrario
*/
int compruebaTeclaRaqueta2Down (fsm_t* this) {
	int result;

	piLock (FLAGS_KEY);
	result = (flags & FLAG_RAQUETA_2_DOWN);
	piUnlock (FLAGS_KEY);

	return result;
}
/**
* @brief función donde se comprueba la interrupción de la pulsación simultánea del boton_1_up y boton_1_down
* @param this
* @return devuelve 1 si la iterrupción ha sido causada por estas pulsaciones; 0 en caso contrario
*/
int compruebaPausePlay1 (fsm_t* this) {
	int result;

	piLock (FLAGS_KEY);
	result = (flags & (FLAG_RAQUETA_1_UP || FLAG_RAQUETA_1_DOWN));
	piUnlock (FLAGS_KEY);

	return result;
}
/**
* @brief función donde se comprueba la interrupción de la pulsación simultánea del boton_2_up y boton_2_down
* @param this
* @return devuelve 1 si la iterrupción ha sido causada por estas pulsaciones; 0 en caso contrario
*/
int compruebaPausePlay2 (fsm_t* this) {
	int result;

	piLock (FLAGS_KEY);
	result = (flags & (FLAG_RAQUETA_2_UP || FLAG_RAQUETA_2_DOWN));
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
* @brief rutina donde se inicia el juego
* @param fsm
*/
void InicializaJuegoFSM(fsm_t* fsm) {
	piLock (FLAGS_KEY);
	flags &= ~FLAG_TECLA;
	piUnlock (FLAGS_KEY);

	InicializaPongPi((tipo_pongPi*)(&juego.pongPi));
	tmr_startms(tmr_pelota, TIMEOUT_PELOTA);

	piLock (STD_IO_BUFFER_KEY);
	ActualizaPantalla((tipo_pongPi*)(&juego.pongPi));
	PintaPantallaPorTerminal((tipo_pantalla*)(&juego.pongPi.pantalla));
	piUnlock (STD_IO_BUFFER_KEY);

}
/**
* @brief rutina donde se modifica la pelota en una posición siguiendo su trayectoria, permitiendo que uno de los dos jugadores gane
* @param fsm
*/
void MovimientoPelotaFSM(fsm_t* fsm) {
	//piLock (FLAGS_KEY);
	//flags &= ~FLAG_PELOTA;
	//piUnlock (FLAGS_KEY);

		int pelota_x = juego.pongPi.pelota.x;
		int pelota_y = juego.pongPi.pelota.y;
		int pelota_xv = juego.pongPi.pelota.xv;
		int pelota_yv = juego.pongPi.pelota.yv;
		int pelota_sig_x = pelota_x + pelota_xv;
		int pelota_sig_y = pelota_y + pelota_yv;

		int raqueta1_x = juego.pongPi.raqueta1.x;
		int raqueta1_y = juego.pongPi.raqueta1.y;
		int raqueta2_x = juego.pongPi.raqueta2.x;
		int raqueta2_y = juego.pongPi.raqueta2.y;

		softPwmWrite(GPIO_PWM, 0);

		if (pelota_x == 1 || pelota_x == 8 || pelota_y == 0 || pelota_y == 6){
			if (pelota_y == 0){
				if (pelota_yv < 0){
					juego.pongPi.pelota.yv = 1;
					pelota_yv = juego.pongPi.pelota.yv;
					pelota_sig_y = pelota_y + pelota_yv;
					cnt_rebotes++;
					softPwmWrite(GPIO_PWM, 75);
				}
			}
			if (pelota_y == 6){
				if (pelota_yv > 0){
					juego.pongPi.pelota.yv = -1;
					pelota_yv = juego.pongPi.pelota.yv;
					pelota_sig_y = pelota_y + pelota_yv;
					cnt_rebotes++;
					softPwmWrite(GPIO_PWM, 75);
				}
			}
			if (pelota_x == 1){
				if (pelota_xv < 0){
					if ((pelota_sig_y == (raqueta1_y)) && (pelota_sig_x == raqueta1_x)) { // arriba
						if (pelota_y == 0){
							juego.pongPi.pelota.xv = 1;
							juego.pongPi.pelota.yv = 1;
							pelota_xv = juego.pongPi.pelota.xv;
							pelota_yv = juego.pongPi.pelota.yv;
							pelota_sig_x = pelota_x + pelota_xv;
							pelota_sig_y = pelota_y + pelota_yv;
						}else{
							juego.pongPi.pelota.xv = 1;
							juego.pongPi.pelota.yv = -1;
							pelota_xv = juego.pongPi.pelota.xv;
							pelota_yv = juego.pongPi.pelota.yv;
							pelota_sig_x = pelota_x + pelota_xv;
							pelota_sig_y = pelota_y + pelota_yv;
						}
						cnt_rebotes++;
						softPwmWrite(GPIO_PWM, 50);
					}else if ((pelota_sig_y == (raqueta1_y + 1)) && (pelota_sig_x == raqueta1_x)) { // centro
						juego.pongPi.pelota.xv = 1;
						juego.pongPi.pelota.yv = 0;
						pelota_xv = juego.pongPi.pelota.xv;
						pelota_yv = juego.pongPi.pelota.yv;
						pelota_sig_x = pelota_x + pelota_xv;
						pelota_sig_y = pelota_y + pelota_yv;
						cnt_rebotes++;
						softPwmWrite(GPIO_PWM, 50);
					}else if ((pelota_sig_y == (raqueta1_y + 2)) && (pelota_sig_x == raqueta1_x)) { // abajo
						if (pelota_y == 6){
							juego.pongPi.pelota.xv = 1;
							juego.pongPi.pelota.yv = -1;
							pelota_xv = juego.pongPi.pelota.xv;
							pelota_yv = juego.pongPi.pelota.yv;
							pelota_sig_x = pelota_x + pelota_xv;
							pelota_sig_y = pelota_y + pelota_yv;
						}else{
							juego.pongPi.pelota.xv = 1;
							juego.pongPi.pelota.yv = 1;
							pelota_xv = juego.pongPi.pelota.xv;
							pelota_yv = juego.pongPi.pelota.yv;
							pelota_sig_x = pelota_x + pelota_xv;
							pelota_sig_y = pelota_y + pelota_yv;
						}
						cnt_rebotes++;
						softPwmWrite(GPIO_PWM, 50);
					}else{
						juego.pongPi.pelota.x = 0;

						piLock (FLAGS_KEY);
						flags |= FLAG_FINAL_JUEGO;
						piUnlock (FLAGS_KEY);
					}
				}
			}
			if (pelota_x == 8){
				if (pelota_xv > 0){
					if ((pelota_sig_y == (raqueta2_y)) && (pelota_sig_x == raqueta2_x)) { // arriba
						if (pelota_y == 0){
							juego.pongPi.pelota.xv = -1;
							juego.pongPi.pelota.yv = 1;
							pelota_xv = juego.pongPi.pelota.xv;
							pelota_yv = juego.pongPi.pelota.yv;
							pelota_sig_x = pelota_x + pelota_xv;
							pelota_sig_y = pelota_y + pelota_yv;
						}else{
							juego.pongPi.pelota.xv = -1;
							juego.pongPi.pelota.yv = -1;
							pelota_xv = juego.pongPi.pelota.xv;
							pelota_yv = juego.pongPi.pelota.yv;
							pelota_sig_x = pelota_x + pelota_xv;
							pelota_sig_y = pelota_y + pelota_yv;
						}
						cnt_rebotes++;
						softPwmWrite(GPIO_PWM, 50);
					}else if ((pelota_sig_y == (raqueta2_y + 1)) && (pelota_sig_x == raqueta2_x)) { // centro
							juego.pongPi.pelota.xv = -1;
							juego.pongPi.pelota.yv = 0;
							pelota_xv = juego.pongPi.pelota.xv;
							pelota_yv = juego.pongPi.pelota.yv;
							pelota_sig_x = pelota_x + pelota_xv;
							pelota_sig_y = pelota_y + pelota_yv;
							cnt_rebotes++;
							softPwmWrite(GPIO_PWM, 50);
					}else if ((pelota_sig_y == (raqueta2_y + 2)) && (pelota_sig_x == raqueta2_x)) { // abajo
						if (pelota_y == 6){
							juego.pongPi.pelota.xv = -1;
							juego.pongPi.pelota.yv = -1;
							pelota_xv = juego.pongPi.pelota.xv;
							pelota_yv = juego.pongPi.pelota.yv;
							pelota_sig_x = pelota_x + pelota_xv;
							pelota_sig_y = pelota_y + pelota_yv;
						}else{
							juego.pongPi.pelota.xv = -1;
							juego.pongPi.pelota.yv = 1;
							pelota_xv = juego.pongPi.pelota.xv;
							pelota_yv = juego.pongPi.pelota.yv;
							pelota_sig_x = pelota_x + pelota_xv;
							pelota_sig_y = pelota_y + pelota_yv;
						}
						cnt_rebotes++;
						softPwmWrite(GPIO_PWM, 50);
					}else{
						juego.pongPi.pelota.x = 9;

						piLock (FLAGS_KEY);
						flags |= FLAG_FINAL_JUEGO;
						piUnlock (FLAGS_KEY);
					}
				}
			}
		}

		if (juego.pongPi.pelota.x != 0 && juego.pongPi.pelota.x != 9){
		juego.pongPi.pelota.x += juego.pongPi.pelota.xv;
		juego.pongPi.pelota.y += juego.pongPi.pelota.yv;
		}
	piLock (STD_IO_BUFFER_KEY);
		ActualizaPantalla((tipo_pongPi*)(&juego.pongPi));
		PintaPantallaPorTerminal((tipo_pantalla*)(&juego.pongPi.pantalla));
	piUnlock (STD_IO_BUFFER_KEY);
}
/**
* @brief método asignado al temporizador que hace mover la pelota automáticamente
*/
void pelota_auto(union sigval value) {
	if ((cnt_rebotes % 5) == 0) {
		v_pelota -= 10;
		if (v_pelota <= 50){
			v_pelota = 50;
		}
	}
	MovimientoPelotaFSM(inicio_fsm);
	tmr_startms(tmr_pelota, v_pelota);
}
/**
* @brief rutina donde se modifica la raqueta1 en una posición hacia arriba
* @param fsm
*/
void MueveRaqueta1UpFSM(fsm_t *fsm) {
	piLock (FLAGS_KEY);
	flags &= ~FLAG_RAQUETA_1_UP;
	piUnlock (FLAGS_KEY);

	if(juego.pongPi.raqueta1.y > -1){
		juego.pongPi.raqueta1.y = juego.pongPi.raqueta1.y-1;
	} else {
		juego.pongPi.raqueta1.y = -1;
	}
	piLock (STD_IO_BUFFER_KEY);
	ActualizaPantalla((tipo_pongPi*)(&juego.pongPi));
	PintaPantallaPorTerminal((tipo_pantalla*)(&juego.pongPi.pantalla));
	piUnlock (STD_IO_BUFFER_KEY);
}
/**
* @brief método asignado a la inturrupción por la pulsación del boton asignado
*/
void boton_1_up(void) {
	// Pin event (key / button event) debouncing procedure
		if (millis () < debounceTime)
		{
			debounceTime = millis () + DEBOUNCE_TIME ;
			return;
		}

		piLock (FLAGS_KEY);
		flags |= FLAG_RAQUETA_1_UP;
		piUnlock (FLAGS_KEY);

			// Wait for key to be released
		while (digitalRead (GPIO_BOTON_1_UP) == HIGH) {
			delay (1);
			if (digitalRead (GPIO_BOTON_1_DOWN) == HIGH) {
				piLock (FLAGS_KEY);
				flags |= (FLAG_RAQUETA_1_UP || FLAG_RAQUETA_1_DOWN);
				piUnlock (FLAGS_KEY);
			}
		}

		debounceTime = millis () + DEBOUNCE_TIME ;
}
/**
* @brief rutina donde se modifica la raqueta1 en una posición hacia abajo
* @param fsm
*/
void MueveRaqueta1DownFSM(fsm_t *fsm) {
	piLock (FLAGS_KEY);
	flags &= ~FLAG_RAQUETA_1_DOWN;
	piUnlock (FLAGS_KEY);

	if(juego.pongPi.raqueta1.y < (MATRIZ_ALTO - RAQUETA_ANCHO)+1){
		juego.pongPi.raqueta1.y = juego.pongPi.raqueta1.y+1;
	} else {
		juego.pongPi.raqueta1.y = (MATRIZ_ALTO - RAQUETA_ANCHO)+1;
	}
	piLock (STD_IO_BUFFER_KEY);
	ActualizaPantalla((tipo_pongPi*)(&juego.pongPi));
	PintaPantallaPorTerminal((tipo_pantalla*)(&juego.pongPi.pantalla));
	piUnlock (STD_IO_BUFFER_KEY);
}
/**
* @brief método asignado a la inturrupción por la pulsación del boton asignado
*/
void boton_1_down(void) {
	// Pin event (key / button event) debouncing procedure
		if (millis () < debounceTime)
		{
			debounceTime = millis () + DEBOUNCE_TIME ;
			return;
		}

		piLock (FLAGS_KEY);
		flags |= FLAG_RAQUETA_1_DOWN;
		piUnlock (FLAGS_KEY);

			// Wait for key to be released
		while (digitalRead (GPIO_BOTON_1_DOWN) == HIGH) {
			delay (1);
			if (digitalRead (GPIO_BOTON_1_UP) == HIGH) {
				piLock (FLAGS_KEY);
				flags |= (FLAG_RAQUETA_1_UP || FLAG_RAQUETA_1_DOWN);
				piUnlock (FLAGS_KEY);
			}
		}

		debounceTime = millis () + DEBOUNCE_TIME ;
}
/**
* @brief rutina donde se modifica la raqueta2 en una posición hacia arriba
* @param fsm
*/
void MueveRaqueta2UpFSM(fsm_t *fsm) {
	piLock (FLAGS_KEY);
	flags &= ~FLAG_RAQUETA_2_UP;
	piUnlock (FLAGS_KEY);

	if(juego.pongPi.raqueta2.y > -1){
		juego.pongPi.raqueta2.y = juego.pongPi.raqueta2.y-1;
	} else {
		juego.pongPi.raqueta2.y = -1;
	}
	piLock (STD_IO_BUFFER_KEY);
	ActualizaPantalla((tipo_pongPi*)(&juego.pongPi));
	PintaPantallaPorTerminal((tipo_pantalla*)(&juego.pongPi.pantalla));
	piUnlock (STD_IO_BUFFER_KEY);
}
/**
* @brief método asignado a la inturrupción por la pulsación del boton asignado
*/
void boton_2_up(void) {
	// Pin event (key / button event) debouncing procedure
		if (millis () < debounceTime)
		{
			debounceTime = millis () + DEBOUNCE_TIME ;
			return;
		}

		piLock (FLAGS_KEY);
		flags |= FLAG_RAQUETA_2_UP;
		piUnlock (FLAGS_KEY);

			// Wait for key to be released
		while (digitalRead (GPIO_BOTON_2_UP) == HIGH) {
			delay (1);
			if (digitalRead (GPIO_BOTON_2_DOWN) == HIGH) {
				piLock (FLAGS_KEY);
				flags |= (FLAG_RAQUETA_2_UP || FLAG_RAQUETA_2_DOWN);
				piUnlock (FLAGS_KEY);
			}
		}

		debounceTime = millis () + DEBOUNCE_TIME ;
}
/**
* @brief rutina donde se modifica la raqueta2 en una posición hacia abajo
* @param fsm
*/
void MueveRaqueta2DownFSM(fsm_t *fsm) {
	piLock (FLAGS_KEY);
	flags &= ~FLAG_RAQUETA_2_DOWN;
	piUnlock (FLAGS_KEY);

	if(juego.pongPi.raqueta2.y < (MATRIZ_ALTO - RAQUETA_ANCHO)+1){
		juego.pongPi.raqueta2.y = juego.pongPi.raqueta2.y+1;
	} else {
		juego.pongPi.raqueta2.y = (MATRIZ_ALTO - RAQUETA_ANCHO)+1;
	}
	piLock (STD_IO_BUFFER_KEY);
	ActualizaPantalla((tipo_pongPi*)(&juego.pongPi));
	PintaPantallaPorTerminal((tipo_pantalla*)(&juego.pongPi.pantalla));
	piUnlock (STD_IO_BUFFER_KEY);
}
/**
* @brief método asignado a la inturrupción por la pulsación del boton asignado
*/
void boton_2_down(void) {
	// Pin event (key / button event) debouncing procedure
		if (millis () < debounceTime)
		{
			debounceTime = millis () + DEBOUNCE_TIME ;
			return;
		}

		piLock (FLAGS_KEY);
		flags |= FLAG_RAQUETA_2_DOWN;
		piUnlock (FLAGS_KEY);

			// Wait for key to be released
		while (digitalRead (GPIO_BOTON_2_DOWN) == HIGH) {
			delay (1);
			if (digitalRead (GPIO_BOTON_2_UP) == HIGH) {
				piLock (FLAGS_KEY);
				flags |= (FLAG_RAQUETA_2_UP || FLAG_RAQUETA_2_DOWN);
				piUnlock (FLAGS_KEY);
			}
		}

		debounceTime = millis () + DEBOUNCE_TIME ;
}
/**
* @brief rutina donde se detiene el juego por interación del jugador1
* @param fsm
*/
void PauseIt1 (fsm_t *fsm) {
	piLock (FLAGS_KEY);
	flags &= ~(FLAG_RAQUETA_1_UP || FLAG_RAQUETA_1_DOWN);
	piUnlock (FLAGS_KEY);

	piLock (STD_IO_BUFFER_KEY);
	printf("Pause Player 1.\n Press the buttons again to continue: ... ");
	ActualizaPantalla((tipo_pongPi*)(&juego.pongPi));
	PintaPantallaPorTerminal((tipo_pantalla*)(&juego.pongPi.pantalla));
	piUnlock (STD_IO_BUFFER_KEY);

	tmr_destroy(tmr_pelota);
}
/**
* @brief rutina donde se detiene el juego por interación del jugador2
* @param fsm
*/
void PauseIt2 (fsm_t *fsm) {
	piLock (FLAGS_KEY);
	flags &= ~(FLAG_RAQUETA_2_UP || FLAG_RAQUETA_2_DOWN);
	piUnlock (FLAGS_KEY);

	piLock (STD_IO_BUFFER_KEY);
	printf("Pause Player 2.\n Press the buttons again to continue: ... ");
	ActualizaPantalla((tipo_pongPi*)(&juego.pongPi));
	PintaPantallaPorTerminal((tipo_pantalla*)(&juego.pongPi.pantalla));
	piUnlock (STD_IO_BUFFER_KEY);

	tmr_destroy(tmr_pelota);
}
/**
* @brief rutina donde se reanuda el juego por interación del jugador1
* @param fsm
*/
void PlayIt1 (fsm_t *fsm) {
	piLock (FLAGS_KEY);
	flags &= ~(FLAG_RAQUETA_1_UP || FLAG_RAQUETA_1_DOWN);
	piUnlock (FLAGS_KEY);

	piLock (STD_IO_BUFFER_KEY);
	printf("Play Player 1.\n Continue... ");
	ActualizaPantalla((tipo_pongPi*)(&juego.pongPi));
	PintaPantallaPorTerminal((tipo_pantalla*)(&juego.pongPi.pantalla));
	piUnlock (STD_IO_BUFFER_KEY);

	tmr_pelota = tmr_new(pelota_auto);
	tmr_startms(tmr_pelota, v_pelota);
}
/**
* @brief rutina donde se reanuda el juego por interación del jugador2
* @param fsm
*/
void PlayIt2 (fsm_t *fsm) {
	piLock (FLAGS_KEY);
	flags &= ~(FLAG_RAQUETA_2_UP || FLAG_RAQUETA_2_DOWN);
	piUnlock (FLAGS_KEY);

	piLock (STD_IO_BUFFER_KEY);
	printf("Play Player 2.\n Continue... ");
	ActualizaPantalla((tipo_pongPi*)(&juego.pongPi));
	PintaPantallaPorTerminal((tipo_pantalla*)(&juego.pongPi.pantalla));
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
/**
* @brief rutina donde se finaliza el juego y se muestra un mensaje por pantalla dependiendo de si se ha ganado o se ha perdido
* @param fsm
*/
void FinalJuegoFSM(fsm_t* fsm) {
	piLock (FLAGS_KEY);
	flags &= ~FLAG_FINAL_JUEGO;
	piUnlock (FLAGS_KEY);

	piLock (STD_IO_BUFFER_KEY);
	if (juego.pongPi.pelota.x == 9) {
			printf("\n");
			printf("Player 1 wins!");
			printf("\n");
	} else if(juego.pongPi.pelota.x == 0) {
			printf("\n");
			printf("Player 2 wins!");
			printf("\n");
	}
	PintaMensajeInicialPantalla((tipo_pantalla*)(&juego.pongPi.pantalla),(tipo_pantalla*)(&mensaje_derrota));
	PintaPantallaPorTerminal((tipo_pantalla*)(&juego.pongPi.pantalla));
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

	ReseteaMatriz((tipo_pantalla*)(&juego.pongPi.pantalla.matriz));

	ReseteaPelota((tipo_pelota*)(&juego.pongPi.pelota));
	ReseteaRaquetas((tipo_raqueta*)(&juego.pongPi.raqueta1), (tipo_raqueta*)(&juego.pongPi.raqueta2));

	tmr_pelota = tmr_new(pelota_auto);
	cnt_rebotes = 1;
	v_pelota = TIMEOUT_PELOTA;

	piLock (STD_IO_BUFFER_KEY);
	ActualizaPantalla((tipo_pongPi*)(&juego.pongPi));
	PintaPantallaPorTerminal((tipo_pantalla*)(&juego.pongPi.pantalla));
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

	digitalWrite(GPIO_FILA_0, !juego.pongPi.pantalla.matriz[columna][0]);
	digitalWrite(GPIO_FILA_1, !juego.pongPi.pantalla.matriz[columna][1]);
	digitalWrite(GPIO_FILA_2, !juego.pongPi.pantalla.matriz[columna][2]);
	digitalWrite(GPIO_FILA_3, !juego.pongPi.pantalla.matriz[columna][3]);
	digitalWrite(GPIO_FILA_4, !juego.pongPi.pantalla.matriz[columna][4]);
	digitalWrite(GPIO_FILA_5, !juego.pongPi.pantalla.matriz[columna][5]);
	digitalWrite(GPIO_FILA_6, !juego.pongPi.pantalla.matriz[columna][6]);

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
	PintaMensajeInicialPantalla((tipo_pantalla*)(&juego.pongPi.pantalla),(tipo_pantalla*)(&mensaje_inicial));
	PintaPantallaPorTerminal((tipo_pantalla*)(&juego.pongPi.pantalla));
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
				case 'a':
					piLock (FLAGS_KEY);
					flags |= FLAG_RAQUETA_1_UP;
					piUnlock (FLAGS_KEY);
					break;

				case 's':
					piLock (FLAGS_KEY);
					flags |= FLAG_RAQUETA_1_DOWN;
					piUnlock (FLAGS_KEY);
					break;

				case 'o':
					piLock (FLAGS_KEY);
					flags |= FLAG_RAQUETA_2_UP;
					piUnlock (FLAGS_KEY);
					break;

				case 'p':
					piLock (FLAGS_KEY);
					flags |= FLAG_RAQUETA_2_DOWN;
					piUnlock (FLAGS_KEY);
					break;

				case 'q':
					exit(0);
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
				{ WAIT_PUSH, compruebaPausePlay1, WAIT_PAUSE_1,   PauseIt1 },
				{ WAIT_PUSH, compruebaPausePlay2, WAIT_PAUSE_2,   PauseIt2 },
				{ WAIT_PUSH, compruebaTeclaRaqueta1Up, WAIT_PUSH,   MueveRaqueta1UpFSM },
				{ WAIT_PUSH, compruebaTeclaRaqueta1Down, WAIT_PUSH,   MueveRaqueta1DownFSM },
				{ WAIT_PUSH, compruebaTeclaRaqueta2Up, WAIT_PUSH,   MueveRaqueta2UpFSM },
				{ WAIT_PUSH, compruebaTeclaRaqueta2Down, WAIT_PUSH,   MueveRaqueta2DownFSM },
				{ WAIT_PAUSE_1, compruebaPausePlay1, WAIT_PUSH,   PlayIt1 },
				{ WAIT_PAUSE_2, compruebaPausePlay2, WAIT_PUSH,   PlayIt2 },
				{ WAIT_PUSH, compruebaFinalJuego, WAIT_END,   FinalJuegoFSM },
				{ WAIT_END, compruebaTeclaPulsada, WAIT_START,   ReseteaJuegoFSM },
				{ -1, NULL, -1, NULL },
		};
		

		/*Inicializaci�n del Hardware*/
		wiringPiSetupGpio();

		/* Asociaci�n de entradas del Hardware */
		pinMode(GPIO_BOTON_1_UP,INPUT);
		pinMode(GPIO_BOTON_1_DOWN,INPUT);
		pinMode(GPIO_BOTON_2_UP,INPUT);
		pinMode(GPIO_BOTON_2_DOWN,INPUT);
		pullUpDnControl(GPIO_BOTON_1_UP, 0);
		pullUpDnControl(GPIO_BOTON_1_DOWN, 0);
		pullUpDnControl(GPIO_BOTON_2_UP, 0);
		pullUpDnControl(GPIO_BOTON_2_DOWN, 0);

		wiringPiISR(GPIO_BOTON_1_UP, INT_EDGE_FALLING, boton_1_up);
		wiringPiISR(GPIO_BOTON_1_DOWN, INT_EDGE_FALLING, boton_1_down);
		wiringPiISR(GPIO_BOTON_2_UP, INT_EDGE_FALLING, boton_2_up);
		wiringPiISR(GPIO_BOTON_2_DOWN, INT_EDGE_FALLING, boton_2_down);

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

		/* Creaci�n y activaci�n del timer------- */
		tmr = tmr_new(excitaColumna);
		tmr_startms(tmr, TIMEOUT);
		tmr_pelota = tmr_new(pelota_auto);

		next = millis();
		while (1) {
			fsm_fire (inicio_fsm);
			next += CLK_MS;
			delay_until (next);
		}

	    /*Destrucci�n de la m�quina de estados*/
		fsm_destroy (inicio_fsm);

		/*Destrucci�n del timer*/
		tmr_destroy(tmr);
		tmr_destroy(tmr_pelota);
}
