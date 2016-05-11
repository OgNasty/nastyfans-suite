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
#include <dirent.h>
#include <errno.h>
#include <sys/types.h>
#include "error.h"
#include "json.h"

double account_amount(const char *name)
{
	const char *account_root;
	struct dirent *entry;
	double amount = 0.0;
	char *account;
	int ret;
	DIR *d;

	account_root = getenv("ACCOUNT_ROOT");

	if (!account_root)
		error_exit();

	if (!account_root[0])
		error_exit();

	ret = asprintf(&account, "%s/%s", account_root, name);
	if (ret < 0)
		error_exit();

	d = opendir(account);
	if (!d)
		error_exit();

	while (1) {
		char *account_tx;

		entry = readdir(d);
		if (!entry)
			break;

		if (entry->d_name[0] == '.')
			continue;

		ret = asprintf(&account_tx, "%s/%s", account, entry->d_name);
		if (ret < 0)
			error_exit();

		amount += get_amount(account_tx);

		free(account_tx);
	}

	closedir(d);

	free(account);

	return amount;
}
