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
 * @file    Practica3Ejercicio11.c
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

/* TODO: insert other definitions and declarations here. */
typedef enum{APAGADO, PARPADEA}mef;	// ESTADOS DE LA MEF
#define RETARDO 150000;

void parpadeo_intercalado(uint32_t retardo);			// FUNCION PARPADEO
uint32_t check_sw1(uint32_t band_sw1);
uint32_t check_sw3(uint32_t band_sw3);
uint32_t velocidad_aumenta(uint32_t retardo, uint32_t *band_sw1);
uint32_t velocidad_disminuye(uint32_t retardo, uint32_t *band_sw3);

/*mef sw1_presionado(mef estado_actual, mef proximo_estado, uint32_t *band_sw1);
uint32_t check_sw1();
uint32_t check_sw2();
void aumenta_retardo();
void disminuye_retardo();*/

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

	// LED VERDE PORT D PIN 5
	SIM->SCGC5 |= 1<<12;
	PORTD->PCR[5] |= 1<<8;
	PTD->PCOR |= 1<<5;
	PTD->PDDR |= 1<<5;

	// LED ROJO PORT E PIN 29
	SIM->SCGC5 |= 1<<13;
	PORTE->PCR[29] |= 1<<8;
	PTE->PCOR |= 1<<29;
	PTE->PDDR |= 1<<29;

	// SWITCH 1 PORT C PIN 3
	SIM->SCGC5 |= 1<<11;
	PORTC->PCR[3] |= 1<<8;
	PORTC->PCR[3] |= 3;

	// SWITCH 3 PORT C PIN 12
	SIM->SCGC5 |= 1<<11;
	PORTC->PCR[12] |= 1<<8;
	PORTC->PCR[12] |= 3;

	// ESTADOS
	mef estado_actual = APAGADO;
	mef proximo_estado = APAGADO;

	// APAGA LOS LEDS
	PTE->PTOR |= 1<<29;
	PTD->PTOR |= 1<<5;

	// BANDERAS
	uint32_t band_sw1 = 0;
	uint32_t band_sw3 = 0;

	// RETARDO
	uint32_t retardo = 150000;

	while(1) {
		switch(estado_actual){
		case APAGADO:
			if(!(PTC->PDIR & (1<<3))){
				band_sw1 = 1;
				PTE->PTOR |= 1<<29;
				proximo_estado = PARPADEA;
			}
			break;
		case PARPADEA:
			// parpadeo intercalado de los leds
			parpadeo_intercalado(retardo);
			// revisa si no esta presionado cada switch
			band_sw1 = check_sw1(band_sw1);
			band_sw3 = check_sw3(band_sw3);

			if(!(PTC->PDIR & (1<<3)) && (band_sw1 == 0)){
				band_sw1 = 1;
				if(retardo > 20000){
					retardo -= 10000;
				}
			}

			if(!(PTC->PDIR & (1<<12)) && (band_sw3 == 0)){
				band_sw3 = 1;
				retardo += 10000;
			}

			break;
		}
		if(estado_actual != proximo_estado){
			estado_actual = proximo_estado;
		}
	}
	return 0 ;
}

void parpadeo_intercalado(uint32_t retardo){
	static uint32_t tiempo = RETARDO;

	if(tiempo>0){
		tiempo--;
	}

	if(tiempo == 0){
		PTE->PTOR |= 1<<29;
		PTD->PTOR |= 1<<5;
		tiempo = retardo;
	}
}

uint32_t check_sw1(uint32_t band_sw1){
	if((PTC->PDIR & (1<<3)) && (band_sw1 == 1)){
		return 0;
	}
	else{
		return band_sw1;
	}
}

uint32_t check_sw3(uint32_t band_sw3){
	if((PTC->PDIR & (1<<12)) && (band_sw3 == 1)){
		return 0;
	}
	else{
		return band_sw3;
	}
}

uint32_t velocidad_aumenta(uint32_t retardo, uint32_t *band_sw1){
	if(!(PTC->PDIR & (1<<3)) && (band_sw1 == 0)){
		*band_sw1 = 1;
		if(retardo > 20000){
			retardo -= 10000;
		}
	}
	return retardo;
}

uint32_t velocidad_disminuye(uint32_t retardo, uint32_t *band_sw3){
	if(!(PTC->PDIR & (1<<3)) && (band_sw3 == 0)){
		*band_sw3 = 1;
		retardo += 10000;
	}
	return retardo;
}

/*mef sw1_presionado(mef estado_actual, mef proximo_estado, uint32_t *band_sw1){
	if(!(PTC->PDIR & (1<<3))){
 *band_sw1 = 1;
		PTE->PTOR |= 1<<29;
		return proximo_estado;
	}
	else{
		return estado_actual;
	}
}*/
