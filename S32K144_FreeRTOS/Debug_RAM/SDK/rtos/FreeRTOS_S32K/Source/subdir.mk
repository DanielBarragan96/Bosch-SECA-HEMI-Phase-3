################################################################################
# Automatically-generated file. Do not edit!
################################################################################

-include ../../../../makefile.local

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS_QUOTED += \
"../SDK/rtos/FreeRTOS_S32K/Source/croutine.c" \
"../SDK/rtos/FreeRTOS_S32K/Source/event_groups.c" \
"../SDK/rtos/FreeRTOS_S32K/Source/list.c" \
"../SDK/rtos/FreeRTOS_S32K/Source/queue.c" \
"../SDK/rtos/FreeRTOS_S32K/Source/tasks.c" \
"../SDK/rtos/FreeRTOS_S32K/Source/timers.c" \

C_SRCS += \
../SDK/rtos/FreeRTOS_S32K/Source/croutine.c \
../SDK/rtos/FreeRTOS_S32K/Source/event_groups.c \
../SDK/rtos/FreeRTOS_S32K/Source/list.c \
../SDK/rtos/FreeRTOS_S32K/Source/queue.c \
../SDK/rtos/FreeRTOS_S32K/Source/tasks.c \
../SDK/rtos/FreeRTOS_S32K/Source/timers.c \

OBJS_OS_FORMAT += \
./SDK/rtos/FreeRTOS_S32K/Source/croutine.o \
./SDK/rtos/FreeRTOS_S32K/Source/event_groups.o \
./SDK/rtos/FreeRTOS_S32K/Source/list.o \
./SDK/rtos/FreeRTOS_S32K/Source/queue.o \
./SDK/rtos/FreeRTOS_S32K/Source/tasks.o \
./SDK/rtos/FreeRTOS_S32K/Source/timers.o \

C_DEPS_QUOTED += \
"./SDK/rtos/FreeRTOS_S32K/Source/croutine.d" \
"./SDK/rtos/FreeRTOS_S32K/Source/event_groups.d" \
"./SDK/rtos/FreeRTOS_S32K/Source/list.d" \
"./SDK/rtos/FreeRTOS_S32K/Source/queue.d" \
"./SDK/rtos/FreeRTOS_S32K/Source/tasks.d" \
"./SDK/rtos/FreeRTOS_S32K/Source/timers.d" \

OBJS += \
./SDK/rtos/FreeRTOS_S32K/Source/croutine.o \
./SDK/rtos/FreeRTOS_S32K/Source/event_groups.o \
./SDK/rtos/FreeRTOS_S32K/Source/list.o \
./SDK/rtos/FreeRTOS_S32K/Source/queue.o \
./SDK/rtos/FreeRTOS_S32K/Source/tasks.o \
./SDK/rtos/FreeRTOS_S32K/Source/timers.o \

OBJS_QUOTED += \
"./SDK/rtos/FreeRTOS_S32K/Source/croutine.o" \
"./SDK/rtos/FreeRTOS_S32K/Source/event_groups.o" \
"./SDK/rtos/FreeRTOS_S32K/Source/list.o" \
"./SDK/rtos/FreeRTOS_S32K/Source/queue.o" \
"./SDK/rtos/FreeRTOS_S32K/Source/tasks.o" \
"./SDK/rtos/FreeRTOS_S32K/Source/timers.o" \

C_DEPS += \
./SDK/rtos/FreeRTOS_S32K/Source/croutine.d \
./SDK/rtos/FreeRTOS_S32K/Source/event_groups.d \
./SDK/rtos/FreeRTOS_S32K/Source/list.d \
./SDK/rtos/FreeRTOS_S32K/Source/queue.d \
./SDK/rtos/FreeRTOS_S32K/Source/tasks.d \
./SDK/rtos/FreeRTOS_S32K/Source/timers.d \


# Each subdirectory must supply rules for building sources it contributes
SDK/rtos/FreeRTOS_S32K/Source/croutine.o: ../SDK/rtos/FreeRTOS_S32K/Source/croutine.c
	@echo 'Building file: $<'
	@echo 'Executing target #7 $<'
	@echo 'Invoking: Standard S32DS C Compiler'
	arm-none-eabi-gcc "@SDK/rtos/FreeRTOS_S32K/Source/croutine.args" -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -c -o "SDK/rtos/FreeRTOS_S32K/Source/croutine.o" "$<"
	@echo 'Finished building: $<'
	@echo ' '

SDK/rtos/FreeRTOS_S32K/Source/event_groups.o: ../SDK/rtos/FreeRTOS_S32K/Source/event_groups.c
	@echo 'Building file: $<'
	@echo 'Executing target #8 $<'
	@echo 'Invoking: Standard S32DS C Compiler'
	arm-none-eabi-gcc "@SDK/rtos/FreeRTOS_S32K/Source/event_groups.args" -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -c -o "SDK/rtos/FreeRTOS_S32K/Source/event_groups.o" "$<"
	@echo 'Finished building: $<'
	@echo ' '

SDK/rtos/FreeRTOS_S32K/Source/list.o: ../SDK/rtos/FreeRTOS_S32K/Source/list.c
	@echo 'Building file: $<'
	@echo 'Executing target #9 $<'
	@echo 'Invoking: Standard S32DS C Compiler'
	arm-none-eabi-gcc "@SDK/rtos/FreeRTOS_S32K/Source/list.args" -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -c -o "SDK/rtos/FreeRTOS_S32K/Source/list.o" "$<"
	@echo 'Finished building: $<'
	@echo ' '

SDK/rtos/FreeRTOS_S32K/Source/queue.o: ../SDK/rtos/FreeRTOS_S32K/Source/queue.c
	@echo 'Building file: $<'
	@echo 'Executing target #10 $<'
	@echo 'Invoking: Standard S32DS C Compiler'
	arm-none-eabi-gcc "@SDK/rtos/FreeRTOS_S32K/Source/queue.args" -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -c -o "SDK/rtos/FreeRTOS_S32K/Source/queue.o" "$<"
	@echo 'Finished building: $<'
	@echo ' '

SDK/rtos/FreeRTOS_S32K/Source/tasks.o: ../SDK/rtos/FreeRTOS_S32K/Source/tasks.c
	@echo 'Building file: $<'
	@echo 'Executing target #11 $<'
	@echo 'Invoking: Standard S32DS C Compiler'
	arm-none-eabi-gcc "@SDK/rtos/FreeRTOS_S32K/Source/tasks.args" -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -c -o "SDK/rtos/FreeRTOS_S32K/Source/tasks.o" "$<"
	@echo 'Finished building: $<'
	@echo ' '

SDK/rtos/FreeRTOS_S32K/Source/timers.o: ../SDK/rtos/FreeRTOS_S32K/Source/timers.c
	@echo 'Building file: $<'
	@echo 'Executing target #12 $<'
	@echo 'Invoking: Standard S32DS C Compiler'
	arm-none-eabi-gcc "@SDK/rtos/FreeRTOS_S32K/Source/timers.args" -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -c -o "SDK/rtos/FreeRTOS_S32K/Source/timers.o" "$<"
	@echo 'Finished building: $<'
	@echo ' '


