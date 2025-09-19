SHELL = /bin/bash
.PHONY: build buildfs check clean test set-pipeline upload uploadfs

clean:
	rm -rf .pio

SRC_FILES := $(shell find . -path ./test -prune -false -o -name "*.cpp" -o -name "*.h")
TEST_FILES := $(shell find ./test -name "*.cpp" -o -name "*.h")

test: platformio.ini ${SRC_FILES} ${TEST_FILES}
	pio test --environment testing --verbose

check: platformio.ini ${SRC_FILES}
	pio check --verbose --skip-packages

build: .pio/build/led_matrix/firmware.bin

.pio/build/led_matrix/firmware.bin: platformio.ini
	pio run --environment led_matrix

upload: platformio.ini .pio/build/led_matrix/firmware.bin
	pio run --environment led_matrix --target upload

buildfs: .pio/build/led_matrix/littlefs.bin

DATA_FILES=$(shell find data -type f)
.pio/build/led_matrix/littlefs.bin: platformio.ini $(DATA_FILES)
	pio run --environment led_matrix --target buildfs

# Upload LittleFS filesystem image from ./public to the device
uploadfs: .pio/build/led_matrix/littlefs.bin
	pio run --environment led_matrix --target uploadfs
