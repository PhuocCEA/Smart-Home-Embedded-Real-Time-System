#ifndef MOTOR_H
#define MOTOR_H

#include <xc.h>

// Relay pins (theo mạch bạn)
#define RELAY1 RD2
#define RELAY2 RD3
#define RELAY3 RD4
#define RELAY4 RD5

void control_relay(unsigned char temp, unsigned char hum, unsigned int soil){

    // TEMP control
    if(temp > 30) RELAY1 = 1;   // fan
    else RELAY1 = 0;

    // HUM control
    if(hum < 50) RELAY2 = 1;   // humidifier
    else RELAY2 = 0;

    // SOIL control
    if(soil < 300) RELAY3 = 1; // pump
    else RELAY3 = 0;

    // LIGHT (LDR)
    if(soil > 800) RELAY4 = 1;
    else RELAY4 = 0;
}

#endif