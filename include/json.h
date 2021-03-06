/*
Copyright 2016 nonnakip
This file is part of nastyfans-suite.

nastyfans-suite is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

nastyfans-suite is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with nastyfans-suite.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef JSON_H
#define JSON_H

#include <stdio.h>

struct unspent;
struct payee;
struct move;
struct account;

extern struct unspent *load_unspent(const char *filename,
	struct unspent *(insert)(struct unspent *, struct unspent *));
extern void unload_unspent(struct unspent *ulist);
extern void print_inputs(struct unspent *ulist, struct unspent *ulast,
			 long long sum);

extern struct payee *parse_payee(const char *arg);
extern void free_payee(struct payee *plist);
extern void print_outputs(struct payee *plist, long long max);

extern long long get_amount(const char *filename);
extern unsigned int get_time(const char *filename);
extern char *alloc_txid(const char *filename);

extern void write_account_move(FILE *f, struct move *mv);
extern void print_accounts(struct account *alist);

#endif /* JSON_H */
