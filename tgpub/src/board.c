/***************************************************************************
 *  Original Diku Mud copyright (C) 1990, 1991 by Sebastian Hammer,        *
 *  Michael Seifert, Hans Henrik St{rfeldt, Tom Madsen, and Katja Nyboe.   *
 *                                                                         *
 *  Merc Diku Mud improvments copyright (C) 1992, 1993 by Michael          *
 *  Chastain, Michael Quan, and Mitchell Tse.                              *
 *                                                                         *
 *  In order to use any part of this Merc Diku Mud, you must comply with   *
 *  both the original Diku license in 'license.doc' as well the Merc       *
 *  license in 'license.txt'.  In particular, you may not remove either of *
 *  these copyright notices.                                               *
 *                                                                         *
 *  Much time and thought has gone into this software and you are          *
 *  benefitting.  We hope that you share your changes too.  What goes      *
 *  around, comes around.                                                  *
 ***************************************************************************/

#if defined(macintosh)
#include <types.h>
#else
#include <sys/types.h>
#endif
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "merc.h"


const char board_id[] = "$Id";

/*

===========================================================================
This snippet was written by Erwin S. Andreasen, 4u2@aabc.dk. You may use
this code freely, as long as you retain my name in all of the files. You
also have to mail me telling that you are using it. I am giving this,
hopefully useful, piece of source code to you for free, and all I require
from you is some feedback.

Please mail me if you find any bugs or have any new ideas or just comments.

All my snippets are publically available at:

http://login.dknet.dk/~ea/

If you do not have WWW access, try ftp'ing to login.dknet.dk and examine
the /pub/ea directory.
===========================================================================
 
 Note Board system, (c) 1995-96 Erwin S. Andreasen, 4u2@aabc.dk
 ==============================================================
 
 Basically, the notes are split up into several boards. The boards do not
 exist physically, they can be read anywhere and in any position.
 
 Each of the note boards has its own file. Each of the boards can have its own
 "rights": who can read/write.
 
 Each character has an extra field added, namele the timestamp of the last note
 read by him/her on a certain board.
 
 The note entering system is changed too, making it more interactive. When
 entering a note, a character is put AFK and into a special CON_ state.
 Everything typed goes into the note.
 
 There is also the option of REPLY'ing to a note instead of writing one.
 Replying sets up a thread which can be followed using certain commands.
 
 For the immortals it is possible to purge notes based on age. An Archive
 options is available which moves the notes older than X days into a special
 board. The file of this board should then be moved into some other directory
 during e.g. the startup script and perhaps renamed depending on date.
 
 Note that write_level MUST be >= read_level or else there will be strange
 output in certain functions.
 
 Board DEFAULT_BOARD must be at least readable by *everyone*.
 
 NOTE That the board names are only examples. You should probably modify their
 names to something different, to avoid the same name boards at every MUD! :)
 
*/ 

#define L_IMM (MAX_LEVEL - 4)
#define L_SUP (MAX_LEVEL - 1)


BOARD_DATA boards[MAX_BOARD] =
{

{ "General" , "News of general interest ", 0,     1,     "all", DEF_INCLUDE,14, NULL, FALSE },
{ "Personal", "Addressed to individuals ", 0,     1,     "all", DEF_EXCLUDE, 7, NULL, FALSE },
{ "Clan"    , "Exchange among clan members", 0,     1,     "all", DEF_CLAN, 14, NULL, FALSE },
{ "Announce", "Changes and notices ", 0,     L_IMM, "all", DEF_NORMAL, 30, NULL, FALSE },
{ "Ideas"   , "What do you think? ", 0,     1,     "all", DEF_NORMAL, 21, NULL, FALSE }, 
{ "Bugs"    , "6-legged creatures ", 0,     0,     "imm", DEF_NORMAL, 30, NULL, FALSE },
{ "GodBoard", "Divine exchange ", L_IMM, L_IMM, "imm", DEF_INCLUDE,30, NULL, FALSE },
{ "ImpBoard", "Coal and Puck ", L_SUP, L_SUP, "all", DEF_INCLUDE,30, NULL, FALSE },
{ "Archive" , "Imm-saved notes ", L_IMM, L_IMM, "all", DEF_NORMAL, 30, NULL, FALSE }

};

#define BOARD_NOACCESS -1
#define BOARD_NOTFOUND -1

