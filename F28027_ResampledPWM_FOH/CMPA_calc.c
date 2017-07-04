/*
 * CMPA_calc.c
 *
 *  Created on: 16Jan.,2017
 *      Author: Edward
 */

#include "DSP28x_Project.h"
#include "config.h"
#include <math.h>

volatile Uint16 prevSample1 = 0;
volatile Uint16 prevSample2 = 0;
volatile Uint16 prevSample3 = 0;


interrupt void isr_CMPA_calc1(void);
interrupt void isr_CMPA_calc2(void);
interrupt void isr_CMPA_calc3(void);


void initCMPAcalc() {
	EALLOW;
	PieVectTable.ADCINT1 = &isr_CMPA_calc1;
	PieVectTable.ADCINT2 = &isr_CMPA_calc2;
	PieVectTable.ADCINT3 = &isr_CMPA_calc3;
	EDIS;

	PieCtrlRegs.PIEIER1.bit.INTx1 = 1;     // Enable ADCINT1
	PieCtrlRegs.PIEIER1.bit.INTx2 = 1;     // Enable ADCINT2
	PieCtrlRegs.PIEIER10.bit.INTx3 = 1;     // Enable ADCINT3
	IER |= M_INT1;
	IER |= M_INT10;

	//DEBUG
	//GpioDataRegs.GPACLEAR.bit.GPIO29 = 1;
	EALLOW;
	GpioCtrlRegs.GPADIR.bit.GPIO29 = 1;				// Set GPIO29 as output
	EDIS;

}


#pragma CODE_SECTION(isr_CMPA_calc1, "ramfuncs");
interrupt void isr_CMPA_calc1(void)
{
	int32 swTBCTR; // Must be unsigned 32 bit integer because (swTBCTR-newSample)*FOH_SCALE could reach 12million; this exceeds the 16-bit integer size
    int16 calc1_rising;
    int16 calc1_falling;
    int32 ti;
	int32 CMPA_value;
	Uint16 swCTRDIR;
	int16 swCMPB;
	Uint16 newSample;

	newSample=AdcResult.ADCRESULT0;
	swCTRDIR = EPwm1Regs.TBSTS.bit.CTRDIR;
	calc1_rising=newSample-prevSample1-FOH_SCALE;  // If I wanted I could probably squeeze another bit of precision into ti by multiplying (newSample-prevSample) by 2; overflow of 2^15 should still be impossible...
	calc1_falling=calc1_rising+FOH_SCALE_doubled;
	if(swCTRDIR) {
		// Note that calc1 is equivalent to the modulating signal slope relative to the carrier slope in this
		// calculation. If it were to have a value of zero then this would imply that the modulating signal has the same slope
		// as the carrier (with the same sign); in principle this should never occur for modulating signals that are
		// band-limited to half the carrier frequency. It has units of register values (in decimal) per RTDS timestep; it is
		// restricted to the range [-2*TBCLK/FS=6000,0] because the band-limitedness of the modulating signal directly implies
		// restriction of (newSample-prevSample) to the range [-TBCLK/FS,TBCLK/FS]. Since TBCLK/FS=3000 atm, a signed 16-bit integer
		// is more than sufficient for calc1.
		swTBCTR = EPwm1Regs.TBCTR;
		ti=((swTBCTR-newSample)*FOH_SCALE)/calc1_rising;
		// Note that ti has units of 1/TBCLK=16.66nS. This is necessary to avoid a second divide at the calculation of
		// CMPA_value. The term (swTBCTR-newSample) will range between -4095 and +4095, thus the term
		// ((swTBCTR-newSample)*FOH_SCALE) will range between -12.3million and +12.3million. Because of this range,
		// a signed 32-bit format for ti is necessary. The smallest non-zero value that ti can take is round(2*3000/6000)=1,
		// thus the effective resolution is 1*1/TBCLK=16.66nS. Range of interest of ti will be (0,3000].
		CMPA_value=swTBCTR+ti;
		if(ti>compare_rejection_factor) {
			EPwm1Regs.CMPA.half.CMPA=CMPA_value;
		}
	} else {
		// Note that calc1 is equivalent to the modulating signal slope relative to the carrier slope in this calculation.
		// Since FOH_SCALE is added to the calculation rather than subtracted, the range is now [0,2*TBCLK/FS=6000].
		swTBCTR = EPwm1Regs.TBCTR;
		ti=((swTBCTR-newSample)*FOH_SCALE)/calc1_falling;
		CMPA_value=swTBCTR-ti;
		if(ti>compare_rejection_factor) {
			EPwm1Regs.CMPA.half.CMPA=CMPA_value;
		}
	}

	swCMPB=EPwm1Regs.CMPB;
	if(swCMPB==SWTBPRD) {
		swCMPB = swCMPB-FOH_SCALE;
		EPwm1Regs.CMPB=swCMPB;
		EPwm1Regs.ETSEL.bit.SOCASEL=7; // Trigger SOCA when CTR=CMPB and timer is decrementing, because CTRDIR is about to change to zero
	} else if(swCMPB==0) {
		swCMPB = swCMPB+FOH_SCALE;
		EPwm1Regs.CMPB=swCMPB;
		EPwm1Regs.ETSEL.bit.SOCASEL=6; // Trigger SOCA when CTR=CMPB and timer is incrementing, because CTRDIR is about to change to one
	} else {
		if(swCTRDIR) {
			swCMPB = swCMPB+FOH_SCALE;
			EPwm1Regs.CMPB=swCMPB;
			if(swCMPB==SWTBPRD) {
				EPwm1Regs.ETSEL.bit.SOCASEL=3; // Trigger SOCA when TBCTR=TBPRD
			}
		} else {
			swCMPB = swCMPB-FOH_SCALE;
			EPwm1Regs.CMPB=swCMPB;
			if(swCMPB==0) {
				EPwm1Regs.ETSEL.bit.SOCASEL=3; // Trigger SOCA when TBCTR=0
			}
		}
	}

	prevSample1=newSample;

	// Acknowledge this interrupt to receive more interrupts from group 1
	PieCtrlRegs.PIEACK.all = PIEACK_GROUP1;
}

