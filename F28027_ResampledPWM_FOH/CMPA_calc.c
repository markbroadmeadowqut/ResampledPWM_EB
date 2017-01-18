/*
 * CMPA_calc.c
 *
 *  Created on: 16Jan.,2017
 *      Author: Edward
 */

#include "DSP28x_Project.h"
#include "config.h"
#include <math.h>

volatile Uint16 newSample = 0;
volatile Uint16 prevSample = 0;

interrupt void isr_CMPA_calc(void);


void initCMPAcalc() {
	EALLOW;
	PieVectTable.ADCINT1 = &isr_CMPA_calc;
	EDIS;

	PieCtrlRegs.PIEIER1.bit.INTx1 = 1;     // Enable PIE Group 1, INT1, ADCINT1
	IER |= M_INT1;
}


#pragma CODE_SECTION(isr_CMPA_calc, "ramfuncs");
interrupt void isr_CMPA_calc(void)
{
	Uint16 swCTRDIR;
	Uint16 swTBCTR;
	Uint16 calc1;
	int32 ti; // Am I correct in assuming that an int32 will have a sign bit (and thus range between -2.15 billion and +2.15 billion?)?
	int32 CMPA_value;

	newSample = AdcResult.ADCRESULT0;
	// Are there any other steps I need to take to read from the ADC result register (e.g. enable Peripheral Frame 0?)?
	// Is it ok to read a 12-bit register straight into a 16-bit variable like this?

	//CpuTimer0.InterruptCount++;

	swTBCTR = EPwm1Regs.TBCTR;   			// Sample EPwm1 counter (Is this the correct way to read from address 0x0004??)
	swCTRDIR = EPwm1Regs.TBSTS.bit.CTRDIR;

	//DEBUG
	//GpioDataRegs.GPASET.bit.GPIO2 = 1;

	// Update CMPA register
	if(swCTRDIR) {
		calc1=(FOH_SCALE+prevSample)-newSample;
		// Note that calc1 is equivalent to the negative of the modulating signal slope relative to the carrier slope in this
		// calculation. If it were to have a value of zero then this would imply that the modulating signal has the same slope
		// as the carrier (with the same sign); in principle this should never occur for modulating signals that are
		// band-limited to half the carrier frequency. It has units of register values (in decimal) per RTDS timestep; it is
		// restricted to the range [0,2*TBCLK/FS=6000] because the band-limitedness of the modulating signal directly implies
		// restriction of (prevSample-newSample) to the range [-3000,3000]. Forcing (FOH_SCALE+prevSample) to happen first
		// should remove any possibility of an intermediate step in the calculation of calc1 that is negative.
		ti=((swTBCTR-newSample)*FOH_SCALE)/(-calc1);
		// Note that ti has units of 1/TBCLK=6.66nS. This is necessary to avoid a second divide at the calculation of
		// CMPA_value. The term (swTBCTR-newSample) will range between -4095 and +4095, thus the term
		// ((swTBCTR-newSample)*FOH_SCALE) will range between -12.3million and +12.3million. Because of this range,
		// a signed 32-bit format for ti is necessary. The smallest non-zero value that ti can take is round(2*3000/6000)=1,
		// thus the effective resolution is 1*1/TBCLK=6.66nS. Range of interest of ti will be (0,3000].
		if(ti<=0) { // Might be a little faster to check (swTBCTR-newSample) for the missed edge instead of ti
			EALLOW;
			EPwm1Regs.AQSFRC.bit.ACTSFA = 1;	// Set output low on software force
			EDIS;
			EPwm1Regs.AQSFRC.bit.OTSFA = 1;
		}
		//CMPA_value=floor((swTBCTR+(FOH_SCALE4*ti))+0.5);
		CMPA_value=swTBCTR+ti;
		EPwm1Regs.CMPA.half.CMPA=CMPA_value;
		//DEBUG
		//GpioDataRegs.GPACLEAR.bit.GPIO2 = 1;
	} else {
		calc1=(FOH_SCALE+newSample)-prevSample;
		// Note that calc1 is equivalent to the modulating signal slope relative to the carrier slope in this calculation.
		// As before it is expected to have the range [0,2*TBCLK/FS=6000] and thus not require a sign bit.
		ti=((swTBCTR-newSample)*FOH_SCALE)/calc1;
		if(ti<=0) {
			EALLOW;
			EPwm1Regs.AQSFRC.bit.ACTSFA = 0;	// Set output high on software force
			EDIS;
			EPwm1Regs.AQSFRC.bit.OTSFA = 1;
		}
		CMPA_value=swTBCTR-ti;
		EPwm1Regs.CMPA.half.CMPA=CMPA_value;
		//DEBUG
		//GpioDataRegs.GPACLEAR.bit.GPIO2 = 1;
	}

	prevSample = newSample;       // Push newSample back on to prevSample at this point in the interrupt so that the write to CMPA happens sooner

	// DEBUG
	//EPwm1Regs.CMPA.half.CMPA=newSample;


	// Acknowledge this interrupt to receive more interrupts from group 1
	PieCtrlRegs.PIEACK.all = PIEACK_GROUP1;
}
