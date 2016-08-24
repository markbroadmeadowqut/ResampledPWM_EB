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

interrupt void EPwm1_isr(void);

void initTimer() {

	//DEBUG
	GpioDataRegs.GPACLEAR.bit.GPIO2 = 1;
	EALLOW;
	GpioCtrlRegs.GPADIR.bit.GPIO2 = 1;				// Set GPIO2 as output
	EDIS;

	EALLOW;
	PieVectTable.EPWM1_INT = &EPwm1_isr;
	EDIS;

	PieCtrlRegs.PIEIER3.bit.INTx1 = 1;     // PIE Group 3, INT1, EPWM1_INT

	IER |= M_INT1;
}

#pragma CODE_SECTION(EPwm1_isr, "ramfuncs");
interrupt void EPwm1_isr(void) // Can I program what happens in this interrupt?
{
	Uint16 INTSEL_current;
	float32 newSample;	
	//CpuTimer0.InterruptCount++;

	//DEBUG
	//GpioDataRegs.GPASET.bit.GPIO2 = 1;
	
	newSample = serviceDDS();	// Precompute sample for the next triggering of this interrupt
	
	EPwm1Regs.CMPA.half.CMPA=newSample;
		
	// Toggle ETSEL[INTSEL] so that the interrupt will be triggered by the next appropriate event (i.e. TBCTR=0 or TBCTR=TBPRD)
	INTSEL_current = EPwm1Regs.ETSEL.bit.INTSEL;
	if(INTSEL_current == 2) {  
		EPwm1Regs.ETSEL.bit.INTSEL = 1;
	} else {
		EPwm1Regs.ETSEL.bit.INTSEL = 2;
	}		
	
	// Acknowledge this interrupt to receive more interrupts from group 3
	PieCtrlRegs.PIEACK.all = PIEACK_GROUP3;
}
