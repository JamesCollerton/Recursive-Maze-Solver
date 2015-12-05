//								PREPROCESSING

#include "maze_headers.h"

//								FUNCTIONS FOR USE IN MAZE_SOLVING.
 

//This is used in various functions to clear the screen. It makes 100 attempts, warning the user it has failed every 10 attempts.
void clear_screen(void)
{
	int check_system_one, check_system_two, attempt_counter = 0;

	do{
		attempt_counter++;
		check_system_one = system("sleep .25");		//sleeps the screen for .25 of a second before moving on.
		check_system_two = system("clear");		//used for clearing the screen.

		if( ( check_system_one != CHECK_SUCCESS || check_system_two != CHECK_SUCCESS ) && attempt_counter % NUM_CLEAR_ATTEMPT_WARN == 0){
			printf("\nAttempt %d. Sorry, one of the system commands has failed, retrying.\n", attempt_counter);
		}
		if(attempt_counter == NUM_CLEAR_ATTEMPT){
			printf("\n%d attempts failed.\nAborting.\n" 	  		\
			       "The screen will not clear per generation.\n", NUM_CLEAR_ATTEMPT);
			exit(EXIT_CODE);
		}

	}while( ( check_system_one != CHECK_SUCCESS || check_system_two != CHECK_SUCCESS ) && attempt_counter != NUM_CLEAR_ATTEMPT );

}

// Used in main to check what arguments have been entered into the console. All actual errors are followed with additional standard errors
// in case the user is redirecting the printed ouputs of the function to a file as discussed in lectures.
void check_command_line_args(int argc, char *argv[], SDL_on_off *SDL_indicator, player_maze maze, int maze_dims[NUM_DIMENSIONS], SDL_Simplewin *sw)
{
	//No additional arguments added with command.
	if(argc == TOO_FEW_ARGS){

		printf("\n\nYou have to enter your filename with the command! Try again... \n");
		fprintf(stderr, "\n(stderr: No filename entered at command line.)\n\n");
		exit(EXIT_CODE);

	}
	//A possible filename has been entered with the command.
	else if(argc == POSS_FILENAME){

		char *filename = argv[1];
		check_filename_copy_maze(filename, maze, maze_dims, sw);

	}
	//A possible command for SDL activation has been entered.
	else if(argc == POSS_SDL){

		char *filename = argv[1];
		char *SDL_Entry = argv[2];

		SDL_check(SDL_Entry, SDL_indicator, sw);

		check_filename_copy_maze(filename, maze, maze_dims, sw);


	}
	//Too many arguments entered.
	else{

		printf("\n\nYou have entered too many commands! Try again... \n");
		fprintf(stderr, "\n(stderr: Too many options at the command line)\n\n");
		exit(EXIT_CODE);

	}
}

// Used when an argument that could possibly be to activate SDL is entered and checks the input etc.
// Quits and prints an error message if the person has entered something unrecognisable as the SDL indicator.
// Note: SDL entry is case sensitive as specified.
void SDL_check(char *SDL_Entry, SDL_on_off *SDL_indicator, SDL_Simplewin *sw)
{

	if( strcmp(SDL_Entry, "SDL") == 0 ){
		printf("\nYou have selected to activate SDL! \n");
		*SDL_indicator = SDL_on;
		Neill_SDL_Init(sw);
		
	}
	else{
		printf("\nThe SDL extension you entered wasn't recognised. Try 'SDL'... \n");
		fprintf(stderr, "\n(stderr: SDL extension not recognised.)\n\n");
		exit(EXIT_CODE);

	}

}

// Makes sure that whatever has been entered as the argument along with the executable has a file in the current directory or is RANDOM (case sensitive).
// If it is random it passes into the random_grid_procedure function and generates a random grid.
// Otherwise reads in the dimensions of the maze from the file and copies the maze from the file into the program and stores it in the 'maze' variable.
void check_filename_copy_maze(char *filename, player_maze maze, int maze_dims[NUM_DIMENSIONS], SDL_Simplewin *sw)
{
	FILE *maze_file;
	rand_selected random_maze_indicator = random_unselected;

	get_correct_maze_name(filename, &maze_file, &random_maze_indicator);

	if(random_maze_indicator == random_selected){

		random_grid_procedure(maze, maze_dims, sw);
		
	}
	else{
		get_maze_dims(maze_file, maze_dims);

		initialise_maze(maze, maze_dims);

		copy_in_maze(maze_file, maze, maze_dims);

		fclose(maze_file);
	}
}

