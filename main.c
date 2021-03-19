#include <stdio.h>
#include <unistd.h>                 
#include <termios.h>        
#include <string.h>
#include <signal.h>
#include <stdlib.h>
#include <time.h>
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
void manda_alarm(int sinal){
  alarm(1);
}

void cria_arquivo(){
  FILE *file = fopen("./grafico/grafico.csv", "w");
  fprintf(file," TE,TI,TR,DataHora\n");
}

void escreve_arquivo(float te, float ti, float tr){
    time_t rawtime;
    struct tm * timeinfo;
    timeinfo = localtime ( &rawtime );
    FILE *file = fopen("./grafico/grafico.csv", "a");
    fprintf(file," %f,%f,%f,%s\n", te,ti,tr,asctime (timeinfo));
}

int main(){

  cria_arquivo();
  pid_configura_constantes(5,1,5);
  
  signal(SIGALRM, manda_alarm);
  
  while (1)
  {
    alarm(1);
    signal(SIGINT, paragpio);

    float te = bme();
    float ti = uart(SOLICITA_TEMP_INTERNA);
    float tr = uart(SOLICITA_TEMP_POT);

    lcd(te, ti, tr); //escreve no lcd
    
    printf("TE:%f TI:%f TR:%f\n",te,ti,tr); //printa as temperaturas
    escreve_arquivo(te,ti,tr);
    pid_atualiza_referencia(tr);
    int res_pid = (int) pid_controle(ti);
    if(res_pid > 0){
      gpio_liga(4,res_pid); //liga resistencia
      gpio_desliga(5); //desliga ventoinha
    }else if(res_pid < -40){
      gpio_liga(5,res_pid); //liga ventoinha
      gpio_desliga(4); //desliga resistencia
    }else{
      gpio_desliga(4); //desliga resistencia
      gpio_desliga(5); //desliga ventoinha
    }

    pause();
  }
return 0;
}
