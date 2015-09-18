/*

Week 10
James Collerton - 46114
Conway's Soldiers

This is the first extended version of the Conway's soldier's game with tweaks to the algorithm for searching.

First of all the algorithm realises that the board is symmetric and so only looks for solutions on one side of the
board, mirroring them to the correct coordinates if necessary. Secondly the algorithm does not search through all of the
boards for identical boards, it only searches them until the point where the previous generation of parent boards begins.

It does some work about hashing as described in the documentation. I tried a lot of different things and they can all
be found in the pdf.

*/

#include "extended_functions.h"

//MAIN

//Does error checking on all of the command line arguments. Initialises the first board for the game including a border. 
//The border is so that the algorithm can look outside of the player's board part of the array and not access junk. 
//Then if the user has not given the game some coordinates that are already inhabited by pegs the game looks to find a 
//way of reaching the coordinates. Finally it prints off the solution boards and frees the memory.

int main(int argc, char *argv[])
{
	board_struct player_board;
	board_struct *final_board = NULL;

	full_empty hashing_array[VERY_LARGE_NUMBER] = {empty};

	success_failure success_index = unsuccessful;
	int row_coordinate, col_coordinate;
	clock_t start = clock();

	SDL_Simplewin sw;

	check_unsigned_long_long();

	check_input_arguments(argc, argv, &row_coordinate, &col_coordinate, &success_index);

	initialise_player_board(&player_board);

	if(success_index != successful){
		final_board = make_a_move(row_coordinate, col_coordinate, &player_board, &player_board, &success_index, hashing_array);
	}
	else{
		final_board = &player_board;
	}

	print_time_taken(start);

	Neill_SDL_Init(&sw);

	if(success_index == successful || success_index == symmetric_successful){
		print_successful_boards(&player_board, &sw, row_coordinate, col_coordinate, success_index);
	}

	free_all_boards(final_board);

	atexit(SDL_Quit);

	return(0);
}
