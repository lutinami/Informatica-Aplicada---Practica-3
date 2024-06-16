/*
 * Copyright 2016-2024 NXP
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * o Redistributions of source code must retain the above copyright notice, this list
 *   of conditions and the following disclaimer.
 *
 * o Redistributions in binary form must reproduce the above copyright notice, this
 *   list of conditions and the following disclaimer in the documentation and/or
 *   other materials provided with the distribution.
 *
 * o Neither the name of NXP Semiconductor, Inc. nor the names of its
 *   contributors may be used to endorse or promote products derived from this
 *   software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
 * ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

/**
 * @file    Practica3Ejercicio16.c
 * @brief   Application entry point.
 */
#include <stdio.h>
#include "board.h"
#include "peripherals.h"
#include "pin_mux.h"
#include "clock_config.h"
#include "MKL46Z4.h"
#include "fsl_debug_console.h"
/* TODO: insert other include files here. */
#include <time.h>
#include <stdlib.h>
/* TODO: insert other definitions and declarations here. */
typedef enum{APAGADO, COMIENZA_JUEGO, RESOLVER_PATRON, CHECK_SW1, CHECK_SW3, GAME_OVER}estados;
#define RETARDO 150000;
#define GOVER 50000;
/*
 * @brief   Application entry point.
 */
int main(void) {

	/* Init board hardware. */
	BOARD_InitBootPins();
	BOARD_InitBootClocks();
	BOARD_InitBootPeripherals();
#ifndef BOARD_INIT_DEBUG_CONSOLE_PERIPHERAL
	/* Init FSL debug console. */
	BOARD_InitDebugConsole();
#endif

	// LED VERDE
	SIM->SCGC5 |= 1<<12;
	PORTD->PCR[5] |= 1<<8;
	PTD->PCOR |= 1<<5;
	PTD->PDDR |= 1<<5;

	// LED ROJO
	SIM->SCGC5 |= 1<<13;
	PORTE->PCR[29] |= 1<<8;
	PTE->PCOR |= 1<<29;
	PTE->PDDR |= 1<<29;

	// SW1
	SIM->SCGC5 |= 1<<11;
	PORTC->PCR[3] |= 1<<8;
	PORTC->PCR[3] |= 3;

	// SW3
	SIM->SCGC5 |= 1<<11;
	PORTC->PCR[12] |= 1<<8;
	PORTC->PCR[12] |= 3;

	// APAGO LEDS
	PTE->PTOR |= 1<<29;
	PTD->PTOR |= 1<<5;

	// ESTADOS
	estados estado_actual = APAGADO;
	estados proximo_estado = APAGADO;

	// TIMERS
	srand(time(NULL));
	uint32_t tiempo = RETARDO;

	// CONTADORES
	uint32_t contador, cantidad_jugadas;
	contador = 0;
	cantidad_jugadas = 1;

	// ALMACENADORES DE PATRONES
	uint32_t* patron;
	patron = (uint32_t*)malloc(100*sizeof(uint32_t));	// ponemos como limite 100 partidas

	// ALMACENADOR DE ENTRADA Y SALIDA
	uint32_t  input, output;		// defino 0 como sw1 y 1 como sw3

	while(1) {
		switch(estado_actual){
		case APAGADO:
			if(!(PTC->PDIR & (1<<3))){
				proximo_estado = COMIENZA_JUEGO;
			}
			break;
		case COMIENZA_JUEGO:
			if(tiempo>0){
				tiempo--;
			}

			if(tiempo == 75000){
				input = rand() % 2;
				if(input == 0){
					PTD->PTOR |= 1<<5;
					patron[contador] = 0;
				}
				if(input == 1){
					PTE->PTOR |= 1<<29;
					patron[contador] = 1;
				}
			}

			if(tiempo == 0){
				if(input == 0){
					PTD->PTOR |= 1<<5;
					patron[contador] = 0;
				}
				if(input == 1){
					PTE->PTOR |= 1<<29;
					patron[contador] = 1;
				}
				contador++;
				tiempo = RETARDO;
			}

			if(contador == cantidad_jugadas){
				contador = 0;
				proximo_estado = RESOLVER_PATRON;
			}

			break;
		case RESOLVER_PATRON:
			if(!(PTC->PDIR & (1<<3))){
				output = 0;
				proximo_estado = CHECK_SW1;
			}

			if(!(PTC->PDIR & (1<<12))){
				output = 1;
				proximo_estado = CHECK_SW3;
			}

			if(contador == cantidad_jugadas){
				contador = 0;
				cantidad_jugadas++;
				proximo_estado = COMIENZA_JUEGO;
			}
			break;
		case CHECK_SW1:
			if(PTC->PDIR & (1<<3)){
				if(output == patron[contador]){
					contador++;
					proximo_estado = RESOLVER_PATRON;
				}
				else{
					tiempo = GOVER;
					contador = 0;
					PTD->PSOR |= 1<<5;
					PTE->PSOR |= 1<<29;
					proximo_estado = GAME_OVER;
				}
			}
			break;
		case CHECK_SW3:
			if(PTC->PDIR & (1<<12)){
				if(output == patron[contador]){
					contador++;
					proximo_estado = RESOLVER_PATRON;
				}
				else{
					tiempo = GOVER;
					contador = 0;
					PTD->PSOR |= 1<<5;
					PTE->PSOR |= 1<<29;
					proximo_estado = GAME_OVER;
				}
			}
			break;
		case GAME_OVER:
			if(tiempo>0){
				tiempo--;
			}

			if(tiempo == 25000){
				PTE->PTOR |= 1<<29;
			}

			if(tiempo == 0){
				PTE->PTOR |= 1<<29;
				contador++;
				tiempo = GOVER;
			}

			if(contador == 3){
				PTD->PSOR |= 1<<5;
				PTE->PSOR |= 1<<29;
				contador = 0;
				cantidad_jugadas = 1;
				free(patron);
				patron = (uint32_t*)malloc(100*sizeof(uint32_t));
				tiempo = RETARDO;
				proximo_estado = APAGADO;
			}
			break;
		}
		if(estado_actual != proximo_estado){
			estado_actual = proximo_estado;
		}
	}
	return 0 ;
}
