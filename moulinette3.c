/*
 *  moulinette3.c
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
#include <windows.h>
#include <unistd.h>
#include "moulinette3.h"
#include "filelist.h"
#include "buttonbar.h"
#include "fileitem.h"
#include "fileutils.h"
#include "process.h"
#include "stringutils.h"

GtkWidget *window;
GtkFileList *tree;
GtkTreeSelection *tree_selection;
GtkButtonBar *button_bar;
int process_running = 0;

enum {
	DRAG_TARGET_URI,
	DRAG_TARGET_COUNT
};

gboolean window_on_close(GtkWidget *widget, gpointer data)
{
	GtkWidget *dialog;
	gboolean return_code = FALSE;
	
	if (process_running)
	{
		dialog = gtk_message_dialog_new(GTK_WINDOW(window), GTK_DIALOG_MODAL, GTK_MESSAGE_INFO, GTK_BUTTONS_OK_CANCEL, PROCESS_RUNNING_WARNING);
		if (gtk_dialog_run(GTK_DIALOG(dialog)) == GTK_RESPONSE_CANCEL)
			return_code = TRUE;
		
		gtk_widget_destroy(dialog);
	}
	return return_code;
}

void window_on_destroy(GtkWidget *widget, gpointer data)
{
	gtk_main_quit();
}


int import_file(char *file_uri)
{
	char *filename;
	char *dbx_file=NULL;
	char *file_ext=NULL;
	FileItem *item=NULL;
	GtkWidget *dialog;
	
	if (str_starts_with("file://",file_uri))
		{
			filename = g_filename_from_uri(file_uri,NULL,NULL);
		}
		else
			filename = file_uri;
	
	if (fileutils_cmp_ext(filename,FILE_EXT_DBX) != 0) // if we have an audio file
	{
		if (file_list_search_audio(tree, filename)) // if file is already in the list, then ignore it
			return 0;
		if (!(fileutils_cmp_ext(filename,FILE_EXT_MP3) == 0 || fileutils_cmp_ext(filename,FILE_EXT_MP2) == 0 || fileutils_cmp_ext(filename,FILE_EXT_WAV) == 0))
			return 1;
		
		dbx_file = malloc(sizeof(char) * (strlen(filename) + 1 + strlen(FILE_EXT_DBX))); // In case of file ext is smaller than FILE_EXT_DBX
		if (!dbx_file)
			return 1;
		
		strcpy(dbx_file, filename);
		file_ext = strrchr(dbx_file, '.') + 1;
		strcpy(file_ext, FILE_EXT_DBX);
		
		if (access(dbx_file, F_OK) != -1) // DBX file exists, ignore the .wav file and import dbx instead
		{
			import_file(dbx_file);
			free(dbx_file);
			return 0;
		}		
		free(dbx_file);
	}
	else
	{
		if (file_list_search_dbx(tree, filename))
			return 0;
	}
	
	item = file_item_new(filename);
	
	if (item == NULL)
	{
		dialog = gtk_message_dialog_new(GTK_WINDOW(window), GTK_DIALOG_MODAL, GTK_MESSAGE_ERROR, GTK_BUTTONS_OK, ERROR_MSG_IMPORT);
		gtk_dialog_run(GTK_DIALOG(dialog));
		gtk_widget_destroy(dialog);
		return 1;
	}
	
	if (strcmp(item->type, "Project") == 0)
	{
		dialog = gtk_message_dialog_new(GTK_WINDOW(window), GTK_DIALOG_MODAL, GTK_MESSAGE_ERROR, GTK_BUTTONS_OK, ERROR_MSG_PROJECT);
		gtk_dialog_run(GTK_DIALOG(dialog));
		gtk_widget_destroy(dialog);
		file_item_delete(item);
		return 1;
	}
	
	file_list_add(tree, item);
	
	if (filename != file_uri)
		free(filename);
	
	return 0;
}

void tree_on_drag(GtkWidget     *widget,
               GdkDragContext   *context,
               gint              x,
               gint              y,
               GtkSelectionData *data,
               guint             info,
               guint             time,
               gpointer          user_data)
{
	gchar **fileURIs;
	int i=0;
	
	if (data != NULL)
	{
		fileURIs = gtk_selection_data_get_uris(data);
		if (fileURIs != NULL)
		{
			while(fileURIs[i] != NULL)
			{
				if (import_file(fileURIs[i]) == 0)
				{
					gtk_widget_set_sensitive(button_bar->button_start, TRUE);
					gtk_widget_set_sensitive(button_bar->button_drop, TRUE);
				}
				i++;
			}
			g_strfreev(fileURIs);
		}
	}
}

void toggle_buttons(void)
{
	if (!process_running)
	{
		gtk_button_set_label(GTK_BUTTON(button_bar->button_start), BUTTON_START_LABEL);
		if (tree->count > 0)
			gtk_widget_set_sensitive(button_bar->button_drop, TRUE);
		if (gtk_tree_selection_get_selected(tree_selection, NULL, NULL))
			gtk_widget_set_sensitive(button_bar->button_remove, TRUE);
		else
			gtk_widget_set_sensitive(button_bar->button_remove, FALSE);
	}
	else
	{
		gtk_widget_set_sensitive(button_bar->button_remove, FALSE);
		gtk_button_set_label(GTK_BUTTON(button_bar->button_start), BUTTON_STOP_LABEL);
		gtk_widget_set_sensitive(button_bar->button_drop, FALSE);
	}
}

void reset_buttons(void)
{
	gtk_button_set_label(GTK_BUTTON(button_bar->button_start), BUTTON_START_LABEL);
	
	if (tree->count > 0)
	{
		if (!process_running)
			gtk_widget_set_sensitive(button_bar->button_drop, TRUE);
		else
			gtk_widget_set_sensitive(button_bar->button_drop, FALSE);
		gtk_widget_set_sensitive(button_bar->button_start, TRUE);
	}
	else
	{
		gtk_widget_set_sensitive(button_bar->button_drop, FALSE);
		gtk_widget_set_sensitive(button_bar->button_start, FALSE);
	}
	
	if (gtk_tree_selection_get_selected(tree_selection, NULL, NULL) && !process_running)
		gtk_widget_set_sensitive(button_bar->button_remove, TRUE);
	else
		gtk_widget_set_sensitive(button_bar->button_remove, FALSE);
}

int process_loop(void* event)
{
	int error_code;
	GtkWidget *dialog;
	unsigned long ret_code;
	char *cmd=NULL;
	char *error_msg;
	
	error_code = process_run(&ret_code);
	if (error_code == 1)
		return TRUE;
	else if (error_code == 0)
	{
		if (ret_code != STILL_ACTIVE && process_running)
		{
			cmd = malloc(sizeof(char) * (strlen(PROCESS_END_CMD) + strlen(process_get_dest_dir()) + 1));
			if (cmd)
			{
				process_running = 0;
				strcpy(cmd, PROCESS_END_CMD);
				strcat(cmd, process_get_dest_dir());
				system(cmd);
				dialog = gtk_message_dialog_new(GTK_WINDOW(window), GTK_DIALOG_MODAL, GTK_MESSAGE_INFO, GTK_BUTTONS_NONE, PROCESS_END_MSG);
				gtk_dialog_add_buttons(GTK_DIALOG(dialog), PROCESS_END_BTN, NULL);
				gtk_dialog_run(GTK_DIALOG(dialog));
				gtk_widget_destroy(dialog);
				process_stop();
				reset_buttons();
			}
		}
		return FALSE;
	}
	else if (error_code < 0)
	{
		if ((error_msg=malloc(sizeof(char)*(strlen(PROCESS_ERROR_MSG) + strlen(PROCESS_RUN_ERROR_STR[abs(error_code)]) +1))))
		{
			strcpy(error_msg, PROCESS_ERROR_MSG);
			strcat(error_msg, PROCESS_RUN_ERROR_STR[abs(error_code)]);
			dialog = gtk_message_dialog_new(GTK_WINDOW(window), GTK_DIALOG_MODAL, GTK_MESSAGE_ERROR, GTK_BUTTONS_OK, error_msg);
			gtk_dialog_run(GTK_DIALOG(dialog));
			gtk_widget_destroy(dialog);
			free(error_msg);
		}
		process_running = 0;
		process_stop();
		reset_buttons();
		return FALSE;
	}
	else
		return FALSE;
}

void start_button_on_click(GtkButton *button, gpointer user_data)
{
	GtkWidget *dialog;
	
	if (!process_running)
	{
		process_running = 1;
		toggle_buttons();
		
		file_list_rewind(tree);
		
		if (process_start(tree) == -1)
		{
			dialog = gtk_message_dialog_new(GTK_WINDOW(window), GTK_DIALOG_MODAL, GTK_MESSAGE_ERROR, GTK_BUTTONS_OK, ERROR_MSG_FFMPEG);
			gtk_dialog_run(GTK_DIALOG(dialog));
			gtk_widget_destroy(dialog);
			process_running = 0;
			reset_buttons();
		}
		else
		{
			g_timeout_add(100,process_loop,NULL);
		}
	}
	else
	{
		process_running = 0;
		process_stop();
		toggle_buttons();
	}
}

void drop_button_on_click(GtkButton *button, gpointer user_data)
{
	if (!process_running)
	{
		file_list_drop(tree);
		reset_buttons();
	}
}

void remove_button_on_click(GtkButton *button, gpointer user_data)
{
	for (int i=0; i<tree->count; i++)
	{
		if (gtk_tree_selection_iter_is_selected(tree_selection, &(tree->files[i]->iter)))
		{
			file_list_remove(tree, i);
			break;
		}
	}
	reset_buttons();
}

void selection_on_change(GtkTreeSelection *selection, gpointer user_data)
{
	if (!process_running)
	{
		if (gtk_tree_selection_get_selected(selection, NULL, NULL))
			gtk_widget_set_sensitive(button_bar->button_remove, TRUE);
		else
			gtk_widget_set_sensitive(button_bar->button_remove, FALSE);
	}
}

int main (int argc, char *argv[])
{
	int i;
	GtkWidget *vbox;
	GtkTargetEntry drag_target_entries[] = {{"text/uri-list",0,DRAG_TARGET_URI}};
	
	gtk_init(&argc,&argv);
	
	window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	gtk_window_set_default_size(GTK_WINDOW(window), MAIN_WINDOW_WIDTH, MAIN_WINDOW_HEIGHT);
	gtk_window_set_title(GTK_WINDOW(window), MAIN_WINDOW_TITLE);
	gtk_window_set_icon_from_file(GTK_WINDOW(window), "logo.png", NULL);
	
	tree = file_list_new();
	file_list_init_columns(tree);
	
	gtk_drag_dest_set(GTK_WIDGET(tree->view), GTK_DEST_DEFAULT_ALL, drag_target_entries, DRAG_TARGET_COUNT, GDK_ACTION_COPY);
	
	button_bar = button_bar_new(GTK_WINDOW(window));
	
	reset_buttons();
	
	vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL,0);
	gtk_box_pack_start(GTK_BOX(vbox), tree->view, TRUE, TRUE, VBOX_TREE_PADDING);
	gtk_box_pack_end(GTK_BOX(vbox), button_bar->hbox, FALSE, FALSE, VBOX_BUTTONBAR_PADDING);
	
	tree_selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(tree->view));
	gtk_container_add(GTK_CONTAINER(window), vbox);
	g_signal_connect(G_OBJECT(window), "delete-event", G_CALLBACK(window_on_close), NULL);
	g_signal_connect(G_OBJECT(window), "destroy", G_CALLBACK(window_on_destroy), NULL);
	g_signal_connect(G_OBJECT(tree->view), "drag-data-received", G_CALLBACK(tree_on_drag), NULL);
	g_signal_connect(G_OBJECT(button_bar->button_start), "clicked", G_CALLBACK(start_button_on_click), NULL);
	g_signal_connect(G_OBJECT(button_bar->button_drop), "clicked", G_CALLBACK(drop_button_on_click), NULL);
	g_signal_connect(G_OBJECT(tree_selection), "changed", G_CALLBACK(selection_on_change), NULL);
	g_signal_connect(G_OBJECT(button_bar->button_remove), "clicked", G_CALLBACK(remove_button_on_click), NULL);

	gtk_widget_show_all(window);
	
	if (argc > 1)
	{
		for (i=1; i<argc; i++)
		{
			gchar *utf8_filename = g_convert(argv[i], -1, "utf-8", "latin1", NULL, NULL, NULL);
			if (import_file(utf8_filename) == 0)
			{
				gtk_widget_set_sensitive(button_bar->button_start, TRUE);
				gtk_widget_set_sensitive(button_bar->button_drop, TRUE);
			}
			g_free(utf8_filename);
		}
	}
	
	gtk_main();
	
	return EXIT_SUCCESS;
}