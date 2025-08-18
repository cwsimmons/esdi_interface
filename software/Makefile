
# Copyright 2024 Christopher Simmons

# This program is free software: you can redistribute it and/or modify it
# under the terms of the GNU General Public License as published by the Free
# Software Foundation, either version 2 of the License, or (at your option)
# any later version.

# This program is distributed in the hope that it will be useful, but WITHOUT
# ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
# FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for
# more details.

# You should have received a copy of the GNU General Public License along
# with this program. If not, see <https://www.gnu.org/licenses/>.


CC=aarch64-none-linux-gnu-gcc
# CC=gcc
CFLAGS=-g

DEVICE=MPSOC

INCDIR=include
OBJDIR=obj

_DEPS = addresses.h crc.h serial_command.h serial_command_values.h types.h datapath.h drive.h util.h controller.h ibm_ps2.h ibm_rt_enhanced.h adaptec_acb2322.h ultrastor_12f.h ibm_5364.h
_OBJ = main.o crc.o serial_command.o datapath.o drive.o util.o ibm_ps2.o ibm_rt_enhanced.o adaptec_acb2322.o ultrastor_12f.o ibm_5364.o

DEPS = $(patsubst %,$(INCDIR)/%,$(_DEPS))
OBJ = $(patsubst %,$(OBJDIR)/%,$(_OBJ))

$(OBJDIR)/%.o: %.c $(DEPS)
	$(CC) -c -o $@ $(CFLAGS) $< -I $(INCDIR) -D$(DEVICE)

esdi_read: $(OBJ)
	$(CC) -o $@ $^

esdi_read_zynq7: CC=arm-none-linux-gnueabihf-gcc
esdi_read_zynq7: DEVICE=ZYNQ7
esdi_read_zynq7: $(OBJ)
	$(CC) -o $@ $^

.PHONY: clean

clean:
	rm -f $(OBJDIR)/*.o 