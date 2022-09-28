################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../LD/LD_config.c \
../LD/LD_prog.c 

OBJS += \
./LD/LD_config.o \
./LD/LD_prog.o 

C_DEPS += \
./LD/LD_config.d \
./LD/LD_prog.d 


# Each subdirectory must supply rules for building sources it contributes
LD/%.o: ../LD/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: AVR Compiler'
	avr-gcc -Wall -g2 -gstabs -O0 -fpack-struct -fshort-enums -ffunction-sections -fdata-sections -std=gnu99 -funsigned-char -funsigned-bitfields -mmcu=atmega32 -DF_CPU=16000000UL -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


