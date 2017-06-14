#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <assert.h>

#include <bx.h>


typedef struct Coord{
	double x,y;
}Coord;

typedef enum Dir {
	// NB_DIRS = nombre de directions, uniquement des constantes entières !
	DIR_INVALID= -1,
	DIR_NORTH, DIR_EAST, DIR_SOUTH, DIR_WEST, 
	NB_DIRS
} Dir;

typedef struct Pos{
	int row, col;
} Pos;

typedef struct Cell{
	unsigned char walls[NB_DIRS];
} Cell;

typedef struct Maze
{
	Pos dim;
	Cell * cells;
} Maze;

typedef struct Stack
{
	Pos * elements;
	int nb_elements, capacity;
} Stack;

//------------------------------------------------------------------------
// 1 - Directions : enum Dir
//------------------------------------------------------------------------

char const * Dir_Name(Dir dir){
	switch(dir){
		case DIR_NORTH : return "N";
		case DIR_SOUTH : return "S";
		case DIR_WEST  : return "W";
		case DIR_EAST  : return "E";
		default		   : return "?";
	}
}

Dir Dir_Opposite(Dir dir){
	return (dir + NB_DIRS / 2) % NB_DIRS;
}

Dir Dir_Random(void){
	return rand() % NB_DIRS;
}

void MainTest_Dir(void){
	srand(time(NULL));
	Dir base, opp;

	for(int k = 0; k < 30; k++){
		base = Dir_Random();
		opp  = Dir_Opposite(base);
		printf("%s -> %s\n", Dir_Name(base), Dir_Name(opp));
	}
}

//----------------------------------------------------------------------
// 2 - Positions et dimensions : struct Pos
//----------------------------------------------------------------------

Pos Pos_Make(int row, int col){
	return(Pos){
		.row = row,
		.col = col
	};
}

Pos Pos_Neighbor(Pos pos, Dir dir){
	switch(dir){
		case DIR_NORTH : return Pos_Make(pos.row - 1, pos.col);
		case DIR_SOUTH : return Pos_Make(pos.row + 1, pos.col);
		case DIR_EAST  : return Pos_Make(pos.row, pos.col + 1);
		case DIR_WEST  : return Pos_Make(pos.row, pos.col - 1);
		default		   : return pos;
	}
}

int Int_RandomAmongFirst(int nb_values){
	return rand() % nb_values;
}

Pos Pos_Random(Pos dim){
	return Pos_Make(Int_RandomAmongFirst(dim.row),Int_RandomAmongFirst(dim.col));
}

void MainTest_Pos(void){
	srand(time(NULL));
	Pos dim = Pos_Make(4, 2);

	for(int k = 0; k < 30 ; k++){
		Pos pos = Pos_Random(dim);
		Dir dir = Dir_Random();
		Pos next_pos = Pos_Neighbor(pos, dir);
		printf("(%d, %d) -> %s -> (%d, %d)\n", pos.row, pos.col, Dir_Name(dir), next_pos.row, next_pos.col);
	}
}

//----------------------------------------------------------------------
// 3 - Cellules : struct Cell
//----------------------------------------------------------------------

void Cell_Init(Cell * cell){

	for(int dir = 0; dir < NB_DIRS; dir++){
		cell->walls[dir] = 1;
	}
}

void Cell_EraseWall(Cell * cell, Dir dir){
	cell->walls[dir] = 0;
}

int Cell_HasWall(Cell const * cell, Dir dir){
	return cell->walls[dir];
}

int Cell_IsIsolated(Cell const * cell){

	for(int dir = 0; dir < NB_DIRS; dir ++ ){
		if(!Cell_HasWall(cell, dir))	return 0;
	}
	return 1;
}

char * Cell_ToString( Cell const * cell){
	static char text[100];
	char status = Cell_IsIsolated(cell) ?  'I' : 'O';
	int k = sprintf(text, "%c : ", status);

	for(Dir dir = 0; dir < NB_DIRS; dir++){
		char const * name = Cell_HasWall(cell, dir)? Dir_Name(dir) : "-";
		k += sprintf(text + k, "%s", name);
	}

	return text;
}

