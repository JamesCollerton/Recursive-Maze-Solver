#include "pagoda_functions.h"

// FUNCTION DEFINITIONS

//This checks to make sure segfaults will not occur with the use of the hash key.

void check_unsigned_long_long(void)
{
	if(sizeof(unsigned long long) < LINUX_ULL){
		clear_screen();
		fprintf(stderr, "\nYour system does not support sufficiently long integers for the hash key.\n");
	}
}

//This checks the input arguments to make sure there are enough and that they are what we expected.

void check_input_arguments(int argc, char *argv[], int *row_coordinate, int *col_coordinate, success_failure *success_index)
{
	//We need three arguments, the executable and two dimensions.
	if(argc == 1 || argc == 2){
		clear_screen();
		fprintf(stderr, "\nNot enough input arguments! Please try again.\n\n");
		exit(1);
	}
	if(argc == 3){

		check_row_col_coordinates(row_coordinate, col_coordinate, argv, success_index);

	}
	//Any more than three is too many and the user must be warned and the program quit.
	if(argc > 3){
		clear_screen();
		fprintf(stderr, "\nToo many arguments! Please try again.\n\n");
		exit(1);
	}
}

//Wrapper for clearing the screen.

void clear_screen(void)
{
	int clear_checker;

	clear_checker = system("clear");

	if(clear_checker != SUCCESS){
		fprintf(stderr, "\nThere was a problem clearing the screen.\n\n");
		exit(1);
	}
}

//Scans the inputted arguments for integers and makes sure that what has been given to the function is sensible.

void check_row_col_coordinates(int *row_coordinate, int *col_coordinate, char *argv[], success_failure *success_index)
{
	int scan_checker_one, scan_checker_two;

	scan_checker_one = sscanf(argv[1], "%d", col_coordinate);
	scan_checker_two = sscanf(argv[2], "%d", row_coordinate);

	if(scan_checker_one == 0 || scan_checker_two == 0){
		clear_screen();
		fprintf(stderr, "\nYour arguments did not contain integers! Please try again.\n\n");
		exit(1);
	}

	if(*row_coordinate < MIN_ROW || *row_coordinate > BOARD_H_NO_BORDER || *col_coordinate < MIN_COL || *col_coordinate > BOARD_W_NO_BORDER){
		clear_screen();
		fprintf(stderr, "\nInvalid peg coordinates! Please try again.\n\n");
		exit(1);
	}

	clear_screen();
	printf("\nYour chosen coordinates are: (%d, %d)\n", *col_coordinate, *row_coordinate);

	if( (MIN_ROW <= *row_coordinate && *row_coordinate <= BOARD_H_NO_BORDER / 2) ){
		printf("\n\nWe started at these coordinates!\n\n");
		*success_index = successful;
	}

}

//Used to initialise the board structure as handed to the function. Puts pegs in the upper part
//spaces in the bottom part and a border around the edge.

void initialise_player_board(board_struct *player_board)
{
	int i, j;

	for(i = 0; i < BOARD_H; i++){
		for(j = 0; j < BOARD_W; j++){
			if(i == 0 || j == 0 || i == BOARD_H - BORDER || j == BOARD_W - BORDER){
				player_board->board_array[i][j] = border;
			}
			else if(i < BOARD_H / 2){
				player_board->board_array[i][j] = peg;
			}
			else{
				player_board->board_array[i][j] = space;
			}
		}
	}

	player_board->last_board = NULL;
	player_board->parent_board = NULL;
	player_board->solution_board = NULL;
	player_board->all_moves_made = no;
	player_board->number_pegs = STARTING_PEGS;
	player_board->unique_hashing_number = 0;
	player_board->unique_symmetric_hashing_number = 0;
	player_board->modulus_hashing_number = 0;
	player_board->modulus_symmetric_hashing_number = 0;
	player_board->pagoda_weighting = 0;

}

//This is used in the pagoda weighted version to give an idea of how 'good' a board is. (Description of weighting in .pdf)

