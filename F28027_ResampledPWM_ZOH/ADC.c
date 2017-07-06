
/*
 * ADC.c
 *
 *  Created on: 16Jan.,2017
 *      Author: Edward
 */

#include "DSP28x_Project.h"
#include "config.h"

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
	AdcRegs.SOCPRICTL.bit.SOCPRIORITY = 0x10; // Set SOC priority so that all SOCs are in high priority mode; SOC0 has the smallest number and will therefore always be executed first if any other SOC is triggered accidently
	AdcRegs.ADCSAMPLEMODE.bit.SIMULEN0 = 0; // Set all SOCs to Single Sample Mode aka Sequential Sampling Mode (rather than Simultaneous Sampling Mode)
	AdcRegs.ADCSAMPLEMODE.bit.SIMULEN2 = 0;
	AdcRegs.ADCSAMPLEMODE.bit.SIMULEN4 = 0;
	AdcRegs.ADCSAMPLEMODE.bit.SIMULEN6 = 0;
	AdcRegs.ADCSAMPLEMODE.bit.SIMULEN8 = 0;
	AdcRegs.ADCSAMPLEMODE.bit.SIMULEN10 = 0;
	AdcRegs.ADCSAMPLEMODE.bit.SIMULEN12 = 0;
	AdcRegs.ADCSAMPLEMODE.bit.SIMULEN14 = 0;

	AdcRegs.INTSEL1N2.bit.INT1CONT = 1; // Generate ADCINT1 pulses whever an EOC0 pulse occurs regardless of whether or not the flag bit is cleared
	AdcRegs.INTSEL1N2.bit.INT1E = 1; // enable ADCINT1
	AdcRegs.INTSEL1N2.bit.INT1SEL = 0x00; // EOC0 (assocated with SOC0) is the trigger for ADCINT1

	AdcRegs.INTSEL1N2.bit.INT2CONT = 1;
	AdcRegs.INTSEL1N2.bit.INT2E = 1;
	AdcRegs.INTSEL1N2.bit.INT2SEL = 0X01; // EOC1 (associated with SOC1) is the trigger for ADCINT2

	AdcRegs.INTSEL3N4.bit.INT3CONT = 1;
	AdcRegs.INTSEL3N4.bit.INT3E = 1;
	AdcRegs.INTSEL3N4.bit.INT3SEL = 0x02; // EOC2 (associated with SOC2) is the trigger for ADCINT3

	AdcRegs.ADCINTSOCSEL1.bit.SOC0 = 0x00; // Don't allow any ADCINTs to trigger SOC0

	AdcRegs.ADCSOC0CTL.bit.ACQPS = 0x06; // 6 cycles is the shortest allowed window size for the ADC; I'll have to do some testing of the GTAO output characteristics to see if 6 cycles is long enough to charge the ADC capacitor completely (i.e. long enough to get an accurate conversion)

	AdcRegs.ADCSOC0CTL.bit.CHSEL = 0x09; // Associate input channel ADCINB1 with SOC0

	AdcRegs.ADCSOC0CTL.bit.TRIGSEL = 0x05; // Trigger SOC0 (which feeds EPWM1) from EPWM1, ADCSOCA


	EDIS;
}
