/** File name	: arkanoPiLib.c
 *  Description	: Funcionalidades referidas a la matriz, los ladrillos, la raqueta y la pelota
 */

/*Includes---------------------------------------*/
#include "pongPiLib.h"

/*Private variables------------------------------*/
/*tipo_pantalla mensaje_inicial = {
			{
			{0,0,0,0,0,1,0},
			{1,1,1,0,0,1,1},
			{1,0,1,0,1,0,1},
			{1,1,1,0,1,1,1},
			{0,0,0,0,1,1,1},
			{0,0,0,0,0,0,0},
			{1,1,1,0,0,1,0},
			{1,0,1,0,1,0,0},
			{1,1,1,1,1,1,1},
			{0,0,0,0,0,0,0},
}};*/
tipo_pantalla mensaje_inicial = {
			{
			{0,0,0,0,0,0,0},
			{1,1,1,1,1,1,1},
			{1,0,1,0,1,0,0},
			{1,1,1,0,0,1,0},
			{0,0,0,0,1,1,1},
			{0,0,0,0,0,0,0},
			{1,1,1,0,1,1,1},
			{1,0,1,0,1,0,1},
			{1,1,1,0,0,1,1},
			{0,0,0,0,0,1,0},
}};
tipo_pantalla mensaje_derrota = {
			{
			{0,0,0,0,0,0,0},
			{0,0,0,0,0,0,0},
			{0,1,1,0,0,0,1},
			{0,1,1,0,0,1,0},
			{0,0,0,0,0,1,0},
			{0,0,0,0,0,1,0},
			{0,1,1,0,0,1,0},
			{0,1,1,0,0,0,1},
			{0,0,0,0,0,0,0},
			{0,0,0,0,0,0,0},
}};
//------------------------------------------------------
// FUNCIONES DE INICIALIZACION / RESET
//------------------------------------------------------
/**
 * @brief	m�todo que pone la matriz entera a cero
 * @param	p_pantalla
 */
void ReseteaMatriz(tipo_pantalla *p_pantalla) {
	/* Local variables */
	int i, j = 0;

	for(i=0;i<MATRIZ_ANCHO;i++) {
		for(j=0;j<MATRIZ_ALTO;j++) {
			p_pantalla->matriz[i][j] = 0;
		}
	}
}

/**
 * @brief	m�todo que restablece la pelota a su estado inicial
 * @param	p_pelota
 */
void ReseteaPelota(tipo_pelota *p_pelota) {
	// Pelota inicialmente en el centro de la pantalla
	p_pelota->x = MATRIZ_ANCHO/2 - 1;
	p_pelota->y = MATRIZ_ALTO/2 - 1;

	// Trayectoria inicial
	p_pelota->yv = 0;
	p_pelota->xv = 1;
}

/**
 * @brief	m�todo que restablece las raquetas a su estado inicial
 * @param	p_raqueta_1
 * @param	p_raqueta_2
 */
void ReseteaRaquetas(tipo_raqueta *p_raqueta_1, tipo_raqueta *p_raqueta_2) {
	// Raqueta inicialmente en los extremos y en el centro de la pantalla
	p_raqueta_1->x = 0;
	p_raqueta_1->y = MATRIZ_ALTO/2 - p_raqueta_1->alto/2;
	p_raqueta_1->ancho = RAQUETA_ANCHO;
	p_raqueta_1->alto = RAQUETA_ALTO;

	p_raqueta_2->x = MATRIZ_ANCHO - 1;
	p_raqueta_2->y = MATRIZ_ALTO/2 - p_raqueta_2->alto/2;
	p_raqueta_2->ancho = RAQUETA_ANCHO;
	p_raqueta_2->alto = RAQUETA_ALTO;
}

//------------------------------------------------------
// FUNCIONES DE VISUALIZACION (ACTUALIZACION DEL OBJETO PANTALLA QUE LUEGO USARA EL DISPLAY)
//------------------------------------------------------
/**
 * @brief	m�todo encargado de aprovechar el display para presentar un mensaje de bienvenida al usuario
 * @param	p_pantalla
 * @param	p_pantalla_inicial
 */
void PintaMensajeInicialPantalla (tipo_pantalla *p_pantalla, tipo_pantalla *p_pantalla_inicial) {
	/* Local variables */
	int i, j = 0;
	//p_pantalla_inicial = &(mensaje_inicial);
			for(i=0;i<MATRIZ_ANCHO;i++) {
				for(j=0;j<MATRIZ_ALTO;j++) {
					p_pantalla->matriz[i][j] = p_pantalla_inicial->matriz[i][j];
				}
			}
}

/**
 * @brief	m�todo encargado de mostrar el contenido o la ocupaci�n de la matriz de leds en la ventana de terminal o consola
 * @param	p_pantalla
 */