void initialise_pagoda_board(weighting_board pagoda_board, int row_coordinate, int col_coordinate)
{
	int i, j;

	for(i = BOARD_H - BORDER; i >= 0; --i){
		for(j = BOARD_W - BORDER; j >= 0; --j){
			if(i > row_coordinate){
				pagoda_board[i][j] = 0;
			}
			else if(i == row_coordinate){
				pagoda_board[i][j] = abs(j - col_coordinate);
			}
			else if(j == col_coordinate){
				pagoda_board[i][j] = abs(i - row_coordinate);
			}
			else{
				pagoda_board[i][j] = pagoda_board[i + 1][j] + 1; 						//Elements in the row below are worth the element above + 1
			}
		}
	}

}

//Used when we know that the maze still hasn't been solved yet. It checks the board contents to see if any moves can be made
//passing back the last element in the huge linked list structure we're creating to be used in the next iteration. Then it
//searches for a fresh board to start making moves from. After it has done the search it checks to see if we have succeeded having
//looked at the previous board, if so prints a message. If the uninvestigated board is NULL then we have investigated all boards
//and still not found a solution. Otherwise we continue to search.

board_struct *make_a_move(int row_coordinate, int col_coordinate, board_struct *player_board, board_struct *current_board, 
			  success_failure *success_index, full_empty hashing_array[VERY_LARGE_NUMBER], weighting_board pagoda_board)
{

	board_struct *uninvestigated_board;

	current_board = check_board_contents(current_board, player_board, success_index, row_coordinate, col_coordinate, hashing_array, pagoda_board);

	uninvestigated_board = search_for_fresh_board(current_board);

	if(*success_index == successful || *success_index == symmetric_successful){
		printf("\n\nWe found it!\n\n");
	}
	else if(uninvestigated_board == NULL){
		clear_screen();
		printf("\n\nWe couldn't do it... I'm sorry!\n\n");
	}
	else{
		current_board = make_a_move(row_coordinate, col_coordinate, uninvestigated_board, current_board, success_index, hashing_array, pagoda_board);
	}

	return(current_board);
}

//This scans back through all of the boards and looks for the first board where the board that comes BEFORE it in the queue
//has had all the moves made off it checked and that board has not. If this is true then this is the board that was created
//earliest that has not had all of its possible moves checked. When there comes a point where this has not happened and the
//board before the one we are examining is empty then we have examined all possible moves.

board_struct *search_for_fresh_board(board_struct *current_board)
{
	if(current_board->last_board != NULL){
		if(current_board->last_board->all_moves_made == yes && current_board->all_moves_made == no){
			return(current_board);
		}
		else{
			return(search_for_fresh_board(current_board->last_board));
		}
	}
	else{
		return(NULL);
	}
}

//This attempts to move pegs down, left and right around the board. If it detects that we have found the solution earlier
//it skips everything in the for loops and moves on (without using a break). Once it has gone through and done all of the
//possible moves it signals this in the board structure and returns the current board so more boards can be added to the
//end of the list.

//Again here we make the fair assumption that you never move away from the target board.

board_struct *check_board_contents(board_struct *current_board, board_struct *player_board, success_failure *success_index,
				   int row_coordinate, int col_coordinate, full_empty hashing_array[VERY_LARGE_NUMBER], weighting_board pagoda_board)
{
	int i, j;

	for(i = BORDER; i < BOARD_H; ++i){
		for(j = BORDER; j < BOARD_W; ++j){
			if(player_board->board_array[i][j] == peg && *success_index == unsuccessful){

				//Move down (note the last two arguments are increments added to the row and col coordinates).
				current_board = attempt_to_move(player_board, current_board, success_index, row_coordinate, col_coordinate, 
												hashing_array, pagoda_board, i, j, 1, 0);

				//Move left.
				current_board = attempt_to_move(player_board, current_board, success_index, row_coordinate, col_coordinate, 
												hashing_array, pagoda_board, i, j, 0, -1);

				//Move right.
				current_board = attempt_to_move(player_board, current_board, success_index, row_coordinate, col_coordinate, 
												hashing_array, pagoda_board, i, j, 0, 1);
			}
		}
	}

	player_board->all_moves_made = yes;

	return(current_board);
}

//This was just a nice way of trying all the different move combinations. 'Increments' are passed into the function and added
//onto the current row and column to look for moves. If a move is possible then it generates the board, checks to see if that
//is the successful board and counts the number of pegs in the board. Finally it checks if that board is already in the list,
//if it is then the board is discarded, otherwise it is added onto the list as well.

//The hashing and board checking works the same as in the previous extended version.

