/*
 * dds.c
 *
 *  Created on: 04/08/2016
 *      Author: broadmea
 */
#include "DSP28x_Project.h"
#include "config.h"

#include <math.h>

Uint16 ddsTable[1024];

volatile Uint32 phaseInc = (Uint32) 0x0624DD3;	// = 6442451 (f0 = 60Hz = 6442451/(2^32*25e-6) )
volatile Uint32 phaseAccumulator = (Uint32) 0x0624DD3; // Initialise phaseAccumulator at 1*phaseInc so that 2*phaseInc is loaded into phaseAccumulator by the end of the first call to isr_cpu_timer0; that way the modulating signal has correct phase for the second and all subsequent calls to isr_cpu_timer0

void generateTable() {
	Uint16 i;

	Uint16 x = swTBPRD/2;
	
	for (i = 0; i < 1024; i++) {
		ddsTable[i] = (x*0.8*sin(2*PI*i/1024.0))+x; // Note that the scaling by 18750 rather than 32768 is due to the fact that the carrier waveform effectively only varies between 0 and TBPRD rather than 0 and 2^16
	}
}

void initDDS() {
	generateTable();
	phaseAccumulator = 0;
}

#pragma CODE_SECTION(serviceDDS, "ramfuncs");
Uint16 serviceDDS() {
	phaseAccumulator += phaseInc;

	return ddsTable[(phaseAccumulator>>22)&0x3FF];	// For 10-bit table, top 10 bits of accumulator are index
}


