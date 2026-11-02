# ARM9-only BlocksDS ROM; source/protocol and source/storage also compile on host.
BLOCKSDS ?= /opt/blocksds/core
WONDERFUL_TOOLCHAIN ?= /opt/wonderful

NAME := streetpass-plaza-ds
GAME_TITLE := StreetPass Plaza DS
GAME_SUBTITLE := DS and DSi homebrew
GAME_AUTHOR := Independent homebrew
SOURCEDIRS := source
LIBS := -lnds9
CFLAGS += -std=gnu11 -Wall -Wextra -Werror

# No proprietary art is embedded. The SDK's default banner is used.
include $(BLOCKSDS)/sys/default_makefiles/rom_arm9/Makefile

.PHONY: test
test:
	@mkdir -p build-host
	cc -std=c11 -D_POSIX_C_SOURCE=200809L -Wall -Wextra -Werror -pedantic -Isource source/protocol.c source/storage.c tests/test_protocol.c -o build-host/test_protocol
	./build-host/test_protocol
