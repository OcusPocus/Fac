/* info.h - 24/02/2012 */

#ifndef INFO__H
#define INFO__H

#include <stdio.h>
#include <stdlib.h>
#include <gtk/gtk.h>

#define TIMEOUT1 30
#define TIMEOUT2 10

typedef struct Racquet
{
    int center_rac_x, center_rac_y;
    int radius_x, radius_y;
}Racquet;

typedef struct Ball
{
	int x, y;
	int dx, dy;
	int r;
}Ball;

typedef struct {
    int dummy;  /* a supprimer ... */
    int anim;
    int area_width;
    int area_height;
    guint timeout1;
    guint timeout2;
	Ball ball;    
    Racquet rac_left, rac_right;
    int key_LT, key_LB, key_RT, key_RB;
} Info;

void info_init_default (Info *info);
void info_anim_start_timeout1(Info * info, gpointer data);
void info_anim_stop_timeout1(Info * info);
void info_anim_start_timeout2(Info * info, gpointer data);
void info_anim_stop_timeout2(Info * info);
#endif /* INFO__H */

