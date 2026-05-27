#ifndef LIGHT_SENSOR_H
#define	LIGHT_SENSOR_H

#include <xc.h>

// ??nh ngh?a ph?n c?ng
#define RELAY_PIN RD0     // Chân ?i?u khi?n Relay
#define LIGHT_CHANNEL 1   // Chân RA1 t??ng ?ng v?i kênh AN1 c?a ADC

// Kh?i t?o b? chuy?n ??i Analog sang Digital (ADC)
void ADC_Init(void) {
    // ADCON1: C?n l? ph?i (ADFM=1), PCFG=0100 -> AN0, AN1, AN3 là Analog, còn l?i (g?m c? PORT E c?a LCD) là Digital.
    ADCON1 = 0x84; 
    
    // ADCON0: Ch?n xung clock Fosc/8, B?t module ADC (ADON=1)
    ADCON0 = 0x41; 
}

// ??c giá tr? Analog t? c?m bi?n (Tr? v? 0 - 1023)
unsigned int Light_Read(void) {
    // Xóa bit ch?n kênh c? và n?p kênh m?i (AN1)
    ADCON0 &= 0xC5; 
    ADCON0 |= (LIGHT_CHANNEL << 3);
    
    __delay_ms(2); // ??i t? s?c ?n ??nh
    
    GO_nDONE = 1;  // B?t ??u chuy?n ??i
    while(GO_nDONE); // Ch? chuy?n ??i xong
    
    // G?p 2 thanh ghi 8-bit thành 1 s? 10-bit
    return ((ADRESH << 8) + ADRESL); 
}

// Kh?i t?o Relay
void Relay_Init(void) {
    TRISDbits.TRISD0 = 0; // ??t RD0 làm ngõ ra (Output)
    RELAY_PIN = 0;        // T?t Relay m?c ??nh
}

// Hàm x? lý logic: Sáng -> B?t Relay, T?i -> T?t Relay
// Ng??ng threshold t? 0 ??n 1023
void Light_Control_Relay(unsigned int threshold) {
    unsigned int current_light = Light_Read();
    
    // L?U Ý: Tùy module c?m bi?n quang, ánh sáng m?nh thì ADC có th? T?NG ho?c GI?M.
    // N?u m?ch c?a b?n ng??c l?i, ch? c?n ??i d?u '>' thành '<'
    if(current_light > threshold) { 
        RELAY_PIN = 1; // Ánh sáng v??t ng??ng -> ?óng Relay
    } else {
        RELAY_PIN = 0; // Ánh sáng d??i ng??ng -> Ng?t Relay
    }
}

#endif	/* LIGHT_SENSOR_H */
