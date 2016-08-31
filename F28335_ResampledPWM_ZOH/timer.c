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
	ConfigCpuTimer(&CpuTimer0, 150, 80); 			// Set clock to 150MHz, timer period to 80us period so that fs=12.5kHz 
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
     	Uint16 swCTRDIR;
		Uint16 swTBCTR;
		Uint16 CMPA_value;
		float32 newSample;

		//CpuTimer0.InterruptCount++;

		swTBCTR = EPwm1Regs.TBCTR;   			// Sample EPwm1 counter (Is this the correct way to read from address 0x0004??)
		swCTRDIR = EPwm1Regs.TBSTS.bit.CTRDIR;

		//DEBUG
		//GpioDataRegs.GPASET.bit.GPIO2 = 1;

		newSample = serviceDDS();
		
		// Update CMPA register (and perform FOH calcs in the same line; hopefully it is faster to do it this way?)
		if(swCTRDIR) {
			CMPA_value=newSample;
			EPwm1Regs.CMPA.half.CMPA=CMPA_value;
			//DEBUG
			//GpioDataRegs.GPACLEAR.bit.GPIO2 = 1;
			if(CMPA_value<(swTBCTR+1)) {
				EALLOW;
				EPwm1Regs.AQSFRC.bit.ACTSFA = 1;	// Set output low on software force
				EDIS;
				EPwm1Regs.AQSFRC.bit.OTSFA = 1;
			}
		} else {
			CMPA_value=newSample;
			EPwm1Regs.CMPA.half.CMPA=CMPA_value;
			//DEBUG
			//GpioDataRegs.GPACLEAR.bit.GPIO2 = 1;
			if(CMPA_value>(swTBCTR-1)) {
				EALLOW;
				EPwm1Regs.AQSFRC.bit.ACTSFA = 0;	// Set output high on software force
				EDIS;
				EPwm1Regs.AQSFRC.bit.OTSFA = 1;
			}
		}

		
		// Acknowledge this interrupt to receive more interrupts from group 1
		PieCtrlRegs.PIEACK.all = PIEACK_GROUP1;
}
