#include <bx.h>
#define PI 3.14
//  1 Rotation et interpolation linéaire
typedef struct Coord{
	double x,y;
}Coord;

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

Coord Coord_Lerp(Coord p, double alpha, Coord q){
	Coord vector_PQ      = Coord_Vector( p, q);
	Coord vector_AlphaPQ = Coord_Scale( vector_PQ, alpha);
	Coord lerp           = Coord_Add( p , vector_AlphaPQ);
	return lerp;
}

void DrawUtil_Line(bx_window win, Coord p ,Coord q){
	bx_draw_line(win, p.x, p.y, q.x, q.y);
}

void DrawUtil_Circle(bx_window win, Coord center, double radius, int filled){
	int diam = 2*radius;
	bx_draw_circle(win, center.x - radius, center.y - radius, diam, diam, filled);
}

Coord Coord_FromAngle( double theta ) {
	return Coord_FromXY( cos(theta), sin(theta) );
}

Coord Coord_FromPolar( double rho, double theta ){
	Coord angle = Coord_FromAngle(theta);
	return Coord_Scale( angle, rho);
}

Coord Coord_ComplexMul( Coord p, Coord q){
	double abscisse =  p.x * q.x  -  p.y * q.y ;
	double ordonnee =  p.x * q.y  +  p.y * q.x ;
	return Coord_FromXY( abscisse, ordonnee);
}

Coord Coord_Rotate ( Coord p , double theta){
	Coord rot_vec = Coord_FromAngle(theta);
	return Coord_ComplexMul(p, rot_vec);
}

Coord Coord_RotateAround( Coord p , double theta, Coord center){
	Coord shifted = Coord_Sub (p, center);
	Coord rot_shifted = Coord_Rotate ( shifted, theta);
	return Coord_Add ( rot_shifted, center);
}
Coord Coord_FromMouse(bx_mouse mouse){
	return Coord_FromXY(bx_mouse_x(mouse), bx_mouse_y(mouse));
}

// 2 Flocon de koch


void KochStep (bx_window window, Coord p, Coord q){
	Coord point_at_one_third = Coord_Lerp (p, 1.0/3.0, q);
	Coord point_at_two_third = Coord_Lerp (p, 2.0/3.0, q);
	Coord top_triangle 		 = Coord_RotateAround(q, 2.0*PI/3.0 , point_at_two_third);

	bx_set_color(bx_blue());	DrawUtil_Line(window, p, point_at_one_third);
								DrawUtil_Line(window, point_at_one_third, top_triangle);
								DrawUtil_Line(window, top_triangle, point_at_two_third);
								DrawUtil_Line(window, point_at_two_third, q);
}


void KochLine(bx_window window, Coord p, Coord q, int level){	
	if( level == 0 ){
		DrawUtil_Line(window, p, q);
		return;
	}

	Coord point_at_one_third = Coord_Lerp (p, 1.0 / 3.0, q);
	Coord point_at_two_third = Coord_Lerp (p, 2.0 / 3.0, q);
	Coord top_triangle 		 = Coord_RotateAround (q, 2.0 * PI/3.0, point_at_two_third);

	// on part de level vers level = 0
	level--;
	
	KochLine(window, p, point_at_one_third, level);
	KochLine(window, point_at_one_third, top_triangle, level);
	KochLine(window, top_triangle, point_at_two_third, level);
	KochLine(window, point_at_two_third, q, level);
}

void KochFlake(bx_window window, Coord center, Coord corner1, int level){
	// le center et un coin il faut les 2 autres	
	Coord second_corner = Coord_RotateAround(corner1, 2 * PI/3.0, center);
	Coord third_corner	= Coord_RotateAround(second_corner, 2 * PI/3.0, center);

	KochLine(window, corner1, third_corner, level);
	KochLine(window, third_corner, second_corner,level);
	KochLine(window, second_corner, corner1,level);
	
}
	

// ==========================================================================================================
// ==========================================================================================================
//	
//		Dragon de Heighway
//
// ==========================================================================================================
// ==========================================================================================================


