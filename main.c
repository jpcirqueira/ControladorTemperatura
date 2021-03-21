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

int usateterminal=0;
float teTerminal=0;
void paragpio(int sinal) {
  gpio_desliga(4); //desliga resistencia
  gpio_desliga(5); //desliga ventoinha
  ClrLcd(); // limpa lcd
  printf("gpio desligada.\n");
  exit(1);
}
void manda_alarm(int sinal){
  alarm(1);
}

void cria_arquivo(){
  FILE *file = fopen("./grafico/grafico.csv", "w");
  fprintf(file,"TE,TI,TR,PID,DATA,HORA\n");
}

void escreve_arquivo(float te, float ti, float tr, int pid){
    FILE *file = fopen("./grafico/grafico.csv", "a");
    time_t t = time(NULL);
    struct tm tm = *localtime(&t);
    fprintf(file,"%f,%f,%f,%d,%d/%d/%d, %02d:%02d:%02d\n", te,ti,tr,pid,tm.tm_mday,tm.tm_mon+ 1,tm.tm_year+ 1900,tm.tm_hour, tm.tm_min, tm.tm_sec);
    fclose(file);
}

void menu(){
  printf("----------------------------------Trabalho1---------------------------------\n");
  printf("|digite 1 se deseja inserir uma temperatura de referencia.                 |\n");
  printf("|digite 2 para pegar a temperatura de referencia do potenciometro.         |\n");
  printf("|pressione ctrl + d definir a temperatura de referencia durante a execucao.|\n");
  printf("|pressione ctrl + c para parar finalizar execucao.                         |\n");
  printf("----------------------------------------------------------------------------\n");
}

void pegaTEterminal(int sinal){
  if(sinal == SIGTSTP){
    usateterminal=1;
    printf("digite a temperatura de referencia:\n");
    scanf("%f",&teTerminal);
  }else{
    exit(0);
  }
}

int main(){
  time_t t = time(NULL);
  cria_arquivo();
  pid_configura_constantes(5,1,5);
  menu();
  signal(SIGALRM, manda_alarm);
  signal(SIGTSTP, pegaTEterminal);
  
  while (1)
  {
    alarm(1);
    signal(SIGINT, paragpio);
    float te = bme();
    float ti = uart(SOLICITA_TEMP_INTERNA);
    float tr;
    if(usateterminal == 1){
      tr = teTerminal;
    }else{
      tr = uart(SOLICITA_TEMP_POT);
    }
    //float tr = uart(SOLICITA_TEMP_POT);
    lcd(te, ti, tr); //escreve no lcd
    pid_atualiza_referencia(tr);
    int res_pid = (int) pid_controle(ti);
    escreve_arquivo(te,ti,tr,res_pid);
    printf("TE:%f TI:%f TR:%f PID:%d\n",te,ti,tr,res_pid); //printa as temperaturas
    if(res_pid > 0){
      printf("%d\n",res_pid);
      gpio_liga(4,res_pid); //liga resistencia
      gpio_desliga(5); //desliga ventoinha
    }else if(res_pid < -40){
      res_pid= res_pid *-1;
      printf("%d\n",res_pid);
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
