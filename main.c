#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <limits.h>
#include <gtk/gtk.h>

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

//GtkWidget * coin[ROWS][COLS];
//GtkWidget * playerImage;
//GtkWidget * winnerImage;


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
    g_print("set state %d\n",p->state);
}

int get_state(pointType * p){
    return p->state;
}

struct board{
    pointType **grid;
    int * heights;
    int * moves;
	int rows;
	int cols;
    int currentPlayer;
    int lastMove;
    int lineSize;
    pointType *** possibleLines;
};

/* Function to generate mapping between grid and possible line patterns */
pointType *** generate_possible_lines(pointType ** grid, int rows, int cols, int lineSize){
	int i,t;
	int y,x;
	int count=0;
	pointType *** lines = (pointType ***)malloc(lineSize*sizeof(pointType **));
	for(i = 0; i < lineSize; i++){
		lines[i] = (pointType **)malloc(4 * sizeof(pointType *));
	}

    /* mapping possible horizontal lines*/
	for(y=0;y<rows;y++){
		for( x=0;x<cols-3;x++){
			for(i=x;i<x+4;i++){
				lines[count][i-x]=&grid[y][i];
			}count++;
		}
	}

    /* mapping possible vertical lines*/
	for( x=0;x<cols;x++){
		for( y=0;y<rows-3;y++){
			for( i=y;i<y+4;i++){
				lines[count][i-y]=&grid[i][x];
			}count++;
		}
	}

    /* mapping possible left_top-right_bottom diagonal lines*/
	for( x=0;x<cols-3;x++){
		for( y=0;y<rows-3;y++){
			for( t=x,i=y;t<x+4 && i<y+4;t++,i++){
				lines[count][i-y]=&grid[i][t];
			}count++;
		}
	}

    /* mapping possible left_bottom-right_top diagonal lines*/
	for( x=0;x<cols-3;x++){
		for( y=rows-1;y>2;y--){
			for( t=x,i=y;t<x+4 && i>-1;t++,i--){
				lines[count][t-x]=&grid[i][t];
			}count++;
		}
	}
	return lines;
}

void destroy_possible_lines(pointType *** destroyPossibleLines,int destroyLineSize){
    int i,j;
    for(i = 0; i < destroyLineSize; i++){
        for(j = 0; j < 4; j++)
		free(destroyPossibleLines[i][j]);
	}
	free(destroyPossibleLines);
}

boardType * create_board( int bRows, int bCols){
    int i,j;
	boardType * b = (boardType*)malloc(sizeof(boardType));
    b->lastMove=-1;
	b->currentPlayer=playerOne;
	b->rows = bRows;
	b->cols = bCols;
	b->heights = (int *)malloc(b->rows*sizeof(int));
	b->moves = (int *)malloc(b->rows*b->cols*sizeof(int));
	b->grid = (pointType **)malloc(b->cols*sizeof(pointType *));
	for(i = 0; i < b->cols; i++){
		b->grid[i] = (pointType *)malloc(b->rows*sizeof(pointType ));
	}

	for(i = 0; i <b->cols; i++){
		b->heights[i] = 0;
		for(j = 0; j<b->rows; j++){
			set_point(&b->grid[i][j],i,j);
		}
	}

	/* lineSize is the number of possible winning lines. */
	b->lineSize = (b->rows/4+b->rows%4)*(b->cols/4+b->cols%4)*2+b->cols*(b->rows%4+b->rows/4)+b->rows*(b->cols%4+b->cols/4);
	b->possibleLines = generate_possible_lines(b->grid,b->rows,b->cols,b->lineSize);

	return b;
}

void delete_board(boardType * b){
	free(b->possibleLines);
	free(b->grid);
	free(b);
}

int valid_move(boardType * b, int input){
	return b->heights[input]<b->rows;
}

int valid_moves_left(boardType * b){
	return b->lastMove<(b->cols*b->rows-1);
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
	//g_print("size %d\n",b->lineSize);
	for( i=0;i<b->lineSize;i++){
        score=0;
		for(j=0;j<4;j++){
			score +=b->possibleLines[i][j]->state;
			if(score == 4)g_print("line[i]finally red %i\n",i,score);
			if(score < 0){
                g_print("line[i]finally blue %d\n",i,score);
			}
		}
		if(score==4){
			return playerOne;
		}else if(score==-4){
			return playerTwo;
		}
	}
/*
	for(i=0;i<6;i++){
        for(j=0;j<7;j++){
            test +=b->grid[i][j].state;
            if(test > 0)g_print("finally test%d\n",test);
        }
	}*/
	return 0;
}

