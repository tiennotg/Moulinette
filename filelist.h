/*
 *  filelist.h
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


// Protection contre les inclusions multiples
#ifndef FILE_LIST_C
#define FILE_LIST_C

#include <gtk/gtk.h>
#include "fileitem.h"

enum {
    FILE_NAME,
	FILE_TITLE,
    PROGRESS_BAR,
	FILE_STATUS,
    N_COLUMNS
};

typedef struct GtkFileList GtkFileList;
struct GtkFileList {
	GtkWidget *view;
	GtkListStore *model;
	FileItem **files;
	int i;
	int count;
};

GtkFileList* file_list_new(void);
void file_list_init_columns(GtkFileList *tree);
void file_list_add(GtkFileList *tree, FileItem *item);
FileItem *file_list_get_next_file(GtkFileList *tree);
void file_list_update(GtkFileList *tree, FileItem *item);
void file_list_remove(GtkFileList *tree, int index);
void file_list_rewind(GtkFileList *tree);
FileItem *file_list_search_dbx(GtkFileList *tree, const char *dbx_filename);
FileItem *file_list_search_audio(GtkFileList *tree, const char *audio_filename);
void file_list_drop(GtkFileList *tree);
void file_list_delete(GtkFileList *tree);

#endif