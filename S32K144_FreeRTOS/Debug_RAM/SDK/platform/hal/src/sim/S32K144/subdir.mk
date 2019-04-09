################################################################################
# Automatically-generated file. Do not edit!
################################################################################

-include ../../../../../../makefile.local

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS_QUOTED += \
"../SDK/platform/hal/src/sim/S32K144/sim_hal_S32K144.c" \

C_SRCS += \
../SDK/platform/hal/src/sim/S32K144/sim_hal_S32K144.c \

OBJS_OS_FORMAT += \
./SDK/platform/hal/src/sim/S32K144/sim_hal_S32K144.o \

C_DEPS_QUOTED += \
"./SDK/platform/hal/src/sim/S32K144/sim_hal_S32K144.d" \

OBJS += \
./SDK/platform/hal/src/sim/S32K144/sim_hal_S32K144.o \

OBJS_QUOTED += \
"./SDK/platform/hal/src/sim/S32K144/sim_hal_S32K144.o" \

C_DEPS += \
./SDK/platform/hal/src/sim/S32K144/sim_hal_S32K144.d \


# Each subdirectory must supply rules for building sources it contributes
SDK/platform/hal/src/sim/S32K144/sim_hal_S32K144.o: ../SDK/platform/hal/src/sim/S32K144/sim_hal_S32K144.c
	@echo 'Building file: $<'
	@echo 'Executing target #16 $<'
	@echo 'Invoking: Standard S32DS C Compiler'
	arm-none-eabi-gcc "@SDK/platform/hal/src/sim/S32K144/sim_hal_S32K144.args" -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -c -o "SDK/platform/hal/src/sim/S32K144/sim_hal_S32K144.o" "$<"
	@echo 'Finished building: $<'
	@echo ' '


