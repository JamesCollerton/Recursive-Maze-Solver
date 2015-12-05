/*

Week 6
James Collerton (46114)
Maze

I attempted all of it.

Headers are found in maze_headers.h, functions for solving the maze are found in maze_functions.c and the random maze generation functions
are in random_maze_generation.c. Note that the neillsdl files have been altered slightly so the versions provided should be used.

If you save all of the files in a folder and compile using 'make all' the executable should be maze, which you then follow with your arguments.
Please note that the arguments are case sensitive and that any mismatched arguments will result in an error and you are returned to the terminal.
I kept them case sensitive because that matches what was specified in the brief.

Make the terminal box a little longer to get in all of the grid for bigger mazes.

The code should function as in the brief, however I have tweaked the algorithm for generating the random maze slightly because I thought the original
algorithm is designed for dividing grids with thin lines rather than for filling in spaces on the grid with blocks. This should be
explained in the code. Also I am not sure if the original method was designed for when there is an entrance and an exit, so I have added those
in too.

Another thing was the test mazes that we were given. I think when directly downloaded mazes 3 and 4 have strange end of line characters which my code picks up 
and so throws an error. However if you copy and paste in the code from the page to a .txt file and save it it works fine, so I don't know if it's a 
problem with the file or not. I asked Neill and he said I could include the versions of the mazes I have used to test it to prove it works just in case.

Finally, the animation for solving the maze is printed in reverse. I didn't think that it really mattered if it went from exit to entrance or vise
versa as it was never specified that an animation was needed and I thought it was nice to go into the maze from the other end and find the user 
to lead them out. I checked this and it was approved and the solved maze is still printed to the screen as required in the brief.

*/

#include "maze_headers.h"

//										MAIN

// First of all checks all the arguments entered with the command prompt, then fills the player maze and does some checking of the file.
// Next finds the entrance and looks to solve the maze, returning a suitable message and an indicator of possible or impossible depending on the result.
// If SDL has been detected to be on it is then closed.

int main(int argc, char *argv[])
{

	player_maze maze;
	SDL_on_off SDL_indicator = SDL_off; 
	possible_impossible solveable_indicator;
	int maze_dims[NUM_DIMENSIONS];
	int entrance_coords[NUM_DIMENSIONS];
	SDL_Simplewin sw;

	clear_screen();

	check_command_line_args(argc, argv, &SDL_indicator, maze, maze_dims, &sw);

	find_entrance(maze, maze_dims, entrance_coords, &sw, SDL_indicator);

	solveable_indicator = solve_maze(maze, maze_dims, entrance_coords[row], entrance_coords[col], &sw, entrance_coords);

	end_action(maze, maze_dims, solveable_indicator, &sw, SDL_indicator);

	if(SDL_indicator == SDL_on){
		// Clear up graphics subsystems
   		atexit(SDL_Quit);
	}

	return(solveable_indicator);

}
