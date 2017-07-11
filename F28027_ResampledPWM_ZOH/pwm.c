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

	//SysCtrlRegs.PCLKCR1.bit.EPWM1ENCLK = 1;
	//SysCtrlRegs.PCLKCR1.bit.EPWM2ENCLK = 1;
	//SysCtrlRegs.PCLKCR1.bit.EPWM3ENCLK = 1;

	//SysCtrlRegs.PCLKCR0.bit.TBCLKSYNC = 0;

	EPwm1Regs.TBPRD = SWTBPRD; // fc = 1/(2*4095/TBCLK) = 1/(2*4095/60000000) = 7.327kHz
	EPwm1Regs.TBPHS.half.TBPHS = 0; // Set Phase register for first module to zero; other two modules are phase displaced wrt this one
	//EPwm1Regs.TBCTR = 0;
	EPwm1Regs.TBCTL.bit.CTRMODE = TB_FREEZE; // Make sure the counter is not counting yet when we are initialising EPwm1
	EPwm1Regs.TBCTL.bit.PHSDIR = 1; // After the sync event, ePWM1 will count up from phase 0
	EPwm1Regs.TBCTL.bit.PHSEN = TB_ENABLE; // Phase loading enabled
	EPwm1Regs.TBCTL.bit.PRDLD = TB_IMMEDIATE; // Load the TBPRD register immediately without using a shadow register
	EPwm1Regs.TBCTL.bit.HSPCLKDIV = 000; // TBCLK = SYSCLKOUT
	EPwm1Regs.TBCTL.bit.CLKDIV = 000; // TBCLK = SYSCLKOUT
	EPwm1Regs.TBCTL.bit.SYNCOSEL = 00; // Associate ePWM1SYNCO with ePWM1SYNCI; this should ensure that the software-forced sync event triggers all of the other ePWM modules
	EPwm1Regs.CMPCTL.bit.SHDWAMODE = CC_SHADOW;
	EPwm1Regs.CMPCTL.bit.SHDWBMODE = CC_SHADOW;
	EPwm1Regs.CMPCTL.bit.LOADAMODE = 2; // load at the peaks and the troughs
	EPwm1Regs.CMPCTL.bit.LOADBMODE = 2;
	EPwm1Regs.AQCTLA.bit.CAU = AQ_CLEAR; // I want my PWM signal to be CLEARED to low when the carrier is increasing and the modulating signal falls below it...
	EPwm1Regs.AQCTLA.bit.CAD = AQ_SET; // ...and to be SET to high when the carrier is decreasing and the modulating signal rises above it.
	EPwm1Regs.DBCTL.bit.OUT_MODE = 0; // Bypass the dead-band module
	EPwm1Regs.ETSEL.bit.SOCAEN=1; // Enable SOCA
	EPwm1Regs.ETSEL.bit.SOCASEL=3; // Trigger SOCA when CTR=0 or PRD
	EPwm1Regs.ETPS.bit.SOCAPRD=01; // Generate the SOCA pulse on the first event


	
	GpioCtrlRegs.GPAMUX1.bit.GPIO0 = 01;	// EPWM1A on GPIO0
	GpioCtrlRegs.GPADIR.bit.GPIO0 = 1;	// Output on GPIO0
	GpioCtrlRegs.GPAMUX1.bit.GPIO1 = 01;	// EPWM1B on GPIO1
	GpioCtrlRegs.GPADIR.bit.GPIO1 = 1;	// Output on GPIO1


	GpioCtrlRegs.GPADIR.bit.GPIO16 = 1;	// Output on GPIO16

	// DEBUG
	//EPwm1Regs.CMPA.half.CMPA = 2047;	// 50% duty

	//SysCtrlRegs.PCLKCR0.bit.TBCLKSYNC = 1;

	EDIS;
}

void pwmStart() {
	EALLOW;

	EPwm1Regs.TBCTL.bit.CTRMODE = TB_COUNT_UPDOWN;



	// Initiate a software-sync event on ePWM1; the pulse will carry through to ePWM2 and ePWM3 also, forcing them to be synchronized with ePWM1
	EPwm1Regs.TBCTL.bit.SWFSYNC = 1;

	// DEBUG
	//EPwm1Regs.CMPA.half.CMPA=4000;
	EDIS;
}

void pwmStop() {
	EALLOW;
	EPwm1Regs.TBCTL.bit.CTRMODE = TB_FREEZE;  // How to reset GPIO???
	EPwm1Regs.TBCTR = 0;	// Reset counter
	EPwm1Regs.AQSFRC.bit.OTSFA = 1;
	EPwm2Regs.TBCTL.bit.CTRMODE = TB_FREEZE;  // How to reset GPIO???
	EPwm2Regs.TBCTR = 0;	// Reset counter
	EPwm2Regs.AQSFRC.bit.OTSFA = 1;
	EPwm3Regs.TBCTL.bit.CTRMODE = TB_FREEZE;  // How to reset GPIO???
	EPwm3Regs.TBCTR = 0;	// Reset counter
	EPwm3Regs.AQSFRC.bit.OTSFA = 1;
	EDIS;
}
