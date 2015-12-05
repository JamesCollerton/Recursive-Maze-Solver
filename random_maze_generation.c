/*

Initial Comment: This is the same as the algorithm given in the brief. The way I have interpreted it is that the
maze is square and has an odd number of rows. All of the border is a wall apart from one exit and one entrance (this
wasn't really specified in the brief or wikipedia page). Then any odd row or column can be a wall (this prevents walls
being next to each other) and gaps can be at any even point on the walls (this prevents walls covering gaps). 

This should bridge the problem of the algorithm being designed for walls of very small thickness.

The rest of the design follows as usual and splits the maze into four, then four again, recursively iterating the function
as necessary until the dimensions become too small and it stops.

*/

//								PREPROCESSING

#include "maze_headers.h"

//								START OF FUNCTIONS FOR RANDOM GRID GENERATION

// Used to initialise the random maze with the proper dimensions. Slightly different to the regular maze
// initialisation as it takes different parameters and initialises a border as well (hence <=).
void initialise_maze_rand(player_maze maze, int random_dimension)
{
	int i, j;

	for(i = 0; i <= random_dimension + FIRST_ROW; ++i){
		for(j = 0; j <= random_dimension; ++j){
				maze[i][j] = space;
		}
	}

}

// This is the recursive method for generating the maze. It operates by splitting the grid into four, then continuously
// splitting the quadrants of the grid until it detects that the dimensions are too small and stops.
generate_maze generate_random_maze(player_maze maze, int row_start, int row_end, int col_start, int col_end, SDL_Simplewin *sw, int random_dimension)
{
	int col_rand_point, row_rand_point;

	if(check_dimensions(row_start, row_end, col_start, col_end)){

		initialise_and_print(maze, row_start, row_end, col_start, col_end, sw, random_dimension, &col_rand_point, &row_rand_point);

		//Top left grid (the +1 parts are in order to not include the borders of the grid we have just created).
		if(generate_random_maze(maze, row_rand_point + 1, row_end, col_start + 1, col_rand_point, sw, random_dimension)){
			return(more_maze);
		}

		//Top right grid.
		if(generate_random_maze(maze, row_rand_point + 1, row_end, col_rand_point + 1, col_end, sw, random_dimension)){
			return(more_maze);
		}

		//Bottom left grid.
		if(generate_random_maze(maze, row_start + 1, row_rand_point, col_start + 1, col_rand_point, sw, random_dimension)){
			return(more_maze);
		}

		//Bottom right grid.
		if(generate_random_maze(maze, row_start + 1, row_rand_point, col_rand_point + 1, col_end, sw, random_dimension)){
			return(more_maze);
		}

	}

	return(no_more_maze);
}

// Fills parts of the maze as wall and then randomly chooses where to put holes. Finally it prints the maze.
// The maze_dims look to be specified strangely but they now match those when the border is put back on and 
// so the random maze generation is centred on the screen and aligned with the borders.
void initialise_and_print(player_maze maze, int row_start, int row_end, int col_start, int col_end, SDL_Simplewin *sw, int random_dimension, int *col_rand_point, int *row_rand_point)
{
	int maze_dims[NUM_DIMENSIONS] = {random_dimension + RAND_ROW_START, random_dimension + RAND_COL_START + RAND_END};
	static int iteration = 0;

	fill_walls(maze, col_start, col_end, row_start, row_end, col_rand_point, row_rand_point);

	clear_screen();

	printf("\nGenerating maze, iteration %d\n", iteration++);
	print_maze(maze, maze_dims);
	SDL_print_maze(maze, maze_dims, sw);
}

// Used in the recursion as a break clause and checks that there is more than one cell remaining to 'split'
// before stopping.
generate_maze check_dimensions(int row_start, int row_end, int col_start, int col_end)
{
	if( (row_end - row_start > MIN_DIM) && (col_end - col_start > MIN_DIM) ){
		return(more_maze);
	}

	return(no_more_maze); 
}

