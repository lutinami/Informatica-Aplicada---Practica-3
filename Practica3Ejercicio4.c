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
 * @file    Practica3Ejercicio4.c
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
typedef enum{OFF, AND, OR, XOR}MEF;


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
	/* LED VERDE */
	SIM->SCGC5 |= 1 << 12;
	PORTD->PCR[5] |= 1 << 8;
	PTD->PCOR |= 1 << 5;
	PTD->PDDR |= 1 << 5;
	/* LED ROJO */
	SIM->SCGC5 |= 1 << 13;
	PORTE->PCR[29] |= 1 << 8;
	PTE->PCOR |= 1 << 29;
	PTE->PDDR |= 1 << 29;
	/* SWITCH 1 */
	SIM->SCGC5 |= 1 << 11;
	PORTC->PCR[3] |= 1 << 8;
	PORTC->PCR[3] |= 3;

	/* SWITCH 3 */
	SIM->SCGC5 |= 1 << 11;
	PORTC->PCR[12] |= 1 << 8;
	PORTC->PCR[12] |= 3;

	/* ESTADOS */
	MEF estado_actual = OFF;
	MEF proximo_estado = OFF;
	uint32_t flag = 0;

	PTE->PTOR |= 1 << 29;
	PTD->PTOR |= 1 << 5;

	while(1) {
		switch(estado_actual){
		case OFF:	//NOT
			// AND (AMBOS LEDS)
			if((!(PTC->PDIR & (1<<3))) && (!(PTC->PDIR & (1<<12)))){
				PTD->PTOR |= 1 << 5;
				PTE->PTOR |= 1 << 29;
				proximo_estado = AND;
			}
			// OR (LED VERDE)
			if((!(PTC->PDIR & (1<<3))) || (!(PTC->PDIR & (1<<12)))){
				if(!(PTC->PDIR & (1<<3))){
					flag = 1;
				}
				PTD->PSOR |= 1 << 5;
				proximo_estado = OR;
			}
			// XOR (LED ROJO)
			/*if((!(PTC->PDIR & (1<<3))) ^ (!(PTC->PDIR & (1<<12)))){
				proximo_estado = XOR;
				PTE->PSOR |= 1 << 29;
			}*/
			break;
		case AND:
			if((PTC->PDIR & (1<<3)) && (PTC->PDIR & (1<<12))){
				PTD->PTOR |= 1 << 5;
				PTE->PTOR |= 1 << 29;
				proximo_estado = OFF;
			}
			break;
		case OR:
			if(flag == 1){
				if(PTC->PDIR & (1<<3)){
					PTD->PCOR |= 1 << 5;
					proximo_estado = OFF;
				}
			}
			else{
				if(PTC->PDIR & (1<<12)){
					PTD->PCOR |= 1 << 5;
				}
			}
			break;
		/*case XOR:
			if((PTC->PDIR & (1<<3)) ^ (PTC->PDIR & (1<<12))){
				PTE->PCOR |= 1 << 29;
				proximo_estado = OFF;
			}*/
		}
		if(estado_actual != proximo_estado){
			estado_actual = proximo_estado;
		}
	}
	return 0 ;
}
