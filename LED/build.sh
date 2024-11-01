#!/bin/bash

# Check if the correct number of arguments is provided
if [ "$#" -ne 2 ]; then
    echo "Usage: $0 <source_file_without_extension> <output_file_without_extension>"
    exit 1
fi

# Assign arguments to variables
SOURCE_FILE="./Source/$1"
OUTPUT_ELF="Binaries/$2.elf"
OUTPUT_BIN="Binaries/$2.bin"
LINKER_SCRIPT="../coresys/LinkerScript/linker.ld"
MAP_FILE="Binaries/$2.map"

# Compile and link the project with optimizations
arm-none-eabi-gcc \
    -mcpu=cortex-m4 \
    -mthumb \
    -mfpu=fpv4-sp-d16 \
    -mfloat-abi=hard \
    -DSTM32F401RETx \
    -DNUCLEO_F401RE \
    -O2 -Os \
    -Wall \
    "$SOURCE_FILE" \
    ../coresys/Startup/startup.s \
    ../coresys/PseudoSyscalls/syscalls.c \
    ../coresys/PseudoSyscalls/sysmem.c \
    -T "$LINKER_SCRIPT" \
    --specs=nano.specs \
    -o "$OUTPUT_ELF" \
    -Wl,-Map,"$MAP_FILE" \
    -Wl,--gc-sections  # Remove unused sections

# Convert ELF to binary
arm-none-eabi-objcopy -O binary "$OUTPUT_ELF" "$OUTPUT_BIN"

echo "Compilation and linking complete."
echo "Output ELF: $OUTPUT_ELF"
echo "Output Binary: $OUTPUT_BIN"
echo "Map File: $MAP_FILE"