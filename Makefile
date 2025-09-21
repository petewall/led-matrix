SHELL = /bin/bash
.PHONY: build buildfs check clean test set-pipeline upload uploadfs \
	lint lint-cpp lint-css lint-html

clean:
	rm -rf .pio

SRC_FILES := $(shell find . -path ./test -prune -false -o -name "*.cpp" -o -name "*.h")
TEST_FILES := $(shell find ./test -name "*.cpp" -o -name "*.h")

test: platformio.ini ${SRC_FILES} ${TEST_FILES}
	pio test --environment testing --verbose

check: lint-cpp

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

CPP_FILES := ${SRC_FILES} ${TEST_FILES}
CSS_FILES := $(shell find data -name "*.css")
HTML_FILES := $(shell find data -name "*.html")

yarn.lock: package.json
	yarn install

node_modules/.bin/linthtml: yarn.lock
	yarn install

node_modules/.bin/stylelint: yarn.lock
	yarn install

lint: lint-cpp lint-css lint-html

lint-cpp: platformio.ini ${CPP_FILES}
	pio check --verbose --skip-packages

lint-css: node_modules/.bin/stylelint
	node_modules/.bin/stylelint $(CSS_FILES)

lint-html: node_modules/.bin/linthtml
	node_modules/.bin/linthtml $(HTML_FILES)
