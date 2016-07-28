
CC=avr-gcc.exe
LD=avr-gcc.exe
RM=del
CPU=16000000L
CFLAGS=-g -Os -Wall -DF_CPU=$(CPU) -mmcu=atmega328p -c
LFLAGS=-mmcu=atmega328p -DF_CPU=$(CPU)
INCLUDE=-I. -I"C:\Program Files (x86)\Atmel\Studio\7.0\toolchain\avr8\avr8-gnu-toolchain\avr\include"
LIB=
# The main sections of the kernel
KERNAL_MAIN_SRC=kernel\kernel.c kernel\tasks.c
# The kernel extras
KERNEL_ALT_SRC=kernel\serial.c kernel\adc.c kernel\shift.c
# All of ther kernel sources together
KERNEL_SOURCES=$(KERNAL_MAIN_SRC) $(KERNEL_ALT_SRC)

# The sources for the user program
PROGRAM_SOURCES=program\main.c program\test.c

# All of the sources together
SOURCES=$(KERNEL_SOURCES) $(PROGRAM_SOURCES)

OBJECTS=$(SOURCES:.c=.o)
OUTPUT=main
TARGET=$(OUTPUT).elf
TARGET_HEX=$(OUTPUT).hex

INCLUDE=-I.

all: $(SOURCES) link convert

link: $(OBJECTS)
	$(CC) $(LFLAGS) $(OBJECTS) -o $(TARGET)

.c.o:
	$(CC) $(INCLUDE) $(CFLAGS) $(INCLUDE) $< -o $@

convert:
	avr-objcopy -O ihex -R .eeprom $(TARGET) $(TARGET_HEX)

burn:
	avrdude -C"C:\Program Files (x86)\Arduino\hardware\tools\avr/etc/avrdude.conf" -v -patmega328p -carduino -PCOM3 -b115200 -D -Uflash:w:main.hex:i

BACKUPS=$(SOURCES:.c=.bak)

clean:
	$(RM) $(OBJECTS)
	$(RM) $(TARGET)
	$(RM) $(TARGET_HEX)
	$(RM) $(BACKUPS)
