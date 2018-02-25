#include "juegosPi.h"

static volatile tipo_juego juego;
volatile int flags = 0;
tmr_t* tmr;
static int gpio_cols[4]={GPIO_COL_0,GPIO_COL_1,GPIO_COL_2,GPIO_COL_3};  // array de columnas
unsigned volatile int columna = 0;
int debounceTime;
fsm_t* inicio_fsm;
tipo_pantalla mensaje_inicial;
tipo_pantalla mensaje_victoria;
tipo_pantalla mensaje_derrota;
tipo_pantalla ladrillos_nv2;
tipo_pantalla ladrillos_nv3;

tmr_t* tmr_pelota;
int cnt_rebotes = 1;
int v_pelota = TIMEOUT_PELOTA;
int vidas = 3;
int nivel = 1;

tmr_t* tmr_pelota_pong;
int cnt_rebotes_pong = 1;
int v_pelota_pong = TIMEOUT_PELOTA_PONG;

tmr_t* tmr_serpiente;
int cnt_frutas = 1;
int v_serpiente = TIMEOUT_SERPIENTE;

//------------------------------------------------------
// FUNCIONES DE ACCION
//------------------------------------------------------

// espera hasta la prÃ³xima activaciÃ³n del reloj
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
* @brief función donde se comprueba la interrupción de la pulsación del boton_up
* @param this
* @return devuelve 1 si la iterrupción ha sido causada por esta pulsación; 0 en caso contrario
*/
int compruebaTeclaRaqueta2Up (fsm_t* this) {
	int result;

	piLock (FLAGS_KEY);
	result = (flags & FLAG_RAQUETA_UP);
	piUnlock (FLAGS_KEY);

	return result;
}
/**
* @brief función donde se comprueba la interrupción de la pulsación del boton_down
* @param this
* @return devuelve 1 si la iterrupción ha sido causada por esta pulsación; 0 en caso contrario
*/
int compruebaTeclaRaqueta2Down (fsm_t* this) {
	int result;

	piLock (FLAGS_KEY);
	result = (flags & FLAG_RAQUETA_DOWN);
	piUnlock (FLAGS_KEY);

	return result;
}
/**
* @brief función donde se comprueba la interrupción de la pulsación simultánea del boton_up y boton_down
* @param this
* @return devuelve 1 si la iterrupción ha sido causada por estas pulsaciones; 0 en caso contrario
*/
int compruebaPausePlay2 (fsm_t* this) {
	int result;

	piLock (FLAGS_KEY);
	result = (flags & (FLAG_RAQUETA_UP || FLAG_RAQUETA_DOWN));
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
//------------------------------------------------------
// FUNCIONES DE SALIDA ARKANO
//------------------------------------------------------
/**
* @brief rutina donde se inicia el juego Arkano, sacando por pantalla y dibujando ladrillos_basico
* @param fsm
*/
void InicializaJuegoArkanoFSM(fsm_t* fsm) {
	piLock (FLAGS_KEY);
	//flags &= ~FLAG_TECLA;
	flags &= ~FLAG_RAQUETA_DERECHA;
	piUnlock (FLAGS_KEY);

	InicializaArkanoPi((tipo_arkanoPi*)(&juego.arkanoPi));
	juego.pantalla = juego.arkanoPi.pantalla;
	//tmr_pelota = tmr_new(pelota_auto);
	tmr_startms(tmr_pelota, TIMEOUT_PELOTA);

	cnt_rebotes = 1;
	v_pelota = TIMEOUT_PELOTA;
	vidas = 3;
	nivel = 1;

	piLock (STD_IO_BUFFER_KEY);
	ActualizaPantallaArkano((tipo_arkanoPi*)(&juego.arkanoPi));
	PintaPantallaPorTerminal((tipo_pantalla*)(&juego.pantalla));
	piUnlock (STD_IO_BUFFER_KEY);
}
/**
* @brief rutina donde se modifica la pelota en una posición siguiendo su trayectoria, pudiendo avanzar de nivel, ganar o perder el juego.
* @param fsm
*/
void MovimientoPelotaFSM(fsm_t* fsm) {
	/*piLock (FLAGS_KEY);
	flags &= ~FLAG_PELOTA;
	piUnlock (FLAGS_KEY);*/

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
						vidas--;
						if (vidas == 0) {
							/*piLock (STD_IO_BUFFER_KEY);
							printf("\n");
							printf("You lose in the %d level, %d bricks remaining!", nivel, CalculaLadrillosRestantes((tipo_pantalla*)(&juego.arkanoPi.ladrillos.matriz)));
							printf("\n");
							juego.arkanoPi.pantalla = mensaje_derrota;
							PintaMensajeInicialPantalla((tipo_pantalla*)(&juego.pantalla),(tipo_pantalla*)(&mensaje_derrota));
							PintaPantallaPorTerminal((tipo_pantalla*)(&juego.pantalla));
							piUnlock (STD_IO_BUFFER_KEY);*/

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
			if (CalculaLadrillosRestantes((tipo_pantalla*)(&juego.arkanoPi.ladrillos.matriz)) == 0) { // no quedan más ladrillos
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
						softPwmWrite(GPIO_PWM, 0);
						
						piLock (FLAGS_KEY);
						flags |= FLAG_FINAL_JUEGO;
						piUnlock (FLAGS_KEY);

						/*piLock (STD_IO_BUFFER_KEY);
						printf("\n");
						printf("You win!");
						printf("\n");
						juego.arkanoPi.pantalla = mensaje_victoria;
						PintaMensajeInicialPantalla((tipo_pantalla*)(&juego.pantalla),(tipo_pantalla*)(&mensaje_victoria));
						PintaPantallaPorTerminal((tipo_pantalla*)(&juego.pantalla));
						piUnlock (STD_IO_BUFFER_KEY);*/
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
	ActualizaPantallaArkano((tipo_arkanoPi*)(&juego.arkanoPi));
	juego.pantalla = juego.arkanoPi.pantalla;
	PintaPantallaPorTerminal((tipo_pantalla*)(&juego.pantalla));
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
	ActualizaPantallaArkano((tipo_arkanoPi*)(&juego.arkanoPi));
	juego.pantalla = juego.arkanoPi.pantalla;
	PintaPantallaPorTerminal((tipo_pantalla*)(&juego.pantalla));
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
	ActualizaPantallaArkano((tipo_arkanoPi*)(&juego.arkanoPi));
	juego.pantalla = juego.arkanoPi.pantalla;
	PintaPantallaPorTerminal((tipo_pantalla*)(&juego.pantalla));
	piUnlock (STD_IO_BUFFER_KEY);

	tmr_pelota = tmr_new(pelota_auto);
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
	ActualizaPantallaArkano((tipo_arkanoPi*)(&juego.arkanoPi));
	juego.pantalla = juego.arkanoPi.pantalla;
	PintaPantallaPorTerminal((tipo_pantalla*)(&juego.pantalla));
	piUnlock (STD_IO_BUFFER_KEY);
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
	ActualizaPantallaArkano((tipo_arkanoPi*)(&juego.arkanoPi));
	juego.pantalla = juego.arkanoPi.pantalla;
	PintaPantallaPorTerminal((tipo_pantalla*)(&juego.pantalla));
	piUnlock (STD_IO_BUFFER_KEY);
}

//------------------------------------------------------
// FUNCIONES DE SALIDA PONG
//------------------------------------------------------
/**
* @brief rutina donde se inicia el juego Pong
* @param fsm
*/
void InicializaJuegoPongFSM(fsm_t* fsm) {
	piLock (FLAGS_KEY);
	//flags &= ~FLAG_TECLA;
	flags &= ~FLAG_RAQUETA_IZQUIERDA;
	piUnlock (FLAGS_KEY);

	InicializaPongPi((tipo_pongPi*)(&juego.pongPi));
	juego.pantalla = juego.pongPi.pantalla;
	//tmr_pelota_pong = tmr_new(pelota_auto_pong);
	tmr_startms(tmr_pelota_pong, TIMEOUT_PELOTA_PONG);

	cnt_rebotes_pong = 1;
	v_pelota_pong = TIMEOUT_PELOTA_PONG;

	piLock (STD_IO_BUFFER_KEY);
	ActualizaPantallaPong((tipo_pongPi*)(&juego.pongPi));
	PintaPantallaPorTerminal((tipo_pantalla*)(&juego.pantalla));
	piUnlock (STD_IO_BUFFER_KEY);
}
/**
* @brief rutina donde se modifica la pelota en una posición siguiendo su trayectoria, permitiendo que uno de los dos jugadores gane
* @param fsm
*/
void MovimientoPelotaPongFSM(fsm_t* fsm) {
	/*piLock (FLAGS_KEY);
	flags &= ~FLAG_PELOTA;
	piUnlock (FLAGS_KEY);*/

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
					cnt_rebotes_pong++;
					softPwmWrite(GPIO_PWM, 75);
				}
			}
			if (pelota_y == 6){
				if (pelota_yv > 0){
					juego.pongPi.pelota.yv = -1;
					pelota_yv = juego.pongPi.pelota.yv;
					pelota_sig_y = pelota_y + pelota_yv;
					cnt_rebotes_pong++;
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
						cnt_rebotes_pong++;
						softPwmWrite(GPIO_PWM, 50);
					}else if ((pelota_sig_y == (raqueta1_y + 1)) && (pelota_sig_x == raqueta1_x)) { // centro
						juego.pongPi.pelota.xv = 1;
						juego.pongPi.pelota.yv = 0;
						pelota_xv = juego.pongPi.pelota.xv;
						pelota_yv = juego.pongPi.pelota.yv;
						pelota_sig_x = pelota_x + pelota_xv;
						pelota_sig_y = pelota_y + pelota_yv;
						cnt_rebotes_pong++;
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
						cnt_rebotes_pong++;
						softPwmWrite(GPIO_PWM, 50);
					}else{
						juego.pongPi.pelota.x = 0;

						piLock (FLAGS_KEY);
						flags |= FLAG_FINAL_JUEGO;
						piUnlock (FLAGS_KEY);

						/*piLock (STD_IO_BUFFER_KEY);
						printf("\n");
						printf("Player 2 wins!");
						printf("\n");
						//juego.pongPi.pantalla = mensaje_derrota;
						PintaMensajeInicialPantalla((tipo_pantalla*)(&juego.pantalla),(tipo_pantalla*)(&mensaje_derrota));
						//PintaPantallaPorTerminal((tipo_pantalla*)(&juego.pantalla));
						piUnlock (STD_IO_BUFFER_KEY);*/
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
						cnt_rebotes_pong++;
						softPwmWrite(GPIO_PWM, 50);
					}else if ((pelota_sig_y == (raqueta2_y + 1)) && (pelota_sig_x == raqueta2_x)) { // centro
							juego.pongPi.pelota.xv = -1;
							juego.pongPi.pelota.yv = 0;
							pelota_xv = juego.pongPi.pelota.xv;
							pelota_yv = juego.pongPi.pelota.yv;
							pelota_sig_x = pelota_x + pelota_xv;
							pelota_sig_y = pelota_y + pelota_yv;
							cnt_rebotes_pong++;
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
						cnt_rebotes_pong++;
						softPwmWrite(GPIO_PWM, 50);
					}else{
						juego.pongPi.pelota.x = 9;

						piLock (FLAGS_KEY);
						flags |= FLAG_FINAL_JUEGO;
						piUnlock (FLAGS_KEY);

						/*piLock (STD_IO_BUFFER_KEY);
						printf("\n");
						printf("Player 1 wins!");
						printf("\n");
						//juego.pongPi.pantalla = mensaje_derrota;
						PintaMensajeInicialPantalla((tipo_pantalla*)(&juego.pantalla),(tipo_pantalla*)(&mensaje_derrota));
						//PintaPantallaPorTerminal((tipo_pantalla*)(&juego.pantalla));
						piUnlock (STD_IO_BUFFER_KEY);*/
					}
				}
			}
		}

		if (juego.pongPi.pelota.x != 0 && juego.pongPi.pelota.x != 9){
		juego.pongPi.pelota.x += juego.pongPi.pelota.xv;
		juego.pongPi.pelota.y += juego.pongPi.pelota.yv;
		}
	piLock (STD_IO_BUFFER_KEY);
		ActualizaPantallaPong((tipo_pongPi*)(&juego.pongPi));
		juego.pantalla = juego.pongPi.pantalla;
		PintaPantallaPorTerminal((tipo_pantalla*)(&juego.pantalla));
	piUnlock (STD_IO_BUFFER_KEY);
}
/**
* @brief método asignado al temporizador que hace mover la pelota automáticamente
*/
void pelota_auto_pong(union sigval value) {
	if ((cnt_rebotes_pong % 5) == 0) {
		v_pelota_pong -= 10;
		if (v_pelota_pong <= 50){
			v_pelota_pong = 50;
		}
	}
	MovimientoPelotaPongFSM(inicio_fsm);
	tmr_startms(tmr_pelota_pong, v_pelota_pong);
}
/**
* @brief rutina donde se detiene el juego por interación del jugador1
* @param fsm
*/
void PauseIt1 (fsm_t *fsm) {
	piLock (FLAGS_KEY);
	flags &= ~(FLAG_RAQUETA_DERECHA || FLAG_RAQUETA_IZQUIERDA);
	piUnlock (FLAGS_KEY);

	piLock (STD_IO_BUFFER_KEY);
	printf("Pause Player 1.\n Press the buttons again to continue: ... ");
	ActualizaPantallaPong((tipo_pongPi*)(&juego.pongPi));
	juego.pantalla = juego.pongPi.pantalla;
	PintaPantallaPorTerminal((tipo_pantalla*)(&juego.pantalla));
	piUnlock (STD_IO_BUFFER_KEY);

	tmr_destroy(tmr_pelota_pong);
}
/**
* @brief rutina donde se detiene el juego por interación del jugador2
* @param fsm
*/
void PauseIt2 (fsm_t *fsm) {
	piLock (FLAGS_KEY);
	flags &= ~(FLAG_RAQUETA_UP || FLAG_RAQUETA_DOWN);
	piUnlock (FLAGS_KEY);

	piLock (STD_IO_BUFFER_KEY);
	printf("Pause Player 2.\n Press the buttons again to continue: ... ");
	ActualizaPantallaPong((tipo_pongPi*)(&juego.pongPi));
	juego.pantalla = juego.pongPi.pantalla;
	PintaPantallaPorTerminal((tipo_pantalla*)(&juego.pantalla));
	piUnlock (STD_IO_BUFFER_KEY);

	tmr_destroy(tmr_pelota_pong);
}
/**
* @brief rutina donde se reanuda el juego por interación del jugador1
* @param fsm
*/
void PlayIt1 (fsm_t *fsm) {
	piLock (FLAGS_KEY);
	flags &= ~(FLAG_RAQUETA_DERECHA || FLAG_RAQUETA_IZQUIERDA);
	piUnlock (FLAGS_KEY);

	piLock (STD_IO_BUFFER_KEY);
	printf("Play Player 1.\n Continue... ");
	ActualizaPantallaPong((tipo_pongPi*)(&juego.pongPi));
	juego.pantalla = juego.pongPi.pantalla;
	PintaPantallaPorTerminal((tipo_pantalla*)(&juego.pantalla));
	piUnlock (STD_IO_BUFFER_KEY);

	tmr_pelota_pong = tmr_new(pelota_auto_pong);
	tmr_startms(tmr_pelota_pong, v_pelota_pong);
}
/**
* @brief rutina donde se reanuda el juego por interación del jugador2
* @param fsm
*/
void PlayIt2 (fsm_t *fsm) {
	piLock (FLAGS_KEY);
	flags &= ~(FLAG_RAQUETA_UP || FLAG_RAQUETA_DOWN);
	piUnlock (FLAGS_KEY);

	piLock (STD_IO_BUFFER_KEY);
	printf("Play Player 2.\n Continue... ");
	ActualizaPantallaPong((tipo_pongPi*)(&juego.pongPi));
	juego.pantalla = juego.pongPi.pantalla;
	PintaPantallaPorTerminal((tipo_pantalla*)(&juego.pantalla));
	piUnlock (STD_IO_BUFFER_KEY);

	tmr_pelota_pong = tmr_new(pelota_auto_pong);
	tmr_startms(tmr_pelota_pong, v_pelota_pong);
}
/**
* @brief rutina donde se modifica la raqueta1 en una posición hacia arriba
* @param fsm
*/
void MueveRaqueta1UpFSM(fsm_t *fsm) {
	piLock (FLAGS_KEY);
	flags &= ~FLAG_RAQUETA_DERECHA;
	piUnlock (FLAGS_KEY);

	if(juego.pongPi.raqueta1.y > -1){
		juego.pongPi.raqueta1.y = juego.pongPi.raqueta1.y-1;
	} else {
		juego.pongPi.raqueta1.y = -1;
	}
	piLock (STD_IO_BUFFER_KEY);
	ActualizaPantallaPong((tipo_pongPi*)(&juego.pongPi));
	juego.pantalla = juego.pongPi.pantalla;
	PintaPantallaPorTerminal((tipo_pantalla*)(&juego.pantalla));
	piUnlock (STD_IO_BUFFER_KEY);
}
/**
* @brief rutina donde se modifica la raqueta2 en una posición hacia arriba
* @param fsm
*/
void MueveRaqueta2UpFSM(fsm_t *fsm) {
	piLock (FLAGS_KEY);
	flags &= ~FLAG_RAQUETA_UP;
	piUnlock (FLAGS_KEY);

	if(juego.pongPi.raqueta2.y > -1){
		juego.pongPi.raqueta2.y = juego.pongPi.raqueta2.y-1;
	} else {
		juego.pongPi.raqueta2.y = -1;
	}
	piLock (STD_IO_BUFFER_KEY);
	ActualizaPantallaPong((tipo_pongPi*)(&juego.pongPi));
	juego.pantalla = juego.pongPi.pantalla;
	PintaPantallaPorTerminal((tipo_pantalla*)(&juego.pantalla));
	piUnlock (STD_IO_BUFFER_KEY);
}
/**
* @brief rutina donde se modifica la raqueta2 en una posición hacia abajo
* @param fsm
*/
void MueveRaqueta2DownFSM(fsm_t *fsm) {
	piLock (FLAGS_KEY);
	flags &= ~FLAG_RAQUETA_DOWN;
	piUnlock (FLAGS_KEY);

	if(juego.pongPi.raqueta2.y < (MATRIZ_ALTO - RAQUETA_ANCHO)+1){
		juego.pongPi.raqueta2.y = juego.pongPi.raqueta2.y+1;
	} else {
		juego.pongPi.raqueta2.y = (MATRIZ_ALTO - RAQUETA_ANCHO)+1;
	}
	piLock (STD_IO_BUFFER_KEY);
	ActualizaPantallaPong((tipo_pongPi*)(&juego.pongPi));
	juego.pantalla = juego.pongPi.pantalla;
	PintaPantallaPorTerminal((tipo_pantalla*)(&juego.pantalla));
	piUnlock (STD_IO_BUFFER_KEY);
}
/**
* @brief rutina donde se modifica la raqueta1 en una posición hacia abajo
* @param fsm
*/
void MueveRaqueta1DownFSM(fsm_t *fsm) {
	piLock (FLAGS_KEY);
	flags &= ~FLAG_RAQUETA_IZQUIERDA;
	piUnlock (FLAGS_KEY);

	if(juego.pongPi.raqueta1.y < (MATRIZ_ALTO - RAQUETA_ANCHO)+1){
		juego.pongPi.raqueta1.y = juego.pongPi.raqueta1.y+1;
	} else {
		juego.pongPi.raqueta1.y = (MATRIZ_ALTO - RAQUETA_ANCHO)+1;
	}
	piLock (STD_IO_BUFFER_KEY);
	ActualizaPantallaPong((tipo_pongPi*)(&juego.pongPi));
	juego.pantalla = juego.pongPi.pantalla;
	PintaPantallaPorTerminal((tipo_pantalla*)(&juego.pantalla));
	piUnlock (STD_IO_BUFFER_KEY);
}

