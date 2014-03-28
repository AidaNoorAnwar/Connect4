#include <stdlib.h>
#include <stdio.h>
#include <limits.h>

/* Grid Dimension */
#define ROWS 6
#define COLS 7

/* Constant Declaration */
static const int playerOne=1;
static const int playerTwo=-1;
static const int noPlayer=0;
static const int newgame=0;
static const int ongoing=1;
static const int complete=-1;

/* Global declaration for current game status and selected difficulty level */
int gameDifficultyLevel;
int gameStatus;

typedef struct point pointType;
typedef struct board boardType;

/* game function initialization */
void set_point(pointType * grid,int yAxis, int xAxis);
void delete_Point(pointType * p);
void set_state(pointType * p, int player);
int get_state(pointType * p);
pointType *** generate_possible_lines(pointType ** grid, int lineSize);
void init_board(boardType *b);
boardType * create_board();
void delete_board(boardType * b);
int valid_move(boardType * b, int input);
int valid_moves_left(boardType * b);
void make_move(boardType * b, int input);
void undo_move(boardType * b);
int winner_is(boardType * b);
int get_current_player(boardType * b);
int get_high_difficulty_move(boardType *b);
int get_medium_difficulty_move(boardType *b);
int get_easy_difficulty_move(boardType *b);


/** structure point
 *
 * point is for set grid to contain
 * x=coordinate x grid
 * y=cordinate y grid
 * state=which player drop the coin
 *
 */

struct point{
    int x, y, state;
};

/** structure board
 *
 * grid = to create board game
 * heights = to contain latest board height
 * moves = to contain remaining moves
 * currentPlayer = to contain id of the current player
 * lastMove = contain previous move
 * lineSize = total of possible line combination player can win
 * possibleLines = possible line map with grid
 *
 */

struct board{
    pointType **grid;
    int * heights;
    int * moves;
    int currentPlayer;
    int lastMove;
    int lineSize;
    pointType *** possibleLines;
};
