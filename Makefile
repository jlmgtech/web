# cflags
CFLAGS=-I. -fsanitize=null -fsanitize=undefined -fsanitize=address -g3 -Og -Wall -Wextra -Wpedantic
# libs
LIBS=-lm -lssl

# libuv dependencies
CFLAGS += $(shell pkg-config --cflags libuv)
LIBS += $(shell pkg-config --libs libuv)

.PHONY: clean test
test:
	make clean
	make a.out
	./a.out

clean:
	rm -f a.out

a.out: *.c **/*.c
	gcc -o $@ $^ $(CFLAGS) $(LIBS)

