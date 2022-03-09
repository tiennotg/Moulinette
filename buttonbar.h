// Protection contre les inclusions multiples
#ifndef BUTTONBAR_H
#define BUTTONBAR_H

#define BUTTONBAR_MARGIN 0
#define BUTTONBAR_PADDING 5

#define BUTTON_STOP_LABEL "Arrêter"
#define BUTTON_START_LABEL "Démarrer"
#define BUTTON_DROP_LABEL "Tout supprimer"
#define BUTTON_REMOVE_LABEL "Supprimer"

#define BUTTONBAR_USAGE_DISPLAY_URI "about:usage"
#define BUTTONBAR_USAGE_WARNING "Attention ! La moulinette n'est pas <a href=\"about:usage\">un outil magique !</a>"

typedef struct GtkButtonBar GtkButtonBar;
struct GtkButtonBar
{
	GtkWidget *hbox;
	GtkWidget *button_start;
	GtkWidget *button_drop;
	GtkWidget *button_remove;
};

GtkButtonBar* button_bar_new(GtkWindow *parent);
void button_bar_delete(GtkButtonBar *bar);

#endif