#include <stdlib.h>
#include <stdio.h>
#include <time.h>

void cria_arquivo(){
  FILE *file = fopen("./grafico/dados.csv", "w");
  fprintf(file,"TE,TI,TR,RESISTOR,VENTOINHA,DATA,HORA,PID\n");
}

void escreve_arquivo(float te, float ti, float tr,int pid){
    int ventoinha=0;
    FILE *file = fopen("./grafico/dados.csv", "a");
    time_t t = time(NULL);
    struct tm tm = *localtime(&t);
    
    if(pid<0){
      ventoinha = pid*-1;
      fprintf(file,"%f,%f,%f,%d,%d,%d/%d/%d,%02d:%02d:%02d,%d\n", te,ti,tr,0,ventoinha,tm.tm_mday,tm.tm_mon+ 1,tm.tm_year+ 1900,tm.tm_hour, tm.tm_min, tm.tm_sec,pid);
    }else{
      fprintf(file,"%f,%f,%f,%d,%d,%d/%d/%d,%02d:%02d:%02d,%d\n", te,ti,tr,pid,0,tm.tm_mday,tm.tm_mon+ 1,tm.tm_year+ 1900,tm.tm_hour, tm.tm_min, tm.tm_sec,pid);
    }
    fclose(file);
}