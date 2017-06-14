/* gui.c - 24/02/2012 */

#include "gui.h"
#include <gdk/gdkkeysyms.h>  /* pour keyvals tq GDK_q */


/*------------------------------ I N I T ------------------------------------*/

#define GUI_MAGIC 0x46EA7E05

void gui_init (Gui *g, Info *info)
{
    printf ("gui_init ...\n");
    g->magic = GUI_MAGIC;
    g->info = info;
    gui_init_window (g);
    gui_init_layout (g);
    gui_init_top    (g);
    gui_init_area   (g);
    gui_init_statusbar (g);
}


/* Cette fonction permet de tester si le data que l'on a recupere' dans
 * un handler de signaux contient bien un gui ; sinon, cela veut dire que :
 *  - soit on a oublie' de transmettre gui dans g_signal_connect,
 *  - soit on s'est trompe' dans le nombre de parametres du handler de signaux.
*/

Gui *gui_check (gpointer data)
{
    if (data == NULL) { 
        fprintf (stderr, "gui_check: NULL data\n"); return NULL; 
    }
    if (((Gui *)data)->magic != GUI_MAGIC) { 
        fprintf (stderr, "gui_check: bad magic number\n"); return NULL; 
    }
    return data;
}


/*---------------------------- W I N D O W S --------------------------------*/

#define WIN1_TITLE  "Five finger death punch"
#define WIN1_W   500
#define WIN1_H   450


void on_window_destroy (GtkWidget U *widget, gpointer U data)
{
    /* Gui *g = gui_check (data); */

    printf ("on_window_destroy\n");
    gtk_main_quit ();
}

gboolean on_window_delete_event (GtkWidget U *widget, GdkEvent U *event, 
    gpointer U data)
{
    /* Gui *g = gui_check (data); */

    printf ("on_window_delete_event\n");
    return FALSE;
}


void gui_init_window (Gui *g)
{
    g->window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title (GTK_WINDOW (g->window), WIN1_TITLE);
    gtk_window_set_default_size (GTK_WINDOW (g->window), WIN1_W, WIN1_H);

    ON_SIGNAL (g->window, "delete-event", on_window_delete_event, g);
    ON_SIGNAL (g->window, "destroy",      on_window_destroy,      g);
}


/*----------------------------- L A Y O U T ---------------------------------*/


void gui_init_layout (Gui *g)
{
    Layout *l = &g->layout;

    l->main_box = gtk_vbox_new (!HOMOGENEOUS, 0);
    l->top_box  = gtk_hbox_new (!HOMOGENEOUS, 0);

    gtk_container_add (GTK_CONTAINER (g->window), l->main_box);
    gtk_box_pack_start (GTK_BOX (l->main_box), l->top_box, !EXPAND, !FILL, 0);
}


/*-------------------------------- T O P ------------------------------------*/

void on_start_clicked(GtkButton U *button, gpointer U data)
{
    Gui     * g     = gui_check (data);
    Info    * info  = g->info;
    info->anim      = !info->anim;

    if(info->anim == 1)
    {
        gtk_button_set_label(button, "Stop");
        info_anim_start_timeout1(info, data);
        info_anim_start_timeout2(info, data);
    }
    else
    {
        gtk_button_set_label(button, "Start");
        gtk_widget_grab_focus(g->area);

        info_anim_stop_timeout1(info);
        info_anim_stop_timeout2(info);
    }
}

void on_quit_clicked (GtkButton U *widget, gpointer U data)
{
    /* Gui *g = gui_check (data); */

    gtk_main_quit ();
}

void gui_init_top (Gui *g)
{
    Layout *l = &g->layout;
    GtkWidget *tmp_quit;
    GtkWidget * tmp_start;

    tmp_quit    = gtk_button_new_with_label ("Quit");
    tmp_start   = gtk_button_new_with_label ("Start");
    gtk_box_pack_start (GTK_BOX (l->top_box), tmp_quit, !EXPAND, !FILL, 0);
    gtk_box_pack_start (GTK_BOX (l->top_box), tmp_start, !EXPAND, !FILL, 10);

    ON_SIGNAL (tmp_quit, "clicked", on_quit_clicked, g);
    ON_SIGNAL (tmp_start, "clicked", on_start_clicked, g);
}


/*------------------------ D R A W I N G   A R E A --------------------------*/

