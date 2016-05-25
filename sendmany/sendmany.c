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
#include <json-c/json.h>
#include "unspent_sort.h"
#include "unspent.h"
#include "payee.h"
#include "account.h"
#include "error.h"
#include "json.h"

/* sendmany "fromaccount" {"address":amount,...} */
int main(int argc, char *argv[])
{
	struct payee *change_payee = NULL;
	double amount_use_unspent = 0.0;
	const char *change_address;
	double amount_payout = 0.0;
	double amount_fromaccount;
	const char *listunspent;
	struct unspent *ulist;
	struct unspent *ulast;
	const char *txfee_str;
	struct payee *plist;
	struct unspent *u;
	struct payee *p;
	double txfee;

	if (argc != 3) {
		fprintf(stderr, "usage: %s \"fromaccount\" "
				"{\"address\":amount,...}\n", argv[0]);
		return 1;
	}

	change_address = getenv("CHANGE_ADDRESS");
	if (!change_address)
		error_exit();
	if (!change_address[0])
		error_exit();

	listunspent = getenv("LISTUNSPENT");
	if (!listunspent)
		error_exit();
	if (!listunspent[0])
		error_exit();

	txfee_str = getenv("TX_FEE");
	if (!txfee_str)
		error_exit();
	if (!txfee_str[0])
		error_exit();

	txfee = strtod(txfee_str, NULL);
	if (txfee < 0.00000001)
		error_exit();

	plist = parse_payee(argv[2]);
	if (!plist)
		error_exit();

	amount_fromaccount = account_amount(argv[1]);

	ulist = load_unspent(listunspent, sort_insert_unspent);
	if (!ulist)
		error_exit();

	for (p = plist; p; p = p->next) {
		amount_payout += p->amount;
		if (strcmp(change_address, p->address) == 0)
			change_payee = p;
	}

	if (!change_payee)
		error_exit();

	amount_payout += txfee;

	if (amount_payout > amount_fromaccount)
		error_exit();

	for (u = ulist; u; u = u->next) {
		amount_use_unspent += u->amount;
		if (amount_use_unspent >= amount_payout)
			break;
	}
	ulast = u;

	change_payee->amount += amount_use_unspent - amount_payout;

	printf("RAW_TX=`bitcoin-cli createrawtransaction '");
	print_inputs(ulist, ulast, amount_use_unspent);
	printf("' '");
	print_outputs(plist, amount_use_unspent);
	printf("'`\n");

	printf("bitcoin-cli signrawtransaction $RAW_TX '[]' \"[");
	for (u = ulist; u; u = u->next) {
		printf("\\\"`bitcoin-cli dumpprivkey %s`\\\"", u->address);
		if (u == ulast)
			break;
		printf(",");
	}
	printf("]\"\n");

	unload_unspent(ulist);
	free_payee(plist);

	return 0;
}
