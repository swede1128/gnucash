/********************************************************************\
 * Transaction-matcher.c --                                         *
 * See file generic-import-design.txt for                           *
 * description                                                      *
 *                        (GnuCash)                                 *
 * Copyright (C) 2002 Benoit Gr�goire <bock@step.polymtl.ca>        *
 *                                                                  *
 * This program is free software; you can redistribute it and/or    *
 * modify it under the terms of the GNU General Public License as   *
 * published by the Free Software Foundation; either version 2 of   *
 * the License, or (at your option) any later version.              *
 *                                                                  *
 * This program is distributed in the hope that it will be useful,  *
 * but WITHOUT ANY WARRANTY; without even the implied warranty of   *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the    *
 * GNU General Public License for more details.                     *
 *                                                                  *
 * You should have received a copy of the GNU General Public License*
 * along with this program; if not, contact:                        *
 *                                                                  *
 * Free Software Foundation           Voice:  +1-617-542-5942       *
 * 59 Temple Place - Suite 330        Fax:    +1-617-542-2652       *
 * Boston, MA  02111-1307,  USA       gnu@gnu.org                   *
\********************************************************************/

#define _GNU_SOURCE

#include "config.h"


#include <glib.h>
#include <gmodule.h>

#include <glade/glade.h>

#include <stdlib.h> 
#include <math.h>
#include "gnc-generic-import.h"
#include "Account.h"
#include "Transaction.h"
#include "dialog-utils.h"

#include "gnc-engine-util.h"

#include "gnc-ui-util.h"

/********************************************************************\
 *   Constants   *
\********************************************************************/

#define NUM_COLUMNS_DOWNLOADED_CLIST 7
static const int DOWNLOADED_CLIST_ACTION = 0;
static const int DOWNLOADED_CLIST_ACCOUNT = 1;
static const int DOWNLOADED_CLIST_DATE = 2;
static const int DOWNLOADED_CLIST_AMOUNT = 3;
static const int DOWNLOADED_CLIST_DESCRIPTION = 4;
static const int DOWNLOADED_CLIST_MEMO = 5;
static const int DOWNLOADED_CLIST_BALANCED = 6;

#define NUM_COLUMNS_MATCHER_CLIST 5
static const int MATCHER_CLIST_CONFIDENCE = 0;
static const int MATCHER_CLIST_DATE = 1;
static const int MATCHER_CLIST_AMOUNT = 2;
static const int MATCHER_CLIST_DESCRIPTION = 3;
static const int MATCHER_CLIST_MEMO = 4;

/********************************************************************\
 *               Structures passed between the functions             *
\********************************************************************/
/* If you modify this, modify get_next_action */
typedef enum _action{
  IGNORE,
  ADD,
  RECONCILE,
  REPLACE,
  LAST_ACTION,
  INVALID_ACTION
} Action;


struct _transmatcherdialog {
  gboolean initialised;
  GtkWidget * transaction_matcher;
  GtkCList * downloaded_clist;
  GtkCList * match_clist;
  struct _transactioninfo * selected_trans_info;

  char * action_add_text;
  char * action_reconcile_text;       
  char * action_replace_text;       
  char * action_ignore_text;
};

struct _transactioninfo
{

  Transaction * trans;
  Split * first_split;
  const char * action_text;
  char date_text[20];
  char amount_text[20];
  char balance_text[20];
  const char * clist_text[NUM_COLUMNS_DOWNLOADED_CLIST];
  GList * match_list;
  struct _matchinfo * selected_match_info;
  Action action;
  Action previous_action;

};

struct _matchinfo
{
  Transaction * trans;
  Split * split;
  //GNC_match_probability probability;
  gint probability;
  char probability_text[10];
  char date_text[20];
  char amount_text[20];
  const char * clist_text[NUM_COLUMNS_MATCHER_CLIST];
};

/********************************************************************\
 *   Constants, should idealy be defined a user preference dialog    *
\********************************************************************/

static short module = MOD_IMPORT;
static const double MATCH_ATM_FEE_TRESHOLD=3.00;
static const int MATCH_DATE_TRESHOLD=4; /*within 4 days*/
/*Transaction who's best match probability is equal or higher than
  this will reconcile their best match by default */
static const int TRANSACTION_RECONCILE_PROBABILITY_THRESHOLD = 6;
/*Transaction who's best match probability is below or equal to 
  this will be added as new by default */
