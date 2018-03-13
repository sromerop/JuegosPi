/** File name	: snakePiLib.c
 *  Description	: Funcionalidades referidas a la matriz, la serpiente y las frutas
 */

/*Includes---------------------------------------*/
#include "snakePiLib.h"

/*Private variables------------------------------*/
tipo_pantalla mensaje_inicial = {
			{
			{0,0,0,0,0,0,0},
			{0,0,0,0,0,0,0},
			{0,0,1,0,0,1,0},
			{0,1,0,1,0,1,0},
			{0,1,0,1,0,1,0},
			{0,1,0,1,0,1,0},
			{0,1,0,1,0,1,0},
			{0,1,0,0,1,0,0},
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
 * @brief	m�todo que restablece la serpiente a su estado inicial
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
void PintaPantallaPorTerminal(tipo_pantalla *p_pantalla) {
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
 * @brief	m�todo encargado de pintar la serpiente en su posici�n correspondiente dentro del �rea de juego
 * @param	p_serpiente
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
/**
 * @brief	m�todo encargado de pintar la fruta en su posici�n correspondiente dentro del �rea de juego
 * @param	p_fruta
 * @param	p_pantalla
 */
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
 * @brief	m�todo encargado de dar el parámetro posición a la fruta una vez se inicia el juego (o se resetea)
 * @param	p_fruta
 */
void ReseteaFruta(tipo_fruta *p_fruta){
	p_fruta->x = MATRIZ_ANCHO - 7;
	p_fruta->y = MATRIZ_ALTO - 2;
}
/**
 * @brief	m�todo encargado de dar el parámetro posición a la fruta de manera aleatoria
 * @param	p_fruta
 */
void ReseteaFrutaAleatorio(tipo_fruta *p_fruta){
//void ReseteaFrutaAleatorio(tipo_fruta *p_fruta, tipo_pantalla *p_pantalla){
	int x = rand() % MATRIZ_ANCHO;
	int y = rand() % MATRIZ_ALTO;
	
	/*while (p_pantalla->matriz[x][y] == 1){
		x = rand() % MATRIZ_ANCHO;
		y = rand() % MATRIZ_ALTO;
	}*/
	
	p_fruta->x = x;
	p_fruta->y = y;
}

/**
 * @brief	m�todo cuya ejecucion estara ligada a cualquiera de los movimientos de la raqueta o de la pelota y que
 * 			ser� el encargado de actualizar convenientemente la estructura de datos
 * 			en memoria que representa el �rea de juego y su correspondiente estado
 * @param	p_arkanoPi
 */
void ActualizaPantalla(tipo_snakePi* p_snakePi) {
    // Borro toda la pantalla
	ReseteaMatriz((tipo_pantalla*)(&(p_snakePi->pantalla)));

	// Completo la pantalla con los elementos correspondientes
	PintaSerpiente((tipo_serpiente*)(&(p_snakePi->serpiente)), (tipo_pantalla*)(&(p_snakePi->pantalla)));
	PintaFruta((tipo_fruta*)(&(p_snakePi->fruta)), (tipo_pantalla*)(&(p_snakePi->pantalla)));
}

/**
 * @brief	m�todo encargado de la inicializaci�n de toda variable o estructura de datos especificamente ligada al
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
