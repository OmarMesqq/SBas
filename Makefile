VALGRIND_LOG := /tmp/valgrind.log
OUTPUT := /tmp/sbas
TEST_OUTPUT := /tmp/sbas_test

LIB_REPO = https://github.com/OmarMesqq/cutilities
LIB_PATH := libs/libcutilities

INCLUDES := -I$(LIB_PATH)
LDFLAGS  := -L$(LIB_PATH) -lcutilities -Wl,-rpath,$(shell pwd)/$(LIB_PATH)

$(LIB_PATH):
	mkdir -p libs
	git clone $(LIB_REPO) $(LIB_PATH)
	$(MAKE) -C $(LIB_PATH) release

debug: $(LIB_PATH)
	gcc -g -no-pie -Wall -Wextra -Winline main.c sbas.c assembler.c linker.c $(INCLUDES) $(LDFLAGS) -o $(OUTPUT) -lm

release: $(LIB_PATH)
	gcc -O3 -Wall -Wextra -Winline  main.c sbas.c assembler.c linker.c $(INCLUDES) $(LDFLAGS) -o $(OUTPUT) -lm

test: $(LIB_PATH)
	gcc -g -Wall -Wextra -Winline run_tests.c sbas.c assembler.c linker.c $(INCLUDES) $(LDFLAGS) -o $(TEST_OUTPUT)

memleak-check: test
	@valgrind -s --leak-check=full --track-origins=yes --show-leak-kinds=all /tmp/sbas_test 2> $(VALGRIND_LOG)
	@grep -Fq "All heap blocks were freed -- no leaks are possible" $(VALGRIND_LOG) && \
	grep -Fq "ERROR SUMMARY: 0 errors from 0 contexts" $(VALGRIND_LOG) && \
	echo "✅ No leaks or errors detected." || \
	(echo "❌ Memory/resource leaks or errors found!"; cat $(VALGRIND_LOG); exit 1)

clean:
	rm -f $(VALGRIND_LOG) $(OUTPUT) $(TEST_OUTPUT)
