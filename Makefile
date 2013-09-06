MCU=atxmega128a3u
AVRDUDEMCU=atxmega128a3

SOURCES := $(shell find . -name '[!.]*.S') $(shell find . -name '[!.]*.c')
MODULES := $(SOURCES:.c=.o)

TARGET=main

INC=-Iavr
LIBS=-lm -lprintf_flt -Wl,-u,vfprintf

HEXFORMAT=ihex

AVRDUDE_PROGRAMMERID=avrisp2
AVRDUDE_PORT=usb

CC=avr-gcc
AS=avr-as
OBJCOPY=avr-objcopy
OBJDUMP=avr-objdump
SIZE=avr-size
AVRDUDE=avrdude
REMOVE=rm -f

LDFLAGS=-Wl,-Map,$(TARGET).map -mmcu=$(MCU) $(LIBS) -Wl,--section-start=.BOOT=0x10000

CFLAGS=-I. $(INC) -g -mmcu=$(MCU) -Os \
	-fpack-struct -fshort-enums \
	-funsigned-bitfields -funsigned-char \
	-Wall

.SUFFIXES : .c .cc .cpp .C .o .out .s .S .hex .ee.hex .h .hh .hpp

DEPFILE := $(TARGET).dep

$(TARGET): $(DEPFILE) $(MODULES)
	$(CC) $(LDFLAGS) -o $(TARGET) -Wl,--start-group $(MODULES) -Wl,--end-group

$(DEPFILE): $(SOURCES)
	$(CC) $(CFLAGS) -MM $(SOURCES) > $(DEPFILE)

all: $(TARGET)

hex: $(TARGET).hex

%.o: %.c *.h avr/*.h %.S
	$(CC) $(CFLAGS) -c $*.c -o $@

writeflash: hex
	$(AVRDUDE) -v -c $(AVRDUDE_PROGRAMMERID) -p $(AVRDUDEMCU) -P $(AVRDUDE_PORT) -e -U flash:w:$(TARGET).hex

install: writeflash

clean:
	rm -f $(MODULES:.S=.o) *.out *.hex *.lst *.out.map *.map $(TARGET) $(DEPFILE)

.out.hex:
	$(OBJCOPY) -j .text -j .data -O $(HEXFORMAT) $< $@

.out.ee.hex:
	$(OBJCOPY) -j .eeprom --change-section-lma .eeprom=0 -O $(HEXFORMAT) $< $@

stats: $(TARGET)
	$(OBJDUMP) -h $(TARGET)
	$(SIZE) $(TARGET)