static const int TRANSACTION_ADD_PROBABILITY_THRESHOLD = 2;
/*Transaction's match probability must be at least this much to be 
  displayed in the match list.  Dont set this to 0 except for 
  debugging purposes, otherwise all transactions of every accounts 
  will be shown in the list */
static const int TRANSACTION_DISPLAY_PROBABILITY_THRESHOLD = 1;
/*Transaction's who have an online_id kvp frame have been downloaded 
  online can probably be skipped in the match list, since it is very 
  unlikely that they would match a transaction downloaded at a later
  date and yet not have the same online_id.  This also increases
  performance of the matcher. */
static const int SHOW_TRANSACTIONS_WITH_UNIQUE_ID = FALSE;

static const int ACTION_IGNORE_ENABLED = TRUE;
static const int ACTION_ADD_ENABLED = TRUE;
static const int ACTION_RECONCILE_ENABLED = TRUE;
static const int ACTION_REPLACE_ENABLED = TRUE;

/* XPM */
static char * fleche_xpm[] = {
"17 22 41 1",
" 	c None",
".	c #FFFFFF",
"+	c #000000",
"@	c #FFFAFF",
"#	c #F6FFF6",
"$	c #EEEEE6",
"%	c #B4B29C",
"&	c #F6F6F6",
"*	c #F6F2F6",
"=	c #EFF7EF",
"-	c #EEF2EE",
";	c #EEEEEE",
">	c #F6EEF6",
",	c #E6EEE6",
"'	c #EEEAEE",
")	c #E6EAE6",
"!	c #EEE6EE",
"~	c #E6E6E6",
"{	c #DEE2DE",
"]	c #E6E2E6",
"^	c #DEDEDE",
"/	c #E6DEE6",
"(	c #DEDADE",
"_	c #D5DED5",
":	c #D5DAD5",
"<	c #DED6DE",
"[	c #D5D6D5",
"}	c #D5D2D5",
"|	c #CDD6CD",
"1	c #CDD2CD",
"2	c #CDCECD",
"3	c #D5CED5",
"4	c #CDCACD",
"5	c #C5CAC5",
"6	c #C5C6C5",
"7	c #CDC6CD",
"8	c #BDC6BD",
"9	c #C5C2C5",
"0	c #C5BEC5",
"a	c #BDC2BD",
"b	c #BDBEBD",
".+++++++++++++++.",
"+@.............#+",
"+.$$$$$$$$$$$$$%+",
"+.$&&*&&&&&=&&&%+",
"+.$*--*-**-*-*-%+",
"+.$;;;;>-;;;;;-%+",
"+.$,',';;';',';%+",
"+.$)!)!)!))))')%+",
"+.$~~~~~~~~~~~~%+",
"+.${]+++++++]]]%+",
"+.${^/+++++/{^{%+",
"+.$(^(_+++((_(^%+",
"+.$:<:(<+<::<(<%+",
"+.$[[<[[[[[<[[[%+",
"+.$}|}}}}|}}}}}%+",
"+.$111112131131%+",
"+.$242442422424%+",
"+.$454545444545%+",
"+.$676676656767%+",
"+.$689689696966%+",
"+0%%%%%%%%%%%%%a+",
"b+++++++++++++++b"};

static  GdkPixmap* gen_probability_pixmap(gint score, struct _transmatcherdialog * matcher)
{
  GdkPixmap* retval = NULL;
  gint i, j;
  const gint height = 15;
  const gint width_each_bar = 7;
  gchar * green_bar = ("bggggb ");
  gchar * yellow_bar = ("byyyyb ");
  gchar * red_bar = ("brrrrb ");
  gchar * black_bar = ("bbbbbb ");
  const gint num_colors = 5;
  gchar * size_str = g_strdup_printf("%d%s%d%s%d%s",width_each_bar*score/*width*/," ",height," ",num_colors," 1"/*characters per pixel*/);
  gchar * none_color_str = g_strdup_printf("  c None");
  gchar * green_color_str = g_strdup_printf("g c green");
  gchar * yellow_color_str = g_strdup_printf("y c yellow");
  gchar * red_color_str = g_strdup_printf("r c red");
  gchar * black_color_str = g_strdup_printf("b c black");
  gchar * xpm[2+num_colors+height];

  /*DEBUG("Begin");*/
  xpm[0]=size_str;
  xpm[1]=none_color_str;
  xpm[2]=green_color_str;
  xpm[3]=yellow_color_str;
  xpm[4]=red_color_str; 
  xpm[5]=black_color_str;
  
  for(i=0;i<height;i++)
    {
      xpm[num_colors+1+i]= g_new0(char,(width_each_bar*score)+1);
      for(j=0;j<score;j++)
	{
	  if(i==0||i==height-1)
	    {
	      strcat(xpm[num_colors+1+i],black_bar);
	    }
	  else
	    {
	      if (j<=TRANSACTION_ADD_PROBABILITY_THRESHOLD-1)
		{
		  strcat(xpm[num_colors+1+i],red_bar);
		}
	      else if (j>=TRANSACTION_RECONCILE_PROBABILITY_THRESHOLD-1)
		{
		  strcat(xpm[num_colors+1+i],green_bar);
		}
	      else
		{
		  strcat(xpm[num_colors+1+i],yellow_bar);
		}
	    }
	}
    }  

    if(score!=0)
      {
	retval =  gdk_pixmap_colormap_create_from_xpm_d    (NULL,
							    gtk_widget_get_colormap(matcher->transaction_matcher),
							    NULL,
							    NULL,
							    xpm);
      }
   for(i=0;i<=num_colors+height;i++)
    { 
      /*DEBUG("free_loop i=%d%s%s",i,": ",xpm[i]);*/
      g_free(xpm[i]);
    }

  return retval;
}

