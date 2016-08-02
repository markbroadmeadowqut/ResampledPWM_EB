/*
 * timer.c
 *
 *  Created on: 22/05/2015
 *      Author: broadmea
 */
#include "DSP28x_Project.h"
#include "config.h"

interrupt void isr_cpu_timer0(void);

void initTimer() {
	InitCpuTimers();
	ConfigCpuTimer(&CpuTimer0, 100, 1000); 			// Set 1mS period (at 100 MHz)
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
	CpuTimer0.InterruptCount++;

	// Do something on timer interrupt

	// Acknowledge this interrupt to receive more interrupts from group 1
	PieCtrlRegs.PIEACK.all = PIEACK_GROUP1;
}
