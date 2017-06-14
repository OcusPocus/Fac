#include <stdio.h>
#include <stdlib.h>
#include <bx.h>
#define MAX_JOINTS 50

typedef struct Coord{
	double x,y;
}Coord;

typedef struct Joint{
	Coord center;
	double radius;
}Joint;

typedef struct Snake{
	Joint joints[MAX_JOINTS];
	int nb_joints;
}Snake;

Coord Coord_FromXY(double x, double y){
	Coord p;
	p.x = x;
	p.y = y;
	return p;
}

Coord Coord_Add(Coord p, Coord q){
	Coord sum;
	sum.x = p.x + q.x;
	sum.y = p.y + q.y;
	return sum;
}

Coord Coord_Sub(Coord p, Coord q){
	Coord diff;
	diff.x = p.x - q.x;
	diff.y = p.y - q.y;
	return diff;
}

Coord Coord_Vector (Coord start, Coord end)
{
	return Coord_Sub (end, start);
}

double Coord_Dot(Coord p, Coord q){
	return p.x * q.x 
		+  p.y * q.y;
}

double Coord_Length2(Coord p){
	return Coord_Dot (p, p);
}

double Coord_Length(Coord p){
	return sqrt(Coord_Length2(p));
}

Coord Coord_Scale(Coord p, double lambda){
	return Coord_FromXY(p.x * lambda, p.y * lambda);
}

#define EPSILON 0.0000001
int Double_IsZero(double value){
	return fabs(value) < EPSILON;
}
Coord Coord_Normalize(Coord p){
	double length = Coord_Length(p);
	return Double_IsZero(length) ? 
        Coord_FromXY(1, 0) :
   	    Coord_Scale(p, 1/length);
}

Coord Coord_Resize (Coord p, double length)
{
	Coord normalized= Coord_Normalize(p);
	return Coord_Scale (normalized, length);
}



Coord Coord_MoveAtDistance(Coord p, double distance, Coord q){
	Coord qp = Coord_Vector (q, p);
	Coord resized_qp = Coord_Resize(qp, distance);
	return Coord_Add (q, resized_qp);
}

Coord Coord_FromMouse(bx_mouse mouse){
	return Coord_FromXY(bx_mouse_x(mouse), bx_mouse_y(mouse));
}

void DrawUtil_Line(bx_window win, Coord p ,Coord q){
	bx_draw_line(win, p.x, p.y, q.x, q.y);
}

void DrawUtil_Box(bx_window win ,Coord center,double radius, int filled){
	int diam = 2*radius;
	bx_draw_box(win, center.x-radius, center.y-radius, diam, diam, filled);
}

void DrawUtil_Circle(bx_window win, Coord center, double radius, int filled){
	int diam = 2*radius;
	bx_draw_circle(win, center.x-radius, center.y-radius, diam, diam, filled);
}

double Double_Lerp(double x0, double alpha, double x1){
	return x0+(alpha*(x1-x0));
}

void Joint_Init(Joint*j,Coord center,double radius){
	j->center.x = center.x;
	j->center.y = center.y;
	j->radius   = radius;
}

void Snake_Init(Snake*s,int nb_joints,Coord head_center,double head_radius,double tail_radius){
	int k = 0;	
	s->nb_joints = nb_joints;
	Joint_Init(&s->joints[0], head_center, head_radius);

	for(k = 1; k<nb_joints ;k++){
		double lambda = k/(double)(nb_joints-1); 
		double radius = Double_Lerp(head_radius, lambda, tail_radius);

		Joint * prev = &s->joints[k-1];
		Coord shift = Coord_FromXY(prev->radius + radius, 0);
		Coord center = Coord_Add(prev->center, shift);
		Joint_Init(&s->joints[k], center, radius);
	}
}

void Joint_Draw(Joint const*j, bx_window window){
	bx_set_color(bx_red());   DrawUtil_Circle(window ,j->center ,j->radius,1);
	bx_set_color(bx_blue());  DrawUtil_Circle(window ,j->center ,j->radius,0);
}

void Snake_Draw(Snake const *s, bx_window window){
	int k;	
	for(k = 0;k < s->nb_joints ; k++){
		Joint_Draw(&s->joints[k],window);
	}	
}

double Snake_IdealDistanceFrompreviousJoint(Snake const*s,int joint_index){
	int pre_index = joint_index-1;
	return s->joints[pre_index].radius  +  s->joints[joint_index].radius;
}

void Snake_MoveHeadTo(Snake*s,Coord head_center){
	int k;
	double distance;	
	s->joints[0].center = head_center;

	for(k=1;k<s->nb_joints;k++){
		distance = Snake_IdealDistanceFrompreviousJoint(s,k);
		s->joints[k].center = Coord_MoveAtDistance(s->joints[k].center , distance , s->joints[k-1].center );
	}
}

int main(void){
	bx_window win;
	char win_title []="Serpentin POWAAAAA";
	Coord win_pos  =  Coord_FromXY(50,50);
	Coord canvas_dim  =  Coord_FromXY(600,400);
	Snake snake, twin;
	bx_init();
	win = bx_create_window(win_title, win_pos.x, win_pos.y, canvas_dim.x, canvas_dim.y);
	/*for(;;){

		Coord center = Coord_Scale(canvas_dim,0.5);
		Coord pointer = Coord_FromMouse(bx_read_mouse(win));
		double distance = 50;
		Coord moon = Coord_MoveAtDistance(pointer,distance,center);
			
		bx_clear_canvas(win, bx_white());
		bx_set_color(bx_black()); DrawUtil_Line (win,center,pointer);
		bx_set_color(bx_red());	  DrawUtil_Box(win,center,5,1);
		bx_set_color(bx_blue());  DrawUtil_Circle(win,pointer,5,1);
		
		bx_set_color(bx_green()); DrawUtil_Circle(win,center,distance,0);
		bx_set_color(bx_orange());DrawUtil_Circle(win,moon,5,0);
	*/
	Snake_Init(&snake, 10, Coord_FromXY (50, 320), 20, 5);
	Snake_Init(&twin , 10, Coord_FromXY (50, 320), 20, 5);
	for(;;){
		Coord pointer = Coord_FromMouse(bx_read_mouse(win));
		Snake_MoveHeadTo(&snake, pointer);
		Snake_MoveHeadTo(&twin, Coord_FromXY (pointer.y, pointer.x));
		bx_clear_canvas(win,bx_white());
		Snake_Draw(&snake, win);
		Snake_Draw(&twin, win);
		bx_show_canvas(win,10);
	}
	bx_loop();
	return 0;
}

