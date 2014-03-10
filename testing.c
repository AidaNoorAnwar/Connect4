#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <limits.h>


static const int PLAYER_ONE = 1;
static const int PLAYER_TWO = -1;
static const int EMPTY = 0;
typedef struct point point_type;

struct point{
	int x, y, state;
};

point_type * newPoint(int a, int b){
	point_type * p = (point_type*)malloc(sizeof(point_type));
	p->x=a;
	p->y=b;
	p->state=EMPTY;
	return p;
}

void deletepoint(point_type* p){
	free(p);
}

int equalsPosition(point_type * a, point_type* b){
	return a->x == b->x && a->y == b->y;
}

void setState(point_type * a, int player){
	a->state=player;
}

int getState(point_type * a){
	return a->state;
}

typedef struct board board_type;

struct board {
	point_type*** grid;
	int * heights;

	int cols;
	int rows;

	int * moves;
	int lm;

	int cp;
	point_type *** cl;
};

point_type*** generateCL(point_type *** grid){
	point_type *** lines = (point_type ***)malloc(69 * sizeof(point_type **));
	int i;

	for(i = 0; i < 69; i++){
		lines[i] = (point_type **)malloc(4 * sizeof(point_type *));
	}

	int count =0;
	int y;
	int x;
	int t;
	for(y=0;y<6;y++){
		for( x=0;x<4;x++){
			point_type ** temp = (point_type **)malloc(4 * sizeof(point_type *));
			for(i=x;i<x+4;i++){
				temp[i-x]=grid[i][y];
			}
			lines[count]=temp;
			count++;
		}
	}

	for( x=0;x<7;x++){
		for( y=0;y<3;y++){
			point_type ** temp = (point_type**)malloc(4 * sizeof(point_type *));
			for( i=y;i<y+4;i++){
				temp[i-y]=grid[x][i];
			}
			lines[count]=temp;
			count++;
		}
	}

	for( x=0;x<4;x++){
		for( y=0;y<3;y++){
			point_type** temp = (point_type**)malloc(4 * sizeof(point_type *));
			for( t=x,i=y;t<x+4 && i<y+4;t++,i++){
				temp[i-y]=grid[t][i];
			}
			lines[count]=temp;
			count++;
		}
	}

	for( x=0;x<4;x++){
		for( y=5;y>2;y--)
		{
			point_type ** temp = (point_type **)malloc(4 * sizeof(point_type *));
			for( t=x,i=y;t<x+4 && i>-1;t++,i--){
				temp[t-x]=grid[t][i];
			}
			lines[count]=temp;
			count++;
		}
	}
	return lines;
}

board_type * createBoard(int a, int b){
	board_type * p = (board_type*)malloc(sizeof(board_type));
	p->cols=a;
	p->rows=b;
	p->lm=-1;
	p->cp=PLAYER_ONE;
	p->heights = (int *)malloc(p->cols * sizeof(int));
	p->grid = (point_type ***)malloc(p->cols * sizeof(point_type **));
	int x;
	int y;

	for(x = 0; x < p->cols; x++){
		p->grid[x] =(point_type **)malloc(p->rows * sizeof(point_type *));
		p->heights[x] = 0;
		for(y = 0; y< p->rows; y++){
			p->grid[x][y] = newPoint(x,y);
		}
	}

	p->moves = (int *)malloc(p->cols * p->rows * sizeof(int));
	p->cl = generateCL(p->grid);
	return p;
}

void deleteboard(board_type* p){
	free(p->cl);
	free(p->grid);
	free(p->heights);
	free(p->moves);
	free(p);
}

int validMove(board_type * b, int column){
	return b->heights[column]<b->rows;
}

void makeMove(board_type * b, int column){
	setState(b->grid[column][b->heights[column]],b->cp);

	b->heights[column]++;
	b->lm++;
	b->moves[b->lm]=column;
	b->cp=-b->cp;
}


void undoMove(board_type * b){
	setState(b->grid[b->moves[b->lm]][b->heights[b->moves[b->lm]]-1],(EMPTY));
	b->heights[b->moves[b->lm]]--;
	b->lm--;
	b->cp=-b->cp;
}


