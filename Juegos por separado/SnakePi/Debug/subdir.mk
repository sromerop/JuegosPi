################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../fsm.c \
../kbhit.c \
../snakePi.c \
../snakePiLib.c \
../tmr.c 

OBJS += \
./fsm.o \
./kbhit.o \
./snakePi.o \
./snakePiLib.o \
./tmr.o 

C_DEPS += \
./fsm.d \
./kbhit.d \
./snakePi.d \
./snakePiLib.d \
./tmr.d 


# Each subdirectory must supply rules for building sources it contributes
%.o: ../%.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross GCC Compiler'
	arm-linux-gnueabihf-gcc -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


