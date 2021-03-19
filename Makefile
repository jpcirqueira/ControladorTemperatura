run: main
	./run
main: lcd.o bme.o uart.o gpio.o pid.o
	gcc main.c ./binarios/*.o -o ./run -lwiringPi
pid.o: 
	gcc -c ./pid/pid.c -o ./binarios/pid.o
lcd.o: 
	gcc -c ./lcd/lcd.c -o ./binarios/lcd.o
gpio.o:
	gcc -c ./gpio/gpio.c -o ./binarios/gpio.o
bme.o: bme280.o
	gcc -c ./bme/bme.c -o ./binarios/bme.o
bme280.o:
	gcc -c ./bme/bme280.c -o ./binarios/bme280.o
uart.o: crc.o
	gcc -c ./uart/uart.c -o ./binarios/uart.o
crc.o:
	gcc -c ./uart/crc16.c -o ./binarios/crc.o