static Action get_next_action(Action current_action)
{
  Action retval = INVALID_ACTION;
  Action i = current_action;
  
  do
    {
      if (i == LAST_ACTION)
	{
	  i=0;
	}
      else
	{
	  i++;
	}
      if(i==IGNORE&&ACTION_IGNORE_ENABLED==TRUE)
	retval = i;
      if(i==ADD&&ACTION_ADD_ENABLED==TRUE)
	retval = i;
      if(i==RECONCILE&&ACTION_RECONCILE_ENABLED==TRUE)
	retval = i;
      if(i==REPLACE&&ACTION_REPLACE_ENABLED==TRUE)
	retval = i;
    }while(retval==INVALID_ACTION&&i!=current_action);
  return retval;
}

static void downloaded_transaction_refresh_gui( struct _transmatcherdialog * matcher,struct _transactioninfo * transaction_info)
{
  gint row_number;
  gint i;
  GdkPixmap* fleche;

  DEBUG("Begin");
  row_number = gtk_clist_find_row_from_data(matcher->downloaded_clist,
					    transaction_info);
  switch(transaction_info->action)
    {
    case ADD: transaction_info->action_text = matcher->action_add_text;
      break;
    case RECONCILE: transaction_info->action_text= matcher->action_reconcile_text;
      break;
    case REPLACE:transaction_info->action_text=matcher->action_replace_text;
      break;
    case IGNORE: transaction_info->action_text=matcher->action_ignore_text;
      break;
    default:
      PERR("Unknown action");
    }
 
  transaction_info->clist_text[DOWNLOADED_CLIST_ACTION]=transaction_info->action_text;/*Action*/
  transaction_info->clist_text[DOWNLOADED_CLIST_ACCOUNT]=xaccAccountGetName(xaccSplitGetAccount(transaction_info->first_split)); /*Account*/
 
  printDateSecs(transaction_info->date_text, xaccTransGetDate(transaction_info->trans));
  transaction_info->clist_text[DOWNLOADED_CLIST_DATE]=transaction_info->date_text; /*Date*/

  sprintf(transaction_info->amount_text, 
	  "%.2f",
	  gnc_numeric_to_double(xaccSplitGetAmount(transaction_info->first_split)));
  transaction_info->clist_text[DOWNLOADED_CLIST_AMOUNT]=transaction_info->amount_text;

  transaction_info->clist_text[DOWNLOADED_CLIST_DESCRIPTION]=xaccTransGetDescription(transaction_info->trans);
  transaction_info->clist_text[DOWNLOADED_CLIST_MEMO]=xaccSplitGetMemo(transaction_info->first_split);
 
  sprintf(transaction_info->balance_text, 
	  "%.2f",
	  gnc_numeric_to_double(xaccTransGetImbalance(transaction_info->trans)));
  transaction_info->clist_text[DOWNLOADED_CLIST_BALANCED]=transaction_info->balance_text;

  for(i=0;i<NUM_COLUMNS_DOWNLOADED_CLIST;i++)
    {
      gtk_clist_set_text              (matcher->downloaded_clist,
				       row_number,
				       i,
				       transaction_info->clist_text[i]);
    } 

  fleche =  gdk_pixmap_colormap_create_from_xpm_d    (NULL,
						      gtk_widget_get_colormap(matcher->transaction_matcher),
						      NULL,
						      NULL,
						      fleche_xpm);
  
  gtk_clist_set_pixtext           (matcher->downloaded_clist,
				   row_number,
				   DOWNLOADED_CLIST_ACTION,
				   transaction_info->action_text,
				   3,
				   fleche,
				   NULL);
 
  gtk_clist_set_row_height        (matcher->downloaded_clist,
				   23);
}


