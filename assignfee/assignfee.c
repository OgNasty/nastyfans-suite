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
#include "account.h"

/* assignfee "toaccount" txid */
int main(int argc, char *argv[])
{
	if (argc != 3) {
		fprintf(stderr, "usage: %s \"toaccount\" txid\n", argv[0]);
		fprintf(stderr, "    mandatory environment variables:\n");
		fprintf(stderr, "        ACCOUNT_ROOT\n");
		return 1;
	}

	account_assignfee(argv[1], argv[2]);

	return 0;
}
