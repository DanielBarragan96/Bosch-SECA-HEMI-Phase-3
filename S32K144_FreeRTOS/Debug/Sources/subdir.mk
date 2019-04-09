################################################################################
# Automatically-generated file. Do not edit!
################################################################################

-include ../makefile.local

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS_QUOTED += \
"../Sources/ADC.c" \
"../Sources/can_driver.c" \
"../Sources/clocks_and_modes.c" \
"../Sources/main.c" \
"../Sources/rtos_driver.c" \
"../Sources/transceiver.c" \

C_SRCS += \
../Sources/ADC.c \
../Sources/can_driver.c \
../Sources/clocks_and_modes.c \
../Sources/main.c \
../Sources/rtos_driver.c \
../Sources/transceiver.c \

OBJS_OS_FORMAT += \
./Sources/ADC.o \
./Sources/can_driver.o \
./Sources/clocks_and_modes.o \
./Sources/main.o \
./Sources/rtos_driver.o \
./Sources/transceiver.o \

C_DEPS_QUOTED += \
"./Sources/ADC.d" \
"./Sources/can_driver.d" \
"./Sources/clocks_and_modes.d" \
"./Sources/main.d" \
"./Sources/rtos_driver.d" \
"./Sources/transceiver.d" \

OBJS += \
./Sources/ADC.o \
./Sources/can_driver.o \
./Sources/clocks_and_modes.o \
./Sources/main.o \
./Sources/rtos_driver.o \
./Sources/transceiver.o \

OBJS_QUOTED += \
"./Sources/ADC.o" \
"./Sources/can_driver.o" \
"./Sources/clocks_and_modes.o" \
"./Sources/main.o" \
"./Sources/rtos_driver.o" \
"./Sources/transceiver.o" \

C_DEPS += \
./Sources/ADC.d \
./Sources/can_driver.d \
./Sources/clocks_and_modes.d \
./Sources/main.d \
./Sources/rtos_driver.d \
./Sources/transceiver.d \


# Each subdirectory must supply rules for building sources it contributes
Sources/ADC.o: ../Sources/ADC.c
	@echo 'Building file: $<'
	@echo 'Executing target #1 $<'
	@echo 'Invoking: Standard S32DS C Compiler'
	arm-none-eabi-gcc "@Sources/ADC.args" -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -c -o "Sources/ADC.o" "$<"
	@echo 'Finished building: $<'
	@echo ' '

Sources/can_driver.o: ../Sources/can_driver.c
	@echo 'Building file: $<'
	@echo 'Executing target #2 $<'
	@echo 'Invoking: Standard S32DS C Compiler'
	arm-none-eabi-gcc "@Sources/can_driver.args" -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -c -o "Sources/can_driver.o" "$<"
	@echo 'Finished building: $<'
	@echo ' '

Sources/clocks_and_modes.o: ../Sources/clocks_and_modes.c
	@echo 'Building file: $<'
	@echo 'Executing target #3 $<'
	@echo 'Invoking: Standard S32DS C Compiler'
	arm-none-eabi-gcc "@Sources/clocks_and_modes.args" -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -c -o "Sources/clocks_and_modes.o" "$<"
	@echo 'Finished building: $<'
	@echo ' '

Sources/main.o: ../Sources/main.c
	@echo 'Building file: $<'
	@echo 'Executing target #4 $<'
	@echo 'Invoking: Standard S32DS C Compiler'
	arm-none-eabi-gcc "@Sources/main.args" -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -c -o "Sources/main.o" "$<"
	@echo 'Finished building: $<'
	@echo ' '

Sources/rtos_driver.o: ../Sources/rtos_driver.c
	@echo 'Building file: $<'
	@echo 'Executing target #5 $<'
	@echo 'Invoking: Standard S32DS C Compiler'
	arm-none-eabi-gcc "@Sources/rtos_driver.args" -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -c -o "Sources/rtos_driver.o" "$<"
	@echo 'Finished building: $<'
	@echo ' '

Sources/transceiver.o: ../Sources/transceiver.c
	@echo 'Building file: $<'
	@echo 'Executing target #6 $<'
	@echo 'Invoking: Standard S32DS C Compiler'
	arm-none-eabi-gcc "@Sources/transceiver.args" -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -c -o "Sources/transceiver.o" "$<"
	@echo 'Finished building: $<'
	@echo ' '


