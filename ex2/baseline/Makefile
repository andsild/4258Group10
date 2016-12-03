# Exercise 2, TDT4258
#
TARGET = ex2
SRCDIR = src
INCLUDEDIR = include
LIBDIR = lib
OBJDIR = obj

SOURCES = $(wildcard $(SRCDIR)/*.c) 
OBJECTS = $(SOURCES:$(SRCDIR)/%.c=$(OBJDIR)/%.o)

CC=arm-none-eabi-gcc
LD=arm-none-eabi-gcc
OBJCOPY=arm-none-eabi-objcopy
CFLAGS=-mcpu=cortex-m3 -mthumb -g -std=c99 -Wall -I$(INCLUDEDIR) 
LDFLAGS=-mcpu=cortex-m3 -mthumb -g -lgcc -lc -lcs3 -lcs3unhosted -lefm32gg  -L$(LIBDIR) 
ASFLAGS=-mcpu=cortex-m3 -mthumb -g
LINKERSCRIPT=lib/efm32gg.ld

default: $(TARGET).bin

$(TARGET).bin: $(TARGET).elf
	${OBJCOPY} -O binary $< $@

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
	-indent *.c *.h

.PHONY : upload
upload :
	-eACommander.sh -r --address 0x00000000 -f "$(TARGET).bin" -r

.PHONY : clean
clean :
	@$(RM) -rv $(OBJECTS) $(OBJDIR)
	@-rm -rfv *.o *.elf *.bin *.hex 