/********************************************************************\
 *                                                                   *
 *                       GUI callbacks                               *
 *                                                                   *
\********************************************************************/

static void
downloaded_transaction_select_cb (GtkCList *clist,
				  gint row,
				  gint column,
				  GdkEventButton *event,
				  gpointer user_data) {
  struct _transmatcherdialog * matcher = user_data;
  struct _matchinfo * match_info;
  GList * list_element;
  gint row_number;
  gboolean valid_action_found;
  DEBUG("row: %d%s%d",row,", column: ",column);
  
  if(  matcher->selected_trans_info != gtk_clist_get_row_data(clist, row) 
       && matcher->selected_trans_info != NULL 
       && column == DOWNLOADED_CLIST_ACTION)
    {
      /*We just screwed up the action of a previous selection because of the way GTK automatically
	calls unselect row.  Let's fix it*/
      matcher->selected_trans_info->action=matcher->selected_trans_info->previous_action;
      downloaded_transaction_refresh_gui(matcher,matcher->selected_trans_info);
      row_number = gtk_clist_find_row_from_data(matcher->downloaded_clist,
						matcher->selected_trans_info);
      gtk_clist_unselect_row (clist,
			      row_number,
			      DOWNLOADED_CLIST_AMOUNT);/* Anything but DOWNLOADED_CLIST_ACTION */
    }

  matcher->selected_trans_info = gtk_clist_get_row_data(clist, row);
  

  gtk_clist_clear(matcher->match_clist);
  list_element = g_list_first(matcher->selected_trans_info->match_list);
  while(list_element!=NULL)
    {
      match_info = list_element->data;
      row_number = gtk_clist_append(matcher->match_clist,
				    (char **)(match_info->clist_text)); 
      gtk_clist_set_row_data          (matcher->match_clist,
				       row_number,
				       match_info);
      if(match_info->probability!=0)
	{
	  gtk_clist_set_pixtext           (matcher->match_clist,
					   row_number,
					   MATCHER_CLIST_CONFIDENCE,
					   match_info->probability_text,
					   3,
					   gen_probability_pixmap(match_info->probability, matcher),
					   NULL);
	}
      
      gtk_clist_set_row_height        (matcher->match_clist,
				       0);

      if(match_info==matcher->selected_trans_info->selected_match_info)
	{
	  gtk_clist_select_row            (matcher->match_clist,
					   row_number,
					   0);
	}
      
      list_element=g_list_next(list_element);
    }

  if(column == DOWNLOADED_CLIST_ACTION)
    {
      matcher->selected_trans_info->previous_action=matcher->selected_trans_info->action;
      valid_action_found=FALSE;
      while(valid_action_found==FALSE)
	{
	  matcher->selected_trans_info->action=get_next_action(matcher->selected_trans_info->action);
	  valid_action_found=TRUE;
	  if(matcher->selected_trans_info->selected_match_info==NULL&&
	     (matcher->selected_trans_info->action==REPLACE||matcher->selected_trans_info->action==RECONCILE))
	    {
	      valid_action_found=FALSE;
	    }
	}
    }

  downloaded_transaction_refresh_gui(matcher,matcher->selected_trans_info);
}

static void
downloaded_transaction_unselect_cb(GtkCList *clist,
				   gint row,
				   gint column,
				   GdkEventButton *event,
				   gpointer user_data) {
  struct _transmatcherdialog * matcher = user_data;
  DEBUG("row: %d%s%d",row,", column: ",column);

  if(  matcher->selected_trans_info == gtk_clist_get_row_data(clist, row) && column == DOWNLOADED_CLIST_ACTION)
    {
      gtk_clist_select_row (clist,
			    row,
			    column);
    }
  else
    {
      matcher->selected_trans_info = NULL;
      gtk_clist_clear(matcher->match_clist);
      
    }
}

static void
match_transaction_select_cb (GtkCList *clist,
			     gint row,
			     gint column,
			     GdkEventButton *event,
			     gpointer user_data) {
  struct _transmatcherdialog * matcher = user_data;
  DEBUG("row: %d%s%d",row,", column: ",column);
  matcher->selected_trans_info->selected_match_info=gtk_clist_get_row_data(clist,
									   row);
}