void HeighwayDragonStep(bx_window window, Coord p, Coord q){
	Coord point_at_mid = Coord_Lerp(q, 1/2, p);
	Coord mid_to_point = Coord_RotateAround(p, PI/2.0, point_at_mid);
	bx_set_color(bx_blue());	DrawUtil_Line(window, p, point_at_mid);
								DrawUtil_Line(window, point_at_mid, mid_to_point);
								DrawUtil_Line(window, q, p);

}

void HeighwayDragon(bx_window window, Coord p, Coord q, int level){
	if(level == 0)	DrawUtil_Line(window , p, q);
	if(level == 1)	HeighwayDragonStep(window, p, q);
	if(level > 1){
		Coord point_at_mid = Coord_Lerp(p, 1/2, q);
		Coord mid_to_point = Coord_RotateAround(q, PI/2.0, point_at_mid);
		Coord point_rotated2 = Coord_RotateAround(point_at_mid, PI/2.0, mid_to_point);

		level--;
		HeighwayDragon(window, p, point_at_mid, level);
		HeighwayDragon(window, point_at_mid, mid_to_point, level);
		HeighwayDragon(window, mid_to_point, point_rotated2, level);
		HeighwayDragon(window, point_rotated2, q, level);
		
	}
}


int MainTest_LerpAndRotate (void){
	bx_window win;
	char win_title [] = "Fractale";
	Coord dim = Coord_FromXY(600 , 400);
	
	bx_init();
	win = bx_create_window (win_title, 0, 0, dim.x, dim.y);

	for(;;){
		Coord center = Coord_Scale (dim, 0.5);
		int milliseconds_delay = 10;
		bx_mouse mouse = bx_read_mouse (win);
		Coord cursor = Coord_FromMouse (mouse);
		int radius = 5 , filled = 1 ;
		bx_clear_canvas (win, bx_white());
		
		bx_set_color (bx_black());  DrawUtil_Line (win, center, cursor);
		bx_set_color (bx_red  ());	DrawUtil_Circle (win, center, radius, filled);

		// Point au 2/3 du segment
		Coord point_at_two_third = Coord_Lerp ( center , 2.0/3.0 , cursor);
		bx_set_color (bx_green());	DrawUtil_Circle (win, point_at_two_third, radius, filled);

		// Deuxième segment avec rot = pi/3
		Coord rot = Coord_RotateAround (cursor, PI/3.0, center);
		bx_set_color (bx_red());  DrawUtil_Line (win, rot, center);		
		
		bx_show_canvas (win, milliseconds_delay);
	}
}

int MainTest_KochFlake(void){
	bx_window win;
	char win_title [] = "Fractale";
	Coord dim = Coord_FromXY(600 , 400);
	
	bx_init();
	win = bx_create_window (win_title, 0, 0, dim.x, dim.y);

	for(;;){
		Coord center = Coord_Scale (dim, 0.5);
		int milliseconds_delay = 10;
		bx_mouse mouse = bx_read_mouse (win);
		Coord cursor = Coord_FromMouse (mouse);
		bx_clear_canvas (win, bx_white());
		
	    //KochStep(win, center, cursor);
		//KochLine(win, center, cursor, 2);
		KochFlake(win, center, cursor, 0);
		bx_show_canvas (win, milliseconds_delay);
	}
}

int MainTest_HeighwayDragonStep(void){
	bx_window win;
	char win_title [] = "Fiddlesticks";
	Coord dim = Coord_FromXY(600 , 400);
	
	bx_init();
	win = bx_create_window (win_title, 0, 0, dim.x, dim.y);

	for(;;){
		Coord center = Coord_Scale (dim, 0.5);
		bx_mouse mouse = bx_read_mouse (win);
		int milliseconds_delay = 10;
		Coord cursor = Coord_FromMouse (mouse);
		bx_clear_canvas (win, bx_white());

	//	HeighwayDragonStep(win, center, cursor);
		HeighwayDragon(win, center, cursor, 3);
		bx_show_canvas(win, milliseconds_delay);
	}
}

int main(void){
	//MainTest_LerpAndRotate();
	//MainTest_KochFlake();
	MainTest_HeighwayDragonStep();
	return 0;
}