char * to_string(boardType * b){
	char * temp = (char *)malloc(b->rows*(b->cols+1)*sizeof(char)+1);
	char * curr = temp;
	int y;
	int x;
	for( y=b->rows-1;y>-1;y--){
		for( x=0;x<b->cols;x++){
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

int get_random_player_move(boardType *b){
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
    boardType * boardInput;
    GtkWidget ** coinInput;
    GtkWidget * playerImageInput;
    GtkWidget * winnerImageInput;
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
    inputType *input = (inputType *)user_data;

    g_image_set_from_file(input->coinInput[input->boardInput->heights[input->slot]][input->slot],"CoinA.png");
    g_widget_show(input->coinInput[input->boardInput->heights[input->slot]][input->slot]);
    make_move(input->boardInput,input->slot);
    g_image_set_from_file(input->playerImageInput,"CoinBTURN.png");
    winner = winner_is(input->boardInput);
    if(winner==1){
        g_print("red win");
        g_image_set_from_file(input->winnerImageInput,"CoinAWIN.png");
        exit;
    }else if(winner==-1){
        g_image_set_from_file(input->winnerImageInput,"CoinBWIN.png");
        exit;
    }
    //wait(3);
    randomMove = get_random_player_move(input->boardInput);
    g_image_set_from_file(input->coinInput[input->boardInput->heights[randomMove]][randomMove],"CoinB.png");
    make_move(input->boardInput,randomMove);

    g_image_set_from_file(input->playerImageInput,"CoinATURN.png");
    winner = winner_is(input->boardInput);
    if(winner==1){
        g_print("red win");
        g_image_set_from_file(input->winnerImageInput,"CoinAWIN.png");
        exit;
    }else if(winner==-1){
        g_image_set_from_file(input->winnerImageInput,"CoinBWIN.png");
        exit;
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

void display_setting(GtkWidget *win, GtkWidget * buttonDisplay[7], GtkWidget ** coinDisplay, GtkWidget *playerImageDisplay, GtkWidget *winnerImageDisplay, int rows, int cols){
    int i,j;
	GtkWidget *levelButton[3];
    GtkWidget *playButton[3];
    GtkWidget *quitButton;
    GtkWidget *mainTable;
    GtkWidget *boardTable;
    GtkWidget *scoreTable;
    GtkWidget *image[cols];
    GtkWidget *newGameButton;
   //GtkWidget *newMatchButton;
    GtkWidget *frame1,*frame2,*frame3,*frame4,*frame5;
    GtkWidget *label1,*turnLabel;
    //GtkWidget *hbox1,*hbox2,*hbox3;
    GtkWidget *vbox1,*vbox2,*vbox3,*vbox4,*vbox5,*vbox6;
    GdkColor color;
    GString *str[cols];

    mainTable = gtk_table_new(2+rows,4+cols,TRUE);
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
    playerImageDisplay = gtk_image_new_from_file("CoinATURN.png");
    gtk_container_add (GTK_CONTAINER (vbox3), playerImageDisplay);

    frame4 = gtk_frame_new ("WINNER");
    gtk_table_attach_defaults (GTK_TABLE(mainTable), frame4, 2, 4, 5, 7);
    vbox4 = gtk_vbox_new (FALSE, 0);
    gtk_container_add(GTK_CONTAINER (frame4),vbox4);
    winnerImageDisplay= gtk_image_new_from_file("NoWINNER.png");
    gtk_container_add(GTK_CONTAINER (vbox4),winnerImageDisplay);


    frame5 = gtk_frame_new ("GAME");
    gtk_table_attach_defaults (GTK_TABLE(mainTable), frame5, 0, 2, 5, 7);
    vbox5 = gtk_vbox_new(FALSE,0);
    gtk_container_add (GTK_CONTAINER (frame5), vbox5);
    playButton[0] = gtk_button_new_with_label("New");
    gtk_container_add (GTK_CONTAINER (vbox5), playButton[0]);

    playButton[1] = gtk_button_new_with_label("Quit");
    gtk_container_add (GTK_CONTAINER (vbox5), playButton[1]);

    turnLabel = gtk_label_new("Design Copyright 2014 by bizz-enigma.");
    gtk_table_attach_defaults (GTK_TABLE(mainTable), turnLabel, 0, 4+cols, 1+rows, 2+rows);


    str[0] = "slot0.png";
    str[1] = "slot1.png";
    str[2] = "slot2.png";
    str[3] = "slot3.png";
    str[4] = "slot4.png";
    str[5] = "slot5.png";
    str[6] = "slot6.png";

    //buttonDisplay = create_button(cols);
    for(j=0;j<cols;j++){
        buttonDisplay[j] = gtk_button_new();
        image[j] = gtk_image_new_from_file(str[j]);
        gtk_button_set_image(buttonDisplay[j],image[j]);
        gtk_table_attach_defaults (GTK_TABLE(mainTable), buttonDisplay[j], j+4, j+5, 0, 1);
    }

    //boardTable = gtk_table_new (ROWS,COLS,FALSE);
    gtk_table_attach_defaults (GTK_TABLE(mainTable), boardTable, 4, 4+cols, 1, 1+rows);
    coinDisplay = create_coin(rows,cols);
    init_board_display(boardTable,coinDisplay,rows,cols);

}

void init_board_display(GtkWidget * boardTable, GtkWidget ** boardCoin,int rows, int cols){
    int i,j;
    int t=5; //how did i get t=5
    for(i=0;i<rows;i++){
        for(j=0;j<cols;j++){
            boardCoin[i][j] = gtk_image_new_from_file("CoinC.png");
            gtk_table_attach_defaults (GTK_TABLE(boardTable), boardCoin[i][j], j, j+1, t, t+1);
        }
        t-=1;
    }
}

GtkWidget ** create_coin(int rows, int cols){
    int i;
    GtkWidget ** createCoin = (GtkWidget **)malloc(rows*sizeof(GtkWidget *));
    for(i=0;i<rows;i++){
        createCoin[i]=(GtkWidget *)malloc(cols*sizeof(GtkWidget ));
    }
    return createCoin;
}

GtkWidget * create_button(int cols){
    GtkWidget * createButton = (GtkWidget *)malloc(cols*sizeof(GtkWidget));
    return createButton;
}



void destroy_coin(GtkWidget ** destroyCoin){
    int i;
    for(i=0;i<rows;i++){
        free(createCoin[j]);
    }
    free(createCoin);
}

int main(int argc, char** argv) {
	/* main gtk widget initialization */
	GtkWidget *win = NULL;
	GtkWidget button[7];
	GtkWidget **coin;
	GtkWidget * playerImage;
    GtkWidget * winnerImage;

	int input,i;
    boardType * b;

	b = create_board(ROWS,COLS);

	srand (time(NULL));

	g_log_set_handler ("Gtk", G_LOG_LEVEL_WARNING, (GLogFunc) gtk_false, NULL); //delete at the end
	gtk_init (&argc, &argv);
	g_log_set_handler ("Gtk", G_LOG_LEVEL_WARNING, g_log_default_handler, NULL); //delete at the end

    win = gtk_window_new (GTK_WINDOW_TOPLEVEL);
    new_window(win);
	display_setting(win,&button,coin,playerImage,winnerImage,ROWS,COLS);
    /*
	inputType data[COLS];
    for(i=0;i<7;i++){
        data[i].boardInput = b;
        data[i].slot = i;
        g_signal_connect (button[i], "clicked", G_CALLBACK (drop_coin), &data[i]);
    }*/

    /* action events */
	/*g_signal_connect (button[0], "clicked", G_CALLBACK (drop_coin), &data[0]);
    g_signal_connect (button[1], "clicked", G_CALLBACK (drop_coin), &data[1]);
    g_signal_connect (button[2], "clicked", G_CALLBACK (drop_coin), &data[2]);
    g_signal_connect (button[3], "clicked", G_CALLBACK (drop_coin), &data[3]);
    g_signal_connect (button[4], "clicked", G_CALLBACK (drop_coin), &data[4]);
    g_signal_connect (button[5], "clicked", G_CALLBACK (drop_coin), &data[5]);
    g_signal_connect (button[6], "clicked", G_CALLBACK (drop_coin), &data[6]);*/

    gtk_widget_show_all (win);
    gtk_main ();


	return 0;
}
