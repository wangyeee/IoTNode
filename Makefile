TARGET:=test
TOOLCHAIN_ROOT:=/Users/wang/gcc-arm-none-eabi
TOOLCHAIN_PATH:=$(TOOLCHAIN_ROOT)/bin
TOOLCHAIN_PREFIX:=arm-none-eabi

# Optimization level, can be [0, 1, 2, 3, s].
OPTLVL:=0
DBG:=-g

STARTUP:=$(CURDIR)/hardware
LINKER_SCRIPT:=$(CURDIR)/Utilities/STM32F0308.ld

BUILD_DIR = $(CURDIR)/build
BIN_DIR = $(CURDIR)/binary

INCLUDE=-I$(CURDIR)
INCLUDE+=-I$(CURDIR)/Libraries/CMSIS/Device/ST/STM32F0xx/Include
INCLUDE+=-I$(CURDIR)/Libraries/CMSIS/Include
INCLUDE+=-I$(CURDIR)/Libraries/STM32F0xx_StdPeriph_Driver/inc
INCLUDE+=-I$(CURDIR)/hardware
INCLUDE+=-I$(CURDIR)/Libraries/SPI
INCLUDE+=-I$(CURDIR)/Libraries/nrf24l01
INCLUDE+=-I$(CURDIR)/Libraries/config
INCLUDE+=-I$(CURDIR)/Libraries/GPIO
INCLUDE+=-I$(CURDIR)/Libraries/ADC
INCLUDE+=-I$(CURDIR)/Libraries/PWM

SRC_DIR=$(CURDIR)
SRC_DIR+=$(CURDIR)/Libraries/STM32F0xx_StdPeriph_Driver/src
SRC_DIR+=$(CURDIR)/Libraries/syscall
SRC_DIR+=$(CURDIR)/hardware
SRC_DIR+=$(CURDIR)/Libraries/SPI
SRC_DIR+=$(CURDIR)/Libraries/nrf24l01
SRC_DIR+=$(CURDIR)/Libraries/config
SRC_DIR+=$(CURDIR)/Libraries/GPIO
SRC_DIR+=$(CURDIR)/Libraries/ADC
SRC_DIR+=$(CURDIR)/Libraries/PWM

# vpath is used so object files are written to the current directory instead
# of the same directory as their source files
vpath %.c $(SRC_DIR)
vpath %.s $(STARTUP)

ASRC=startup_stm32f030.s

# Project Source Files
SRC+=stm32f0xx_it.c
SRC+=system_stm32f0xx.c
SRC+=main.c
SRC+=syscalls.c
SRC+=usart.c
SRC+=delay.c
SRC+=spi.c
SRC+=nrf24l01.c
SRC+=flash.c
SRC+=node_config.c
SRC+=gpio.c
SRC+=adc.c
SRC+=pwm.c

# Standard Peripheral Source Files
SRC+=stm32f0xx_adc.c
SRC+=stm32f0xx_dbgmcu.c
SRC+=stm32f0xx_i2c.c
SRC+=stm32f0xx_rtc.c
SRC+=stm32f0xx_wwdg.c
SRC+=stm32f0xx_cec.c
SRC+=stm32f0xx_dma.c
SRC+=stm32f0xx_iwdg.c
SRC+=stm32f0xx_spi.c
SRC+=stm32f0xx_comp.c
SRC+=stm32f0xx_exti.c
SRC+=stm32f0xx_misc.c
SRC+=stm32f0xx_syscfg.c
SRC+=stm32f0xx_crc.c
SRC+=stm32f0xx_flash.c
SRC+=stm32f0xx_pwr.c
SRC+=stm32f0xx_tim.c
SRC+=stm32f0xx_dac.c
SRC+=stm32f0xx_gpio.c
SRC+=stm32f0xx_rcc.c
SRC+=stm32f0xx_usart.c

CDEFS=-DUSE_STDPERIPH_DRIVER
CDEFS+=-DSTM32F0XX
CDEFS+=-DHSE_VALUE=8000000

MCUFLAGS=-mcpu=cortex-m0 -mthumb -mfloat-abi=soft
COMMONFLAGS=-O$(OPTLVL) $(DBG) -Wall
CFLAGS=$(COMMONFLAGS) $(MCUFLAGS) $(INCLUDE) $(CDEFS)
LDLIBS=-lm
LDFLAGS=$(MCUFLAGS) -fno-exceptions -ffunction-sections -fdata-sections -nostartfiles -Wl,--gc-sections,-T$(LINKER_SCRIPT)

CC=$(TOOLCHAIN_PATH)/$(TOOLCHAIN_PREFIX)-gcc
LD=$(TOOLCHAIN_PATH)/$(TOOLCHAIN_PREFIX)-gcc
OBJCOPY=$(TOOLCHAIN_PATH)/$(TOOLCHAIN_PREFIX)-objcopy
AS=$(TOOLCHAIN_PATH)/$(TOOLCHAIN_PREFIX)-as
AR=$(TOOLCHAIN_PATH)/$(TOOLCHAIN_PREFIX)-ar
GDB=$(TOOLCHAIN_PATH)/$(TOOLCHAIN_PREFIX)-gdb

OBJ = $(SRC:%.c=$(BUILD_DIR)/%.o)

$(BUILD_DIR)/%.o: %.c
	@echo [CC] $(notdir $<)
	@$(CC) $(CFLAGS) $< -c -o $@

all: $(OBJ)
	@echo [AS] $(notdir $(STARTUP)/$(ASRC))
	@$(AS) -o $(ASRC:%.s=$(BUILD_DIR)/%.o) $(STARTUP)/$(ASRC)
	@echo [LD] $(notdir $(BIN_DIR)/$(TARGET).elf)
	@$(LD) -o $(BIN_DIR)/$(TARGET).elf $(LDFLAGS) $(OBJ) $(ASRC:%.s=$(BUILD_DIR)/%.o) $(LDLIBS)
	@echo [HEX] $(notdir $(BIN_DIR)/$(TARGET).hex)
	@$(OBJCOPY) -O ihex $(BIN_DIR)/$(TARGET).elf $(BIN_DIR)/$(TARGET).hex
	@echo [BIN] $(notdir $(BIN_DIR)/$(TARGET).bin)
	@$(OBJCOPY) -O binary $(BIN_DIR)/$(TARGET).elf $(BIN_DIR)/$(TARGET).bin

.PHONY: clean tags

clean:
	@echo [RM] Objects
	@rm -f $(OBJ)
	@rm -f $(ASRC:%.s=$(BUILD_DIR)/%.o)
	@echo [RM] Binaries
	@rm -f $(BIN_DIR)/$(TARGET).elf
	@rm -f $(BIN_DIR)/$(TARGET).hex
	@rm -f $(BIN_DIR)/$(TARGET).bin


flash: all
	@st-flash write $(BIN_DIR)/$(TARGET).bin 0x8000000

reflash: clean flash

tags:
	rm -f TAGS
	find . -name "*.h" | xargs etags

# TODO for use with auto-complete clang
include:
	@echo $(INCLUDE)
