#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <limits.h>
#include <gtk/gtk.h>
#include <gdk/gdk.h>

#ifdef _WIN32
    #include <windows.h>
#else
    #include <unitstd.h>
#endif

#define ROWS 6
#define COLS 7

static const int playerOne=1;
static const int playerTwo=-1;
static const int empty=0;
typedef struct point pointType;
typedef struct board boardType;
typedef struct input inputType;
typedef struct level levelType;
typedef struct moves possibleMove;
typedef enum { newgame, ongoing, complete} bool;
int gameDifficultyLevel;
bool gameStatus;


GtkWidget * coin[ROWS][COLS];
GtkWidget * playerImage;
GtkWidget * winnerImage;
GtkWidget * levelButton[3];
GtkWidget * button[COLS];


struct point{
    int x, y, state;
};

void set_point(pointType * grid,int yAxis, int xAxis){
    grid->y=yAxis;
    grid->x=xAxis;
    grid->state=empty;
}

void delete_Point(pointType * p){
    free(p);
}

void set_state(pointType * p, int player){
    p->state=player;
}

int get_state(pointType * p){
    return p->state;
}

struct board{
    pointType **grid;
    int * heights;
    int * moves;
    int currentPlayer;
    int lastMove;
    int lineSize;
    pointType *** possibleLines;
};

pointType *** generate_possible_lines(pointType ** grid, int lineSize){
	int i,t;
	int y,x;
	int count=0;
	pointType *** lines = (pointType ***)malloc(lineSize*sizeof(pointType **));
	for(i = 0; i < lineSize; i++){
		lines[i] = (pointType **)malloc(4 * sizeof(pointType *));
	}

    /* mapping possible horizontal lines*/
	for(y=0;y<ROWS;y++){
		for( x=0;x<COLS-3;x++){
			for(i=x;i<x+4;i++){
				lines[count][i-x]=&grid[y][i];
			}count++;
		}
	}

    /* mapping possible vertical lines*/
	for( x=0;x<COLS;x++){
		for( y=0;y<ROWS-3;y++){
			for( i=y;i<y+4;i++){
				lines[count][i-y]=&grid[i][x];
			}count++;
		}
	}

    /* mapping possible left_top-right_bottom diagonal lines*/
	for( x=0;x<COLS-3;x++){
		for( y=0;y<ROWS-3;y++){
			for( t=x,i=y;t<x+4 && i<y+4;t++,i++){
				lines[count][i-y]=&grid[i][t];
			}count++;
		}
	}

    /* mapping possible left_bottom-right_top diagonal lines*/
	for( x=0;x<COLS-3;x++){
		for( y=ROWS-1;y>2;y--){
			for( t=x,i=y;t<x+4 && i>-1;t++,i--){
				lines[count][t-x]=&grid[i][t];
			}count++;
		}
	}
	return lines;
}

boardType * create_board(){
    int i,j;
	boardType * b = (boardType*)malloc(sizeof(boardType));
    b->lastMove=-1;
	b->currentPlayer=playerOne;
	b->heights = (int *)malloc(COLS*sizeof(int));
	b->moves = (int *)malloc(COLS*ROWS*sizeof(int));
	b->grid = (pointType **)malloc(COLS*sizeof(pointType *));
	for(i = 0; i < COLS; i++){
		b->grid[i] = (pointType *)malloc(ROWS*sizeof(pointType ));
		b->heights[i] = 0;
	}

	for(i = 0; i <ROWS; i++){
		for(j = 0; j<COLS; j++){
			set_point(&b->grid[i][j],i,j);
		}
	}

	/* lineSize is the number of possible winning lines. */
	b->lineSize = (ROWS/4+ROWS%4)*(COLS/4+COLS%4)*2+COLS*(ROWS%4+ROWS/4)+ROWS*(COLS%4+COLS/4);
	b->possibleLines = generate_possible_lines(b->grid,b->lineSize);
    //g_print("create lineSize %d \n",b->lineSize);
	return b;
}

