CC = gcc
CFLAGS = -g -Wall -Werror
LDFLAGS = -ljson-c
HEADERS = json.h error.h unspent.h unspent_sort.h payee.h account.h
SRC = sendmany.c json.c unspent_sort.c account.c

sendmany: $(SRC) $(HEADERS)
	$(CC) $(CFLAGS) $(LDFLAGS) $(SRC) -o$@

clean:
	rm -f sendmany

.PHONY: clean
