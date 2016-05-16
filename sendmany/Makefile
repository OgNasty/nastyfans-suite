CC = gcc
CFLAGS = -g -Wall -Werror -D_GNU_SOURCE -I../include
LDFLAGS = -ljson-c
HEADERS = ../include/json.h ../include/error.h ../include/unspent.h unspent_sort.h ../include/payee.h ../include/account.h
SRC = sendmany.c ../lib/json.c unspent_sort.c ../lib/account.c ../lib/error.c

sendmany: $(SRC) $(HEADERS)
	$(CC) $(CFLAGS) $(LDFLAGS) $(SRC) -o$@

clean:
	rm -f sendmany

.PHONY: clean
