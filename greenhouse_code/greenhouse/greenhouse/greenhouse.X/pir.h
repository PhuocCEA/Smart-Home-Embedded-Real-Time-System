#ifndef PIR_H
#define	PIR_H

#include <xc.h>

// ??nh ngh?a ph?n c?ng cho Module PIR và Còi/?èn báo
#define PIR_PIN PORTCbits.RC5
#define PIR_DIR TRISCbits.TRISC5

#define ALARM_PIN PORTDbits.RD1
#define ALARM_DIR TRISDbits.TRISD1

// Hàm kh?i t?o các chân
void PIR_Init(void) {
    PIR_DIR = 1;   // RC5 làm ngõ vào (Input) ?? ??c tín hi?u t? c?m bi?n
    ALARM_DIR = 0; // RD1 làm ngõ ra (Output) ?? kích còi/?èn
    ALARM_PIN = 0; // T?t còi m?c ??nh khi m?i c?p ngu?n
}

// Hàm ??c tr?ng thái c?m bi?n (Tr? v? 1: Có ng??i, 0: Không có ng??i)
unsigned char PIR_Read(void) {
    return PIR_PIN;
}

// Hàm x? lý logic t? ??ng: Có ng??i -> B?t còi, ?i khu?t -> T?t còi
void PIR_Control_Alarm(void) {
    if(PIR_Read() == 1) {
        ALARM_PIN = 1; // Báo ??ng!
    } else {
        ALARM_PIN = 0; // An toàn
    }
}

#endif	/* PIR_H */
