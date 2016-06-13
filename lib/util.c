#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "error.h"

void fprint_satoshi2btc(FILE *f, long long satoshi)
{
	const char *neg = "";
	long long decimal;
	long long whole;

	if (satoshi < 0) {
		satoshi = -satoshi;
		neg = "-";
	}

	whole = satoshi / 100000000;
	decimal = satoshi % 100000000;

	fprintf(f, "%s%lld.%08lld", neg, whole, decimal);
}

int double2satoshi(double d, long long *amount)
{
	char buf[32];
	char *p;
	int ret;

	snprintf(buf, sizeof(buf), "%.08f", d);
	buf[sizeof(buf) - 1] = 0;

	p = strchr(buf, '.');
	if (!p)
		error_exit();

	memmove(p, p + 1, strlen(p));

	for (p = &buf[0]; *p == '0'; p++)
		;

	if (!*p) {
		*amount = 0;
		return 0;
	}

	ret = sscanf(p, "%lld", amount);
	if (ret != 1)
		error_exit();
	return 0;
}

int btcstr2satoshi(const char *btcstr, long long *amount)
{
	return double2satoshi(strtod(btcstr, NULL), amount);
}