//This contains all the functions involved in making the random grid. The method I took was that the random grid was square and had to have an even
//number of dimensions (explained why in the random maze generation functions later). Everything is then initialised to a space. From there the INSIDE 
//of the grid is passed to the random maze generation algorithm, hence the non-zero start and smaller end points, with the random maze generation algorithm 
//filling in the inside. Next all of the borders are filled in around the maze and two gaps are left as entrance and exit. Finally the maze_dims are
//set for the random maze to be used in the rest of the function.
void random_grid_procedure(player_maze maze, int maze_dims[NUM_DIMENSIONS], SDL_Simplewin *sw)
{
	int random_dimension;

	do{
		random_dimension = (rand() % SENSIBLE_DIMS) + SENSIBLE_DIMS;		//Gives random dimensions of a sensible size to make a nice maze.
	}while(random_dimension % 2 == 1);						//To make even.

	initialise_maze_rand(maze, random_dimension);

	generate_random_maze(maze, RAND_ROW_START, random_dimension, RAND_COL_START, random_dimension - RAND_END, sw, random_dimension - RAND_END);

	fill_surrounding_walls(maze, random_dimension);

	maze_dims[row] = random_dimension + BORDER;
	maze_dims[col] = random_dimension + BORDER;

	clear_screen();
	printf("\n\n\n\n");								//So the maze remains stationary on the terminal screen with the addition of extra dimensions.
	print_maze(maze, maze_dims);							//Needs to be printed again to display the borders and the full random grid.
	SDL_print_maze(maze, maze_dims, sw); 
}

//This initialises a maze of the correct size according to the dimensions stored in the file.
//These dimensions are checked to make sure they are accurate in a later function.
void initialise_maze(player_maze maze, int maze_dims[NUM_DIMENSIONS])
{
	int i, j;

	for(i = 0; i < maze_dims[row] + FIRST_ROW; ++i){
		for(j = 0; j < maze_dims[col]; ++j){				
			maze[i][j] = space;
		}
	}

}

//First of all checks to see if the user has specified a random input, if so it returns this as an indicator to be used later.
//Otherwise it has a pointer to the FILE pointer passed to it and uses it to try to open up the file and store it in maze_file.
//Runs checks to make sure that whatever it has tried to open up has succeeded and if not prints an error message.
void get_correct_maze_name(char *filename, FILE **maze_file, rand_selected *random_maze_indicator)
{

		if( ( strcmp(filename, "RANDOM") ) == 0 ){
			*random_maze_indicator = random_selected;
		}
		else{
			*maze_file = fopen(filename, "r");
			if(*maze_file == NULL){
				printf("\nThe name of the file you entered wasn't recognised. Try again... \n");
				fprintf(stderr, "\n(stderr: Filename not recognised.)\n\n");
				exit(EXIT_CODE);
			}
		}

}

//Searches for the first two digits in the file and assumes that they are the dimensions of the grid.
//(Later on checks are run to make sure that this is the truth.)
void get_maze_dims(FILE *maze_file, int maze_dims[NUM_DIMENSIONS])
{	
	int dimension_index = 0;
	int scan_checker; 

	do{

		scan_checker = fscanf(maze_file, "%d", &maze_dims[dimension_index]);
		if(scan_checker == 0){
			printf("\nThere appears to be a problem scanning the dimensions in the file. \n");
			fprintf(stderr, "\n(stderr: Scanning dimensions error.)\n\n");
			exit(EXIT_CODE);
		}

		++dimension_index;

	}while(dimension_index < NUM_DIMENSIONS);

	check_scanned_dimensions(maze_dims);
}

