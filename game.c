/** This file contain functionality of connect4
 *  The file originated from main.c
 *  Only the required functions remains here
 *  All other declaration functions,variables,structures were moved to game.h
 *
 *  Original existing functions: New function name
 *  point_type * newPoint(int a, int b)>> void set_point(pointType * grid,int yAxis, int xAxis);
 *  void deletepoint(point_type* p)>> void delete_Point(pointType * p);
 *  void setState(point_type * a, int player) >> void set_state(pointType * p, int player);
 *  int getState(point_type * a) >> int get_state(pointType * p)
 *  point_type*** generateCL(point_type *** grid)
 *           >> pointType *** generate_possible_lines(pointType ** grid, int lineSize);
 *  board_type * createBoard(int a, int b)
 *  void deleteboard(board_type* p) >> void delete_board(boardType * b)
 *  int validMove(board_type * b, int column) >> int valid_move(boardType * b, int input);
 *  void makeMove(board_type * b, int column) >> void make_move(boardType * b, int input);
 *  void undoMove(board_type * b) >> function still exist but commented out
 *  int validMovesLeft(board_type * b) >> int valid_moves_left(boardType * b);
 *  int winnerIs(board_type * b) >> int winner_is(boardType * b);
 *  int getRandomPlayerMove(board_type *b) >> int get_easy_difficulty_move(boardType *b);
 *
 *
 *  Removed functions:
 *  int equalsPosition(point_type * a, point_type* b)
 *  int getScore(point_type * points[])
 *  point_type * newPoint(int a, int b)
 *  char * toString(board_type * b)
 *  int getStrength(board_type * b)
 *  int getReasonedMove(board_type * cB)
 *  int minValue(board_type * cB, int ply)
 *  int maxValue(board_type * cB, int ply)
 *
 *  Altered functions:
 *  -Remaining functions name has been altered to suit naming convention
 *  -refer to the original function and the new function name
 *
 *  New functions added:
 *  int get_current_player(boardType * b);
 *  int get_high_difficulty_move(boardType *b);
 *  int get_medium_difficulty_move(boardType *b);
 *  int get_easy_difficulty_move(boardType *b);
 *
 *
 *
 *
 */

#include "game.h"


/* initialize structure pointType in grid */
void set_point(pointType * grid,int yAxis, int xAxis){
    grid->y=yAxis;
    grid->x=xAxis;
    set_state(grid,noPlayer);
}

/* set player's status in the grid */
void set_state(pointType * p, int player){
    p->state=player;
}

/* return player's status in the grid*/
int get_state(pointType * p){
    return p->state;
}

/* function to map possible lines to grid */
pointType *** generate_possible_lines(pointType ** grid, int lineSize){
	int i,t;
	int y,x;
	int count=0;
	pointType *** lines = (pointType ***)malloc(lineSize*sizeof(pointType **));
	for(i = 0; i < lineSize; i++){
		lines[i] = (pointType **)malloc(4 * sizeof(pointType *));
	}

    /* mapping possible horizontal lines*/
    for(y=ROWS-1;y>-1;y--){
		for( x=0;x<COLS-3;x++){
			for(i=x;i<x+4;i++){
				lines[count][i-x]=&grid[y][i];
			}count++;
		}
	}

    /* mapping possible vertical lines*/
	for( x=COLS-1;x>-1;x--){
		for( y=0;y<ROWS-3;y++){
			for( i=y;i<y+4;i++){
				lines[count][i-y]=&grid[i][x];
			}count++;
		}
	}

    /* mapping possible left_top-right_bottom diagonal lines*/
	for( x=COLS-4;x>-1;x--){
		for( y=0;y<ROWS-3;y++){
			for( t=x,i=y;t<x+4 && i<y+4;t++,i++){
				lines[count][i-y]=&grid[i][t];
			}count++;
		}
	}

    /* mapping possible left_bottom-right_top diagonal lines*/
	for( x=COLS-4;x>-1;x--){
		for( y=ROWS-1;y>2;y--){
			for( t=x,i=y;t<x+4 && i>-1;t++,i--){
				lines[count][t-x]=&grid[i][t];
			}count++;
		}
	}
	return lines;
}

