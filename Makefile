CC ?= cc
CFLAGS ?= -O2
WARNINGS = -std=c11 -Wall -Wextra -Werror -pedantic

.PHONY: all test clean sanitize
all: riscy_emulator

riscy_emulator: main.c riscy.c riscy.h programs.h
	$(CC) $(CFLAGS) $(WARNINGS) main.c riscy.c -o $@

tests/test_riscy: tests/test_riscy.c riscy.c riscy.h programs.h
	$(CC) $(CFLAGS) $(WARNINGS) -I. tests/test_riscy.c riscy.c -o $@

test: riscy_emulator tests/test_riscy
	./tests/test_riscy
	python3 tests/test_cli.py ./riscy_emulator

sanitize:
	$(MAKE) clean
	$(MAKE) test CFLAGS='-O1 -g -fsanitize=address,undefined -fno-omit-frame-pointer'

clean:
	rm -f riscy_emulator tests/test_riscy
