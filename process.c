/*
 *  process.c
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
#include <time.h>
#include <math.h>
#include <windows.h>
#include <libgen.h>
#include <dirent.h>
#include "process.h"

GtkFileList *file_list_ptr = NULL;
FileItem *fi_ptr = NULL;
char *output_file = NULL;
char *process_dest_dir = NULL;

STARTUPINFO startup_info;
PROCESS_INFORMATION process_info;
SECURITY_ATTRIBUTES sec_attr;

HANDLE stdout_write_handle = NULL;
HANDLE stdout_read_handle = NULL;

int processing = 0;

// returns time in seconds
int get_current_time(char *ffmpeg_output)
{
	char *pos = strstr(ffmpeg_output, "time=");
	int t=0;
	int h=0, m=0, s=0, ms=0;
	
	if (pos != NULL)
	{
		pos += sizeof(char) * 5;
		h = (pos[0]-48) * 10 + (pos[1]-48);
		m = (pos[3]-48) * 10 + (pos[4]-48);
		s = (pos[6]-48) * 10 + (pos[7]-48);
		ms = (pos[9]-48) * 100 + (pos[10]-48) * 10;
		t = s * 1000 + m * 60000 + h * 3600000 + ms;
		return t;
	}
	else
		return -1;
}

/* 0 -> OK
 * 1 -> erreur avec *file_list
 */

int process_start(GtkFileList *file_list)
{
	DWORD var_size=0;
	char appdata_var[128];
	if (file_list)
	{
		if (!(var_size=GetEnvironmentVariable("APPDATA",appdata_var,sizeof(appdata_var))))
			return -1;
		
		if (var_size < sizeof(appdata_var))
		{
			appdata_var[var_size+1] = '\0';
		}
		else
			return -1;
		
		if (!(process_dest_dir = malloc(sizeof(char) * (strlen(appdata_var) + strlen(PROCESS_OUTPUT_DIR) + 2))))
			return -1;
		
		sprintf(process_dest_dir, "%s\\%s",appdata_var,PROCESS_OUTPUT_DIR);
		
		// Si on a une erreur à la création du dossier de sortie, deux cas possibles :
		//  - soit le dossier existe déjà (dans ce cas, tant mieux !)
		//  - soit on n'a pas réussi à le créer -> erreur
		if (!CreateDirectoryA(process_dest_dir, NULL))
			if (GetLastError() != ERROR_ALREADY_EXISTS)
				return -1;

		file_list_ptr = file_list;
		fi_ptr = file_list_get_next_file(file_list_ptr);
		process_empty_dir();
		return 0;
	}
	else
		return 1;
}

/* 0 -> conversion lancée
 * -1 -> erreur générale
 * -2 -> impossible de créer le pipe
 * -3 -> impossible de lancer ffmpeg
 */
int process_file(char *filename)
{
	char *cmd;
	int cmd_len;
	
	
	sec_attr.nLength = sizeof(SECURITY_ATTRIBUTES); 
	sec_attr.bInheritHandle = TRUE; 
	sec_attr.lpSecurityDescriptor = NULL;
	
	if (!CreatePipe(&stdout_read_handle, &stdout_write_handle, &sec_attr, 0))
		return -2;
	if (!SetHandleInformation(stdout_read_handle, HANDLE_FLAG_INHERIT, 0))
		return -2;
	
	ZeroMemory(&startup_info, sizeof(startup_info));
	startup_info.cb = sizeof(startup_info);
	startup_info.hStdError = stdout_write_handle;
	startup_info.hStdOutput = NULL;
	startup_info.hStdInput = NULL;
	startup_info.dwFlags |= STARTF_USESTDHANDLES;
	ZeroMemory(&process_info, sizeof(process_info));
	
	// Plus 8 pour les espaces et les guillemets
	cmd_len = strlen(PROCESS_CMD)+strlen(filename)+strlen(PROCESS_ARGS1)+strlen(PROCESS_ARGS2)+strlen(process_dest_dir)+strlen(basename(filename))+8;
	cmd = malloc(sizeof(char) * (cmd_len+1)); // Plus 1 pour le \0 final
	if (!cmd)
		return -1;
	
	sprintf(cmd,"%s %s \"%s\" %s \"%s%s\"", PROCESS_CMD, PROCESS_ARGS1, filename, PROCESS_ARGS2, process_dest_dir, basename(filename));
	
	output_file = malloc(sizeof(char)*(strlen(basename(filename))+strlen(process_dest_dir)+1));
	if (!output_file)
		return -1;
	
	strcpy(output_file,process_dest_dir);
	strcat(output_file,basename(filename));
	
	if (!CreateProcess(NULL, cmd, NULL, NULL, TRUE, CREATE_NO_WINDOW, NULL, NULL, &startup_info, &process_info))
	{
		free(cmd);
		return -3;
	}
	
	free(cmd);
	
	fi_ptr->status = FILE_STATUS_PROCESSING;
	fi_ptr->progress = 0;
	file_list_update(file_list_ptr, fi_ptr);
	return 0;
}

