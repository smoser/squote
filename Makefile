CC     = cc
CFLAGS = -Wall -Wextra -O2

all: bin-c bin-rust

bin-c: squote.c squote.h
	$(CC) $(CFLAGS) -o bin-c squote.c

bin-rust:
	cargo build --release
	cp target/release/squote bin-rust

squote_test: squote.c squote_test.c squote.h
	$(CC) $(CFLAGS) -DNO_MAIN squote.c squote_test.c -o squote_test

test: test-c test-python test-rust test-sh

test-c: squote_test
	./squote_test

test-python:
	python3 squote_test.py

test-rust:
	cargo test

test-sh:
	python3 squote_test_sh.py

clean:
	rm -f bin-c bin-rust squote_test
	cargo clean

.PHONY: all test test-c test-python test-rust test-sh clean
