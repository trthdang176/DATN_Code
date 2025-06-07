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
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F407xx -c -I../Core/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/CMSIS/Include -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-lib-2f-ISO_TP

clean-lib-2f-ISO_TP:
	-$(RM) ./lib/ISO_TP/isotp.cyclo ./lib/ISO_TP/isotp.d ./lib/ISO_TP/isotp.o ./lib/ISO_TP/isotp.su

.PHONY: clean-lib-2f-ISO_TP

