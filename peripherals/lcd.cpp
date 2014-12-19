/*
 * lcd.cpp
 *
 *  Created on: 14 lis 2014
 *      Author: Adrian Ratajczyk
 */

#include "stm32l1xx.h"
#include "gpio.h"
#include "lcd.h"
#include "bsp.h"
#include <string.h>
#include <stdlib.h>

/**
  @verbatim
================================================================================
                              GLASS LCD MAPPING
================================================================================
      A
     _  ----------
COL |_| |\   |J  /|
       F| H  |  K |B
     _  |  \ | /  |
COL |_| --G-- --M--
        |   /| \  |
       E|  Q |  N |C
     _  | /  |P  \|
DP  |_| -----------
      D

 An LCD character coding is based on the following matrix:
      { E , D , P , N   }
      { M , C , COL , DP}
      { B , A , K , J   }
      { G , F , Q , H   }

 The character 'A' for example is:
  -------------------------------
LSB   { 1 , 0 , 0 , 0   }
      { 1 , 1 , 0 , 0   }
      { 1 , 1 , 0 , 0   }
MSB   { 1 , 1 , 0 , 0   }
      -------------------
  'A' =  F    E   0   0 hexa

  @endverbatim
*/

/*---------------------------------------------------------------------------------------------------------------------+
 | local variables
 +---------------------------------------------------------------------------------------------------------------------*/

/* Constant table for cap characters 'A' --> 'Z' */
const uint16_t CapLetterMap[26]=
{
    /* A      B      C      D      E      F      G      H      I  */
    0xFE00,0x6714,0x1d00,0x4714,0x9d00,0x9c00,0x3f00,0xfa00,0x0014,
    /* J      K      L      M      N      O      P      Q      R  */
    0x5300,0x9841,0x1900,0x5a48,0x5a09,0x5f00,0xFC00,0x5F01,0xFC01,
    /* S      T      U      V      W      X      Y      Z  */
    0xAF00,0x0414,0x5b00,0x18c0,0x5a81,0x00c9,0x0058,0x05c0
};

/* Constant table for number '0' --> '9' */
const uint16_t NumberMap[10]=
{
	/* 0      1      2      3      4      5      6      7      8      9  */
    0x5F00,0x4200,0xF500,0x6700,0xEa00,0xAF00,0xBF00,0x04600,0xFF00,0xEF00
};

/*---------------------------------------------------------------------------------------------------------------------+
| local functions
+---------------------------------------------------------------------------------------------------------------------*/

/**
 * \brief Initialazes LCD driver
 *
 * Enables RCC clocks for LCD driver. Enables GPIO ports. Configures LCD driver to work with default LCD pannel on STM32L152C-discovery board.
 *
 */
