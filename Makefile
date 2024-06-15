BOARD_TTY = /dev/tty.usbmodem11301
BAUD_RATE = 115200

.PHONY: configure
configure:
	pio init --ide vscode
	pio run

.PHONY: deploy
flash:
	pio run -t upload

.PHONY: monitor
monitor:
	pio device monitor -p $(BOARD_TTY) --baud $(BAUD_RATE)