static void
match_transaction_unselect_cb(GtkCList *clist,
			      gint row,
			      gint column,
			      GdkEventButton *event,
			      gpointer user_data) {
  struct _transmatcherdialog * matcher = user_data;
  DEBUG("row: %d%s%d",row,", column: ",column);
  matcher->selected_trans_info->selected_match_info=NULL;

  /*You can't replace or reconcile a nonexistent transaction*/
  if(matcher->selected_trans_info->action==RECONCILE||
     matcher->selected_trans_info->action==REPLACE)
    {
      matcher->selected_trans_info->action=IGNORE;
      downloaded_transaction_refresh_gui(matcher,matcher->selected_trans_info);
    }
}

static void 
on_matcher_apply_clicked (GtkButton *button,
			  gpointer user_data)
{
  struct _transmatcherdialog * matcher = user_data;
  struct _transactioninfo * transaction_info;
  gint i;
  gint row_number;

  DEBUG("Begin");
  transaction_info = (struct _transactioninfo *) gtk_clist_get_row_data(matcher->downloaded_clist,
									0);
  for(i=1;transaction_info!=NULL;i++)
    {
      DEBUG("Iteration %d",i);
      if(transaction_info->action!=IGNORE)
	{
	  if(transaction_info->action==ADD)
	    {
	      xaccSplitSetReconcile(transaction_info->first_split,CREC);
	      /*Set reconcile date to today*/
	      xaccSplitSetDateReconciledSecs(transaction_info->first_split,time(NULL));
	      xaccTransCommitEdit(transaction_info->trans);
	    }
	  else if(transaction_info->action==RECONCILE)
	    {
	      if(transaction_info->selected_match_info->split==NULL)
		{
		  PERR("The split I am trying to reconcile is NULL, shouldn't happen!");
		}
	      else
		{
		  /* Reconcile the matching transaction */
		  xaccTransBeginEdit(transaction_info->selected_match_info->trans);
		  xaccSplitSetReconcile(transaction_info->selected_match_info->split,YREC);
		  /*Set reconcile date to today*/
		  xaccSplitSetDateReconciledSecs(transaction_info->selected_match_info->split,time(NULL));
		  /* Copy the online id to the reconciled transaction, so the match will be remembered */ 
		  gnc_import_set_trans_online_id(transaction_info->selected_match_info->trans, gnc_import_get_trans_online_id(transaction_info->trans));

		  xaccTransCommitEdit(transaction_info->selected_match_info->trans);
		  
		  /* Erase the downloaded transaction */
		  xaccTransRollbackEdit(transaction_info->trans);
		  xaccTransCommitEdit(transaction_info->trans);
		}
	    }
	  else if(transaction_info->action==REPLACE)
	    {
	      if(transaction_info->selected_match_info->split==NULL)
		{
		  PERR("The split I am trying to replace is NULL, shouldn't happen!");
		}
	      else
		{
		  DEBUG("Deleting the previous transaction");
		  /*Erase the matching transaction*/
		  xaccTransBeginEdit(transaction_info->selected_match_info->trans);
		  xaccTransDestroy(transaction_info->selected_match_info->trans);
		  xaccTransCommitEdit(transaction_info->selected_match_info->trans);
		  
		  /*Replace it with the new one*/
		  xaccSplitSetReconcile(transaction_info->first_split,CREC);
		  /*Set reconcile date to today*/
		  xaccSplitSetDateReconciledSecs(transaction_info->first_split,time(NULL));
		  xaccTransCommitEdit(transaction_info->trans);
		} 
	    }
	  row_number = gtk_clist_find_row_from_data(matcher->downloaded_clist,
						    transaction_info);
	  gtk_clist_remove(matcher->downloaded_clist,
			   row_number);
	  i--;
	}
      transaction_info = (struct _transactioninfo *) gtk_clist_get_row_data(matcher->downloaded_clist,
									    i);
    }
}

static void 
on_matcher_cancel_clicked (GtkButton *button,
			   gpointer user_data)
{
  struct _transmatcherdialog * matcher = user_data;
  DEBUG("Begin");
  gtk_clist_clear(matcher->downloaded_clist);
  matcher->initialised=FALSE;
  gtk_widget_destroy(matcher->transaction_matcher);
  g_free (matcher->action_add_text);
  g_free (matcher->action_reconcile_text);
  g_free (matcher->action_replace_text);    
  g_free (matcher->action_ignore_text);
}

