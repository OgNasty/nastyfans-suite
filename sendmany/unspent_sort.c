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
#include "unspent.h"

struct unspent *sort_insert_unspent(struct unspent *sorted_ulist,
				    struct unspent *u)
{
	struct unspent *prev = NULL;
	struct unspent *cur;

	for (cur = sorted_ulist; cur; prev = cur, cur = cur->next) {
		if (u->confirmations > cur->confirmations) {
			break;
		} else if (u->confirmations == cur->confirmations) {
			if (u->amount < cur->amount)
				break;
		}
	}

	if (!prev) {
		u->next = sorted_ulist;
		sorted_ulist = u;
	} else {
		prev->next = u;
		u->next = cur;
	}

	return sorted_ulist;
}