/* initialize board element */
void init_board(boardType *b){
    int i,j;
    b->lastMove=-1;
	b->currentPlayer=playerOne;

    for(j=0;j<COLS;j++){
        b->heights[j] = 0;

    }

    for(i=0;i<ROWS;i++){
        for(j=0;j<COLS;j++){
            set_state(&b->grid[i][j],noPlayer);
        }
    }
}

/* create board elements. */
boardType * create_board(){
    int i,j;

	boardType * b = (boardType*)malloc(sizeof(boardType));
	b->heights = (int *)malloc(COLS*sizeof(int));
	b->moves = (int *)malloc(COLS*ROWS*sizeof(int));
	b->grid = (pointType **)malloc(COLS*sizeof(pointType *));
	for(i = 0; i < COLS; i++){
		b->grid[i] = (pointType *)malloc(ROWS*sizeof(pointType ));
	}

	for(i = 0; i <ROWS; i++){
		for(j = 0; j<COLS; j++){
			set_point(&b->grid[i][j],i,j);
		}
	}

	/* lineSize is the number of possible winning lines. */
	b->lineSize = (ROWS/4+ROWS%4)*(COLS/4+COLS%4)*2+COLS*(ROWS%4+ROWS/4)+ROWS*(COLS%4+COLS/4);
	b->possibleLines = generate_possible_lines(b->grid,b->lineSize);

	return b;
}

/* delete board = freeing the allocated memory */
void delete_board(boardType * b){
    int i;

    free(b->heights);
    free(b->moves);
    for(i = 0; i < b->lineSize; i++){
		free(b->possibleLines[i]);
	}
	free(b->possibleLines);
	free(b->possibleLines);
	free(b->grid);
	free(b);
}

/* check if the move is allowed */
int valid_move(boardType * b, int input){
	return b->heights[input]<ROWS;
}

/* check remaining moves */
int valid_moves_left(boardType * b){
	return b->lastMove<(COLS*ROWS-1);
}

/* make move, update board */
void make_move(boardType * b, int input){
	set_state(&b->grid[b->heights[input]][input],b->currentPlayer);
	b->heights[input]++;
	b->lastMove;
	b->moves[b->lastMove]=input;
	b->currentPlayer=-b->currentPlayer;
}

/* find winner for the game */
int winner_is(boardType * b){
	int i,j;
	int score;
	for( i=0;i<b->lineSize;i++){
        score=0;
		for(j=0;j<4;j++){
			score +=b->possibleLines[i][j]->state;
		}
		if(score==4){
			return playerOne;
		}else if(score==-4){
			return playerTwo;
		}
	}
	return 0;
}

/* return current player */
int get_current_player(boardType * b){
	return b->currentPlayer;
}


/** function for medium difficulty level
 *
 *  val = temp variable to contain return value
 *  maxTemp = to set players highest score
 *  possible[COLS] = to save opponent accumulated score from each crossing line in each column
 *  max = to save score for opponent in all possible lines
 *  max2 = to save score for computer in all possible lines
 */
int get_high_difficulty_move(boardType *b){
    int i,j;
    int val=-1;
    int maxTemp=0;
    int possible[COLS];
    //
    int * max = (int)malloc(b->lineSize*sizeof(int));
    int * max2 = (int)malloc(b->lineSize*sizeof(int));

    //Initialize opponent score in each possible move column
    for(i=0;i<COLS;i++){
        possible[i]=0;
    }

    //to calculate score in each line for opponent and computer
    for( i=0;i<b->lineSize;i++){
        max[i]=0;
        max2[i]=0;
        for(j=0;j<4;j++){
            if(b->possibleLines[i][j]->state==playerOne){
                max[i]+=1;
            }else if(b->possibleLines[i][j]->state==playerTwo){
                max2[i]+=1;
            }
        }
        //if the line is full reset line to zero to reduce checking later
        if(max[i]+max2[i]==4){
            max[i]=0;
            max2[i]=0;
        }
    }

    //check if each line contain first 3 computer's coin then return the possible win move
    for( i=0;i<b->lineSize;i++){
        if(max2[i]==3){
            for(j=0;j<4;j++){
                if(b->possibleLines[i][j]->state==noPlayer){
                    free(max);
                    free(max2);
                    return b->possibleLines[i][j]->x;

                }
            }
        }
    }

    //check if each line contain first 3 opponent's coin then return blocking move
    for( i=0;i<b->lineSize;i++){
        if(max[i]==3){
            for(j=0;j<4;j++){
                if(b->possibleLines[i][j]->state==noPlayer){
                    if(b->possibleLines[i][j]->y==b->heights[b->possibleLines[i][j]->x]){
                        free(max);
                        free(max2);
                        return b->possibleLines[i][j]->x;
                    }

                }
            }
        }
    }

    //check if each line contain 1 opponent's coin then set possible score to 1
    for( i=0;i<b->lineSize;i++){
        if(max[i]==1 && max2[i]==0){
            for(j=0;j<4;j++){
                if(b->possibleLines[i][j]->x > 0){
                    possible[b->possibleLines[i][j]->x-1]=1;
                    break; //break loop when found any line with cols > 0
                }else if(b->possibleLines[i][j]->x == 0){
                    possible[b->possibleLines[i][j]->x+1]=1;
                    break;
                }
            }
            maxTemp=1;
        }
    }

    //check if each line contain 2 opponent's coins then set the max possible point = 2
    for( i=0;i<b->lineSize;i++){
        if(max[i]==2){
            for(j=0;j<4;j++){
                possible[b->possibleLines[i][j]->x]=2;
            }
            maxTemp=2;
        }
    }

    //randomize movement until possible move has the same value as the highest score=maxTemp
    while(val == -1){
		int rad = rand() % 7;
		if(possible[rad]==maxTemp){
			val = rad;
		}
	}
	free(max);
    free(max2);
    return val;
}