board_struct *attempt_to_move(board_struct *parent_board, board_struct *current_board, success_failure *success_index, 
			      int row_coordinate, int col_coordinate, full_empty hashing_array[VERY_LARGE_NUMBER], weighting_board pagoda_board,
			      int row, int col, int row_incr, int col_incr)
{

	board_struct *new_board;

	//The two is as the space two away from the peg in the direction we wish to move must be empty in order to put a peg in.
	if(parent_board->board_array[row + row_incr][col + col_incr] == peg && 
	   parent_board->board_array[row + 2 * row_incr][col + 2 * col_incr] == space){

	   	new_board = create_new_board_struct();

		copy_in_parent_board(new_board, parent_board);

		generate_board(new_board, parent_board, current_board, pagoda_board, row, col, row_incr, col_incr);

		check_for_success(new_board, row_coordinate, col_coordinate, success_index);

		count_board_pegs(new_board);

		generate_hash_string(new_board);

		if( check_modulus_hash_number(new_board, hashing_array) == seen_before){

			if( check_unique_hash_number(new_board, current_board, new_board->number_pegs) == seen_before){

						free(new_board);

						return(current_board);

			}

		}

		//This if is used to avoid complexities that only occur in the first set of boards and makes no difference as there are only a small
		//number to go through.
		if(new_board->number_pegs < MIN_NUM_PEGS){

			print_messages(new_board->number_pegs, success_index);

			current_board = assign_place_in_queue(new_board, current_board, on);

			return(current_board);

		}
		else{

			return(new_board);

		}

	}

	return(current_board);

}

//Used to allocate memory for the new board structure and to set up all the components of the structure.

board_struct *create_new_board_struct(void)
{
	board_struct *temp;

	temp = (board_struct *)malloc(sizeof(board_struct));

	if(temp == NULL){
		clear_screen();
		fprintf(stderr, "\n\nCannot allocate new board structure.\n\n");
		exit(1);
	}

	temp->parent_board = NULL;
	temp->last_board = NULL;
	temp->solution_board = NULL;
	temp->all_moves_made = no;
	temp->number_pegs = 0;
	temp->unique_hashing_number = 0;
	temp->unique_symmetric_hashing_number = 0;
	temp->modulus_hashing_number = 0;
	temp->modulus_symmetric_hashing_number = 0;
	temp->pagoda_weighting = 0;

	return(temp);
}

//This copies in the parent board to the prospective new board for moves to be tried.

void copy_in_parent_board(board_struct *new_board, board_struct *parent_board)
{
	int i, j;

	for(i = 0; i < BOARD_H; ++i){
		for(j = 0; j < BOARD_W; ++j){

			new_board->board_array[i][j] = parent_board->board_array[i][j];

		}
	}

}

//This is used to actually make the move and change what has occurred on the board.

void generate_board(board_struct *new_board, board_struct *parent_board, board_struct *current_board, weighting_board pagoda_board, 
					int row, int col, int row_incr, int col_incr)
{

	//The two is as the space two away from the peg in the direction we wish to move must be empty in order to put a peg in.
	new_board->board_array[row][col] = space;
	new_board->board_array[row + row_incr][col + col_incr] = space;
	new_board->board_array[row + 2 * row_incr][col + 2 * col_incr] = peg;

	create_board_weighting(new_board, pagoda_board);

	new_board->parent_board = parent_board;
	new_board->last_board = current_board;
	new_board->all_moves_made = no;
	new_board->solution_board = NULL;

}

//Creates a weighting for the board based on the pagoda system.

void create_board_weighting(board_struct *new_board, weighting_board pagoda_board)
{
	int i, j;

	for(i = 0; i < BOARD_H; ++i){
		for(j = 0; j < BOARD_W; ++j){
			if(new_board->board_array[i][j] == peg){
				new_board->pagoda_weighting += pow(PAGODA_CONST, pagoda_board[i][j]); 
			}
		}
	}

}

//Checks to see if the current board has a peg at the solution space the same as in the previous extended version.

void check_for_success(board_struct *new_board, int row_coordinate, int col_coordinate, success_failure *success_index)
{
	if(new_board->board_array[row_coordinate][col_coordinate] == peg){
		*success_index = successful;
		link_successful_boards(new_board);
	}
	else if(new_board->board_array[row_coordinate][BOARD_W - BORDER - col_coordinate] == peg){
		*success_index = symmetric_successful;
		link_successful_boards(new_board);
	}
}

