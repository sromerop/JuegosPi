################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../fsm.c \
../juegosPi.c \
../juegosPiLib.c \
../kbhit.c \
../tmr.c 

OBJS += \
./fsm.o \
./juegosPi.o \
./juegosPiLib.o \
./kbhit.o \
./tmr.o 

C_DEPS += \
./fsm.d \
./juegosPi.d \
./juegosPiLib.d \
./kbhit.d \
./tmr.d 


# Each subdirectory must supply rules for building sources it contributes
%.o: ../%.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross GCC Compiler'
	arm-linux-gnueabihf-gcc -I"C:\SysGCC\Raspberry\include" -I"C:\SysGCC\Raspberry\include\wiringPi" -O0 -g3 -Wall -Werror -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