//Used in get_maze_dims to make sure that what the user has entered as the dimensions of the grid in their
//file are sensible. The -1 parts are as MAX_DIM is defined as the maximum dimensions plus one to include the dimensions line.
void check_scanned_dimensions(int maze_dims[NUM_DIMENSIONS])
{
	if(maze_dims[row] > MAX_DIM - FIRST_ROW || maze_dims[col] > MAX_DIM - FIRST_ROW){
		printf("\nThe amount of rows or columns indicated in the file is too great. Please Check.\n");
		fprintf(stderr, "\n(stderr: File dimensions do not match.)\n\n");
		exit(EXIT_CODE);
	}
	else if(maze_dims[row] < MIN_DIM || maze_dims[col] < MIN_DIM){
		printf("\nThe amount of rows or columns indicated in the file is too small. Please Check.\n");
		fprintf(stderr, "\n(stderr: File dimensions do not match.)\n\n");
		exit(EXIT_CODE);
	}

}

//Uses the grid dimensions from earlier and scans in all the characters that comprise the grid.
//Note that one more row has to be scanned in order to account for the first row.
//If the dimensions of the grid don't match what the user told us it prints an error, otherwise it prints the grid.
void copy_in_maze(FILE *maze_file, player_maze maze, int maze_dims[NUM_DIMENSIONS])
{
	int i = 0, j = 0, c;

	while( (c = fgetc(maze_file)) != EOF){
		if( c == '\n' ){
			if(j != 0 && j != maze_dims[col]){
				printf("\nThe columns indicated are incorrect. Please check the file.\n");
				fprintf(stderr, "\n(stderr: File dimensions do not match.)\n\n");
				exit(EXIT_CODE);
			}
			j = 0;
			i++;
		}
		else{
			maze[i][j] = c;
			j++;
		}
	}

	if(i != maze_dims[row] + FIRST_ROW){
		printf("\nThe rows indicated are incorrect. Please check the file.\n");
		fprintf(stderr, "\n(stderr: File dimensions do not match.)\n\n");
		exit(EXIT_CODE);
	}

	check_maze_contents(maze, maze_dims);

	printf("\n\nThe maze has been scanned in as below: \n\n");

	print_maze(maze, maze_dims);

}

//Cycles through all the components of the grid and makes sure that they are all either a hash or a space.
void check_maze_contents(player_maze maze, int maze_dims[NUM_DIMENSIONS])
{
	int i, j;

	for(i = FIRST_ROW; i < maze_dims[row] + FIRST_ROW; ++i){
		for(j = 0; j < maze_dims[col]; ++j){
			if(maze[i][j] != space && maze[i][j] != wall){
				printf("\nThe contents of the file at row: %d, col %d, are incorrect.\n" 	\
					  "(Not a wall or space).\n", i, j);
				fprintf(stderr, "\n(stderr: File contents do not match what is expected.)\n\n");
				exit(EXIT_CODE);			
			}
		}
	}
 
}

//This is used to print the maze at different stages.
//The loop for the rows is staggered by one to prevent printing out the line concerned with specifying the dimensions.
//Also the placeholders used in later code that are not overwritten by path markers are translated back into spaces
//as they are not used to show the route.
void print_maze(player_maze maze, int maze_dims[NUM_DIMENSIONS])
{
	int i, j;

	printf("\n");

	for(i = FIRST_ROW; i < maze_dims[row] + FIRST_ROW; ++i){
		for(j = 0; j < maze_dims[col]; ++j){

			if(maze[i][j] == PLACEHOLDER){
				printf(" ");
			}
			else{
				printf("%c", maze[i][j]);
			}

		}
		printf("\n");
	}

	printf("\n\n");

}

//This is used for printing outputs through SDL. It scales the components of the current maze board
//as shown in the terminal and prints them as green or white rectangles depending on if they are part
//of the path or wall. The counting for the row starts from one so as not to count the dimensions row.
//Also, the maze is centred but the SDL window is not fitted to it and the SDL window can be closed during
//the animation.
void SDL_print_maze(player_maze maze, int maze_dims[NUM_DIMENSIONS], SDL_Simplewin *sw)
{
	int i, j;
	SDL_Rect rectangle;
	rectangle.w = RECT_SIZE;
	rectangle.h = RECT_SIZE;

	if(!sw->finished){

		for(i = FIRST_ROW; i < maze_dims[row] + FIRST_ROW; ++i){
			for(j = 0; j < maze_dims[col]; ++j){
		
			rectangle.y = (SCALE_CONST * i) +  ( WHEIGHT - ( WHEIGHT / MAX_DIM ) * (maze_dims[row]) )  / 2;						//To centralise the maze.
			rectangle.x = (SCALE_CONST * j) +  ( WWIDTH - ( WWIDTH / MAX_DIM ) * (maze_dims[col]) )  / 2;			

				if(maze[i][j] == wall){
					Neill_SDL_SetDrawColour(sw, SDL_8BITCOLOUR - 1, SDL_8BITCOLOUR - 1, SDL_8BITCOLOUR - 1);				//Sets the colour to white.
					SDL_RenderFillRect(sw->renderer, &rectangle); 
				}
				else if(maze[i][j] == path){
					Neill_SDL_SetDrawColour(sw, SDL_8BITCOLOUR - GREEN_ONE, SDL_8BITCOLOUR - GREEN_TWO, SDL_8BITCOLOUR - GREEN_THREE);	//Sets the colour to green.
					SDL_RenderFillRect(sw->renderer, &rectangle); 			
				}

			}
		}

		SDL_update(sw);
	}
	else{

		printf("\nUser chose to quit program.\n\n");
		exit(EXIT_CODE);	
		
	}

}

