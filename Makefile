#
# Author: Ice.Marek
# 2023 IceNET Technology
#
TARGET 		= stack
GCC  		= arm-none-eabi-gcc
OBJCOPY    	= arm-none-eabi-objcopy
OBJDUMP		= arm-none-eabi-objdump

CFLAGS= -DNDEBUG -DCPU_MK64FN1M0VLL12 -DUSE_RTOS=1 -DPRINTF_ADVANCED_ENABLE=1 \
-DFRDM_K64F -DFREEDOM -DFSL_RTOS_FREE_RTOS -Os -Wall -fno-common \
-ffunction-sections -fdata-sections -ffreestanding -fno-builtin \
-mthumb -mapcs -std=gnu99 -mcpu=cortex-m4 -mfloat-abi=hard \
-mfpu=fpv4-sp-d16 -MMD -MP \
--specs=nano.specs --specs=nosys.specs -Wall -fno-common -ffunction-sections \
-fdata-sections -ffreestanding -fno-builtin -mthumb -mapcs -Xlinker --gc-sections \
-Xlinker -static -Xlinker -z -Xlinker muldefs -Xlinker -Map=stack.map -mcpu=cortex-m4 \
-mfloat-abi=hard -mfpu=fpv4-sp-d16 -Xlinker --defsym=__stack_size__=2048 -Xlinker \
--defsym=__heap_size__=25600

LDSCRIPT    = linker/MK64FN1M0xxx12_flash.ld

ASM_SOURCES = $(shell find . -name "*.S")
GCC_SOURCES = $(shell find . -name "*.c")

INCLUDES=\
-Iapp/include \
-IFreeRTOS/src/portable/GCC/ARM_CM4F \
-ICMSIS \
-IFreeRTOS/include \
-IFreeRTOS/include/private \
-Idrivers/phyksz8081 \
-Ifsl \
-Iinclude \
-Idrivers/uart \
-Idrivers/serial_manager \
-Idrivers/lists \
-Ilwip/port \
-Ilwip/src \
-Ilwip/src/include \

GCC_OBJECTS = $(GCC_SOURCES:.c=.o)
GCC_DEFINIT = $(GCC_SOURCES:.c=.d)

all: elf hex bin size

elf: $(TARGET)

$(TARGET): $(GCC_OBJECTS)
	$(GCC) $(CFLAGS) -T $(LDSCRIPT) $(ASM_SOURCES) $^ -o $@ 

%.o: %.c
	$(GCC) $(CFLAGS) $(INCLUDES) -c -o $@ $<

hex: $(TARGET)
	$(OBJCOPY) $(TARGET) -O ihex $(TARGET).hex

bin: $(TARGET)
	$(OBJCOPY) $(TARGET) -O binary $(TARGET).bin

size:
	arm-none-eabi-size $(TARGET)

clean:
	rm -f $(GCC_OBJECTS) $(GCC_DEFINIT) $(TARGET) $(TARGET).hex $(TARGET).bin $(TARGET).map $(TARGET).d

.PHONY: all elf hex bin size clean
