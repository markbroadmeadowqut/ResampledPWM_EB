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

#define PI 3.14159265
// Choosing to name all variables that represent values in the ePWM1 registers with the prefix sw meaning "software" due to the fact that strings like "TBPRD" have already been given a definition in the TI header files
#define SWTBPRD 60000  // 1250Hz
//#define FS 10000.0
//#define TBCLK 150000000.0
//#define FOH_SCALE ((FS/TBCLK)-1.0)
//#define FOH_SCALE2 (FS/TBCLK)
//#define FOH_SCALE3 12500.0 // =FS in Hz (should be a float)
//#define FOH_SCALE4 150000000.0 // =TBCLK in Hz (should be a float)
#endif /* CONFIG_H_ */
