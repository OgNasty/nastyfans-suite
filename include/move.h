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

#ifndef MOVE_H
#define MOVE_H

struct move {
	const char *account;
	unsigned int time;
	double amount;
	const char *otheraccount;
};

#endif /* MOVE_H */
