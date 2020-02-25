################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../MesSources/neoPixel.c \
../MesSources/testNeoPixel.c 

OBJS += \
./MesSources/neoPixel.o \
./MesSources/testNeoPixel.o 

C_DEPS += \
./MesSources/neoPixel.d \
./MesSources/testNeoPixel.d 


# Each subdirectory must supply rules for building sources it contributes
MesSources/neoPixel.o: ../MesSources/neoPixel.c
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DUSE_HAL_DRIVER -DDEBUG -DSTM32L432xx -c -I../Inc -I../Drivers/CMSIS/Include -I../Drivers/CMSIS/Device/ST/STM32L4xx/Include -I../Drivers/STM32L4xx_HAL_Driver/Inc -I../Drivers/STM32L4xx_HAL_Driver/Inc/Legacy -I../MesIncludes -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"MesSources/neoPixel.d" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"
MesSources/testNeoPixel.o: ../MesSources/testNeoPixel.c
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DUSE_HAL_DRIVER -DDEBUG -DSTM32L432xx -c -I../Inc -I../Drivers/CMSIS/Include -I../Drivers/CMSIS/Device/ST/STM32L4xx/Include -I../Drivers/STM32L4xx_HAL_Driver/Inc -I../Drivers/STM32L4xx_HAL_Driver/Inc/Legacy -I../MesIncludes -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"MesSources/testNeoPixel.d" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

