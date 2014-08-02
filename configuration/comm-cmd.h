/*
 * comm-cmd.h
 *
 *  Created on: 28-10-2012
 *      Author: grochu
 *
 *      This file contains whole commands that are available between MB and
 *   modules. The communication model is describe in uml in file comm.uxf and
 *   can be opened by free uml tools Umlet
 */

#ifndef COMM_CMD_H_
#define COMM_CMD_H_

/*
 * I2C flagues that can be send by Module ------ Mother_Board
 * slave commands:
 */
#define HARD_ERROR        0	 /*An unknown error occurred on Board*/
#define MODULE_READY      1	 /*We are ready to start measurements*/
#define MODULE_NOT_READY  2	 /*We are not ready to start measurements. Calibration in progress*/
#define DATA_READY        3	 /*We have ready data that can be send*/
#define DATA_NOT_READY    4	 /*We don't have part data from ADC that can be send*/
#define ALL_FINE	      5	 /*No problems on board*/


 /* master commands:
 */
#define GET_DATA 		100			/*Send me a data (0x64)*/
#define ARE_YOU_READY	101			/*Are you ready to start ;) (0x65)*/
#define POWER_DOWN		102			/*Go sleep (0x66)*/
#define POWER_UP		103			/*Weak up (0x67)*/
#define START_MEASUR	104	        /*Start timer and adc (0x68)*/
#define HOW_MUCH        105         /*Number of data bytes (0x69)*/
#define EXIT			106         /*End of one communication*/

#define FK_OFF			200			/*Corruption on I2C!*/
/*
 * possible errors that may occurred:
 */
#define NO_SIGNAL_ERROR 254		/*We don't get anything to ADC (0x254)*/



#endif /* COMM_CMD_H_ */