// Fills a proportion of the wall array with actual wall signs (hashes) and then removes some according to the algorithm.
void fill_walls(player_maze maze, int col_start, int col_end, int row_start, int row_end, int *col_rand_point, int *row_rand_point)
{

	make_all_wall(row_rand_point, col_rand_point, row_end, row_start, col_start, col_end, maze);

	make_random_spaces(col_end, col_rand_point, row_rand_point, col_start, row_end, maze);
	

}

// Randomly chooses a row and a column of the matrix to turn into a wall and fills it as described before. Note that it looks
// like one row is selected to be odd and one is selected to be even, but this is because the random grid is created with an extra
// row so that the rest of the code which is designed for mazes with an extra dimensions row still functions. In reality both are odd.
void make_all_wall(int *row_rand_point, int *col_rand_point, int row_end, int row_start, int col_start, int col_end, player_maze maze)
{
	int i;

	do{	
		*row_rand_point = ( rand() % (row_end - row_start) )  + row_start;
	}while(*row_rand_point % 2 == 0 );

	for(i = col_start; i < col_end; ++i){
		maze[*row_rand_point][i] = wall;
	}

	do{
		*col_rand_point = ( rand() % (col_end - col_start) ) + col_start;
	}while(*col_rand_point % 2 == 1 );

	for(i = row_start; i < row_end; ++i){
		maze[i][*col_rand_point] = wall;
	}

}

// Randomly chooses cells in the wall to turn into spaces as described earlier with the odd and even technique.
// Two holes are put in the row wall and only one hole is put in the column wall. The brief didn't specify which of the two had
// to have more holes and it doesn't matter as long as one does. Note that it looks like holes are put in even places
// in the row but odd in the column, in reality this isn't true as you must account for the dimensions row.
void make_random_spaces(int col_end, int *col_rand_point, int *row_rand_point, int col_start, int row_end, player_maze maze)
{
	int rand_row_gap, rand_col_gap;

	//Hole on one side of the intersection of the walls.
	if(col_end - *col_rand_point > 0){									//checks dimensions to make sure rand() is applicable.
		do{
			rand_row_gap = ( rand() % (col_end - *col_rand_point + 1) )  + *col_rand_point;		//+ 1 so that the range of numbers this spans includes col_end
		}while( (rand_row_gap % 2 == 0 || rand_row_gap == *col_rand_point) );	 
		maze[*row_rand_point][rand_row_gap] = space;
	}

	//Hole on the other side of the intersection of the walls.
	if(*col_rand_point - col_start > 0){									//checks dimensions to make sure rand() is applicable.
		do{
			rand_row_gap = rand() % (*col_rand_point - col_start + 1);				//+ 1 so that the range of numbers this spans includes col_rand_point
		}while( (rand_row_gap % 2 == 0 || rand_row_gap == *col_rand_point) );
		maze[*row_rand_point][rand_row_gap] = space;
	}

	//Hole above the intersection of the walls.
	if(row_end - *row_rand_point > 0){									//checks dimensions to make sure rand() is applicable.
		do{
			rand_col_gap = ( rand() % (row_end - *row_rand_point + 1) )  + *row_rand_point;		//+ 1 so that the range of numbers this spans includes row_end
		}while( (rand_col_gap % 2 == 1 || rand_col_gap == *row_rand_point) );
		maze[rand_col_gap][*col_rand_point] = space;
	}
}

//Used to fill the surrounding walls of the maze with wall and create an entrance and an exit.
void fill_surrounding_walls(player_maze maze, int random_dimension)
{
	int i, j;

	for(i = 0; i <= random_dimension + FIRST_ROW; ++i){
		for(j = 0; j <= random_dimension; ++j){
			if( (i == FIRST_ROW && j == ENTRANCE_COORD_COL) || (i == random_dimension + EXIT_COORD_ROW && j == random_dimension - EXIT_COORD_COL ) ){
				maze[i][j] = space;
			} 
			else if(j == FIRST_COL || i == FIRST_ROW || j == random_dimension || i == random_dimension + FIRST_ROW){
				maze[i][j] = wall;
			}
		}
	}
}
