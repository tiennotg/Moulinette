/*
 *  filelist.c
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
#include <gtk/gtk.h>
#include <string.h>
#include "filelist.h"

GtkFileList* file_list_new(void)
{
	GtkFileList *file_list = malloc(sizeof(GtkFileList));
	
	file_list->model = gtk_list_store_new(N_COLUMNS,
                               G_TYPE_STRING,   /* FILE_NAME */
							   G_TYPE_STRING,	/* FILE_TITLE */
                               G_TYPE_UINT,     /* PROGRESS_BAR */
                               G_TYPE_STRING    /* FILE_STATUS */
                              );
	file_list->view = gtk_tree_view_new_with_model(GTK_TREE_MODEL(file_list->model));
	file_list->i = -1;
	file_list->count = 0;
	file_list->files = NULL;
	return file_list;
}

void file_list_init_columns(GtkFileList *tree)
{
	GtkTreeViewColumn *column;
	
	column = gtk_tree_view_column_new_with_attributes("Nom",
                                                      gtk_cell_renderer_text_new(),
                                                      "text", FILE_TITLE,
                                                      NULL);
    gtk_tree_view_append_column(GTK_TREE_VIEW(tree->view), column);

    column = gtk_tree_view_column_new_with_attributes("Conversion",
                                                      gtk_cell_renderer_progress_new(),
                                                      "value", PROGRESS_BAR,
                                                      NULL);
    gtk_tree_view_append_column(GTK_TREE_VIEW(tree->view), column);

    column = gtk_tree_view_column_new_with_attributes("Statut",
                                                      gtk_cell_renderer_text_new(),
                                                      "text", FILE_STATUS,
                                                      NULL);
    gtk_tree_view_append_column(GTK_TREE_VIEW(tree->view), column);
}

void file_list_add(GtkFileList *tree, FileItem *item)
{
	char *status_str;
	if (item != NULL)
	{
		status_str = file_item_status_to_string(item->status);
		gtk_list_store_insert_with_values(tree->model, &(item->iter), -1,
										  FILE_NAME, item->filename,
										  FILE_TITLE, item->title,
										  PROGRESS_BAR, item->progress,
										  FILE_STATUS, status_str,
										  -1);
		tree->count++;
		if (tree->files != NULL)
		{
			tree->files = realloc(tree->files, sizeof(FileItem*)*tree->count);
		}
		else
		{
			tree->files = malloc(sizeof(FileItem*));
		}
		
		tree->files[tree->count-1] = item;
		if (tree->i < 0)
			tree->i = 0;
		free(status_str);
	}
}

FileItem *file_list_get_next_file(GtkFileList *tree)
{
	if (tree->i < 0 || tree->i >= tree->count)
		return NULL;
	else
	{
		tree->i++;
		return tree->files[tree->i-1];
	}
}

void file_list_rewind(GtkFileList *tree)
{
	tree->i = 0;
}

void file_list_remove(GtkFileList *tree, int index)
{
	int j;
	if (index >= 0 && index < tree->count)
	{
		file_item_delete(tree->files[index]);
		gtk_list_store_remove(tree->model, &tree->files[index]->iter);
		for (j=index; j < tree->count-1; j++)
			tree->files[j] = tree->files[j+1];
		tree->count--;
		if (tree->count == 0)
			tree->i = -1;
	}
}

void file_list_drop(GtkFileList *tree)
{
	while(tree->count > 0)
		file_list_remove(tree, 0);
}

void file_list_delete(GtkFileList *tree)
{
	int i;
	for (i=0; i<tree->count; i++)
	{
		file_item_delete(tree->files[i]);
	}
	free(tree->files);
	g_object_unref(tree->view);
	g_object_unref(tree->model);
	free(tree);
}

void file_list_update(GtkFileList *tree, FileItem *item)
{
	char *status_str;
	if (tree && item)
	{
		status_str = file_item_status_to_string(item->status);
		gtk_list_store_set(tree->model, &item->iter,
			FILE_NAME, item->filename,
			FILE_TITLE, item->title,
			FILE_STATUS, status_str,
			PROGRESS_BAR, item->progress,
			-1);
		free(status_str);
	}
}

FileItem *file_list_search_dbx(GtkFileList *tree, const char *dbx_filename)
{
	int i;
	FileItem *item = NULL;
	for (i=0; i<tree->count; i++)
	{
		if (tree->files[i]->dbxname)
			if (strcmp(tree->files[i]->dbxname, dbx_filename) == 0)
				item = tree->files[i];
	}
	return item;
}

FileItem *file_list_search_audio(GtkFileList *tree, const char *audio_filename)
{
	int i;
	FileItem *item = NULL;
	for (i=0; i<tree->count; i++)
	{
		if (strcmp(tree->files[i]->filename, audio_filename) == 0)
			item = tree->files[i];
	}
	return item;
}