################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Switch/Switch_config.c \
../Switch/Switch_prog.c 

OBJS += \
./Switch/Switch_config.o \
./Switch/Switch_prog.o 

C_DEPS += \
./Switch/Switch_config.d \
./Switch/Switch_prog.d 


# Each subdirectory must supply rules for building sources it contributes
Switch/%.o: ../Switch/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: AVR Compiler'
	avr-gcc -Wall -g2 -gstabs -O0 -fpack-struct -fshort-enums -ffunction-sections -fdata-sections -std=gnu99 -funsigned-char -funsigned-bitfields -mmcu=atmega32 -DF_CPU=16000000UL -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


