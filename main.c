#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <limits.h>
#include <gtk/gtk.h>
#include <gdk/gdk.h>

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



gint delete_event( GtkWidget *widget,GdkEvent *event, gpointer user_data ){
    gtk_main_quit();
    return(FALSE);
}


void dropCoin(struct send_Data * arg ){
    //printf("%d\n",arg.slot);
     //makeMove(arg.slot,0);
}

void newWindow(GtkWidget *win){
  gtk_container_set_border_width (GTK_CONTAINER (win), 10);
  gtk_window_set_title (GTK_WINDOW (win), "Connect4 Game");
  gtk_window_set_position (GTK_WINDOW (win), GTK_WIN_POS_CENTER);
  gtk_widget_realize (win);
  gtk_window_set_default_size(GTK_WINDOW(win), 867, 460);
  gtk_window_set_resizable (GTK_WINDOW(win), FALSE);
  /* Catch event close main window */
  gtk_signal_connect (win, "delete_event", GTK_SIGNAL_FUNC (delete_event), NULL);
}

void windowSetting(GtkWidget *win, board_type *b, GtkWidget *button[7],GtkWidget *coin[6][7],GtkWidget *playerImage){
    int i,j,t=5;
    //GtkWidget *button[7];
    GtkWidget *levelButton[3];
    GtkWidget *playButton[3];
    GtkWidget *quitButton;
    GtkWidget *mainTable;
    GtkWidget *boardTable;
    GtkWidget *scoreTable;
    //GtkWidget *coin[6][7];
//GtkWidget *image[7],*playerImage,*iconImage1,*iconImage2,*iconImage3;
    GtkWidget *image[7],*iconImage1,*iconImage2,*iconImage3,*winnerImage;
    GtkWidget *newGameButton;
    GtkWidget *newMatchButton;
    GtkWidget *frame1,*frame2,*frame3,*frame4,*frame5,*frame6;
    GtkWidget *hbox1,*hbox2,*hbox3;
    GtkWidget *vbox1,*vbox2,*vbox3,*vbox4,*vbox5,*vbox6;
    GtkWidget *label1,*playerLabel,*scoreLabel1,*scoreLabel2,*turnLabel,*winnerLabel;
    GdkColor color;
    GString *str[7];
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
    //gdk_color_parse()
    //gtk_widget_modify_fg (levelButton[0], GTK_STATE_NORMAL, &color);
    gtk_container_add (GTK_CONTAINER (vbox2), levelButton[0]);

    levelButton[1] = gtk_button_new_with_label("Medium");
    gtk_container_add (GTK_CONTAINER (vbox2), levelButton[1]);

    levelButton[2] = gtk_button_new_with_label("Hard");
    gtk_container_add (GTK_CONTAINER (vbox2), levelButton[2]);

    frame3 = gtk_frame_new ("PLAYER'S TURN");
    gtk_table_attach_defaults (GTK_TABLE(mainTable), frame3, 2, 4, 3, 5);
    vbox3 = gtk_vbox_new (FALSE, 0);
    gtk_container_add (GTK_CONTAINER (frame3), vbox3);
    playerImage = gtk_image_new_from_file("CoinB25.png");
    gtk_container_add (GTK_CONTAINER (vbox3), playerImage);

    frame4 = gtk_frame_new ("WINNER");
    gtk_table_attach_defaults (GTK_TABLE(mainTable), frame4, 2, 4, 5, 7);
    vbox4 = gtk_vbox_new (FALSE, 0);
    gtk_container_add(GTK_CONTAINER (frame4),vbox4);
    winnerImage= gtk_image_new_from_file("CoinB25.png");
    gtk_container_add(GTK_CONTAINER (vbox4),winnerImage);


    frame5 = gtk_frame_new ("GAME");
    gtk_table_attach_defaults (GTK_TABLE(mainTable), frame5, 0, 2, 5, 7);
    vbox5 = gtk_vbox_new(FALSE,0);
    gtk_container_add (GTK_CONTAINER (frame5), vbox5);
    playButton[0] = gtk_button_new_with_label("New");
    gtk_container_add (GTK_CONTAINER (vbox5), playButton[0]);

    playButton[1] = gtk_button_new_with_label("Quit");
    gtk_container_add (GTK_CONTAINER (vbox5), playButton[1]);
    /*
    hbox1 = gtk_hbox_new(FALSE,0);
    gtk_container_add (GTK_CONTAINER (vbox5), hbox1);
    hbox2 = gtk_hbox_new(FALSE,0);
    gtk_container_add (GTK_CONTAINER (vbox5), hbox2);
    iconImage1 = gtk_image_new_from_file("CoinBicon.png");
    gtk_container_add (GTK_CONTAINER (hbox1), iconImage1);
    scoreLabel1 = gtk_label_new("0");
    gtk_container_add (GTK_CONTAINER (hbox1), scoCoinAreLabel1);
    iconImage2 = gtk_image_new_from_file("CoinAicon.png");
    gtk_container_add (GTK_CONTAINER (hbox2), iconImage2);
    scoreLabel2 = gtk_label_new("0");
    gtk_container_add (GTK_CONTAINER (hbox2), scoreLabel2);

    hbox3 = gtk_hbox_new(FALSE,0);
    gtk_container_add (GTK_CONTAINER (vbox5), hbox3);
    winnerLabel = gtk_label_new("Winner:");
    gtk_container_add (GTK_CONTAINER (hbox3), winnerLabel);
    iconImage3 = gtk_image_new_from_file("CoinCicon.png");
    gtk_container_add (GTK_CONTAINER (hbox3), iconImage3);*/

    turnLabel = gtk_label_new("Design Copyright 2014 by bizz-enigma.");
    gtk_table_attach_defaults (GTK_TABLE(mainTable), turnLabel, 0, 11, 7, 8);


    str[0] = "slot0.png";
    str[1] = "slot1.png";
    str[2] = "slot2.png";
    str[3] = "slot3.png";
    str[4] = "slot4.png";
    str[5] = "slot5.png";
    str[6] = "slot6.png";

    for(j=0;j<7;j++){
        button[j] = gtk_button_new();
        image[j] = gtk_image_new_from_file(str[j]);
        gtk_button_set_image(button[j],image[j]);
        gtk_table_attach_defaults (GTK_TABLE(mainTable), button[j], j+4, j+5, 0, 1);
    }

    boardTable = gtk_table_new (6,7,FALSE);
    gtk_table_attach_defaults (GTK_TABLE(mainTable), boardTable, 4, 11, 1, 7);

    for(i=0;i<6;i++){
        for(j=0;j<7;j++){
            coin[i][j] = gtk_image_new_from_file("CoinC.png");
            gtk_table_attach_defaults (GTK_TABLE(boardTable), coin[i][j], j, j+1, t, t+1);
        }
        t-=1;
    }


    gtk_image_set_from_file(coin[0][2],"CoinB.png");
    gtk_image_set_from_file(coin[0][4],"CoinB.png");
    gtk_image_set_from_file(coin[0][6],"CoinB.png");
    gtk_image_set_from_file(coin[1][3],"CoinB.png");
    gtk_image_set_from_file(coin[2][4],"CoinB.png");
    gtk_image_set_from_file(coin[3][5],"CoinB.png");

    gtk_image_set_from_file(coin[0][3],"CoinA.png");
    gtk_image_set_from_file(coin[0][5],"CoinA.png");
    gtk_image_set_from_file(coin[1][5],"CoinA.png");
    gtk_image_set_from_file(coin[2][5],"CoinA.png");
    gtk_image_set_from_file(coin[1][4],"CoinA.png");



    /*gtk_signal_connect (button[1], "clicked", GTK_SIGNAL_FUNC (dropCoin), NULL);
    gtk_signal_connect (button[2], "clicked", GTK_SIGNAL_FUNC (dropCoin), NULL);
    gtk_signal_connect (button[3], "clicked", GTK_SIGNAL_FUNC (dropCoin), NULL);
    gtk_signal_connect (button[4], "clicked", GTK_SIGNAL_FUNC (dropCoin), NULL);
    gtk_signal_connect (button[5], "clicked", GTK_SIGNAL_FUNC (dropCoin), NULL);
    gtk_signal_connect (button[6], "clicked", GTK_SIGNAL_FUNC (dropCoin), NULL);*/

}

