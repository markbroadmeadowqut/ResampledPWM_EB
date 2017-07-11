/*
 * main.c
 */

#include "DSP28x_Project.h"
#include "config.h"
#include "pwm.h"
#include "timer.h"
#include "ADC.h"
#include "CMPA_calc.h"



//extern Uint16 RamfuncsLoadStart;
//extern Uint16 RamfuncsLoadEnd;
//extern Uint16 RamfuncsRunStart;

void init() {
	InitSysCtrl();	// Initialise PLL, Clocks, WD
	InitGpio();		// Set GPIO in default state

	DINT;			// Disable interrupts

	InitPieCtrl();	// Initialise the PIE control registers to their default state.

	// Disable CPU interrupts and clear all CPU interrupt flags:
	IER = 0x0000;
	IFR = 0x0000;

	// Initialise the PIE vector table with pointers to the shell Interrupt Service Routines (ISR).
	InitPieVectTable();

	// Copy time critical code and Flash setup code to RAM
	//MemCopy(&RamfuncsLoadStart, &RamfuncsLoadEnd, &RamfuncsRunStart);

	// INITIALISE PERIPHERALS
	initCMPAcalc();
	initPWM();
	//inittriggerSOC0();
	initADC();
	//initTimer(); // Important that timer is activated AFTER the ADC and PWM have been initialised

	PieCtrlRegs.PIECTRL.bit.ENPIE = 1;     // Enable the PIE block

	// Enable global Interrupts and higher priority real-time debug events:
	EINT;   // Enable Global interrupt INTM
	ERTM;   // Enable Global realtime interrupt DBGM
}

void main(void) {
	init();

	pwmStart();

	for (;;) {
		//Loop forever
	}

	// Code should never reach this point
}