/* recycle a note */
void free_note (NOTE_DATA *note)
{
	if (note->sender)
		free_string (note->sender);
	if (note->to_list)
		free_string (note->to_list);
	if (note->subject)
		free_string (note->subject);
	if (note->date_stamp)
		free_string (note->date);
	if (note->text)
		free_string (note->text);
		
	note->next = note_free;
	note_free = note;	
}

/* allocate memory for a new note or recycle */
NOTE_DATA *new_note ()
{
	NOTE_DATA *note;
	
	if (note_free)
	{
		note = note_free;
		note_free = note_free->next;
	}
	else
		note = alloc_mem (sizeof(NOTE_DATA));

	/* Zero all the fields, just in case */	
	note->next = NULL;
	note->sender = NULL;		
	note->expire = 0;
	note->to_list = NULL;
	note->subject = NULL;
	note->date = NULL;
	note->date_stamp = 0;
	note->text = NULL;
	
	return note;
}

/* append this note to the given file */
void append_note (FILE *fp, NOTE_DATA *note)
{
	fprintf (fp, "Sender  %s~\n", note->sender);
	fprintf (fp, "Date    %s~\n", note->date);
	fprintf (fp, "Stamp   %ld\n", note->date_stamp);
	fprintf (fp, "Expire  %ld\n", note->expire);
	fprintf (fp, "To      %s~\n", note->to_list);
	fprintf (fp, "Subject %s~\n", note->subject);
	fprintf (fp, "Text\n%s~\n\n", note->text);
}

/* Save a note in a given board */
void finish_note (BOARD_DATA *board, NOTE_DATA *note)
{
	FILE *fp;
	NOTE_DATA *p;
	char filename[200];
	
	if (board->note_first) /* are there any notes in there now? */
	{
		for (p = board->note_first; p->next; p = p->next ); /* empty */
		
		p->next = note;
	}
	else /* nope. empty list. */
		board->note_first = note;

	/* append note to note file */		
	
	sprintf (filename, "%s%s", NOTE_DIR, board->short_name);
	
	fp = fopen (filename, "a");
	if (!fp)
	{
		bug ("Could not open one of the note files in append mode",0);
		board->changed = TRUE; /* set it to TRUE hope it will be OK later? */
		return;
	}
	
	append_note (fp, note);
	fclose (fp);
}

/* Find the number of a board */
int board_number (BOARD_DATA *board)
{
	int i;
	
	for (i = 0; i < MAX_BOARD; i++)
		if (board == &boards[i])
			return i;

	return -1;
}

/* Find a board number based on  a string */
int board_lookup (char *name)
{
	int i;
	
	for (i = 0; i < MAX_BOARD; i++)
		if (!str_cmp (boards[i].short_name, name))
			return i;

	return -1;
}

/* Remove list from the list. Do not free note */
void unlink_note (BOARD_DATA *board, NOTE_DATA *note)
{
	NOTE_DATA *p;
	
	if (board->note_first == note)
		board->note_first = note->next;
	else
	{
		for (p = board->note_first; p && p->next != note; p = p->next);
		if (!p)
			bug ("unlink_note: could not find note.",0);
		else
			p->next = note->next;
	}
}

/* Find the nth note on a board. Return NULL if ch has no access to that note */
NOTE_DATA* find_note (CHAR_DATA *ch, BOARD_DATA *board, int num)
{
	int count = 0;
	NOTE_DATA *p;
	
	for (p = board->note_first; p ; p = p->next)
			if (++count == num)
				break;
	
	if ( (count == num) && is_note_to (ch, p))
		return p;
	else
		return NULL;
	
}

/* save a single board */
void save_board (BOARD_DATA *board)
{
	FILE *fp;
	char filename[200];
	char buf[300];
	NOTE_DATA *note;
	
	sprintf (filename, "%s%s", NOTE_DIR, board->short_name);
	
	fp = fopen (filename, "w");
	if (!fp)
	{
		sprintf (buf, "Error writing to: %s", filename);
		bug (buf, 0);
	}
	else
	{
		for (note = board->note_first; note ; note = note->next)
			append_note (fp, note);
			
		fclose (fp);
	}
}

