CC      = cc
CFLAGS  = -Wall -Wextra -O2

all: squote

squote: squote.c
	$(CC) $(CFLAGS) -o squote squote.c

test: squote_test
	./squote_test

squote_test: squote.c
	$(CC) $(CFLAGS) -DTEST -o squote_test squote.c

clean:
	rm -f squote squote_test

.PHONY: all test clean
