################################################################################
# Automatically-generated file. Do not edit!
################################################################################

#Progress monitor hint: 30
first : all
-include ../makefile.init

# This file contains definitions of environment variables used in the makefiles and .args files if exist.
-include makefile.local

RM := rm -rf

# All of the sources participating in the build are defined here
-include sources.mk
-include subdir.mk
-include Sources/subdir.mk
-include SDK/rtos/FreeRTOS_S32K/Source/subdir.mk
-include SDK/rtos/FreeRTOS_S32K/Source/portable/MemMang/subdir.mk
-include SDK/rtos/FreeRTOS_S32K/Source/portable/GCC/ARM_CM4F/subdir.mk
-include SDK/platform/hal/src/smc/subdir.mk
-include SDK/platform/hal/src/sim/S32K144/subdir.mk
-include SDK/platform/hal/src/scg/subdir.mk
-include SDK/platform/hal/src/port/subdir.mk
-include SDK/platform/hal/src/pmc/subdir.mk
-include SDK/platform/hal/src/pcc/subdir.mk
-include SDK/platform/drivers/src/pins/subdir.mk
-include SDK/platform/drivers/src/interrupt/subdir.mk
-include SDK/platform/drivers/src/clock/subdir.mk
-include SDK/platform/drivers/src/clock/S32K144/subdir.mk
-include SDK/platform/devices/subdir.mk
-include SDK/platform/devices/S32K144/startup/subdir.mk
-include Project_Settings/Startup_Code/subdir.mk
-include Generated_Code/subdir.mk
-include objects.mk

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
endif

-include ../makefile.defs

# Add inputs and outputs from these tool invocations to the build variables 
EXECUTABLES += \
S32K144_FreeRTOS.elf \

EXECUTABLES_QUOTED += \
"S32K144_FreeRTOS.elf" \

EXECUTABLES_OS_FORMAT += \
S32K144_FreeRTOS.elf \

SECONDARY_SIZE += \
S32K144_FreeRTOS.siz \

SECONDARY_SIZE_QUOTED += \
"S32K144_FreeRTOS.siz" \

SECONDARY_SIZE_OS_FORMAT += \
S32K144_FreeRTOS.siz \


# All Target
all: warning-messages explicit-dependencies S32K144_FreeRTOS.elf secondary-outputs

# Tool invocations
S32K144_FreeRTOS.elf: $(OBJS) C:/BoschProject/SECA_workspace/S32K144_FreeRTOS/Project_Settings/Linker_Files/S32K144_64_ram.ld $(USER_OBJS)
	@echo 'Building target: $@'
	@echo 'Executing target #31 $@'
	@echo 'Invoking: Standard S32DS C Linker'
	arm-none-eabi-gcc -o "S32K144_FreeRTOS.elf" "@S32K144_FreeRTOS.args"  $(USER_OBJS_QUOTED) $(LIBS_QUOTED)
	@echo 'Finished building target: $@'
	@echo ' '

S32K144_FreeRTOS.siz: $(EXECUTABLES)
	@echo 'Executing target #32 $@'
	@echo 'Invoking: Standard S32DS Print Size'
	arm-none-eabi-size --format=berkeley $(EXECUTABLES)
	@echo 'Finished building: $@'
	@echo ' '

# Other Targets
clean:
	-$(RM) ./*/*.d ./*/*/*/*/*/*.d ./*/*/*/*.d ./*/*/*/*/*/*/*.d ./*/*/*/*/*/*/*/*.d ./*/*/*/*/*.d  ./*/*.o ./*/*/*.o ./*/*/*/*/*/*.o ./*/*/*/*.o ./*/*/*/*/*/*/*.o ./*/*/*/*/*/*/*/*.o ./*/*/*/*/*.o  $(SECONDARY_SIZE_QUOTED) $(EXECUTABLES_QUOTED)
	-@echo ' '

secondary-outputs: $(SECONDARY_SIZE)

.PHONY: all clean dependents explicit-dependencies warning-messages
.SECONDARY:

-include ../makefile.targets

explicit-dependencies: 

warning-messages: 

