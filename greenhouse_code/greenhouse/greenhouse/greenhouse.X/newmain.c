// ==========================================
// C?U HÌNH BIT CHO PIC16F877A
// ==========================================
#pragma config FOSC = HS      
#pragma config WDTE = OFF       
#pragma config PWRTE = ON       
#pragma config BOREN = ON       
#pragma config LVP = OFF        
#pragma config CPD = OFF        
#pragma config WRT = OFF        
#pragma config CP = OFF         

#include <xc.h>
#include <stdio.h>

#define _XTAL_FREQ 8000000

// G?i 3 "?àn em" th? vi?n vào làm vi?c
#include "LightSensor.h"
#include "DHT11.h"
#include "PIR.h"

// ==========================================
// TH? VI?N LCD (Gi? nguyên)
// ==========================================
#define RS RE0
#define EN RE1
#define D4 RD4
#define D5 RD5
#define D6 RD6
#define D7 RD7

void Lcd_Port(char a) {
    if(a & 1) D4 = 1; else D4 = 0;
    if(a & 2) D5 = 1; else D5 = 0;
    if(a & 4) D6 = 1; else D6 = 0;
    if(a & 8) D7 = 1; else D7 = 0;
}
void Lcd_Cmd(char a) {
    RS = 0; Lcd_Port(a >> 4); EN = 1; __delay_ms(1); EN = 0;
    Lcd_Port(a); EN = 1; __delay_ms(1); EN = 0;
}
void Lcd_Clear(void) { Lcd_Cmd(0x01); __delay_ms(2); }
void Lcd_Set_Cursor(char row, char col) {
    char temp;
    if(row == 1) temp = 0x80 + col - 1;
    else if(row == 2) temp = 0xC0 + col - 1;
    Lcd_Cmd(temp);
}
void Lcd_Init(void) {
    Lcd_Port(0x00); __delay_ms(20);
    Lcd_Cmd(0x03); __delay_ms(5);
    Lcd_Cmd(0x03); __delay_ms(11);
    Lcd_Cmd(0x03);
    Lcd_Cmd(0x02); Lcd_Cmd(0x28); Lcd_Cmd(0x0C); Lcd_Cmd(0x06);
    Lcd_Clear();
}
void Lcd_Write_Char(char a) {
    RS = 1; Lcd_Port(a >> 4); EN = 1; __delay_ms(1); EN = 0;
    Lcd_Port(a); EN = 1; __delay_ms(1); EN = 0;
}
void Lcd_Write_String(char *a) {
    for(int i = 0; a[i] != '\0'; i++) Lcd_Write_Char(a[i]);
}

// ==========================================
// CH??NG TRÌNH CHÍNH (MAIN)
// ==========================================
void main(void) {
    // 1. C?u hình Port chung
    TRISD &= 0x0F; // RD4-RD7 làm Output cho LCD
    TRISE &= 0xFC; // RE0, RE1 làm Output cho LCD
    
    // 2. Kh?i t?o toàn b? các module t? Th? vi?n
    ADC_Init();    // Kh?i t?o ADC (LightSensor)
    Relay_Init();  // Kh?i t?o chân RD0 (LightSensor)
    PIR_Init();    // Kh?i t?o chân RC5 và RD1 (PIR)
    Lcd_Init();    // Kh?i t?o màn hình
    
    Lcd_Set_Cursor(1, 1);
    Lcd_Write_String("Smart Home Pro..");
    __delay_ms(1000);
    Lcd_Clear();

    // Các bi?n ph?c v? hi?n th? ?a nhi?m
    char buffer[17]; 
    int tick = 0;           
    char screen_page = 0; // 0: Ánh sáng, 1: DHT11, 2: PIR
    unsigned char dht_status = 1; 
    
    while(1) {
        // [NHI?M V? 1]: X? LÝ S? KI?N T?C TH?I (Real-time)
        // Hai tác v? này ch?y liên t?c v?i t?c ?? bàn th?, không b? delay
        
        // C?p nh?t Relay theo ánh sáng (Ng??ng 500)
        unsigned int light_val = Light_Read();
        Light_Control_Relay(500); 
        
        // C?p nh?t Còi/?èn báo ??ng theo PIR
        PIR_Control_Alarm();

        // [NHI?M V? 2]: CHUY?N TRANG LCD (M?i 2 giây / 20 ticks)
        if (tick == 0) {
            screen_page = 0; 
            Lcd_Clear(); 
        } 
        else if (tick == 20) {
            screen_page = 1; 
            Lcd_Clear();
            // ??c DHT11 khi v?a chuy?n sang trang c?a nó
            dht_status = DHT11_Read(); 
        }
        else if (tick == 40) {
            screen_page = 2; 
            Lcd_Clear(); 
        }

        // [NHI?M V? 3]: C?P NH?T GIAO DI?N LÊN MÀN HÌNH
        if (screen_page == 0) {
            Lcd_Set_Cursor(1, 1); sprintf(buffer, "Light: %04u     ", light_val); Lcd_Write_String(buffer);
            Lcd_Set_Cursor(2, 1);
            if(RELAY_PIN == 1) Lcd_Write_String("Relay: ON       ");
            else               Lcd_Write_String("Relay: OFF      ");
        } 
        else if (screen_page == 1) {
            if (dht_status == 1) {
                Lcd_Set_Cursor(1, 1); sprintf(buffer, "Temp: %u C      ", dht_temp); Lcd_Write_String(buffer);
                Lcd_Set_Cursor(2, 1); sprintf(buffer, "Humi: %u %%      ", dht_hum); Lcd_Write_String(buffer);
            } else {
                Lcd_Set_Cursor(1, 1); Lcd_Write_String("DHT11 Error!    ");
            }
        }
        else if (screen_page == 2) {
            Lcd_Set_Cursor(1, 1); Lcd_Write_String("Security System ");
            Lcd_Set_Cursor(2, 1);
            if(PIR_Read() == 1) Lcd_Write_String("-> MOTION DETECT");
            else                Lcd_Write_String("-> CLEAR        ");
        }
        
        // T?ng b? ??m th?i gian
        tick++;
        if(tick >= 60) tick = 0; // Hoàn thành chu k? 6 giây
        
        __delay_ms(100); 
    }
}