static void 
on_matcher_ok_clicked (GtkButton *button,
		       gpointer user_data)
{
  DEBUG("Begin");
  on_matcher_apply_clicked (button,
			    user_data);
  on_matcher_cancel_clicked (button,
			     user_data);
}

/********************************************************************\
 * remove_downloaded_transaction() 
\********************************************************************/
static void
downloaded_trans_row_destroy_cb(gpointer data)
{
  struct _transactioninfo * transaction_info = data;
  DEBUG("Begin");
  g_list_free (transaction_info->match_list);
  /*If the transaction is still open, it must be destroyed*/
  if(xaccTransIsOpen(transaction_info->trans)==TRUE)
    {
      xaccTransDestroy(transaction_info->trans);
      xaccTransCommitEdit(transaction_info->trans);
    }
  g_free(transaction_info);
}

/********************************************************************\
 * init_matcher_gui()
\********************************************************************/
static void
init_matcher_gui(struct _transmatcherdialog * matcher)
{
  GladeXML *xml;
  
  DEBUG("Begin...");
  /* load the interface */
  xml = gnc_glade_xml_new ("generic-import.glade", "transaction_matcher");
  /* connect the signals in the interface */
  if(xml==NULL)
    {
      PERR("Error opening the glade interface\n");
    }
  
  glade_xml_signal_connect_data(xml,
				"downloaded_transaction_select_cb",
				GTK_SIGNAL_FUNC(downloaded_transaction_select_cb), 
				matcher);
  glade_xml_signal_connect_data(xml,
				"downloaded_transaction_unselect_cb", 
				GTK_SIGNAL_FUNC(downloaded_transaction_unselect_cb),
				matcher);
  glade_xml_signal_connect_data(xml,
				"match_transaction_select_cb", 
				GTK_SIGNAL_FUNC(match_transaction_select_cb),
				matcher);
  glade_xml_signal_connect_data(xml,
				"match_transaction_unselect_cb", 
				GTK_SIGNAL_FUNC(match_transaction_unselect_cb),
				matcher);
  glade_xml_signal_connect_data(xml,
				"on_matcher_ok_clicked", 
				GTK_SIGNAL_FUNC(on_matcher_ok_clicked),
				matcher);
  glade_xml_signal_connect_data(xml,
				"on_matcher_apply_clicked", 
				GTK_SIGNAL_FUNC(on_matcher_apply_clicked),
				matcher);
  glade_xml_signal_connect_data(xml,
				"on_matcher_cancel_clicked", 
				GTK_SIGNAL_FUNC(on_matcher_cancel_clicked),
				matcher);


  matcher->transaction_matcher = glade_xml_get_widget (xml, "transaction_matcher");
  matcher->downloaded_clist = (GtkCList *)glade_xml_get_widget (xml, "downloaded_clist");
  matcher->match_clist =  (GtkCList *)glade_xml_get_widget (xml, "match_clist");

  matcher->action_add_text = g_strdup(_("ADD"));
  matcher->action_reconcile_text =  g_strdup(_("RECONCILE"));       
  matcher->action_replace_text =  g_strdup(_("REPLACE"));       
  matcher->action_ignore_text =  g_strdup(_("IGNORE"));

  gtk_widget_show(matcher->transaction_matcher);  
  matcher->initialised=TRUE;  
    
}/* end init_matcher_gui */

