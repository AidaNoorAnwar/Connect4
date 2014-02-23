#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <limits.h>
#include <gtk/gtk.h>

gint delete_event( GtkWidget *widget, GdkEvent *event, gpointer data ){
    gtk_main_quit();
    return(FALSE);
}

int main (int argc, char *argv[])
{
  /* Gtk variable declaration */
  int i,j,t=5;
  GtkWidget *win = NULL;
  GtkWidget *button[7];
  GtkWidget *levelButton[3];
  GtkWidget *playButton[3];
  GtkWidget *quitButton;
  GtkWidget *mainTable;
  GtkWidget *boardTable;
  GtkWidget *scoreTable;
  GtkWidget *coin[6][7];
  GtkWidget *image[7],*playerImage,*iconImage1,*iconImage2,*iconImage3;
  GtkWidget *newGameButton;
  GtkWidget *newMatchButton;
  GtkWidget *frame1,*frame2,*frame3,*frame4,*frame5,*frame6;
  GtkWidget *hbox1,*hbox2,*hbox3;
  GtkWidget *vbox1,*vbox2,*vbox3,*vbox4,*vbox5,*vbox6;
  GtkWidget *label1,*playerLabel,*scoreLabel1,*scoreLabel2,*turnLabel,*winnerLabel;
  GdkColor color;
  GString *str[7];

  /* Gtk initialization */
  //g_log_set_handler ("Gtk", G_LOG_LEVEL_WARNING, (GLogFunc) gtk_false, NULL); //delete at the end
  gtk_init (&argc, &argv);
  //g_log_set_handler ("Gtk", G_LOG_LEVEL_WARNING, g_log_default_handler, NULL); //delete at the end

  /* Create the main window */
  win = gtk_window_new (GTK_WINDOW_TOPLEVEL);
  gtk_container_set_border_width (GTK_CONTAINER (win), 10);
  gtk_window_set_title (GTK_WINDOW (win), "Connect4 Game");
  gtk_window_set_position (GTK_WINDOW (win), GTK_WIN_POS_CENTER);
  //gtk_window_set_default_icon_from_file("Icon.png",);
  gtk_widget_realize (win);
  gtk_window_set_default_size(GTK_WINDOW(win), 867, 460);
  gtk_window_set_resizable (GTK_WINDOW(win), FALSE);
  /* Catch event close main window */
  gtk_signal_connect (win, "delete_event", GTK_SIGNAL_FUNC (delete_event), NULL);

  mainTable = gtk_table_new(8,11,TRUE);
  gtk_container_add (GTK_CONTAINER (win), mainTable);


  frame1 = gtk_frame_new ("GAME INSTRUCTIONS");
  gtk_table_attach_defaults (GTK_TABLE(mainTable), frame1, 0, 4, 0, 3);

  vbox1 = gtk_vbox_new (FALSE, 0);
  gtk_container_add (GTK_CONTAINER (frame1), vbox1);

  /* Standard message dialog */
  label1 = gtk_label_new ("To Win:\nCONNECT 4 COINS IN A ROW, COLUMN OR DIAGONAL.\nTo Play:\n1. Select Difficulty Level.\n2. Click any SLOT BUTTON to drop your coin.\nNew Match: To reset a match or play a new match.\nNew Game: To play new game.\nNote: A match can have many games.\nTo Exit:Click button 'x' at the top right corner.");
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

  frame3 = gtk_frame_new ("NEW");
  gtk_table_attach_defaults (GTK_TABLE(mainTable), frame3, 2, 4, 3, 5);


  vbox3 = gtk_vbox_new (FALSE, 0);
  gtk_container_set_border_width (GTK_CONTAINER (vbox3),0);
  gtk_container_add (GTK_CONTAINER (frame3), vbox3);

  playButton[0] = gtk_button_new_with_label("Match");
  gtk_container_add (GTK_CONTAINER (vbox3), playButton[0]);

  playButton[1] = gtk_button_new_with_label("Game");
  gtk_container_add (GTK_CONTAINER (vbox3), playButton[1]);

  frame4 = gtk_frame_new ("PLAYER'S TURN");
  gtk_table_attach_defaults (GTK_TABLE(mainTable), frame4, 0, 2, 5, 7);
  vbox4 = gtk_vbox_new (FALSE, 0);
  gtk_container_set_border_width (GTK_CONTAINER (vbox4), 0);
  gtk_container_add (GTK_CONTAINER (frame4), vbox4);
  playerImage = gtk_image_new_from_file("CoinA25.png");
  gtk_container_add (GTK_CONTAINER (vbox4), playerImage);
  //turnLabel = gtk_label_new("It's Your Turn");
  //gtk_container_add (GTK_CONTAINER (vbox4), turnLabel);

  frame5 = gtk_frame_new ("WIN SCORES");
  gtk_table_attach_defaults (GTK_TABLE(mainTable), frame5, 2, 4, 5, 7);
  vbox5 = gtk_vbox_new(FALSE,0);
  gtk_container_add (GTK_CONTAINER (frame5), vbox5);
  hbox1 = gtk_hbox_new(FALSE,0);
  gtk_container_add (GTK_CONTAINER (vbox5), hbox1);
  hbox2 = gtk_hbox_new(FALSE,0);
  gtk_container_add (GTK_CONTAINER (vbox5), hbox2);
  iconImage1 = gtk_image_new_from_file("CoinBicon.png");
  gtk_container_add (GTK_CONTAINER (hbox1), iconImage1);
  scoreLabel1 = gtk_label_new("0");
  gtk_container_add (GTK_CONTAINER (hbox1), scoreLabel1);
  iconImage2 = gtk_image_new_from_file("CoinAicon.png");
  gtk_container_add (GTK_CONTAINER (hbox2), iconImage2);
  scoreLabel2 = gtk_label_new("0");
  gtk_container_add (GTK_CONTAINER (hbox2), scoreLabel2);
  hbox3 = gtk_hbox_new(FALSE,0);
  gtk_container_add (GTK_CONTAINER (vbox5), hbox3);
  winnerLabel = gtk_label_new("Winner:");
  gtk_container_add (GTK_CONTAINER (hbox3), winnerLabel);
  iconImage3 = gtk_image_new_from_file("CoinAicon.png");
  gtk_container_add (GTK_CONTAINER (hbox3), iconImage3);

  /*vbox6 = gtk_vbox_new(TRUE,0);
  gtk_table_attach_defaults (GTK_TABLE(mainTable), vbox6, 0, 4, 2, 3);
  gtk_widget_set_default_size(308,55);
  quitButton = gtk_button_new_with_label("QUIT GAME");
  gtk_button_box_set_child_size(quitButton,308,44);
  gtk_container_add(GTK_CONTAINER (vbox6), quitButton);*/


  turnLabel = gtk_label_new("Copyright 2014 by bizz-enigma.");
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
  //gtk_widget_set_size_request(boardTable, 539, 330);

  for(i=0;i<6;i++){
    for(j=0;j<7;j++){
        coin[i][j] = gtk_image_new_from_file("CoinC.png");
        gtk_table_attach_defaults (GTK_TABLE(boardTable), coin[i][j], j, j+1, t, t+1);
    }
    t-=1;
  }

  gtk_image_set_from_file(coin[0][2],"CoinA.png");
  gtk_image_set_from_file(coin[1][2],"CoinA.png");
  gtk_image_set_from_file(coin[2][2],"CoinB.png");

  /*exitButton = gtk_button_new_with_label ("Exit Game");
  gtk_table_attach_defaults (GTK_TABLE(mainTable), exitButton, 0, 7, 8, 9);*/

  gtk_widget_show_all (win);
  gtk_main ();
  return 0;
}