/* Show one not to a character */
void show_note_to_char (CHAR_DATA *ch, NOTE_DATA *note, int num)
{
	char buf[MAX_STRING_LENGTH];

	/* Ugly colors ? */	
	sprintf (buf,
			 "[%4d] %s: %s\n\r"
	         "Date:  %s\n\r"
			 "To:    %s\n\r"
	         "===========================================================================\n\r"
	         "%s\n\r",
	         num, note->sender, note->subject,
	         note->date,
	         ch->pcdata->board->force_type == DEF_CLAN ? 
		    "Your clan" : note->to_list,
	         note->text);

	send_to_char (buf,ch);	         
}

/* Save changed boards */
void save_notes ()
{
	int i;
	 
	for (i = 0; i < MAX_BOARD; i++)
		if (boards[i].changed) /* only save changed boards */
			save_board (&boards[i]);
}

/* Load a single board */
void load_board (BOARD_DATA *board)
{
	FILE *fp;
	NOTE_DATA *last_note;
	char filename[200];
	
	sprintf (filename, "%s%s", NOTE_DIR, board->short_name);
	
	fp = fopen (filename, "r");
	
	/* Silently return */
	if (!fp)
		return;		
		
	/* Start note fetching. copy of db.c:load_notes() */

    last_note = NULL;

    for ( ; ; )
    {
        NOTE_DATA *pnote;
        char letter;

        do
        {
            letter = getc( fp );
            if ( feof(fp) )
            {
                fclose( fp );
                return;
            }
        }
        while ( isspace((int)letter) );
        ungetc( letter, fp );

        pnote             = alloc_perm( sizeof(*pnote) );

        if ( str_cmp( fread_word( fp ), "sender" ) )
            break;
        pnote->sender     = fread_string( fp );

        if ( str_cmp( fread_word( fp ), "date" ) )
            break;
        pnote->date       = fread_string( fp );

        if ( str_cmp( fread_word( fp ), "stamp" ) )
            break;
        pnote->date_stamp = fread_number( fp );

        if ( str_cmp( fread_word( fp ), "expire" ) )
            break;
        pnote->expire = fread_number( fp );

        if ( str_cmp( fread_word( fp ), "to" ) )
            break;
        pnote->to_list    = fread_string( fp );

        if ( str_cmp( fread_word( fp ), "subject" ) )
            break;
        pnote->subject    = fread_string( fp );

        if ( str_cmp( fread_word( fp ), "text" ) )
            break;
        pnote->text       = fread_string( fp );
        
        pnote->next = NULL; /* jic */

        if ( board->note_first == NULL )
            board->note_first = pnote;
        else
            last_note->next     = pnote;

        last_note         = pnote;
    }

    bug( "Load_notes: bad key word.", 0 );
    return; /* just return */
}

/* Initialize structures. Load all boards. */
void load_boards ()
{
	int i;
	
	for (i = 0; i < MAX_BOARD; i++)
		load_board (&boards[i]);
}

/* Returns TRUE if the specified note is address to ch */
bool is_note_to (CHAR_DATA *ch, NOTE_DATA *note)
{
	if (!str_cmp (ch->name, note->sender))
		return TRUE;
	
	if (is_full_name ("all", note->to_list))
		return TRUE;
		
	if (IS_IMMORTAL(ch) && ( 
		is_full_name ("imm", note->to_list) ||
		is_full_name ("imms", note->to_list) ||
		is_full_name ("immortal", note->to_list) ||
		is_full_name ("god", note->to_list) ||
		is_full_name ("gods", note->to_list) ||
		is_full_name ("immortals", note->to_list)))
		return TRUE;

	if ((get_trust(ch) == MAX_LEVEL) && (
		is_full_name ("imp", note->to_list) ||
		is_full_name ("imps", note->to_list) ||
		is_full_name ("implementor", note->to_list) ||
		is_full_name ("implementors", note->to_list)))
		return TRUE;
		
	if (is_full_name (ch->name, note->to_list))
		return TRUE;

    if (ch->pcdata->board->force_type == DEF_CLAN)
    {
	if (IS_CLASS(ch, CLASS_MAGE) &&
		atoi(note->to_list) == ch->pcdata->powers[MPOWER_RUNE0] &&
		strlen(note->to_list) < 2)
	    return TRUE;
	else if (!str_cmp(ch->clan, note->to_list))
	    return TRUE;
    }

    return FALSE;
}
/* puck */
bool can_remove (CHAR_DATA *ch, NOTE_DATA *note)
{
    if (!str_cmp (ch->name, note->sender))
	return TRUE;

    if (ch->trust == MAX_LEVEL)
	return TRUE;

    return FALSE;
}

