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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "account.h"
#include "json.h"

static struct account *insert_alphabetical(struct account *sorted_alist,
					   struct account *a)
{
	struct account *prev = NULL;
	const char *use_name;
	struct account *cur;

	use_name = account_get_name(a);

	for (cur = sorted_alist; cur; prev = cur, cur = cur->next) {
		if (strcmp(use_name, cur->name) < 0)
			break;
	}

	if (!prev) {
		a->next = sorted_alist;
		sorted_alist = a;
	} else {
		prev->next = a;
		a->next = cur;
	}

	return sorted_alist;
}

/* listaccounts */
int main(int argc, char *argv[])
{
	struct account *alist;

	if (argc != 1) {
		fprintf(stderr, "usage: %s\n", argv[0]);
		return 1;
	}

	alist = accounts_load(insert_alphabetical);

	print_accounts(alist);

	accounts_unload(alist);

	return 0;
}
