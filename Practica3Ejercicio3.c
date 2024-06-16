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
 * @file    Practica3Ejercicio3.c
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

/*
 * @brief   Application entry point.
 */

typedef enum{APAGADO, PARPADEA}MEF;
#define DIFERIDO 150000;


int main(void) {

    /* Init board hardware. */
    BOARD_InitBootPins();
    BOARD_InitBootClocks();
    BOARD_InitBootPeripherals();
#ifndef BOARD_INIT_DEBUG_CONSOLE_PERIPHERAL
    /* Init FSL debug console. */
    BOARD_InitDebugConsole();
#endif

    /* LED VERDE PORT D 	5*/
    SIM->SCGC5 |= 1 << 12;		// CLOCK
    PORTD->PCR[5] |= 1 << 8;	// PIN MUX CONTROL
    PTD->PCOR |= 1 << 5;		// INICIALIZACION
    PTD->PDDR |= 1 << 5;		// SETEA COMO SALIDA

    /* LED ROJO PORT E 	29*/
    SIM->SCGC5 |= 1 << 13;		// CLOCK
    PORTE->PCR[29] |= 1 << 8;	// PIN MUX CONTROL
    PTE->PCOR |= 1 << 29;		// INICIALIZACION
    PTE->PDDR |= 1 << 29;		// SETEA COMO SALIDA

    /* SWITCH1 PORT C	3*/
    SIM->SCGC5 |= 1 << 11;		// CLOCK
    PORTC->PCR[3] |= 1 << 8;	// PIN MUX CONTROL
    PORTC->PCR[3] |= 3;			// INICIALIZA PULLS
    PTC->PCOR |= 1 << 3;		// INICIALIZACION

    /* DEFINICION DE ESTADOS */
    MEF estado_actual = APAGADO;
    MEF proximo_estado = APAGADO;
    /* TIEMPO */
    uint32_t tiempo;
    /* APAGA LOS LEDS*/
    PTD->PTOR |= 1 << 5;	// APAGA LED VERDE
    PTE->PTOR |= 1 << 29;	// APAGA LED ROJO
    while(1) {
        switch(estado_actual){
        case APAGADO:
        	if(!(PTC->PDIR & (1<<3))){	// esto es igual a PTC->PDIR & (1<<3) == 0
        		PTE->PTOR |= 1 << 29;		// prendo el led rojo
        		tiempo = DIFERIDO;			// defino el tiempo de retardo
        		proximo_estado = PARPADEA;	// entro a PARPADEA
        	}
        	else{
        		PTD->PSOR |= 1 << 5;
        		PTE->PSOR |= 1 << 29;
        	}

        	break;
        case PARPADEA:
        	if(tiempo > 0){				// MIENTRAS SEA DISTINTO DE CERO
        		tiempo--;				// RESTO TIEMPO
        	}
        	if(tiempo == 0){			// SI LLEGA A CERO
        		PTE->PTOR |= 1 << 29;	// TOGGLE LED ROJO
        		PTD->PTOR |= 1 << 5;	// TOGGLE LED VERDE
        		tiempo = DIFERIDO;
        		//proximo_estado = APAGADO;
        	}
        	if(PTC->PDIR & (1<<3)){
        		proximo_estado = APAGADO;
        	}
        	// proximo_estado = APAGADO;
        	break;
        }
        if(estado_actual != proximo_estado){
        	estado_actual = proximo_estado;
        }
    }
    return 0 ;
}
