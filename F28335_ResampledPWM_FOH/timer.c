/*
 * timer.c
 *
 *  Created on: 22/05/2015
 *      Author: broadmea
 */
#include "DSP28x_Project.h"
#include "config.h"
#include "dds.h"

volatile static Uint16 newSample = 0x0000;

interrupt void isr_cpu_timer0(void);

void initTimer() {
	InitCpuTimers();
	ConfigCpuTimer(&CpuTimer0, 150, 30); 			// Set 30us period (at 150 MHz)
	CpuTimer0Regs.TCR.all = 0x4001; 				// Start CPU Timer0. Use write-only instruction to set TSS bit = 0

	EALLOW;
	PieVectTable.TINT0 = &isr_cpu_timer0;
	EDIS;

	PieCtrlRegs.PIEIER1.bit.INTx7 = 1;     // PIE Group 1, INT7, TINT0

	IER |= M_INT1;
}

#pragma CODE_SECTION(isr_cpu_timer0, "ramfuncs");
interrupt void isr_cpu_timer0(void)
{
	//CpuTimer0.InterruptCount++;

	// Sample ePWM1 counter
	// Perform FOH calculations
	// Update CMPA register

	newSample = serviceDDS();	// Precompute sample for next cycle

	// Acknowledge this interrupt to receive more interrupts from group 1
	PieCtrlRegs.PIEACK.all = PIEACK_GROUP1;
}