void MainTest_Cell(void){
	Cell cell;
	Cell_Init(&cell);

	for( int k = 0; k < 10 ; k++){
	//    Cell_Init(&cell);
		Dir dir = Dir_Random();
		printf("%s - %s", Cell_ToString(&cell), Dir_Name(dir));
		Cell_EraseWall(&cell, dir);
		printf(" = %s  ", Cell_ToString(&cell));
		printf("\n");
	}
	printf("\n");
}

// ---------------------------------------------------------------------------------------
//	 4 - Labyrinthe : struct Maze
// ---------------------------------------------------------------------------------------

int Maze_RowIsOutOfBound(Maze const * maze, int row){
	return row >= maze->dim.row || row < 0;
}

int Maze_ColIsOutOfBound(Maze const * maze, int col){
	return col >= maze->dim.col || col < 0;
}

int Maze_PosIsOutOfBound(Maze const * maze, Pos pos){
	return Maze_RowIsOutOfBound(maze, pos.row)
		 ||Maze_ColIsOutOfBound(maze, pos.col);
}


int Maze_NbCells(Maze const * maze){
	return maze->dim.row * maze->dim.col ;
}

int Maze_FlatIndex(Maze const * maze, Pos pos){
	return  pos.row * maze->dim.col + pos.col;
}


Cell * Maze_CellAt(Maze const * maze, Pos pos){
	assert (! Maze_PosIsOutOfBound(maze, pos));

	int flat_index = Maze_FlatIndex(maze, pos);
	return & maze->cells[flat_index];
}

void Maze_Init(Maze * maze, Pos dim){
	maze->dim = dim;
	int nb_cells = Maze_NbCells(maze);
	maze->cells = malloc((nb_cells) * sizeof * maze->cells);

	for(int k = 0; k < nb_cells; k++){
		Cell_Init(&maze->cells[k]);
	}
}

void Maze_Clean(Maze * maze){
	free(maze->cells);
}


// ---------------------------------------------------------------------------------------
//  5 - Affichage graphique
// ---------------------------------------------------------------------------------------


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

void DrawUtil_Line(bx_window win, Coord p ,Coord q){
	bx_draw_line(win, p.x, p.y, q.x, q.y);
}

Coord Coord_Scale(Coord p, double lambda){
	return Coord_FromXY(p.x * lambda, p.y * lambda);
}

void Cell_Draw(Cell const * cell, bx_window win, Coord top_left, int side){
	
	Coord diagonal  = Coord_FromXY(side - 2, side - 2);
	Coord bot_right = Coord_Add(top_left, diagonal);
	Coord top_right = Coord_FromXY(bot_right.x, top_left.y);
	Coord bot_left  = Coord_FromXY(top_left.x, bot_right.y);

	if(Cell_HasWall(cell, DIR_NORTH))	DrawUtil_Line(win, top_left, top_right);
	if(Cell_HasWall(cell, DIR_SOUTH))	DrawUtil_Line(win, bot_left, bot_right);
	if(Cell_HasWall(cell, DIR_EAST))	DrawUtil_Line(win, top_right, bot_right);
	if(Cell_HasWall(cell, DIR_WEST))	DrawUtil_Line(win, top_left, bot_left);
}

Coord Maze_CellCoord(Pos pos, Coord maze_coord, int cell_side){
	Coord coord = Coord_FromXY(pos.col,pos.row);
	Coord scale = Coord_Scale(coord, cell_side);
	return Coord_Add(scale, maze_coord);
}

void Maze_Draw(Maze const * maze, bx_window win, Coord top_left, int cell_side){
	Pos pos;
	for(pos.row = 0; pos.row < maze->dim.row; pos.row++){
		for(pos.col = 0; pos.col < maze->dim.col; pos.col++){

			Coord cell_coord = Maze_CellCoord(pos, top_left, cell_side);
			Cell * cell = Maze_CellAt(maze, pos);
			Cell_Draw(cell, win, cell_coord, cell_side);
		
		}
	}
}

