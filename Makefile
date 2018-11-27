MCU=attiny13
#F_CPU=1200000
F_CPU=9600000
CC=avr-gcc
OBJCOPY=avr-objcopy
CFLAGS=-std=c99 -Wall -g -Os -mmcu=${MCU} -DF_CPU=${F_CPU} -I.
TARGET=test
SRCS=main.c
SIZE=avr-size -C


all: ${TARGET}.hex

asm: test.c
	${CC} ${CFLAGS} -S -o ${TARGET}.asm test.c

${TARGET}.bin: test.c Makefile
	${CC} ${CFLAGS} -o ${TARGET}.bin test.c
	${SIZE} --mcu ${MCU} "$@"

${TARGET}.hex: ${TARGET}.bin
	${OBJCOPY} -j .text -j .data -O ihex $? "$@"

flash: ${TARGET}.hex
	avrdude -p ${MCU} -c usbasp -U flash:w:${TARGET}.hex:i -F -P usb

fuse:
	avrdude -p ${MCU} -c usbasp -U lfuse:w:0x7A:m -F -P usb

clean:
	rm -f *.bin *.hex
