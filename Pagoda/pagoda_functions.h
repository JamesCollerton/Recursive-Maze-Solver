// 							PREPROCESSING

#include<stdio.h>
#include<stdlib.h>
#include<math.h>
#include <time.h>
#include "neillsdl2.h"

#define LINUX_ULL 8 							//Used to check that ULL is long enough to store the hash key
#define BOARD_W 9							//Note that an extra two has been added to the width and height to create a border.
#define BOARD_H 10
#define BOARD_H_NO_BORDER 8 						//Used when we want to think about the dimensions without a border. 
#define BOARD_W_NO_BORDER 7
#define BORDER 1
#define SUCCESS 0 							//For seeing if functions have been successful.
#define MIN_ROW 1
#define MIN_COL 1
#define STARTING_PEGS 28 						//Num of pegs in the initial board.
#define MIN_NUM_PEGS 27 						//For in the pagoda board insertion.
#define NUM_CLEARS 100 							//Number of unique boards before a new message is printed.
#define SCALE_FACT 100							//Scaling factor for the SDL work.
#define RECT_SIZE 80							//Used to define rectangle components for the SDL.
#define VERY_LARGE_NUMBER 1000000					//Used to create a hashing array.
#define PAGODA_CONST 0.618						//From the pagoda weighting formula


// 							TYPEDEFS AND ENUMERATIONS

enum space_types {border = '.', space = ' ', peg = 'x'};
typedef enum space_types space_types;

enum yes_no {yes, no};
typedef enum yes_no yes_no;

enum success_failure {successful, symmetric_successful, unsuccessful};
typedef enum success_failure success_failure;

enum contained_indicator {unseen, seen_before};
typedef enum contained_indicator contained_indicator;

enum on_off {off, on};
typedef enum on_off on_off;

enum full_empty {empty, full};
typedef enum full_empty full_empty;

enum colours {white = 255, green_one = 100, green_two = 200, green_three = 10, red_one = 200, black = 0, red_three = 20};
typedef enum colours colours;

typedef space_types board[BOARD_H][BOARD_W];
typedef int weighting_board[BOARD_H][BOARD_W];

struct board_struct{
	board board_array;
	struct board_struct *parent_board;
	struct board_struct *last_board;
	struct board_struct *solution_board;
	yes_no all_moves_made;
	int number_pegs;
	double pagoda_weighting;
	unsigned long long unique_hashing_number;
	unsigned long long modulus_hashing_number;
	unsigned long long unique_symmetric_hashing_number;
	unsigned long long modulus_symmetric_hashing_number;
};
typedef struct board_struct board_struct;

struct colour_struct{
	Uint8 colour_r; 
	Uint8 colour_g; 
	Uint8 colour_b;
};
typedef struct colour_struct colour_struct;	

// 							FUNCTIONS

void check_unsigned_long_long(void);

void check_input_arguments(int argc, char *argv[], int *row_coordinate, int *col_coordinate, success_failure *success_index);

void clear_screen(void);

void check_row_col_coordinates(int *row_coordinate, int *col_coordinate, char *argv[], success_failure *success_index);

void initialise_player_board(board_struct *player_board);

void initialise_pagoda_board(weighting_board pagoda_board, int row_coordinate, int col_coordinate);

board_struct *make_a_move(int row_coordinate, int col_coordinate, board_struct *player_board, board_struct *current_board, 
						  success_failure *success_index, full_empty hashing_array[VERY_LARGE_NUMBER], weighting_board pagoda_board);

board_struct *search_for_fresh_board(board_struct *current_board);

board_struct *check_board_contents(board_struct *current_board, board_struct *player_board, success_failure *success_index,
								   int row_coordinate, int col_coordinate, full_empty hashing_array[VERY_LARGE_NUMBER], weighting_board pagoda_board);

board_struct *attempt_to_move(board_struct *parent_board, board_struct *current_board, success_failure *success_index, 
							  int row_coordinate, int col_coordinate, full_empty hashing_array[VERY_LARGE_NUMBER], weighting_board pagoda_board,
							  int row, int col, int row_incr, int col_incr);

board_struct *create_new_board_struct(void);

void copy_in_parent_board(board_struct *new_board, board_struct *parent_board);

void generate_board(board_struct *new_board, board_struct *parent_board, board_struct *current_board, weighting_board pagoda_board, 
					int row, int col, int row_incr, int col_incr);

void create_board_weighting(board_struct *new_board, weighting_board pagoda_board);

void check_for_success(board_struct *new_board, int row_coordinate, int col_coordinate, success_failure *success_index);

void link_successful_boards(board_struct *new_board);

void count_board_pegs(board_struct *current_board);

void generate_hash_string(board_struct *new_board);

unsigned long long generate_first_half(board_struct *new_board);

void generate_second_half_and_symmetric(board_struct *new_board, unsigned long long current_power_of_ten);

contained_indicator check_modulus_hash_number(board_struct *new_board, full_empty hashing_array[VERY_LARGE_NUMBER]);

contained_indicator check_unique_hash_number(board_struct *new_board, board_struct *current_board, int last_peg_number);

contained_indicator same_board_checker(board_struct *new_board, board_struct *current_board);

contained_indicator symmetric_board_checker(board_struct *new_board, board_struct *current_board);

void print_messages(int new_board_peg_num, success_failure *success_index);

board_struct *assign_place_in_queue(board_struct *new_board, board_struct *current_board, on_off first_pass_checker);

void print_time_taken(clock_t start);

void print_successful_boards(board_struct *current_board, SDL_Simplewin *sw, int row_coordinate, int col_coordinate,
							 success_failure success_index);

void print_board_SDL(board_struct *current_board, SDL_Simplewin *sw, on_off intermediate_indicator,
					 int row_coordinate, int col_coordinate, success_failure success_index);

void colour_chooser(board_struct *current_board, int i, int j, colour_struct *colour_range, on_off intermediate_index, int row_coordinate, int col_coordinate);

void colour_setter(colour_struct *colour_range, colours r, colours g, colours b);

void free_all_boards(board_struct *current_board);
