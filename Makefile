# BlocksDS / devkitARM-style libnds project.
# Use make with DEVKITARM and BLOCKSDS installed.
ifeq ($(strip $(DEVKITARM)),)
$(error DEVKITARM is not set; install BlocksDS and source its environment)
endif

TARGET := streetpass-mii-plaza-ds
BUILD := build
SOURCES := source
INCLUDES := include
DATA :=
LIBS := -lnds9
LIBDIRS :=
ARCH := -mthumb -mthumb-interwork

include $(DEVKITARM)/ds_rules

.PHONY: all clean
all: $(TARGET).nds

$(BUILD):
	@mkdir -p $(BUILD)

$(BUILD)/main.o: source/main.c | $(BUILD)
	$(CC) $(ARCH) -O2 -Wall -Wextra -I$(BLOCKSDS)/libs/libnds/include -c $< -o $@

$(TARGET).elf: $(BUILD)/main.o
	$(CC) $(ARCH) -specs=ds_arm9.specs -o $@ $^ $(LIBS)

$(TARGET).nds: $(TARGET).elf
	ndstool -c $@ -9 $<

clean:
	rm -rf $(BUILD) $(TARGET).elf $(TARGET).nds
