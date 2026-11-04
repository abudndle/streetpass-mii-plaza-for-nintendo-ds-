# ARM9-only BlocksDS ROM; source/protocol and source/storage also compile on host.
BLOCKSDS ?= /opt/blocksds/core
WONDERFUL_TOOLCHAIN ?= /opt/wonderful

NAME := streetpass-plaza-ds
GAME_TITLE := StreetPass Plaza DS
GAME_SUBTITLE := DS and DSi homebrew
GAME_AUTHOR := Independent homebrew
GAME_ICON := assets/icon.bmp
NDSTOOL_ARGS += -g SPDS
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
	cc -std=c11 -D_POSIX_C_SOURCE=200809L -Wall -Wextra -Werror -pedantic -Isource source/protocol.c source/storage.c tests/test_queue.c -o build-host/test_queue
	./build-host/test_queue
	cc -std=c11 -D_POSIX_C_SOURCE=200809L -Wall -Wextra -Werror -pedantic -Isource source/protocol.c source/storage.c tests/test_protocol.c -o build-host/test_protocol
	./build-host/test_protocol
	cc -std=c11 -D_POSIX_C_SOURCE=200809L -Wall -Wextra -Werror -pedantic -Isource source/protocol.c source/storage.c source/background.c tests/test_background.c -o build-host/test_background
	./build-host/test_background
	cc -std=c11 -D_POSIX_C_SOURCE=200809L -Wall -Wextra -Werror -pedantic -Isource source/mii_bridge.c source/service_config.c source/storage.c source/protocol.c tests/test_mii_bridge.c -o build-host/test_mii_bridge
	./build-host/test_mii_bridge
	cc -std=c11 -D_POSIX_C_SOURCE=200809L -Wall -Wextra -Werror -pedantic -Isource source/visitor.c source/mii_bridge.c source/storage.c source/protocol.c tests/test_visitor.c -o build-host/test_visitor
	./build-host/test_visitor
	cc -std=c11 -Wall -Wextra -Werror -pedantic -Isource source/discovery.c tests/test_discovery.c -o build-host/test_discovery
	./build-host/test_discovery
	cc -std=c11 -Wall -Wextra -Werror -pedantic -Isource source/bark_mode.c tests/test_bark_mode.c -o build-host/test_bark_mode
	./build-host/test_bark_mode
	cc -std=c11 -Wall -Wextra -Werror -pedantic -Isource source/relay.c source/protocol.c tests/test_relay.c -o build-host/test_relay
	./build-host/test_relay
	cc -std=c11 -Wall -Wextra -Werror -pedantic -Isource -Icompanion source/protocol.c source/relay.c companion/spdr_companion.c tests/test_spdr_companion.c -o build-host/test_spdr_companion
	./build-host/test_spdr_companion
	cc -std=c11 -Wall -Wextra -Werror -pedantic -Isource source/plaza_roster.c tests/test_plaza_roster.c -o build-host/test_plaza_roster
	./build-host/test_plaza_roster
	cc -std=c11 -Wall -Wextra -Werror -pedantic -Isource source/plaza_roster.c source/plaza_scene.c tests/test_plaza_scene.c -o build-host/test_plaza_scene
	./build-host/test_plaza_scene
	cc -std=c11 -Wall -Wextra -Werror -pedantic -Isource source/plaza_roster.c source/plaza_scene.c source/plaza_motion.c tests/test_plaza_motion.c -o build-host/test_plaza_motion
	./build-host/test_plaza_motion
	cc -std=c11 -Wall -Wextra -Werror -pedantic -Isource source/plaza_roster.c source/plaza_scene.c source/plaza_touch.c tests/test_plaza_touch.c -o build-host/test_plaza_touch
	./build-host/test_plaza_touch
	cc -std=c11 -Wall -Wextra -Werror -pedantic -Isource source/plaza_pixels.c tests/test_plaza_pixels.c -o build-host/test_plaza_pixels
	./build-host/test_plaza_pixels
	cc -std=c11 -Wall -Wextra -Werror -pedantic -Isource source/radio_ack.c source/protocol.c tests/test_radio_ack.c -o build-host/test_radio_ack
	./build-host/test_radio_ack
	cc -std=c11 -Wall -Wextra -Werror -pedantic -Isource source/arrival_notice.c tests/test_arrival_notice.c -o build-host/test_arrival_notice
	./build-host/test_arrival_notice
	cc -std=c11 -D_POSIX_C_SOURCE=200809L -Wall -Wextra -Werror -pedantic -Isource source/puzzle_swap.c source/protocol.c tests/test_puzzle_swap.c -o build-host/test_puzzle_swap
	./build-host/test_puzzle_swap
	cc -std=c11 -Wall -Wextra -Werror -pedantic -Isource source/streetpass_probe.c tests/test_streetpass_probe.c -o build-host/test_streetpass_probe
	./build-host/test_streetpass_probe
	python3 -m py_compile tools/inspect_ncch.py tools/romfs_inventory.py tools/private_asset_pipeline.py tools/verify_nds.py tools/index_darc.py
	python3 -m unittest discover -s tests -p 'test_*.py' -v
