#ifndef ADCINIT_H
#define ADCINIT_H

#include <xc.h>

void adcinit(){
    ADCON0 = 0x41; // AN0 ON
    ADCON1 = 0x80;
}

unsigned int read_adc(){
    __delay_ms(2);
    GO_nDONE = 1;
    while(GO_nDONE);
    return ((ADRESH<<8)+ADRESL);
}

#endif