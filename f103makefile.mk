MCU := F103
PART := GD32F103

TARGETS_$(MCU) := $(call get_targets,$(MCU))

HAL_FOLDER_$(MCU) := $(HAL_FOLDER)/$(call lc,$(MCU))

MCU_$(MCU) := -mthumb -mcpu=cortex-m3
LDSCRIPT_$(MCU) := $(wildcard $(HAL_FOLDER_$(MCU))/*.ld)

SRC_BASE_DIR_$(MCU) := \
	$(HAL_FOLDER_$(MCU))/Drivers/CMSIS \
	$(HAL_FOLDER_$(MCU))/Drivers/GD32F10x_standard_peripheral/Source \
	$(HAL_FOLDER_$(MCU))/Startup/gcc

SRC_DIR_$(MCU) := $(SRC_BASE_DIR_$(MCU)) \
	$(HAL_FOLDER_$(MCU))/Src

CFLAGS_$(MCU) += \
	-I$(HAL_FOLDER_$(MCU))/Inc \
	-I$(HAL_FOLDER_$(MCU))/Drivers/CMSIS \
	-I$(HAL_FOLDER_$(MCU))/Drivers/CMSIS/GD/GD32F10x/Include \
	-I$(HAL_FOLDER_$(MCU))/Drivers/GD32F10x_standard_peripheral/Include

CFLAGS_$(MCU) += \
	-D__bool_true_false_are_defined=1 \
	-DGD32F103 \
	-DGD32F10X_HD \
	-DUSE_STDPERIPH_DRIVER \
	-D__CM3_REV=0x0200 \
	-Wno-array-bounds

SRC_$(MCU) := $(foreach dir,$(SRC_DIR_$(MCU)),$(wildcard $(dir)/*.[cs]))
# Exclude ENET driver (not needed for ESC, saves flash space)
SRC_$(MCU) := $(filter-out %/gd32f10x_enet.c,$(SRC_$(MCU)))
