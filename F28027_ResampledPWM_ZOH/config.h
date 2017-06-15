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
#define SWTBPRD (Uint16)3750 // 8kHz switching frequency; need to rescale voltage fed into the ADC so that it varies between 0 and 3.022V rather than 0 amd 3.3V
#define SWTBPHS23 (Uint16)2730  // =2*SWTBPRD/3; phase offset for both ePWM2 and ePWM3
#define CMPB_increment (int16)750 // 12.5us resampling period, but since the triggering is internal this could be reduced further
#endif /* CONFIG_H_ */