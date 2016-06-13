CC = gcc
CFLAGS = -g -Wall -Werror -D_GNU_SOURCE -I../include
LDFLAGS = -ljson-c
HEADERS = ../include/json.h ../include/error.h ../include/account.h ../include/move.h ../include/move.h ../include/util.h
SRC = move.c ../lib/json.c ../lib/account.c ../lib/error.c ../lib/util.c

move: $(SRC) $(HEADERS)
	$(CC) $(CFLAGS) $(LDFLAGS) $(SRC) -o$@

clean:
	rm -f move

.PHONY: clean
