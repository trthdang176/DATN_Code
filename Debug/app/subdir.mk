################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (13.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../app/Screen.c \
../app/app_main.c \
../app/app_post_test.c \
../app/test_os.c 

OBJS += \
./app/Screen.o \
./app/app_main.o \
./app/app_post_test.o \
./app/test_os.o 

C_DEPS += \
./app/Screen.d \
./app/app_main.d \
./app/app_post_test.d \
./app/test_os.d 


# Each subdirectory must supply rules for building sources it contributes
app/%.o app/%.su app/%.cyclo: ../app/%.c app/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m3 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F103xB -c -I../Core/Inc -I../Drivers/STM32F1xx_HAL_Driver/Inc/Legacy -I../Drivers/STM32F1xx_HAL_Driver/Inc -I../Drivers/CMSIS/Device/ST/STM32F1xx/Include -I../Drivers/CMSIS/Include -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfloat-abi=soft -mthumb -o "$@"

clean: clean-app

clean-app:
	-$(RM) ./app/Screen.cyclo ./app/Screen.d ./app/Screen.o ./app/Screen.su ./app/app_main.cyclo ./app/app_main.d ./app/app_main.o ./app/app_main.su ./app/app_post_test.cyclo ./app/app_post_test.d ./app/app_post_test.o ./app/app_post_test.su ./app/test_os.cyclo ./app/test_os.d ./app/test_os.o ./app/test_os.su

.PHONY: clean-app

