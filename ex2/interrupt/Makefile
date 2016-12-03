# Example Makefile
#
# Exercise 2, TDT4258
#
TARGET = ex2
SRCDIR = src
INCLUDEDIR = include
LIBDIR = lib
OBJDIR = obj

SOURCES = $(wildcard $(SRCDIR)/*.c) 
OBJECTS = $(SOURCES:$(SRCDIR)/%.c=$(OBJDIR)/%.o)
GNUINDENTBACKUPFILES=$(wildcard $(SRCDIR)/*.c~ $(INCLUDEDIR)/*.h~) 

CC=arm-none-eabi-gcc
LD=arm-none-eabi-gcc
OBJCOPY=arm-none-eabi-objcopy
CFLAGS=-mcpu=cortex-m3 -mthumb -g -std=c99 -Wall -I$(INCLUDEDIR) 
LDFLAGS=-mcpu=cortex-m3 -mthumb -g -lgcc -lc -lcs3 -lcs3unhosted -lefm32gg  -L$(LIBDIR) 
LINKERSCRIPT=lib/efm32gg.ld

default: $(TARGET).bin

$(TARGET).bin: $(TARGET).elf
	${OBJCOPY} -j .text -O binary $< $@

$(TARGET).elf: $(OBJECTS)
	${LD} -T ${LINKERSCRIPT} $^ -o $@ ${LDFLAGS} 

$(OBJECTS): $(OBJDIR)/%.o : $(SRCDIR)/%.c | $(OBJDIR)
	${CC} ${CFLAGS} -c $< -o $@

$(OBJDIR):
	mkdir -p $(OBJDIR)

%.o : %.c
	${CC} ${CFLAGS} -c $< -o $@

.PHONY : pretty
pretty :
	-indent --no-tabs --linux --tab-size 4 --*.c *.h 

.PHONY : upload
upload :
	-eACommander.sh -r --address 0x00000000 -f "$(TARGET).bin" -r

.PHONY : clean
clean :
	@$(RM) -rv $(OBJECTS) $(OBJDIR) $(GNUINDENTBACKUPFILES)
	@-rm -rvf *.o *.elf *.bin *.hex 
