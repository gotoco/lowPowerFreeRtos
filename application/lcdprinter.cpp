/*
 * lcdprinter.cpp
 *
 *  Created on: 1 gru 2014
 *      Author: Adrian
 */

#include "stm32l1xx.h"
#include "lcdprinter.h"
#include "lcd.h"
#include <string.h>

/*---------------------------------------------------------------------------------------------------------------------+
| local functions
+---------------------------------------------------------------------------------------------------------------------*/

/**
 * \brief Example of use function LCD_WriteChar.
 *
 * Displays "A" whith colon on digit 1 and displays "B" whith point on digit 3.
 *
 */
void LCD_WriteChar_example()
{
	LCD_WriteChar('a',0,1,1);
	LCD_WriteChar('b',1,0,3);
}

/**
 *  \brief Example of use function LCD_WriteString.
 *
 *  Displays string.
 *
 */
void LCD_WriteString_example()
{
	char string[10];
	strcpy(string,"ub.ir.ds");
	LCD_WriteString(string);
}


