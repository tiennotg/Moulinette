/*
 *  fileutils.c
 *
 *  This file is part of Moulinette.
 *
 *  Moulinette is free software: you can redistribute it and/or modify it under the terms of
 *  the GNU General Public License as published by the Free Software Foundation, either
 *  version 3 of the License, or (at your option) any later version.
 * 
 *  Moulinette is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY;
 *  without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 *  PURPOSE. See the GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License along with Moulinette.
 *  If not, see <https://www.gnu.org/licenses/>.
 */


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include "fileutils.h"

// returns 0 if the file ext of *filename is *extention.
int fileutils_cmp_ext(const char *filename, const char *extension)
{
	const char *fileext = fileutils_get_ext(filename);
	char *extref = extension;
	
	if (extref[0] == '.')
	{
		if (strlen(extref) == 1)
		{
			extref = NULL;
		}
		else
		{
			extref++;
		}
	}
		
	if (filename == NULL)
		if (extref == NULL)
			return 0;
		else
			return 1;
	else if (extref == NULL)
		return 1;
	else
	{
		if (fileext == NULL)
			return 1;
		else
			return strcasecmp(fileutils_get_ext(filename), extref);
	}
}

const char *fileutils_get_ext(const char *filename)
{
    const char *dot = strrchr(filename, '.');
	
    if (!dot || dot == filename)
		return NULL;
	else
		return dot + 1;
}