int validMovesLeft(board_type * b){
	return b->lm<((b->cols*b->rows)-1);
}
//check this program
int getScore(point_type * points[]) {
	int playerone=0;
	int playertwo=0;
	int i;

	for( i=0;i<4;i++){
		if(getState(points[i])==PLAYER_ONE){
			playerone++;
		}else if(getState(points[i])==PLAYER_TWO){
			playertwo++;
		}
	}

	if((playerone+playertwo>0) && (!(playerone>0 && playertwo>0))){
		return (playerone!=0)?playerone:playertwo;
	}else{
		return 0;
	}
}

int getStrength(board_type * b){
	int sum=0;
	int weights[] = {0,1,10,50,600};
	int i;

	for( i=0;i<69;i++){
		sum+=(getScore(b->cl[i])>0)?weights[abs(getScore(b->cl[i]))]:-weights[abs(getScore(b->cl[i]))];
	}

	return sum+(b->cp==PLAYER_ONE?16:-16);
}

int winnerIs(board_type * b){
	int i;
	for( i=0;i<69;i++){
		if(getScore(b->cl[i])==4)
		{
		return PLAYER_ONE;
		}
		else if(getScore(b->cl[i])==-4)
		{
		return PLAYER_TWO;
		}
	}
	return 0;
}

char * toString(board_type * b){
	char * temp = (char *)malloc(b->rows*(b->cols+1)*sizeof(char)+1);
	char * curr = temp;
	int y;
	int x;
	for( y=b->rows-1;y>-1;y--){
		for( x=0;x<b->cols;x++){
			if(getState(b->grid[x][y])==EMPTY){
				*curr = '-';
			}else if(getState(b->grid[x][y])==PLAYER_ONE){
				*curr = 'O';
			}else{
				*curr = 'X';
			}
			curr++;
		}
		*curr = '\n';
		curr++;
	}
	return temp;
}

int cp(board_type * b){
	return b->cp;
}

int getRandomPlayerMove(board_type *b){
	int val =-1;
	int possible[7];
	int i;
	for( i = 0; i <7; i++)
	{
		if(validMove(b,i)){
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

int minValue(board_type * cB, int ply);
int maxValue(board_type * cB, int ply);
// should return a number
int getReasonedMove(board_type * cB){
	int moves[7];
	int highest = 0;
	int i;
	for( i=0;i<7;i++)
	{
		moves[i] = INT_MIN;
		if(validMove(cB, i))
		{
			makeMove(cB,i);
			moves[i] = minValue(cB,4);
			//check this if doing what.
			if(moves[i]>=moves[highest])
			highest=i;
			undoMove(cB);
		}
	}
	return highest;
}

// don't change this unless you understand it
int minValue(board_type * cB, int ply){
	int moves[7];
	int lowest = 0;
	int i;
	for( i=0;i<7;i++){
		moves[i] = INT_MAX;
		if(validMove(cB,i)){
			makeMove(cB,i);
			if((winnerIs(cB) == 0) && ply>0){
				moves[i] = maxValue(cB,ply-1);
			}else{
				moves[i] = -getStrength(cB);
			}
			//check this if doing what.
			if(moves[i]<moves[lowest])
			lowest=i;
			undoMove(cB);
		}

	}

	return moves[lowest];

}
//careful with this
int maxValue(board_type * cB, int ply){
	int moves[7];
	int highest = 0;
	int i;
	for( i=0;i<7;i++)
	{
		moves[i] = INT_MAX;
		if(validMove(cB,i)){
			makeMove(cB,i);
			if((winnerIs(cB) == 0) && ply>0){
				moves[i] = minValue(cB,ply-1);
			}
			else
			moves[i] =-getStrength(cB);
			if(moves[i]<moves[highest])
			highest=i;
			undoMove(cB);
		}
	}

	return moves[highest];
}

typedef struct sendData send_Data;
struct sendData{
	board_type * b;
	int slot;
	//GtkWidget *coin;
};


/*
 *
 */
int main(int argc, char** argv) {
	srand (time(NULL));
	board_type * b = createBoard(7,6);

	int input;

	send_Data * data = (send_Data*) malloc(sizeof(send_Data));
    data.b =b;
    data.slot =0;
	/*
	while((winnerIs(b)==0) && validMovesLeft(b)){
		if(cp(b) == PLAYER_ONE){
			scanf("%d", &input);
			makeMove(b,input);// Make it so!
		}else{
			makeMove(b, getReasonedMove(b));// Make it so!
		}
		printf("%s\n", toString(b));


	}*/
	return 0;
}


