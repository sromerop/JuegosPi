#include "snakePi.h"

static volatile tipo_juego juego;

volatile int flags = 0;
tmr_t* tmr;
tmr_t* tmr_serpiente;
static int gpio_cols[4]={GPIO_COL_0,GPIO_COL_1,GPIO_COL_2,GPIO_COL_3};  // array de columnas
unsigned volatile int columna = 0;
int debounceTime;
fsm_t* inicio_fsm;
tipo_pantalla mensaje_inicial;
int cnt_frutas = 1;
int v_serpiente = TIMEOUT_SERPIENTE;
tipo_pantalla mensaje_derrota;
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
/*int compruebaTeclaSerpiente (fsm_t* this) {
	int result;

	piLock (FLAGS_KEY);
	result = (flags & FLAG_SERPIENTE);
	piUnlock (FLAGS_KEY);

	return result;
}*/
/**
* @brief función donde se comprueba la interrupción de la pulsación del boton_up
* @param this
* @return devuelve 1 si la iterrupción ha sido causada por esta pulsación; 0 en caso contrario
*/
int compruebaTeclaSerpienteUp (fsm_t* this) {
	int result;

	piLock (FLAGS_KEY);
	result = (flags & FLAG_SERPIENTE_UP);
	piUnlock (FLAGS_KEY);

	return result;
}
/**
* @brief función donde se comprueba la interrupción de la pulsación del boton_down
* @param this
* @return devuelve 1 si la iterrupción ha sido causada por esta pulsación; 0 en caso contrario
*/
int compruebaTeclaSerpienteDown (fsm_t* this) {
	int result;

	piLock (FLAGS_KEY);
	result = (flags & FLAG_SERPIENTE_DOWN);
	piUnlock (FLAGS_KEY);

	return result;
}
/**
* @brief función donde se comprueba la interrupción de la pulsación del boton_right
* @param this
* @return devuelve 1 si la iterrupción ha sido causada por esta pulsación; 0 en caso contrario
*/
int compruebaTeclaSerpienteRight (fsm_t* this) {
	int result;

	piLock (FLAGS_KEY);
	result = (flags & FLAG_SERPIENTE_RIGHT);
	piUnlock (FLAGS_KEY);

	return result;
}
/**
* @brief función donde se comprueba la interrupción de la pulsación del boton_left
* @param this
* @return devuelve 1 si la iterrupción ha sido causada por esta pulsación; 0 en caso contrario
*/
int compruebaTeclaSerpienteLeft (fsm_t* this) {
	int result;

	piLock (FLAGS_KEY);
	result = (flags & FLAG_SERPIENTE_LEFT);
	piUnlock (FLAGS_KEY);

	return result;
}
/**
* @brief función donde se comprueba la interrupción de la pulsación simultánea del boton_up y boton_down
* @param this
* @return devuelve 1 si la iterrupción ha sido causada por estas pulsaciones; 0 en caso contrario
*/
int compruebaPausePlayUpDown (fsm_t* this) {
	int result;

	piLock (FLAGS_KEY);
	result = (flags & (FLAG_SERPIENTE_UP || FLAG_SERPIENTE_DOWN));
	piUnlock (FLAGS_KEY);

	return result;
}
/**
* @brief función donde se comprueba la interrupción de la pulsación simultánea del boton_right y boron_left
* @param this
* @return devuelve 1 si la iterrupción ha sido causada por estas pulsaciones; 0 en caso contrario
*/
int compruebaPausePlayRightLeft (fsm_t* this) {
	int result;

	piLock (FLAGS_KEY);
	result = (flags & (FLAG_SERPIENTE_RIGHT || FLAG_SERPIENTE_LEFT));
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

	InicializaSnakePi((tipo_snakePi*)(&juego.snakePi));
	tmr_startms(tmr_serpiente, TIMEOUT_SERPIENTE);

	piLock (STD_IO_BUFFER_KEY);
	ActualizaPantalla((tipo_snakePi*)(&juego.snakePi));
	PintaPantallaPorTerminal((tipo_pantalla*)(&juego.snakePi.pantalla));
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
		//juego.snakePi.pantalla.matriz[fruta_x][frut:qa_y] = 1;
		
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
	//piLock (FLAGS_KEY);
	//flags &= ~FLAG_SERPIENTE;
	//piUnlock (FLAGS_KEY);

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
				}
			}
			if (serpiente_y == 6){
				if (serpiente_yv > 0){
					juego.snakePi.serpiente.y[0] = 6;
					piLock (FLAGS_KEY);
					flags |= FLAG_FINAL_JUEGO;
					piUnlock (FLAGS_KEY);
				}
			}
			if (serpiente_x == 0){
				if (serpiente_xv < 0){
					juego.snakePi.serpiente.x[0] = 0;
					piLock (FLAGS_KEY);
					flags |= FLAG_FINAL_JUEGO;
					piUnlock (FLAGS_KEY);
				}
			}
			if (serpiente_x == 9){
				if (serpiente_xv > 0){
					juego.snakePi.serpiente.x[0] = 9;
					piLock (FLAGS_KEY);
					flags |= FLAG_FINAL_JUEGO;
					piUnlock (FLAGS_KEY);
				}
			}
		}
		if(juego.snakePi.pantalla.matriz[serpiente_sig_x][serpiente_sig_y] == 1){
			if(serpiente_sig_x == fruta_x && serpiente_sig_y == fruta_y){
				juego.snakePi.pantalla.matriz[serpiente_sig_x][serpiente_sig_y] = 0;
				juego.snakePi.serpiente.alto++;
				cnt_frutas++;
				//juego.snakePi.fruta.x = -1;
				//juego.snakePi.fruta.y = -1;
				ReseteaFrutaAleatorio((tipo_fruta*)(&(juego.snakePi.fruta)));
				//ReseteaFrutaAleatorio((tipo_fruta*)(&(juego.snakePi.fruta)), (tipo_pantalla*)(&juego.snakePi.pantalla));
				softPwmWrite(GPIO_PWM, 75);
			} else {
				juego.snakePi.serpiente.x[0] = serpiente_sig_x;
				juego.snakePi.serpiente.y[0] = serpiente_sig_y;
				piLock (FLAGS_KEY);
				flags |= FLAG_FINAL_JUEGO;
				piUnlock (FLAGS_KEY);
			}
		}
		if (juego.snakePi.serpiente.x[0] >= 0 && juego.snakePi.serpiente.x[0] <= 9 &&
				juego.snakePi.serpiente.y[0] >= 0 && juego.snakePi.serpiente.y[0] <= 6){
			juego.snakePi.serpiente.x[0] += juego.snakePi.serpiente.xv;
			juego.snakePi.serpiente.y[0] += juego.snakePi.serpiente.yv;
		}
	piLock (STD_IO_BUFFER_KEY);
		ActualizaPantalla((tipo_snakePi*)(&juego.snakePi));
		PintaPantallaPorTerminal((tipo_pantalla*)(&juego.snakePi.pantalla));
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
	flags &= ~FLAG_SERPIENTE_UP;
	piUnlock (FLAGS_KEY);

	if(juego.snakePi.serpiente.yv == 0){
		juego.snakePi.serpiente.yv = -1;
		juego.snakePi.serpiente.xv = 0;
	} 
	juego.snakePi.serpiente.x[0] += juego.snakePi.serpiente.xv;
	juego.snakePi.serpiente.y[0] += juego.snakePi.serpiente.yv;
	piLock (STD_IO_BUFFER_KEY);
	ActualizaPantalla((tipo_snakePi*)(&juego.snakePi));
	PintaPantallaPorTerminal((tipo_pantalla*)(&juego.snakePi.pantalla));
	piUnlock (STD_IO_BUFFER_KEY);
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
		flags |= FLAG_SERPIENTE_UP;
		piUnlock (FLAGS_KEY);

			// Wait for key to be released
		while (digitalRead (GPIO_BOTON_UP) == HIGH) {
			delay (1);
			if (digitalRead (GPIO_BOTON_DOWN) == HIGH) {
				piLock (FLAGS_KEY);
				flags |= (FLAG_SERPIENTE_UP || FLAG_SERPIENTE_DOWN);
				piUnlock (FLAGS_KEY);
			}
		}

		debounceTime = millis () + DEBOUNCE_TIME ;
}
/**
* @brief rutina donde se modifica la serpiente en una posición hacia abajo
* @param fsm
*/
void MueveSerpienteDownFSM(fsm_t *fsm) {
	piLock (FLAGS_KEY);
	flags &= ~FLAG_SERPIENTE_DOWN;
	piUnlock (FLAGS_KEY);

	if(juego.snakePi.serpiente.yv == 0){
		juego.snakePi.serpiente.yv = 1;
		juego.snakePi.serpiente.xv = 0;
	} 
	juego.snakePi.serpiente.x[0] += juego.snakePi.serpiente.xv;
	juego.snakePi.serpiente.y[0] += juego.snakePi.serpiente.yv;
	piLock (STD_IO_BUFFER_KEY);
	ActualizaPantalla((tipo_snakePi*)(&juego.snakePi));
	PintaPantallaPorTerminal((tipo_pantalla*)(&juego.snakePi.pantalla));
	piUnlock (STD_IO_BUFFER_KEY);
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
		flags |= FLAG_SERPIENTE_DOWN;
		piUnlock (FLAGS_KEY);

			// Wait for key to be released
		while (digitalRead (GPIO_BOTON_DOWN) == HIGH) {
			delay (1);
			if (digitalRead (GPIO_BOTON_UP) == HIGH) {
				piLock (FLAGS_KEY);
				flags |= (FLAG_SERPIENTE_UP || FLAG_SERPIENTE_DOWN);
				piUnlock (FLAGS_KEY);
			}
		}

		debounceTime = millis () + DEBOUNCE_TIME ;
}
/**
* @brief rutina donde se modifica la serpiente en una posición hacia la derecha
* @param fsm
*/
void MueveSerpienteRightFSM(fsm_t *fsm) {
	piLock (FLAGS_KEY);
	flags &= ~FLAG_SERPIENTE_RIGHT;
	piUnlock (FLAGS_KEY);

	if(juego.snakePi.serpiente.xv == 0){
		juego.snakePi.serpiente.xv = 1;
		juego.snakePi.serpiente.yv = 0;
	} 
	juego.snakePi.serpiente.x[0] += juego.snakePi.serpiente.xv;
	juego.snakePi.serpiente.y[0] += juego.snakePi.serpiente.yv;
	piLock (STD_IO_BUFFER_KEY);
	ActualizaPantalla((tipo_snakePi*)(&juego.snakePi));
	PintaPantallaPorTerminal((tipo_pantalla*)(&juego.snakePi.pantalla));
	piUnlock (STD_IO_BUFFER_KEY);
}
/**
* @brief método asignado a la inturrupción por la pulsación del boton asignado
*/
void boton_right(void) {
	// Pin event (key / button event) debouncing procedure
		if (millis () < debounceTime)
		{
			debounceTime = millis () + DEBOUNCE_TIME ;
			return;
		}

		piLock (FLAGS_KEY);
		flags |= FLAG_SERPIENTE_RIGHT;
		piUnlock (FLAGS_KEY);

			// Wait for key to be released
		while (digitalRead (GPIO_BOTON_RIGHT) == HIGH) {
			delay (1);
			if (digitalRead (GPIO_BOTON_LEFT) == HIGH) {
				piLock (FLAGS_KEY);
				flags |= (FLAG_SERPIENTE_RIGHT || FLAG_SERPIENTE_LEFT);
				piUnlock (FLAGS_KEY);
			}
		}

		debounceTime = millis () + DEBOUNCE_TIME ;
}
/**
* @brief rutina donde se modifica la serpiente en una posición hacia la izquierda
* @param fsm
*/
void MueveSerpienteLeftFSM(fsm_t *fsm) {
	piLock (FLAGS_KEY);
	flags &= ~FLAG_SERPIENTE_LEFT;
	piUnlock (FLAGS_KEY);

	if(juego.snakePi.serpiente.xv == 0){
		juego.snakePi.serpiente.xv = -1;
		juego.snakePi.serpiente.yv = 0;
	} 
	juego.snakePi.serpiente.x[0] += juego.snakePi.serpiente.xv;
	juego.snakePi.serpiente.y[0] += juego.snakePi.serpiente.yv;
	piLock (STD_IO_BUFFER_KEY);
	ActualizaPantalla((tipo_snakePi*)(&juego.snakePi));
	PintaPantallaPorTerminal((tipo_pantalla*)(&juego.snakePi.pantalla));
	piUnlock (STD_IO_BUFFER_KEY);
}
/**
* @brief método asignado a la inturrupción por la pulsación del boton asignado
*/
void boton_left(void) {
	// Pin event (key / button event) debouncing procedure
		if (millis () < debounceTime)
		{
			debounceTime = millis () + DEBOUNCE_TIME ;
			return;
		}

		piLock (FLAGS_KEY);
		flags |= FLAG_SERPIENTE_LEFT;
		piUnlock (FLAGS_KEY);

			// Wait for key to be released
		while (digitalRead (GPIO_BOTON_LEFT) == HIGH) {
			delay (1);
			if (digitalRead (GPIO_BOTON_RIGHT) == HIGH) {
				piLock (FLAGS_KEY);
				flags |= (FLAG_SERPIENTE_RIGHT || FLAG_SERPIENTE_LEFT);
				piUnlock (FLAGS_KEY);
			}
		}

		debounceTime = millis () + DEBOUNCE_TIME ;
}
/**
* @brief rutina donde se detiene el juego por interacción de los botones up y down
* @param fsm
*/
void PauseItUpDown (fsm_t *fsm) {
	piLock (FLAGS_KEY);
	flags &= ~(FLAG_SERPIENTE_UP || FLAG_SERPIENTE_DOWN);
	piUnlock (FLAGS_KEY);

	piLock (STD_IO_BUFFER_KEY);
	printf("Pause Up&Down.\n Press the buttons again to continue: ... ");
	ActualizaPantalla((tipo_snakePi*)(&juego.snakePi));
	PintaPantallaPorTerminal((tipo_pantalla*)(&juego.snakePi.pantalla));
	piUnlock (STD_IO_BUFFER_KEY);

	tmr_destroy(tmr_serpiente);
}
/**
* @brief rutina donde se detiene el juego por interacción de los botones right y left
* @param fsm
*/
void PauseItRightLeft (fsm_t *fsm) {
	piLock (FLAGS_KEY);
	flags &= ~(FLAG_SERPIENTE_RIGHT || FLAG_SERPIENTE_LEFT);
	piUnlock (FLAGS_KEY);

	piLock (STD_IO_BUFFER_KEY);
	printf("Pause Right&Left.\n Press the buttons again to continue: ... ");
	ActualizaPantalla((tipo_snakePi*)(&juego.snakePi));
	PintaPantallaPorTerminal((tipo_pantalla*)(&juego.snakePi.pantalla));
	piUnlock (STD_IO_BUFFER_KEY);

	tmr_destroy(tmr_serpiente);
}
/**
* @brief rutina donde se reanuda el juego por interacción de los botones up y down
* @param fsm
*/
void PlayItUpDown (fsm_t *fsm) {
	piLock (FLAGS_KEY);
	flags &= ~(FLAG_SERPIENTE_UP || FLAG_SERPIENTE_DOWN);
	piUnlock (FLAGS_KEY);

	piLock (STD_IO_BUFFER_KEY);
	printf("Play Up&Down.\n Continue... ");
	ActualizaPantalla((tipo_snakePi*)(&juego.snakePi));
	PintaPantallaPorTerminal((tipo_pantalla*)(&juego.snakePi.pantalla));
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
	flags &= ~(FLAG_SERPIENTE_RIGHT || FLAG_SERPIENTE_LEFT);
	piUnlock (FLAGS_KEY);

	piLock (STD_IO_BUFFER_KEY);
	printf("Play Right&Left.\n Continue... ");
	ActualizaPantalla((tipo_snakePi*)(&juego.snakePi));
	PintaPantallaPorTerminal((tipo_pantalla*)(&juego.snakePi.pantalla));
	piUnlock (STD_IO_BUFFER_KEY);

	tmr_serpiente = tmr_new(serpiente_auto);
	tmr_startms(tmr_serpiente, v_serpiente);
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
		printf("\n");
		printf("You lose!");
		printf("\n");
		if (juego.snakePi.serpiente.x[0] == 0 || juego.snakePi.serpiente.x[0] == 9 || juego.snakePi.serpiente.y[0] == 0 || juego.snakePi.serpiente.y[0] == 6){
			printf("\n");
			printf("You crashed into a wall! %d fruits eaten", cnt_frutas);
			printf("\n");
		} else{
			printf("\n");
			printf("Did you try to eat yourself? %d fruits eaten", cnt_frutas);
			printf("\n");
		}
		PintaMensajeInicialPantalla((tipo_pantalla*)(&juego.snakePi.pantalla),(tipo_pantalla*)(&mensaje_derrota));
		PintaPantallaPorTerminal((tipo_pantalla*)(&juego.snakePi.pantalla));
	piUnlock (STD_IO_BUFFER_KEY);

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

	ReseteaMatriz((tipo_pantalla*)(&juego.snakePi.pantalla.matriz));
	ReseteaSerpiente((tipo_serpiente*)(&juego.snakePi.serpiente));
	ReseteaFruta((tipo_fruta*)(&juego.snakePi.fruta));
	
	tmr_serpiente = tmr_new(serpiente_auto);
	//longitud = juego.snakePi.serpiente.alto;
	juego.snakePi.serpiente.alto = SERPIENTE_ALTO;
	cnt_frutas = 1;
	v_serpiente = TIMEOUT_SERPIENTE;

	piLock (STD_IO_BUFFER_KEY);
	ActualizaPantalla((tipo_snakePi*)(&juego.snakePi));
	PintaPantallaPorTerminal((tipo_pantalla*)(&juego.snakePi.pantalla));
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

	digitalWrite(GPIO_FILA_0, !juego.snakePi.pantalla.matriz[columna][0]);
	digitalWrite(GPIO_FILA_1, !juego.snakePi.pantalla.matriz[columna][1]);
	digitalWrite(GPIO_FILA_2, !juego.snakePi.pantalla.matriz[columna][2]);
	digitalWrite(GPIO_FILA_3, !juego.snakePi.pantalla.matriz[columna][3]);
	digitalWrite(GPIO_FILA_4, !juego.snakePi.pantalla.matriz[columna][4]);
	digitalWrite(GPIO_FILA_5, !juego.snakePi.pantalla.matriz[columna][5]);
	digitalWrite(GPIO_FILA_6, !juego.snakePi.pantalla.matriz[columna][6]);

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
	int x = 0;

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
	PintaMensajeInicialPantalla((tipo_pantalla*)(&juego.snakePi.pantalla),(tipo_pantalla*)(&mensaje_inicial));
	PintaPantallaPorTerminal((tipo_pantalla*)(&juego.snakePi.pantalla));
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
					flags |= FLAG_SERPIENTE_LEFT;
					piUnlock (FLAGS_KEY);
					break;

				case 's':
					piLock (FLAGS_KEY);
					flags |= FLAG_SERPIENTE_RIGHT;
					piUnlock (FLAGS_KEY);
					break;

				case 'w':
					piLock (FLAGS_KEY);
					flags |= FLAG_SERPIENTE_UP;
					piUnlock (FLAGS_KEY);
					break;

				case 'z':
					piLock (FLAGS_KEY);
					flags |= FLAG_SERPIENTE_DOWN;
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
		// {EstadoOrigen,FunciÃ³nDeEntrada,EstadoDestino,FunciÃ³nDeSalida}
		fsm_trans_t fsm_tabla[] = {
				{ WAIT_START, compruebaTeclaPulsada,  WAIT_PUSH, InicializaJuegoFSM },
				//{ WAIT_PUSH, compruebaTeclaSerpiente, WAIT_PUSH,   MovimientoSerpienteFSM },
				{ WAIT_PUSH, compruebaPausePlayUpDown, WAIT_PAUSE_UD,   PauseItUpDown },
				{ WAIT_PUSH, compruebaPausePlayRightLeft, WAIT_PAUSE_RL,   PauseItRightLeft },
				{ WAIT_PUSH, compruebaTeclaSerpienteUp, WAIT_PUSH,   MueveSerpienteUpFSM },
				{ WAIT_PUSH, compruebaTeclaSerpienteDown, WAIT_PUSH,   MueveSerpienteDownFSM },
				{ WAIT_PUSH, compruebaTeclaSerpienteRight, WAIT_PUSH,   MueveSerpienteRightFSM },
				{ WAIT_PUSH, compruebaTeclaSerpienteLeft, WAIT_PUSH,   MueveSerpienteLeftFSM },
				{ WAIT_PAUSE_UD, compruebaPausePlayUpDown, WAIT_PUSH,   PlayItUpDown },
				{ WAIT_PAUSE_RL, compruebaPausePlayRightLeft, WAIT_PUSH,   PlayItRightLeft },
				{ WAIT_PUSH, compruebaFinalJuego, WAIT_END,   FinalJuegoFSM },
				{ WAIT_END, compruebaTeclaPulsada, WAIT_START,   ReseteaJuegoFSM },
				{ -1, NULL, -1, NULL },
		};
		

		/*Inicialización del Hardware*/
		wiringPiSetupGpio();

		/* Asociación de entradas del Hardware */
		pinMode(GPIO_BOTON_UP,INPUT);
		pinMode(GPIO_BOTON_DOWN,INPUT);
		pinMode(GPIO_BOTON_RIGHT,INPUT);
		pinMode(GPIO_BOTON_LEFT,INPUT);
		pullUpDnControl(GPIO_BOTON_UP, 0);
		pullUpDnControl(GPIO_BOTON_DOWN, 0);
		pullUpDnControl(GPIO_BOTON_RIGHT, 0);
		pullUpDnControl(GPIO_BOTON_LEFT, 0);

		wiringPiISR(GPIO_BOTON_UP, INT_EDGE_FALLING, boton_up);
		wiringPiISR(GPIO_BOTON_DOWN, INT_EDGE_FALLING, boton_down);
		wiringPiISR(GPIO_BOTON_RIGHT, INT_EDGE_FALLING, boton_right);
		wiringPiISR(GPIO_BOTON_LEFT, INT_EDGE_FALLING, boton_left);

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
		tmr_serpiente = tmr_new(serpiente_auto);
		//tmr_fruta = tmr_new(pintaFrutaTMR);
		

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
		tmr_destroy(tmr_serpiente);
		//tmr_destroy(tmr_fruta);
}
