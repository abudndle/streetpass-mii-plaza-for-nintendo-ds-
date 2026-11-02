# ARM9-only BlocksDS ROM; source/protocol and source/storage also compile on host.
BLOCKSDS ?= /opt/blocksds/core
WONDERFUL_TOOLCHAIN ?= /opt/wonderful

NAME := streetpass-plaza-ds
GAME_TITLE := StreetPass Plaza DS
GAME_SUBTITLE := DS and DSi homebrew
GAME_AUTHOR := Independent homebrew
SOURCEDIRS := source
LIBS := -ldswifi9 -lnds9
LIBDIRS := $(BLOCKSDS)/libs/dswifi
ARM7ELF := $(BLOCKSDS)/sys/arm7/main_core/arm7_dswifi_maxmod.elf
CFLAGS += -std=gnu11 -Wall -Wextra -Werror

# No proprietary art is embedded. The SDK's default banner is used.
ifeq ($(filter test,$(MAKECMDGOALS)),)
include $(BLOCKSDS)/sys/default_makefiles/rom_arm9/Makefile
endif

.PHONY: test
test:
	@mkdir -p build-host
	cc -std=c11 -D_POSIX_C_SOURCE=200809L -Wall -Wextra -Werror -pedantic -Isource source/protocol.c source/storage.c tests/test_protocol.c -o build-host/test_protocol
	./build-host/test_protocol
	cc -std=c11 -D_POSIX_C_SOURCE=200809L -Wall -Wextra -Werror -pedantic -Isource source/protocol.c source/storage.c source/background.c tests/test_background.c -o build-host/test_background
	./build-host/test_background
	python3 -m py_compile tools/inspect_ncch.py tools/romfs_inventory.py tools/private_asset_pipeline.py
	python3 -m unittest discover -s tests -p 'test_*.py' -v
