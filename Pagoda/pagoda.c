/*

Week 10
James Collerton - 46114
Conway's Soldiers

This is the second extended version of the Conway's soldier's game with slight tweaks to the algorithm for searching.

This version still does things like look for symmetric boards and hashing, but no longer keeps the same level search approach and
instead orders the board from the start according to the pagoda weighting described in the documentation.

*/

#include "pagoda_functions.h"

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
	
	weighting_board pagoda_board;

	SDL_Simplewin sw;

	check_unsigned_long_long();

	check_input_arguments(argc, argv, &row_coordinate, &col_coordinate, &success_index);

	initialise_player_board(&player_board);

	initialise_pagoda_board(pagoda_board, row_coordinate, col_coordinate);

	if(success_index != successful){
		final_board = make_a_move(row_coordinate, col_coordinate, &player_board, &player_board, &success_index, hashing_array, pagoda_board);
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
