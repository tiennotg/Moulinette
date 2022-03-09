/*
 *  usagewindow.c
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
#include "usagewindow.h"

void usage_window_destroy(GtkUsageWindow *win)
{
	gtk_widget_destroy(win->window);
	free(win);
}

GtkUsageWindow* usage_window_create(GtkWindow *parent)
{
	GtkUsageWindow* usage_window = malloc(sizeof(GtkUsageWindow));
	
	if (!usage_window)
		return NULL;
	
	usage_window->window = gtk_dialog_new_with_buttons(USAGE_WINDOW_TITLE,parent,GTK_DIALOG_MODAL,USAGE_WINDOW_BUTTON,GTK_RESPONSE_ACCEPT, NULL);
	
	GtkWidget *vbox = gtk_dialog_get_content_area(GTK_DIALOG(usage_window->window));
	
	usage_window->label = gtk_label_new(NULL);
	gtk_label_set_markup(GTK_LABEL(usage_window->label), USAGE_WINDOW_TEXT);
	gtk_label_set_justify(GTK_LABEL(usage_window->label), GTK_JUSTIFY_CENTER);
	gtk_widget_set_margin_top(usage_window->label, USAGE_WINDOW_LABEL_MARGIN);
	gtk_widget_set_margin_start(usage_window->label, USAGE_WINDOW_LABEL_MARGIN);
	gtk_widget_set_margin_end(usage_window->label, USAGE_WINDOW_LABEL_MARGIN);
	gtk_widget_set_margin_bottom(usage_window->label, USAGE_WINDOW_LABEL_MARGIN);
	gtk_container_add(GTK_CONTAINER(vbox), usage_window->label);
	
	return usage_window;
}

void usage_window_display(GtkUsageWindow *win)
{
	gtk_widget_show_all(win->window);
	gtk_dialog_run(GTK_DIALOG(win->window));
	usage_window_destroy(win);
}