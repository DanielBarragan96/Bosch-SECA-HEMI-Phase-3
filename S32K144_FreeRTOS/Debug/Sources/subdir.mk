################################################################################
# Automatically-generated file. Do not edit!
################################################################################

-include ../makefile.local

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS_QUOTED += \
"../Sources/can_driver.c" \
"../Sources/clocks_and_modes.c" \
"../Sources/main.c" \
<<<<<<< HEAD
"../Sources/rtos_driver.c" \
=======
"../Sources/rtos_can_driver.c" \
>>>>>>> a3dd161c8022f4657cb604d20d963ad7e7804d23
"../Sources/transceiver.c" \

C_SRCS += \
../Sources/can_driver.c \
../Sources/clocks_and_modes.c \
../Sources/main.c \
<<<<<<< HEAD
../Sources/rtos_driver.c \
=======
../Sources/rtos_can_driver.c \
>>>>>>> a3dd161c8022f4657cb604d20d963ad7e7804d23
../Sources/transceiver.c \

OBJS_OS_FORMAT += \
./Sources/can_driver.o \
./Sources/clocks_and_modes.o \
./Sources/main.o \
<<<<<<< HEAD
./Sources/rtos_driver.o \
=======
./Sources/rtos_can_driver.o \
>>>>>>> a3dd161c8022f4657cb604d20d963ad7e7804d23
./Sources/transceiver.o \

C_DEPS_QUOTED += \
"./Sources/can_driver.d" \
"./Sources/clocks_and_modes.d" \
"./Sources/main.d" \
<<<<<<< HEAD
"./Sources/rtos_driver.d" \
=======
"./Sources/rtos_can_driver.d" \
>>>>>>> a3dd161c8022f4657cb604d20d963ad7e7804d23
"./Sources/transceiver.d" \

OBJS += \
./Sources/can_driver.o \
./Sources/clocks_and_modes.o \
./Sources/main.o \
<<<<<<< HEAD
./Sources/rtos_driver.o \
=======
./Sources/rtos_can_driver.o \
>>>>>>> a3dd161c8022f4657cb604d20d963ad7e7804d23
./Sources/transceiver.o \

OBJS_QUOTED += \
"./Sources/can_driver.o" \
"./Sources/clocks_and_modes.o" \
"./Sources/main.o" \
<<<<<<< HEAD
"./Sources/rtos_driver.o" \
=======
"./Sources/rtos_can_driver.o" \
>>>>>>> a3dd161c8022f4657cb604d20d963ad7e7804d23
"./Sources/transceiver.o" \

C_DEPS += \
./Sources/can_driver.d \
./Sources/clocks_and_modes.d \
./Sources/main.d \
<<<<<<< HEAD
./Sources/rtos_driver.d \
=======
./Sources/rtos_can_driver.d \
>>>>>>> a3dd161c8022f4657cb604d20d963ad7e7804d23
./Sources/transceiver.d \


# Each subdirectory must supply rules for building sources it contributes
Sources/can_driver.o: ../Sources/can_driver.c
	@echo 'Building file: $<'
	@echo 'Executing target #1 $<'
	@echo 'Invoking: Standard S32DS C Compiler'
	arm-none-eabi-gcc "@Sources/can_driver.args" -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -c -o "Sources/can_driver.o" "$<"
	@echo 'Finished building: $<'
	@echo ' '

Sources/clocks_and_modes.o: ../Sources/clocks_and_modes.c
	@echo 'Building file: $<'
	@echo 'Executing target #2 $<'
	@echo 'Invoking: Standard S32DS C Compiler'
	arm-none-eabi-gcc "@Sources/clocks_and_modes.args" -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -c -o "Sources/clocks_and_modes.o" "$<"
	@echo 'Finished building: $<'
	@echo ' '

Sources/main.o: ../Sources/main.c
	@echo 'Building file: $<'
	@echo 'Executing target #3 $<'
	@echo 'Invoking: Standard S32DS C Compiler'
	arm-none-eabi-gcc "@Sources/main.args" -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -c -o "Sources/main.o" "$<"
	@echo 'Finished building: $<'
	@echo ' '

<<<<<<< HEAD
Sources/rtos_driver.o: ../Sources/rtos_driver.c
	@echo 'Building file: $<'
	@echo 'Executing target #4 $<'
	@echo 'Invoking: Standard S32DS C Compiler'
	arm-none-eabi-gcc "@Sources/rtos_driver.args" -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -c -o "Sources/rtos_driver.o" "$<"
=======
Sources/rtos_can_driver.o: ../Sources/rtos_can_driver.c
	@echo 'Building file: $<'
	@echo 'Executing target #4 $<'
	@echo 'Invoking: Standard S32DS C Compiler'
	arm-none-eabi-gcc "@Sources/rtos_can_driver.args" -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -c -o "Sources/rtos_can_driver.o" "$<"
>>>>>>> a3dd161c8022f4657cb604d20d963ad7e7804d23
	@echo 'Finished building: $<'
	@echo ' '

Sources/transceiver.o: ../Sources/transceiver.c
	@echo 'Building file: $<'
	@echo 'Executing target #5 $<'
	@echo 'Invoking: Standard S32DS C Compiler'
	arm-none-eabi-gcc "@Sources/transceiver.args" -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -c -o "Sources/transceiver.o" "$<"
	@echo 'Finished building: $<'
	@echo ' '