/* Return the number of unread notes 'ch' has in 'board' */
/* Returns BOARD_NOACCESS if ch has no access to board */
int unread_notes (CHAR_DATA *ch, BOARD_DATA *board)
{
    NOTE_DATA *note;
    time_t last_read;
    int count = 0;
	
    if (board->read_level > get_trust(ch))
	return BOARD_NOACCESS;

    if (board->force_type == DEF_CLAN)
	if (!IS_CLASS(ch, CLASS_MAGE) && !str_cmp(ch->clan, ""))
	    return BOARD_NOACCESS;
		
    last_read = ch->pcdata->last_note[board_number(board)];
	
    for (note = board->note_first; note; note = note->next)
	if (is_note_to(ch, note) && ((long)last_read < (long)note->date_stamp))
	    count++;
			
    return count;
}

/*
 * COMMANDS
 */

/* Start writing a note */
void do_nwrite (CHAR_DATA *ch, char *argument)
{
	char *strtime;
	char buf[200];
	
	if (IS_NPC(ch)) /* NPC cannot post notes */
		return;
		
	if (get_trust(ch) < ch->pcdata->board->write_level)
	{
		send_to_char ("You cannot post notes on this board.\n\r",ch);
		return;
	}
	
	/* continue previous note, if any text was written*/ 
	if (ch->pcdata->in_progress && (!ch->pcdata->in_progress->text))
	{
		send_to_char ("Note in progress cancelled because you did not manage to write any text \n\r"
		              "before losing link.\n\r\n\r",ch);
		free_note (ch->pcdata->in_progress);		              
	}
	
	
	if (!ch->pcdata->in_progress)
	{
		ch->pcdata->in_progress = new_note();
		ch->pcdata->in_progress->sender = str_dup (ch->name);
		ch->pcdata->in_progress->date_stamp = current_time;

		/* convert to ascii. ctime returns a string which last character is \n, so remove that */	
		strtime = ctime (&current_time);
		strtime[strlen(strtime)-1] = '\0';
	
		ch->pcdata->in_progress->date = str_dup (strtime);
	}

	/* Begin writing the note ! */
	sprintf (buf, "You are now %s a new note on the %s board.\n\r"
	              "If you are using tintin, type ##ver(bose) to turn off alias expansion!\n\r\n\r",
	               ch->pcdata->in_progress->text ? "continuing" : "posting",
	               ch->pcdata->board->short_name);
	send_to_char (buf,ch);
	
	sprintf (buf, "From:    %s\n\r", ch->name);
	send_to_char (buf,ch);

	if (!ch->pcdata->in_progress->text) /* Are we continuing an old note or not? */
	{
	    switch (ch->pcdata->board->force_type)
	    {
		case DEF_NORMAL:
			sprintf (buf, "If you press Return, default recipient \"%s\" will be chosen.\n\r",
					  ch->pcdata->board->names);
			break;
		case DEF_INCLUDE:
			sprintf (buf, "The recipient list MUST include \"%s\". If not, it will be added automatically.\n\r",
						   ch->pcdata->board->names);
			break;
	
		case DEF_EXCLUDE:
			sprintf (buf, "The recipient of this note must NOT include: \"%s\".",
						   ch->pcdata->board->names);
			break;
		case DEF_CLAN:
		    send_to_char( "This note will be automatically addressed to your clan.  Press RETURN.\r\n", ch);
		    ch->desc->connected = CON_NOTE_TO;
		    return;
		    break;
		}			
		
		send_to_char (buf,ch);
		send_to_char ("To:      ",ch);
	
		ch->desc->connected = CON_NOTE_TO;
		/* nanny takes over from here */
		
	}
	else /* we are continuing, print out all the fields and the note so far*/
	{
		sprintf (buf, "To:      %s\n\r"
		              "Expires: %s\n\r"
		              "Subject: %s\n\r", 
		               ch->pcdata->in_progress->to_list,
		               ctime(&ch->pcdata->in_progress->expire),
		               ch->pcdata->in_progress->subject);
		send_to_char (buf,ch);
		send_to_char ("Your note so far:\n\r",ch);
		send_to_char (ch->pcdata->in_progress->text,ch);
		
		send_to_char ("\n\rEnter text. Type ~ or END on an empty line to end note.\n\r"
		                    "=======================================================\n\r",ch);
		

		ch->desc->connected = CON_NOTE_TEXT;		            

	}
	
}