int MainTest_Drawing(void){

	int nb_pixels = 30;
    
    bx_window win;
    char win_title[] = " Labibi";
	Coord win_pos  =  Coord_FromXY(50,50);
	Coord canvas_dim  =  Coord_FromXY(600,400);
	bx_init();
	win = bx_create_window(win_title, win_pos.x, win_pos.y, canvas_dim.x, canvas_dim.y);
	

	Pos dim = Pos_Make(3, 10);
	Maze maze;
	Maze_Init(&maze, dim);
	Pos north_pos = Pos_Make(0, dim.col/2);
	Pos south_pos = Pos_Make(dim.row - 1, dim.col/2);
	Pos east_pos =  Pos_Make(dim.row/2, dim.col - 1);
	Pos west_pos = 	Pos_Make(dim.row/2, 0);


	Cell_EraseWall(Maze_CellAt(&maze, north_pos), DIR_NORTH);
	Cell_EraseWall(Maze_CellAt(&maze, south_pos), DIR_SOUTH);
	Cell_EraseWall(Maze_CellAt(&maze, west_pos),  DIR_WEST);
	Cell_EraseWall(Maze_CellAt(&maze, east_pos),  DIR_EAST);

	Maze_Draw(&maze, win, Coord_FromXY(50,20), nb_pixels);
	bx_show_canvas(win, 0);
	bx_loop();
	return 0;
}


// ---------------------------------------------------------------------------------------
// 6 - Destruction aleatoire de murs
// ---------------------------------------------------------------------------------------


Pos Maze_RandomPos(Maze const * maze){
	return Pos_Random(maze->dim);
}

void Maze_EraseCellWall(Maze * maze, Pos pos, Dir dir){
	Cell * cell = Maze_CellAt(maze, pos);
	Pos next_pos = Pos_Neighbor(pos, dir);
	Dir opp_dir  = Dir_Opposite(dir);
	Cell * next_cell = Maze_CellAt(maze, next_pos);
	Cell_EraseWall(cell, dir);
	Cell_EraseWall(next_cell, opp_dir);
}

void Maze_EraseRandomWalls_Iter(Maze * maze, int nb_times){
	for(int k = 0; k < nb_times ; k++){
		Pos pos = Maze_RandomPos(maze);
		Dir dir = Dir_Random();
		Maze_EraseCellWall(maze, pos, dir);
	}
}

void Maze_EraseRandomWalls_Rec(Maze * maze, int nb_times){
	if(nb_times == 0)	return;
	
	Pos pos = Maze_RandomPos(maze);
	Dir dir = Dir_Random();
	Maze_EraseCellWall(maze, pos, dir);
	Maze_EraseRandomWalls_Rec(maze, nb_times - 1);
}


void MainTest_EraseRandomWalls(void){
	int nb_pixels = 30;
    
    bx_window win;
    char win_title[] = " Labibi";
	Coord win_pos  =  Coord_FromXY(50,50);
	Coord canvas_dim  =  Coord_FromXY(600,400);
	bx_init();
	win = bx_create_window(win_title, win_pos.x, win_pos.y, canvas_dim.x, canvas_dim.y);
	

	Pos dim = Pos_Make(5, 10);
	Maze maze;
	Maze_Init(&maze, dim);

	Maze_EraseRandomWalls_Iter(&maze, 6);
	//Maze_EraseRandomWalls_Rec(&maze, 6);
	
	Maze_Draw(&maze, win, Coord_FromXY(50,20), nb_pixels);
	bx_show_canvas(win, 0);
	bx_loop();

}

// ---------------------------------------------------------------------------------------
// 7 - Marche aléatoire
// ---------------------------------------------------------------------------------------

int Maze_CountIsolatedNeighbors ( Maze const * maze, Pos pos) {
	int nb_isolated = 0; 
	for (int dir = 0; dir < NB_DIRS; dir++) {
		Pos next_pos = Pos_Neighbor(pos, dir);
		if (Maze_PosIsOutOfBound (maze, next_pos)) continue;
		Cell * next_cell = Maze_CellAt(maze, next_pos);
		if (! Cell_IsIsolated(next_cell)) continue;
		nb_isolated ++;
	}
	return nb_isolated;
}

