#ifndef DHT11_H
#define	DHT11_H

#include <xc.h>

// ??nh ngh?a chân RC4 cho DHT11
#define DHT11_PIN PORTCbits.RC4
#define DHT11_DIR TRISCbits.TRISC4

// Bi?n toàn c?c ?? l?u tr? nhi?t ?? và ?? ?m
unsigned char dht_temp = 0;
unsigned char dht_hum = 0;

// Hàm g?i tín hi?u Start ?? ?ánh th?c DHT11
void DHT11_Start(void) {
    DHT11_DIR = 0;     // C?u hình RC4 là ngõ ra
    DHT11_PIN = 0;     // Kéo xu?ng m?c 0
    __delay_ms(18);    // Gi? m?c 0 t?i thi?u 18ms theo Datasheet
    DHT11_PIN = 1;     // Kéo lên m?c 1
    __delay_us(30);    // Ch? 30us
    DHT11_DIR = 1;     // C?u hình RC4 thành ngõ vào ?? l?ng nghe
}

// Hàm ki?m tra DHT11 có ph?n h?i không
unsigned char DHT11_Check_Response(void) {
    unsigned char timeout = 0;
    while(DHT11_PIN == 1) { // Ch? DHT11 kéo xu?ng 0
        __delay_us(1);
        if(++timeout > 100) return 0; // L?i m?t k?t n?i
    }
    timeout = 0;
    while(DHT11_PIN == 0) { // Ch? DHT11 kéo lên 1 l?i
        __delay_us(1);
        if(++timeout > 100) return 0;
    }
    timeout = 0;
    while(DHT11_PIN == 1) { // Ch? DHT11 kéo xu?ng 0 ?? b?t ??u g?i Data
        __delay_us(1);
        if(++timeout > 100) return 0;
    }
    return 1; // Kh?i t?o thành công
}

// Hàm ??c 1 Byte (8 bit) t? DHT11
unsigned char DHT11_Read_Byte(void) {
    unsigned char data = 0, i, timeout;
    for(i = 0; i < 8; i++) {
        timeout = 0;
        while(DHT11_PIN == 0) { // Ch? h?t m?c th?p
            __delay_us(1);
            if(++timeout > 100) break;
        }
        
        __delay_us(30); // ??i 30us, n?u sau 30us chân v?n ? m?c 1 thì bit ?ó là bit 1
        
        if(DHT11_PIN == 1) {
            data = (data << 1) | 1; // Ghi bit 1
        } else {
            data = (data << 1);     // Ghi bit 0
        }
        
        timeout = 0;
        while(DHT11_PIN == 1) { // Ch? h?t m?c cao ?? sang bit ti?p theo
            __delay_us(1);
            if(++timeout > 100) break;
        }
    }
    return data;
}

// Hàm t?ng h?p: ??c và c?p nh?t Nhi?t ??, ?? ?m
// Tr? v? 1 n?u ??c thành công, 0 n?u th?t b?i
unsigned char DHT11_Read(void) {
    unsigned char hum_int, hum_dec, temp_int, temp_dec, checksum;
    
    DHT11_Start();
    if(DHT11_Check_Response()) {
        hum_int  = DHT11_Read_Byte();
        hum_dec  = DHT11_Read_Byte();
        temp_int = DHT11_Read_Byte();
        temp_dec = DHT11_Read_Byte();
        checksum = DHT11_Read_Byte();
        
        // Ki?m tra Checksum ?? ch?ng nhi?u d? li?u
        if((hum_int + hum_dec + temp_int + temp_dec) == checksum) {
            dht_hum = hum_int;
            dht_temp = temp_int;
            return 1; // OK
        }
    }
    return 0; // L?i ??c
}

#endif	/* DHT11_H */