/* Read next note in current group. If no more notes, go to next board */
void do_nread (CHAR_DATA *ch, char *argument)
{
	NOTE_DATA *p;
	int count = 0, number;
	char buf[MAX_STRING_LENGTH];
	time_t *last_note = &ch->pcdata->last_note[board_number(ch->pcdata->board)];
	
	if (!str_cmp(argument, "again"))
	{ /* read last note again */
	
	}
	else if (is_number (argument))
	{
		number = atoi(argument);
		
		for (p = ch->pcdata->board->note_first; p; p = p->next)
			if (++count == number)
				break;
		
		if (!p || !is_note_to(ch, p))
			send_to_char ("No such note.\n\r",ch);
		else
		{
			show_note_to_char (ch,p,count);
			*last_note =  UMAX (*last_note, p->date_stamp);
		}
	}
	else /* just next one */
	{
		count = 1;
		for (p = ch->pcdata->board->note_first; p ; p = p->next, count++)
			if ((p->date_stamp > *last_note) && is_note_to(ch,p))
			{
				show_note_to_char (ch,p,count);
				/* Advance if new note is newer than the currently newest for that char */
				*last_note =  UMAX (*last_note, p->date_stamp);
				return;
			}
		
		send_to_char ("No new notes in this board.\n\r",ch);
		if (next_board (ch))
		    sprintf (buf, "Changed to next board, %s, where there are %d new notes.\n\r", 
			ch->pcdata->board->short_name,
 			unread_notes(ch,&boards[board_number (ch->pcdata->board)]));

// unread_notes(ch,&boards[board_number(ch->pcdata->board) + 1));

		else
		{
		    ch->pcdata->board = &boards[DEFAULT_BOARD];
		    sprintf (buf, "There are no more new notes.\n\r");
		}
		send_to_char (buf,ch);

	}
}

/* Remove a note */
void do_nremove (CHAR_DATA *ch, char *argument)
{
	NOTE_DATA *p;
	
	if (!is_number(argument))
	{
		send_to_char ("Remove which note?\n\r",ch);
		return;
	}

	p = find_note (ch, ch->pcdata->board, atoi(argument));
	if (!p)
	{
		send_to_char ("No such note.\n\r",ch);
		return;
	}

    if (!can_remove(ch, p))
    {
	send_to_char("You can't remove that note.\r\n", ch);
	return;
    }

    unlink_note (ch->pcdata->board,p);
    free_note (p);
    send_to_char ("Note removed.\n\r",ch);
	
    save_board(ch->pcdata->board); /* save the board */
}

/* Purge notes. Deletion is irreversible */
void do_npurge (CHAR_DATA *ch, char *argument)
{
	NOTE_DATA *p, *p_next;
	char buf[MAX_STRING_LENGTH];
	int count = 0;
	
	if (!IS_IMMORTAL(ch))
	{
		send_to_char ("Only immortals can purge notes.\n\r",ch);
		return;
	}
		
	for (p = ch->pcdata->board->note_first; p ; p = p_next)
	{
		p_next = p->next;
		
		if (p->expire < current_time)
		{
			sprintf (buf, "Purging note from %s about %s.\n\r",p->sender,p->subject);		
			send_to_char (buf,ch);
			unlink_note (ch->pcdata->board, p);
			free_note (p);
			count++;
		}
	}
	
	if (count == 0)
		send_to_char ("No notes purged.\n\r",ch);
	else if (count == 1)
		send_to_char ("One note purged.\n\r",ch);
	else
	{
		sprintf (buf, "%d notes purged.\n\r",count);
		send_to_char (buf,ch);
	}
	
	save_board (ch->pcdata->board); /* save changes */
}