void delete_board(boardType * b){
	free(b->possibleLines);
	free(b->grid);
	free(b);
}

int valid_move(boardType * b, int input){
	return b->heights[input]<ROWS;
}

int valid_moves_left(boardType * b){
	return b->lastMove<(COLS*ROWS-1);
}

void make_move(boardType * b, int input){
	set_state(&b->grid[input][b->heights[input]],b->currentPlayer);
	b->heights[input]++;
	b->lastMove;
	b->moves[b->lastMove]=input;
	b->currentPlayer=-b->currentPlayer;
}

void undo_move(boardType * b){
	set_state(&(b->grid[b->moves[b->lastMove]][b->heights[b->moves[b->lastMove]]-1]),(empty));
	b->heights[b->moves[b->lastMove]]--;
	b->lastMove--;
	b->currentPlayer=-b->currentPlayer;
}

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

char * to_string(boardType * b){
	char * temp = (char *)malloc(ROWS*(COLS+1)*sizeof(char)+1);
	char * curr = temp;
	int y;
	int x;
	for( y=ROWS-1;y>-1;y--){
		for( x=0;x<COLS;x++){
			if(get_state(&b->grid[x][y])==empty){
				*curr = '-';
			}else if(get_state(&b->grid[x][y])==playerOne){
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

int get_current_player(boardType * b){
	return b->currentPlayer;
}

int get_high_difficulty_move(boardType *b){
    int val =-1;

    return val;
}

struct moves{
    int totEmptyCell;
    int emptyCell;
    int available;
};
int get_medium_difficulty_move(boardType *b){
    int val=-1;
    int emptyLoc;
    int connected;
    possibleMove possible[COLS];

    for(i=0;i<COLS;i++){
        if(valid_move(b,i)){
			possible[i].available = 1;
		}else{
			possible[i].available = 0;
		}
    }

    for(i=0;i<b->lineSize;i++){
        emptyLoc=0;
        connected=0;
        for(j=0;j<4;j++){
            if(b->possibleLines[i][j].x!=b->heights[based on col])
            if(b->possibleLines[i][j].state==empty){
                emptyLoc=+1;
            }
            if(b->possibleLines[i][j].state==playerTwo){
                connected=+1;
            }
        }
        if(emptyLoc<3 and connected<3){

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

struct input{
    boardType * bInput;
    int slot;
};

gint delete_event( GtkWidget *widget,GdkEvent *event, gpointer user_data ){
    gtk_main_quit();
    return(FALSE);
}

void wait(int secs) {
    #ifdef _WIN32
        Sleep( 500*secs);
    #else
        Sleep(secs);
    #endif //
}

void drop_coin(GtkWidget *widget,gpointer user_data){
    int randomMove,winner;
    GtkWidget *dialog;
    inputType *input = (inputType *)user_data;
    //g_print("lineSize %d, slot %d, \n",input->bInput->lineSize ,input->slot);
    if(gameStatus == complete)return;
    if(input->bInput->heights[input->slot]==6){
        return;
    }
    gtk_image_set_from_file(coin[input->bInput->heights[input->slot]][input->slot],"CoinA.png");
    make_move(input->bInput,input->slot);
    winner = winner_is(input->bInput);
    if(winner==1){
        gtk_image_set_from_file(winnerImage,"CoinAWIN.png");
        gameStatus = complete;
        reset_board();
        return;
    }
    gtk_image_set_from_file(playerImage,"CoinBTURN.png");
    //wait(3);
    randomMove = get_random_player_move(input->bInput);
    gtk_image_set_from_file(coin[input->bInput->heights[randomMove]][randomMove],"CoinB.png");
    make_move(input->bInput,randomMove);
    winner = winner_is(input->bInput);
    if(winner==-1){
        gtk_image_set_from_file(winnerImage,"CoinBWIN.png");
        gameStatus = complete;
        return;
    }
    gtk_image_set_from_file(playerImage,"CoinATURN.png");
}

void set_game(int gameLevel){
    int j;
    if(gameStatus == ongoing) return;
    if(gameStatus == complete) return;
    gameStatus = ongoing;
    reset_board(TRUE);
    gtk_image_set_from_file(playerImage,"CoinATURN.png");
    gtk_widget_set_sensitive(levelButton[gameLevel],TRUE);
}

void reset_board(gboolean buttonStatus){
    int i,j;

    for(i=0;i<3;i++){
        gtk_widget_set_sensitive(levelButton[i],FALSE);
    }

    for(j=0;j<COLS;j++){
        gtk_widget_set_sensitive(button[j],TRUE);
    }

    for(i=0;i<ROWS;i++){
        for(j=0;j<COLS;j++){
            gtk_image_set_from_file(coin[i][j],"CoinC.png");
        }
    }
}

void new_window(GtkWidget *win){
    gtk_container_set_border_width (GTK_CONTAINER (win), 10);
    gtk_window_set_title (GTK_WINDOW (win), "Connect4 Game");
    gtk_window_set_position (GTK_WINDOW (win), GTK_WIN_POS_CENTER);
    gtk_widget_realize (win);
    gtk_window_set_default_size(GTK_WINDOW(win), 867, 460);
    gtk_window_set_resizable (GTK_WINDOW(win), FALSE);
    /* Catch event close main window */
    g_signal_connect (win, "delete_event", GTK_SIGNAL_FUNC (delete_event), NULL);
}

void display_setting(GtkWidget *win){
    int i,j,t=5;
    GtkWidget *mainTable;
    GtkWidget *boardTable;
    GtkWidget *scoreTable;
    GtkWidget *image[COLS];
    GtkWidget *newGameButton;
    GtkWidget *quitGameButton;
    GtkWidget *frame1,*frame2,*frame3,*frame4,*frame5;
    GtkWidget *label1,*turnLabel;
    GtkWidget *hbox1,*hbox2,*hbox3;
    GtkWidget *vbox1,*vbox2,*vbox3,*vbox4,*vbox5,*vbox6;
    gchar* str = g_malloc(10);

    mainTable = gtk_table_new(8,11,TRUE);
    gtk_container_add (GTK_CONTAINER (win), mainTable);


    frame1 = gtk_frame_new ("GAME INSTRUCTIONS");
    gtk_table_attach_defaults (GTK_TABLE(mainTable), frame1, 0, 4, 0, 3);

    vbox1 = gtk_vbox_new (FALSE, 0);
    gtk_container_add (GTK_CONTAINER (frame1), vbox1);

    /* Standard message dialog */
    label1 = gtk_label_new ("To Win:\nCONNECT 4 COINS IN A ROW, COLUMN OR DIAGONAL.\nTo Play:\n1. Select Difficulty Level. Easy is the default.\n2. Click any SLOT BUTTON to drop your coin.\nNew Match: To reset a match or play a new match.\nNew Game: To play new game.\nNote: A match can have many games.\nTo Exit:Click button 'x' at the top right corner.");
    gtk_box_pack_start (GTK_BOX (vbox1), label1, FALSE, FALSE, 0);
    gtk_label_set_line_wrap( GTK_LABEL( label1 ), TRUE );

    frame2 = gtk_frame_new ("DIFFICULTY LEVEL");
    gtk_table_attach_defaults (GTK_TABLE(mainTable), frame2, 0, 2, 3, 5);

    vbox2 = gtk_vbox_new (FALSE, 0);
    gtk_container_set_border_width (GTK_CONTAINER (vbox2), 5);
    gtk_container_add (GTK_CONTAINER (frame2), vbox2);

    levelButton[0] = gtk_button_new_with_label("Easy");
    gtk_container_add (GTK_CONTAINER (vbox2), levelButton[0]);

    levelButton[1] = gtk_button_new_with_label("Medium");
    gtk_container_add (GTK_CONTAINER (vbox2), levelButton[1]);

    levelButton[2] = gtk_button_new_with_label("Hard");
    gtk_container_add (GTK_CONTAINER (vbox2), levelButton[2]);

    frame3 = gtk_frame_new ("PLAYER'S TURN");
    gtk_table_attach_defaults (GTK_TABLE(mainTable), frame3, 2, 4, 3, 5);
    vbox3 = gtk_vbox_new (FALSE, 0);
    gtk_container_add (GTK_CONTAINER (frame3), vbox3);
    playerImage = gtk_image_new_from_file("NoWINNER.png");
    gtk_container_add (GTK_CONTAINER (vbox3), playerImage);

    frame4 = gtk_frame_new ("WINNER");
    gtk_table_attach_defaults (GTK_TABLE(mainTable), frame4, 2, 4, 5, 7);
    vbox4 = gtk_vbox_new (FALSE, 0);
    gtk_container_add(GTK_CONTAINER (frame4),vbox4);
    winnerImage= gtk_image_new_from_file("NoWINNER.png");
    gtk_container_add(GTK_CONTAINER (vbox4),winnerImage);


    frame5 = gtk_frame_new ("GAME");
    gtk_table_attach_defaults (GTK_TABLE(mainTable), frame5, 0, 2, 5, 7);
    vbox5 = gtk_vbox_new(FALSE,0);
    gtk_container_add (GTK_CONTAINER (frame5), vbox5);
    newGameButton = gtk_button_new_with_label("New");
    gtk_container_add (GTK_CONTAINER (vbox5), newGameButton);

    quitGameButton = gtk_button_new_with_label("Quit");
    gtk_container_add (GTK_CONTAINER (vbox5), quitGameButton);

    turnLabel = gtk_label_new("Design Copyright 2014 by bizz-enigma.");
    gtk_table_attach_defaults (GTK_TABLE(mainTable), turnLabel, 0, 4+COLS, 1+ROWS, 2+ROWS);

    for(j=0;j<COLS;j++){

        g_snprintf(str,10, "slot%d.png",j);
        button[j] = gtk_button_new();
        image[j] = gtk_image_new_from_file(str);
        gtk_button_set_image(button[j],image[j]);
        gtk_table_attach_defaults (GTK_TABLE(mainTable), button[j], j+4, j+5, 0, 1);
        gtk_widget_set_sensitive(button[j],FALSE);
    }

    boardTable = gtk_table_new (ROWS,COLS,FALSE);
    gtk_table_attach_defaults (GTK_TABLE(mainTable), boardTable, 4, 4+COLS, 1, 1+ROWS);


    for(i=0;i<ROWS;i++){
        for(j=0;j<COLS;j++){
            coin[i][j] = gtk_image_new_from_file("CoinC.png");
            gtk_table_attach_defaults (GTK_TABLE(boardTable), coin[i][j], j, j+1, t, t+1);
        }t-=1;
    }

}



int main(int argc, char** argv) {
	GtkWidget *win = NULL;
	int input,i;
	int gameLevel=1;

	boardType * b = create_board();

	srand (time(NULL));
	gameStatus = newgame;

	g_log_set_handler ("Gtk", G_LOG_LEVEL_WARNING, (GLogFunc) gtk_false, NULL); //delete at the end
	gtk_init (&argc, &argv);
	g_log_set_handler ("Gtk", G_LOG_LEVEL_WARNING, g_log_default_handler, NULL); //delete at the end

    win = gtk_window_new (GTK_WINDOW_TOPLEVEL);
    new_window(win);

	display_setting(win);

	inputType data[COLS];
    for(i=0;i<COLS;i++){
        data[i].bInput = b;
        data[i].slot = i;
        //g_print("display slot:%d, lineSize:%d\n",data[i].slot,data[i].bInput->lineSize);
        g_signal_connect (button[i], "clicked", G_CALLBACK (drop_coin), &data[i]);
    }

    for(i=0;i<3;i++){
        gtk_signal_connect_object (levelButton[i], "clicked", G_CALLBACK(set_game),i);
    }


    gtk_widget_show_all (win);
    gtk_main ();


	return 0;
}