//Just a wrapper for updating the SDL window, delaying the output and looking for events.
void SDL_update(SDL_Simplewin *sw)
{
	SDL_RenderPresent(sw->renderer);					//Updates the screen with the objects.
	SDL_UpdateWindowSurface(sw->win);
	SDL_Delay(DELAY);
	Neill_SDL_Events(sw);							//Taken from Neill's functions and looks for closing the window etc.
}

//Used to find the entrance to the maze. Scans through all elements of the maze and looks to see if they are on the border and not a wall.
//If so it works out the distance between the entrance and the top left hand corner, comparing it to the previous smallest distance
//(max_distance). If it senses that this new entrance is closer it reassigns the entry coordinates to that entrance and changes the max distance.
void find_entrance(player_maze maze, int maze_dims[NUM_DIMENSIONS], int entrance_coords[NUM_DIMENSIONS], SDL_Simplewin *sw, SDL_on_off SDL_indicator)
{
	int i, j, distance, entrance_counter = 0;
	int max_distance = MAX_DIM * MAX_DIM;				//Initialised with a very large number so the first found entrance is automatically the closest entrance.

	for(i = FIRST_ROW; i < maze_dims[row] + FIRST_ROW; ++i){
		for(j = 0; j < maze_dims[col]; ++j){

			if( check_on_edge(i, j, maze_dims) && maze[i][j] != wall){
				distance = i + j;
				++entrance_counter;
				if( distance < 	max_distance ){
					entrance_coords[row] = i;
					entrance_coords[col] = j;
					max_distance = distance;
				}
			}

		}
	}

	number_of_entrances_checker(entrance_counter, maze, maze_dims, sw, SDL_indicator);
}

//Checks how many entrances have been registered and if it's 0 or 1 (i.e. entrance, no exit) then this is registered as an error as it is not a valid maze.
void number_of_entrances_checker(int entrance_counter, player_maze maze, int maze_dims[NUM_DIMENSIONS], SDL_Simplewin *sw, SDL_on_off SDL_indicator)
{

	if(entrance_counter == NO_ENTRANCE || entrance_counter == NO_EXIT){
		if(entrance_counter == NO_ENTRANCE){
			printf("\nThere's not even an entrance!\n\n");
		}
		else{
			printf("\nI can't get to you! Looks like you're stuck... FOREVER!\n\n");
		}
		SDL_print_maze(maze, maze_dims, sw);

		if(SDL_indicator == SDL_on){
			SDL_quit_checker(sw);
		}

		exit(EXIT_CODE);
	}

}

//Used when either there are no entrances or only one entrance and waits for the user to close the SDL window. I have treated
//these two cases as errors and only returned impossible from main when there is an exit and an entrance but no root inbetween which is
//why there is the exit clause. 
void SDL_quit_checker(SDL_Simplewin *sw)
{
	do{
		Neill_SDL_Events(sw);
	}while(!sw->finished);
	
}

//This is used in the above function when finding the entrance to check if the space is on the edge or not.
//The -1 from the maze_dims[col] is as array counting starts at zero but the scanned dimensions are counted from 1.
//Note that there is no -1 from the maze_dims[row] as there is an extra row where the dimensions were scanned in.
edges check_on_edge(int row_coord, int col_coord, int maze_dims[NUM_DIMENSIONS])
{
	if( row_coord == FIRST_ROW || row_coord == maze_dims[row] || col_coord == FIRST_COL || col_coord == maze_dims[col] - 1){
		return(on_edge);
	}
	else{
		return(not_on_edge);
	}

}								 

