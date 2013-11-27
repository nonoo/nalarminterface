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
STRIP=avr-strip
OBJCOPY=avr-objcopy
OBJDUMP=avr-objdump
SIZE=avr-size
AVRDUDE=avrdude
DFUPROGRAMMER=dfu-programmer
REMOVE=rm -f

LDFLAGS=-Wl,-Map,$(TARGET).map -mmcu=$(MCU) $(LIBS)

CFLAGS=-I. $(INC) -g -mmcu=$(MCU) -Os \
	-fpack-struct -fshort-enums \
	-funsigned-bitfields -funsigned-char \
	-Wall

.SUFFIXES : .c .cc .cpp .C .o .out .s .S .hex .ee.hex .h .hh .hpp

DEPFILE := $(TARGET).dep

$(TARGET): $(DEPFILE) $(MODULES)
	$(CC) $(LDFLAGS) -o $(TARGET) -Wl,--start-group $(MODULES) -Wl,--end-group
	$(STRIP) $(TARGET)

$(DEPFILE): $(SOURCES)
	$(CC) $(CFLAGS) -MM $(SOURCES) > $(DEPFILE)

all: $(TARGET)

hex: $(TARGET).hex

%.o: %.c *.h avr/*.h %.S
	$(CC) $(CFLAGS) -c $*.c -o $@

writeflash:
	$(AVRDUDE) -c $(AVRDUDE_PROGRAMMERID) -p $(AVRDUDEMCU) -P $(AVRDUDE_PORT) -U flash:w:$(TARGET).hex -U eeprom:w:$(AVRDUDEMCU).eeprom.hex:i

readeeprom:
	$(AVRDUDE) -c $(AVRDUDE_PROGRAMMERID) -p $(AVRDUDEMCU) -P $(AVRDUDE_PORT) -U eeprom:r:$(AVRDUDEMCU).eeprom.hex:i

erase:
	$(AVRDUDE) -c $(AVRDUDE_PROGRAMMERID) -p $(AVRDUDEMCU) -P $(AVRDUDE_PORT) -e

ispinstall: hex readeeprom erase writeflash

install: erasedfu writedfu

erasedfu:
	$(DFUPROGRAMMER) $(MCU) erase

writedfu: hex
	$(DFUPROGRAMMER) $(MCU) flash $(TARGET).hex

clean:
	rm -f $(MODULES:.S=.o) *.out *.hex *.lst *.out.map *.map $(TARGET) $(DEPFILE)

.out.hex:
	$(OBJCOPY) -j .text -j .data -O $(HEXFORMAT) $< $@

.out.ee.hex:
	$(OBJCOPY) -j .eeprom --change-section-lma .eeprom=0 -O $(HEXFORMAT) $< $@

stats: $(TARGET)
	$(OBJDUMP) -h $(TARGET)
	$(SIZE) --mcu=$(MCU) -C $(TARGET)
