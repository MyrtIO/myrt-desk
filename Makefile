BOARD_HOST =
PROVISIONING_PAGE_DIR = provisioning_page

.PHONY: build
build: provisioning-page
	pio run -e release

.PHONY: flash
flash:
	pio run -e release -t upload

configure:
	@cd $(PROVISIONING_PAGE_DIR); bun install
	@pio run -t compiledb

.PHONY: format
format:
	@find \
		lib/ \
		src/ \
		-iname '*.h' -o -iname '*.c' -o -iname '*.cpp' \
		| xargs clang-format -i

.PHONY: provisioning-page
provisioning-page:
	@cd $(PROVISIONING_PAGE_DIR); bun run build
	@python3 scripts/bin2source.py \
	    $(PROVISIONING_PAGE_DIR)/dist/index.html.gz \
		src/provisioning_page \
		provisioning_page
