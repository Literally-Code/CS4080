JAVAC ?= javac
BUILD_DIR := build
JAVA_SOURCES := $(shell find com -type f -name '*.java' -print)
BUILD_STAMP := $(BUILD_DIR)/.build

.PHONY: all clean run

all: jlox

jlox: $(BUILD_STAMP)
	@printf '%s\n' '#!/usr/bin/env bash' \
		'set -e' \
		'BASE_DIR="$$(cd -- "$$(dirname -- "$${BASH_SOURCE[0]}")" && pwd)"' \
		'exec java -cp "$$BASE_DIR/$(BUILD_DIR)" com.craftinginterpreters.lox.Lox "$$@"' > $@
	@chmod +x $@

$(BUILD_STAMP): $(JAVA_SOURCES)
	@mkdir -p $(BUILD_DIR)
	$(JAVAC) -d $(BUILD_DIR) $(JAVA_SOURCES)
	@touch $@

run: jlox
	./jlox $(ARGS)

clean:
	rm -rf $(BUILD_DIR) jlox
