/*
 * config.h
 *
 *  Created on: 24/05/2015
 *      Author: broadmea
 */

#ifndef CONFIG_H_
#define CONFIG_H_

#define false 0
#define true !false

// Choosing to name all variables that represent values in the ePWM1 registers with the prefix sw meaning "software" due to the fact that strings like "TBPRD" have already been given a definition in the TI header files
#define SWTBPRD 4095  // Guarantees maximum possible accuracy from the 12-bit ADC; shifts the carrier frequency to 18315 Hz (=TBCLK/(2*TBPRD))
#define FOH_SCALE 3000 // =TBCLK/FS; both in Hz, TBCLK=150 MHz and FS corresponding to a 20us ADC conversion time
#endif /* CONFIG_H_ */