//If there has been a success then we go back up all of the parent boards and link them forward to create a
//linked list containing the solution.

void link_successful_boards(board_struct *successful_board)
{
	if(successful_board->parent_board != NULL){
		successful_board->parent_board->solution_board = successful_board;
		link_successful_boards(successful_board->parent_board);
	}
}

//Counts all the pegs in the current board.

void count_board_pegs(board_struct *current_board)
{
	int i, j;

	for(i = 0; i < BOARD_H; ++i){
		for(j = 0; j < BOARD_W; ++j){
			if(current_board->board_array[i][j] == peg){
				++current_board->number_pegs;
			}
		}
	}

}

//A function used to generate the hash string for the board. It generates the first half and then the symmetric component.
//Finally it generates the modulus numbers as described earlier.

void generate_hash_string(board_struct *new_board)
{
	unsigned long long current_power_of_ten;

	current_power_of_ten = generate_first_half(new_board);

	generate_second_half_and_symmetric(new_board, current_power_of_ten);

	new_board->modulus_hashing_number = new_board->unique_hashing_number % VERY_LARGE_NUMBER;
	new_board->modulus_symmetric_hashing_number = new_board->unique_symmetric_hashing_number % VERY_LARGE_NUMBER;

}

//Generates the first half of the hash string by going through the board and adding up the number of pegs on each row.
//Each digit of the hash string is then described by that counter.

unsigned long long generate_first_half(board_struct *new_board)
{
	int i, j; 
	unsigned long long counter = 0, power_of_ten = 1;

	new_board->unique_hashing_number = 0;

	for(i = BORDER; i < BOARD_H - BORDER; ++i){
		for(j = BORDER; j < BOARD_W; ++j){
			if(new_board->board_array[i][j] == peg){
				++counter;
			}
		}
		new_board->unique_hashing_number += counter * power_of_ten;
		new_board->unique_symmetric_hashing_number += counter * power_of_ten;
		power_of_ten *= 10; 														//To move the counter digit up by one place each time.
		counter = 0;
	}

	return(power_of_ten);
}

//Does the same as the above but for the second half of the string and also calculates the symmetric string.

void generate_second_half_and_symmetric(board_struct *new_board, unsigned long long current_power_of_ten)
{
	int i, j; 
	unsigned long long counter = 0, symmetric_counter = 0, temp;
	
	for(j = BORDER; j < BOARD_W; ++j){
		for(i = BORDER; i < BOARD_H - BORDER; ++i){

			temp = BOARD_W - BORDER - j;

			if(new_board->board_array[i][j] == peg){
				++counter;
			}
			if(new_board->board_array[i][temp] == peg){
				++symmetric_counter;
			}
		}
		new_board->unique_hashing_number += counter * current_power_of_ten;
		new_board->unique_symmetric_hashing_number += symmetric_counter * current_power_of_ten;
		current_power_of_ten *= 10;													//To move the counter digit up by one place each time.
		counter = 0;
		symmetric_counter = 0;
	}
}

//The hashing number is modulused by a very large number to fit into a suitable array. This smaller number is then
//checked against the index of the array to see if there is anything in it. If there is then we may have seen this 
//board before and so further checks should be made. If there isn't then this board definitely hasn't been seen and
//we can put it in.

contained_indicator check_modulus_hash_number(board_struct *new_board, full_empty hashing_array[VERY_LARGE_NUMBER])
{

	if(hashing_array[new_board->modulus_hashing_number] || hashing_array[new_board->modulus_symmetric_hashing_number]){
		return(seen_before);	
	}
	else{
		hashing_array[new_board->modulus_hashing_number] = full;
		return(unseen);
	}

	return(seen_before);
}

//If we see from the modulus number that we might have seen the board before we carry out more comprehensive checks.
//We go through the list and check all the unique hash numbers. If we come across one that is supposedly seen before
//then we check to make sure that is true. Otherwise if it comes back as unseen then we leave it.

