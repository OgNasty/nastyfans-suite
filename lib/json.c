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
#include <string.h>
#include <errno.h>
#include <json-c/json.h>
#include "unspent.h"
#include "payee.h"
#include "move.h"
#include "account.h"
#include "error.h"

static struct json_object *global_unspent_obj;
static struct json_object *global_payee_obj;
static struct unspent *global_ulist;

static const char *get_string(struct json_object *o, const char *key)
{
	struct json_object *so;
	const char *s;

	if (key) {
		if (!json_object_object_get_ex(o, key, &so))
			error_exit();
	} else {
		/* passed json_object is already int object */
		so = o;
	}

	if (!json_object_is_type(so, json_type_string))
		error_exit();

	s = json_object_get_string(so);
	if (!s)
		error_exit();

	return s;
}

static int get_int(struct json_object *o, const char *key)
{
	struct json_object *so;
	int i;

	if (key) {
		if (!json_object_object_get_ex(o, key, &so))
			error_exit();
	} else {
		/* passed json_object is already int object */
		so = o;
	}

	if (!json_object_is_type(so, json_type_int))
		error_exit();

	i = json_object_get_int(so);

	if (i == INT32_MIN)
		error_exit();

	if (i == INT32_MAX)
		error_exit();

	return i;
}

static double get_double(struct json_object *o, const char *key)
{
	struct json_object *so;
	double d;

	if (key) {
		if (!json_object_object_get_ex(o, key, &so))
			error_exit();
	} else {
		/* passed json_object is already double object */
		so = o;
	}

	if (json_object_is_type(so, json_type_double)) {
		errno = 0;
		d = json_object_get_double(so);

		if (errno)
			error_exit();

	} else if (json_object_is_type(so, json_type_int)) {
		d = get_int(so, NULL);
	} else {
		error_exit();
	}

	return d;
}

static struct unspent *alloc_unspent(struct json_object *o)
{
	struct unspent *u;

	if (!o)
		error_exit();

	u = calloc(sizeof(*u), 1);
	if (!u)
		error_exit();

	u->txid = get_string(o, "txid");
	u->vout = get_int(o, "vout");
	u->pubkey = get_string(o, "scriptPubKey");
	u->address = get_string(o, "address");
	u->amount = get_double(o, "amount");
	if (u->amount < 0.00000001)
		error_exit();
	u->confirmations = get_int(o, "confirmations");

	return u;
}

struct unspent *load_unspent(const char *filename,
	struct unspent *(insert)(struct unspent *, struct unspent *))
{
	struct unspent *ulist = NULL;
	struct json_object *o;
	struct unspent *u;
	int len;
	int i;

	if (global_unspent_obj)
		error_exit();

	if (global_ulist)
		error_exit();

	o = json_object_from_file(filename);
	if (!o)
		error_exit();

	if (!json_object_is_type(o, json_type_array))
		error_exit();

	len = json_object_array_length(o);
	if (len < 1)
		error_exit();

	for (i = 0; i < len; i++) {
		u = alloc_unspent(json_object_array_get_idx(o, i));
		if (!u)
			error_exit();
		ulist = insert(ulist, u);
	}

	global_unspent_obj = o;
	global_ulist = ulist;

	return ulist;
}

void unload_unspent(struct unspent *ulist)
{
	struct unspent *u;

	if (!global_unspent_obj)
		error_exit();

	if (!global_ulist)
		error_exit();

	while (ulist) {
		u = ulist;
		ulist = u->next;
		free(u);
	}

	json_object_put(global_unspent_obj);

	global_unspent_obj = NULL;
	global_ulist = NULL;
}

void print_inputs(struct unspent *ulist, struct unspent *ulast, double sum)
{
	struct unspent *u;
	double d = 0.0;

	for (u = ulist; u; u = u->next) {
		d += u->amount;

		if (u == ulast)
			break;
	}

	if (d != sum)
		error_exit();

	printf("[");

	for (u = ulist; u; u = u->next) {
		printf("{");
		printf("\"txid\":\"%s\",", u->txid);
		printf("\"vout\":%d,", u->vout);
		printf("\"scriptPubKey\":\"%s\"", u->pubkey);
		printf("}");

		if (u != ulast)
			printf(",");

		if (u == ulast)
			break;
	}

	printf("]");
}

static struct payee *find_payee(struct payee *plist, const char *address)
{
	struct payee *p;

	for (p = plist; p; p = p->next) {
		if (strcmp(p->address, address) == 0)
			return p;
	}

	return NULL;
}

struct payee *parse_payee(const char *arg)
{
	struct json_object_iterator it_end;
	struct json_object_iterator it;
	enum json_tokener_error error;
	struct payee *plist = NULL;
	struct json_tokener *tok;
	struct payee *plist_tail;
	struct json_object* obj;
	struct json_object* v;
	struct payee *p_dup;
	struct payee *p;
	const char *s;

	if (global_payee_obj)
		error_exit();

	tok = json_tokener_new();
	if (!tok)
		error_exit();

	json_tokener_set_flags(tok, JSON_TOKENER_STRICT);