Dir Maze_IsolatedDir (Maze const * maze, Pos pos, int n) {
	int  nb_isolated_dirs = 0;
	for (int dir = 0; dir < NB_DIRS; dir++) {
		Pos next_pos = Pos_Neighbor(pos, dir);
		if (Maze_PosIsOutOfBound (maze, next_pos)) continue;
		Cell * next_cell = Maze_CellAt(maze, next_pos);
		if (! Cell_IsIsolated(next_cell)) continue;
		nb_isolated_dirs ++;
		if (nb_isolated_dirs == n) return dir; 
	}
	return DIR_INVALID;
}


int RandomAmongN(int n){
	return rand() % n;
}

int RandomInRange(int range_start, int range_length){
	return range_start + RandomAmongN (range_length);
}

int RandomBetween(int min, int max){
	return RandomInRange (min, max-min+1);
}


Dir Maze_RandomIsolatedDir(Maze const * maze, Pos pos, int n){
	int random_n_dir = RandomBetween(1, n);
	return Maze_IsolatedDir(maze, pos, random_n_dir);
}

void Maze_DoRandomWalk_Iter (Maze * maze, Pos start, bx_window win, Coord maze_coord, int cell_side, int delay){
	Maze_Draw(maze, win, maze_coord, cell_side);
	Pos current_pos = start;
	for (;;) {
		int nbrIsolatedDir = Maze_CountIsolatedNeighbors(maze, current_pos);
		if (nbrIsolatedDir == 0) break;
		Dir dir = Maze_RandomIsolatedDir(maze, current_pos, nbrIsolatedDir);
		Maze_EraseCellWall(maze, current_pos, dir);
		Pos next_pos = Pos_Neighbor(current_pos, dir);
	//	printf ("(%d %d)->(%d %d)\n", current_pos.row, current_pos.col, next_pos.row, next_pos.col);
		current_pos= next_pos;
		bx_clear_canvas(win, bx_white());
		Maze_Draw(maze, win, maze_coord, cell_side);
		bx_show_canvas(win, delay);
	}

	bx_loop();
}

void Maze_DoRandomWalk_Rec(Maze * maze, Pos start, bx_window win, Coord maze_coord, int cell_side, int delay){
	Pos current_pos = start;
	int nbrIsolatedDir = Maze_CountIsolatedNeighbors(maze, current_pos);
	if (nbrIsolatedDir == 0) return;
	
	Dir dir = Maze_RandomIsolatedDir(maze, current_pos, nbrIsolatedDir);
	Maze_EraseCellWall(maze, current_pos, dir);
	current_pos = Pos_Neighbor(current_pos, dir);
	
	bx_clear_canvas(win, bx_white());
	Maze_Draw(maze, win, maze_coord, cell_side);
	bx_show_canvas(win, delay);
	Maze_DoRandomWalk_Rec(maze, current_pos, win, maze_coord, cell_side, delay);
}

void MainTest_DoRandomWalk(void){
	bx_window win;
    char win_title[] = " Mudada";
	Coord win_pos  =  Coord_FromXY(50,50);
	Coord canvas_dim  =  Coord_FromXY(600,400);

	srand(time(NULL));
	
	bx_init();
	win = bx_create_window(win_title, win_pos.x, win_pos.y, canvas_dim.x, canvas_dim.y);
	
	Maze maze;
	
	Pos dim = Pos_Make( 5, 10);
	Pos start = Pos_Random(dim);

	Maze_Init(&maze, dim);
	Coord maze_coord = Coord_FromXY(2,4);

//	Maze_DoRandomWalk_Iter(&maze, start, win, maze_coord , 30, 10);
	Maze_DoRandomWalk_Rec(&maze, start, win, maze_coord, 30, 10);

	bx_loop();
}