//------------------------------------------------------
// FUNCIONES DE SALIDA SNAKE
//------------------------------------------------------
/**
* @brief rutina donde se inicia el juego Snake
* @param fsm
*/
void InicializaJuegoSnakeFSM(fsm_t* fsm) {
	piLock (FLAGS_KEY);
	flags &= ~FLAG_TECLA;
	piUnlock (FLAGS_KEY);

	InicializaSnakePi((tipo_snakePi*)(&juego.snakePi));
	juego.pantalla = juego.snakePi.pantalla;
	tmr_startms(tmr_serpiente, TIMEOUT_SERPIENTE);

	piLock (STD_IO_BUFFER_KEY);
	ActualizaPantallaSnake((tipo_snakePi*)(&juego.snakePi));
	PintaPantallaPorTerminal((tipo_pantalla*)(&juego.pantalla));
	piUnlock (STD_IO_BUFFER_KEY);
}
/**void pintaFrutaTMR(union sigval value) {
	//int fruta_x = rand() % MATRIZ_ANCHO;
	//int fruta_y = rand() % MATRIZ_ALTO;
	int fruta_x=3;
	int fruta_y=1;

	if(juego.snakePi.pantalla.matriz[fruta_x][fruta_y] == 0){
		juego.snakePi.fruta.x = fruta_x;
		juego.snakePi.fruta.y = fruta_y;
		//juego.snakePi.pantalla.matriz[fruta_x][fruta_y] = 1;

		PintaFruta((tipo_fruta*)(&juego.snakePi.fruta), (tipo_pantalla*)(&juego.snakePi.pantalla));

		piLock (STD_IO_BUFFER_KEY);
			ActualizaPantalla((tipo_snakePi*)(&juego.snakePi));
			PintaPantallaPorTerminal((tipo_pantalla*)(&juego.snakePi.pantalla));
		piUnlock (STD_IO_BUFFER_KEY);
	}

	tmr_startms(tmr_fruta, v_fruta);
}*/
/**
* @brief rutina donde se modifica la serpiente en una posición siguiendo su trayectoria, hasta que se pierda en el juego
* @param fsm
*/
void MovimientoSerpienteFSM(fsm_t* fsm) {
	/*piLock (FLAGS_KEY);
	flags &= ~FLAG_PELOTA;
	piUnlock (FLAGS_KEY);*/

		int serpiente_x = juego.snakePi.serpiente.x[0];
		int serpiente_y = juego.snakePi.serpiente.y[0];
		int serpiente_xv = juego.snakePi.serpiente.xv;
		int serpiente_yv = juego.snakePi.serpiente.yv;
		int serpiente_sig_x = serpiente_x + serpiente_xv;
		int serpiente_sig_y = serpiente_y + serpiente_yv;

		int fruta_x = juego.snakePi.fruta.x;
		int fruta_y = juego.snakePi.fruta.y;

		softPwmWrite(GPIO_PWM, 0);
		
		if (serpiente_x == 0 || serpiente_x == 9 || serpiente_y == 0 || serpiente_y == 6){
			if (serpiente_y == 0){
				if (serpiente_yv < 0){
					juego.snakePi.serpiente.y[0] = 0;

					piLock (FLAGS_KEY);
					flags |= FLAG_FINAL_JUEGO;
					piUnlock (FLAGS_KEY);
					
					/*piLock (STD_IO_BUFFER_KEY);
					printf("\n");
					printf("You lose crashing with a wall! %d fruits eaten", cnt_frutas);
					printf("\n");
					//juego.snakePi.pantalla = mensaje_derrota;
					PintaMensajeInicialPantalla((tipo_pantalla*)(&juego.pantalla),(tipo_pantalla*)(&mensaje_derrota));
					//PintaPantallaPorTerminal((tipo_pantalla*)(&juego.pantalla));
					piUnlock (STD_IO_BUFFER_KEY);*/
				}
			}
			if (serpiente_y == 6){
				if (serpiente_yv > 0){
					juego.snakePi.serpiente.y[0] = 6;

					piLock (FLAGS_KEY);
					flags |= FLAG_FINAL_JUEGO;
					piUnlock (FLAGS_KEY);
					
					/*piLock (STD_IO_BUFFER_KEY);
					printf("\n");
					printf("You lose crashing with a wall! %d fruits eaten", cnt_frutas);
					printf("\n");
					//juego.snakePi.pantalla = mensaje_derrota;
					PintaMensajeInicialPantalla((tipo_pantalla*)(&juego.pantalla),(tipo_pantalla*)(&mensaje_derrota));
					//PintaPantallaPorTerminal((tipo_pantalla*)(&juego.pantalla));
					piUnlock (STD_IO_BUFFER_KEY);*/
				}
			}
			if (serpiente_x == 0){
				if (serpiente_xv < 0){
					juego.snakePi.serpiente.x[0] = 0;

					piLock (FLAGS_KEY);
					flags |= FLAG_FINAL_JUEGO;
					piUnlock (FLAGS_KEY);
					
					/*piLock (STD_IO_BUFFER_KEY);
					printf("\n");
					printf("You lose crashing with a wall! %d fruits eaten", cnt_frutas);
					printf("\n");
					//juego.snakePi.pantalla = mensaje_derrota;
					PintaMensajeInicialPantalla((tipo_pantalla*)(&juego.pantalla),(tipo_pantalla*)(&mensaje_derrota));
					//PintaPantallaPorTerminal((tipo_pantalla*)(&juego.pantalla));
					piUnlock (STD_IO_BUFFER_KEY);*/
				}
			}
			if (serpiente_x == 9){
				if (serpiente_xv > 0){
					juego.snakePi.serpiente.x[0] = 9;

					piLock (FLAGS_KEY);
					flags |= FLAG_FINAL_JUEGO;
					piUnlock (FLAGS_KEY);
					
					/*piLock (STD_IO_BUFFER_KEY);
					printf("\n");
					printf("You lose crashing with a wall! %d fruits eaten", cnt_frutas);
					printf("\n");
					//juego.snakePi.pantalla = mensaje_derrota;
					PintaMensajeInicialPantalla((tipo_pantalla*)(&juego.pantalla),(tipo_pantalla*)(&mensaje_derrota));
					//PintaPantallaPorTerminal((tipo_pantalla*)(&juego.pantalla));
					piUnlock (STD_IO_BUFFER_KEY);*/
				}
			}
		}
		if(juego.snakePi.pantalla.matriz[serpiente_sig_x][serpiente_sig_y] == 1){
			if(serpiente_sig_x == fruta_x && serpiente_sig_y == fruta_y){
				juego.snakePi.pantalla.matriz[serpiente_sig_x][serpiente_sig_y] = 0;
				juego.snakePi.serpiente.alto++;
				cnt_frutas++;
				//ReseteaFrutaAleatorio((tipo_fruta*)(&(juego.snakePi.fruta)));
				ReseteaFrutaAleatorio((tipo_fruta*)(&(juego.snakePi.fruta)), (tipo_pantalla*)(&juego.snakePi.pantalla));
				//ReseteaFrutaAleatorio((tipo_fruta*)(&(juego.snakePi.fruta)), (tipo_pantalla*)(&juego.pantalla));
				softPwmWrite(GPIO_PWM, 50);
				//juego.snakePi.fruta.x = -1;
				//juego.snakePi.fruta.y = -1;
			} else {
				juego.snakePi.serpiente.x[0] = serpiente_sig_x;
				juego.snakePi.serpiente.y[0] = serpiente_sig_y;

				piLock (FLAGS_KEY);
				flags |= FLAG_FINAL_JUEGO;
				piUnlock (FLAGS_KEY);
				
				/*piLock (STD_IO_BUFFER_KEY);
				printf("\n");
				printf("Did you try to eat yourself? %d fruits eaten", cnt_frutas);
				printf("\n");
				//juego.snakePi.pantalla = mensaje_derrota;
				PintaMensajeInicialPantalla((tipo_pantalla*)(&juego.pantalla),(tipo_pantalla*)(&mensaje_derrota));
				//PintaPantallaPorTerminal((tipo_pantalla*)(&juego.pantalla));
				piUnlock (STD_IO_BUFFER_KEY);*/
			}
		}
		if (juego.snakePi.serpiente.x[0] >= 0 && juego.snakePi.serpiente.x[0] <= 9 &&
				juego.snakePi.serpiente.y[0] >= 0 && juego.snakePi.serpiente.y[0] <= 6){
			juego.snakePi.serpiente.x[0] += juego.snakePi.serpiente.xv;
			juego.snakePi.serpiente.y[0] += juego.snakePi.serpiente.yv;
		}
	piLock (STD_IO_BUFFER_KEY);
		ActualizaPantallaSnake((tipo_snakePi*)(&juego.snakePi));
		juego.pantalla = juego.snakePi.pantalla;
		PintaPantallaPorTerminal((tipo_pantalla*)(&juego.pantalla));
	piUnlock (STD_IO_BUFFER_KEY);
}
/**
* @brief método asignado al temporizador que hace mover la serpiente automáticamente
*/
void serpiente_auto(union sigval value) {
	if ((cnt_frutas % 5) == 0) {
		v_serpiente -= 10;
		if (v_serpiente <= 200){
			v_serpiente = 200;
		}
	}
	MovimientoSerpienteFSM(inicio_fsm);
	tmr_startms(tmr_serpiente, v_serpiente);
}
/**
* @brief rutina donde se modifica la serpiente en una posición hacia arriba
* @param fsm
*/
void MueveSerpienteUpFSM(fsm_t *fsm) {
	piLock (FLAGS_KEY);
	flags &= ~FLAG_RAQUETA_UP;
	piUnlock (FLAGS_KEY);

	if(juego.snakePi.serpiente.yv == 0){
		juego.snakePi.serpiente.yv = -1;
		juego.snakePi.serpiente.xv = 0;
	}
	juego.snakePi.serpiente.x[0] += juego.snakePi.serpiente.xv;
	juego.snakePi.serpiente.y[0] += juego.snakePi.serpiente.yv;
	piLock (STD_IO_BUFFER_KEY);
	ActualizaPantallaSnake((tipo_snakePi*)(&juego.snakePi));
	juego.pantalla = juego.snakePi.pantalla;
	PintaPantallaPorTerminal((tipo_pantalla*)(&juego.pantalla));
	piUnlock (STD_IO_BUFFER_KEY);
}
/**
* @brief rutina donde se modifica la serpiente en una posición hacia abajo
* @param fsm
*/
void MueveSerpienteDownFSM(fsm_t *fsm) {
	piLock (FLAGS_KEY);
	flags &= ~FLAG_RAQUETA_DOWN;
	piUnlock (FLAGS_KEY);

	if(juego.snakePi.serpiente.yv == 0){
		juego.snakePi.serpiente.yv = 1;
		juego.snakePi.serpiente.xv = 0;
	}
	juego.snakePi.serpiente.x[0] += juego.snakePi.serpiente.xv;
	juego.snakePi.serpiente.y[0] += juego.snakePi.serpiente.yv;
	piLock (STD_IO_BUFFER_KEY);
	ActualizaPantallaSnake((tipo_snakePi*)(&juego.snakePi));
	juego.pantalla = juego.snakePi.pantalla;
	PintaPantallaPorTerminal((tipo_pantalla*)(&juego.pantalla));
	piUnlock (STD_IO_BUFFER_KEY);
}
/**
* @brief rutina donde se modifica la serpiente en una posición hacia la derecha
* @param fsm
*/
void MueveSerpienteRightFSM(fsm_t *fsm) {
	piLock (FLAGS_KEY);
	flags &= ~FLAG_RAQUETA_DERECHA;
	piUnlock (FLAGS_KEY);

	if(juego.snakePi.serpiente.xv == 0){
		juego.snakePi.serpiente.xv = 1;
		juego.snakePi.serpiente.yv = 0;
	}
	juego.snakePi.serpiente.x[0] += juego.snakePi.serpiente.xv;
	juego.snakePi.serpiente.y[0] += juego.snakePi.serpiente.yv;
	piLock (STD_IO_BUFFER_KEY);
	ActualizaPantallaSnake((tipo_snakePi*)(&juego.snakePi));
	juego.pantalla = juego.snakePi.pantalla;
	PintaPantallaPorTerminal((tipo_pantalla*)(&juego.pantalla));
	piUnlock (STD_IO_BUFFER_KEY);
}
/**
* @brief rutina donde se modifica la serpiente en una posición hacia la izquierda
* @param fsm
*/
void MueveSerpienteLeftFSM(fsm_t *fsm) {
	piLock (FLAGS_KEY);
	flags &= ~FLAG_RAQUETA_IZQUIERDA;
	piUnlock (FLAGS_KEY);

	if(juego.snakePi.serpiente.xv == 0){
		juego.snakePi.serpiente.xv = -1;
		juego.snakePi.serpiente.yv = 0;
	}
	juego.snakePi.serpiente.x[0] += juego.snakePi.serpiente.xv;
	juego.snakePi.serpiente.y[0] += juego.snakePi.serpiente.yv;
	piLock (STD_IO_BUFFER_KEY);
	ActualizaPantallaSnake((tipo_snakePi*)(&juego.snakePi));
	juego.pantalla = juego.snakePi.pantalla;
	PintaPantallaPorTerminal((tipo_pantalla*)(&juego.pantalla));
	piUnlock (STD_IO_BUFFER_KEY);
}
/**
* @brief rutina donde se detiene el juego por interacción de los botones up y down
* @param fsm
*/
void PauseItUpDown (fsm_t *fsm) {
	piLock (FLAGS_KEY);
	flags &= ~(FLAG_RAQUETA_UP || FLAG_RAQUETA_DOWN);
	piUnlock (FLAGS_KEY);

	piLock (STD_IO_BUFFER_KEY);
	printf("Pause Up&Down.\n Press the buttons again to continue: ... ");
	ActualizaPantallaSnake((tipo_snakePi*)(&juego.snakePi));
	juego.pantalla = juego.snakePi.pantalla;
	PintaPantallaPorTerminal((tipo_pantalla*)(&juego.pantalla));
	piUnlock (STD_IO_BUFFER_KEY);

	tmr_destroy(tmr_serpiente);
}
/**
* @brief rutina donde se detiene el juego por interacción de los botones right y left
* @param fsm
*/
void PauseItRightLeft (fsm_t *fsm) {
	piLock (FLAGS_KEY);
	flags &= ~(FLAG_RAQUETA_DERECHA || FLAG_RAQUETA_IZQUIERDA);
	piUnlock (FLAGS_KEY);

	piLock (STD_IO_BUFFER_KEY);
	printf("Pause Right&Left.\n Press the buttons again to continue: ... ");
	ActualizaPantallaSnake((tipo_snakePi*)(&juego.snakePi));
	juego.pantalla = juego.snakePi.pantalla;
	PintaPantallaPorTerminal((tipo_pantalla*)(&juego.pantalla));
	piUnlock (STD_IO_BUFFER_KEY);

	tmr_destroy(tmr_serpiente);
}
/**
* @brief rutina donde se reanuda el juego por interacción de los botones up y down
* @param fsm
*/
void PlayItUpDown (fsm_t *fsm) {
	piLock (FLAGS_KEY);
	flags &= ~(FLAG_RAQUETA_UP || FLAG_RAQUETA_DOWN);
	piUnlock (FLAGS_KEY);

	piLock (STD_IO_BUFFER_KEY);
	printf("Play Up&Down.\n Continue... ");
	ActualizaPantallaSnake((tipo_snakePi*)(&juego.snakePi));
	juego.pantalla = juego.snakePi.pantalla;
	PintaPantallaPorTerminal((tipo_pantalla*)(&juego.pantalla));
	piUnlock (STD_IO_BUFFER_KEY);

	tmr_serpiente = tmr_new(serpiente_auto);
	tmr_startms(tmr_serpiente, v_serpiente);
}
/**
* @brief rutina donde se reanuda el juego por interacción de los botones right y left
* @param fsm
*/
void PlayItRightLeft (fsm_t *fsm) {
	piLock (FLAGS_KEY);
	flags &= ~(FLAG_RAQUETA_DERECHA || FLAG_RAQUETA_IZQUIERDA);
	piUnlock (FLAGS_KEY);

	piLock (STD_IO_BUFFER_KEY);
	printf("Play Right&Left.\n Continue... ");
	ActualizaPantallaSnake((tipo_snakePi*)(&juego.snakePi));
	juego.pantalla = juego.snakePi.pantalla;
	PintaPantallaPorTerminal((tipo_pantalla*)(&juego.pantalla));
	piUnlock (STD_IO_BUFFER_KEY);

	tmr_serpiente = tmr_new(serpiente_auto);
	tmr_startms(tmr_serpiente, v_serpiente);
}

