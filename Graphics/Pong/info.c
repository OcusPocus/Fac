/* info.c - 24/02/2012 */

#include "info.h"
#include "gui.h"
/*------------------------------ R A C Q U E T-------------------------------*/

void racquet_init(Racquet * rac)
{
	rac->center_rac_x 	= 100;
	rac->center_rac_y 	= 100;
	rac->radius_x		= 10;
	rac->radius_y		= 70;
}

/*------------------------------ B A L L ------------------------------------*/

void ball_init(Info * info)
{
	info->ball.x 	= 0;
	info->ball.y 	= 0;
	info->ball.dx 	= 2;
	info->ball.dy 	= 2;
	info->ball.r 	= 15;
}

/*------------------------------ I N I T ------------------------------------*/


void info_init_default (Info *info)
{
    printf ("info_init_default ...\n");

    info->dummy 		= 0;  /* a supprimer ... */
    info->anim			= 0;
    info->timeout1 		= 0;
    info->timeout2		= 0;
    info->area_width 	= 0;
    info->area_height	= 0;
    info->key_LT		= 0;
    info->key_LB		= 0;
    info->key_RT		= 0;
    info->key_RB		= 0;

    ball_init(info);
    racquet_init(&info->rac_left);
    racquet_init(&info->rac_right);
}


/*-------------------------- A N I M A T I O N ------------------------------*/

gboolean on_timeout1(gpointer data)
{
	Gui *g = gui_check (data);
    area_redraw(g->area);
	return TRUE;
}

gboolean on_timeout2(gpointer data)
{
	Gui *g = gui_check (data);
	ball_next_step(g->info);
	racquet_next_step(&g->info->rac_left,  g->info->area_height, g->info->key_LT, g->info->key_LB);
	racquet_next_step(&g->info->rac_right, g->info->area_height, g->info->key_RT, g->info->key_RB);
	return TRUE;
}

void info_anim_start_timeout1(Info * info, gpointer data)
{
	if(info->timeout1 == 0)
		info->timeout1 = g_timeout_add(TIMEOUT1, on_timeout1, data);
}

void info_anim_stop_timeout1(Info * info)
{	
	if(info->timeout1 != 0)
	{
		g_source_remove(info->timeout1);
		info->timeout1 = 0;
	}
}

void info_anim_start_timeout2(Info * info, gpointer data)
{
	if(info->timeout2 == 0)
		info->timeout2 = g_timeout_add(TIMEOUT2, on_timeout2, data);
}

void info_anim_stop_timeout2(Info * info)
{	
	if(info->timeout2 != 0)
	{
		g_source_remove(info->timeout2);
		info->timeout2 = 0;
	}
}

/*---------------------------------------------------------------------------*/