#pragma CODE_SECTION(isr_CMPA_calc2, "ramfuncs");
interrupt void isr_CMPA_calc2(void)
{
	int32 swTBCTR; // Must be unsigned 32 bit integer because (swTBCTR-newSample)*FOH_SCALE could reach 12million; this exceeds the 16-bit integer size
    int16 calc1_rising;
    int16 calc1_falling;
    int32 ti;
	int32 CMPA_value;
	Uint16 swCTRDIR;
	int16 swCMPB;
	Uint16 newSample;

	newSample=AdcResult.ADCRESULT1;
	swCTRDIR = EPwm2Regs.TBSTS.bit.CTRDIR;
	calc1_rising=newSample-prevSample2-FOH_SCALE;  // If I wanted I could probably squeeze another bit of precision into ti by multiplying (newSample-prevSample) by 2; overflow of 2^15 should still be impossible...
	calc1_falling=calc1_rising+FOH_SCALE_doubled;
	if(swCTRDIR) {
		// Note that calc1 is equivalent to the modulating signal slope relative to the carrier slope in this
		// calculation. If it were to have a value of zero then this would imply that the modulating signal has the same slope
		// as the carrier (with the same sign); in principle this should never occur for modulating signals that are
		// band-limited to half the carrier frequency. It has units of register values (in decimal) per RTDS timestep; it is
		// restricted to the range [-2*TBCLK/FS=6000,0] because the band-limitedness of the modulating signal directly implies
		// restriction of (newSample-prevSample) to the range [-TBCLK/FS,TBCLK/FS]. Since TBCLK/FS=3000 atm, a signed 16-bit integer
		// is more than sufficient for calc1.
		swTBCTR = EPwm2Regs.TBCTR;
		ti=((swTBCTR-newSample)*FOH_SCALE)/calc1_rising;
		// Note that ti has units of 1/TBCLK=16.66nS. This is necessary to avoid a second divide at the calculation of
		// CMPA_value. The term (swTBCTR-newSample) will range between -4095 and +4095, thus the term
		// ((swTBCTR-newSample)*FOH_SCALE) will range between -12.3million and +12.3million. Because of this range,
		// a signed 32-bit format for ti is necessary. The smallest non-zero value that ti can take is round(2*3000/6000)=1,
		// thus the effective resolution is 1*1/TBCLK=16.66nS. Range of interest of ti will be (0,3000].
		CMPA_value=swTBCTR+ti;
		if(ti>compare_rejection_factor) {
			EPwm2Regs.CMPA.half.CMPA=CMPA_value;
		}
	} else {
		// Note that calc1 is equivalent to the modulating signal slope relative to the carrier slope in this calculation.
		// Since FOH_SCALE is added to the calculation rather than subtracted, the range is now [0,2*TBCLK/FS=6000].
		swTBCTR = EPwm2Regs.TBCTR;
		ti=((swTBCTR-newSample)*FOH_SCALE)/calc1_falling;
		CMPA_value=swTBCTR-ti;
		if(ti>compare_rejection_factor) {
			EPwm2Regs.CMPA.half.CMPA=CMPA_value;
		}
	}


	swCMPB=EPwm2Regs.CMPB;
	if(swCMPB==SWTBPRD) {
		swCMPB = swCMPB-FOH_SCALE;
		EPwm2Regs.CMPB=swCMPB;
		EPwm2Regs.ETSEL.bit.SOCASEL=7; // Trigger SOCA when CTR=CMPB and timer is decrementing, because CTRDIR is about to change to zero
	} else if(swCMPB==0) {
		swCMPB = swCMPB+FOH_SCALE;
		EPwm2Regs.CMPB=swCMPB;
		EPwm2Regs.ETSEL.bit.SOCASEL=6; // Trigger SOCA when CTR=CMPB and timer is incrementing, because CTRDIR is about to change to one
	} else {
		if(swCTRDIR) {
			swCMPB = swCMPB+FOH_SCALE;
			EPwm2Regs.CMPB=swCMPB;
			if(swCMPB==SWTBPRD) {
				EPwm2Regs.ETSEL.bit.SOCASEL=3; // Trigger SOCA when TBCTR=TBPRD
			}
		} else {
			swCMPB = swCMPB-FOH_SCALE;
			EPwm2Regs.CMPB=swCMPB;
			if(swCMPB==0) {
				EPwm2Regs.ETSEL.bit.SOCASEL=3; // Trigger SOCA when TBCTR=0
			}
		}
	}

	prevSample2=newSample;

	// Acknowledge this interrupt to receive more interrupts from group 1
	PieCtrlRegs.PIEACK.all = PIEACK_GROUP1;
}

