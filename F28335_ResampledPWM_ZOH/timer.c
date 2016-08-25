/*
 * timer.c
 *
 *  Created on: 22/05/2015
 *      Author: broadmea
 */
#include "DSP28x_Project.h"
#include "config.h"
#include "dds.h"
#include <math.h>

interrupt void isr_cpu_timer0(void);

void initTimer() {
	InitCpuTimers();
	//double timerPrd = 1000000/(double)fs;
	ConfigCpuTimer(&CpuTimer0, 150, 25); 			// Set clock to 150MHz, timer period to 25us period so that fs=40kHz (would be good to make this automatically updated with fs in config.h) 
	CpuTimer0Regs.TCR.all = 0x4001; 				// Start CPU Timer0. Use write-only instruction to set TSS bit = 0

	//DEBUG
	GpioDataRegs.GPACLEAR.bit.GPIO2 = 1;
	EALLOW;
	GpioCtrlRegs.GPADIR.bit.GPIO2 = 1;				// Set GPIO2 as output
	EDIS;

	EALLOW;
	PieVectTable.TINT0 = &isr_cpu_timer0;
	EDIS;

	PieCtrlRegs.PIEIER1.bit.INTx7 = 1;     // PIE Group 1, INT7, TINT0

	IER |= M_INT1;
}

#pragma CODE_SECTION(isr_cpu_timer0, "ramfuncs");
interrupt void isr_cpu_timer0(void)
{

	float32 newSample;

	//CpuTimer0.InterruptCount++;
		
	//DEBUG
	//GpioDataRegs.GPASET.bit.GPIO2 = 1;

	newSample = serviceDDS();	// Precompute sample for the next triggering of this interrupt
		
	// Enter the ZOH value into the register
	EPwm1Regs.CMPA.half.CMPA=newSample;
			
	// Acknowledge this interrupt to receive more interrupts from group 1
	PieCtrlRegs.PIEACK.all = PIEACK_GROUP1;
}