/********************************************************************\
 * split_find_match() 
 * The main function for transaction matching.  The heuristics are
 * here. 
\********************************************************************/
static void split_find_match( gpointer data, gpointer user_data)
{
  Split * split = data;
  struct _transactioninfo * transaction_info=user_data;
  struct _matchinfo * match_info;
  double downloaded_split_amount;
  double match_split_amount;
  time_t temp_time_t;
  struct tm downloaded_split_date;
  struct tm match_split_date;
  DEBUG("Begin");

  /*Ignore the split if the transaction is open for edit, meaning it was just downloaded
    Ignore the split if the transaction has an online ID, unless overriden in prefs */
  if(xaccTransIsOpen(xaccSplitGetParent(split))==FALSE&&
     (gnc_import_get_trans_online_id(xaccSplitGetParent(split))==NULL || SHOW_TRANSACTIONS_WITH_UNIQUE_ID==TRUE))
    {
      match_info = g_new0(struct _matchinfo,1);
    
      match_info->split=split;
      match_info->trans = xaccSplitGetParent(split);
    
      downloaded_split_amount=gnc_numeric_to_double(xaccSplitGetAmount(transaction_info->first_split));
      DEBUG(" downloaded_split_amount=%f", downloaded_split_amount);
      match_split_amount=gnc_numeric_to_double(xaccSplitGetAmount(split));
      DEBUG(" match_split_amount=%f", match_split_amount);
      temp_time_t = xaccTransGetDate(xaccSplitGetParent(split));
      match_split_date = *localtime(&temp_time_t);
      temp_time_t = xaccTransGetDate(transaction_info->trans);
      downloaded_split_date = *localtime(&temp_time_t);
    
      /* Matching heuristics */
      match_info->probability=0;
    
      /* Amount heuristics */
      if(gnc_numeric_equal(xaccSplitGetAmount(transaction_info->first_split),
			   xaccSplitGetAmount(split)))
	{
	  match_info->probability=match_info->probability+2;
	  DEBUG("heuristics:  probability + 2 (amount)");
	}
      else if(fabs(downloaded_split_amount-match_split_amount)<=MATCH_ATM_FEE_TRESHOLD)
	{
	  /* ATM fees are sometimes added directly in the transaction.  So you withdraw 100$ and get charged 101,25$
	     in the same transaction */ 
	  match_info->probability=match_info->probability+1;
	  DEBUG("heuristics:  probability + 1 (amount)");
	}
    
      /* Date heuristics */
      if(downloaded_split_date.tm_year==match_split_date.tm_year)
	{
	  if(downloaded_split_date.tm_yday==match_split_date.tm_yday)
	    {
	      match_info->probability=match_info->probability+2;
	      DEBUG("heuristics:  probability + 2 (date)");
	    }
	  else if(downloaded_split_date.tm_yday>match_split_date.tm_yday&&
		  downloaded_split_date.tm_yday<=match_split_date.tm_yday+MATCH_DATE_TRESHOLD)
	    {
	      match_info->probability=match_info->probability+1;
	      DEBUG("heuristics:  probability + 1 (date)");
	    }
	  /*Note: The above won't won't work as expected for transactions close to the end of the year.
	    So we have this special case to handle it:*/
	  else if(downloaded_split_date.tm_year==match_split_date.tm_year+1&&
		  match_split_date.tm_yday+MATCH_DATE_TRESHOLD>=365&&
		  downloaded_split_date.tm_yday<=match_split_date.tm_yday+MATCH_DATE_TRESHOLD-365)
	    {
	      match_info->probability=match_info->probability+1;
	      DEBUG("heuristics:  probability + 1 (date special case)");
	    }
	}
    
      /* Memo and Description heuristics */  
      if((strcmp(xaccSplitGetMemo(transaction_info->first_split),
		 xaccSplitGetMemo(match_info->split))
	  ==0)
	 ||
	 (strcmp(xaccTransGetDescription(transaction_info->trans),
		 xaccTransGetDescription(xaccSplitGetParent(match_info->split)))
	  ==0))
	{	
	  /*An exact match gives a +2, but someone should write something more fuzzy, 
	    worth a +1*/
	  match_info->probability=match_info->probability+2;
	  DEBUG("heuristics:  probability + 2 (description or memo)");
	}
    
      if(gnc_import_get_trans_online_id(xaccSplitGetParent(split))!=NULL)
	{
	  /*If the pref is to show match even with online ID's, reverse the confidence value to distinguish them */
	  match_info->probability=0-match_info->probability;
	}
  
      sprintf(match_info->probability_text, 
	      "%d",
	      match_info->probability);
      match_info->clist_text[MATCHER_CLIST_CONFIDENCE]=match_info->probability_text;/*Probability*/
    
      printDateSecs(match_info->date_text,
		    xaccTransGetDate(xaccSplitGetParent(split)));
      match_info->clist_text[MATCHER_CLIST_DATE]=match_info->date_text; /*Date*/
      sprintf(match_info->amount_text, 
	      "%.2f",
	      match_split_amount);
    
      match_info->clist_text[MATCHER_CLIST_AMOUNT]=match_info->amount_text;/*Amount*/
    
      match_info->clist_text[MATCHER_CLIST_DESCRIPTION]=xaccTransGetDescription(xaccSplitGetParent(split));/*Description*/
    
      match_info->clist_text[MATCHER_CLIST_MEMO]=xaccSplitGetMemo(split);/*Split memo*/
    
      if(match_info->probability >= TRANSACTION_DISPLAY_PROBABILITY_THRESHOLD)
	{
	  transaction_info->match_list = g_list_append(transaction_info->match_list,
						   match_info);
	}
      else
	{
	  g_free(match_info);
	}
    }
}/* end split_find_match */

