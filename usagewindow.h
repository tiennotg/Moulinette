/*
 *  usagewindow.h
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

#ifndef USAGE_WINDOW_H
#define USAGE_WINDOW_H

struct GtkUsageWindow {
	GtkWidget *window, *label;
};

typedef struct GtkUsageWindow GtkUsageWindow;

#define USAGE_WINDOW_TITLE "Du bon usage de la moulinette..."
#define USAGE_WINDOW_BUTTON "J’ai _compris !"
#define USAGE_WINDOW_TEXT "<big>La moulinette est un outil conçu avant tout\npour le traitement des <b>voix brutes</b>.\n\n\
Le résultat risque d’être <b>décevant</b>\navec des <b>émissions mixées\nou des musiques</b> !\n\nNotez aussi qu’un son trop bas\n\
risque de générer du souffle.\n\n<b>Bonne utilisation !</b></big>"

#define USAGE_WINDOW_LABEL_MARGIN 10
#define USAGE_WINDOW_PADDING 10

GtkUsageWindow* usage_window_create(GtkWindow *parent);
void usage_window_display(GtkUsageWindow *win);

#endif