/* 0 -> conversions terminées
 * 1 -> conversions en cours
 * entre 0 et -10 -> voir process_file
 * -11 -> erreur de copie du .DBX
 * -12 -> ffmpeg a retourné une erreur.
 */

int process_run(unsigned long *ret_code)
{
	int error;
	char buffer[128];
	char *dbx_dest;
	unsigned long data_size = 0;
	float p;
	
	if (!fi_ptr)
		return 0;
	
	if (!processing)
	{
		if (fi_ptr->dbxname)
		{
			if (!(dbx_dest = malloc(sizeof(char) * (strlen(process_dest_dir) + strlen(basename(fi_ptr->dbxname)) + 1))))
				return -1;
			
			strcpy(dbx_dest, process_dest_dir);
			strcat(dbx_dest, basename(fi_ptr->dbxname));
			
			if (!CopyFile(fi_ptr->dbxname, dbx_dest, FALSE))
				return -11;
		}
		
		if ((error=process_file(fi_ptr->filename)) < 0)
		{
			processing = 0;
			return error;
		}
		processing = 1;
	}
	else
	{
		if (GetExitCodeProcess(process_info.hProcess, ret_code))
		{
			if (*ret_code != STILL_ACTIVE)
			{
				processing = 0;
				fi_ptr->progress=100;
				fi_ptr->status = FILE_STATUS_PROCESSED;
				file_list_update(file_list_ptr, fi_ptr);
				CloseHandle(stdout_read_handle);
				CloseHandle(stdout_write_handle);
				CloseHandle(process_info.hProcess);
				CloseHandle(process_info.hThread);
				fi_ptr = file_list_get_next_file(file_list_ptr);
				if (fi_ptr == NULL)
				{
					file_list_drop(file_list_ptr);
					return 0;
				}
			}
			else
			{
				if (fi_ptr->progress < 100 && ReadFile(stdout_read_handle, buffer, sizeof(buffer) - 1, &data_size, NULL))
				{
					if (data_size < sizeof(buffer)-1)
						buffer[data_size+1] = '\0';
					else
						buffer[sizeof(buffer)-1] = '\0';
					
					p = ((float) get_current_time(buffer)/ (float) fi_ptr->duration)*100;
					if (p > 0 && p <= 100)
					{
						fi_ptr->progress = round(p);
						file_list_update(file_list_ptr, fi_ptr);
					}
				}
			}
		}
		else
			return -12;
	}
	file_list_update(file_list_ptr, fi_ptr);
	return 1;
}

void process_stop(void)
{
	if (processing)
	{
		TerminateProcess(process_info.hProcess, 0);
		CloseHandle(stdout_read_handle);
		CloseHandle(stdout_write_handle);
		CloseHandle(process_info.hProcess);
		CloseHandle(process_info.hThread);
		remove(output_file);
	}
	processing = 0;
	fi_ptr = NULL;
	file_list_ptr = NULL;
	free(process_dest_dir);
}

void process_empty_dir(void)
{
	DIR *d;
	struct dirent *dir;
	char *fullpath;
	
	d = opendir(process_dest_dir);
	if (d)
	{
		while ((dir = readdir(d)) != NULL)
		{
			if (strcmp(dir->d_name, "..") != 0 && strcmp(dir->d_name, "."))
			{
				if ((fullpath = malloc(sizeof(char) * (strlen(dir->d_name) + strlen(process_dest_dir) + 2))))
				{
					strcpy(fullpath,process_dest_dir);
					strcat(fullpath,dir->d_name);
					remove(fullpath);
					free(fullpath);
				}
			}
		}
		closedir(d);
	}
}

char *process_get_dest_dir(void)
{
	return process_dest_dir;
}