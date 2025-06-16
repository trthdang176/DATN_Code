################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (13.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../lib/AT24Cxx.c \
../lib/Common.c \
../lib/DS3231.c \
../lib/DWIN_HMI.c 

OBJS += \
./lib/AT24Cxx.o \
./lib/Common.o \
./lib/DS3231.o \
./lib/DWIN_HMI.o 

C_DEPS += \
./lib/AT24Cxx.d \
./lib/Common.d \
./lib/DS3231.d \
./lib/DWIN_HMI.d 


# Each subdirectory must supply rules for building sources it contributes
lib/%.o lib/%.su lib/%.cyclo: ../lib/%.c lib/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F302xC -c -I../Core/Inc -I../Drivers/STM32F3xx_HAL_Driver/Inc/Legacy -I../Drivers/STM32F3xx_HAL_Driver/Inc -I../Drivers/CMSIS/Device/ST/STM32F3xx/Include -I../Drivers/CMSIS/Include -I../FATFS/Target -I../FATFS/App -I../Middlewares/Third_Party/FatFs/src -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-lib

clean-lib:
	-$(RM) ./lib/AT24Cxx.cyclo ./lib/AT24Cxx.d ./lib/AT24Cxx.o ./lib/AT24Cxx.su ./lib/Common.cyclo ./lib/Common.d ./lib/Common.o ./lib/Common.su ./lib/DS3231.cyclo ./lib/DS3231.d ./lib/DS3231.o ./lib/DS3231.su ./lib/DWIN_HMI.cyclo ./lib/DWIN_HMI.d ./lib/DWIN_HMI.o ./lib/DWIN_HMI.su

.PHONY: clean-lib

