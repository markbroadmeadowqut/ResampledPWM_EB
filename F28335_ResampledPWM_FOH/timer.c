/*
 * timer.c
 *
 *  Created on: 22/05/2015
 *      Author: broadmea
 */
#include "DSP28x_Project.h"
#include "config.h"
#include "dds.h"
#include "math.h"

volatile static Uint16 newSample = 0x0000;
volatile static Uint16 prevSample = 0x0000;

interrupt void isr_cpu_timer0(void);

void initTimer() {
	InitCpuTimers();
	//double timerPrd = 1000000/(double)fs;
	ConfigCpuTimer(&CpuTimer0, 150, 25); 			// Set clock to 150MHz, timer period to 25us period so that fs=40kHz (would be good to make this automatically updated with fs in config.h) 
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
	
	swTBCTR = EPwm1Regs.TBCTR.all;   	// Sample EPwm1 counter (Is this the correct way to read from address 0x0004??)
	swCTRDIR = EPwm1Regs.TBSTS.bit.CTRDIR;
	
	
	// Update CMPA register (and perform FOH calcs in the same line; hopefully it is faster to do it this way?)
	if( swCTRDIR == 1 ) {
		double calc1 = (double)fs/TBCLK;
		double calc2 = (newSample-prevSample)*(calc1-1.0)
		double calc3 = (swTBCTR-newSample)/calc2+swTBCTR;
		EPwm1Regs.CMPA.all = roundl(calc3);
	}	
	else if( swCTRDIR == 0) {
		double calc1 = (double)fs/TBCLK;
		double calc2 = (-newSample+prevSample)*(calc1-1.0)
		double calc3 = (swTBCTR-newSample)/calc2+swTBCTR;
		EPwm1Regs.CMPA.all = roundl(calc3);		
	}
		
	prevSample = newSample;       // Sample the modulating waveform at this point in the interrupt so that the write to CMPA happens sooner
	newSample = serviceDDS();	// Precompute sample for the next triggering of this interrupt
	// Should make sure that this ISR gets first priority if there are any other ISRs that might be called at the same time or before it.
			
		
	// Acknowledge this interrupt to receive more interrupts from group 1
	PieCtrlRegs.PIEACK.all = PIEACK_GROUP1;
}
