BOARD_TTY = /dev/cu.usbmodem11301
BAUD_RATE = 115200

.PHONY: configure
configure:
	@pio init --ide vscode
	@make build

.PHONY: build
build: config
	@pio run

.PHONY: deploy
flash: config
	@pio run -t upload -e release

.PHONY: flash-debug
flash-debug:
	@pio run -t upload -e debug
	@sleep 1
	@make monitor

.PHONY: ota
ota:
	@pio run -e release
	@python scripts/espota.py \
		-i 192.168.1.224 \
		-p 2040 \
		-f .pio/build/release/firmware.bin

.PHONY: monitor
monitor:
	pio device monitor -p $(BOARD_TTY) --baud $(BAUD_RATE)

.PHONY: format
format:
	@astyle --project=.astylerc -n \
		-r 'src/*.cc' \
		-r 'src/*.h' \
		-r 'lib/*.cc' \
		-r 'lib/*.h' \
		-r 'include/*.h'

config: src/config.yaml
	@python scripts/gen_config.py \
		src/config.yaml \
		include/config.h
