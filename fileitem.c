/*
 *  fileitem.c
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
#include <libgen.h>
#include <time.h>
#include <windows.h>
#include <math.h>
#include <unistd.h>
#include "fileitem.h"
#include "digas.h"

FileItem* file_item_new(const char *filename)
{
	DWORD data_size;
	char **digas_data;
	char *cmd;
	char buffer[128];
	float f_size = 0;
	STARTUPINFO startup_info;
	PROCESS_INFORMATION process_info;
	SECURITY_ATTRIBUTES sec_attr;

	HANDLE stdout_write_handle = NULL;
	HANDLE stdout_read_handle = NULL;
	
	if (filename != NULL)
	{		
		FileItem *item = malloc(sizeof(FileItem));
		if (!item)
			return NULL;
		
		digas_data = digas_get_data_from_file(filename);
		
		if (!digas_data) // If filename is not a DBX file
		{
			
			if (!(item->title = malloc(sizeof(char) * (strlen(basename(filename)) + 1))))
				return NULL;
			
			item->filename = g_convert(filename, -1, "latin1", "utf8", NULL, NULL, NULL);
			if (!item->filename)
				return NULL;
			
			item->dbxname = NULL;
			strcpy(item->title, basename(filename));
			item->type = "audio";
			
			sec_attr.nLength = sizeof(SECURITY_ATTRIBUTES); 
			sec_attr.bInheritHandle = TRUE; 
			sec_attr.lpSecurityDescriptor = NULL;
			
			if (!CreatePipe(&stdout_read_handle, &stdout_write_handle, &sec_attr, 0))
				return NULL;
			if (!SetHandleInformation(stdout_read_handle, HANDLE_FLAG_INHERIT, 0))
				return NULL;
			
			ZeroMemory(&startup_info, sizeof(startup_info));
			startup_info.cb = sizeof(startup_info);
			startup_info.hStdError = NULL;
			startup_info.hStdOutput = stdout_write_handle;
			startup_info.hStdInput = NULL;
			startup_info.dwFlags |= STARTF_USESTDHANDLES;
			ZeroMemory(&process_info, sizeof(process_info));
			
			if (!(cmd = malloc(sizeof(char)*(strlen(CMD_GET_AUDIO_DURATION_CMD) + strlen(CMD_GET_AUDIO_DURATION_ARGS) + 5 + strlen(item->filename)))))
			{
				g_free(item->filename);
				return NULL;
			}
			sprintf(cmd, "%s %s \"%s\"", CMD_GET_AUDIO_DURATION_CMD, CMD_GET_AUDIO_DURATION_ARGS, item->filename);
			
			if (!CreateProcess(NULL, cmd, NULL, NULL, TRUE, CREATE_NO_WINDOW, NULL, NULL, &startup_info, &process_info))
			{
				free(cmd);
				g_free(item->filename);
				free(item);
				return NULL;
			}
			
			ReadFile(stdout_read_handle, buffer, sizeof(buffer) - 1, &data_size, NULL);
			CloseHandle(stdout_read_handle);
			CloseHandle(stdout_write_handle);
			buffer[data_size+1] = '\0';
			f_size = atof(buffer);
			item->duration = round(f_size * 1000);
			free(cmd);
		}
		else
		{
			item->filename = digas_data[DIGAS_DATA_AUDIO_FILE];
			item->title = g_locale_to_utf8(digas_data[DIGAS_DATA_TITLE], strlen(digas_data[DIGAS_DATA_TITLE]), NULL, NULL, NULL);
			free(digas_data[DIGAS_DATA_TITLE]); // Free only this pointer, which is useless since we use the UTF-8 string returned by g_locale_to_utf8
			item->duration = atoi(digas_data[DIGAS_DATA_DURATION]);
			item->type = digas_data[DIGAS_DATA_TYPE];
			free(digas_data);
		
			if ((item->dbxname = malloc(sizeof(char) * (strlen(filename)+1))))
				strcpy(item->dbxname, filename);
		}
		
		item->progress = 0;
		item->status = FILE_STATUS_WAITING;
		return item;
	}
	else
		return NULL;
}

void file_item_delete(FileItem *item)
{
	if (item != NULL)
	{
		free(item->filename);
		free(item->title);
		free(item);
	}
}

char* file_item_status_to_string(int status)
{
	char *str=NULL;
	
	switch(status)
	{
		case FILE_STATUS_WAITING:
			str = malloc(sizeof(char)*(strlen(FILE_STATUS_STRING_WAITING)+1));
			strcpy(str,FILE_STATUS_STRING_WAITING);
		break;
		case FILE_STATUS_ERROR:
			str = malloc(sizeof(char)*(strlen(FILE_STATUS_STRING_ERROR)+1));
			strcpy(str,FILE_STATUS_STRING_ERROR);
		break;
		case FILE_STATUS_PROCESSING:
			str = malloc(sizeof(char)*(strlen(FILE_STATUS_STRING_PROCESSING)+1));
			strcpy(str,FILE_STATUS_STRING_PROCESSING);
		break;
		case FILE_STATUS_PROCESSED:
			str = malloc(sizeof(char)*(strlen(FILE_STATUS_STRING_PROCESSED)+1));
			strcpy(str,FILE_STATUS_STRING_PROCESSED);
		break;
	}
	return str;
}

