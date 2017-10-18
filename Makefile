CFLAGS := -O2 -std=c99 -pedantic
INCLUDE := -Iinclude

.PHONY: all clean

all: bin/example

bin/%: examples/%.c include/roar.h
	@mkdir -p bin
	@$(CC) -o $@ $(CFLAGS) $(INCLUDE) $<

clean:
	@rm -rf bin
