/*
 * timer.c
 *
 *  Created on: 22/05/2015
 *      Author: broadmea
 */
#include "DSP28x_Project.h"
#include "config.h"


void initTimer() {

	InitCpuTimers();
	//double timerPrd = 1000000/(double)fs;
	ConfigCpuTimer(&CpuTimer0, 60, 12.5); 			// Set clock to 60MHz, timer period to 10us
	CpuTimer0Regs.TCR.all = 0x4001; 				// Start CPU Timer0. Use write-only instruction to set TSS bit = 0

	//DEBUG
	GpioDataRegs.GPACLEAR.bit.GPIO2 = 1;
	EALLOW;
	GpioCtrlRegs.GPADIR.bit.GPIO2 = 1;				// Set GPIO2 as output
	EDIS;

	// Assuming that CPU Timer 0 doesn't need to trigger an ISR when acting as a trigger for the ADC? Just needs to be counting?
}