	obj = json_tokener_parse_ex(tok, arg, strlen(arg));
	if (!obj)
		error_exit();

	error = json_tokener_get_error(tok);
	if (error != json_tokener_success)
		error_exit();

	it = json_object_iter_begin(obj);
	it_end = json_object_iter_end(obj);

	while (!json_object_iter_equal(&it, &it_end)) {
		p = calloc(sizeof(*p), 1);
		if (!p)
			error_exit();

		p->address = json_object_iter_peek_name(&it);
		if (!p->address)
			error_exit();

		s = strstr(arg, p->address);
		if (!s)
			error_exit();

		s = strstr(s + 1, p->address);
		if (s)
			error_exit();

		v = json_object_iter_peek_value(&it);
		if (!v)
			error_exit();

		p->amount = get_double(v, NULL);
		if (p->amount < 0.00000000)
			error_exit();

		p_dup = find_payee(plist, p->address);
		if (p_dup) {
			/*
			 * there should not be dup's since libjson does not
			 * support them, but we will handle them anyway
			 */
			p_dup->amount += p->amount;
			free(p);
		} else {
			/* preserve order */
			if (!plist)
				plist = p;
			else
				plist_tail->next = p;
			plist_tail = p;
		}

		json_object_iter_next(&it);
	}

	json_tokener_free(tok);

	global_payee_obj = obj;

	return plist;
}

void free_payee(struct payee *plist)
{
	struct payee *p;

	if (!global_payee_obj)
		error_exit();

	while (plist) {
		p = plist;
		plist = p->next;
		free(p);
	}

	json_object_put(global_payee_obj);

	global_payee_obj = NULL;
}

void print_outputs(struct payee *plist, double max)
{
	struct payee *p;
	double d = 0.0;

	for (p = plist; p; p = p->next) {
		if (p->amount < 0.00000001)
			continue;

		d += p->amount;
	}

	if (d > max)
		error_exit();

	printf("{");

	for (p = plist; p; p = p->next) {
		if (p->amount < 0.00000001)
			continue;

		printf("\"%s\":%0.8f", p->address, p->amount);

		if (p->next)
			printf(",");
	}

	printf("}");
}

double get_amount(const char *filename)
{
	struct json_object *o;
	double d;

	o = json_object_from_file(filename);
	if (!o)
		error_exit();

	d = get_double(o, "amount");

	json_object_put(o);

	return d;
}

unsigned int get_time(const char *filename)
{
	struct json_object *o;
	int i;

	o = json_object_from_file(filename);
	if (!o)
		error_exit();

	i = get_int(o, "time");

	json_object_put(o);

	return i;
}

char *alloc_txid(const char *filename)
{
	struct json_object *o;
	const char *cs;
	char *s;

	o = json_object_from_file(filename);
	if (!o)
		error_exit();

	cs = get_string(o, "category");
	if (strcmp(cs, "fee") == 0 ||
	    strcmp(cs, "send") == 0 ||
	    strcmp(cs, "receive") == 0) {
		cs = get_string(o, "txid");
	} else {
		cs = "NoTxId";
	}

	s = strdup(cs);
	if (!s)
		error_exit();

	json_object_put(o);

	return s;
}

#if 0
static void print_flat_object(struct json_object *obj)
{
	struct json_object_iterator it_end;
	struct json_object_iterator it;
	struct json_object *v;
	const char *key;

	it = json_object_iter_begin(obj);
	it_end = json_object_iter_end(obj);

	printf("{");
	while (!json_object_iter_equal(&it, &it_end)) {
		key = json_object_iter_peek_name(&it);
		if (!key)
			error_exit();

		v = json_object_iter_peek_value(&it);
		if (!v)
			error_exit();

		printf("\"%s\": ", key);

		if (json_object_is_type(v, json_type_double))
			printf("%0.8f", get_double(v, NULL));
		else if (json_object_is_type(v, json_type_int))
			printf("%d", get_int(v, NULL));
		else if (json_object_is_type(v, json_type_string))
			printf("\"%s\"", get_string(v, NULL));
		else
			error_exit();

		json_object_iter_next(&it);
		if (!json_object_iter_equal(&it, &it_end))
			printf(",");
	}
	printf("}");
}
#endif

void write_account_move(FILE *f, struct move *mv)
{
	fprintf(f, "{\n");
	fprintf(f, "  \"account\": \"%s\",\n", mv->account);
	fprintf(f, "  \"category\": \"move\",\n");
	fprintf(f, "  \"time\": %u,\n", mv->time);
	fprintf(f, "  \"amount\": %0.8f,\n", mv->amount);
	fprintf(f, "  \"otheraccount\": \"%s\",\n", mv->otheraccount);
	fprintf(f, "  \"comment\": \"%s\"\n", mv->comment ? mv->comment : "");
	fprintf(f, "}\n");
}

void print_accounts(struct account *alist)
{
	struct account *a;

	printf("{\n");

	for (a = alist; a; a = a->next) {
		printf("  \"%s\": %0.8f", account_get_name(a), a->amount);
		if (a->next)
			printf(",");
		printf("\n");
	}

	printf("}\n");
}