void LCD_Init()
{
   	// getting access to RCC registers
	PWR->CR |= PWR_CR_DBP;

	// select the RTC clock source
	RCC->CSR |= (RCC_CSR_RTCSEL_LSE | RCC_CSR_RTCEN | RCC_CSR_LSEON);

	// Enable rcc clock fot gpio
	RCC->AHBENR |= RCC_AHBENR_GPIOAEN | RCC_AHBENR_GPIOBEN | RCC_AHBENR_GPIOCEN;

	// Enable rcc clock for lcd
	RCC->APB1ENR|=RCC_APB1ENR_LCDEN;

	// RTC and LCD clock source selection
	// 00 - noclock
	// 01 - LSE     RCC_CSR_RTCSEL_LSE
	// 10 - LSI     RCC_CSR_RTCSEL_LSI
	// 11 - HSE		RCC_CSR_RTCSEL_HSE

	// enabling RTC clock
	RCC->CSR |= (RCC_CSR_RTCSEL_LSI | RCC_CSR_RTCEN);

	// Configure the LCD GPIO pins as alternate functions
	// Comons
	gpioConfigurePin(LCD_COM0_GPIO, LCD_COM0_pin, GPIO_COM_CONFIGURATION);
	gpioConfigurePin(LCD_COM1_GPIO, LCD_COM1_pin, GPIO_COM_CONFIGURATION);
	gpioConfigurePin(LCD_COM2_GPIO, LCD_COM2_pin, GPIO_COM_CONFIGURATION);
	gpioConfigurePin(LCD_COM3_GPIO, LCD_COM3_pin, GPIO_COM_CONFIGURATION);

	// Segments
	gpioConfigurePin(LCD_SEG0_GPIO, LCD_SEG0_pin, GPIO_SEG_CONFIGURATION);
	gpioConfigurePin(LCD_SEG1_GPIO, LCD_SEG1_pin, GPIO_SEG_CONFIGURATION);
	gpioConfigurePin(LCD_SEG2_GPIO, LCD_SEG2_pin, GPIO_SEG_CONFIGURATION);
	gpioConfigurePin(LCD_SEG3_GPIO, LCD_SEG3_pin, GPIO_SEG_CONFIGURATION);
	gpioConfigurePin(LCD_SEG4_GPIO, LCD_SEG4_pin, GPIO_SEG_CONFIGURATION);
	gpioConfigurePin(LCD_SEG5_GPIO, LCD_SEG5_pin, GPIO_SEG_CONFIGURATION);
	gpioConfigurePin(LCD_SEG6_GPIO, LCD_SEG6_pin, GPIO_SEG_CONFIGURATION);
	gpioConfigurePin(LCD_SEG7_GPIO, LCD_SEG7_pin, GPIO_SEG_CONFIGURATION);
	gpioConfigurePin(LCD_SEG8_GPIO, LCD_SEG8_pin, GPIO_SEG_CONFIGURATION);
	gpioConfigurePin(LCD_SEG9_GPIO, LCD_SEG9_pin, GPIO_SEG_CONFIGURATION);
	gpioConfigurePin(LCD_SEG10_GPIO, LCD_SEG10_pin, GPIO_SEG_CONFIGURATION);
	gpioConfigurePin(LCD_SEG11_GPIO, LCD_SEG11_pin, GPIO_SEG_CONFIGURATION);
	gpioConfigurePin(LCD_SEG12_GPIO, LCD_SEG12_pin, GPIO_SEG_CONFIGURATION);
	gpioConfigurePin(LCD_SEG13_GPIO, LCD_SEG13_pin, GPIO_SEG_CONFIGURATION);
	gpioConfigurePin(LCD_SEG14_GPIO, LCD_SEG14_pin, GPIO_SEG_CONFIGURATION);
	gpioConfigurePin(LCD_SEG15_GPIO, LCD_SEG15_pin, GPIO_SEG_CONFIGURATION);
	gpioConfigurePin(LCD_SEG16_GPIO, LCD_SEG16_pin, GPIO_SEG_CONFIGURATION);
	gpioConfigurePin(LCD_SEG17_GPIO, LCD_SEG17_pin, GPIO_SEG_CONFIGURATION);
	gpioConfigurePin(LCD_SEG18_GPIO, LCD_SEG18_pin, GPIO_SEG_CONFIGURATION);
	gpioConfigurePin(LCD_SEG19_GPIO, LCD_SEG19_pin, GPIO_SEG_CONFIGURATION);
	gpioConfigurePin(LCD_SEG20_GPIO, LCD_SEG20_pin, GPIO_SEG_CONFIGURATION);
	gpioConfigurePin(LCD_SEG21_GPIO, LCD_SEG21_pin, GPIO_SEG_CONFIGURATION);
	gpioConfigurePin(LCD_SEG22_GPIO, LCD_SEG22_pin, GPIO_SEG_CONFIGURATION);
	gpioConfigurePin(LCD_SEG23_GPIO, LCD_SEG23_pin, GPIO_SEG_CONFIGURATION);

	// multiplexed pin enabled
	LCD->CR|=LCD_CR_MUX_SEG;

	// BIAS in LCD_CR 1/3
	LCD->CR|=LCD_CR_BIAS_1;

	// DUTY in LCD_CR 1/4
	LCD->CR|=(LCD_CR_DUTY_0 | LCD_CR_DUTY_1);

	// Voltage internal source

	// Enable the display (LCDEN bit in LCD_FCR register)
	LCD->CR|=LCD_CR_LCDEN;

	// prescaler 1

	// divider 31
	LCD->FCR|=LCD_FCR_DIV;

	// blink mode off

	// blink frequency 32
	LCD->FCR|=LCD_FCR_BLINKF_1;

	// Program the contrast (CC bits in LCD_CR)
	LCD->FCR|=(LCD_FCR_CC_2);

	// deadtime 0

	// pulse on duration 4
	LCD->FCR|=LCD_FCR_PON_2;
}

