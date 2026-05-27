#ifndef LCD_H
#define LCD_H

#include <xc.h>

#define RS RC0
#define EN RC1
#define LCD PORTB

void Lcd_Cmd(char cmd){
    RS = 0;
    LCD = cmd;
    EN = 1; __delay_ms(2); EN = 0;
}

void Lcd_Data(char data){
    RS = 1;
    LCD = data;
    EN = 1; __delay_ms(2); EN = 0;
}

void Lcd_Init(){
    __delay_ms(20);
    Lcd_Cmd(0x38);
    Lcd_Cmd(0x0C);
    Lcd_Cmd(0x06);
    Lcd_Cmd(0x01);
}

void Lcd_Clear(){
    Lcd_Cmd(0x01);
}

void Lcd_Set_Cursor(char row, char col){
    char pos = (row==1)? (0x80+col-1):(0xC0+col-1);
    Lcd_Cmd(pos);
}

void Lcd_String(const char *str){
    while(*str) Lcd_Data(*str++);
}

#endif