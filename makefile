CFLAGS = -Wall -Wpedantic -Wextra -Wswitch-enum -std=c11

stacky: main.c
	cc $(CFLAGS) $< -o $@
