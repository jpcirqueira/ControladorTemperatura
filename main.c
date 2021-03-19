#include <stdio.h>
#include <unistd.h>                 
#include <termios.h>        
#include <string.h>
#include <signal.h>
#include <stdlib.h>

#include "./bme/bme.h"
#include "./lcd/lcd.h"
#include "./uart/uart.h"
#include "./pid/pid.h"
#include "./gpio/gpio.h"

#define SOLICITA_TEMP_INTERNA 0xC1
#define SOLICITA_TEMP_POT 0xC2


void paragpio(int sinal) {
  gpio_desliga(4); //desliga resistencia
  gpio_desliga(5); //desliga ventoinha
  printf("gpio desligada.\n");
  exit(1);
}

int main(){
  pid_configura_constantes(5,1,5);
  while (1)
  {
    float te = bme();
    float ti = uart(SOLICITA_TEMP_INTERNA);
    float tr = uart(SOLICITA_TEMP_POT);
    lcd(te, ti, tr);
    printf("TE:%f TI:%f TR:%f\n",te,ti,tr);
    signal(SIGINT, paragpio);

    pid_atualiza_referencia(tr);
    int res_pid = (int) pid_controle(ti);
    if(res_pid > 0){
      gpio_liga(4,res_pid); //liga resistencia
      gpio_desliga(5); //desliga ventoinha
    }else if(res_pid < -40){
      gpio_liga(5,res_pid); //liga ventoinha
      gpio_desliga(4); //desliga resistencia
    }
    sleep(1);
  }
return 0;
}