/* compare_probability() is used by g_list_sort to sort by probability */
static gint compare_probability (gconstpointer a,
				 gconstpointer b)
{
  return(((struct _matchinfo *)b)->probability - ((struct _matchinfo *)a)->probability);
}

/********************************************************************\
 * check_trans_online_id() Weird function, to be used by 
 * xaccAccountForEachTransaction.  Takes pointers to two transaction
 * and returns TRUE if their online_id kvp_frame do NOT match or
 * if both pointers point to the same transaction 
\********************************************************************/
static gboolean check_trans_online_id(Transaction *trans1, void *trans2)
{
  gchar * online_id1 = gnc_import_get_trans_online_id(trans1);
  gchar * online_id2 = gnc_import_get_trans_online_id((Transaction *)trans2);

  if(trans1==(Transaction *)trans2||online_id1==NULL||online_id2==NULL||strcmp(online_id1, online_id2)!=0)
    {
      return TRUE;
    }
  else
    {
      //printf("test_trans_online_id(): Duplicate found\n");
      return FALSE;
    }
}

/********************************************************************\
 * gnc_import_add_trans(Transaction *trans) 
 * The transaction passed to this function must contain at least
 * one split, and this split must have been associated with an account
 * Only the first split will be used for matching.  The 
 * transaction should not be commited.
\********************************************************************/
void gnc_import_add_trans(Transaction *trans)
{
  static struct _transmatcherdialog * matcher;
  gint i;
  Account * dest_acct;
  gboolean trans_not_found=TRUE;
  struct _transactioninfo * transaction_info = NULL;
  gint row_number;
  struct _matchinfo * best_match;
  Split * source_split;

  gnc_should_log(MOD_IMPORT, GNC_LOG_TRACE);

  DEBUG("%s", "Begin...");

  /*For each split in the transaction, check if the parent account contains a transaction
    with the same online id.*/
  for(i=0;(source_split=xaccTransGetSplit(trans,i))!=NULL&&(trans_not_found==TRUE);i++)
    {
      DEBUG("%s%d%s","Checking split ",i," for duplicates");
      dest_acct=xaccSplitGetAccount(source_split);
      trans_not_found = xaccAccountForEachTransaction(dest_acct,
						      check_trans_online_id,
						      trans);
    }
  /*If it does, abort the process for this transaction, since it is already in the system */
  if(trans_not_found==FALSE)
    {
      DEBUG("%s","Transaction with same online ID exists, destroying current transaction");
      xaccTransDestroy(trans);
      xaccTransCommitEdit(trans);
    }
  else
    {
      if(matcher == NULL)
	{
	  DEBUG("Gui not yet opened");
	  matcher = g_new0(struct _transmatcherdialog, 1);
	  init_matcher_gui(matcher);
	  DEBUG("Gui init done");
	}
      else if(matcher->initialised==FALSE)
	{
	  init_matcher_gui(matcher);
	  DEBUG("Matcher reinitialised");
	}
      
      transaction_info=g_new0(struct _transactioninfo,1);
      
      transaction_info->first_split=xaccTransGetSplit(trans,0);/*Only use first split, the source split*/
      transaction_info->trans=trans;
      
      g_list_foreach( xaccAccountGetSplitList(xaccSplitGetAccount(transaction_info->first_split)),
		      split_find_match,
		      (gpointer)transaction_info);
      
      /*WRITEME:  sort match list and determine default action*/
      transaction_info->match_list=g_list_sort(transaction_info->match_list,
					       compare_probability);
      best_match=g_list_nth_data(transaction_info->match_list,0);
      if(best_match != NULL && 
	 best_match->probability >= TRANSACTION_RECONCILE_PROBABILITY_THRESHOLD)
	{
	  transaction_info->action=RECONCILE;
	  transaction_info->selected_match_info=best_match;
	}
      else if(best_match == NULL ||
	      best_match->probability<=TRANSACTION_ADD_PROBABILITY_THRESHOLD)
	{
	  transaction_info->action=ADD;
	}
      else
	{
	  transaction_info->action=IGNORE;
	}
      
      transaction_info->previous_action=transaction_info->action;
      row_number = gtk_clist_append(matcher->downloaded_clist,
				    (char **)(transaction_info->clist_text));
      gtk_clist_set_row_data_full(matcher->downloaded_clist,
				  row_number,
				  transaction_info,
				  downloaded_trans_row_destroy_cb);
      downloaded_transaction_refresh_gui(matcher,
					 transaction_info);
    }
  return;
}/* end gnc_import_add_trans() */


