/**
 * @file LCD.cpp
 *
 * @author Luke Waller
 * 
 * @section LICENSE
 *
 * Copyright (c) 2022 Luke Waller
 *
 *    This program is free software: you can redistribute it and/or modify
 *    it under the terms of the GNU General Public License as published by
 *    the Free Software Foundation, either version 3 of the License, or
 *    (at your option) any later version.
 *
 *    This program is distributed in the hope that it will be useful,
 *    but WITHOUT ANY WARRANTY; without even the implied warranty of
 *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *    GNU General Public License for more details.
 *
 *    You should have received a copy of the GNU General Public License
 *    along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * @section DESCRIPTION
 *
 * LCD Display using a HD4478 driver chip
 *
 */

/**
 * Includes
 */
#include "LCD.h"

/**
 * Methods
 */
 LCD::LCD(PinName RS, PinName RW, PinName Enable, PinName D4, PinName D5, PinName D6, PinName D7):
           _rs(RS), _rw(RW), _enable(Enable), _data(D4, D5, D6, D7){
        _mode = 1;
        _data.output();
        _rs = 0;
        _rw = 0;
        _enable = 0;

        wait_us(20000);
        LCD_cmd(LCD_FUNCTION_4_BIT_2_LINE_5x7_DOTS);
        LCD_cmd(LCD_DISP_ON_CURSOR_BLINKING);
        LCD_cmd(LCD_CLEAR);
        LCD_cmd(LCD_ENTRY_MODE_AUTO_INCREMENT);
    };

 LCD::LCD(PinName RS, PinName RW, PinName Enable, PinName D0, PinName D1, PinName D2, PinName D3, PinName D4, PinName D5, PinName D6, PinName D7):
           _rs(RS), _rw(RW), _enable(Enable), _data(D0, D1, D2, D3, D4, D5, D6, D7){
        _mode = 0;
        _data.output();
        _rs = 0;
        _rw = 0;
        _enable = 0;

        wait_us(20000);
        LCD_cmd(LCD_FUNCTION_8_BIT_2_LINE_5x7_DOTS);
        LCD_cmd(LCD_DISP_ON_CURSOR_BLINKING);
        LCD_cmd(LCD_CLEAR);
        LCD_cmd(LCD_ENTRY_MODE_AUTO_INCREMENT);
    };

void LCD::LCD_cmd(unsigned char cmd){
    if(_mode == 1){
        wait_busy();
        _data = cmd >> 4;
        _rs = 0;
        _rw = 0;
        strobe_LCD();
        wait_busy();
        _data = cmd & 0x0F;
        strobe_LCD();
    } else {
        wait_busy();
        _data = cmd;
        _rs = 0;
        _rw = 0;
        strobe_LCD();
    }
}



void LCD::cursor_Set(int column, int row){
    if(row == 1){
        LCD_cmd((LCD_LINE_1) + (column-1));
    } else {
        LCD_cmd((LCD_LINE_2) + (column-1));
    }
}

 void LCD::wait_busy(void){
    _data.input();
    if (_data == 0b1000000) {}
    _data.output();
}

 void LCD::strobe_LCD(void){
    _enable = 1;
    wait_us(10000);
    _enable = 0;
}


void LCD::put_char(unsigned char cmd){
    if(_mode == 1){
        wait_busy();
        _data = cmd >> 4;
        _rs = 1;
        _rw = 0;
        strobe_LCD();
        wait_busy();
        _data = cmd & 0x0F;
        strobe_LCD();
    } else {
        wait_busy();
        _data = cmd;
        _rs = 1;
        _rw = 0;
        strobe_LCD();
    }
}

void LCD::put_char_array(char *data, int length){
    for ( int i = 0; i < length; i++ ) {
        put_char(*data++);
    }
}

void LCD::put_decimal(int decimal_val){
    char dec[sizeof(decimal_val)];
    int array = 0;
    if(decimal_val == 0){
        put_char('0');
    } else {
    while (decimal_val > 0){
        dec[array] = (decimal_val % 10) + 0x30;
        decimal_val /= 10;
        array++;
    }
    for (int i = array - 1; i >= 0; i--){
        unsigned char character = dec[i];
        put_char(character);
    }
    }
}

void LCD::put_float(float float_val, int precision){
    char value[16];
    sprintf(value, "%.*f", precision, float_val);
    int length = strlen(value);
    put_char_array(value, length);
}

void LCD::operator=(int value){put_decimal(value);}
void LCD::operator=(float value){put_float(value);}
void LCD::operator=(char character){put_char(character);}