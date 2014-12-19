/*
 * M41T56C64.cpp
 *
 *  Created on: 8 gru 2014
 *      Author: Adrian
 */

#include "stm32l1xx.h"
#include "M41T56C64\M41T56C64.h"
#include "i2c.h"

/**
 * \brief	Initializes RTC Module M41T56C64. Writing to RTC memory desired Time.
 *
 * \param time	Pointer to table which contains hours, minutes and seconds
 */
void M41T56C64_Init(uint8_t* time)
{
	uint8_t tab[2]={M41T56_SECONDS, *(time+2)};

	uint8_t temp;

	i2cInitialize();

	// Converting to BCD format
	// Hours
	temp=*time;
	temp=*(time)%10;
	temp+=( ( *(time)/10 ) << 4 );
	*(time)=temp;

	// Minutes
	temp=*(time+1);
	temp=*(time+1)%10;
	temp+=( ( *(time+1)/10 ) << 4);
	*(time+1)=temp;

	// Seconds
	temp=*(time+2);
	temp=*(time+2)%10;
	temp+=( ( *(time+2)/10 ) << 4);
	*(time+2)=temp;


	// setting seconds
	tab[0]=M41T56_SECONDS;
	tab[1]=*(time+2);
	i2cWrite(M41T56_SlaveAddress, tab, 2);

	// setting minutes
	tab[0]=M41T56_MINUTES;
	tab[1]=*(time+1);
	i2cWrite(M41T56_SlaveAddress, tab, 2);

	// setting hours
	tab[0]=M41T56_HOURS;
	tab[1]=*time;
	i2cWrite(M41T56_SlaveAddress, tab, 2);
}

/**
 * \brief	Redaing from RTC actual time in BCD format
 *
 * \param tab	Pointer to table where hours, minutes and seconds will be written
 */
void M41T56C64_ReadTime(uint8_t* tab)
{
	// Reading hours in BCD format
	i2cWriteOneByteWhithoutStop(M41T56_SlaveAddress, M41T56_HOURS);
	i2cRead(M41T56_SlaveAddress, tab, 1);

	tab++;

	// Reading minutes in BCD format
	i2cWriteOneByteWhithoutStop(M41T56_SlaveAddress, M41T56_MINUTES);
	i2cRead(M41T56_SlaveAddress, tab, 1);

	tab++;

	// Reading seconds in BCD format
	i2cWriteOneByteWhithoutStop(M41T56_SlaveAddress, M41T56_SECONDS);
	i2cRead(M41T56_SlaveAddress, tab, 1);
}

/**
 * \brief	Converting time from BCD to normal format
 *
 * \param tab	Pointer to table which contains number of hours, minutes and seconds in BCD format
 */
void M41T56C64_ConvertToInt(uint8_t* tab)
{
	uint8_t bufor;

	// Hours
	bufor=( *tab & 0x0F );
	bufor+=( ( ( *tab & 0b00110000 ) >> 4 ) * 10 );
	*tab=bufor;

	tab++;
	// Minutes
	bufor=( *tab & 0x0F );
	bufor+=( ( ( *tab & 0b01110000 ) >> 4 )* 10 );
	*tab=bufor;

	tab++;
	// Seconds
	bufor=( *tab & 0x0F );
	bufor+=( ( ( *tab & 0b01110000 ) >> 4 ) * 10 );
	*tab=bufor;
}
/**
 * \brief Converts Hours, Minutes and seconds written in uint8_t table to char table.
 *
 * \param tab	table which contains hours, minutes and seconds in uint8_t format
 * \param text	table to which time will be printed
 */
void M41T56C64_ConvertToString(uint8_t* time, char* text)
{
	// Hours
	text[0]=(char)( ( *time / 10 ) + 48 );
	text[1]=(char)( ( *time % 10 ) + 48 );
	text[2]=':';

	time++;

	// Minutes
	text[3]=(char)( ( *time / 10 ) + 48 );
	text[4]=(char)( ( *time % 10 ) + 48 );
	text[5]=':';

	time++;

	// Seconds
	text[6]=(char)( ( *time / 10 ) + 48 );
	text[7]=(char)( ( *time % 10 ) + 48 );

	text[8]='\0';
}


