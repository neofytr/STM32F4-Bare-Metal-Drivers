# Compiler and binutils
CC = arm-none-eabi-gcc
OBJCOPY = arm-none-eabi-objcopy

# Directories
SRCDIR = Source
BINDIR = Binaries
COREDIR = ../coresys

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
STARTUP = $(COREDIR)/Startup/startup.s
SYSCALLS = $(COREDIR)/PseudoSyscalls/syscalls.c
SYSMEM = $(COREDIR)/PseudoSyscalls/sysmem.c
LINKER_SCRIPT = $(COREDIR)/LinkerScript/linker.ld

# Compiler flags
CFLAGS = -mcpu=cortex-m4 \
         -mthumb \
         -mfpu=fpv4-sp-d16 \
         -mfloat-abi=hard \
         -DSTM32F401RETx \
         -DNUCLEO_F401RE \
         -O2 -Os \
         -Wall \
         --specs=nano.specs

# Linker flags
LDFLAGS = -T$(LINKER_SCRIPT) \
          -Wl,-Map=$(BINDIR)/output.map \
          -Wl,--gc-sections

# Default target
all: directories $(BINDIR)/output.elf $(BINDIR)/output.bin

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
$(BINDIR)/output.elf: $(OBJ)
	$(CC) $(CFLAGS) $(OBJ) $(LDFLAGS) -o $@

# Generate binary
$(BINDIR)/output.bin: $(BINDIR)/output.elf
	$(OBJCOPY) -O binary $< $@

# Clean
clean:
	rm -rf $(BINDIR)

.PHONY: all clean directories