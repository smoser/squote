CC     = cc
CFLAGS = -Wall -Wextra -O2

all: squote

squote: squote.c squote.h
	$(CC) $(CFLAGS) -o squote squote.c

squote_test: squote.c squote_test.c squote.h
	$(CC) $(CFLAGS) -DNO_MAIN squote.c squote_test.c -o squote_test

test: squote_test
	./squote_test
	python3 squote_test.py

clean:
	rm -f squote squote_test

.PHONY: all test clean
