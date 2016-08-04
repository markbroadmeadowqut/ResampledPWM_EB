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

volatile Uint32 phaseInc = (Uint32) 0x0F5C28F6;	// 2kHz @ 30us service interval (check this)
volatile Uint32 phaseAccumulator = 0;

void generateTable() {
	Uint16 i;

	for (i = 0; i < 1024; i++) {
		ddsTable[i] = (32768.0*0.8*sin(2*PI*i/1024.0))+32768.0;
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


