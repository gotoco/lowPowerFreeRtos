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

void LCD_Deinit();

static void LCD_Conv_Char_Seg(char c,bool point,bool column, uint8_t* digit);

void LCD_WriteChar(char ch, bool point, bool column, uint8_t position);

void LCD_WriteString(char* s);

void LCD_WriteFloat(float* f, uint8_t d, uint8_t p);

void floatToChar(char *ptr, float number, uint8_t d, uint8_t p);

void LCD_WriteTime(uint8_t* time);



#endif /* LCD_H_ */
