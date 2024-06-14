
CC=aarch64-none-linux-gnu-gcc
# CC=gcc
CFLAGS=-g

INCDIR=include
OBJDIR=obj

_DEPS = crc.h serial_command.h serial_command_values.h types.h datapath.h drive.h util.h controller.h ibm_ps2.h ibm_rt_enhanced.h
_OBJ = main.o crc.o serial_command.o datapath.o drive.o util.o ibm_ps2.o ibm_rt_enhanced.o

DEPS = $(patsubst %,$(INCDIR)/%,$(_DEPS))
OBJ = $(patsubst %,$(OBJDIR)/%,$(_OBJ))

$(OBJDIR)/%.o: %.c $(DEPS)
	$(CC) -c -o $@ $(CFLAGS) $< -I $(INCDIR)

esdi_read: $(OBJ)
	$(CC) -o $@ $^

.PHONY: clean

clean:
	rm -f $(OBJDIR)/*.o 