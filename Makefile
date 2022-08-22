PROJ_NAME 	= s32f103xb-max7219
INCLUDE_DIR = include
SRC_DIR 		= src
CMSIS_DIR   = cmsis
BUILD_DIR 	= build
LIB_DIR 		= lib

SOURCE_FILES  = max7219.c 						
OBJECT_FILES  = $(SOURCE_FILES:.c=.o)
INCLUDE_FILES = max7219.h
LINKER_SCRIPT = cmsis/STM32F103XB_FLASH.ld

CC 			= $(ARM_TOOLCHAIN_DIR)/arm-none-eabi-gcc
AR 			= $(ARM_TOOLCHAIN_DIR)/arm-none-eabi-ar
OBJCOPY = $(ARM_TOOLCHAIN_DIR)/arm-none-eabi-objcopy

CFLAGS  = -O2 -I. -I$(INCLUDE_DIR) -T$(LINKER_SCRIPT) --specs=nosys.specs # general flags and linker script
CFLAGS += -mlittle-endian -mthumb -mcpu=cortex-m3 -mfloat-abi=soft -DSTM32F103xB  # mcpu specific flags and defs
ARFLAGS = cr

# commands
all: proj

proj: lib$(PROJ_NAME).a

lib$(PROJ_NAME).a: $(OBJECT_FILES) | $(LIB_DIR)
	$(AR) $(ARFLAGS) $(LIB_DIR)/lib$(PROJ_NAME).a $(addprefix $(BUILD_DIR)/,$^)

$(OBJECT_FILES): %.o: %.c | $(BUILD_DIR)
	$(CC) -c $(CFLAGS) $(SRC_DIR)/$< -o $(BUILD_DIR)/$@ 

$(SOURCE_FILES): %.c:

$(BUILD_DIR):
	mkdir $(BUILD_DIR)

$(LIB_DIR):
	mkdir $(LIB_DIR)

TEST_SOURCE_FILES = 								\
	$(SRC_DIR)/main.c 								\
	$(CMSIS_DIR)/system_stm32f1xx.c 	\
	$(CMSIS_DIR)/startup_stm32f103xb.s 
test: proj
	$(CC) $(CFLAGS) $(TEST_SOURCE_FILES) -L$(LIB_DIR) -l$(PROJ_NAME) -o $(BUILD_DIR)/$(PROJ_NAME)_$@.elf
	$(OBJCOPY) -O binary $(BUILD_DIR)/$(PROJ_NAME)_$@.elf $(BUILD_DIR)/$(PROJ_NAME)_$@.bin

flash: test 
	st-flash write $(BUILD_DIR)/$(PROJ_NAME)_$^.bin 0x08000000

clean: proj
	rm -f $(BUILD_DIR)/*.o







