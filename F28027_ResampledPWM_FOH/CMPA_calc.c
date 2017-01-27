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

	//DEBUG
	GpioDataRegs.GPACLEAR.bit.GPIO2 = 1;
	EALLOW;
	GpioCtrlRegs.GPADIR.bit.GPIO2 = 1;				// Set GPIO2 as output
	EDIS;


	PieCtrlRegs.PIEIER1.bit.INTx1 = 1;     // Enable PIE Group 1, INT1, ADCINT1
	IER |= M_INT1;
}


#pragma CODE_SECTION(isr_CMPA_calc, "ramfuncs");
interrupt void isr_CMPA_calc(void)
{
	Uint16 swCTRDIR;
	int32 swTBCTR; // Must be unsigned 32 bit integer because (swTBCTR-newSample)*FOH_SCALE could reach 12million; this exceeds the 16-bit integer size
	int16 calc1;
	int32 ti;
	int32 CMPA_value;

	newSample = AdcResult.ADCRESULT0;
	// Are there any other steps I need to take to read from the ADC result register (e.g. enable Peripheral Frame 0?)?
	// Is it ok to read a 12-bit register straight into a 16-bit variable like this?

	//CpuTimer0.InterruptCount++;

	//DEBUG
	//GpioDataRegs.GPATOGGLE.bit.GPIO16 = 1;
	// Update CMPA register for ePWM1
	swCTRDIR = EPwm1Regs.TBSTS.bit.CTRDIR;
	if(swCTRDIR) {
		calc1=newSample-prevSample-FOH_SCALE;  // If I wanted I could probably squeeze another bit of precision into ti by multiplying (newSample-prevSample) by 2; overflow of 2^15 should still be impossible...
		// Note that calc1 is equivalent to the modulating signal slope relative to the carrier slope in this
		// calculation. If it were to have a value of zero then this would imply that the modulating signal has the same slope
		// as the carrier (with the same sign); in principle this should never occur for modulating signals that are
		// band-limited to half the carrier frequency. It has units of register values (in decimal) per RTDS timestep; it is
		// restricted to the range [-2*TBCLK/FS=6000,0] because the band-limitedness of the modulating signal directly implies
		// restriction of (newSample-prevSample) to the range [-TBCLK/FS,TBCLK/FS]. Since TBCLK/FS=3000 atm, a signed 16-bit integer
		// is more than sufficient for calc1.
		swTBCTR = EPwm1Regs.TBCTR;
		ti=((swTBCTR-newSample)*FOH_SCALE)/calc1;
		// Note that ti has units of 1/TBCLK=16.66nS. This is necessary to avoid a second divide at the calculation of
		// CMPA_value. The term (swTBCTR-newSample) will range between -4095 and +4095, thus the term
		// ((swTBCTR-newSample)*FOH_SCALE) will range between -12.3million and +12.3million. Because of this range,
		// a signed 32-bit format for ti is necessary. The smallest non-zero value that ti can take is round(2*3000/6000)=1,
		// thus the effective resolution is 1*1/TBCLK=16.66nS. Range of interest of ti will be (0,3000].
		CMPA_value=swTBCTR+ti;
		EPwm1Regs.CMPA.half.CMPA=CMPA_value;
		if(ti<96) {
			//DEBUG
//			GpioDataRegs.GPATOGGLE.bit.GPIO16 = 1;
			EALLOW;
			EPwm1Regs.AQSFRC.bit.ACTSFA = 1;	// Set output low on software force
			EDIS;
			EPwm1Regs.AQSFRC.bit.OTSFA = 1;
		}
		//DEBUG
//			GpioDataRegs.GPACLEAR.bit.GPIO16 = 1;
		} else {
		calc1=newSample-prevSample+FOH_SCALE;
		// Note that calc1 is equivalent to the modulating signal slope relative to the carrier slope in this calculation.
		// Since FOH_SCALE is added to the calculation rather than subtracted, the range is now [0,2*TBCLK/FS=6000].
		swTBCTR = EPwm1Regs.TBCTR;
		ti=((swTBCTR-newSample)*FOH_SCALE)/calc1;
		CMPA_value=swTBCTR-ti;
		EPwm1Regs.CMPA.half.CMPA=CMPA_value;
		if(ti<96) {
			//DEBUG
//			GpioDataRegs.GPATOGGLE.bit.GPIO16 = 1;
			EALLOW;
			EPwm1Regs.AQSFRC.bit.ACTSFA = 10;	// Set output high on software force
			EDIS;
			EPwm1Regs.AQSFRC.bit.OTSFA = 1;
		}
		//DEBUG
//		GpioDataRegs.GPACLEAR.bit.GPIO16 = 1;
	}

	// Update CMPA register for ePWM2
	swCTRDIR = EPwm2Regs.TBSTS.bit.CTRDIR;
	if(swCTRDIR) {
		// calc1 is the same for all ePWM modules
		swTBCTR = EPwm2Regs.TBCTR;
		ti=((swTBCTR-newSample)*FOH_SCALE)/calc1;
		// Note that ti has units of 1/TBCLK=16.66nS. This is necessary to avoid a second divide at the calculation of
		// CMPA_value. The term (swTBCTR-newSample) will range between -4095 and +4095, thus the term
		// ((swTBCTR-newSample)*FOH_SCALE) will range between -12.3million and +12.3million. Because of this range,
		// a signed 32-bit format for ti is necessary. The smallest non-zero value that ti can take is round(2*3000/6000)=1,
		// thus the effective resolution is 1*1/TBCLK=16.66nS. Range of interest of ti will be (0,3000].
		CMPA_value=swTBCTR+ti;
		EPwm2Regs.CMPA.half.CMPA=CMPA_value;
		if(ti<204) { //96*2+12=204, factir of 12 is due to the extra ~0.2us required for software forcing check on the last module
			EALLOW;
			EPwm2Regs.AQSFRC.bit.ACTSFA = 1;	// Set output low on software force
			EDIS;
			EPwm2Regs.AQSFRC.bit.OTSFA = 1;
		}
	} else {
		swTBCTR = EPwm2Regs.TBCTR;
		ti=((swTBCTR-newSample)*FOH_SCALE)/calc1;
		CMPA_value=swTBCTR-ti;
		EPwm2Regs.CMPA.half.CMPA=CMPA_value;
		if(ti<204) {
			EALLOW;
			EPwm2Regs.AQSFRC.bit.ACTSFA = 10;	// Set output high on software force
			EDIS;
			EPwm2Regs.AQSFRC.bit.OTSFA = 1;
		}
	}

	// Update CMPA register for ePWM3
	swCTRDIR = EPwm3Regs.TBSTS.bit.CTRDIR;
	if(swCTRDIR) {
		// calc1 is the same for all ePWM modules
		swTBCTR = EPwm3Regs.TBCTR;
		ti=((swTBCTR-newSample)*FOH_SCALE)/calc1;
		// Note that ti has units of 1/TBCLK=16.66nS. This is necessary to avoid a second divide at the calculation of
		// CMPA_value. The term (swTBCTR-newSample) will range between -4095 and +4095, thus the term
		// ((swTBCTR-newSample)*FOH_SCALE) will range between -12.3million and +12.3million. Because of this range,
		// a signed 32-bit format for ti is necessary. The smallest non-zero value that ti can take is round(2*3000/6000)=1,
		// thus the effective resolution is 1*1/TBCLK=16.66nS. Range of interest of ti will be (0,3000].
		CMPA_value=swTBCTR+ti;
		EPwm3Regs.CMPA.half.CMPA=CMPA_value;
		if(ti<312) { //96*3+2*12=312; Note that 312 is about 10% of 3000, so only 10% of all FOH switching instant
			// calculations for ePWM3 are made innaccurate by the need for software forcing, and that inaccuracy should
			// only amount to samples from a uniform distribution that is bounded by [0us,5us].
			EALLOW;
			EPwm3Regs.AQSFRC.bit.ACTSFA = 1;	// Set output low on software force
			EDIS;
			EPwm3Regs.AQSFRC.bit.OTSFA = 1;
		}
	} else {
		swTBCTR = EPwm3Regs.TBCTR;
		ti=((swTBCTR-newSample)*FOH_SCALE)/calc1;
		CMPA_value=swTBCTR-ti;
		EPwm3Regs.CMPA.half.CMPA=CMPA_value;
		if(ti<312) {
			EALLOW;
			EPwm3Regs.AQSFRC.bit.ACTSFA = 10;	// Set output high on software force
			EDIS;
			EPwm3Regs.AQSFRC.bit.OTSFA = 1;
		}
	}


	prevSample = newSample;       // Push newSample back on to prevSample at this point in the interrupt so that the write to CMPA happens sooner

	//DEBUG
	//GpioDataRegs.GPACLEAR.bit.GPIO16 = 1;


	// Acknowledge this interrupt to receive more interrupts from group 1
	PieCtrlRegs.PIEACK.all = PIEACK_GROUP1;
}
