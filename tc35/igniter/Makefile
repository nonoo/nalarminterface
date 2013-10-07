MCU = attiny25
CC = avr-gcc
STRIP = avr-strip
OBJCOPY = avr-objcopy
AVRDUDE = avrdude -v -v -c avrispmkii -p $(MCU) -P usb

CFLAGS = -mmcu=$(MCU) -Os

main.o: main.c
	$(CC) $(CFLAGS) $? -o $@
	$(STRIP) $@

main.hex: main.o
	$(OBJCOPY) -O ihex $? $@

install: main.hex
	$(AVRDUDE) -U flash:w:main.hex:i

clean:
	rm -f *.o *.hex
