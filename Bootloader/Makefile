# Compiler and binutils
CC = arm-none-eabi-gcc
OBJCOPY = arm-none-eabi-objcopy

# Directories
SRCDIR = ./Source
BINDIR = ../coresys/bootloader
COREDIR = ../coresys
COREBIN = ../coresys/bootloader/bootloader.bin

# Find all source files
SRC = $(wildcard $(SRCDIR)/*.c)
ASM = $(wildcard $(SRCDIR)/*.s)

# Generate object file names
OBJ = $(patsubst $(SRCDIR)/%.c,$(BINDIR)/%.o,$(SRC)) \
$(patsubst $(SRCDIR)/%.s,$(BINDIR)/%.o,$(ASM)) \
$(BINDIR)/startup.o \
$(BINDIR)/syscalls.o \
$(BINDIR)/sysmem.o

# Core system files
STARTUP = ./Startup/startup.s
SYSCALLS = $(COREDIR)/PseudoSyscalls/syscalls.c
SYSMEM = $(COREDIR)/PseudoSyscalls/sysmem.c
LINKER_SCRIPT = ./LinkerScript/linker.ld

# Compiler flags
CFLAGS = -mcpu=cortex-m4 \
	-mthumb \
	-mfpu=fpv4-sp-d16 \
	-mfloat-abi=hard \
	-DSTM32F401RETx \
	-DNUCLEO_F401RE \
	-O0 -O \
	-Wall \
	--specs=nano.specs \
	-g

# Linker flags
LDFLAGS = -T$(LINKER_SCRIPT) \
	-Wl,-Map=$(BINDIR)/bootloader.map \
	-Wl,--gc-sections

# Default target
all: directories $(BINDIR)/bootloader.elf $(BINDIR)/bootloader.bin

# Create necessary directories
directories:
	@mkdir -p $(BINDIR)

# Compile C files
$(BINDIR)/%.o: $(SRCDIR)/%.c
	$(CC) $(CFLAGS) -c $< -o $@

# Compile assembly files
$(BINDIR)/%.o: $(SRCDIR)/%.s
	$(CC) $(CFLAGS) -c $< -o $@

# Compile core system files
$(BINDIR)/startup.o: $(STARTUP)
	$(CC) $(CFLAGS) -c $< -o $@

$(BINDIR)/syscalls.o: $(SYSCALLS)
	$(CC) $(CFLAGS) -c $< -o $@

$(BINDIR)/sysmem.o: $(SYSMEM)
	$(CC) $(CFLAGS) -c $< -o $@

# Link
$(BINDIR)/bootloader.elf: $(OBJ)
	$(CC) $(CFLAGS) $(OBJ) $(LDFLAGS) -o $@

# Generate binary
$(BINDIR)/bootloader.bin: $(BINDIR)/bootloader.elf
	$(OBJCOPY) -O binary $< $@

# Clean
clean:
	rm -rf $(BINDIR)

flash:
	st-flash write $(COREBIN) 0x08000000

.PHONY: all clean directories