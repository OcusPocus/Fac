#include <bx.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#define EPSILON 0.0000001
#define PI 3.14

typedef struct Coord {
	double x, y;
} Coord;

Coord Coord_FromXY (double x, double y) {
	Coord r;
	r.x = x;
	r.y = y;
	return r;
}

Coord Coord_FromMouse (bx_mouse mouse) {
	return Coord_FromXY(bx_mouse_x(mouse), 
						bx_mouse_y(mouse));
}

Coord Coord_Add (Coord p, Coord q) {
	Coord r;
	r.x = p.x + q.x;
	r.y = p.y + q.y;
	return r;
}

Coord Coord_Sub (Coord p, Coord q) {
	Coord r;
	r.x = p.x - q.x;
	r.y = p.y - q.y;
	return r;
}

double Coord_Dot (Coord p, Coord q) {
	return (p.x * q.x) 
		 + (p.y * q.y);
}

double Coord_Length2 (Coord p) {
	return Coord_Dot(p, p);
}

double Coord_Length (Coord p) {
	return sqrt(Coord_Length2(p));
}

Coord Coord_Scale (Coord p, double lambda) {
	return Coord_FromXY(p.x * lambda, 
						p.y * lambda);
}

int Double_IsZero (double value) {
	return fabs(value) < EPSILON;
}

Coord Coord_Normalize (Coord p) {
	double length = Coord_Length(p);
	return Double_IsZero(length) ? 
	Coord_FromXY(1, 0) : Coord_Scale(p, 1 / length);
}

void DrawUtil_Line (bx_window win, Coord p, Coord q) {
	bx_draw_line (win, p.x, p.y, q.x, q.y);
}

void DrawUtil_Circle (bx_window win, Coord center, double radius, int filled) {
	int diam = 2 * radius;
	bx_draw_circle(win, center.x - radius, center.y - radius, diam, diam, filled);
}

Coord Coord_Lerp (Coord p, double alpha, Coord q) {
	return Coord_Add(p,Coord_Scale(Coord_Sub(q, p), alpha));
}

Coord Coord_FromAngle(double theta) {
	return Coord_FromXY(cos(theta), sin(theta));
}

Coord Coord_FromPolar (double rho, double theta) {
	return Coord_Scale(Coord_FromAngle(theta), rho);
}

Coord Coord_ComplexMul(Coord p, Coord q) {
	double real = p.x * q.x - p.y * q.y;
	double img = p.x * q.y + p.y * q.x;
	return Coord_FromXY(real, img);
}

Coord Coord_Rotate (Coord p, double theta) {
	Coord ctheta = Coord_FromAngle(theta);
	return Coord_ComplexMul(p, ctheta);
}

Coord Coord_RotateAround (Coord p, double theta, Coord center) {
	Coord rel = Coord_Sub(p, center);
	Coord rot = Coord_Rotate(rel, theta);
	return Coord_Add(center, rot);
}

//Flocon de Koch

void KochStep (bx_window window, Coord p, Coord q) {
	Coord cutOne = Coord_Lerp(p, 1.0 / 3.0, q);
	Coord cutTwo = Coord_Lerp(p, 2.0 / 3.0, q);
	Coord corner = Coord_RotateAround(q, (2 * PI) / 3.0, cutTwo);
	DrawUtil_Line(window, cutOne, p);
	DrawUtil_Line(window, cutTwo, q);
	DrawUtil_Line(window, cutOne, corner);
	DrawUtil_Line(window, cutTwo, corner);
}

void KochLine (bx_window window, Coord p, Coord q, int level) {
	if(level == 0) DrawUtil_Line(window, p, q);
	if(level == 1) KochStep(window, p, q);
	if(level > 1){
		Coord cutOne = Coord_Lerp(p, 1.0 / 3.0, q);
		Coord cutTwo = Coord_Lerp(p, 2.0 / 3.0, q);
		Coord corner = Coord_RotateAround(q, (2 * PI) / 3.0, cutTwo);
		level--;
		KochLine(window, p, cutOne, level);
		KochLine(window, cutOne, corner, level);
		KochLine(window, corner, cutTwo, level);
		KochLine(window, cutTwo, q, level);
	}
}

void KochFlake (bx_window window, Coord center, Coord corner1, int level) {
	Coord corner2 = Coord_RotateAround(corner1, (2 * PI) / 3.0, center);
	Coord corner3 = Coord_RotateAround(corner2, (2 * PI) / 3.0, center);
	KochLine(window, corner1, corner3, level);
	KochLine(window, corner3, corner2, level);
	KochLine(window, corner2, corner1, level);
}

//Dragon de Heighway

void HeighwayDragonStep (bx_window window, Coord p, Coord q) {
	Coord base = Coord_Lerp(p, 1/2.0, q);
	Coord t = Coord_RotateAround(q, PI / 2, base);
	DrawUtil_Line(window, p, t);
	DrawUtil_Line(window, t, q);
}

void HeighwayDragon(bx_window window, Coord p , Coord q, int level) {
	if(level == 0) DrawUtil_Line(window, p, q);
	if(level == 1) HeighwayDragonStep(window, p, q);
	if(level > 1){
		Coord base = Coord_Lerp(p, 1/2.0, q);
		Coord t = Coord_RotateAround(q, PI / 2, base);
		Coord cut = Coord_Lerp(q, 1/2.0, t);
		Coord corner = Coord_RotateAround(t, -PI / 2, cut);
		level--;
		HeighwayDragon(window, base, p, level);
		HeighwayDragon(window, base, t, level);
		HeighwayDragon(window, corner, t, level);
		HeighwayDragon(window, corner, q, level);
	}
}

int main (void) {
	bx_window win;
	char win_title[] = "Hue hue hue";
	Coord win_pos = Coord_FromXY(50, 50);
	Coord canvas_dim = Coord_FromXY(600, 400);
	bx_init();
	win = bx_create_window (win_title, win_pos.x, win_pos.y, canvas_dim.x, canvas_dim.y);
	int millisecondes_delay = 10;
	for(;;){
		Coord center = Coord_Scale(canvas_dim, 0.5);
		Coord pointer = Coord_FromMouse(bx_read_mouse(win));
		bx_clear_canvas(win, bx_white());
		KochFlake(win, center, pointer, 5);
		HeighwayDragon(win, center, pointer, 5);
		bx_show_canvas(win, millisecondes_delay);
	}
	bx_loop();
	return 0;
}
