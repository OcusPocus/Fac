/* gui.h - 24/02/2012 */

#ifndef GUI__H
#define GUI__H


#include <gtk/gtk.h>
#include "info.h"
#include "util.h"


typedef struct {
    GtkWidget *main_box, *top_box;
} Layout;

typedef struct {
    guint magic;
    Info *info;
    Layout layout;
    GtkWidget *window, *area, *statusbar;
} Gui;

void gui_init_window    (Gui *g);
void gui_init_layout    (Gui *g);
void gui_init_top       (Gui *g);
void gui_init_area      (Gui *g);
void gui_init_statusbar (Gui *g);

void ball_next_step(Info * info);
void racquet_next_step(Racquet * rac, int area_height, int key_T, int key_B);
void gui_init (Gui *g, Info *info);
Gui *gui_check (gpointer data);

#endif /* GUI__H */

