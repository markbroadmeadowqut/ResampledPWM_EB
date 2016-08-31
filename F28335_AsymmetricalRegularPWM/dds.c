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

volatile Uint32 phaseInc = 0x61847F5L;	// = 102254581 (f0 = 59.52Hz = 102254581/(2^32*0.4e-3) )
volatile Uint32 phaseAccumulator = 0x61847F5L; // Initialise phaseAccumulator at 1*phaseInc so that 2*phaseInc is loaded into phaseAccumulator by the end of the first call to isr_cpu_timer0; that way the modulating signal has correct phase for the second and all subsequent calls to isr_cpu_timer0

void generateTable() {
	Uint16 i;
	
	for (i = 0; i < 1024; i++) {
		ddsTable[i] = floor((0.5*SWTBPRD*0.8*sin(2*PI*i/1024.0))+(0.5*SWTBPRD)+0.5); // Note that the scaling by 18750 rather than 32768 is due to the fact that the carrier waveform effectively only varies between 0 and TBPRD rather than 0 and 2^16
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


