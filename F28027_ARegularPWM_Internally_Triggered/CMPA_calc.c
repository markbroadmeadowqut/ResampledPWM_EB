/*
 * CMPA_calc.c
 *
 *  Created on: 16Jan.,2017
 *      Author: Edward
 */

#include "DSP28x_Project.h"
#include "config.h"
#include <math.h>

volatile Uint16 CMPA;
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
	GpioDataRegs.GPACLEAR.bit.GPIO29 = 1;
	EALLOW;
	GpioCtrlRegs.GPADIR.bit.GPIO29 = 1;				// Set GPIO29 as output
	EDIS;

}


#pragma CODE_SECTION(isr_CMPA_calc1, "ramfuncs");
interrupt void isr_CMPA_calc1(void)
{
	Uint16 swCTRDIR;

	//GpioDataRegs.GPASET.bit.GPIO29 = 1;

	// Write the new sample to shadow of CMPA
	EPwm1Regs.CMPA.half.CMPA=AdcResult.ADCRESULT0;


	// Change CMPB value for ePWM1
	swCTRDIR=EPwm1Regs.TBSTS.bit.CTRDIR;
	if(swCTRDIR) {
		EPwm1Regs.CMPB = SOC_LEAD_FACTOR;
		EPwm1Regs.ETSEL.bit.SOCASEL=111; // Trigger SOCA when CTR=CMPB and timer is decrementing, because CTRDIR is about to change to zero
	} else {
		EPwm1Regs.CMPB = SWTBPRD-SOC_LEAD_FACTOR;
		EPwm1Regs.ETSEL.bit.SOCASEL=110; // Trigger SOCA when CTR=CMPB and timer is incrementing, because CTRDIR is about to change to one
	}

	// Acknowledge this interrupt to receive more interrupts from group 1
	PieCtrlRegs.PIEACK.all = PIEACK_GROUP1;

	//GpioDataRegs.GPACLEAR.bit.GPIO29 = 1;


}

#pragma CODE_SECTION(isr_CMPA_calc2, "ramfuncs");
interrupt void isr_CMPA_calc2(void)
{
	Uint16 swCTRDIR;


	// Write the new sample to shadow of CMPA
	EPwm2Regs.CMPA.half.CMPA=AdcResult.ADCRESULT1;

	// Change CMPB value for ePWM2
	swCTRDIR=EPwm2Regs.TBSTS.bit.CTRDIR;
	if(swCTRDIR) {
		EPwm2Regs.CMPB = SOC_LEAD_FACTOR;
		EPwm2Regs.ETSEL.bit.SOCASEL=111; // Trigger SOCA when CTR=CMPB and timer is decrementing, because CTRDIR is about to change to zero
	} else {
		EPwm2Regs.CMPB = SWTBPRD-SOC_LEAD_FACTOR;
		EPwm2Regs.ETSEL.bit.SOCASEL=110; // Trigger SOCA when CTR=CMPB and timer is incrementing, because CTRDIR is about to change to one
	}

	// Acknowledge this interrupt to receive more interrupts from group 1
	PieCtrlRegs.PIEACK.all = PIEACK_GROUP1;
}

#pragma CODE_SECTION(isr_CMPA_calc3, "ramfuncs");
interrupt void isr_CMPA_calc3(void)
{
	Uint16 swCTRDIR;


	// Write the new sample to shadow of CMPA
	EPwm3Regs.CMPA.half.CMPA=AdcResult.ADCRESULT2;

	// Change CMPB value for ePWM3
	swCTRDIR=EPwm3Regs.TBSTS.bit.CTRDIR;
	if(swCTRDIR) {
		EPwm3Regs.CMPB = SOC_LEAD_FACTOR;
		EPwm3Regs.ETSEL.bit.SOCASEL=111; // Trigger SOCA when CTR=CMPB and timer is decrementing, because CTRDIR is about to change to zero
	} else {
		EPwm3Regs.CMPB = SWTBPRD-SOC_LEAD_FACTOR;
		EPwm3Regs.ETSEL.bit.SOCASEL=110; // Trigger SOCA when CTR=CMPB and timer is incrementing, because CTRDIR is about to change to one
	}

	// Acknowledge this interrupt to receive more interrupts from group 10
	PieCtrlRegs.PIEACK.all = PIEACK_GROUP10;
}


