CC ?= cc
CFLAGS ?= -std=c99 -Wall -Wextra -O2
SRC_DIR := ../src
COMMON_SOURCES := $(filter-out $(SRC_DIR)/main.c,$(wildcard $(SRC_DIR)/*.c))

all: table_bench

table_bench: table_bench.c $(COMMON_SOURCES)
	$(CC) $(CFLAGS) -I$(SRC_DIR) -o $@ table_bench.c $(COMMON_SOURCES)

clean:
	rm -f table_bench