#pragma CODE_SECTION(isr_CMPA_calc3, "ramfuncs");
interrupt void isr_CMPA_calc3(void)
{
	int32 swTBCTR; // Must be unsigned 32 bit integer because (swTBCTR-newSample)*FOH_SCALE could reach 12million; this exceeds the 16-bit integer size
    int16 calc1_rising;
    int16 calc1_falling;
    int32 ti;
	int32 CMPA_value;
	Uint16 swCTRDIR;
	int16 swCMPB;
	Uint16 newSample;

	newSample=AdcResult.ADCRESULT2;
	swCTRDIR = EPwm3Regs.TBSTS.bit.CTRDIR;
	calc1_rising=newSample-prevSample3-FOH_SCALE;  // If I wanted I could probably squeeze another bit of precision into ti by multiplying (newSample-prevSample) by 2; overflow of 2^15 should still be impossible...
	calc1_falling=calc1_rising+FOH_SCALE_doubled;
	if(swCTRDIR) {
		// Note that calc1 is equivalent to the modulating signal slope relative to the carrier slope in this
		// calculation. If it were to have a value of zero then this would imply that the modulating signal has the same slope
		// as the carrier (with the same sign); in principle this should never occur for modulating signals that are
		// band-limited to half the carrier frequency. It has units of register values (in decimal) per RTDS timestep; it is
		// restricted to the range [-2*TBCLK/FS=6000,0] because the band-limitedness of the modulating signal directly implies
		// restriction of (newSample-prevSample) to the range [-TBCLK/FS,TBCLK/FS]. Since TBCLK/FS=3000 atm, a signed 16-bit integer
		// is more than sufficient for calc1.
		swTBCTR = EPwm3Regs.TBCTR;
		ti=((swTBCTR-newSample)*FOH_SCALE)/calc1_rising;
		// Note that ti has units of 1/TBCLK=16.66nS. This is necessary to avoid a second divide at the calculation of
		// CMPA_value. The term (swTBCTR-newSample) will range between -4095 and +4095, thus the term
		// ((swTBCTR-newSample)*FOH_SCALE) will range between -12.3million and +12.3million. Because of this range,
		// a signed 32-bit format for ti is necessary. The smallest non-zero value that ti can take is round(2*3000/6000)=1,
		// thus the effective resolution is 1*1/TBCLK=16.66nS. Range of interest of ti will be (0,3000].
		CMPA_value=swTBCTR+ti;
		if(ti>compare_rejection_factor) {
			EPwm3Regs.CMPA.half.CMPA=CMPA_value;
		}
	} else {
		// Note that calc1 is equivalent to the modulating signal slope relative to the carrier slope in this calculation.
		// Since FOH_SCALE is added to the calculation rather than subtracted, the range is now [0,2*TBCLK/FS=6000].
		swTBCTR = EPwm3Regs.TBCTR;
		ti=((swTBCTR-newSample)*FOH_SCALE)/calc1_falling;
		CMPA_value=swTBCTR-ti;
		if(ti>compare_rejection_factor) {
			EPwm3Regs.CMPA.half.CMPA=CMPA_value;
		}
	}

	swCMPB=EPwm3Regs.CMPB;
	if(swCMPB==SWTBPRD) {
		swCMPB = swCMPB-FOH_SCALE;
		EPwm3Regs.CMPB=swCMPB;
		EPwm3Regs.ETSEL.bit.SOCASEL=7; // Trigger SOCA when CTR=CMPB and timer is decrementing, because CTRDIR is about to change to zero
	} else if(swCMPB==0) {
		swCMPB = swCMPB+FOH_SCALE;
		EPwm3Regs.CMPB=swCMPB;
		EPwm3Regs.ETSEL.bit.SOCASEL=6; // Trigger SOCA when CTR=CMPB and timer is incrementing, because CTRDIR is about to change to one
	} else {
		if(swCTRDIR) {
			swCMPB = swCMPB+FOH_SCALE;
			EPwm3Regs.CMPB=swCMPB;
			if(swCMPB==SWTBPRD) {
				EPwm3Regs.ETSEL.bit.SOCASEL=3; // Trigger SOCA when TBCTR=TBPRD
			}
		} else {
			swCMPB = swCMPB-FOH_SCALE;
			EPwm3Regs.CMPB=swCMPB;
			if(swCMPB==0) {
				EPwm3Regs.ETSEL.bit.SOCASEL=3; // Trigger SOCA when TBCTR=0
			}
		}
	}

	prevSample3=newSample;

	// Acknowledge this interrupt to receive more interrupts from group 10
	PieCtrlRegs.PIEACK.all = PIEACK_GROUP10;
}