/** function for medium difficulty level
 *
 *  val = temp variable to contain return value
 *  maxTemp = to set players highest score
 *  possible[COLS] = to save opponent accumulated score from each crossing line in each column
 *  max = to save score for all possible lines
 *
 */

int get_medium_difficulty_move(boardType *b){
    int i,j;
    int val=-1;
    int maxTemp=0;
    int possible[COLS];
    int * max = (int*)malloc(b->lineSize*sizeof(int));

    /* Initialize score in possible column */
    for(i=0;i<COLS;i++){
        possible[i]=0;
    }

    //calculate opponent score in each possible lines
    for( i=0;i<b->lineSize;i++){
        max[i]=0;
        for(j=0;j<4;j++){
            if(b->possibleLines[i][j]->state==playerOne){
                max[i]+=1;
            }else if(b->possibleLines[i][j]->state==playerTwo){
                max[i]=0;
                break;
            }
        }
    }

    //check if each line contain 1 coin then set the max possible point = 1
    for( i=0;i<b->lineSize;i++){
        if(max[i]==1){
            maxTemp=1;
            for(j=0;j<4;j++){
                possible[b->possibleLines[i][j]->x]=1;
            }
        }
    }

    //check if each line contain 2 coins  then set the max possible point = 2
    for( i=0;i<b->lineSize;i++){
        if(max[i]==2){
            maxTemp=2;
            for(j=0;j<4;j++){
                possible[b->possibleLines[i][j]->x]=2;
            }
        }
    }

    //check if each line contain 3 coins  then return block move
    for( i=0;i<b->lineSize;i++){
        if(max[i]==3){
            maxTemp=3;
            for(j=0;j<4;j++){
                if(b->possibleLines[i][j]->state==noPlayer){
                    free(max);
                    return b->possibleLines[i][j]->x;
                }
            }
        }
    }

    //randomize movement until possible move has the same value as the highest score=maxTemp
    while(val == -1){
		int rad = rand() % 7;
		if(possible[rad]==maxTemp){
			val = rad;
		}
	}
	free(max);
    return val;
}

/** function for difficulty level easy
 *
 * This function use naive randomize algorithm
 *
 */

int get_easy_difficulty_move(boardType *b){
	int val =-1;
	int possible[7];
	int i;
	for( i = 0; i <7; i++)
	{
		if(valid_move(b,i)){
			possible[i] = 1;
		}else{
			possible[i] = 0;
		}
	}

	while(val == -1){
		int rad = rand() % 7;
		if(possible[rad] == 1){
			val = rad;
		}
	}
	return val;
}

/*
*   Function to undo previous move
*   Left it commented in-case use in the future
*
*/
/*
void undo_move(boardType * b){
	set_state(&(b->grid[b->moves[b->lastMove]][b->heights[b->moves[b->lastMove]]-1]),(noPlayer));
	b->heights[b->moves[b->lastMove]]--;
	b->lastMove--;
	b->currentPlayer=-b->currentPlayer;
}
*/
