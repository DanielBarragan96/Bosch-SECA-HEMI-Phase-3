################################################################################
# Automatically-generated file. Do not edit!
################################################################################

-include ../../../../../makefile.local

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS_QUOTED += \
"../SDK/platform/hal/src/smc/smc_hal.c" \

C_SRCS += \
../SDK/platform/hal/src/smc/smc_hal.c \

OBJS_OS_FORMAT += \
./SDK/platform/hal/src/smc/smc_hal.o \

C_DEPS_QUOTED += \
"./SDK/platform/hal/src/smc/smc_hal.d" \

OBJS += \
./SDK/platform/hal/src/smc/smc_hal.o \

OBJS_QUOTED += \
"./SDK/platform/hal/src/smc/smc_hal.o" \

C_DEPS += \
./SDK/platform/hal/src/smc/smc_hal.d \


# Each subdirectory must supply rules for building sources it contributes
SDK/platform/hal/src/smc/smc_hal.o: ../SDK/platform/hal/src/smc/smc_hal.c
	@echo 'Building file: $<'
	@echo 'Executing target #15 $<'
	@echo 'Invoking: Standard S32DS C Compiler'
	arm-none-eabi-gcc "@SDK/platform/hal/src/smc/smc_hal.args" -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -c -o "SDK/platform/hal/src/smc/smc_hal.o" "$<"
	@echo 'Finished building: $<'
	@echo ' '


