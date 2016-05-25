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
#include <stddef.h>
#include <string.h>
#include <dirent.h>
#include <unistd.h>
#include <time.h>
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

static struct dirent *alloc_dirent(const char *dirpath)
{
	long name_max;
	size_t len;

	name_max = pathconf(dirpath, _PC_NAME_MAX);
	if (name_max == -1)
		name_max = PATH_MAX;
	len = offsetof(struct dirent, d_name) + name_max + 1;

	return malloc(len);
}

double account_amount(const char *name)
{
	unsigned int max_time = -1;
	const char *account_root;
	const char *max_time_str;
	struct dirent *entry_buf;
	struct dirent *entry;
	double amount = 0.0;
	unsigned int time;
	char *account;
	int ret;
	DIR *d;

	max_time_str = getenv("MAX_TIME");
	if (max_time_str && max_time_str[0])
		max_time = atoi(max_time_str);

	name = name_to_dirname(name);

	account_root = get_root();

	ret = asprintf(&account, "%s/%s", account_root, name);
	if (ret < 0)
		error_exit();

	entry_buf = alloc_dirent(account);
	if (!entry_buf)
		error_exit();

	d = opendir(account);
	if (!d)
		error_exit();

	while (1) {
		char *account_tx;

		if (readdir_r(d, entry_buf, &entry) != 0)
			error_exit();
		if (!entry)
			break;

		if (entry->d_name[0] == '.')
			continue;

		time = atoi(entry->d_name);
		if (time > max_time)
			continue;

		ret = asprintf(&account_tx, "%s/%s", account, entry->d_name);
		if (ret < 0)
			error_exit();

		amount += get_amount(account_tx);

		free(account_tx);
	}

	closedir(d);

	free(entry_buf);

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

void account_assignfee(const char *name, const char *txid)
{
	const char *account_root;
	struct dirent *entry_buf;
	struct dirent *entry;
	struct move mv;
	double amount;
	char *account;
	int ret;
	DIR *d;

	account_root = get_root();

	ret = asprintf(&account, "%s/(FEE)", account_root);
	if (ret < 0)
		error_exit();

	entry_buf = alloc_dirent(account);
	if (!entry_buf)
		error_exit();

	d = opendir(account);
	if (!d)
		error_exit();

	while (1) {
		char *account_tx;
		char *id;

		if (readdir_r(d, entry_buf, &entry) != 0)
			error_exit();
		if (!entry)
			error_exit();

		if (entry->d_name[0] == '.')
			continue;

		ret = asprintf(&account_tx, "%s/%s", account, entry->d_name);
		if (ret < 0)
			error_exit();

		id = alloc_txid(account_tx);
		if (!id)
			error_exit();

		if (strcmp(txid, id) == 0) {
			amount = get_amount(account_tx);
			if (amount < 0.0)
				amount = -amount;
			free(id);
			free(account_tx);
			break;
		}

		free(id);
		free(account_tx);
	}

	closedir(d);

	free(entry_buf);

	free(account);

	memset(&mv, 0, sizeof(mv));

	mv.otheraccount = name;
	mv.account = "(FEE)";
	mv.amount = amount;
	mv.time = time(NULL);
	mv.comment = txid;

	account_move(&mv);
}

struct account *accounts_load(
	struct account *(insert)(struct account *, struct account *))
{
	struct account *alist = NULL;
	const char *account_root;
	struct dirent *entry_buf;
	struct dirent *entry;
	struct account *a;
	double amount;
	DIR *d;

	account_root = get_root();

	entry_buf = alloc_dirent(account_root);
	if (!entry_buf)
		error_exit();

	d = opendir(account_root);
	if (!d)
		error_exit();

	while (1) {
		if (readdir_r(d, entry_buf, &entry) != 0)
			error_exit();
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

		a->name = strdup(entry->d_name);
		if (!a->name)
			error_exit();
		a->amount = amount;

		alist = insert(alist, a);
	}

	closedir(d);

	free(entry_buf);

	return alist;
}

void accounts_unload(struct account *alist)
{
	struct account *a;

	while (alist) {
		a = alist;
		alist = a->next;
		free(a->name);
		free(a);
	}
}
