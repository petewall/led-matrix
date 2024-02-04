SHELL = /bin/bash
.PHONY: build check clean test set-pipeline upload

clean:
	rm -rf .pio platformio.ini helpers

helpers/buildFlagsToString.yaml: vendir.yml vendir.lock.yml
	vendir sync

helpers/jsonToIni.jq: vendir.yml vendir.lock.yml
	vendir sync

SRC_FILES := $(shell find . -path ./test -prune -false -o -name "*.cpp" -o -name "*.h")
TEST_FILES := $(shell find ./test -name "*.cpp" -o -name "*.h")

library.json: library.yaml version
	ytt \
		--data-value-file version=version \
		--file library.yaml | yq . > library.json

platformio.ini: platformio.yaml helpers/buildFlagsToString.yaml helpers/jsonToIni.jq
	ytt -f platformio.yaml \
		-v googletest.mock.include="${GOOGLETEST_MOCK_INCLUDE}" \
		-v googletest.test.include="${GOOGLETEST_TEST_INCLUDE}" \
		-v googletest.lib="${GOOGLETEST_LIB}" \
		-f helpers/buildFlagsToString.yaml | yq -rf helpers/jsonToIni.jq > platformio.ini

test: platformio.ini ${SRC_FILES} ${TEST_FILES}
	pio test --environment testing --verbose

check: platformio.ini ${SRC_FILES}
	pio check --verbose --skip-packages
	$(call invoke-pio,check,--verbose,--skip-packages)

build: .pio/build/led_matrix/firmware.bin

.pio/build/led_matrix/firmware.bin: platformio.ini
	pio run --environment led_matrix

upload: platformio.ini
	pio run --environment led_matrix --target upload
