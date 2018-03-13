/** File name	: arkanoPiLib.c
 *  Description	: Funcionalidades referidas a la matriz, los ladrillos, la raqueta y la pelota
 */

/*Includes---------------------------------------*/
#include "arkanoPiLib.h"

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

tipo_pantalla mensaje_inicial = {
			{{0,0,0,0,0,0,0},
			{0,0,0,0,0,0,0},
			{0,1,1,1,1,1,0},
			{0,1,1,1,1,1,0},
			{0,1,1,0,1,0,0},
			{0,1,1,0,1,0,0},
			{0,1,1,1,1,1,0},
			{0,1,1,1,1,1,0},
			{0,0,0,0,0,0,0},
			{0,0,0,0,0,0,0},
}};

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

tipo_pantalla ladrillos_nv2 = {
				{{3,2,0,0,0,0,0},
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
				{{4,3,0,0,0,0,0},
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
 * @brief	método encargado de mostrar el contenido o la ocupaci�n de la matriz de leds en la ventana de terminal o consola
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
 * @brief	método encargado de pintar los ladrillos en sus correspondientes posiciones dentro del �rea de juego
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
 * @brief	método encargado de pintar la raqueta en su posici�n correspondiente dentro del �rea de juego
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
 * @brief	método encargado de pintar la pelota en su posici�n correspondiente dentro del �rea de juego
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
void ActualizaPantalla(tipo_arkanoPi* p_arkanoPi) {
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
