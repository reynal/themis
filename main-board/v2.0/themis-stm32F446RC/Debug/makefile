################################################################################
# Automatically-generated file. Do not edit!
################################################################################

-include ../makefile.init

RM := rm -rf

# All of the sources participating in the build are defined here
-include sources.mk
-include Drivers/STM32F4xx_HAL_Driver/Src/subdir.mk
-include Core/Startup/subdir.mk
-include Core/Src/subdir.mk
-include subdir.mk
-include objects.mk

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(S_DEPS)),)
-include $(S_DEPS)
endif
ifneq ($(strip $(S_UPPER_DEPS)),)
-include $(S_UPPER_DEPS)
endif
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
endif

-include ../makefile.defs

# Add inputs and outputs from these tool invocations to the build variables 
EXECUTABLES += \
themis-stm32F446RC.elf \

SIZE_OUTPUT += \
default.size.stdout \

OBJDUMP_LIST += \
themis-stm32F446RC.list \

OBJCOPY_BIN += \
themis-stm32F446RC.bin \


# All Target
all: themis-stm32F446RC.elf secondary-outputs

# Tool invocations
themis-stm32F446RC.elf: $(OBJS) $(USER_OBJS) /Volumes/Samsung_T5/GitHub/themis/main-board/v2.0/themis-stm32F446RC/STM32F446RCTX_FLASH.ld
	arm-none-eabi-gcc -o "themis-stm32F446RC.elf" @"objects.list" $(USER_OBJS) $(LIBS) -mcpu=cortex-m4 -T"/Volumes/Samsung_T5/GitHub/themis/main-board/v2.0/themis-stm32F446RC/STM32F446RCTX_FLASH.ld" --specs=nosys.specs -Wl,-Map="themis-stm32F446RC.map" -Wl,--gc-sections -static --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -Wl,--start-group -lc -lm -Wl,--end-group
	@echo 'Finished building target: $@'
	@echo ' '

default.size.stdout: $(EXECUTABLES)
	arm-none-eabi-size  $(EXECUTABLES)
	@echo 'Finished building: $@'
	@echo ' '

themis-stm32F446RC.list: $(EXECUTABLES)
	arm-none-eabi-objdump -h -S $(EXECUTABLES) > "themis-stm32F446RC.list"
	@echo 'Finished building: $@'
	@echo ' '

themis-stm32F446RC.bin: $(EXECUTABLES)
	arm-none-eabi-objcopy  -O binary $(EXECUTABLES) "themis-stm32F446RC.bin"
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
