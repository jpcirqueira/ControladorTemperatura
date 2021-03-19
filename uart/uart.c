#include <stdio.h>
#include <unistd.h>         //Used for UART
#include <fcntl.h>          //Used for UART
#include <termios.h>        //Used for UART
#include <string.h>
#include <stdint.h>
#include "crc16.h"

#define COD_SERVIDOR 0x01

#define SOLICITA 0x23

union CONV
{
    int inteiro;
    unsigned char bytes[4];
};

typedef union {
    short crc;   
    uint8_t bytes[2]; 
} crcCalculateBytes;

void envia_info(int uart0_filestream,char tipo){
    
    // union CONV num;
    // int matricula = 3244;
    // num.inteiro= matricula;
    char teste[4];
    teste[0] = 0x02;
    teste[1] = 0x03;
    teste[2] = 0x04;
    teste[3] = 0x04;
    unsigned char codigo[9] = {COD_SERVIDOR,SOLICITA,tipo};
    memcpy(&codigo[3],&teste,4);
    crcCalculateBytes crcunion;
    crcunion.crc = calcula_CRC(codigo,7);

    // unsigned char aux[2];
    // aux[0] = (crc & 255);
    // aux[1] = ((crc >> 8) & 255);

    unsigned char msg[9];
    memcpy(msg,codigo,7);
    memcpy(&msg[7], crcunion.bytes, 2);
    // printf("Buffers de memória criados!\n");

    if (uart0_filestream != -1){
        // printf("Escrevendo caracteres na UART ...");
        int count = write(uart0_filestream, &msg[0],9);
        if (count < 0)
        {
            printf("UART TX error\n");
        }
        else
        {
            printf("escrito.\n");
        }
    }
}

float uart(char solicita) {

    int uart0_filestream = -1;
    float data;
    uart0_filestream = open("/dev/serial0", O_RDWR | O_NOCTTY | O_NDELAY);      //Open in non blocking read/write mode
    if (uart0_filestream == -1)
    {
        printf("Erro - Não foi possível iniciar a UART.\n");
    }  
    struct termios options;
    tcgetattr(uart0_filestream, &options);
    options.c_cflag = B9600 | CS8 | CLOCAL | CREAD;
    options.c_iflag = IGNPAR;
    options.c_oflag = 0;
    options.c_lflag = 0;
    tcflush(uart0_filestream, TCIFLUSH);
    tcsetattr(uart0_filestream, TCSANOW, &options);

    envia_info(uart0_filestream,solicita);
    usleep(200000);

    if (uart0_filestream != -1)
    {
        unsigned char rx_buffer[256];
        int rx_length = read(uart0_filestream, (void*)rx_buffer, 255);
        if (rx_length < 0)
        {
            printf("Erro na leitura.\n"); 
        }
        else if (rx_length == 0)
        {
            printf("Nenhum dado disponível.\n");
        }
        else
        {
          memcpy(&data, &rx_buffer[3], 4);
        }
    }

    close(uart0_filestream);
   return data;
}
