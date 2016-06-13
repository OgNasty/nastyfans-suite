CC = gcc
CFLAGS = -g -Wall -Werror -D_GNU_SOURCE -I../include
LDFLAGS = -ljson-c
HEADERS = ../include/json.h ../include/error.h ../include/account.h ../include/util.h
SRC = assignfee.c ../lib/json.c ../lib/account.c ../lib/error.c ../lib/util.c

assignfee: $(SRC) $(HEADERS)
	$(CC) $(CFLAGS) $(LDFLAGS) $(SRC) -o$@

clean:
	rm -f assignfee

.PHONY: clean
