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

#ifndef ACCOUNT_H
#define ACCOUNT_H

struct account {
	const char *name;
	double amount;
	struct account *next;
};

struct move;

extern const char *account_get_name(struct account *a);
extern double account_amount(const char *name);
extern void account_move(struct move *mv);
extern struct account *accounts_load(
	struct account *(insert)(struct account *, struct account *));

#endif /* ACCOUNT_H */
