/*
 * pwm.c
 *
 *  Created on: 05/08/2016
 *      Author: broadmea
 */
#include "DSP28x_Project.h"
#include "config.h"

void initPWM() {
	EALLOW;	// PWM control registers are protected

	EPwm1Regs.TBPRD = SWTBPRD; // fc = 1/(2*65536/TBCLK) = 1/(2*65535/150000000) = 1.1444kHz
	//EPwm1Regs.CMPA.all = SWTBPRD; // Just to make sure a switching event doesn't happen on initialisation
	//EPwm1Regs.CMPB.all = SWTBPRD; // Not going to use EPwm1B, but will program it the same as EPwmA just to be safe
	EPwm1Regs.TBPHS.all = 0; // Set Phase register to zero (don't plan to use synchronisation functionality at this stage)
	EPwm1Regs.TBCTR = 0; // clear TB counter
	
	EPwm1Regs.TBCTL.bit.CTRMODE = TB_FREEZE; // Make sure the counter is not counting yet when we are initialising EPwm1
	EPwm1Regs.TBCTL.bit.PHSEN = TB_DISABLE; // Phase loading disabled
	EPwm1Regs.TBCTL.bit.PRDLD = TB_IMMEDIATE; // Load the TBPRD register immediately without using a shadow register
	EPwm1Regs.TBCTL.bit.SYNCOSEL = TB_SYNC_DISABLE; // Disable EPwmxSYNCO signal
	EPwm1Regs.TBCTL.bit.HSPCLKDIV = TB_DIV1; // TBCLK = SYSCLKOUT
	EPwm1Regs.TBCTL.bit.CLKDIV = TB_DIV1; // TBCLK = SYSCLKOUT
	
	EPwm1Regs.CMPCTL.bit.SHDWAMODE = CC_IMMEDIATE; // load CMPA immediately
	EPwm1Regs.CMPCTL.bit.SHDWBMODE = CC_IMMEDIATE; 
	EPwm1Regs.CMPCTL.bit.LOADAMODE = CC_CTR_ZERO; // Don't care about this load functionality as we are using immediate load mode
	EPwm1Regs.CMPCTL.bit.LOADBMODE = CC_CTR_ZERO; 
	
	EPwm1Regs.AQCTLA.bit.CAU = AQ_CLEAR; // I want my PWM signal to be CLEARED to low when the carrier is increasing and the modulating signal falls below it...
	EPwm1Regs.AQCTLA.bit.CAD = AQ_SET; // ...and to be SET to high when the carrier is decreasing and the modulating signal rises above it.   
	//EPwm1Regs.AQCTLA.bit.CBU = AQ_NO_ACTION; // Make sure that EPwm1A doesn't do anything when the CTR is equal to CMPB
	//EPwm1Regs.AQCTLA.bit.CBD = AQ_NO_ACTION; // Make sure that EPwm1A doesn't do anything when the CTR is equal to CMPB
    //EPwm1Regs.AQCTLA.bit.PRD = AQ_NO_ACTION; // Make sure that EPwm1A doesn't do anything when the CTR is equal to TBPRD
    //EPwm1Regs.AQCTLA.bit.ZRO = AQ_NO_ACTION; // Make sure that EPwm1A doesn't do anything when the CTR is equal to 0
        
	
	//EPwm1Regs.AQCTLB.bit.CBU = AQ_CLEAR; // Don't really care very much what EPwmB is doing, as long as it doesn't interfere with EPwmA...
	//EPwm1Regs.AQCTLB.bit.CBD = AQ_SET;
	
	EPwm1Regs.AQSFRC.bit.ACTSFA = 1;	// Set output low on software force

	GpioCtrlRegs.GPAMUX1.bit.GPIO0 = 1;	// EPWM1A on GPIO0
	GpioCtrlRegs.GPADIR.bit.GPIO0 = 1;	// Output on GPIO0

	// DEBUG
	//EPwm1Regs.CMPA.half.CMPA = 31250;	// 50% duty


	EDIS;
}

void pwmStart() {
	EALLOW;
	EPwm1Regs.TBCTL.bit.CTRMODE = TB_COUNT_UPDOWN;
	EDIS;
}

void pwmStop() {
	EALLOW;
	EPwm1Regs.TBCTL.bit.CTRMODE = TB_FREEZE;  // How to reset GPIO???
	EPwm1Regs.TBCTR = 0;	// Reset counter
	EPwm1Regs.AQSFRC.bit.OTSFA = 1;
	EDIS;
}

