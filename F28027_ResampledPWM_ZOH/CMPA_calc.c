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
	Uint16 newSample;

	newSample = AdcResult.ADCRESULT0;

	// Check if the edge will be missed for ePWM1 by writing the new compare value; if it will then don't write it
	swTBCTR = EPwm1Regs.TBCTR;
	swCTRDIR = EPwm1Regs.TBSTS.bit.CTRDIR;
	if(swCTRDIR) {
		if(newSample>(swTBCTR+compare_rejection_factor)) {
			EPwm1Regs.CMPA.half.CMPA=newSample;
		}
	} else {
		if(newSample<(swTBCTR-compare_rejection_factor)) {
			EPwm1Regs.CMPA.half.CMPA=newSample;
		}
	}

	// Check if the edge will be missed for ePWM2 by writing the new compare value; if it will then don't write it
	swTBCTR = EPwm2Regs.TBCTR;
	swCTRDIR = EPwm2Regs.TBSTS.bit.CTRDIR;
	if(swCTRDIR) {
		if(newSample>(swTBCTR+compare_rejection_factor)) {
			EPwm2Regs.CMPA.half.CMPA=newSample;
		}
	} else {
		if(newSample<(swTBCTR-compare_rejection_factor)) {
			EPwm2Regs.CMPA.half.CMPA=newSample;
		}
	}

	// Check if the edge will be missed for ePWM3 by writing the new compare value; if it will then don't write it
	swTBCTR = EPwm3Regs.TBCTR;
	swCTRDIR = EPwm3Regs.TBSTS.bit.CTRDIR;
	if(swCTRDIR) {
		if(newSample>(swTBCTR+compare_rejection_factor)) {
			EPwm3Regs.CMPA.half.CMPA=newSample;
		}
	} else {
		if(newSample<(swTBCTR-compare_rejection_factor)) {
			EPwm3Regs.CMPA.half.CMPA=newSample;
		}
	}

	// Acknowledge this interrupt to receive more interrupts from group 1
	PieCtrlRegs.PIEACK.all = PIEACK_GROUP1;
}