/*------------------------------ B A L L ------------------------------------*/

void ball_draw(GdkWindow * win, GdkGC * gc , Info * info)
{
    gc_set_rgb_fg(gc, 0xFF0000);
    gdk_draw_arc(win, gc, TRUE, info->ball.x, info->ball.y, 30, 30, 64, 64*360);

}
void ball_next_step(Info * info)
{
    Ball * b = &info->ball;

    b->x = b->x + b->dx;
    b->y = b->y + b->dy;

    if(b->x + b->r > info->area_width)
    {
        b->x  = info->area_width - b->r;
        b->dx = b->dx * -1;
    }

    if(b->x < 0)
    {
        b->x  = b->r;
        b->dx = b->dx * -1;
    }

    if(b->y + b->r > info->area_height)
    {
        b->y  = info->area_height - b->r;
        b->dy = b->dy * -1;
    }

    if(b->y < 0)
    {
        b->y  = b->r;
        b->dy = b->dy * -1;
    }
}
/*------------------------------ R A C Q U E T ------------------------------------*/

void racquet_draw(GdkWindow * win, GdkGC * gc, Racquet * rac)
{
    gc_set_rgb_fg(gc, 0x67ADCC);
    gdk_draw_rectangle(win, gc, FALSE, rac->center_rac_x, rac->center_rac_y, rac->radius_x, rac->radius_y);
}

void racquet_adapt_left(Racquet * rac, int width, int height)
{
    (void)width;
    rac->center_rac_x = rac->radius_x;
    rac->center_rac_y = height / 2 - rac->radius_y;

    if(rac->center_rac_y < 0)
    {   
        rac->center_rac_y = 0 + rac->radius_y;
    }

    if (rac->center_rac_y > height)
    {
        rac->center_rac_y = height - rac->radius_y;
    }

}

void racquet_adapt_right(Racquet * rac, int width, int height)
{
    rac->center_rac_x = width - 2 * rac->radius_x;
    rac->center_rac_y = height / 2 - rac->radius_y;

    if(rac->center_rac_y < 0)
    {
        rac->center_rac_y = rac->radius_y;
    }

    if(rac->center_rac_y > height)
    {
        rac->center_rac_y = height - rac->radius_y;
    }
}


void racquet_next_step(Racquet * rac, int area_height, int key_T, int key_B)
{
    if(key_T == 1 && key_B == 1)    printf("les deux boutons sont pressés\n");;
    if(key_T == 0 && key_B == 0)    printf("0 bouton pressé\n");

    if(key_T == 1)
    {
        rac->center_rac_y = rac->center_rac_y - 4;
        if(rac->center_rac_y < 0)
            rac->center_rac_y = rac->radius_y;
    }

    if(key_B == 1)
    {
        rac->center_rac_y = rac->center_rac_y + 4;
        if(rac->center_rac_y + rac->radius_y > area_height)
            rac->center_rac_y = rac->center_rac_y - rac->radius_y;
    }

}

/*---------------------------------------------------------------------------*/

gboolean on_area_expose (GtkWidget *widget, GdkEventExpose U *e, gpointer U data)
{
    GdkGC *gc= gdk_gc_new (widget->window);
    int width, height;
    Gui *g = data;

    printf ("on_area_expose\n");
    gc_set_rgb_fg (gc, 0xFFFFEE);
    gdk_drawable_get_size (widget->window, &width, &height);
    gdk_draw_rectangle (widget->window, gc, TRUE, 0, 0, width, height);
    ball_draw(widget->window, gc, g->info);
    racquet_draw(widget->window, gc, &g->info->rac_left);
    racquet_draw(widget->window, gc, &g->info->rac_right);

    g_object_unref (gc);
    return TRUE;  /* L'evenement a ete traite' */
}


gboolean on_area_but_press (GtkWidget U *w, GdkEventButton *e, gpointer data)   
{
    Gui *g = gui_check (data);
    gtk_widget_grab_focus(g->area);

    printf ("on_area_but_press  x = %d  y = %d  b = %d\n", 
           (int) e->x, (int) e->y, e->button);
    return TRUE;  /* L'evenement a ete traite' */
}


gboolean on_area_but_release (GtkWidget U *w, GdkEventButton *e, gpointer U d)   
{
    /* Gui *g = gui_check (data); */

    printf ("on_area_but_release  x = %d  y = %d  b = %d\n", 
           (int) e->x, (int) e->y, e->button);

    return TRUE;  /* L'evenement a ete traite' */
}


