#ifndef PORTINIT_H
#define PORTINIT_H

void portinit(void){
    TRISA = 0xFF;   // ADC input
    TRISB = 0x00;   // LCD
    TRISC = 0x00;   // LCD
    TRISD = 0x01;   // RD0 input (DHT11), còn lại output
    TRISE = 0x00;

    PORTA = 0;
    PORTB = 0;
    PORTC = 0;
    PORTD = 0;
    PORTE = 0;
}

#endif