# Exercise 3, TDT4258
#
TARGET=game
SRCDIR=src
INCLUDEDIR=include
OBJDIR=obj

SOURCES=$(wildcard $(SRCDIR)/*.c) 
GNUINDENTBACKUPFILES=$(wildcard $(SRCDIR)/*.c~ $(INCLUDEDIR)/*.h~) 
OBJECTS=$(SOURCES:$(SRCDIR)/%.c=$(OBJDIR)/%.o)
INCLUDES=$(SOURCES:$(SRCDIR)/%.c=$(INCLUDEDIR)/%.h)

CC=arm-cortexm3-uclinuxeabi-gcc 
LD=arm-cortexm3-uclinuxeabi-gcc 
CFLAGS=-Ofast -Wall -I$(INCLUDEDIR) -std=gnu99 -mthumb 
LDFLAGS=-lm

all: $(TARGET)
default: $(TARGET)

$(TARGET): $(OBJECTS)
	${LD} $^ -o $@ ${LDFLAGS} 

$(OBJECTS): $(OBJDIR)/%.o : $(SRCDIR)/%.c | $(OBJDIR)
	${CC} ${CFLAGS} -c $< -o $@

$(OBJDIR):
	mkdir -p $(OBJDIR)

%.o : %.c
	${CC} ${CFLAGS} -c $< -o $@

.PHONY : pretty
pretty :
	-indent --no-tabs -linux --tab-size 4 -i4 src/*.c include/*.h

.PHONY : upload
upload :
	-eACommander.sh -r --address 0x00000000 -f "$(TARGET).bin" -r

.PHONY : clean
clean :
	@$(RM) -rv $(OBJECTS) $(OBJDIR) $(TARGET) $(TARGET).gdb platform $(GNUINDENTBACKUPFILES)
	@-rm -rvf *.o *.elf *.bin *.hex 

.PHONY : install
install :
