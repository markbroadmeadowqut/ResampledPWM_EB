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
	Uint16 newSample;

	newSample = AdcResult.ADCRESULT0;
	EPwm1Regs.CMPA.half.CMPA=newSample;
	EPwm2Regs.CMPA.half.CMPA=newSample;
	EPwm3Regs.CMPA.half.CMPA=newSample;

	// Acknowledge this interrupt to receive more interrupts from group 1
	PieCtrlRegs.PIEACK.all = PIEACK_GROUP1;
}