//------------------------------------------------------
// FUNCIONES DE SALIDA COMUNES
//------------------------------------------------------
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
		PintaMensajeInicialPantalla((tipo_pantalla*)(&juego.pantalla),(tipo_pantalla*)(&mensaje_victoria));
		//PintaPantallaPorTerminal((tipo_pantalla*)(&juego.pantalla));
	} else if (juego.arkanoPi.pelota.y == 6) {
		printf("\n");
		printf("You lose in the %d level, %d bricks remaining!", nivel, CalculaLadrillosRestantes((tipo_pantalla*)(&juego.arkanoPi.ladrillos.matriz)));
		printf("\n");
		PintaMensajeInicialPantalla((tipo_pantalla*)(&juego.pantalla),(tipo_pantalla*)(&mensaje_derrota));
		//PintaPantallaPorTerminal((tipo_pantalla*)(&juego.pantalla));
	} else if (juego.pongPi.pelota.x == 9) {
		printf("\n");
		printf("Player 1 wins!");
		printf("\n");
		PintaMensajeInicialPantalla((tipo_pantalla*)(&juego.pantalla),(tipo_pantalla*)(&mensaje_derrota));
		//PintaPantallaPorTerminal((tipo_pantalla*)(&juego.pantalla));
	} else if(juego.pongPi.pelota.x == 0) {
		printf("\n");
		printf("Player 2 wins!");
		printf("\n");
		PintaMensajeInicialPantalla((tipo_pantalla*)(&juego.pantalla),(tipo_pantalla*)(&mensaje_derrota));
		//PintaPantallaPorTerminal((tipo_pantalla*)(&juego.pantalla));
	} else if (juego.snakePi.serpiente.x[0] == 0 || juego.snakePi.serpiente.x[0] == 9 || juego.snakePi.serpiente.y[0] == 0 || juego.snakePi.serpiente.y[0] == 6){
		printf("\n");
		printf("You crashed into a wall! %d fruits eaten", cnt_frutas);
		printf("\n");
		PintaMensajeInicialPantalla((tipo_pantalla*)(&juego.pantalla),(tipo_pantalla*)(&mensaje_derrota));
		//PintaPantallaPorTerminal((tipo_pantalla*)(&juego.pantalla));
	} else {
		printf("\n");
		printf("Did you try to eat yourself? %d fruits eaten", cnt_frutas);
		printf("\n");
		PintaMensajeInicialPantalla((tipo_pantalla*)(&juego.pantalla),(tipo_pantalla*)(&mensaje_derrota));
		//PintaPantallaPorTerminal((tipo_pantalla*)(&juego.pantalla));
	}

	PintaPantallaPorTerminal((tipo_pantalla*)(&juego.pantalla));
	piUnlock (STD_IO_BUFFER_KEY);
	
	tmr_destroy(tmr_pelota);
	tmr_destroy(tmr_pelota_pong);
	tmr_destroy(tmr_serpiente);
}
/**
* @brief rutina donde se reinicia el juego y los elementos vuelven a sus estados iniciales
* @param fsm
*/
void ReseteaJuegoFSM(fsm_t* fsm) {
	piLock (FLAGS_KEY);
	flags &= ~FLAG_TECLA;
	piUnlock (FLAGS_KEY);

	tmr_pelota = tmr_new(pelota_auto);
	tmr_pelota_pong = tmr_new(pelota_auto_pong);
	tmr_serpiente = tmr_new(serpiente_auto);

	piLock (STD_IO_BUFFER_KEY);
	PintaMensajeInicialPantalla((tipo_pantalla*)(&juego.pantalla),(tipo_pantalla*)(&mensaje_inicial));
	PintaPantallaPorTerminal((tipo_pantalla*)(&juego.pantalla));
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
			//delay (1) ;
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
				//delay (1) ;
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
* @brief método asignado a la inturrupción por la pulsación del boton asignado
*/
void boton_up(void) {
	// Pin event (key / button event) debouncing procedure
		if (millis () < debounceTime)
		{
			debounceTime = millis () + DEBOUNCE_TIME ;
			return;
		}

		piLock (FLAGS_KEY);
		flags |= FLAG_RAQUETA_UP;
		piUnlock (FLAGS_KEY);

			// Wait for key to be released
		while (digitalRead (GPIO_BOTON_2_UP) == HIGH) {
			delay (1);
			if (digitalRead (GPIO_BOTON_2_DOWN) == HIGH) {
				piLock (FLAGS_KEY);
				flags |= (FLAG_RAQUETA_UP || FLAG_RAQUETA_DOWN);
				piUnlock (FLAGS_KEY);
			}
		}

		debounceTime = millis () + DEBOUNCE_TIME ;
}
/**
* @brief método asignado a la inturrupción por la pulsación del boton asignado
*/
void boton_down(void) {
	// Pin event (key / button event) debouncing procedure
		if (millis () < debounceTime)
		{
			debounceTime = millis () + DEBOUNCE_TIME ;
			return;
		}

		piLock (FLAGS_KEY);
		flags |= FLAG_RAQUETA_DOWN;
		piUnlock (FLAGS_KEY);

			// Wait for key to be released
		while (digitalRead (GPIO_BOTON_2_DOWN) == HIGH) {
			delay (1);
			if (digitalRead (GPIO_BOTON_2_UP) == HIGH) {
				piLock (FLAGS_KEY);
				flags |= (FLAG_RAQUETA_UP || FLAG_RAQUETA_DOWN);
				piUnlock (FLAGS_KEY);
			}
		}

		debounceTime = millis () + DEBOUNCE_TIME ;
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
* @brief método asignado a la inturrupción por la pulsación del boton asignado
*/
void boton_salida(void) {
	/*// Pin event (key / button event) debouncing procedure
		if (millis () < debounceTime)
		{
			debounceTime = millis () + DEBOUNCE_TIME ;
			return;
		}*/
		exit(0);

		/*	// Wait for key to be released
		while (digitalRead (GPIO_BOTON_SALIDA) == HIGH) {
			delay (1) ;
		}

		debounceTime = millis () + DEBOUNCE_TIME ;*/
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

	digitalWrite(GPIO_FILA_0, !juego.pantalla.matriz[columna][0]);
	digitalWrite(GPIO_FILA_1, !juego.pantalla.matriz[columna][1]);
	digitalWrite(GPIO_FILA_2, !juego.pantalla.matriz[columna][2]);
	digitalWrite(GPIO_FILA_3, !juego.pantalla.matriz[columna][3]);
	digitalWrite(GPIO_FILA_4, !juego.pantalla.matriz[columna][4]);
	digitalWrite(GPIO_FILA_5, !juego.pantalla.matriz[columna][5]);
	digitalWrite(GPIO_FILA_6, !juego.pantalla.matriz[columna][6]);

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
	PintaMensajeInicialPantalla((tipo_pantalla*)(&juego.pantalla),(tipo_pantalla*)(&mensaje_inicial));
	PintaPantallaPorTerminal((tipo_pantalla*)(&juego.pantalla));
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
		// {EstadoOrigen,FunciÃ³nDeEntrada,EstadoDestino,FunciÃ³nDeSalida}
		fsm_trans_t fsm_tabla[] = {
				{ WAIT_START, compruebaTeclaRaquetaDerecha,  WAIT_PUSH_ARKANO, InicializaJuegoArkanoFSM },
				{ WAIT_START, compruebaTeclaRaquetaIzquierda,  WAIT_PUSH_PONG, InicializaJuegoPongFSM },
				{ WAIT_START, compruebaTeclaPulsada,  WAIT_PUSH_SNAKE, InicializaJuegoSnakeFSM },

				//{ WAIT_PUSH_ARKANO, compruebaTeclaPelota, WAIT_PUSH_ARKANO,   MovimientoPelotaFSM },
				{ WAIT_PUSH_ARKANO, compruebaPausePlay, WAIT_PAUSE_ARKANO,   PauseIt },
				{ WAIT_PUSH_ARKANO, compruebaTeclaRaquetaDerecha, WAIT_PUSH_ARKANO,   MueveRaquetaDerechaFSM },
				{ WAIT_PUSH_ARKANO, compruebaTeclaRaquetaIzquierda, WAIT_PUSH_ARKANO,   MueveRaquetaIzquierdaFSM },
				{ WAIT_PAUSE_ARKANO, compruebaPausePlay, WAIT_PUSH_ARKANO,   PlayIt },
				{ WAIT_PUSH_ARKANO, compruebaFinalJuego, WAIT_END,   FinalJuegoFSM },

				//{ WAIT_PUSH_PONG, compruebaTeclaPelota, WAIT_PUSH_PONG,   MovimientoPelotaPongFSM },
				{ WAIT_PUSH_PONG, compruebaPausePlay, WAIT_PAUSE_1,   PauseIt1 },
				{ WAIT_PUSH_PONG, compruebaPausePlay2, WAIT_PAUSE_2,   PauseIt2 },
				{ WAIT_PUSH_PONG, compruebaTeclaRaquetaDerecha, WAIT_PUSH_PONG,   MueveRaqueta1UpFSM },
				{ WAIT_PUSH_PONG, compruebaTeclaRaquetaIzquierda, WAIT_PUSH_PONG,   MueveRaqueta1DownFSM },
				{ WAIT_PUSH_PONG, compruebaTeclaRaqueta2Up, WAIT_PUSH_PONG,   MueveRaqueta2UpFSM },
				{ WAIT_PUSH_PONG, compruebaTeclaRaqueta2Down, WAIT_PUSH_PONG,   MueveRaqueta2DownFSM },
				{ WAIT_PAUSE_1, compruebaPausePlay, WAIT_PUSH_PONG,   PlayIt1 },
				{ WAIT_PAUSE_2, compruebaPausePlay2, WAIT_PUSH_PONG,   PlayIt2 },
				{ WAIT_PUSH_PONG, compruebaFinalJuego, WAIT_END,   FinalJuegoFSM },

				//{ WAIT_PUSH_SNAKE, compruebaTeclaPelota, WAIT_PUSH_SNAKE,   MovimientoSerpienteFSM },
				{ WAIT_PUSH_SNAKE, compruebaPausePlay, WAIT_PAUSE_UD,   PauseItRightLeft },
				{ WAIT_PUSH_SNAKE, compruebaPausePlay2, WAIT_PAUSE_RL,   PauseItUpDown },
				{ WAIT_PUSH_SNAKE, compruebaTeclaRaquetaDerecha, WAIT_PUSH_SNAKE,   MueveSerpienteRightFSM },
				{ WAIT_PUSH_SNAKE, compruebaTeclaRaquetaIzquierda, WAIT_PUSH_SNAKE,   MueveSerpienteLeftFSM },
				{ WAIT_PUSH_SNAKE, compruebaTeclaRaqueta2Up, WAIT_PUSH_SNAKE,   MueveSerpienteUpFSM },
				{ WAIT_PUSH_SNAKE, compruebaTeclaRaqueta2Down, WAIT_PUSH_SNAKE,   MueveSerpienteDownFSM },
				{ WAIT_PAUSE_UD, compruebaPausePlay, WAIT_PUSH_SNAKE,   PlayItRightLeft },
				{ WAIT_PAUSE_RL, compruebaPausePlay2, WAIT_PUSH_SNAKE,   PlayItUpDown },
				{ WAIT_PUSH_SNAKE, compruebaFinalJuego, WAIT_END,   FinalJuegoFSM },

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

		pinMode(GPIO_BOTON_SALIDA,INPUT);
		pullUpDnControl(GPIO_BOTON_SALIDA, 0);

		wiringPiISR(GPIO_BOTON_SALIDA, INT_EDGE_FALLING, boton_salida);

		pinMode(GPIO_BOTON_2_UP,INPUT);
		pinMode(GPIO_BOTON_2_DOWN,INPUT);
		pullUpDnControl(GPIO_BOTON_2_UP, 0);
		pullUpDnControl(GPIO_BOTON_2_DOWN, 0);

		wiringPiISR(GPIO_BOTON_2_UP, INT_EDGE_FALLING, boton_up);
		wiringPiISR(GPIO_BOTON_2_DOWN, INT_EDGE_FALLING, boton_down);

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
		tmr_pelota_pong = tmr_new(pelota_auto_pong);
		tmr_serpiente = tmr_new(serpiente_auto);

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
