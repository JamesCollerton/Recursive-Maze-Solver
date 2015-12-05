//										PREPROCESSING

#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include "neillsdl2.h"

#define MAX_DIM 21						//Maximum dimensions as specified in the problem plus one for when it is used with scanning in the dimensions row.
#define MIN_DIM 1						//The smallest sensible dimensions for a maze.
#define SENSIBLE_DIMS 9						//Used to make sure that the random grid is generated with sensible dimensions.
#define NUM_DIMENSIONS 2 					//The number of dimenstions of the grid.
#define PLACEHOLDER 0    					//This is used in solve_maze to prevent players retracing their steps.
#define FIRST_COL 0						//Used to index the first element of a column.
#define FIRST_ROW 1						//Index for the first element of a row and is set at one to stop any interaction with the row storing the dimensions.
#define TOO_FEW_ARGS 1						//The following four #defines are used in check_command_line_args.
#define POSS_FILENAME 2						
#define POSS_SDL 3
#define TOO_MANY_ARGS 3
#define CHECK_SUCCESS 0						//The following three #defines are used in clear_screen to monitor attempts to clear the screen.
#define NUM_CLEAR_ATTEMPT_WARN 10				
#define NUM_CLEAR_ATTEMPT 100
#define RECT_SIZE 20						//Used to create squares of pixels in SDL functions.
#define SCALE_CONST 25						//This is so that the original grid is scaled by a reasonable amount for SDL.
#define EXIT_CODE 2						//Used as a special exit code as 0 and 1 are used to see if the maze is possible or not.
#define GREEN_ONE 100						//The next three are used in SDL functions to change the colour to green.
#define GREEN_TWO 29
#define GREEN_THREE 240
#define DELAY 250 						//Time in ms for SDL_Delay
#define RAND_ROW_START 2					//These three are used in the random maze generation algorithm to pass the inside of the random grid to the algorithm.
#define RAND_COL_START 1
#define RAND_END 1
#define NO_ENTRANCE 0						//Next two used for checking the number of entrances to the maze.
#define NO_EXIT 1
#define BORDER 1						//To be added to the randomly generated dimensions of the grid to account for the extra walls added in.
#define ENTRANCE_COORD_COL 1					//These three are used to position the entrance and exit to the random maze.
#define EXIT_COORD_COL 1
#define EXIT_COORD_ROW 1

//										TYPEDEF, ENUMERATIONS AND STRUCTURES

enum space_type {wall = '#', space = ' ', path = '.'};
typedef enum space_type space_type;

enum row_or_col {col, row};
typedef enum row_or_col row_or_col;

enum possible_impossible {impossible, possible};
typedef enum possible_impossible possible_impossible;

enum edges {not_on_edge, on_edge};
typedef enum edges edges;

enum generate_maze {no_more_maze, more_maze};
typedef enum generate_maze generate_maze;

enum rand_selected {random_unselected, random_selected};
typedef enum rand_selected rand_selected;

enum SDL_on_off {SDL_on, SDL_off};
typedef enum SDL_on_off SDL_on_off;

typedef char player_maze[MAX_DIM][MAX_DIM];

//										FUNCTIONS

//										FUNCTIONS USED IN SOLVING

void clear_screen(void);

void check_command_line_args(int argc, char *argv[], SDL_on_off *SDL_indicator, player_maze maze, int maze_dims[NUM_DIMENSIONS], SDL_Simplewin *sw);

void SDL_check(char *SDL_Entry, SDL_on_off *SDL_indicator, SDL_Simplewin *sw);

void check_filename_copy_maze(char *filename, player_maze maze, int maze_dims[NUM_DIMENSIONS], SDL_Simplewin *sw);

void random_grid_procedure(player_maze maze, int maze_dims[NUM_DIMENSIONS], SDL_Simplewin *sw);

void initialise_maze(player_maze maze, int maze_dims[NUM_DIMENSIONS]);

void get_correct_maze_name(char *filename, FILE **maze_file, rand_selected *random_maze_indicator);

void get_maze_dims(FILE *maze_file, int maze_dims[NUM_DIMENSIONS]);

void check_scanned_dimensions(int maze_dims[NUM_DIMENSIONS]);

void copy_in_maze(FILE *maze_file, player_maze maze, int maze_dims[NUM_DIMENSIONS]);

void check_maze_contents(player_maze maze, int maze_dims[NUM_DIMENSIONS]);

void print_maze(player_maze maze, int maze_dims[NUM_DIMENSIONS]);

void SDL_print_maze(player_maze maze, int maze_dims[NUM_DIMENSIONS], SDL_Simplewin *sw);

void SDL_update(SDL_Simplewin *sw);

void find_entrance(player_maze maze, int maze_dims[NUM_DIMENSIONS], int entrance_coords[NUM_DIMENSIONS], SDL_Simplewin *sw, SDL_on_off SDL_indicator);

void number_of_entrances_checker(int entrance_counter, player_maze maze, int maze_dims[NUM_DIMENSIONS], SDL_Simplewin *sw, SDL_on_off SDL_indicator);

void SDL_quit_checker(SDL_Simplewin *sw);

edges check_on_edge(int row_coord, int col_coord, int maze_dims[NUM_DIMENSIONS]);		

possible_impossible solve_maze(player_maze maze, int maze_dims[NUM_DIMENSIONS], int row_coord, int col_coord, SDL_Simplewin *sw, int entrance_coords[NUM_DIMENSIONS]);

possible_impossible solve_maze_case(player_maze maze, int maze_dims[NUM_DIMENSIONS], int row_coord, int col_coord, int i, int j, SDL_Simplewin *sw, int entrance_coords[NUM_DIMENSIONS]);

possible_impossible check_exit(player_maze maze, int row_coord, int col_coord, int maze_dims[NUM_DIMENSIONS], int entrance_coords[NUM_DIMENSIONS]);

void end_action(player_maze maze, int maze_dims[NUM_DIMENSIONS], possible_impossible solveable_indicator, SDL_Simplewin *sw, SDL_on_off SDL_indicator);

//										FUNCTIONS USED IN RANDOM GRID GENERATION

void initialise_maze_rand(player_maze maze, int random_dimension);

generate_maze generate_random_maze(player_maze maze, int row_start, int row_end, int col_start, int col_end, SDL_Simplewin *sw, int random_dimension);

void initialise_and_print(player_maze maze, int row_start, int row_end, int col_start, int col_end, SDL_Simplewin *sw, int random_dimension, int *col_rand_point, int *row_rand_point);

generate_maze check_dimensions(int row_start, int row_end, int col_start, int col_end);

void fill_walls(player_maze maze, int col_start, int col_end, int row_start, int row_end, int *col_rand_point, int *row_rand_point);

void make_all_wall(int *row_rand_point, int *col_rand_point, int row_end, int row_start, int col_start, int col_end, player_maze maze);

void make_random_spaces(int col_end, int *col_rand_point, int *row_rand_point, int col_start, int row_end, player_maze maze);

void fill_surrounding_walls(player_maze maze, int random_dimension);
