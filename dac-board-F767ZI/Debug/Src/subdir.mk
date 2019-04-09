################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Src/dac_board.c \
../Src/main.c \
../Src/stm32f7xx_hal_msp.c \
../Src/stm32f7xx_it.c \
../Src/system_stm32f7xx.c \
../Src/vco_calibration.c 

OBJS += \
./Src/dac_board.o \
./Src/main.o \
./Src/stm32f7xx_hal_msp.o \
./Src/stm32f7xx_it.o \
./Src/system_stm32f7xx.o \
./Src/vco_calibration.o 

C_DEPS += \
./Src/dac_board.d \
./Src/main.d \
./Src/stm32f7xx_hal_msp.d \
./Src/stm32f7xx_it.d \
./Src/system_stm32f7xx.d \
./Src/vco_calibration.d 


# Each subdirectory must supply rules for building sources it contributes
Src/%.o: ../Src/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: MCU GCC Compiler'
	@echo $(PWD)
	arm-none-eabi-gcc -mcpu=cortex-m7 -mthumb -mfloat-abi=hard -mfpu=fpv5-d16 '-D__weak=__attribute__((weak))' '-D__packed="__attribute__((__packed__))"' -DUSE_HAL_DRIVER -DSTM32F767xx -I"/Users/sydxrey/Desktop/dac-board-F767ZI/Inc" -I"/Users/sydxrey/Desktop/dac-board-F767ZI/Drivers/STM32F7xx_HAL_Driver/Inc" -I"/Users/sydxrey/Desktop/dac-board-F767ZI/Drivers/STM32F7xx_HAL_Driver/Inc/Legacy" -I"/Users/sydxrey/Desktop/dac-board-F767ZI/Drivers/CMSIS/Device/ST/STM32F7xx/Include" -I"/Users/sydxrey/Desktop/dac-board-F767ZI/Drivers/CMSIS/Include" -I"/Users/sydxrey/Desktop/dac-board-F767ZI/Inc"  -Og -g3 -Wall -fmessage-length=0 -ffunction-sections -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


