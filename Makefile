BOARD_TTY = /dev/tty.usbmodem11301
BAUD_RATE = 115200

MQTT_CONFIGS = src/controllers/mqtt/topics/config_*.json
MQTT_CONFIG_BASENAME = src/controllers/mqtt/topics/config_data

.PHONY: configure
configure:
	pio init --ide vscode
	make build

.PHONY: build
build: $(MQTT_CONFIG_BASENAME).h src/config.example.h
	pio run

.PHONY: deploy
flash: $(MQTT_CONFIG_BASENAME).h src/config.example.h
	pio run -t upload

.PHONY: monitor
monitor:
	pio device monitor -p $(BOARD_TTY) --baud $(BAUD_RATE)

.PHONY: format
format:
	find include src lib -iname '*.cc' -o -iname '*.h' -o -iname '*.cpp' | xargs clang-format -i

src/config.example.h: src/config.h
	@python scripts/gen_replace.py \
		src/config.h \
		src/config.example.h

$(MQTT_CONFIG_BASENAME).h: $(wildcard $(MQTT_CONFIGS))
	@python scripts/gen_content_char.py \
		"$(MQTT_CONFIG_BASENAME)" \
		$(MQTT_CONFIGS)
