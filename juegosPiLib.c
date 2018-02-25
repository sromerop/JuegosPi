/** File name	: arkanoPiLib.c
 *  Description	: Funcionalidades referidas a la matriz, los ladrillos, la raqueta y la pelota
 */

/*Includes---------------------------------------*/
#include "juegosPiLib.h"

/*Private variables------------------------------*/
int ladrillos_basico[MATRIZ_ANCHO][MATRIZ_ALTO] = {
			{0,1,0,0,0,0,0},
			{2,0,0,0,0,0,0},
			{0,1,0,0,0,0,0},
			{2,0,0,0,0,0,0},
			{0,1,0,0,0,0,0},
			{2,0,0,0,0,0,0},
			{0,1,0,0,0,0,0},
			{2,0,0,0,0,0,0},
			{0,1,0,0,0,0,0},
			{2,0,0,0,0,0,0},
};
tipo_pantalla mensaje_victoria = {
			{
			{0,0,0,0,0,0,0},
			{0,0,0,0,0,0,0},
			{0,1,1,0,1,0,0},
			{0,1,1,0,0,1,0},
			{0,0,0,0,0,1,0},
			{0,0,0,0,0,1,0},
			{0,1,1,0,0,1,0},
			{0,1,1,0,1,0,0},
			{0,0,0,0,0,0,0},
			{0,0,0,0,0,0,0},
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
tipo_pantalla mensaje_inicial = {
			{
			{0,1,1,1,1,1,0},
			{0,1,0,1,0,0,0},
			{0,1,1,1,0,0,0},
			{0,0,1,0,0,1,0},
			{0,1,0,1,0,1,0},
			{0,1,0,1,0,1,0},
			{0,1,0,0,1,0,0},
			{0,1,1,1,1,1,0},
			{0,1,0,1,0,0,0},
			{0,1,1,1,1,1,0},
}};
tipo_pantalla ladrillos_nv2 = {
			{
			{3,2,0,0,0,0,0},
			{3,0,0,0,0,0,0},
			{3,2,0,0,0,0,0},
			{3,0,0,0,0,0,0},
			{3,2,0,0,0,0,0},
			{3,0,0,0,0,0,0},
			{3,2,0,0,0,0,0},
			{3,0,0,0,0,0,0},
			{3,2,0,0,0,0,0},
			{3,0,0,0,0,0,0},
}};
tipo_pantalla ladrillos_nv3 = {
			{
			{4,3,0,0,0,0,0},
			{4,0,2,0,0,0,0},
			{4,3,0,0,0,0,0},
			{4,0,2,0,0,0,0},
			{4,3,0,0,0,0,0},
			{4,0,2,0,0,0,0},
			{4,3,0,0,0,0,0},
			{4,0,2,0,0,0,0},
			{4,3,0,0,0,0,0},
			{4,0,2,0,0,0,0},
}};
//------------------------------------------------------
// FUNCIONES DE INICIALIZACION / RESET
//------------------------------------------------------
/**
 * @brief	método que pone la matriz entera a cero
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
 * @brief	método que restablece los ladrillos a su estado inicial
 * @param	p_ladrillos
 */
void ReseteaLadrillos(tipo_pantalla *p_ladrillos) {
	/* Local variables */
	int i, j = 0;

	for(i=0;i<MATRIZ_ANCHO;i++) {
		for(j=0;j<MATRIZ_ALTO;j++) {
			p_ladrillos->matriz[i][j] = ladrillos_basico[i][j];
		}
	}
}

/**
 * @brief	método que restablece la pelota a su estado inicial
 * @param	p_pelota
 */
void ReseteaPelota(tipo_pelota *p_pelota) {
	// Pelota inicialmente en el centro de la pantalla
	p_pelota->x = MATRIZ_ANCHO/2 - 1;
	p_pelota->y = MATRIZ_ALTO/2 - 1;

	// Trayectoria inicial
	p_pelota->yv = 1;
	p_pelota->xv = 0;
}

/**
 * @brief	método que restablece la raqueta a su estado inicial
 * @param	p_raqueta
 */
void ReseteaRaqueta(tipo_raqueta *p_raqueta) {
	// Raqueta inicialmente en el centro de la pantalla
	p_raqueta->x = MATRIZ_ANCHO/2 - p_raqueta->ancho/2;
	p_raqueta->y = MATRIZ_ALTO - 1;
	p_raqueta->ancho = RAQUETA_ANCHO;
	p_raqueta->alto = RAQUETA_ALTO;
}

/**
 * @brief	método que restablece la pelota a su estado inicial
 * @param	p_pelota
 */
void ReseteaPelotaPong(tipo_pelota *p_pelota) {
	// Pelota inicialmente en el centro de la pantalla
	p_pelota->x = MATRIZ_ANCHO/2 - 1;
	p_pelota->y = MATRIZ_ALTO/2 - 1;

	// Trayectoria inicial
	p_pelota->yv = 0;
	p_pelota->xv = 1;
}

/**
 * @brief	método que restablece la raqueta a su estado inicial
 * @param	p_raqueta
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

/**
 * @brief	método que restablece la raqueta a su estado inicial
 * @param	p_raqueta
 */
void ReseteaSerpiente(tipo_serpiente *p_serpiente) {
	// Serpiente inicialmente en el centro de la pantalla
	p_serpiente->x[0] = MATRIZ_ANCHO/2;
	p_serpiente->x[1] = MATRIZ_ANCHO/2 - 1;
	p_serpiente->x[2] = MATRIZ_ANCHO/2 - 2;
	p_serpiente->y[0] = MATRIZ_ALTO/2 - p_serpiente->alto/2;
	p_serpiente->y[1] = MATRIZ_ALTO/2 - p_serpiente->alto/2;
	p_serpiente->y[2] = MATRIZ_ALTO/2 - p_serpiente->alto/2;
	p_serpiente->x_antigua = MATRIZ_ANCHO/2;
	p_serpiente->y_antigua = MATRIZ_ALTO/2 - p_serpiente->alto/2;
	p_serpiente->x_antigua1 = 0;
	p_serpiente->y_antigua1 = 0;
	p_serpiente->ancho = SERPIENTE_ANCHO;
	p_serpiente->alto = SERPIENTE_ALTO;
	p_serpiente->xv = 1;
	p_serpiente->yv = 0;
}
void ReseteaFruta(tipo_fruta *p_fruta){
	p_fruta->x = MATRIZ_ANCHO - 7;
	p_fruta->y = MATRIZ_ALTO - 2;
}

//void ReseteaFrutaAleatorio(tipo_fruta *p_fruta){
void ReseteaFrutaAleatorio(tipo_fruta *p_fruta, tipo_pantalla *p_pantalla){
	int x = rand() % MATRIZ_ANCHO;
	int y = rand() % MATRIZ_ALTO;
		
	while (p_pantalla->matriz[x][y] == 1){
		x = rand() % MATRIZ_ANCHO;
		y = rand() % MATRIZ_ALTO;
	}
		
	p_fruta->x = x;
	p_fruta->y = y;
}
//------------------------------------------------------
// FUNCIONES DE VISUALIZACION (ACTUALIZACION DEL OBJETO PANTALLA QUE LUEGO USARA EL DISPLAY)
//------------------------------------------------------
/**
 * @brief	método encargado de aprovechar el display para presentar un mensaje de bienvenida al usuario
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
 * @brief	método encargado de mostrar el contenido o la ocupación de la matriz de leds en la ventana de terminal o consola
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
 * @brief	método encargado de pintar los ladrillos en sus correspondientes posiciones dentro del área de juego
 * @param	p_ladrillos
 * @param	p_pantalla
 */
void PintaLadrillos(tipo_pantalla *p_ladrillos, tipo_pantalla *p_pantalla) {
	/* Local variables */
	int i, j = 0;

	for(i=0;i<MATRIZ_ANCHO;i++) {
		for(j=0;j<MATRIZ_ALTO;j++) {
			p_pantalla->matriz[i][j] = p_ladrillos->matriz[i][j];
		}
    }
}

/**
 * @brief	método encargado de pintar la raqueta en su posición correspondiente dentro del área de juego
 * @param	p_raqueta
 * @param	p_pantalla
 */
void PintaRaqueta(tipo_raqueta *p_raqueta, tipo_pantalla *p_pantalla) {
	/* Local variables */
	int i, j = 0;

	for(i=0;i<RAQUETA_ANCHO;i++) {
		for(j=0;j<RAQUETA_ALTO;j++) {
			if (( (p_raqueta->x+i >= 0) && (p_raqueta->x+i < MATRIZ_ANCHO) ) &&
					( (p_raqueta->y+j >= 0) && (p_raqueta->y+j < MATRIZ_ALTO) ))
				p_pantalla->matriz[p_raqueta->x+i][p_raqueta->y+j] = 1;
		}
	}
}

/**
 * @brief	método encargado de pintar la pelota en su posición correspondiente dentro del área de juego
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
 * @brief	método cuya ejecucion estara ligada a cualquiera de los movimientos de la raqueta o de la pelota y que
 * 			será el encargado de actualizar convenientemente la estructura de datos
 * 			en memoria que representa el área de juego y su correspondiente estado
 * @param	p_arkanoPi
 */
void ActualizaPantallaArkano(tipo_arkanoPi* p_arkanoPi) {
    // Borro toda la pantalla
	ReseteaMatriz((tipo_pantalla*)(&(p_arkanoPi->pantalla)));

	// Completo la pantalla con los elementos correspondientes
	PintaLadrillos((tipo_pantalla*)(&(p_arkanoPi->ladrillos)), (tipo_pantalla*)(&(p_arkanoPi->pantalla)));
	PintaRaqueta((tipo_raqueta*)(&(p_arkanoPi->raqueta)), (tipo_pantalla*)(&(p_arkanoPi->pantalla)));
	PintaPelota((tipo_pelota*)(&(p_arkanoPi->pelota)), ((tipo_pantalla*)(&(p_arkanoPi->pantalla))));

}
/*void delay_until_inicial (unsigned int next) {
	unsigned int now = millis();
	if (next > now) {
		delay (next - now);
    }
}*/
/**
 * @brief	método encargado de la inicialización de toda variable o estructura de datos especificamente ligada al
 * 			desarrollo del juego y su visualizacion
 * @param	p_arkanoPi
 */
void InicializaArkanoPi(tipo_arkanoPi *p_arkanoPi) {
	// Borro toda la pantalla
	ReseteaMatriz((tipo_pantalla*)(&(p_arkanoPi->pantalla)));

	// Completo la pantalla con los elementos correspondientes en sus posiciones originales
	ReseteaLadrillos((tipo_pantalla*)(&(p_arkanoPi->ladrillos)));
	ReseteaPelota((tipo_pelota*)(&(p_arkanoPi->pelota)));
	ReseteaRaqueta((tipo_raqueta*)(&(p_arkanoPi->raqueta)));

	// Se procede a mostrar el mensaje inicial
	PintaPantallaPorTerminal((tipo_pantalla*)(&(p_arkanoPi->pantalla)));

}

/**
 * @brief	método encargado de evaluar el estado de ocupacion del area de juego por los ladrillos y devolver
 * 			el número de estos
 * @param	p_ladrillos
 * @return	número de ladrillos restantes
 */
int CalculaLadrillosRestantes(tipo_pantalla *p_ladrillos) {
	/* Local variables */
	int num_ladrillos_restantes = 0;
	int i, j = 0;

		for(i=0;i<LADRILLOS_ANCHO;i++) {
			for(j=0;j<LADRILLOS_ALTO;j++) {
				if (p_ladrillos->matriz[i][j] >= 1){
					num_ladrillos_restantes++;
				}
			}
	    }
	return num_ladrillos_restantes;
}

/**
 * @brief	método encargado de pintar la raqueta en su posición correspondiente dentro del área de juego
 * @param	p_raqueta
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
 * @brief	método encargado de pintar la pelota en su posición correspondiente dentro del área de juego
 * @param	p_pelota
 * @param	p_pantalla
 */
void PintaPelotaPong(tipo_pelota *p_pelota, tipo_pantalla *p_pantalla) {
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
 * @brief	método cuya ejecucion estara ligada a cualquiera de los movimientos de la raqueta o de la pelota y que
 * 			será el encargado de actualizar convenientemente la estructura de datos
 * 			en memoria que representa el área de juego y su correspondiente estado
 * @param	p_arkanoPi
 */
void ActualizaPantallaPong(tipo_pongPi *p_pongPi) {
    // Borro toda la pantalla
	ReseteaMatriz((tipo_pantalla*)(&(p_pongPi->pantalla)));

	// Completo la pantalla con los elementos correspondientes
	PintaRaquetas((tipo_raqueta*)(&(p_pongPi->raqueta1)), (tipo_raqueta*)(&(p_pongPi->raqueta2)), (tipo_pantalla*)(&(p_pongPi->pantalla)));
	PintaPelotaPong((tipo_pelota*)(&(p_pongPi->pelota)), ((tipo_pantalla*)(&(p_pongPi->pantalla))));

}

/**
 * @brief	método encargado de la inicialización de toda variable o estructura de datos especificamente ligada al
 * 			desarrollo del juego y su visualizacion
 * @param	p_arkanoPi
 */
void InicializaPongPi(tipo_pongPi *p_pongPi) {
	// Borro toda la pantalla
	ReseteaMatriz((tipo_pantalla*)(&(p_pongPi->pantalla)));

	// Completo la pantalla con los elementos correspondientes en sus posiciones originales
	ReseteaPelotaPong((tipo_pelota*)(&(p_pongPi->pelota)));
	ReseteaRaquetas((tipo_raqueta*)(&(p_pongPi->raqueta1)), (tipo_raqueta*)(&(p_pongPi->raqueta2)));

	// Se procede a mostrar el mensaje inicial
	PintaPantallaPorTerminal((tipo_pantalla*)(&(p_pongPi->pantalla)));

}

/**
 * @brief	método encargado de pintar la raqueta en su posición correspondiente dentro del área de juego
 * @param	p_raqueta
 * @param	p_pantalla
 */
void PintaSerpiente(tipo_serpiente *p_serpiente, tipo_pantalla *p_pantalla) {
	/* Local variables */
	/*int i, j = 0;

	for(i=0;i<p_serpiente->alto;i++) {
		for(j=0;j<p_serpiente->ancho;j++) {
			if (( (p_serpiente->x+i >= 0) && (p_serpiente->x+i < MATRIZ_ANCHO) ) &&
					( (p_serpiente->y+j >= 0) && (p_serpiente->y+j < MATRIZ_ALTO) ))
				p_pantalla->matriz[p_serpiente->x+i][p_serpiente->y+j] = 1;
		}
	}*/
	/* Local variables */
		/*int i, j = 0;

	    p_pantalla->matriz[p_serpiente->x[0]][p_serpiente->y[0]] = 1;

	    for(i=0;i<p_serpiente->alto - 1;i++) {
	    	  //for(j=0;i<p_serpiente->alto - 1;j++) {
	    	p_serpiente->x[i+1] = p_serpiente->x_antigua;
	    	p_serpiente->y[j+1] = p_serpiente->y_antigua;
	    	if (( (p_serpiente->x[0]+i >= 0) && (p_serpiente->x[0]+i < MATRIZ_ANCHO) ) &&
	                    ( (p_serpiente->y[0]+i >= 0) && (p_serpiente->y[0]+i < MATRIZ_ALTO) )){
	                p_pantalla->matriz[p_serpiente->x[i+1]][p_serpiente->y[i+1]] = 1;
	                p_serpiente->x_antigua = p_serpiente->x[i+1];

	    	}*/
	    	  //}
	    int i = 0;

	    	p_pantalla->matriz[p_serpiente->x[0]][p_serpiente->y[0]] = 1;

	    	for(i=0;i<p_serpiente->alto - 1;i++) {
	    		p_serpiente->x_antigua1 = p_serpiente->x[i+1];
	    		p_serpiente->y_antigua1 = p_serpiente->y[i+1];
	    		p_serpiente->x[i+1] = p_serpiente->x_antigua;
	    		p_serpiente->y[i+1] = p_serpiente->y_antigua;
	    		    if (( (p_serpiente->x[0]+i >= 0) && (p_serpiente->x[0]+i < MATRIZ_ANCHO) ) &&
	    		        ( (p_serpiente->y[0]+i >= 0) && (p_serpiente->y[0]+i < MATRIZ_ALTO) )){
	    		        p_pantalla->matriz[p_serpiente->x[i+1]][p_serpiente->y[i+1]] = 1;
	    		        p_serpiente->x_antigua = p_serpiente->x_antigua1;
	    		        p_serpiente->y_antigua = p_serpiente->y_antigua1;
	    		    }
	    	}
	    	p_serpiente->x_antigua = p_serpiente->x[0];
	    	p_serpiente->y_antigua = p_serpiente->y[0];
}

void PintaFruta(tipo_fruta *p_fruta, tipo_pantalla *p_pantalla) {
	/*if( (p_fruta->x >= 0) && (p_fruta->x < MATRIZ_ANCHO) ) {
		if( (p_fruta->y >= 0) && (p_fruta->y < MATRIZ_ALTO) ) {
			p_pantalla->matriz[p_fruta->x][p_fruta->y] = 1;
		}
		else if(p_fruta->y == -1){
			printf("\n\nLa posicion y=%d de la fruta indica que no hay ninguna por pantalla\n\n", p_fruta->y);
			fflush(stdout);
		}
		else {
			printf("\n\nPROBLEMAS!!!! posicion y=%d de la fruta INVALIDA!!!\n\n", p_fruta->y);
			fflush(stdout);
		}
	}
	else if(p_fruta->x == -1){
		printf("\n\nLa posicion x=%d de la fruta indica que no hay ninguna por pantalla\n\n", p_fruta->y);
		fflush(stdout);
	}
	else {
		printf("\n\nPROBLEMAS!!!! posicion x=%d de la fruta INVALIDA!!!\n\n", p_fruta->x);
		fflush(stdout);
	}*/
	p_pantalla->matriz[p_fruta->x][p_fruta->y] = 1;
}

/**
 * @brief	método cuya ejecucion estara ligada a cualquiera de los movimientos de la raqueta o de la pelota y que
 * 			será el encargado de actualizar convenientemente la estructura de datos
 * 			en memoria que representa el área de juego y su correspondiente estado
 * @param	p_arkanoPi
 */
void ActualizaPantallaSnake(tipo_snakePi* p_snakePi) {
    // Borro toda la pantalla
	ReseteaMatriz((tipo_pantalla*)(&(p_snakePi->pantalla)));

	// Completo la pantalla con los elementos correspondientes
	PintaSerpiente((tipo_serpiente*)(&(p_snakePi->serpiente)), (tipo_pantalla*)(&(p_snakePi->pantalla)));
	PintaFruta((tipo_fruta*)(&(p_snakePi->fruta)), (tipo_pantalla*)(&(p_snakePi->pantalla)));
}

/**
 * @brief	método encargado de la inicialización de toda variable o estructura de datos especificamente ligada al
 * 			desarrollo del juego y su visualizacion
 * @param	p_arkanoPi
 */
void InicializaSnakePi(tipo_snakePi *p_snakePi) {
	// Borro toda la pantalla
	ReseteaMatriz((tipo_pantalla*)(&(p_snakePi->pantalla)));

	// Completo la pantalla con los elementos correspondientes en sus posiciones originales
	ReseteaSerpiente((tipo_serpiente*)(&(p_snakePi->serpiente)));
	ReseteaFruta((tipo_fruta*)(&(p_snakePi->fruta)));

	// Se procede a mostrar el mensaje inicial
	PintaPantallaPorTerminal((tipo_pantalla*)(&(p_snakePi->pantalla)));
}

