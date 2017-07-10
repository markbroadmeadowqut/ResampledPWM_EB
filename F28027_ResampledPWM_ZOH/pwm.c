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
	EPwm1Regs.CMPCTL.bit.SHDWAMODE = CC_SHADOW; // load CMPA immediately
	EPwm1Regs.CMPCTL.bit.SHDWBMODE = CC_SHADOW;
    EPwm1Regs.CMPCTL.bit.LOADAMODE = 2; // load at both peaks and troughs
    EPwm1Regs.CMPCTL.bit.LOADBMODE = 2;
	EPwm1Regs.AQCTLA.bit.CAU = AQ_CLEAR; // I want my PWM signal to be CLEARED to low when the carrier is increasing and the modulating signal falls below it...
	EPwm1Regs.AQCTLA.bit.CAD = AQ_SET; // ...and to be SET to high when the carrier is decreasing and the modulating signal rises above it.
	EPwm1Regs.DBCTL.bit.OUT_MODE = 0; // Bypass the dead-band module so that we can use ePWM1B to represent modulating phase
	EPwm1Regs.ETSEL.bit.INTEN = 1;
	EPwm1Regs.ETSEL.bit.INTSEL = 3; // Trigger the interrupt at the peaks and the troughs

	
	GpioCtrlRegs.GPAMUX1.bit.GPIO0 = 01;	// EPWM1A on GPIO0
	GpioCtrlRegs.GPADIR.bit.GPIO0 = 1;	// Output on GPIO0
	GpioCtrlRegs.GPAMUX1.bit.GPIO1 = 01;	// EPWM1B on GPIO1
	GpioCtrlRegs.GPADIR.bit.GPIO1 = 1;	// Output on GPIO1
	GpioCtrlRegs.GPAMUX1.bit.GPIO2 = 01;	// EPWM2A on GPIO2
	GpioCtrlRegs.GPADIR.bit.GPIO2 = 1;	// Output on GPIO2
	GpioCtrlRegs.GPAMUX1.bit.GPIO3 = 01;	// EPWM2B on GPIO3
	GpioCtrlRegs.GPADIR.bit.GPIO3 = 1;	// Output on GPIO3
	GpioCtrlRegs.GPAMUX1.bit.GPIO4 = 01;	// EPWM3A on GPIO4
	GpioCtrlRegs.GPADIR.bit.GPIO4 = 1;	// Output on GPIO4
	GpioCtrlRegs.GPAMUX1.bit.GPIO5 = 01;	// EPWM3B on GPIO5
	GpioCtrlRegs.GPADIR.bit.GPIO5 = 1;	// Output on GPIO5

	GpioCtrlRegs.GPADIR.bit.GPIO16 = 1;	// Output on GPIO16

	// DEBUG
	//EPwm1Regs.CMPA.half.CMPA = 2047;	// 50% duty

	//SysCtrlRegs.PCLKCR0.bit.TBCLKSYNC = 1;

	EDIS;
}

void pwmStart() {
	EALLOW;

	EPwm1Regs.TBCTL.bit.CTRMODE = TB_COUNT_UPDOWN;



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
