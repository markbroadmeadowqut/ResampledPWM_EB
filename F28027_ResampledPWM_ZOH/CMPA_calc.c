/*
 * CMPA_calc.c
 *
 *  Created on: 16Jan.,2017
 *      Author: Edward
 */

#include "DSP28x_Project.h"
#include "config.h"
#include <math.h>

interrupt void isr_CMPA_calc(void);


void initCMPAcalc() {
	EALLOW;
	PieVectTable.ADCINT1 = &isr_CMPA_calc;
	EDIS;

	PieCtrlRegs.PIEIER1.bit.INTx1 = 1;     // Enable PIE Group 1, INT1, ADCINT1
	IER |= M_INT1;

	//DEBUG
	GpioDataRegs.GPACLEAR.bit.GPIO29 = 1;
	EALLOW;
	GpioCtrlRegs.GPADIR.bit.GPIO29 = 1;				// Set GPIO29 as output
	EDIS;

}


#pragma CODE_SECTION(isr_CMPA_calc, "ramfuncs");
interrupt void isr_CMPA_calc(void)
{
	Uint16 swCTRDIR;
	int32 swTBCTR; // Must be unsigned 32 bit integer because (swTBCTR-newSample)*FOH_SCALE could reach 12million; this exceeds the 16-bit integer size
	int32 CMPA_value;
	Uint16 newSample;

	newSample = AdcResult.ADCRESULT0;
	EPwm1Regs.CMPA.half.CMPA=newSample;
	EPwm2Regs.CMPA.half.CMPA=newSample;
	EPwm3Regs.CMPA.half.CMPA=newSample;

	// Check if the edge has been missed for ePWM1
	swCTRDIR = EPwm1Regs.TBSTS.bit.CTRDIR;
	swTBCTR = EPwm1Regs.TBCTR;
	if(swCTRDIR) {
		if(newSample<=swTBCTR) {
			EALLOW;
			EPwm1Regs.AQSFRC.bit.ACTSFA = 1;	// Set output low on software force
			EDIS;
			EPwm1Regs.AQSFRC.bit.OTSFA = 1;
		}
	} else {
		if(newSample>=swTBCTR) {
			EALLOW;
			EPwm1Regs.AQSFRC.bit.ACTSFA = 10;	// Set output high on software force
			EDIS;
			EPwm1Regs.AQSFRC.bit.OTSFA = 1;
		}
	}

	// Check if the edge has been missed for ePWM2
	swCTRDIR = EPwm2Regs.TBSTS.bit.CTRDIR;
	swTBCTR = EPwm2Regs.TBCTR;
	if(swCTRDIR) {
		if(newSample<=swTBCTR) {
			EALLOW;
			EPwm2Regs.AQSFRC.bit.ACTSFA = 1;	// Set output low on software force
			EDIS;
			EPwm2Regs.AQSFRC.bit.OTSFA = 1;
		}
	} else {
		if(newSample>=swTBCTR) {
			EALLOW;
			EPwm2Regs.AQSFRC.bit.ACTSFA = 10;	// Set output high on software force
			EDIS;
			EPwm2Regs.AQSFRC.bit.OTSFA = 1;
		}
	}

	// Check if the edge has been missed for ePWM3
	swCTRDIR = EPwm3Regs.TBSTS.bit.CTRDIR;
	swTBCTR = EPwm3Regs.TBCTR;
	if(swCTRDIR) {
		if(newSample<=swTBCTR) {
			EALLOW;
			EPwm3Regs.AQSFRC.bit.ACTSFA = 1;	// Set output low on software force
			EDIS;
			EPwm3Regs.AQSFRC.bit.OTSFA = 1;
		}
	} else {
		if(newSample>=swTBCTR) {
			EALLOW;
			EPwm3Regs.AQSFRC.bit.ACTSFA = 10;	// Set output high on software force
			EDIS;
			EPwm3Regs.AQSFRC.bit.OTSFA = 1;
		}
	}

	// Acknowledge this interrupt to receive more interrupts from group 1
	PieCtrlRegs.PIEACK.all = PIEACK_GROUP1;
}
