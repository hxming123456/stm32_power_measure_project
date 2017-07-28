################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/Data_pool.c \
../src/Fitting_method.c \
../src/_write.c \
../src/bsp_iic_2864.c \
../src/bsp_tim.c \
../src/bsp_usart.c \
../src/clib.c \
../src/fonts.c \
../src/iic_2864_gpio_simulate.c \
../src/iic_2864_operate.c \
../src/main.c 

OBJS += \
./src/Data_pool.o \
./src/Fitting_method.o \
./src/_write.o \
./src/bsp_iic_2864.o \
./src/bsp_tim.o \
./src/bsp_usart.o \
./src/clib.o \
./src/fonts.o \
./src/iic_2864_gpio_simulate.o \
./src/iic_2864_operate.o \
./src/main.o 

C_DEPS += \
./src/Data_pool.d \
./src/Fitting_method.d \
./src/_write.d \
./src/bsp_iic_2864.d \
./src/bsp_tim.d \
./src/bsp_usart.d \
./src/clib.d \
./src/fonts.d \
./src/iic_2864_gpio_simulate.d \
./src/iic_2864_operate.d \
./src/main.d 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross ARM GNU C Compiler'
	arm-none-eabi-gcc -mcpu=cortex-m3 -mthumb -Og -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -ffreestanding -fno-move-loop-invariants -Wall -Wextra  -g3 -DDEBUG -DUSE_FULL_ASSERT -DTRACE -DOS_USE_TRACE_SEMIHOSTING_DEBUG -DSTM32F10X_HD -DUSE_STDPERIPH_DRIVER -DHSE_VALUE=8000000 -I"../include" -I"../system/include" -I"../system/include/cmsis" -I"../system/include/stm32f1-stdperiph" -std=gnu11 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


