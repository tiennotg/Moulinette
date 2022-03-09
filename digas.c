/*
 *  digas.c
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
#include <glib.h>
#include <string.h>
#include <windows.h>
#include <winbase.h>
#include "digas.h"
#include "stringutils.h"

// Ne pas oublier les < et > !!
char *get_tag_content(const gchar *content, const char *tag)
{
	gchar *tag_end;
	char *pos_start;
	char *pos_end;
	gchar *tag_content=NULL;
	int tag_content_length=0;
	
	if (content == NULL || tag == NULL)
		return NULL;
	else
	{
		tag_end = malloc(sizeof(char)*(strlen(tag)+2));
		tag_end[0] = '<';
		tag_end[1] = '/';
		strcpy(tag_end+2, tag+1);
		
		pos_start = strstr(content,tag);
		pos_end = strstr(content,tag_end);
		free(tag_end);
		
		if (pos_start == NULL || pos_end == NULL)
			return NULL;
		else
		{
			pos_start += + strlen(tag);
			tag_content_length = pos_end - pos_start;
			tag_content = malloc(sizeof(char)*(tag_content_length+1));
			
			strncpy(tag_content, pos_start, tag_content_length);
			tag_content[tag_content_length] = '\0';
			
			return tag_content;
		}
	}
}

char **digas_get_data_from_file(const char *filename)
{
	char **data = NULL;
	gchar **content = malloc(sizeof(gchar));
	gsize file_size;
	gsize header_size = strlen(DIGAS_DBX_HEADER);
	
	char *filename_latin1 = g_convert(filename, -1, "latin1", "utf8", NULL, NULL, NULL);
	HANDLE f_handle = CreateFileA(filename_latin1, 0, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL);
	g_free(filename_latin1);
	
	if (f_handle == INVALID_HANDLE_VALUE)
		return NULL;
	
	int ret = GetFileSize(f_handle, NULL);
	
	CloseHandle(f_handle);
	
	if (ret < 0 || ret > DIGAS_DBX_MAX_SIZE)
		return NULL;
	
	g_file_get_contents(filename, content, &file_size, NULL);
	
	if (file_size < header_size)
		return NULL;
	else
	{
		if (strncmp(*content, DIGAS_DBX_HEADER, header_size) != 0)
		{
			return NULL;
		}
	}
	
	data = malloc(sizeof(char*)*(DIGAS_DATA_COUNT+1));
	
	data[DIGAS_DATA_TITLE] = get_tag_content(*content, "<TITLE>");
	data[DIGAS_DATA_TYPE] = get_tag_content(*content, "<CLASS>");
	data[DIGAS_DATA_AUDIO_FILE] = get_tag_content(*content, "<FILENAME>");
	data[DIGAS_DATA_DURATION] = get_tag_content(*content, "<DURATION>");
	g_free(*content);
	free(content);
	
	return data;
}