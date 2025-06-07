################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (13.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../lib/ISO_TP/isotp.c 

OBJS += \
./lib/ISO_TP/isotp.o 

C_DEPS += \
./lib/ISO_TP/isotp.d 


# Each subdirectory must supply rules for building sources it contributes
lib/ISO_TP/%.o lib/ISO_TP/%.su lib/ISO_TP/%.cyclo: ../lib/ISO_TP/%.c lib/ISO_TP/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m3 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F103xB -c -I../Core/Inc -I../Drivers/STM32F1xx_HAL_Driver/Inc/Legacy -I../Drivers/STM32F1xx_HAL_Driver/Inc -I../Drivers/CMSIS/Device/ST/STM32F1xx/Include -I../Drivers/CMSIS/Include -I../FATFS/Target -I../FATFS/App -I../Middlewares/Third_Party/FatFs/src -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfloat-abi=soft -mthumb -o "$@"

clean: clean-lib-2f-ISO_TP

clean-lib-2f-ISO_TP:
	-$(RM) ./lib/ISO_TP/isotp.cyclo ./lib/ISO_TP/isotp.d ./lib/ISO_TP/isotp.o ./lib/ISO_TP/isotp.su

.PHONY: clean-lib-2f-ISO_TP

