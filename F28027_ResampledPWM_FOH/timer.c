/*
 * timer.c
 *
 *  Created on: 22/05/2015
 *      Author: broadmea
 */
#include "DSP28x_Project.h"
#include "config.h"

volatile Uint32 newSample = 0;
volatile Uint32 prevSample = 0;

interrupt void isr_cpu_timer0(void);

void initTimer() {
	InitCpuTimers();
	ConfigCpuTimer(&CpuTimer0, 150, 80); 			// Set clock to 150MHz, timer period to 25us period so that fs=10kHz (would be good to make this automatically updated with fs in config.h)
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
	char swCTRDIR;
	Uint16 swTBCTR;
	Uint32 calc1;
	Uint32 ti;
	Uint16 CMPA_value;

	//CpuTimer0.InterruptCount++;
	
	swTBCTR = EPwm1Regs.TBCTR;   			// Sample EPwm1 counter (Is this the correct way to read from address 0x0004??)
	swCTRDIR = EPwm1Regs.TBSTS.bit.CTRDIR;
	
	//DEBUG
	//GpioDataRegs.GPASET.bit.GPIO2 = 1;

	// Update CMPA register (and perform FOH calcs in the same line; hopefully it is faster to do it this way?)
	if(swCTRDIR) {
		calc1=((newSample-prevSample)*FOH_SCALE3)-FOH_SCALE4;
		ti=(swTBCTR-newSample)/calc1;
		//CMPA_value=floor((swTBCTR+(FOH_SCALE4*ti))+0.5);
		CMPA_value=swTBCTR+(FOH_SCALE4*ti);
		EPwm1Regs.CMPA.half.CMPA=CMPA_value;
		//DEBUG
		//GpioDataRegs.GPACLEAR.bit.GPIO2 = 1;
		if(CMPA_value<(swTBCTR+300)) {
			EALLOW;
			EPwm1Regs.AQSFRC.bit.ACTSFA = 1;	// Set output low on software force
			EDIS;
			EPwm1Regs.AQSFRC.bit.OTSFA = 1;
		}
	} else {
		calc1=((newSample-prevSample)*FOH_SCALE3)+FOH_SCALE4;
		ti=(swTBCTR-newSample)/calc1;
		CMPA_value=swTBCTR-(FOH_SCALE4*ti);
		EPwm1Regs.CMPA.half.CMPA=CMPA_value;
		//DEBUG
		//GpioDataRegs.GPACLEAR.bit.GPIO2 = 1;
		if(CMPA_value>(swTBCTR-300)) {
			EALLOW;
			EPwm1Regs.AQSFRC.bit.ACTSFA = 0;	// Set output high on software force
			EDIS;
			EPwm1Regs.AQSFRC.bit.OTSFA = 1;
		}
	}
		

	// DEBUG
	//EPwm1Regs.CMPA.half.CMPA=newSample;


	prevSample = newSample;       // Sample the modulating waveform at this point in the interrupt so that the write to CMPA happens sooner
	newSample = 30000;	// Precompute sample for the next triggering of this interrupt
	// Should make sure that this ISR gets first priority if there are any other ISRs that might be called at the same time or before it.
			
	// Acknowledge this interrupt to receive more interrupts from group 1
	PieCtrlRegs.PIEACK.all = PIEACK_GROUP1;
}
