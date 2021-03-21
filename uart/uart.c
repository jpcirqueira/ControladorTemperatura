#include <stdio.h>
#include <unistd.h>         //Used for UART
#include <fcntl.h>          //Used for UART
#include <termios.h>        //Used for UART
#include <string.h>
#include <stdint.h>
#include "crc16.h"

#define COD_SERVIDOR 0x01

#define SOLICITA 0x23
unsigned char rx_buffer[9];
union CONV
{
    int inteiro;
    unsigned char bytes[4];
};

typedef union {
    unsigned short crc;   
    uint8_t bytes[2]; 
} crcCalculateBytes;

int erro = 0;

int confere_crc(){
    crcCalculateBytes crcrecebido;
    crcCalculateBytes crcauxiliar;
    crcrecebido.bytes[0] = rx_buffer[7];
    crcrecebido.bytes[1] = rx_buffer[8];
    crcauxiliar.crc = calcula_CRC(rx_buffer,7);
    if(crcrecebido.crc == crcauxiliar.crc){
        return 1; // o crc enviado foi calculado certo
    }else{
        return -1; // o crc enviado foi calculado errado
    }
}

void envia_info(int uart0_filestream,char tipo){

    char matricula[4];
    matricula[0] = 0x02;
    matricula[1] = 0x03;
    matricula[2] = 0x04;
    matricula[3] = 0x04;
    unsigned char codigo[9] = {COD_SERVIDOR,SOLICITA,tipo};
    memcpy(&codigo[3],&matricula,4);
    crcCalculateBytes crcunion;
    crcunion.crc = calcula_CRC(codigo,7);
    
    unsigned char msg[9];
    memcpy(msg,codigo,7);
    memcpy(&msg[7], crcunion.bytes, 2);

    if (uart0_filestream != -1){
        int count = write(uart0_filestream, &msg[0],9);
        if (count < 0)
        {
            printf("UART TX error\n");
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

        int rx_length = read(uart0_filestream, (void*)rx_buffer, 9);
        if (rx_length <= 0)
        {
            printf("Erro na leitura.\n"); 
            if(erro>2){ // se ja ocorreu 2 erros
                erro =0;
            }else{ // se ocorreu menos de 2 erros solicita novamente
                erro+=1;
                uart(solicita); 
            }
        }
        else
        {
           
            int errocrc = confere_crc();
            if(errocrc==1){// o crc enviado esta certo de acordo com a mensagem enviada
                memcpy(&data, &rx_buffer[3], 4);
            }else if(errocrc==-1){// o crc enviado esta errado de acordo com a mensagem enviada
                if(erro>2){ // se ja ocorreu 2 erros
                erro =0;
                }else{ // se ocorreu menos de 2 erros solicita novamente
                    erro+=1;
                    uart(solicita); 
                }
            }
        }
    }

    close(uart0_filestream);
   return data;
}
