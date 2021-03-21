#include "wiringPi.h" 
#include "softPwm.h"
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

void gpio_liga(int pino, int pwm){
    int w = wiringPiSetup();
    if(w < 0){
        printf("erro ao iniciar wiringPi");
        exit(1);
    }
    
    pinMode(pino,OUTPUT);
    softPwmCreate(pino,0,100);
    softPwmWrite(pino, pwm);
}

void gpio_desliga(int pino){
    int w = wiringPiSetup();
    if(w < 0){
        printf("erro ao iniciar wiringPi");
        exit(1);
    }

    pinMode(pino,OUTPUT);
    softPwmCreate(pino,0,100);
    softPwmWrite(pino, 0);
}