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
 * @file    Practica3Ejercicio13.c
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
typedef enum{APAGADO, sw1PULSADO, sw3PULSADO, PARPADEO}estados;
#define RETARDO 150000;
/* TODO: insert other definitions and declarations here. */

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

	PRINTF("Hello World\n");

	/* LED VERDE PORT D 5 */
	SIM->SCGC5 |= 1<<12;
	PORTD->PCR[5] |= 1<<8;
	PTD->PCOR |= 1<<5;
	PTD->PDDR |= 1<<5;

	/* LED ROJO PORT E 29 */
	SIM->SCGC5 |= 1<<13;
	PORTE->PCR[29] |= 1<<8;
	PTE->PCOR |= 1<<29;
	PTE->PDDR |= 1<<29;

	/* SWITCH 1 PORT C 3 */
	SIM->SCGC5 |= 1<<11;
	PORTC->PCR[3] |= 1<<8;
	PORTC->PCR[3] |= 3;

	/* SWITCH 3 PORT C 12 */
	SIM->SCGC5 |= 1<<11;
	PORTC->PCR[12] |= 1<<8;
	PORTC->PCR[12] |= 3;

	uint32_t tiempo = 300000;
	estados estado_actual = APAGADO;
	estados proximo_estado = APAGADO;

	uint32_t contador, band_sw1, band_sw3;
	uint32_t patron[10];
	contador = 0;
	band_sw1 = 0;
	band_sw3 = 0;

	PTE->PTOR |= 1<<29;
	PTD->PTOR |= 1<<5;

	/* ASIGNO SW1 A LED VERDE Y SW3 A LED ROJO */
	while(1) {
		switch(estado_actual){
		case APAGADO:
			if(!(PTC->PDIR & (1<<3)) && (band_sw1 == 0)){
				band_sw1 = 1;
				proximo_estado = sw1PULSADO;
			}
			if(!(PTC->PDIR & (1<<12)) && (band_sw3 == 0)){
				band_sw3 = 1;
				proximo_estado = sw3PULSADO;
			}
			if(contador > 9){
				contador = 0;
				tiempo = 300000;
				proximo_estado = PARPADEO;
			}
			break;
		case sw1PULSADO:
			if((PTC->PDIR & (1<<3)) && (band_sw1 == 1)){
				patron[contador] = 1;
				contador++;
				band_sw1 = 0;
				proximo_estado = APAGADO;
			}
			break;
		case sw3PULSADO:
			if((PTC->PDIR & (1<<12)) && (band_sw3 == 1)){
				patron[contador] = 0;
				contador++;
				band_sw3 = 0;
				proximo_estado = APAGADO;
			}
			break;
		case PARPADEO:
			if(tiempo > 0){
				tiempo--;
			}
			if(tiempo == 150000){
				if(patron[contador] == 1){
					PTD->PTOR |= 1<<5;
				}
				if(patron[contador] == 0){
					PTE->PTOR |= 1<<29;
				}
			}
			if(tiempo == 0){
				if(patron[contador] == 1){
					PTD->PTOR |= 1<<5;
				}
				if(patron[contador] == 0){
					PTE->PTOR |= 1<<29;
				}
				tiempo = 300000;
				contador++;
			}
			if(contador > 9){
				contador = 0;
				PTE->PSOR |= 1<<29;
				PTD->PSOR |= 1<<5;
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
