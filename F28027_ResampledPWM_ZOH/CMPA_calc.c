/*
 * CMPA_calc.c
 *
 *  Created on: 16Jan.,2017
 *      Author: Edward
 */

#include "DSP28x_Project.h"
#include "config.h"
#include <math.h>

volatile Uint16 swCTRDIR1=1;
volatile Uint16 swCTRDIR2=1;
volatile Uint16 swCTRDIR3=0;

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
	Uint16 swCTRDIR;
	Uint16 swTBCTR;
	int16 CMPB;

	// Write the new sample to CMPA (active)
	EPwm1Regs.CMPA.half.CMPA=AdcResult.ADCRESULT0;

	// Check if the edge has been missed for ePWM1
	swCTRDIR = EPwm1Regs.TBSTS.bit.CTRDIR;
	swTBCTR = EPwm1Regs.TBCTR;
	if(swCTRDIR) {
		if(AdcResult.ADCRESULT0<=swTBCTR) {
			EALLOW;
			EPwm1Regs.AQSFRC.bit.ACTSFA = 1;	// Set output low on software force
			EDIS;
			EPwm1Regs.AQSFRC.bit.OTSFA = 1;
		}
	} else {
		if(AdcResult.ADCRESULT0>=swTBCTR) {
			EALLOW;
			EPwm1Regs.AQSFRC.bit.ACTSFA = 10;	// Set output high on software force
			EDIS;
			EPwm1Regs.AQSFRC.bit.OTSFA = 1;
		}
	}

	CMPB=EPwm1Regs.CMPB;
	if(swCTRDIR1) {
		if(CMPB==SWTBPRD) {
			CMPB = CMPB-CMPB_increment;
			EPwm1Regs.CMPB=CMPB;
			EPwm1Regs.ETSEL.bit.SOCASEL=111; // Trigger SOCA when CTR=CMPB and timer is decrementing, because CTRDIR is about to change to zero
			swCTRDIR1=0;
		} else {
			CMPB = CMPB+CMPB_increment;
			EPwm1Regs.CMPB=CMPB;
			GpioDataRegs.GPATOGGLE.bit.GPIO29 = 1;
		}
	} else {
		if(CMPB==0) {
			CMPB = CMPB+CMPB_increment;
			EPwm1Regs.CMPB=CMPB;
			EPwm1Regs.ETSEL.bit.SOCASEL=110; // Trigger SOCA when CTR=CMPB and timer is incrementing, because CTRDIR is about to change to one
			swCTRDIR1=1;
		} else {
			CMPB = CMPB-CMPB_increment;
			EPwm1Regs.CMPB=CMPB;
			GpioDataRegs.GPACLEAR.bit.GPIO29 = 1;
		}
	}

	// Acknowledge this interrupt to receive more interrupts from group 1
	PieCtrlRegs.PIEACK.all = PIEACK_GROUP1;
}

#pragma CODE_SECTION(isr_CMPA_calc2, "ramfuncs");
interrupt void isr_CMPA_calc2(void)
{
	Uint16 swCTRDIR;
	Uint16 swTBCTR;
	Uint16 newSample;

	// Write the new sample to CMPA (active)
	newSample=AdcResult.ADCRESULT1;
	EPwm2Regs.CMPA.half.CMPA=newSample;

	// Check if the edge has been missed for ePWM1
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

	if(swCTRDIR2) {
		if(EPwm2Regs.CMPB==SWTBPRD) {
			EPwm2Regs.ETSEL.bit.SOCASEL=111; // Trigger SOCA when CTR=CMPB and timer is decrementing, because CTRDIR is about to change to zero
			swCTRDIR2=0;
			EPwm2Regs.CMPB = EPwm2Regs.CMPB-CMPB_increment;
		} else {
			EPwm2Regs.CMPB = EPwm2Regs.CMPB+CMPB_increment;
		}
	} else {
		if(EPwm2Regs.CMPB==0) {
			EPwm2Regs.ETSEL.bit.SOCASEL=110; // Trigger SOCA when CTR=CMPB and timer is incrementing, because CTRDIR is about to change to one
			swCTRDIR2=1;
			EPwm2Regs.CMPB = EPwm2Regs.CMPB+CMPB_increment;
		} else {
			EPwm2Regs.CMPB = EPwm2Regs.CMPB-CMPB_increment;
		}
	}

	// Acknowledge this interrupt to receive more interrupts from group 1
	PieCtrlRegs.PIEACK.all = PIEACK_GROUP1;
}

#pragma CODE_SECTION(isr_CMPA_calc3, "ramfuncs");
interrupt void isr_CMPA_calc3(void)
{
	Uint16 swCTRDIR;
	Uint16 swTBCTR;
	Uint16 newSample;

	// Write the new sample to CMPA (active)
	newSample=AdcResult.ADCRESULT2;
	EPwm3Regs.CMPA.half.CMPA=newSample;

	// Check if the edge has been missed for ePWM1
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

	if(swCTRDIR3) {
		if(EPwm3Regs.CMPB==SWTBPRD) {
			EPwm3Regs.ETSEL.bit.SOCASEL=111; // Trigger SOCA when CTR=CMPB and timer is decrementing, because CTRDIR is about to change to zero
			swCTRDIR3=0;
			EPwm3Regs.CMPB = EPwm3Regs.CMPB-CMPB_increment;
		} else {
			EPwm3Regs.CMPB = EPwm3Regs.CMPB+CMPB_increment;
		}
	} else {
		if(EPwm3Regs.CMPB==0) {
			EPwm3Regs.ETSEL.bit.SOCASEL=110; // Trigger SOCA when CTR=CMPB and timer is incrementing, because CTRDIR is about to change to one
			swCTRDIR3=1;
			EPwm3Regs.CMPB = EPwm3Regs.CMPB+CMPB_increment;
		} else {
			EPwm3Regs.CMPB = EPwm3Regs.CMPB-CMPB_increment;
		}
	}

	// Acknowledge this interrupt to receive more interrupts from group 10
	PieCtrlRegs.PIEACK.all = PIEACK_GROUP10;
}