// ---------------------------------------------------------------------------------------
// 8 - Pile de positions
// ---------------------------------------------------------------------------------------

void Stack_Init( Stack * s, int capacity){
	s->capacity = capacity;
	s-> nb_elements = 0;
	s->elements = malloc(capacity * sizeof * s->elements); 
}

void Stack_Clean(Stack * s){
	s->capacity = 0;
	s->nb_elements = 0;
	free(s);
}

int Stack_IsEmpty (Stack const * s) {
	return s->nb_elements==0;
}

int Stack_IsFull(Stack const * s) {
	return s->nb_elements==s->capacity;
}

void Stack_Push(Stack * s , Pos pushed){
	if( Stack_IsFull(s) )	return;
	s->elements[s->nb_elements] = pushed;
	s->nb_elements++;
}

Pos Stack_Pop(Stack * s){
	assert(!Stack_IsEmpty(s));
	Pos pos = s->elements[s->nb_elements - 1];
	s->nb_elements--;
	return pos;
}

Pos Stack_Peek (Stack const * s) {
	assert (!Stack_IsEmpty(s));
	return s->elements[s->nb_elements - 1];
}

void Stack_Print (Stack const * s, FILE * file) {
	if (file == NULL) return ;
	for (int k = 0; k < s->nb_elements; k++) {
		fprintf(file, "(%d ,%d) ", s->elements[k].row, s->elements[k].col);
	}
	fprintf(file, "\n");
}

//---------------------------------------------------------------------------------------------------
// 9 Génération aléatoire
//---------------------------------------------------------------------------------------------------


void DrawUtil_Box(bx_window win ,Coord center,double radius, int filled){
	int diam = 2*radius;
	bx_draw_box(win, center.x-radius, center.y-radius, diam, diam, filled);
}

void DrawUtil_Circle(bx_window win, Coord center, double radius, int filled){
	int diam = 2*radius;
	bx_draw_circle(win, center.x-radius, center.y-radius, diam, diam, filled);
}

void Maze_DrawPath(Maze const * maze, Stack const * pending, bx_window win, Coord maze_coord, int cell_side){
	Coord cell_center = Coord_FromXY(cell_side/2, cell_side/2);
	
	if (pending->nb_elements == 0) return;
	
	if (pending->nb_elements == 1) {
		Coord cell_coord = Maze_CellCoord(pending->elements[0],maze_coord,cell_side);
		Coord cell_coordcenter = Coord_Add(cell_coord, cell_center);
		DrawUtil_Box(win, cell_coordcenter, cell_side/3, 1);
	}

	if (pending->nb_elements >1) {
		for (int k = 0; k< pending->nb_elements - 2; k++){ 
			Coord cell_coord1 = Maze_CellCoord(pending->elements[k],maze_coord,cell_side);
			Coord cell_coordcenter1 = Coord_Add(cell_coord1, cell_center);
			Coord cell_coord2 = Maze_CellCoord(pending->elements[k + 1],maze_coord,cell_side);
			Coord cell_coordcenter2 = Coord_Add(cell_coord2, cell_center);
			
			DrawUtil_Line(win, cell_coordcenter1, cell_coordcenter2);		
		}
	}
}

void Maze_DoRandomGen_Iter (Maze * maze, Pos start, bx_window win, Coord maze_coord, int cell_side, int delay) {
	Pos current_pos = start;
	Stack stack; 
	Stack_Init(&stack, Maze_NbCells(maze));
	Stack_Push(&stack, current_pos);
	
	for (;;) {
		if (Stack_IsEmpty(&stack)) return;
		
		int nb_isolated = Maze_CountIsolatedNeighbors(maze, current_pos);
		if (nb_isolated == 0) {
			current_pos = Stack_Pop(&stack);		
		} 
		else {		
			Dir dir = Maze_RandomIsolatedDir(maze, current_pos, nb_isolated);
			Maze_EraseCellWall(maze, current_pos, dir);
			current_pos = Pos_Neighbor(current_pos, dir);
			Stack_Push(&stack, current_pos);
			bx_clear_canvas(win, bx_white());
			Maze_Draw(maze, win, maze_coord, cell_side);
			Maze_DrawPath(maze, &stack, win, maze_coord, cell_side);
			bx_show_canvas(win, delay);
		}
		bx_clear_canvas(win, bx_white());
		Maze_Draw(maze, win, maze_coord, cell_side);
		Maze_DrawPath(maze, &stack, win, maze_coord, cell_side);
		bx_show_canvas(win, delay);
	}
}

