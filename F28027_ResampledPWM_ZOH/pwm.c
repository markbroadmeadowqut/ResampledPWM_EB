/*
 * pwm.c
 *
 *  Created on: 05/08/2016
 *      Author: broadmea
 */
#include "DSP28x_Project.h"
#include "config.h"

void initPWM() {
    EALLOW; // PWM control registers are protected

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
    EPwm1Regs.TBCTL.bit.HSPCLKDIV = TB_DIV1; // TBCLK = SYSCLKOUT
    EPwm1Regs.TBCTL.bit.CLKDIV = TB_DIV1; // TBCLK = SYSCLKOUT
    EPwm1Regs.TBCTL.bit.SYNCOSEL = 00; // Associate ePWM1SYNCO with ePWM1SYNCI; this should ensure that the software-forced sync event triggers all of the other ePWM modules
    EPwm1Regs.CMPCTL.bit.SHDWAMODE = CC_IMMEDIATE; // load CMPA immediately
    EPwm1Regs.CMPCTL.bit.SHDWBMODE = CC_IMMEDIATE;
    EPwm1Regs.CMPCTL.bit.LOADAMODE = CC_CTR_ZERO; // Don't care about this load functionality as we are using immediate load mode
    EPwm1Regs.CMPCTL.bit.LOADBMODE = CC_CTR_ZERO;
    EPwm1Regs.AQCTLA.bit.CAU = AQ_CLEAR; // I want my PWM signal to be CLEARED to low when the carrier is increasing and the modulating signal falls below it...
    EPwm1Regs.AQCTLA.bit.CAD = AQ_SET; // ...and to be SET to high when the carrier is decreasing and the modulating signal rises above it.
    EPwm1Regs.AQCTLA.bit.PRD = 00; // Do nothing when TBCTR=TBPRD
    EPwm1Regs.AQCTLA.bit.ZRO = 00; // Do nothing when TBCTR=0
    EPwm1Regs.DBCTL.bit.HALFCYCLE = 0; // Dead-band counters clocked at TBCLK rate
    EPwm1Regs.DBCTL.bit.IN_MODE = 00; // Make sure the ePWM1A is used as input for the signals falling-edge delay and rising-edge delay; don't want to use ePWM1B
    EPwm1Regs.DBCTL.bit.POLSEL = 10; // AHC dead-band mode
    EPwm1Regs.DBCTL.bit.OUT_MODE = 11; // AHC dead-band mode
    EPwm1Regs.DBFED = 1; // Use a falling-edge delay of (1/60)us (=DEL*TBCLK=60*(1/60e6))
    EPwm1Regs.DBRED = 1; // Use a rising-edge delay of (1/60)us (=DEL*TBCLK=60*(1/60e6))

    EPwm2Regs.TBPRD = SWTBPRD;
    EPwm2Regs.TBPHS.half.TBPHS = SWTBPHS23;
    //EPwm2Regs.TBCTR = SWTBPHS23;
    EPwm2Regs.TBCTL.bit.CTRMODE = TB_FREEZE;
    EPwm2Regs.TBCTL.bit.PHSDIR = 1; // After the sync event, ePWM2 will count up from phase SWTBPHS23
    EPwm2Regs.TBCTL.bit.PHSEN = TB_ENABLE;
    EPwm2Regs.TBCTL.bit.PRDLD = TB_IMMEDIATE;
    EPwm2Regs.TBCTL.bit.HSPCLKDIV = TB_DIV1;
    EPwm2Regs.TBCTL.bit.CLKDIV = TB_DIV1;
    EPwm2Regs.TBCTL.bit.SYNCOSEL = 00;
    EPwm2Regs.CMPCTL.bit.SHDWAMODE = CC_IMMEDIATE;
    EPwm2Regs.CMPCTL.bit.SHDWBMODE = CC_IMMEDIATE;
    EPwm2Regs.CMPCTL.bit.LOADAMODE = CC_CTR_ZERO;
    EPwm2Regs.CMPCTL.bit.LOADBMODE = CC_CTR_ZERO;
    EPwm2Regs.AQCTLA.bit.CAU = AQ_CLEAR;
    EPwm2Regs.AQCTLA.bit.CAD = AQ_SET;
    EPwm2Regs.AQCTLA.bit.PRD = 00;
    EPwm2Regs.AQCTLA.bit.ZRO = 00;
    EPwm2Regs.DBCTL.bit.HALFCYCLE = 0;
    EPwm2Regs.DBCTL.bit.IN_MODE = 00;
    EPwm2Regs.DBCTL.bit.POLSEL = 10;
    EPwm2Regs.DBCTL.bit.OUT_MODE = 11;
    EPwm2Regs.DBFED = 1;
    EPwm2Regs.DBRED = 1;

    EPwm3Regs.TBPRD = SWTBPRD;
    EPwm3Regs.TBPHS.half.TBPHS = SWTBPHS23;
    //EPwm3Regs.TBCTR = SWTBPHS23;
    EPwm3Regs.TBCTL.bit.CTRMODE = TB_FREEZE;
    EPwm3Regs.TBCTL.bit.PHSDIR = 0; // After the sync event, ePWM3 will count down from phase SWTBPHS23
    EPwm3Regs.TBCTL.bit.PHSEN = TB_ENABLE;
    EPwm3Regs.TBCTL.bit.PRDLD = TB_IMMEDIATE;
    EPwm3Regs.TBCTL.bit.HSPCLKDIV = TB_DIV1;
    EPwm3Regs.TBCTL.bit.CLKDIV = TB_DIV1;
    EPwm3Regs.TBCTL.bit.SYNCOSEL = 00;
    EPwm3Regs.CMPCTL.bit.SHDWAMODE = CC_IMMEDIATE;
    EPwm3Regs.CMPCTL.bit.SHDWBMODE = CC_IMMEDIATE;
    EPwm3Regs.CMPCTL.bit.LOADAMODE = CC_CTR_ZERO;
    EPwm3Regs.CMPCTL.bit.LOADBMODE = CC_CTR_ZERO;
    EPwm3Regs.AQCTLA.bit.CAU = AQ_CLEAR;
    EPwm3Regs.AQCTLA.bit.CAD = AQ_SET;
    EPwm3Regs.AQCTLA.bit.PRD = 00;
    EPwm3Regs.AQCTLA.bit.ZRO = 00;
    EPwm3Regs.DBCTL.bit.HALFCYCLE = 0;
    EPwm3Regs.DBCTL.bit.IN_MODE = 00;
    EPwm3Regs.DBCTL.bit.POLSEL = 10;
    EPwm3Regs.DBCTL.bit.OUT_MODE = 11;
    EPwm3Regs.DBFED = 1;
    EPwm3Regs.DBRED = 1;

    EPwm4Regs.TBPRD = 375;
    EPwm4Regs.TBPHS.half.TBPHS = 0;
    EPwm4Regs.TBCTL.bit.CTRMODE = TB_FREEZE;
    EPwm4Regs.TBCTL.bit.PHSDIR = 0; // After the sync event, ePWM3 will count down from phase SWTBPHS23
    EPwm4Regs.TBCTL.bit.PHSEN = TB_ENABLE;
    EPwm4Regs.TBCTL.bit.PRDLD = TB_IMMEDIATE;
    EPwm4Regs.TBCTL.bit.HSPCLKDIV = TB_DIV1;
    EPwm4Regs.TBCTL.bit.CLKDIV = TB_DIV1;
    EPwm4Regs.TBCTL.bit.SYNCOSEL = 00;
    EPwm4Regs.CMPCTL.bit.SHDWAMODE = CC_IMMEDIATE;
    EPwm4Regs.CMPCTL.bit.SHDWBMODE = CC_IMMEDIATE;
    EPwm4Regs.CMPCTL.bit.LOADAMODE = CC_CTR_ZERO;
    EPwm4Regs.CMPCTL.bit.LOADBMODE = CC_CTR_ZERO;
    EPwm4Regs.AQCTLA.bit.CAU = AQ_CLEAR;
    EPwm4Regs.AQCTLA.bit.CAD = AQ_SET;
    EPwm4Regs.AQCTLA.bit.PRD = 00;
    EPwm4Regs.AQCTLA.bit.ZRO = 00;
    EPwm4Regs.DBCTL.bit.HALFCYCLE = 0;
    EPwm4Regs.DBCTL.bit.IN_MODE = 00;
    EPwm4Regs.DBCTL.bit.POLSEL = 10;
    EPwm4Regs.DBCTL.bit.OUT_MODE = 11;
    EPwm4Regs.DBFED = 1;
    EPwm4Regs.DBRED = 1;
    EPwm4Regs.CMPA.half.CMPA=225;

    GpioCtrlRegs.GPAMUX1.bit.GPIO0 = 01;    // EPWM1A on GPIO0
    GpioCtrlRegs.GPADIR.bit.GPIO0 = 1;  // Output on GPIO0
    GpioCtrlRegs.GPAMUX1.bit.GPIO1 = 01;    // EPWM1B on GPIO1
    GpioCtrlRegs.GPADIR.bit.GPIO1 = 1;  // Output on GPIO1
    GpioCtrlRegs.GPAMUX1.bit.GPIO2 = 01;    // EPWM2A on GPIO2
    GpioCtrlRegs.GPADIR.bit.GPIO2 = 1;  // Output on GPIO2
    GpioCtrlRegs.GPAMUX1.bit.GPIO3 = 01;    // EPWM2B on GPIO3
    GpioCtrlRegs.GPADIR.bit.GPIO3 = 1;  // Output on GPIO3
    GpioCtrlRegs.GPAMUX1.bit.GPIO4 = 01;    // EPWM3A on GPIO4
    GpioCtrlRegs.GPADIR.bit.GPIO4 = 1;  // Output on GPIO4
    GpioCtrlRegs.GPAMUX1.bit.GPIO5 = 01;    // EPWM3B on GPIO5
    GpioCtrlRegs.GPADIR.bit.GPIO5 = 1;  // Output on GPIO5
    GpioCtrlRegs.GPAMUX1.bit.GPIO6 = 01;    // EPWM4A on GPIO6
    GpioCtrlRegs.GPADIR.bit.GPIO6 = 1;  // Output on GPIO6

    GpioCtrlRegs.GPADIR.bit.GPIO16 = 1; // Output on GPIO16

    // DEBUG
    //EPwm1Regs.CMPA.half.CMPA = 2047;  // 50% duty

    //SysCtrlRegs.PCLKCR0.bit.TBCLKSYNC = 1;

    EDIS;
}