contained_indicator check_unique_hash_number(board_struct *new_board, board_struct *current_board, int last_peg_number)
{
	contained_indicator indicator = unseen, indicator_one = seen_before, indicator_two = seen_before;

	if(current_board != NULL){

		if( (new_board->unique_hashing_number == current_board->unique_hashing_number) ||
			(new_board->unique_symmetric_hashing_number == current_board->unique_hashing_number) ){

			indicator_one = same_board_checker(new_board, current_board);

			indicator_two = symmetric_board_checker(new_board, current_board);

			if(indicator_one == seen_before || indicator_two == seen_before){
				return(seen_before);
			}

		}

		if(indicator == unseen){
			indicator = check_unique_hash_number(new_board, current_board->last_board, current_board->number_pegs);
		}

	}

	return(indicator);
}

//Checks to see if two boards are the same.

contained_indicator same_board_checker(board_struct *new_board, board_struct *current_board)
{
	int i, j;

	for(i = 0; i < BOARD_H; ++i){
			for(j = 0; j < BOARD_W; ++j){
				if(new_board->board_array[i][j] != current_board->board_array[i][j]){
					return(unseen);
			}
		}
	}

	return(seen_before);

}

//Checks to see if two boards are symmetric.

contained_indicator symmetric_board_checker(board_struct *new_board, board_struct *current_board)
{
	int i, j;

	for(i = 0; i < BOARD_H; ++i){
		for(j = BORDER; j < BOARD_W - BORDER; ++j){
			if(new_board->board_array[i][BOARD_W - j] != current_board->board_array[i][j]){
				return(unseen);
			}
		}
	}

	return(seen_before);
}

//Used to print messages to the screen so the user knows that something is happening and the program
//hasn't crashed. It only prints out every 100 boards or on the final board so that too much printing
//is avoided.

void print_messages(int new_board_peg_num, success_failure *success_index)
{
	static int counter = 1;

	if(counter % NUM_CLEARS == 0 || *success_index == successful){

		clear_screen();

		printf("\nCurrently on unique board: %d", counter);
		printf("\nNumber of pegs left on boards: %d\n", new_board_peg_num);
	}
	
	++counter;

}

//This is used to assign a board a place in the queue based on its pagoda weighting. First of all it check to
//see if this is the first time someone has tried to put it in. If it is and the weighting is lower than the current
//weighting or the board at the end is the parent board it sticks it on the end. Otherwise it goes and finds a place
//for it to be inserted in the queue, making sure to put it after its parent board.

board_struct *assign_place_in_queue(board_struct *new_board, board_struct *current_board, on_off first_pass_checker)
{
	if(current_board != NULL && current_board->last_board != NULL && new_board->parent_board != NULL){
		if(first_pass_checker == on){

		 	if(new_board->pagoda_weighting < current_board->pagoda_weighting || current_board->all_moves_made == yes
		 		|| current_board == new_board->parent_board){

		 		new_board->last_board = current_board;
		 		return(new_board);

		 	}
		 	else{

		 		assign_place_in_queue(new_board, current_board->last_board, off);

		 	}
		}
		else{

			if(current_board->last_board->all_moves_made == yes || current_board->last_board->pagoda_weighting > new_board->pagoda_weighting
				||current_board->last_board == new_board->parent_board){

				new_board->last_board = current_board->last_board;
				current_board->last_board = new_board;
				return(current_board);

			}
			else{

				assign_place_in_queue(new_board, current_board->last_board, off);

			}
		}
	}

	return(current_board);
}

//Used to print the time taken to find the solution.

void print_time_taken(clock_t start)
{
	clock_t diff;
	int msec;

	//1000 as there are 1000ms in a s.
	diff = clock() - start;
	msec = diff * 1000 / CLOCKS_PER_SEC;
	printf("\nTime taken: %d seconds %d milliseconds\n\n", msec/1000, msec%1000);
}

//This is used to print the successful boards using the solution pointers. It prints two boards per movement,
//one showing the movement and one showing the position afterwards. The counter is there for the reason described previously

void print_successful_boards(board_struct *current_board, SDL_Simplewin *sw, int row_coordinate, int col_coordinate,
							 success_failure success_index)
{
	static int counter = 1;

	if(current_board != NULL && sw->finished != 1){

		if(counter++ != 1){
			print_board_SDL(current_board, sw, off, row_coordinate, col_coordinate, success_index);
		}

		print_board_SDL(current_board, sw, on, row_coordinate, col_coordinate, success_index);
		print_successful_boards(current_board->solution_board, sw, row_coordinate, col_coordinate, success_index);
	}
}

