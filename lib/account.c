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
#include <dirent.h>
#include <unistd.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include "move.h"
#include "account.h"
#include "json.h"
#include "error.h"

const char *account_get_name(struct account *a)
{
	if (strcmp(a->name, "(EMPTY)") == 0)
		return "";
	return a->name;
}

static const char *name_to_dirname(const char *name)
{
	if (name[0] == 0)
		return "(EMPTY)";
	else
		return name;
}

static const char *get_root(void)
{
	const char *account_root;
	struct stat sb;
	int ret;

	account_root = getenv("ACCOUNT_ROOT");

	if (!account_root)
		error_exit();

	if (!account_root[0])
		error_exit();

	ret = stat(account_root, &sb);
	if (ret != 0)
		error_exit();

	if (!S_ISDIR(sb.st_mode))
		error_exit();

	return account_root;
}

double account_amount(const char *name)
{
	const char *account_root;
	struct dirent *entry;
	double amount = 0.0;
	char *account;
	int ret;
	DIR *d;

	name = name_to_dirname(name);

	account_root = get_root();

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

static void do_account_move(const char *account_root, struct move *mv)
{
	const char *filename;
	struct stat sb;
	char *account;
	FILE *f;
	int ret;
	int i;

	filename = name_to_dirname(mv->account);

	ret = asprintf(&account, "%s/%s", account_root, filename);
	if (ret < 0)
		error_exit();

	ret = stat(account, &sb);
	if (ret != 0) {
		ret = mkdir(account, 0755);
		if (ret != 0)
			error_exit();

		ret = stat(account, &sb);
		if (ret != 0)
			error_exit();
	}

	if (!S_ISDIR(sb.st_mode))
		error_exit();

	free(account);

	for (i = 0; ; i++) {
		ret = asprintf(&account, "%s/%s/%u.%d", account_root,
			       filename, mv->time, i);
		if (ret < 0)
			error_exit();

		ret = stat(account, &sb);
		if (ret != 0)
			break;

		free(account);
	}

	ret = error_add_cleanup(account);
	if (ret != 0)
		error_exit();

	f = fopen(account, "wx");
	if (!f)
		error_exit();

	write_account_move(f, mv);

	fclose(f);

	free(account);
}

void account_move(struct move *mv)
{
	const char *account_root;
	struct move mv2;
	const char *s;

	account_root = get_root();

	memcpy(&mv2, mv, sizeof(mv2));

	mv2.amount *= -1.0;
	s = mv2.account;
	mv2.account = mv2.otheraccount;
	mv2.otheraccount = s;

	do_account_move(account_root, &mv2);
	do_account_move(account_root, mv);

	error_cleanup(0);
}

struct account *accounts_load(
	struct account *(insert)(struct account *, struct account *))
{
	struct account *alist = NULL;
	const char *account_root;
	struct dirent *entry;
	struct account *a;
	double amount;
	DIR *d;

	account_root = get_root();

	d = opendir(account_root);
	if (!d)
		error_exit();

	while (1) {
		entry = readdir(d);
		if (!entry)
			break;

		if (entry->d_name[0] == '.')
			continue;

		amount = account_amount(entry->d_name);

		if (amount <= -0.00000001 || amount >= 0.00000001)
			;
		else
			continue;

		a = calloc(1, sizeof(*a));
		if (!a)
			error_exit();

		a->name = entry->d_name;
		a->amount = amount;

		alist = insert(alist, a);
	}

	closedir(d);

	return alist;
}
