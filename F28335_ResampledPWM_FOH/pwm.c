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

	// Use all of the definitions in the EPwm manual to help program the registers correctly
	#define TB_COUNT_UP 0x0
	#define TB_COUNT_DOWN 0x1
	#define TB_COUNT_UPDOWN 0x2
	#define TB_FREEZE 0x3
	#define TB_DISABLE 0x0
	#define TB_ENABLE 0x1
	#define TB_SHADOW 0x0
	#define TB_IMMEDIATE 0x1
	#define TB_SYNC_IN 0x0
	#define TB_CTR_ZERO 0x1
	#define TB_CTR_CMPB 0x2
	#define TB_SYNC_DISABLE 0x3
	#define TB_DIV1 0x0
	#define TB_DIV2 0x1
	#define TB_DIV4 0x2
	#define TB_DOWN 0x0
	#define TB_UP 0x1
	#define CC_CTR_ZERO 0x0
	#define CC_CTR_PRD 0x1
	#define CC_CTR_ZERO_PRD 0x2 #
    #define CC_LD_DISABLE 0x3
	#define CC_SHADOW 0x0
	#define CC_IMMEDIATE 0x1
    #define AQ_NO_ACTION 0x0
    #define AQ_CLEAR 0x1
    #define AQ_SET 0x2
    #define AQ_TOGGLE 0x3
    #define DB_DISABLE 0x0
    #define DBA_ENABLE 0x1
    #define DBB_ENABLE 0x2
    #define DB_FULL_ENABLE 0x3
    #define DB_ACTV_HI 0x0
    #define DB_ACTV_LOC 0x1
    #define DB_ACTV_HIC 0x2
    #define DB_ACTV_LO 0x3 // PCCTL (chopper control)
    #define CHP_ENABLE 0x0
    #define CHP_DISABLE 0x1
    #define CHP_DIV1 0x0
	#define CHP_DIV2 0x1
	#define CHP_DIV3 0x2
	#define CHP_DIV4 0x3
	#define CHP_DIV5 0x4
	#define CHP_DIV6 0x5
	#define CHP_DIV7 0x6
	#define CHP_DIV8 0x7
	#define CHP1_8TH 0x0
	#define CHP2_8TH 0x1
	#define CHP3_8TH 0x2
	#define CHP4_8TH 0x3
	#define CHP5_8TH 0x4
	#define CHP6_8TH 0x5 #
    #define CHP7_8TH 0x6
	#define TZ_ENABLE 0x0
	#define TZ_DISABLE 0x1
	#define TZ_HIZ 0x0
	#define TZ_FORCE_HI 0x1
	#define TZ_FORCE_LO 0x2
	#define TZ_DISABLE 0x3
	#define ET_CTR_ZERO 0x1
	#define ET_CTR_PRD 0x2
	#define ET_CTRU_CMPA 0x4
	#define ET_CTRD_CMPA 0x5
	#define ET_CTRU_CMPB 0x6
	#define ET_CTRD_CMPB 0x7
	#define ET_DISABLE 0x0
	#define ET_1ST 0x1
	#define ET_2ND 0x2
	#define ET_3RD 0x3
	
	
	EPwm1Regs.TBPRD.all = swTBPRD; // fc = 1/(2*65536/TBCLK) = 1/(2*65535/150000000) = 1.1444kHz
	EPwm1Regs.CMPA.all = swTBPRD; // Just to make sure a switching event doesn't happen on initialisation
	EPwm1Regs.CMPB.all = swTBPRD; // Not going to use EPwm1B, but will program it the same as EPwmA just to be safe
	EPwm1Regs.TBPHS.all = 0; // Set Phase register to zero (don't plan to use synchronisation functionality at this stage)
	EPwm1Regs.TBCTR.all = 0; // clear TB counter
	
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
	EPwm1Regs.AQCTLA.bit.CBU = AQ_NO_ACTION; // Make sure that EPwm1A doesn't do anything when the CTR is equal to CMPB
	EPwm1Regs.AQCTLA.bit.CBD = AQ_NO_ACTION; // Make sure that EPwm1A doesn't do anything when the CTR is equal to CMPB   
    EPwm1Regs.AQCTLA.bit.PRD = AQ_NO_ACTION; // Make sure that EPwm1A doesn't do anything when the CTR is equal to TBPRD   
    EPwm1Regs.AQCTLA.bit.ZRO = AQ_NO_ACTION; // Make sure that EPwm1A doesn't do anything when the CTR is equal to 0   
        
	
	EPwm1Regs.AQCTLB.bit.CBU = AQ_CLEAR; // Don't really care very much what EPwmB is doing, as long as it doesn't interfere with EPwmA...
	EPwm1Regs.AQCTLB.bit.CBD = AQ_SET;
	
	EDIS;
}

void pwmStart() {
	EPwm1Regs.TBCTL.bit.CTRMODE = TB_COUNT_UPDOWN

}

void pwmStop() {
	EPwm1Regs.TBCTL.bit.CTRMODE = TB_FREEZE  // How to reset GPIO???
}

