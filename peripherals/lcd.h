/*
 * lcd.h
 *
 *  Created on: 14 lis 2014
 *      Author: Adrian
 */

#ifndef LCD_H_
#define LCD_H_

/*---------------------------------------------------------------------------------------------------------------------+
| global defines
+---------------------------------------------------------------------------------------------------------------------*/

#define GPIO_COM_CONFIGURATION GPIO_AF11_PP_40MHz
#define GPIO_SEG_CONFIGURATION GPIO_AF11_PP_40MHz


/*---------------------------------------------------------------------------------------------------------------------+
| global functions' declarations
+---------------------------------------------------------------------------------------------------------------------*/

void LCD_Init();

static void LCD_Conv_Char_Seg(char c,bool point,bool column, uint8_t* digit);

void LCD_WriteChar(char ch, bool point, bool column, uint8_t position);

void LCD_WriteString(const char* s);

void LCD_WriteUbirds();

void LCD_WriteChar_example();

void LCD_WriteString_example();

#endif /* LCD_H_ */
