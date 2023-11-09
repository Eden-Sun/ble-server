BOARD?=esp32:esp32:lolin_c3_mini
PORT?=/dev/cu.usbmodem101
BUILD=build

.PHONY: default lint all flash clean

default: build upload

lint:
	cpplint --extensions=ino --filter=-legal/copyright *.ino

all:
	arduino-cli compile --fqbn $(BOARD) --output-dir $(BUILD) ./

flash:
	arduino-cli upload --fqbn $(BOARD) --port $(PORT) --input-dir $(BUILD)

clean:
	rm -r build

build:
	arduino-cli compile --fqbn $(BOARD)
upload:
	arduino-cli upload --fqbn $(BOARD) --port $(PORT)