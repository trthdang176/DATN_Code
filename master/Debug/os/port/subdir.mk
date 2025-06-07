################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (13.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../os/port/os_port.c 

OBJS += \
./os/port/os_port.o 

C_DEPS += \
./os/port/os_port.d 


# Each subdirectory must supply rules for building sources it contributes
os/port/%.o os/port/%.su os/port/%.cyclo: ../os/port/%.c os/port/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m3 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F103xB -c -I../Core/Inc -I../Drivers/STM32F1xx_HAL_Driver/Inc/Legacy -I../Drivers/STM32F1xx_HAL_Driver/Inc -I../Drivers/CMSIS/Device/ST/STM32F1xx/Include -I../Drivers/CMSIS/Include -I../FATFS/Target -I../FATFS/App -I../Middlewares/Third_Party/FatFs/src -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfloat-abi=soft -mthumb -o "$@"

clean: clean-os-2f-port

clean-os-2f-port:
	-$(RM) ./os/port/os_port.cyclo ./os/port/os_port.d ./os/port/os_port.o ./os/port/os_port.su

.PHONY: clean-os-2f-port

