//
// BME280 sample app
//
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <stdint.h>
#include "bme280.h"

float bme()
{
  int i;
  float temperatura = 0;
  int T=0,P, H;
  i = bme280Init(1, 0x76);
  if (i != 0)
  {
    printf("ocorreu um erro ao abrir o bme280");
    return -1;
  }
  // usleep(1000000); // wait for data to settle for first read
  bme280ReadValues(&T, &P, &H);
  T -= 150;
  temperatura = (float)T/100.0;
  //printf("Calibrated temp. = %3.2f C, pres. = %6.2f Pa, hum. = %2.2f%%\n", (float)T/100.0, (float)P/256.0, (float)H/1024.0);
  return  temperatura;
} 