//This is the recursive function for actually solving the maze and is taken from the lectures.
possible_impossible solve_maze(player_maze maze, int maze_dims[NUM_DIMENSIONS], int row_coord, int col_coord, SDL_Simplewin *sw, int entrance_coords[NUM_DIMENSIONS])
{
	if(check_exit(maze, row_coord, col_coord, maze_dims, entrance_coords)){
		return(possible);
	}

	//Marks where we've been.
	maze[row_coord][col_coord] = PLACEHOLDER;

	//If we can go down (the last two arguments correspond to increments added onto the row and col coordinates).
	if(solve_maze_case(maze, maze_dims, row_coord, col_coord, 0, -1, sw, entrance_coords)){
		return(possible);
	}

	//If we can go right.
	if(solve_maze_case(maze, maze_dims, row_coord, col_coord, 1, 0, sw, entrance_coords)){
		return(possible);
	}

	//If we can go left.
	if(solve_maze_case(maze, maze_dims, row_coord, col_coord, -1, 0, sw, entrance_coords)){
		return(possible);
	}

	//If we can go up.
	if(solve_maze_case(maze, maze_dims, row_coord, col_coord, 0, 1, sw, entrance_coords)){
		return(possible);
	}

	return(impossible);
}

//This is used in the above solve_maze recursive function to scan through all of the different cases easily
//using different arguments. If it detects that the maze can be solved on this route then it assigns that
//part of the route to be part of the path and prints it. As it only detects it is part of the path after having
//found the exit this means that it is printed in reverse. 
possible_impossible solve_maze_case(player_maze maze, int maze_dims[NUM_DIMENSIONS], int row_coord, int col_coord, int i, int j, SDL_Simplewin *sw, int entrance_coords[NUM_DIMENSIONS])
{
	static int move_counter = 0;

	if(maze[row_coord + i][col_coord + j] == space){
		if(solve_maze(maze, maze_dims, row_coord + i, col_coord + j, sw, entrance_coords)){
			maze[row_coord][col_coord] = path;

			clear_screen();
			printf("\nComing to get you, move %d. Be patient!\n\n" 					\
			       "(Please note the path is being traced from exit to entrance).\n", ++move_counter); 
			print_maze(maze, maze_dims);
			SDL_print_maze(maze, maze_dims, sw);			

			return(possible);
		}
	}

	return(impossible);
}

//This is used to check if there is an exit for the maze. Overall the tactic is that if we have reached any edge coordinate then we must be at an exit. 
//The number of moves variable and the clause about the entrance_coordinates are just used to make sure that it doesn't return the entrance as an exit. 
//The -1 from maze_dims[col] is due to the array counting starting from zero but the scanned dimensions counting starting from 1.
possible_impossible check_exit(player_maze maze, int row_coord, int col_coord, int maze_dims[NUM_DIMENSIONS], int entrance_coords[NUM_DIMENSIONS])
{
	static int number_moves = 0;

	if( ( row_coord == FIRST_ROW || row_coord == maze_dims[row] ||
	      col_coord == FIRST_COL || col_coord == maze_dims[col] - 1) && number_moves > 0 && row_coord != 0 &&
	    !( row_coord == entrance_coords[row] && col_coord == entrance_coords[col] )){
		maze[row_coord][col_coord] = path;
		return(possible);
	}

	++number_moves;

	return(impossible);
}

//Used at the end of main to explain how the maze ended and whether or not there existed a solution.
//If SDL is on it waits until they close the window.
void end_action(player_maze maze, int maze_dims[NUM_DIMENSIONS], possible_impossible solveable_indicator, SDL_Simplewin *sw, SDL_on_off SDL_indicator)
{
	if(solveable_indicator == possible){

		clear_screen();
		printf("Finally, there you are... lets follow the path back! \n");
		print_maze(maze, maze_dims);
	}
	else{
		printf("\nSorry, I can't get to you! Looks like you're lost... FOREVER!\n\n");
	}

	SDL_print_maze(maze, maze_dims, sw);

	if(SDL_indicator == SDL_on){
		SDL_quit_checker(sw);
	}

}