void PintaPantallaPorTerminal  (tipo_pantalla *p_pantalla) {
	/* Local variables */
	int i, j = 0;

	printf("\n");
	for(i=0;i<MATRIZ_ALTO;i++) {
		for(j=0;j<MATRIZ_ANCHO;j++) {
			printf("%d ",p_pantalla->matriz[j][i]);
		}
		printf("\n");
	}
	printf("\n");
}

/**
 * @brief	m�todo encargado de pintar las raquetas en su posici�n correspondiente dentro del �rea de juego
 * @param	p_raqueta_1
 * @param	p_raqueta_2
 * @param	p_pantalla
 */
void PintaRaquetas(tipo_raqueta *p_raqueta_1, tipo_raqueta *p_raqueta_2, tipo_pantalla *p_pantalla) {
	/* Local variables */
	int i, j = 0;
	int k, l = 0;

	for(i=0;i<RAQUETA_ALTO;i++) {
		for(j=0;j<RAQUETA_ANCHO;j++) {
			if (( (p_raqueta_1->x+i >= 0) && (p_raqueta_1->x+i < MATRIZ_ANCHO) ) &&
					( (p_raqueta_1->y+j >= 0) && (p_raqueta_1->y+j < MATRIZ_ALTO) ))
				p_pantalla->matriz[p_raqueta_1->x+i][p_raqueta_1->y+j] = 1;
		}
	}

	for(k=0;k<RAQUETA_ALTO;k++) {
			for(l=0;l<RAQUETA_ANCHO;l++) {
				if (( (p_raqueta_2->x+k >= 0) && (p_raqueta_2->x+k < MATRIZ_ANCHO) ) &&
						( (p_raqueta_2->y+l >= 0) && (p_raqueta_2->y+l < MATRIZ_ALTO) ))
					p_pantalla->matriz[p_raqueta_2->x+k][p_raqueta_2->y+l] = 1;
			}
		}

}

/**
 * @brief	m�todo encargado de pintar la pelota en su posici�n correspondiente dentro del �rea de juego
 * @param	p_pelota
 * @param	p_pantalla
 */
void PintaPelota(tipo_pelota *p_pelota, tipo_pantalla *p_pantalla) {
	if( (p_pelota->x >= 0) && (p_pelota->x < MATRIZ_ANCHO) ) {
		if( (p_pelota->y >= 0) && (p_pelota->y < MATRIZ_ALTO) ) {
			p_pantalla->matriz[p_pelota->x][p_pelota->y] = 1;
		}
		else {
			printf("\n\nPROBLEMAS!!!! posicion y=%d de la pelota INVALIDA!!!\n\n", p_pelota->y);
			fflush(stdout);
		}
	}
	else {
		printf("\n\nPROBLEMAS!!!! posicion x=%d de la pelota INVALIDA!!!\n\n", p_pelota->x);
		fflush(stdout);
	}
}

/**
 * @brief	m�todo cuya ejecucion estara ligada a cualquiera de los movimientos de la raqueta o de la pelota y que
 * 			ser� el encargado de actualizar convenientemente la estructura de datos
 * 			en memoria que representa el �rea de juego y su correspondiente estado
 * @param	p_pongPi
 */
void ActualizaPantalla(tipo_pongPi *p_pongPi) {
    // Borro toda la pantalla
	ReseteaMatriz((tipo_pantalla*)(&(p_pongPi->pantalla)));

	// Completo la pantalla con los elementos correspondientes
	PintaRaquetas((tipo_raqueta*)(&(p_pongPi->raqueta1)), (tipo_raqueta*)(&(p_pongPi->raqueta2)), (tipo_pantalla*)(&(p_pongPi->pantalla)));
	PintaPelota((tipo_pelota*)(&(p_pongPi->pelota)), ((tipo_pantalla*)(&(p_pongPi->pantalla))));

}

/**
 * @brief	m�todo encargado de la inicializaci�n de toda variable o estructura de datos especificamente ligada al
 * 			desarrollo del juego y su visualizacion
 * @param	p_pongPi
 */
void InicializaPongPi(tipo_pongPi *p_pongPi) {
	// Borro toda la pantalla
	ReseteaMatriz((tipo_pantalla*)(&(p_pongPi->pantalla)));

	// Completo la pantalla con los elementos correspondientes en sus posiciones originales
	ReseteaPelota((tipo_pelota*)(&(p_pongPi->pelota)));
	ReseteaRaquetas((tipo_raqueta*)(&(p_pongPi->raqueta1)), (tipo_raqueta*)(&(p_pongPi->raqueta2)));

	// Se procede a mostrar el mensaje inicial
	PintaPantallaPorTerminal((tipo_pantalla*)(&(p_pongPi->pantalla)));

}
