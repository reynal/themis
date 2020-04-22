################################################################################
# Automatically-generated file. Do not edit!
################################################################################

-include ../makefile.init

RM := rm -rf

# All of the sources participating in the build are defined here
-include sources.mk
-include Startup/subdir.mk
-include Src/subdir.mk
-include Drivers/STM32F7xx_HAL_Driver/Src/subdir.mk
-include subdir.mk
-include objects.mk

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
endif

-include ../makefile.defs

# Add inputs and outputs from these tool invocations to the build variables 
EXECUTABLES += \
dac-board-F767ZI.elf \

SIZE_OUTPUT += \
default.size.stdout \

OBJDUMP_LIST += \
dac-board-F767ZI.list \

OBJCOPY_BIN += \
dac-board-F767ZI.bin \


# All Target
all: dac-board-F767ZI.elf secondary-outputs

# Tool invocations
dac-board-F767ZI.elf: $(OBJS) $(USER_OBJS) /Volumes/Storeva\ 1/Dropbox/dev/GitHub/themis/dac-board-F767ZI/STM32F767ZITX_FLASH.ld
	arm-none-eabi-gcc -o "dac-board-F767ZI.elf" @"objects.list" $(USER_OBJS) $(LIBS) -mcpu=cortex-m7 -T"/Volumes/Storeva 1/Dropbox/dev/GitHub/themis/dac-board-F767ZI/STM32F767ZITX_FLASH.ld" --specs=nosys.specs -Wl,-Map="dac-board-F767ZI.map" -Wl,--gc-sections -static --specs=nano.specs -mfpu=fpv5-d16 -mfloat-abi=hard -mthumb -Wl,--start-group -lc -lm -Wl,--end-group
	@echo 'Finished building target: $@'
	@echo ' '

default.size.stdout: $(EXECUTABLES)
	arm-none-eabi-size  $(EXECUTABLES)
	@echo 'Finished building: $@'
	@echo ' '

dac-board-F767ZI.list: $(EXECUTABLES)
	arm-none-eabi-objdump -h -S $(EXECUTABLES) > "dac-board-F767ZI.list"
	@echo 'Finished building: $@'
	@echo ' '

dac-board-F767ZI.bin: $(EXECUTABLES)
	arm-none-eabi-objcopy  -O binary $(EXECUTABLES) "dac-board-F767ZI.bin"
	@echo 'Finished building: $@'
	@echo ' '

# Other Targets
clean:
	-$(RM) *
	-@echo ' '

secondary-outputs: $(SIZE_OUTPUT) $(OBJDUMP_LIST) $(OBJCOPY_BIN)

.PHONY: all clean dependents
.SECONDARY:

-include ../makefile.targets
