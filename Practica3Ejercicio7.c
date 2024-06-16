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
 * @file    Practica3Ejercicio7.c
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
typedef enum{APAGADO, ENCENDIDO}muf;
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
	/* LED ROJO PORT E 29 */
	SIM->SCGC5 |= 1<<13;	//CLOCK
	PORTE->PCR[29] |= 1<<8;	//MUX
	PTE->PCOR |= 1 << 29;	//INCIALIZA
	PTE->PDDR |= 1 << 29;	//SALIDA

	/* SWITCH 1 PORT C 3*/
	SIM->SCGC5 |= 1 << 11;	//CLOCK
	PORTC->PCR[3] |= 1<<8;	//MUX
	PORTC->PCR[3] |= 3;		//PULLS

	/* SWITCH 3 PORT C 12 */
	SIM->SCGC5 |= 1 << 11;	//CLOCK
	PORTC->PCR[12] |= 1<<8;
	PORTC->PCR[12] |= 3;

	muf estado_actual = APAGADO;
	muf proximo_estado = APAGADO;
	PTE->PTOR |= 1<<29;

	while(1) {
		switch(estado_actual){
		case APAGADO:
			if(!(PTC->PDIR & (1<<3))){
				PTE->PTOR |= 1<<29;
				proximo_estado = ENCENDIDO;
			}
			break;
		case ENCENDIDO:
			if(!(PTC->PDIR & (1<<12))){
				PTE->PTOR |= 1<<29;
				proximo_estado = APAGADO;
			}
			break;
		}
		if (estado_actual != proximo_estado){
			estado_actual = proximo_estado;
		}
	}
	return 0 ;
}