void pwmStart() {
    EALLOW;

    EPwm1Regs.TBCTL.bit.CTRMODE = TB_COUNT_UPDOWN;
    EPwm2Regs.TBCTL.bit.CTRMODE = TB_COUNT_UPDOWN;
    EPwm3Regs.TBCTL.bit.CTRMODE = TB_COUNT_UPDOWN;
    EPwm4Regs.TBCTL.bit.CTRMODE = TB_COUNT_UPDOWN;


    // Initiate a software-sync event on ePWM1; the pulse will carry through to ePWM2 and ePWM3 also, forcing them to be synchronized with ePWM1
    EPwm1Regs.TBCTL.bit.SWFSYNC = 1;

    // DEBUG
    //EPwm1Regs.CMPA.half.CMPA=4000;
    EDIS;
}

void pwmStop() {
    EALLOW;
    EPwm1Regs.TBCTL.bit.CTRMODE = TB_FREEZE;  // How to reset GPIO???
    EPwm1Regs.TBCTR = 0;    // Reset counter
    EPwm1Regs.AQSFRC.bit.OTSFA = 1;
    EPwm2Regs.TBCTL.bit.CTRMODE = TB_FREEZE;  // How to reset GPIO???
    EPwm2Regs.TBCTR = 0;    // Reset counter
    EPwm2Regs.AQSFRC.bit.OTSFA = 1;
    EPwm3Regs.TBCTL.bit.CTRMODE = TB_FREEZE;  // How to reset GPIO???
    EPwm3Regs.TBCTR = 0;    // Reset counter
    EPwm3Regs.AQSFRC.bit.OTSFA = 1;
    EDIS;
}