/* Archive notes older than their expiration date to another board */
void do_narchive (CHAR_DATA *ch, char *argument)
{
	NOTE_DATA *p, *p_next;
	char buf[MAX_STRING_LENGTH];
	int i,count = 0;
	BOARD_DATA *archive_board;
	char archived_header[100];
	char *strtime;

	if (!IS_IMMORTAL(ch))
	{
		send_to_char ("Only immortals can archive notes.\n\r",ch);
		return;
	}
	
	i = board_lookup (ARCHIVE_BOARD);
	
	if (i == BOARD_NOTFOUND)
	{
		send_to_char ("The Archive board does not exist!\n\r",ch);
		return;
	}
	
	archive_board = &boards[i];
	
	if (archive_board == ch->pcdata->board)
	{
		send_to_char ("Archive notes from the Archive board to the Archive board? Get real.\n\r",ch);
		return;
	}
	
	strtime = ctime (&current_time); /* Wed Jun 30 21:49:08 */
	strtime[16] = '\0';
	sprintf (archived_header, " *** Archived from %s on %s\r\n\r\n",
	                            ch->pcdata->board->short_name, strtime);
	

	for (p = ch->pcdata->board->note_first; p ; p = p_next)
	{
		p_next = p->next;
		
		if (p->expire < current_time)
		{
			sprintf (buf, "Archiving note from %s about %s.\n\r",p->sender,p->subject);		
			send_to_char (buf,ch);
	
			unlink_note (ch->pcdata->board, p); /* Remove from old board */
			
			strcpy (buf, archived_header); /* *** Archived from Announce on Sun 29 Jan\r\n */
			strcat (buf, p->text); /* concat current text */
			free_string (p->text); /* remove current text */
			p->text = str_dup(buf); /* copy new text */
			p->next = NULL; /* important */
			
			finish_note (archive_board, p); /* add to archive board */
			count++;
		}
	}
	
	if (count == 0)
		send_to_char ("No notes archived.\n\r",ch);
	else if (count == 1)
		send_to_char ("One note archived.\n\r",ch);
	else
	{
		sprintf (buf, "%d notes archived.\n\r",count);
		send_to_char (buf,ch);
	}
	

	/* Save changes */
	save_board (ch->pcdata->board);
	save_board (archive_board); 
	
}

/* List all notes or if argument given, list N of the last notes */
/* Shows REAL note numbers! */
void do_nlist (CHAR_DATA *ch, char *argument)
{
	int count= 0, show = 0, num = 0, has_shown = 0;
	time_t last_note;
	NOTE_DATA *p;
	char buf[MAX_STRING_LENGTH];
	
	
	if (is_number(argument))	 /* first, count the number of notes */
	{
		show = atoi(argument);
		
		for (p = ch->pcdata->board->note_first; p; p = p->next)
			if (is_note_to(ch,p))
				count++;
	}
	
	send_to_char ("#gNotes on this board:#n\n\r"
	              "#GNote Author      Subject#n\n\r",ch);
	              
	last_note = ch->pcdata->last_note[board_number (ch->pcdata->board)];
	
	for (p = ch->pcdata->board->note_first; p; p = p->next)
	{
		num++;
		if (is_note_to(ch,p))
		{
			has_shown++; /* note that we want to see X VISIBLE note, not just last X */
			if (!show || ((count-show) < has_shown))
			{
				sprintf (buf, "%3d>%c%-13s %s\n\r",
				               num, 
				               last_note < p->date_stamp ? '*' : ' ',
				               p->sender, p->subject);
				send_to_char (buf,ch);
			}
		}
				              
	}
}

/* catch up with some notes */
void do_ncatchup (CHAR_DATA *ch, char *argument)
{
	NOTE_DATA *p;

	/* Find last note */	
	for (p = ch->pcdata->board->note_first; p && p->next; p = p->next);
	
	if (!p)
		send_to_char ("Alas, there are no notes in that board.\n\r",ch);
	else
	{
		ch->pcdata->last_note[board_number(ch->pcdata->board)] = p->date_stamp;
		send_to_char ("All messages skipped.\n\r",ch);
	}
}

/* Dispatch function for backwards compatibility */
void do_note (CHAR_DATA *ch, char *argument)
{
	char arg[MAX_INPUT_LENGTH];

	if (IS_NPC(ch))
		return;
	
	argument = one_argument (argument, arg);
	
	if ((!arg[0]) || (!str_cmp(arg, "read"))) /* 'note' or 'note read X' */
		do_nread (ch, argument);
		
	else if (!str_cmp (arg, "list"))
		do_nlist (ch, argument);

	else if (!str_cmp (arg, "write"))
		do_nwrite (ch, argument);

	else if (!str_cmp (arg, "remove"))
		do_nremove (ch, argument);
		
	else if (!str_cmp (arg, "purge"))
		do_npurge (ch, argument);
	
	else if (!str_cmp (arg, "archive"))
		do_narchive (ch, argument);
	
	else if (!str_cmp (arg, "catchup"))
		do_ncatchup (ch, argument);
	else 
		do_help (ch, "note");
}

