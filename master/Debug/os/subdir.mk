################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (13.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../os/bsp.c \
../os/os.c \
../os/os_event.c 

OBJS += \
./os/bsp.o \
./os/os.o \
./os/os_event.o 

C_DEPS += \
./os/bsp.d \
./os/os.d \
./os/os_event.d 


# Each subdirectory must supply rules for building sources it contributes
os/%.o os/%.su os/%.cyclo: ../os/%.c os/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m3 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F103xB -c -I../Core/Inc -I../Drivers/STM32F1xx_HAL_Driver/Inc/Legacy -I../Drivers/STM32F1xx_HAL_Driver/Inc -I../Drivers/CMSIS/Device/ST/STM32F1xx/Include -I../Drivers/CMSIS/Include -I../FATFS/Target -I../FATFS/App -I../Middlewares/Third_Party/FatFs/src -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfloat-abi=soft -mthumb -o "$@"

clean: clean-os

clean-os:
	-$(RM) ./os/bsp.cyclo ./os/bsp.d ./os/bsp.o ./os/bsp.su ./os/os.cyclo ./os/os.d ./os/os.o ./os/os.su ./os/os_event.cyclo ./os/os_event.d ./os/os_event.o ./os/os_event.su

.PHONY: clean-os