/**
 * \brief	Disables LCD
 */
void LCD_Deinit()
{
	LCD->CR&=~(LCD_CR_LCDEN);
}

/**
 * \brief Converts char to digit on the LCD.
 *
 * \param [in] Char to convert
 * \param [in] Point display enable. Values 0 or 1.
 * \param [in] Colon display enable. Values 0 or 1.
 * \param [in] Digit number. Values from 1 to 6.
 *
 */
static void LCD_Conv_Char_Seg(char c,bool point,bool column, uint8_t* digit)
{
    uint16_t ch = 0 ;
    uint8_t i,j;

    switch (c)
    {
        case ' ' :
        	ch = 0x00;
        	break;
        case '0':
        case '1':
        case '2':
        case '3':
        case '4':
        case '5':
        case '6':
        case '7':
        case '8':
        case '9':
        	ch = NumberMap[c-0x30];
        	break;

        default:
        	/* The character c is one letter in upper case*/
        	if ( (c < 0x5b) && (c > 0x40) ){
        	   	ch = CapLetterMap[c-'A'];
        	}
        	/* The character c is one letter in lower case*/
        	if ( (c <0x7b) && ( c> 0x60) ){
        		ch = CapLetterMap[c-'a'];
        	}
        	break;
    }

    /* Set the digital point can be displayed if the point is on */
    if (point){
    	ch |= 0x0002;
    }

    /* Set the "COL" segment in the character that can be displayed if the column is on */
    if (column){
    	ch |= 0x0020;
    }

    for (i = 12,j=0 ;j<4; i-=4,j++){
    	digit[j] = (ch >> i) & 0x0f; //To isolate the less signifiant dibit
    }
}

/**
 * \brief  Writes a char on the LCD.
 *
 * \param [in] Char to display
 * \param [in] Point display enable. Values 0 or 1.
 * \param [in] Colon display enable. Values 0 or 1.
 * \param [in] Digit number. Values from 1 to 6.
 *
 */
