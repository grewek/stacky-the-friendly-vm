CFLAGS = -Wall -Wpedantic -Wextra -Wswitch-enum -Wstrict-prototypes -g3 -fanalyzer -std=c11
TEST_MODE = -DTEST_MODE

stacky: main.c
	cc $(CFLAGS) $< -o $@

stacky_test: main.c
	cc $(CFLAGS) $(TEST_MODE) $< -o $@

run_test: stacky_test
	./stacky_test

run: stacky
	./stacky
