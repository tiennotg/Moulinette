/*
 *  digas.h
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

#ifndef DIGAS_H
#define DIGAS_H

enum {
	DIGAS_DATA_TITLE,
	DIGAS_DATA_TYPE,
	DIGAS_DATA_AUDIO_FILE,
	DIGAS_DATA_DURATION,
	DIGAS_DATA_COUNT
};

#define DIGAS_DBX_HEADER "<?xml version=\"1.0\""
#define DIGAS_DBX_MAX_SIZE 1000000

char **digas_get_data_from_file(const char *filename);

#endif