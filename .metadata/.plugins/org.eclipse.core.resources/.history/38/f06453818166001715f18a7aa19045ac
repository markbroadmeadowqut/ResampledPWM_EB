/*
 * CMPA_calc.c
 *
 *  Created on: 16Jan.,2017
 *      Author: Edward
 */

#include "DSP28x_Project.h"
#include "config.h"
#include <math.h>


volatile Uint16 count=0;
Uint16 table[16]={2076,2565,3397,3397,3507,3507,2841,2386,1716,926,667,192,192,516,743,1486};

interrupt void isr_CMPA_calc1(void);


void initCMPAcalc() {
	EALLOW;
	PieVectTable.ADCINT1 = &isr_CMPA_calc1;

	EDIS;

	PieCtrlRegs.PIEIER1.bit.INTx1 = 1;     // Enable ADCINT1
	IER |= M_INT1;
	IER |= M_INT10;

	//DEBUG
	//GpioDataRegs.GPACLEAR.bit.GPIO29 = 1;
	EALLOW;
	GpioCtrlRegs.GPADIR.bit.GPIO29 = 1;				// Set GPIO29 as output
	GpioCtrlRegs.GPADIR.bit.GPIO28 = 1;				// Set GPIO29 as output
	EDIS;

}


#pragma CODE_SECTION(isr_CMPA_calc1, "ramfuncs");
interrupt void isr_CMPA_calc1(void)
{


    EPwm1Regs.CMPA.half.CMPA=table[count&0xF];

    // Make Epwm1B be set or cleared by the fourth bit of count
    if(count&0x8) {
        EPwm1Regs.AQCTLB.bit.ZRO=1; // Clear ePWM1B at the next trough
    } else {
        EPwm1Regs.AQCTLB.bit.ZRO=2; // Set ePWM1B at the next trough
    }


    count=count+1;

	// Acknowledge this interrupt to receive more interrupts from group 1
	PieCtrlRegs.PIEACK.all = PIEACK_GROUP1;
}