gboolean on_area_motion (GtkWidget U *w, GdkEventMotion *e, gpointer U d)
{
    /* Gui *g = gui_check (data); */

    printf ("on_area_motion  x = %d  y = %d\n", 
           (int) e->x, (int) e->y);

    return TRUE;  /* L'evenement a ete traite' */
}


gboolean on_area_key_press (GtkWidget U *w, GdkEventKey *e, gpointer d)
{
    Gui *g = gui_check (d);

    /* ev->keyval contient le symbole de la touche pressee */
    switch (e->keyval) {
        case GDK_Escape : gtk_main_quit ();     break;
        case GDK_a      : g->info->key_LT = 1;  break;
        case GDK_q      : g->info->key_LB = 1;  break;
        case GDK_p      : g->info->key_RT = 1;  break;
        case GDK_l      : g->info->key_RB = 1;  break;
        default :  printf ("on_area_key_press    \"%s\"  length = %d  keyval = GDK_%s\n",
            e->string, e->length, gdk_keyval_name(e->keyval)); break;
    }

    return TRUE;  /* L'evenement a ete traite' */
}


gboolean on_area_key_release (GtkWidget U *w, GdkEventKey U *e, gpointer U d)
{
    Gui *g = gui_check (d);

    /* ev->keyval contient le symbole de la touche relachee */
    switch (e->keyval) {
        case GDK_a      : g->info->key_LT = 0;  break;
        case GDK_q      : g->info->key_LB = 0;  break;
        case GDK_p      : g->info->key_RT = 0;  break;
        case GDK_l      : g->info->key_RB = 0;  break; 
        default :  printf ("on_area_key_release  \"%s\"  length = %d  keyval = GDK_%s\n",
            e->string, e->length, gdk_keyval_name(e->keyval)); break;
    }

    return TRUE;  /* L'evenement a ete traite' */
}


gboolean on_area_configure(GtkWidget * area, GdkEventConfigure * e, gpointer data)
{    
    (void)area;

    Gui * g = gui_check(data);
    Info * info = g->info;

    printf("(%d), (%d) \n", (int)e->width, (int)e->height);

    info->area_width    = (int)e->width;
    info->area_height   = (int)e->height;
    racquet_adapt_right(&info->rac_right, info->area_width, info->area_height);
    racquet_adapt_left(&info->rac_left, info->area_width, info->area_height);

    return TRUE;
}

void gui_init_area (Gui *g)
{
    Layout *l = &g->layout;

    g->area = gtk_drawing_area_new ();
    gtk_box_pack_start (GTK_BOX (l->main_box), g->area, EXPAND, FILL, 3);

    GTK_WIDGET_SET_FLAGS  (g->area, GTK_CAN_FOCUS);
    gtk_widget_add_events (g->area, 
        GDK_BUTTON_MOTION_MASK  |
        GDK_BUTTON_PRESS_MASK   |
        GDK_BUTTON_RELEASE_MASK |
        GDK_KEY_PRESS_MASK      |
        GDK_KEY_RELEASE_MASK    |
        GDK_EXPOSURE_MASK       );

    ON_SIGNAL (g->area, "expose-event",         on_area_expose,      g);
    ON_SIGNAL (g->area, "button-press-event",   on_area_but_press,   g);
    ON_SIGNAL (g->area, "button-release-event", on_area_but_release, g);
    ON_SIGNAL (g->area, "motion-notify-event",  on_area_motion,      g);
    ON_SIGNAL (g->area, "key-press-event",      on_area_key_press,   g);
    ON_SIGNAL (g->area, "key-release-event",    on_area_key_release, g);
    ON_SIGNAL (g->area, "configure_event",      on_area_configure,   g);
}


/*--------------------------- S T A T U S B A R -----------------------------*/


void gui_init_statusbar (Gui *g)
{
    Layout *l = &g->layout;
 
    g->statusbar = gtk_statusbar_new ();
    statusbar_put (GTK_STATUSBAR (g->statusbar), "Vive les Pépitos !");
    gtk_box_pack_start (GTK_BOX (l->main_box), g->statusbar, !EXPAND, !FILL, 0);
}


/*---------------------------------------------------------------------------*/

