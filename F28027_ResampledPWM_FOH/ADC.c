/*
 * ADC.c
 *
 *  Created on: 16Jan.,2017
 *      Author: Edward
 */

#include "DSP28x_Project.h"
#include "config.h"
#
void initADC() {
	EALLOW;
	SysCtrlRegs.PCLKCR0.bit.ADCENCLK = 1; // enable the ADC clock; must be done before writing to any of the ADC registers
	AdcRegs.ADCCTL1.bit.ADCREFSEL = 0; // use the internal reference so that RTDS output values need to be between 0 and 3.3V
	AdcRegs.ADCCTL1.bit.ADCPWDN = 1; // Power up the ADC
	AdcRegs.ADCCTL1.bit.ADCBGPWD = 1; // Power up the band-gap circuit (the circuit we use for the 3.3V internal reference??)
	AdcRegs.ADCCTL1.bit.ADCREFPWD = 1; // Power up the Reference Buffer circuitry of the ADC (not sure what this one actually does)...
	AdcRegs.ADCCTL1.bit.ADCENABLE = 1; // enable the ADC
	AdcRegs.ADCCTL1.bit.INTPULSEPOS = 1; // generate the FOH_interrupt pulse 1 cycle before latching conversion result to the result register (how do we know this won't be too early??)
	AdcRegs.ADCCTL1.bit.VREFLOCONV = 0; // disable ADCINB5 connection to VREFLO
	AdcRegs.ADCCTL1.bit.TEMPCONV = 0; // disable ADCINA5 connection to temperature sensor
	AdcRegs.ADCCTL2.bit.ADCNONOVERLAP = 1; // Overlap of sample is not allowed; I'm assuming this means that the sampler that writes to the ADC result register will not do so until the conversion process is finished???
	AdcRegs.ADCCTL2.bit.CLKDIV2EN = 0; // not sure why you would want to divide the ADC clock by 2...
	AdcRegs.INTSEL1N2.bit.INT1CONT = 1; // I will use ADCINT1 to trigger the FOH-ISR and I want this ISR to be called every time an EOC happens on SOC0...(In other words I don't want to use the ADCINTFLGCLR or ADCINTOVFCLR registers)
	AdcRegs.INTSEL1N2.bit.INT1E = 1; // Enable ADCINT1 (I am pretty sure all of the others are disabled by default)
	AdcRegs.INTSEL1N2.bit.INT1SEL = 0; // Select EOC0 as the trigger for ADCINT1
	AdcRegs.SOCPRICTL.bit.SOCPRIORITY = 0x10; // Set SOC priority so that all SOCs are in high priority mode; SOC0 has the smallest number and will therefore always be executed first if any other SOC is triggered accidently
	AdcRegs.ADCSAMPLEMODE.bit.SIMULEN0 = 0; // Set all SOCs to Single Sample Mode aka Sequential Sampling Mode (rather than Simultaneous Sampling Mode)
	AdcRegs.ADCSAMPLEMODE.bit.SIMULEN2 = 0;
	AdcRegs.ADCSAMPLEMODE.bit.SIMULEN4 = 0;
	AdcRegs.ADCSAMPLEMODE.bit.SIMULEN6 = 0;
	AdcRegs.ADCSAMPLEMODE.bit.SIMULEN8 = 0;
	AdcRegs.ADCSAMPLEMODE.bit.SIMULEN10 = 0;
	AdcRegs.ADCSAMPLEMODE.bit.SIMULEN12 = 0;
	AdcRegs.ADCSAMPLEMODE.bit.SIMULEN14 = 0;
	AdcRegs.ADCINTSOCSEL1.all = 0; // Don't allow any ADCINTs to trigger any SOCs
	AdcRegs.ADCINTSOCSEL2.all = 0;
	AdcRegs.ADCSOC0CTL.bit.TRIGSEL = 0x01; // Use CPU Timer 0 as the trigger for SOC0
	AdcRegs.ADCSOC0CTL.bit.CHSEL = 0x01; // Associate input channel ADCINA1 with SOC0 (avoiding ADCINA0 as it is sometimes used for VREFHI)
	AdcRegs.ADCSOC0CTL.bit.ACQPS = 0x06; // 6 cycles is the shortest allowed window size for the ADC; I'll have to do some testing of the GTAO output characteristics to see if 6 cycles is long enough to charge the ADC capacitor completely (i.e. long enough to get an accurate conversion)

	//// Use XINT2 as SOC0 trigger
	//AdcRegs.ADCSOC0CTL.bit.TRIGSEL = 0x04;
	//GpioIntRegs.GPIOXINT2SEL.bit.GPIOSEL = 00011; // Assign XINT2 to GPIO3
	//XIntruptRegs.XINT2CR.bit.POLARITY = 11; // trigger XINT2 on both a rising and falling edge
	//XIntruptRegs.XINT2CR.bit.ENABLE = 1; // enable XINT2
	//GpioCtrlRegs.GPAQSEL1.bit.GPIO3 = 11; // asynchronous qualification of XINT2...wish I knew what this actually means...
	//// Don't need to touch the GPACTRL register if using asynchronous qualification
	//GpioCtrlRegs.GPAMUX1.bit.GPIO0 = 1;	// EPWM1A on GPIO0
	//GpioCtrlRegs.GPADIR.bit.GPIO0 = 1;	// Output on GPIO0
	//GpioDataRegs.GPACLEAR.bit.GPIO2 = 1;
	//GpioCtrlRegs.GPADIR.bit.GPIO2 = 1;				// Set GPIO2 as output
	//PieCtrlRegs.PIEIER1.bit.INTx5 = 1;     // Enable PIE Group 1, INT5, XINT2


	EDIS;
}
