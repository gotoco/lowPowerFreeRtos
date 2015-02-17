/*
 * FC30.h
 *
 *  Created on: 28 lis 2014
 *      Author: Adrian
 */

#ifndef FC30_H_
#define FC30_H_


void FC30_Read(uint8_t SlaveAddress, uint8_t SubAddress, uint8_t* DestinationValue);

void FC30_Write(uint8_t SlaveAddress, uint8_t SubAddress, uint8_t SourceValue);

void FC30_Init();


#endif /* FC30_H_ */
