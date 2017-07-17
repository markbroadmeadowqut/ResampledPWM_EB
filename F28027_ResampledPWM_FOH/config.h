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
#define SWTBPRD (Uint16)3750  // Switching frequency is now 8kHz; voltage fed into ADC must vary between 0 and 3.022V rather than 0 and 3.3V
#define SWTBPHS23 (Uint16)2500  // =2*SWTBPRD/3; phase offset for both ePWM2 and ePWM3
#define FOH_SCALE (Uint16)750 // =TBCLK/FS; both in Hz, TBCLK=60 MHz and FS corresponding to a 50us ADC conversion time
#define FOH_SCALE_doubled (Uint16)1500 // =TBCLK/FS; both in Hz, TBCLK=60 MHz and FS corresponding to a 50us ADC conversion time
#define compare_rejection_factor (int16)96
#endif /* CONFIG_H_ */
