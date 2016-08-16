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
#define swTBPRD 62500  // Therefore fc=1/(2*62500/150e6)=1200Hz
#define fs 40000  
#define TBCLK 150000000

#endif /* CONFIG_H_ */
