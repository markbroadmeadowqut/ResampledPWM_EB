/*
 * pwm.c
 *
 *  Created on: 05/08/2016
 *      Author: broadmea
 */
#include "DSP28x_Project.h"
#include "config.h"
#include "dds.h"

interrupt void isr_epwm1(void);

void initPWM() {
	

	EPwm1Regs.TBPRD = SWTBPRD; // fc = 1/(2*65536/TBCLK) = 1/(2*65535/150000000) = 1.1444kHz
	EPwm1Regs.TBPHS.all = 0; // Set Phase register to zero (don't plan to use synchronisation functionality at this stage)
	EPwm1Regs.TBCTR = 0; // clear TB counter
	
	EPwm1Regs.TBCTL.bit.CTRMODE = TB_FREEZE; // Make sure the counter is not counting yet when we are initialising EPwm1
	EPwm1Regs.TBCTL.bit.PHSEN = TB_DISABLE; // Phase loading disabled
	EPwm1Regs.TBCTL.bit.PRDLD = TB_IMMEDIATE; // Load the TBPRD register immediately without using a shadow register
	EPwm1Regs.TBCTL.bit.SYNCOSEL = TB_SYNC_DISABLE; // Disable EPwmxSYNCO signal
	EPwm1Regs.TBCTL.bit.HSPCLKDIV = TB_DIV1; // TBCLK = SYSCLKOUT
	EPwm1Regs.TBCTL.bit.CLKDIV = TB_DIV1; // TBCLK = SYSCLKOUT
	
	EPwm1Regs.CMPCTL.bit.SHDWAMODE = CC_SHADOW; // load CMPA only at the carrier peaks and troughs
	EPwm1Regs.CMPCTL.bit.LOADAMODE = CC_CTR_ZERO_PRD; // Load the shadow register into the active CMPA register at both the peaks and troughs of the carrier waveform
	
	EPwm1Regs.AQCTLA.bit.CAU = AQ_CLEAR; // I want my PWM signal to be CLEARED to low when the carrier is increasing and the modulating signal falls below it...
	EPwm1Regs.AQCTLA.bit.CAD = AQ_SET; // ...and to be SET to high when the carrier is decreasing and the modulating signal rises above it.   
	
	//EPwm1Regs.AQSFRC.bit.ACTSFA = 1;	// Set output low on software force

	EPwm1Regs.ETSEL.bit.INTEN = 1;
	EPwm1Regs.ETSEL.bit.INTSEL = 2;  // first ISR will occur when TBCTR = TBPRD (because TBCTR is initialised counting up)
	EPwm1Regs.ETPS.bit.INTPRD =  1; // generate an ISR on the first event (but when does ETPS[INTCNT] reset??)
				
	EALLOW;
	GpioCtrlRegs.GPAMUX1.bit.GPIO0 = 1;	// EPWM1A on GPIO0
	GpioCtrlRegs.GPADIR.bit.GPIO0 = 1;	// Output on GPIO0
	EDIS;
	
	
	//DEBUG
	GpioDataRegs.GPACLEAR.bit.GPIO2 = 1;
	EALLOW;
	GpioCtrlRegs.GPADIR.bit.GPIO2 = 1;				// Set GPIO2 as output
	EDIS;
	
	
	EALLOW;
	PieVectTable.EPWM1_INT = &isr_epwm1;
	EDIS;
	
	PieCtrlRegs.PIEIER3.bit.INTx1 = 1;     // PIE Group 3, INT1, EPWM1_INT
	IER |= M_INT3;
	
	// DEBUG
	//EPwm1Regs.CMPA.half.CMPA = 31250;	// 50% duty
}

void pwmStart() {
	EPwm1Regs.TBCTL.bit.CTRMODE = TB_COUNT_UPDOWN;
}

void pwmStop() {
	EPwm1Regs.TBCTL.bit.CTRMODE = TB_FREEZE;  // How to reset GPIO???
	EPwm1Regs.TBCTR = 0;	// Reset counter
	EPwm1Regs.AQSFRC.bit.OTSFA = 1;
}

#pragma CODE_SECTION(isr_epwm1, "ramfuncs");
interrupt void isr_epwm1(void)
{
	Uint16 INTSEL_current;
	Uint16 newSample;	

	//DEBUG
	//GpioDataRegs.GPASET.bit.GPIO2 = 1;
	
	EPwm1Regs.CMPA.half.CMPA = newSample;
		
	// Toggle ETSEL[INTSEL] so that the interrupt will be triggered by the next appropriate event (i.e. TBCTR=0 or TBCTR=TBPRD)
	INTSEL_current = EPwm1Regs.ETSEL.bit.INTSEL;
	if(INTSEL_current == 2) {  
		EPwm1Regs.ETSEL.bit.INTSEL = 1;
	} else {
		EPwm1Regs.ETSEL.bit.INTSEL = 2;
	}

	newSample = serviceDDS();	// Precompute sample for the next triggering of this interrupt	
	
	// Acknowledge this interrupt to receive more interrupts from group 3
	PieCtrlRegs.PIEACK.all = PIEACK_GROUP3;
}

