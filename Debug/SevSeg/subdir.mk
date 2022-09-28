################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../SevSeg/SevSeg_config.c \
../SevSeg/SevSeg_prog.c 

OBJS += \
./SevSeg/SevSeg_config.o \
./SevSeg/SevSeg_prog.o 

C_DEPS += \
./SevSeg/SevSeg_config.d \
./SevSeg/SevSeg_prog.d 


# Each subdirectory must supply rules for building sources it contributes
SevSeg/%.o: ../SevSeg/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: AVR Compiler'
	avr-gcc -Wall -g2 -gstabs -O0 -fpack-struct -fshort-enums -ffunction-sections -fdata-sections -std=gnu99 -funsigned-char -funsigned-bitfields -mmcu=atmega32 -DF_CPU=16000000UL -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


