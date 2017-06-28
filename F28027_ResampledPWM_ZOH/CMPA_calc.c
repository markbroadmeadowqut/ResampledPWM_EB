/*
 * CMPA_calc.c
 *
 *  Created on: 16Jan.,2017
 *      Author: Edward
 */

#include "DSP28x_Project.h"
#include "config.h"
#include <math.h>

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
	int16 swCMPB;
	Uint16 newSample;

	//GpioDataRegs.GPASET.bit.GPIO29 = 1;

	newSample=AdcResult.ADCRESULT0;

	// Check if the edge will be missed by writing the new compare value; if it will then don't write it
	swTBCTR = EPwm1Regs.TBCTR;
	swCTRDIR = EPwm1Regs.TBSTS.bit.CTRDIR;
	if(swCTRDIR) {
		if(newSample>(swTBCTR+36)) {
			EPwm1Regs.CMPA.half.CMPA=newSample;
		}
	} else {
		if(newSample<(swTBCTR-36)) {
			EPwm1Regs.CMPA.half.CMPA=newSample;
		}
	}

	//GpioDataRegs.GPACLEAR.bit.GPIO29 = 1;

	swCMPB=EPwm1Regs.CMPB;
	if(swCMPB==SWTBPRD) {
		swCMPB = swCMPB-CMPB_increment;
		EPwm1Regs.CMPB=swCMPB;
		EPwm1Regs.ETSEL.bit.SOCASEL=0x7; // Trigger SOCA when CTR=CMPB and timer is decrementing, because CTRDIR is about to change to zero
	} else if(swCMPB==0) {
		swCMPB = swCMPB+CMPB_increment;
		EPwm1Regs.CMPB=swCMPB;
		EPwm1Regs.ETSEL.bit.SOCASEL=0x6; // Trigger SOCA when CTR=CMPB and timer is incrementing, because CTRDIR is about to change to one
	} else {
		if(swCTRDIR) {
			swCMPB = swCMPB+CMPB_increment;
			EPwm1Regs.CMPB=swCMPB;
			if(swCMPB==SWTBPRD) {
				EPwm1Regs.ETSEL.bit.SOCASEL=0x3; // Trigger SOCA when TBCTR=TBPRD
			}
		} else {
			swCMPB = swCMPB-CMPB_increment;
			EPwm1Regs.CMPB=swCMPB;
			if(swCMPB==0) {
				EPwm1Regs.ETSEL.bit.SOCASEL=0x3; // Trigger SOCA when TBCTR=0
			}
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
	int16 CMPB;

	newSample=AdcResult.ADCRESULT1;
	swCTRDIR = EPwm2Regs.TBSTS.bit.CTRDIR;

	// Check if the edge will be missed by writing the new compare value; if it will then don't write it
	swTBCTR = EPwm2Regs.TBCTR;
	if(swCTRDIR) {
		if(newSample>swTBCTR) {
			EPwm2Regs.CMPA.half.CMPA=newSample;
		}
	} else {
		if(newSample<swTBCTR) {
			EPwm2Regs.CMPA.half.CMPA=newSample;
		}
	}

	CMPB=EPwm2Regs.CMPB;
	if(CMPB==SWTBPRD) {
		CMPB = CMPB-CMPB_increment;
		EPwm2Regs.CMPB=CMPB;
		EPwm2Regs.ETSEL.bit.SOCASEL=111; // Trigger SOCA when CTR=CMPB and timer is decrementing, because CTRDIR is about to change to zero
	} else if(CMPB==0) {
		CMPB = CMPB+CMPB_increment;
		EPwm2Regs.CMPB=CMPB;
		EPwm2Regs.ETSEL.bit.SOCASEL=110; // Trigger SOCA when CTR=CMPB and timer is incrementing, because CTRDIR is about to change to one
	} else {
		if(swCTRDIR) {
			CMPB = CMPB+CMPB_increment;
			EPwm2Regs.CMPB=CMPB;
		} else {
			CMPB = CMPB-CMPB_increment;
			EPwm2Regs.CMPB=CMPB;
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
	Uint16 CMPB;

	newSample=AdcResult.ADCRESULT2;
	swCTRDIR = EPwm3Regs.TBSTS.bit.CTRDIR;

	// Check if the edge will be missed by writing the new compare value; if it will then don't write it
	swTBCTR = EPwm3Regs.TBCTR;
	if(swCTRDIR) {
		if(newSample>swTBCTR) {
			EPwm3Regs.CMPA.half.CMPA=newSample;
		}
	} else {
		if(newSample<swTBCTR) {
			EPwm3Regs.CMPA.half.CMPA=newSample;
		}
	}

	CMPB=EPwm3Regs.CMPB;
	if(CMPB==SWTBPRD) {
		CMPB = CMPB-CMPB_increment;
		EPwm3Regs.CMPB=CMPB;
		EPwm3Regs.ETSEL.bit.SOCASEL=111; // Trigger SOCA when CTR=CMPB and timer is decrementing, because CTRDIR is about to change to zero
	} else if(CMPB==0) {
		CMPB = CMPB+CMPB_increment;
		EPwm3Regs.CMPB=CMPB;
		EPwm3Regs.ETSEL.bit.SOCASEL=110; // Trigger SOCA when CTR=CMPB and timer is incrementing, because CTRDIR is about to change to one
	} else {
		if(swCTRDIR) {
			CMPB = CMPB+CMPB_increment;
			EPwm3Regs.CMPB=CMPB;
		} else {
			CMPB = CMPB-CMPB_increment;
			EPwm3Regs.CMPB=CMPB;
		}
	}

	// Acknowledge this interrupt to receive more interrupts from group 10
	PieCtrlRegs.PIEACK.all = PIEACK_GROUP10;
}
