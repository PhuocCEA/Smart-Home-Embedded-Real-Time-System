#include <xc.h>
#include <stdio.h>

// CONFIG (dùng th?ch anh ngoài)
#pragma config FOSC = HS
#pragma config WDTE = OFF
#pragma config PWRTE = OFF
#pragma config BOREN = ON
#pragma config LVP = OFF
#pragma config CPD = OFF
#pragma config WRT = OFF
#pragma config CP = OFF

#define _XTAL_FREQ 8000000

// LCD PIN
#define RS PORTBbits.RB2
#define EN PORTBbits.RB3
#define D4 PORTBbits.RB4
#define D5 PORTBbits.RB5
#define D6 PORTBbits.RB6
#define D7 PORTBbits.RB7

// DHT11 RD0
#define DHT_TRIS TRISDbits.TRISD0
#define DHT_PORT PORTDbits.RD0
#define DHT_LAT  LATDbits.LATD0

// ===== SEND 4 BIT =====
void lcd_send_nibble(unsigned char nibble) {
    D4 = (nibble >> 0) & 1;
    D5 = (nibble >> 1) & 1;
    D6 = (nibble >> 2) & 1;
    D7 = (nibble >> 3) & 1;

    EN = 1;
    __delay_us(5);
    EN = 0;
}

// ===== COMMAND =====
void lcd_cmd(unsigned char cmd) {
    RS = 0;
    lcd_send_nibble(cmd >> 4);
    lcd_send_nibble(cmd);
    __delay_ms(2);
}

// ===== DATA =====
void lcd_data(unsigned char data) {
    RS = 1;
    lcd_send_nibble(data >> 4);
    lcd_send_nibble(data);
    __delay_ms(2);
}

// ===== INIT LCD =====
void lcd_init() {
    TRISB = 0x00;
    PORTB = 0x00;

    __delay_ms(20);

    lcd_send_nibble(0x03);
    __delay_ms(5);

    lcd_send_nibble(0x03);
    __delay_us(200);

    lcd_send_nibble(0x03);
    lcd_send_nibble(0x02);

    lcd_cmd(0x28);
    lcd_cmd(0x0C);
    lcd_cmd(0x06);
    lcd_cmd(0x01);
    __delay_ms(2);
}

// ===== PRINT STRING =====
void lcd_puts(const char *s) {
    while(*s) lcd_data(*s++);
}

// ===== SET CURSOR =====
void lcd_gotoxy(unsigned char x, unsigned char y) {
    unsigned char addr;

    if(y == 1) addr = 0x80 + (x - 1);
    else       addr = 0xC0 + (x - 1);

    lcd_cmd(addr);
}

// ===== DHT11 READ BYTE =====
unsigned char dht11_read_byte() {
    unsigned char i, data = 0;

    for(i = 0; i < 8; i++) {
        while(!DHT_PORT);
        __delay_us(30);

        if(DHT_PORT) {
            data |= (1 << (7 - i));
            while(DHT_PORT);
        }
    }
    return data;
}

// ===== DHT11 READ =====
unsigned char dht11_read(unsigned char *temp, unsigned char *humi) {
    unsigned char humi_int, humi_dec;
    unsigned char temp_int, temp_dec;
    unsigned char checksum;

    // start signal
    DHT_TRIS = 0;
    DHT_LAT = 0;
    __delay_ms(20);

    DHT_LAT = 1;
    __delay_us(30);
    DHT_TRIS = 1;

    // response
    if(DHT_PORT) return 0;
    while(!DHT_PORT);
    while(DHT_PORT);
    while(!DHT_PORT);

    humi_int = dht11_read_byte();
    humi_dec = dht11_read_byte();
    temp_int = dht11_read_byte();
    temp_dec = dht11_read_byte();
    checksum = dht11_read_byte();

    if((humi_int + humi_dec + temp_int + temp_dec) != checksum)
        return 0;

    *temp = temp_int;
    *humi = humi_int;

    return 1;
}

// ===== MAIN =====
void main() {
    ADCON1 = 0x06;   // t?t analog
    TRISD = 0x01;    // RD0 input

    lcd_init();

    unsigned char temp, humi;
    char buffer[16];

    while(1) {
        if(dht11_read(&temp, &humi)) {
            lcd_gotoxy(1,1);
            sprintf(buffer, "Temp: %d C   ", temp);
            lcd_puts(buffer);

            lcd_gotoxy(1,2);
            sprintf(buffer, "Humi: %d %%  ", humi);
            lcd_puts(buffer);
        } else {
            lcd_gotoxy(1,1);
            lcd_puts("DHT11 ERROR   ");

            lcd_gotoxy(1,2);
            lcd_puts("              ");
        }

        __delay_ms(2000);
    }
}