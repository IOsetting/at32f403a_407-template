##### Project #####

PROJECT			?= app
BUILD_DIR		= Build


##### Options #####

# Enable printf float %f support, y:yes, n:no
ENABLE_PRINTF_FLOAT	?= n
# Build with FreeRTOS, y:yes, n:no
USE_FREERTOS	?= n
# Programmer, jlink
FLASH_PROGRM	?= jlink


##### Toolchains #######

#ARM_TOOCHAIN	?= /opt/gcc-arm/gcc-arm-11.2-2022.02-x86_64-arm-none-eabi/bin
#ARM_TOOCHAIN	?= /opt/gcc-arm/arm-gnu-toolchain-11.3.rel1-x86_64-arm-none-eabi/bin
ARM_TOOCHAIN	?= /opt/gcc-arm/arm-gnu-toolchain-12.2.mpacbti-bet1-x86_64-arm-none-eabi/bin

# path to JLinkExe (or should be specified in PATH)
JLINKEXE		?= /opt/SEGGER/JLink/JLinkExe
JLINK_DEVICE	?= AT32F403ACGT7


##### Paths ############

# Link descript file for this chip
LDSCRIPT		= Libraries/cmsis/cm4/device_support/startup/gcc/linker/AT32F403AxG_FLASH.ld
# Library build flags
LIB_FLAGS		= USE_STDPERIPH_DRIVER AT32F403ACGT7

# C source folders
CDIRS	:= Libraries/cmsis/cm4/device_support \
		Libraries/drivers/src \
		Libraries/debug \
		User
# C source files (if there are any single ones)
CFILES := 

# ASM source folders
ADIRS	:= User
# ASM single files
AFILES	:= Libraries/cmsis/cm4/device_support/startup/gcc/startup_at32f403a_407.s

# Include paths
INCLUDES	:= User \
			Libraries/cmsis/cm4/core_support \
			Libraries/cmsis/cm4/device_support \
			Libraries/drivers/inc \
			Libraries/debug

ifeq ($(USE_FREERTOS),y)
CDIRS		+= Middlewares/FreeRTOS \
			Middlewares/FreeRTOS/portable/GCC/ARM_CM4F

CFILES		+= Middlewares/FreeRTOS/portable/MemMang/heap_4.c

INCLUDES	+= Middlewares/FreeRTOS/include \
			Middlewares/FreeRTOS/portable/GCC/ARM_CM4F
endif

include ./rules.mk