void LCD_WriteChar(char ch, bool point, bool column, uint8_t position)
{
    uint8_t digit[4];     /* Digit frame buffer */

    /* To convert displayed character in segment in array digit */
    LCD_Conv_Char_Seg(ch,point,column,digit);

    while((LCD->SR&LCD_SR_UDR)!=0);

    switch (position)
    {
      /* Position 1 on LCD (Digit1)*/
      case 1:
    	LCD->RAM[0] &= 0xcffffffc;
    	LCD->RAM[2] &= 0xcffffffc;
    	LCD->RAM[4] &= 0xcffffffc;
    	LCD->RAM[6] &= 0xcffffffc;

    	LCD->RAM[0] |= ((digit[0]& 0x0c) << 26 ) | (digit[0]& 0x03) ; // 1G 1B 1M 1E
   		LCD->RAM[2] |= ((digit[1]& 0x0c) << 26 ) | (digit[1]& 0x03) ; // 1F 1A 1C 1D
   		LCD->RAM[4] |= ((digit[2]& 0x0c) << 26 ) | (digit[2]& 0x03) ; // 1Q 1K 1Col 1P
   		LCD->RAM[6] |= ((digit[3]& 0x0c) << 26 ) | (digit[3]& 0x03) ; // 1H 1J 1DP 1N

   		break;

   	  /* Position 2 on LCD (Digit2)*/
      case 2:
    	LCD->RAM[0] &= 0xf3ffff03;
   		LCD->RAM[2] &= 0xf3ffff03;
   		LCD->RAM[4] &= 0xf3ffff03;
   		LCD->RAM[6] &= 0xf3ffff03;

    	LCD->RAM[0] |= ((digit[0]& 0x0c) << 24 )|((digit[0]& 0x02) << 6 )|((digit[0]& 0x01) << 2 ) ; // 2G 2B 2M 2E
   		LCD->RAM[2] |= ((digit[1]& 0x0c) << 24 )|((digit[1]& 0x02) << 6 )|((digit[1]& 0x01) << 2 ) ; // 2F 2A 2C 2D
   		LCD->RAM[4] |= ((digit[2]& 0x0c) << 24 )|((digit[2]& 0x02) << 6 )|((digit[2]& 0x01) << 2 ) ; // 2Q 2K 2Col 2P
   		LCD->RAM[6] |= ((digit[3]& 0x0c) << 24 )|((digit[3]& 0x02) << 6 )|((digit[3]& 0x01) << 2 ) ; // 2H 2J 2DP 2N

    	break;

      /* Position 3 on LCD (Digit3)*/
      case 3:
    	LCD->RAM[0] &= 0xfcfffcff;
   		LCD->RAM[2] &= 0xfcfffcff;
   		LCD->RAM[4] &= 0xfcfffcff;
   		LCD->RAM[6] &= 0xfcfffcff;

    	LCD->RAM[0] |= ((digit[0]& 0x0c) << 22 ) | ((digit[0]& 0x03) << 8 ) ; // 3G 3B 3M 3E
    	LCD->RAM[2] |= ((digit[1]& 0x0c) << 22 ) | ((digit[1]& 0x03) << 8 ) ; // 3F 3A 3C 3D
   		LCD->RAM[4] |= ((digit[2]& 0x0c) << 22 ) | ((digit[2]& 0x03) << 8 ) ; // 3Q 3K 3Col 3P
   		LCD->RAM[6] |= ((digit[3]& 0x0c) << 22 ) | ((digit[3]& 0x03) << 8 ) ; // 3H 3J 3DP 3N

   		break;

      /* Position 4 on LCD (Digit4)*/
      case 4:
    	LCD->RAM[0] &= 0xffcff3ff;
   		LCD->RAM[2] &= 0xffcff3ff;
   		LCD->RAM[4] &= 0xffcff3ff;
   		LCD->RAM[6] &= 0xffcff3ff;

    	LCD->RAM[0] |= ((digit[0]& 0x0c) << 18 ) | ((digit[0]& 0x03) << 10 ) ; // 4G 4B 4M 4E
   		LCD->RAM[2] |= ((digit[1]& 0x0c) << 18 ) | ((digit[1]& 0x03) << 10 ) ; // 4F 4A 4C 4D
   		LCD->RAM[4] |= ((digit[2]& 0x0c) << 18 ) | ((digit[2]& 0x03) << 10 ) ; // 4Q 4K 4Col 4P
   		LCD->RAM[6] |= ((digit[3]& 0x0c) << 18 ) | ((digit[3]& 0x03) << 10 ) ; // 4H 4J 4DP 4N

   		break;

      /* Position 5 on LCD (Digit5)*/
      case 5:
        LCD->RAM[0] &= 0xfff3cfff;
    	LCD->RAM[2] &= 0xfff3cfff;
    	LCD->RAM[4] &= 0xfff3efff;
    	LCD->RAM[6] &= 0xfff3efff;

    	LCD->RAM[0] |= ((digit[0]& 0x0c) << 16 ) | ((digit[0]& 0x03) << 12 ) ; // 5G 5B 5M 5E
    	LCD->RAM[2] |= ((digit[1]& 0x0c) << 16 ) | ((digit[1]& 0x03) << 12 ) ; // 5F 5A 5C 5D
    	LCD->RAM[4] |= ((digit[2]& 0x0c) << 16 ) | ((digit[2]& 0x01) << 12 ) ; // 5Q 5K   5P
    	LCD->RAM[6] |= ((digit[3]& 0x0c) << 16 ) | ((digit[3]& 0x01) << 12 ) ; // 5H 5J   5N

    	break;

      /* Position 6 on LCD (Digit6)*/
      case 6:
    	LCD->RAM[0] &= 0xfffc3fff;
    	LCD->RAM[2] &= 0xfffc3fff;
    	LCD->RAM[4] &= 0xfffc3fff;
    	LCD->RAM[6] &= 0xfffc3fff;

    	LCD->RAM[0] |= ((digit[0]& 0x04) << 15 ) | ((digit[0]& 0x08) << 13 ) | ((digit[0]& 0x03) << 14 ) ; // 6B 6G 6M 6E
    	LCD->RAM[2] |= ((digit[1]& 0x04) << 15 ) | ((digit[1]& 0x08) << 13 ) | ((digit[1]& 0x03) << 14 ) ; // 6A 6F 6C 6D
    	LCD->RAM[4] |= ((digit[2]& 0x04) << 15 ) | ((digit[2]& 0x08) << 13 ) | ((digit[2]& 0x01) << 14 ) ; // 6K 6Q    6P
    	LCD->RAM[6] |= ((digit[3]& 0x04) << 15 ) | ((digit[3]& 0x08) << 13 ) | ((digit[3]& 0x01) << 14 ) ; // 6J 6H   6N

    	break;

      default:
    	break;
    }

    // Sending update display request
    LCD->SR|=LCD_SR_UDR;
}