void Maze_DoRandomGen_Rec(Maze * maze, Pos current, bx_window win, Coord maze_coord, int cell_side, int delay){
	Pos pos = current;
	for (;;) {
		int nbrIsolatedDir = Maze_CountIsolatedNeighbors(maze, pos);
		if (nbrIsolatedDir == 0) return;
		Dir dir = Maze_RandomIsolatedDir(maze, pos, nbrIsolatedDir);
		Maze_EraseCellWall(maze, pos, dir);
		pos = Pos_Neighbor(pos, dir); 
		bx_clear_canvas(win, bx_white());
		Maze_Draw(maze, win, maze_coord, cell_side);
		bx_show_canvas(win, delay);
		Maze_DoRandomGen_Rec(maze, pos, win, maze_coord, cell_side, delay);
	}
}

void MainTest_DoRandomGen(void){
	bx_window win;
    char win_title[] = " Chewiii";
	Coord win_pos  =  Coord_FromXY(50,50);
	Coord canvas_dim  =  Coord_FromXY(600,400);

	srand(time(NULL));
	
	bx_init();
	win = bx_create_window(win_title, win_pos.x, win_pos.y, canvas_dim.x, canvas_dim.y);
	
	Maze maze;
	
	Pos dim = Pos_Make( 5, 10);
	Pos start = Pos_Random(dim);

	Maze_Init(&maze, dim);
	Coord maze_coord = Coord_FromXY(2,4);
	int cell_side = 30;
	int delay = 1;

	Maze_DoRandomGen_Iter(&maze, start, win, maze_coord, cell_side, delay);

	bx_loop();
}

int main(void){
//	MainTest_Dir();
//	MainTest_Pos();
//	MainTest_Cell();
//	MainTest_Drawing();
//	MainTest_EraseRandomWalls();
//	MainTest_DoRandomWalk();
//	printf("\n");
//	MainTest_DoRandomGen();
//	printf("\n");
    srand(time(NULL));
    Dir direct, opposite;
    Pos dim = Pos_Make(4,2);
    bx_window win;
	char win_title[]="Niiiiiiiinja";
	
	Coord win_pos = Coord_FromXY(50,50);
	Coord canvas_dim= Coord_FromXY(600,400);
	
	bx_init();
	win=bx_create_window(win_title,win_pos.x, win_pos.y,canvas_dim.x,canvas_dim.y );
    for(int k = 0; k < 30; k++){
        direct = Dir_Random();
        opposite = Dir_Opposite(direct);
        printf("%s->%s\n",Dir_Name(direct),Dir_Name(opposite));
        Pos pos = Pos_Random(dim);
        Dir dir = Dir_Random();
        Pos next_pos = Pos_Neighbor(pos, dir);
        printf("(%d, %d) -> %s -> (%d ,%d)\n",pos.row,pos.col,Dir_Name(dir),next_pos.row,next_pos.col);

    }
    Cell cell;
    Cell_Init(&cell);
    for(int k = 0; k < 10; k++){
		Dir dir = Dir_Random();
		printf("(%s)-  %s = ", Cell_ToString(&cell), Dir_Name(dir));
		Cell_EraseWall(&cell, dir);
		printf("(%s)\n",Cell_ToString(&cell));
	}
    Maze maze;
    dim = Pos_Make(6, 13);
    Maze_Init(&maze, dim);
    
    
        
    Pos start = Pos_Random(dim);
    Maze_DoRandomGen_Iter (&maze, start, win, Coord_FromXY(50,20), 40, 100);
    bx_loop();
    
    return 0;
}
