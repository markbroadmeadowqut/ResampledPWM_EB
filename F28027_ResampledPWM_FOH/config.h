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
#define SWTBPRD 60000  // 1250Hz
#define FOH_SCALE3 12500 // =FS in Hz
#define FOH_SCALE4 150000000 // =TBCLK in Hz
#endif /* CONFIG_H_ */
