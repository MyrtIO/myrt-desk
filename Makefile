BOARD_TTY = /dev/tty.usbmodem11301
BAUD_RATE = 115200

.PHONY: generate
generate:
	@python scripts/generate_config_example.py
	@python scripts/generate_file_strings.py

.PHONY: configure
configure:
	pio init --ide vscode
	make build

.PHONY: build
build:
	@make generate
	pio run

.PHONY: deploy
flash:
	pio run -t upload

.PHONY: monitor
monitor:
	pio device monitor -p $(BOARD_TTY) --baud $(BAUD_RATE)