/**
 * \brief  Displays a string on the LCD.
 *
 * \param [in] Char table to display
 *
 */
void LCD_WriteString(char* s)
{
	uint8_t i;
	uint8_t kropki=0;
	for(i=0;*s!='\0' && i<LCD_NUMBER_OF_DIGITS+kropki;i++)
	{
		if(*(s+1)=='.'){
			LCD_WriteChar(*s,1,0,i+1);
			kropki++;
			s++;
		}
		else if(*(s+1)==':'){
			LCD_WriteChar(*s,0,1,i+1);
			kropki++;
			s++;
		}
		else
		{
			LCD_WriteChar(*s,0,0,i+1);
		}
		s++;
	}
	while(i<LCD_NUMBER_OF_DIGITS+kropki)
	{
		LCD_WriteChar(' ',0,0,i+1);
		i++;
	}
}

/**
  * \brief  Converts float to char table with designed precision.
  *
  * \param  *ptr   - Pointer to char table.
  * \param  number - Number to convert.
  * \param  d 	   - Number of digits before dot.
  * \param  p	   - Number of digits after dot.
  */
static void floatToChar(char *ptr, float number, uint8_t d, uint8_t p)
{
	int i;
	for(i=0;i<p;i++){
		number*=10;
	}

	int number_int=(int)(number);

	ptr = ptr + p + d + 1;
	*ptr = '\0';
	ptr--;

	// Convert number: ones, tens, hundreds...
	i= p + d;

	do
	{
		if(p && i==d){
			*ptr='.';
			ptr--;
		}
		*ptr-- = abs(number_int % 10) + '0';
		number_int /= 10;
	}
	while(--i);

	ptr++;
	while(i<d-1)
	{
		if(*ptr != '0')
		{
			break;
		}
		else *ptr=' ';
		ptr++;
		i++;
	}
}


/**
 * \brief	Writes float on LCD with designed precision.
 *
 * \param 	f - Pointer to number in float
 * \param 	d - Number of digits before dot.
 * \param 	p - Number of digits after dot.
 */
void LCD_WriteFloat(float* f, uint8_t d, uint8_t p)
{
	char tab[10];

	floatToChar(tab,*f,d,p);

	LCD_WriteString(tab);
}

/**
 * \brief	Writing on LCD time
 *
 * \param time	Pointer to table which contains number of hours, minutes and seconds in normal format (non BCD)
 */
void LCD_WriteTime(uint8_t* time)
{
	char text[9];

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

	LCD_WriteString(text);
}


