MCU=attiny13a
dMCU=attiny13
F_CPU=9600000
CC=avr-g++
OBJCOPY=avr-objcopy
CFLAGS=-std=c++11 -Wall -g -Os -mmcu=${MCU} -DF_CPU=${F_CPU} -I.
TARGET=main
SRCS=main.c src/*.c
SIZE=avr-size -C


all: ${TARGET}.hex

asm: ${SRCS}
	${CC} ${CFLAGS} -S -o ${TARGET}.asm ${SRCS}

${TARGET}.bin: Makefile ${SRCS}
	${CC} ${CFLAGS} -o ${TARGET}.bin ${SRCS}
	${SIZE} --mcu ${MCU} "$@"

${TARGET}.hex: ${TARGET}.bin
	${OBJCOPY} -j .text -j .data -O ihex $? "$@"

flash: ${TARGET}.hex
	avrdude -p ${dMCU} -c usbasp -U flash:w:${TARGET}.hex:i -P usb

fuse:
	avrdude -p ${dMCU} -c usbasp -U lfuse:w:0x7A:m -P usb

clean:
	rm -f *.bin *.hex
