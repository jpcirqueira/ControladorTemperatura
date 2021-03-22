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
#include "./arquivo/arquivo.h"

#define SOLICITA_TEMP_INTERNA 0xC1
#define SOLICITA_TEMP_POT 0xC2

int usateterminal=0,escrevearquivo=0;
float teTerminal=0;
void paragpio(int sinal) {
  gpio_desliga(4); //desliga resistencia
  gpio_desliga(5); //desliga ventoinha
  ClrLcd(); // limpa lcd
  printf("gpio desligada.\n");
  exit(1);
}
void manda_alarm(int sinal){
  escrevearquivo+=1;
  alarm(1);
}

void menu(){
  printf("-----------------------------------Trabalho1---------------------------------\n");
  printf("|digite 1 se deseja inserir uma temperatura de referencia.                  |\n");
  printf("|digite 2 para pegar a temperatura de referencia do potenciometro.          |\n");
  printf("|pressione ctrl + z para mudar o modo de de pegar temperatura de referencia.|\n");
  printf("|pressione ctrl + c para parar finalizar execucao.                          |\n");
  printf("-----------------------------------------------------------------------------\n");
  int entrada;
  scanf("%d",&entrada);
  if(entrada == 1){
    printf("digite a temperatura de referencia:\n");
    scanf("%f",&teTerminal);
    usateterminal=1;
  }else if(entrada == 2){
    usateterminal=0;

  }
}

void pegaTEterminal(int sinal){
  if(sinal == SIGTSTP){
    if(usateterminal == 0){// esta pegando potenciomentro
      usateterminal=1;
      printf("digite a temperatura de referencia:\n");
      scanf("%f",&teTerminal);
    }else if(usateterminal == 1){// esta pegando do terminal
      usateterminal=0;
      printf("temperatura de referencia do potenciometro\n");
    }
  }else{
    exit(0);
  }
}

int main(){
  cria_arquivo();
  pid_configura_constantes(5,1,5);
  menu();
  signal(SIGALRM, manda_alarm);
  signal(SIGTSTP, pegaTEterminal);
  signal(SIGINT, paragpio);
  while (1)
  {
    alarm(1);
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
    if(escrevearquivo == 2){
      escreve_arquivo(te,ti,tr,res_pid);
      escrevearquivo = 0;
    }
    printf("TE:%f TI:%f TR:%f PID:%d\n",te,ti,tr,res_pid); //printa as temperaturas
    if(res_pid > 0){
      gpio_liga(4,res_pid); //liga resistencia
      gpio_desliga(5); //desliga ventoinha
    }else if(res_pid < -40){
      res_pid= res_pid *-1;
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