struct sendData{
	board_type * b;
	int slot;
	GtkWidget *coin;
};
typedef struct sendData send_Data;
int main (int argc, char *argv[])
{
    /* Gtk variable declaration */
    GtkWidget *win = NULL;
    GtkWidget *button[7];
    GtkWidget *playerImage;
    GtkWidget *coin[6][7];
    GtkObject *myobj;

    /* Functional declaration */
    board_type * b = createBoard(7,6);

    srand (time(NULL));
    /* Gtk initialization */
    g_log_set_handler ("Gtk", G_LOG_LEVEL_WARNING, (GLogFunc) gtk_false, NULL); //delete at the end
    gtk_init (&argc, &argv);
    g_log_set_handler ("Gtk", G_LOG_LEVEL_WARNING, g_log_default_handler, NULL); //delete at the end

    /* Create the main window */
    win = gtk_window_new (GTK_WINDOW_TOPLEVEL);
    newWindow(win);
    windowSetting(win,b,button,coin,playerImage);


    send_Data * data = (send_Data*) malloc(sizeof(send_Data));
    data.b =b;
    data.coin = coin;
    data.slot =0;
    /*
    int i;
    for(i=0;i<7;i++){
        data[i].b = b;
        data[i].coin = coin;
        data[i].slot = i;
    }*/

    //dropCoin(&data);
    //gtk_signal_connect(GTK_OBJECT(button[0]), "clicked", GTK_SIGNAL_FUNC(dropCoin),&data);

    gtk_widget_show_all (win);
    gtk_main ();
    //free(data);
    return 0;
}