/* Show all accessible boards with their numbers of unread messages OR
   change board. New board name can be given as a number or as a name (e.g.
    board personal or board 4 */
void do_board (CHAR_DATA *ch, char *argument)
{
	int i, count, number;
	char buf[200];
	
	if (IS_NPC(ch))
		return;
	
	if (!argument[0]) /* show boards */
	{
		int unread;
		
		count = 1;
		send_to_char ("#YBoard  Name    Unread      Description\n\r"
			      "#y===========================================#n\n\r",ch);
		for (i = 0; i < MAX_BOARD; i++)
		{
			unread = unread_notes (ch,&boards[i]); /* how many unread notes? */
			if (unread != BOARD_NOACCESS)
			{
				sprintf (buf, "%2d> %9s [%c%4d] %s\n\r", 
				                   count, boards[i].short_name, unread ? 'N' : 'x', 
				                    unread, boards[i].long_name);
				send_to_char (buf,ch);	                    
				count++;
			} /* if has access */
			
		} /* for each board */
		
		sprintf (buf, "\n\rYour current board is %s.\n\r", ch->pcdata->board->short_name);
		send_to_char (buf,ch);

		/* Inform of rights */		
		if (ch->pcdata->board->force_type == DEF_CLAN && !IS_CLASS(ch, CLASS_MAGE) && !strcmp(ch->clan, ""))
			send_to_char ("You cannot read nor write notes on this board.\n\r",ch);
		else if (ch->pcdata->board->read_level > get_trust(ch))
			send_to_char ("You cannot read nor write notes on this board.\n\r",ch);
		else if (ch->pcdata->board->write_level > get_trust(ch))
			send_to_char ("You can only read notes from this board.\n\r",ch);
		else
			send_to_char ("You can both read and write on this board.\n\r",ch);

		return;			
	} /* if empty argument */
	
	/* Change board based on its number */
	if (is_number(argument))
	{
		count = 0;
		number = atoi(argument);
		for (i = 0; i < MAX_BOARD; i++)
			if (unread_notes(ch,&boards[i]) != BOARD_NOACCESS)		
				if (++count == number)
					break;
		
		if (count == number) /* found the board.. change to it */
		{
			ch->pcdata->board = &boards[i];
			sprintf (buf, "Current board changed to %s. %s.\n\r",boards[i].short_name,
			              (get_trust(ch) < boards[i].write_level) 
			              ? "You can only read here" 
			              : "You can both read and write here");
			send_to_char (buf,ch);
		}			
		else /* so such board */
			send_to_char ("No such board.\n\r",ch);
			
		return;
	}

	/* Non-number given, find board with that name */
	
	for (i = 0; i < MAX_BOARD; i++)
		if (!str_cmp(boards[i].short_name, argument))
			break;
			
	if (i == MAX_BOARD)
	{
		send_to_char ("No such board.\n\r",ch);
		return;
	}

	/* Does ch have access to this board? */	
	if (unread_notes(ch,&boards[i]) == BOARD_NOACCESS)
	{
		send_to_char ("No such board.\n\r",ch);
		return;
	}
	
	ch->pcdata->board = &boards[i];
	sprintf (buf, "Current board changed to %s. %s.\n\r",boards[i].short_name,
	              (get_trust(ch) < boards[i].write_level) 
	              ? "You can only read here" 
	              : "You can both read and write here");
	send_to_char (buf,ch);
}

bool next_board (CHAR_DATA *ch)
{
      NOTE_DATA *note;

	int i = board_number (ch->pcdata->board) + 1;
      
      while ( i < MAX_BOARD && 
         ( unread_notes(ch,&boards[i]) == BOARD_NOACCESS || unread_notes(ch,&boards[i]) == 0 ) )
	{
        ch->pcdata->board = &boards[i];
        if ( ch->pcdata->board->force_type == DEF_CLAN && ( IS_CLASS( ch, CLASS_MAGE ) || strlen( ch->clan ) > 1 ) )
        {
          for ( note = ch->pcdata->board->note_first; note != NULL; note = note->next )
          {
            if ( !str_cmp( ch->clan, note->to_list ) && (long)note->date_stamp > (long)ch->pcdata->last_note[i] )
              return TRUE;
          }
        }
        i++;
      }      
		
	if (i == MAX_BOARD)
	    return FALSE;
	else
	{
	    ch->pcdata->board = &boards[i];
	    return TRUE;
	}
}