//This is used to print the boards through SDL and works as in the previous extended version.

void print_board_SDL(board_struct *current_board, SDL_Simplewin *sw, on_off intermediate_indicator,
					 int row_coordinate, int col_coordinate, success_failure success_index)
{
	int i, j, temp_x, temp_y;
	SDL_Rect rectangle;
	colour_struct colour_range;

   	rectangle.w = RECT_SIZE;
   	rectangle.h = RECT_SIZE;

   	sw->skip_checker = off;

   	for(i = 0; i < BOARD_H; ++i){
   		for(j = 0; j < BOARD_W; ++j){

   			if(success_index == symmetric_successful){

   				temp_x = BOARD_W - BORDER - j;
   				rectangle.x = temp_x * SCALE_FACT + (WWIDTH - SCALE_FACT * BOARD_W) / 2;
   				colour_chooser(current_board, i, j, &colour_range, intermediate_indicator, row_coordinate, BOARD_W - BORDER - col_coordinate);

   			}
   			else{

   				rectangle.x = j * SCALE_FACT + (WWIDTH - SCALE_FACT * BOARD_W) / 2;
   				colour_chooser(current_board, i, j, &colour_range, intermediate_indicator, row_coordinate, col_coordinate);

   			}

   			temp_y = BOARD_H - BORDER - i;
   			rectangle.y = temp_y * SCALE_FACT + (WHEIGHT - SCALE_FACT * BOARD_H) / 2;						//For centering.

   			Neill_SDL_SetDrawColour(sw, colour_range.colour_r, colour_range.colour_g, colour_range.colour_b);
   			SDL_RenderFillRect(sw->renderer, &rectangle);
   		}
   	}

   	SDL_RenderPresent(sw->renderer);
    	SDL_UpdateWindowSurface(sw->win);
    
    	do{
    		Neill_SDL_Events(sw);
    	}while(sw->skip_checker != on && sw->finished != 1);

}

//This chooses colours depending on whether we are showing a move or showing a board state.
//Looks a little long but it essentially just cycles through all of the possibilities and assigns a
//different colour dependent.

void colour_chooser(board_struct *current_board, int i, int j, colour_struct *colour_range, on_off intermediate_index, int row_coordinate, int col_coordinate)
{
	if(intermediate_index == off){

		//Pegs that have moved to a new space are shown in green.
		if(current_board->board_array[i][j] == peg &&
	   	   current_board->parent_board != NULL &&
	   	   current_board->parent_board->board_array[i][j] == space){

	   	   		colour_setter(colour_range, green_one, green_two, green_three);

	   	}
	   	//Unmoved pegs are shown in white.
	   	else if(current_board->board_array[i][j] == peg){

	   			colour_setter(colour_range, white, white, white);

	   	}
	   	//Pegs that have moved or been taken off are shown in red.
	   	else if(current_board->board_array[i][j] == space &&
	   			current_board->parent_board != NULL &&
	   			current_board->parent_board->board_array[i][j] == peg ){

	   			colour_setter(colour_range, red_one, black, red_three);

	   	}
	   	else{

	   			colour_setter(colour_range, black, black, black);

	   	}
	}
	else{

		//The peg in the correct position is shown in purple.
		if(current_board->board_array[i][j] == peg){
			if(i == row_coordinate && j == col_coordinate){

				colour_setter(colour_range, red_one, green_one, white);

			}
			//Other pegs in this intermediate stage are shown in white.
			else{

				colour_setter(colour_range, white, white, white);

			}

   		}
   		else{

   			colour_setter(colour_range, black, black, black);

   		}
	}
}

//Small wrapper for setting colours depending on what their type is.

void colour_setter(colour_struct *colour_range, colours r, colours g, colours b)
{
	colour_range->colour_r = r;
	colour_range->colour_g = g;
	colour_range->colour_b = b;
}

//The final thing that the program does is to free all the board structures. Note the last
//board doesn't need to be freed as it wasn't malloced.

void free_all_boards(board_struct *current_board)
{
	board_struct *temp;

	while(current_board->last_board != NULL && current_board != NULL){
		temp = current_board->last_board;
		free(current_board);
		current_board = temp;
	}

}
