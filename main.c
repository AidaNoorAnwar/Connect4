/** This file contain main function and gtk functions only
 *  GtkWidget are declared global since passing more than 1 GtkWidget does not work
 */

#include <gtk/gtk.h>
#include <gdk/gdk.h>
#include <time.h>
#include "game.h"

typedef struct input inputType;

/* Declare Global GTKWidget */
GtkWidget * coin[ROWS][COLS];
GtkWidget * playerImage;
GtkWidget * winnerImage;
GtkWidget * levelButton[3];
GtkWidget * button[COLS];
GtkWidget * newGameButton;
GtkWidget * quitGameButton;

/* structure input for drop_button parameters*/
struct input{
    boardType * bInput;
    int slot;
};

/* Delete main windows when user click Quit Game*/
gint delete_event( GtkWidget *widget,GdkEvent *event, gpointer user_data ){
    gtk_main_quit();
    return(FALSE);
}

/* This function is fired when the drop coin button is clicked */
void drop_coin(GtkWidget *widget,gpointer user_data){
    int randomMove=0;
    int winner=0;
    inputType *input = (inputType *)user_data;
    if(gameStatus == complete)return;
    if(input->bInput->heights[input->slot]==ROWS)return;
    if(gameStatus == newgame){
        init_board(input->bInput);
        gameStatus=ongoing;
    }
    gtk_image_set_from_file(coin[input->bInput->heights[input->slot]][input->slot],"CoinA.png");
    make_move(input->bInput,input->slot);
    winner = winner_is(input->bInput);
    if(winner==1){
       gtk_image_set_from_file(winnerImage,"CoinAWIN.png");
       gameStatus = complete;
       init_board(input->bInput);
       return;
    }

    gtk_image_set_from_file(playerImage,"CoinBTURN.png");

    if(gameDifficultyLevel== 0){
        randomMove = get_easy_difficulty_move(input->bInput);
    }else if(gameDifficultyLevel == 1){
        randomMove = get_medium_difficulty_move(input->bInput);
    }else if(gameDifficultyLevel == 2){
        randomMove = get_high_difficulty_move(input->bInput);
    }
    if(input->bInput->heights[randomMove]==ROWS){
            return;
    }
    gtk_image_set_from_file(coin[input->bInput->heights[randomMove]][randomMove],"CoinB.png");
    make_move(input->bInput,randomMove);
    winner = winner_is(input->bInput);
    if(winner==-1){
        gtk_image_set_from_file(winnerImage,"CoinBWIN.png");
        gameStatus = complete;
        init_board(input->bInput);
        return;
    }
    gtk_image_set_from_file(playerImage,"CoinATURN.png");
}

/* This function is fired when any difficulty levels button is clicked
*   -enable board
*/
void set_game(gint temp){
    if(gameStatus == ongoing) return;
    if(gameStatus == complete) return;
    gameStatus = ongoing;
    reset_board(FALSE,TRUE);
    gtk_image_set_from_file(playerImage,"CoinATURN.png");
    gtk_widget_set_sensitive(levelButton[temp],TRUE);
    gameDifficultyLevel=temp;
}

/* This function is fired when newgame button is clicked
*   -reset interface
*/
void reset_game(GtkWidget *widget,gpointer user_data){
    gameDifficultyLevel=0;
    gameStatus = newgame;
    reset_board(TRUE,FALSE);
}

/*
*   reset board interface
*/
void reset_board(gboolean levelState,gboolean buttonState){
    int i,j;

    for(i=0;i<3;i++){
        gtk_widget_set_sensitive(levelButton[i],levelState);
    }

    for(j=0;j<COLS;j++){
        gtk_widget_set_sensitive(button[j],buttonState);
    }

    for(i=0;i<ROWS;i++){
        for(j=0;j<COLS;j++){
            gtk_image_set_from_file(coin[i][j],"CoinC.png");
        }
    }
    gtk_image_set_from_file(winnerImage,"NoWINNER.png");
    gtk_image_set_from_file(playerImage,"NoWINNER.png");
}

/*
*   setup new window
*/
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

/*
*   setup component in the window
*/
void display_setting(GtkWidget *win){
    int i,j,t=5;
    GtkWidget *mainTable;
    GtkWidget *boardTable;
    GtkWidget *image[COLS];
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
    label1 = gtk_label_new ("To Win:\nCONNECT 4 COINS IN A ROW, COLUMN OR DIAGONAL.\nTo Play:\n1. Select Difficulty Level.\n2. Click any SLOT BUTTON to drop your coin.\nNew Game: To play new game.\nTo Exit:Click button 'x' at the top right corner.");
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
	int i;

	boardType * b = create_board(); //create board
	init_board(b); //init board
	srand (time(NULL));
	gameStatus = newgame;

    /* error handler lines commented out for future use and not require for software release*/
	//g_log_set_handler ("Gtk", G_LOG_LEVEL_WARNING, (GLogFunc) gtk_false, NULL); //delete at the end
	gtk_init (&argc, &argv);
	//g_log_set_handler ("Gtk", G_LOG_LEVEL_WARNING, g_log_default_handler, NULL); //delete at the end

    /*initialize new window from gtk library*/
    win = gtk_window_new (GTK_WINDOW_TOPLEVEL);
    new_window(win);

    /* setup window display */
	display_setting(win);

    /* start parsing data to gtk event */
	inputType data[COLS];
    for(i=0;i<COLS;i++){
        data[i].bInput = b;
        data[i].slot = i;
        g_signal_connect (button[i], "clicked", G_CALLBACK (drop_coin),&data[i]);
    }

    for(i=0;i<3;i++){
        gtk_signal_connect_object (levelButton[i], "clicked", G_CALLBACK(set_game),i);
    }

    gtk_signal_connect_object (newGameButton, "clicked", G_CALLBACK(reset_game),NULL);
    gtk_signal_connect_object (quitGameButton, "clicked", G_CALLBACK(delete_event),NULL);

    /* end parsing data to gtk event */

    gtk_widget_show_all (win);
    gtk_main ();

    /* free up allocated memory from the application */
    delete_board(b);
	return 0;
}
