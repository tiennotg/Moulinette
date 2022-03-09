#include <stdio.h>
#include <stdlib.h>
#include <gtk/gtk.h>
#include <string.h>
#include "buttonbar.h"
#include "usagewindow.h"

gboolean show_usage_window(GtkLabel *label, char *uri, gpointer parent)
{
	if (strcmp(BUTTONBAR_USAGE_DISPLAY_URI, uri) != 0)
		return FALSE;
	
	GtkUsageWindow *uw = usage_window_create(GTK_WINDOW(parent));
	usage_window_display(uw);
	
	return TRUE;
}

GtkButtonBar* button_bar_new(GtkWindow *parent)
{
	GtkButtonBar *bar = malloc(sizeof(GtkButtonBar));
	GtkWidget *usage_warning;
	
	usage_warning = gtk_label_new(NULL);
	gtk_label_set_markup(GTK_LABEL(usage_warning), BUTTONBAR_USAGE_WARNING);
	
	bar->hbox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, BUTTONBAR_MARGIN);
	bar->button_start = gtk_button_new_with_label(BUTTON_START_LABEL);
	bar->button_drop = gtk_button_new_with_label(BUTTON_DROP_LABEL);
	bar->button_remove = gtk_button_new_with_label(BUTTON_REMOVE_LABEL);
	gtk_box_pack_end(GTK_BOX(bar->hbox), bar->button_start, FALSE, FALSE, BUTTONBAR_PADDING);
	gtk_box_pack_end(GTK_BOX(bar->hbox), bar->button_drop, FALSE, FALSE, BUTTONBAR_PADDING);
	gtk_box_pack_end(GTK_BOX(bar->hbox), bar->button_remove, FALSE, FALSE, BUTTONBAR_PADDING);
	gtk_box_pack_start(GTK_BOX(bar->hbox), usage_warning, FALSE, FALSE, BUTTONBAR_PADDING);
	
	g_signal_connect(G_OBJECT(usage_warning), "activate-link", G_CALLBACK(show_usage_window), parent);
	
	return bar;
}

void button_bar_delete(GtkButtonBar *bar)
{
	g_object_unref(bar->button_start);
	g_object_unref(bar->button_drop);
	g_object_unref(bar->button_remove);
	g_object_unref(bar->hbox);
	free(bar);
}