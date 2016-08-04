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

	// Add code to configure ePWM1

	EDIS;
}

void pwmStart() {
	// Add code to start PWM (enable counter clock)
}

void pwmStop() {
	// Add code to stop PWM (disable counter clock, reset GPIO)
}

