################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../fsm.c \
../kbhit.c \
../pongPi.c \
../pongPiLib.c \
../tmr.c 

OBJS += \
./fsm.o \
./kbhit.o \
./pongPi.o \
./pongPiLib.o \
./tmr.o 

C_DEPS += \
./fsm.d \
./kbhit.d \
./pongPi.d \
./pongPiLib.d \
./tmr.d 


# Each subdirectory must supply rules for building sources it contributes
%.o: ../%.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross GCC Compiler'
	arm-linux-gnueabihf-gcc -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


