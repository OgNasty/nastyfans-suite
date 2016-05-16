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

#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "error.h"

struct clean_file {
	char *filename;
	struct clean_file *next;
};

static struct clean_file *clean_list;

void error_cleanup(int error)
{
	struct clean_file *cf;

	while (clean_list) {
		cf = clean_list;
		clean_list = cf->next;

		if (error)
			unlink(cf->filename);

		free(cf->filename);
		free(cf);
	}
}

int error_add_cleanup(const char *filename)
{
	struct clean_file *cf;

	cf = calloc(1, sizeof(*cf));
	if (!cf)
		error_exit();

	cf->filename = strdup(filename);
	if (!cf->filename)
		error_exit();

	cf->next = clean_list;
	clean_list = cf;

	return 0;
}
