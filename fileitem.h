/*
 *  fileitem.h
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
#ifndef FILEITEM_H
#define FILEITEM_H

#include <gtk/gtk.h>

enum {
	FILE_STATUS_PROCESSED,
	FILE_STATUS_WAITING,
	FILE_STATUS_PROCESSING,
	FILE_STATUS_ERROR
};

#define FILE_STATUS_STRING_PROCESSED "Terminé"
#define FILE_STATUS_STRING_WAITING "En attente"
#define FILE_STATUS_STRING_PROCESSING "En cours"
#define FILE_STATUS_STRING_ERROR "Erreur"

#define FILE_ITEM_ID_SIZE 12

#define CMD_GET_AUDIO_DURATION_CMD "bin\\ffprobe.exe"
#define CMD_GET_AUDIO_DURATION_ARGS "-show_format -show_entries format=duration -of compact=p=0:nk=1 -v 0"

typedef struct FileItem FileItem;
struct FileItem {
	char *filename;
	char *title;
	char *type;
	char *dbxname;
	GtkTreeIter iter;
	int status;
	int progress;
	int duration;
};

FileItem* file_item_new(const char *filename);
char* file_item_status_to_string(int status);
void file_item_delete(FileItem *item);

#endif