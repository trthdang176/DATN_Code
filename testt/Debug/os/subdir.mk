################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (13.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../os/bsp.c \
../os/os.c 

OBJS += \
./os/bsp.o \
./os/os.o 

C_DEPS += \
./os/bsp.d \
./os/os.d 


# Each subdirectory must supply rules for building sources it contributes
os/%.o os/%.su os/%.cyclo: ../os/%.c os/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F407xx -c -I../Core/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/CMSIS/Include -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-os

clean-os:
	-$(RM) ./os/bsp.cyclo ./os/bsp.d ./os/bsp.o ./os/bsp.su ./os/os.cyclo ./os/os.d ./os/os.o ./os/os.su

.PHONY: clean-os

