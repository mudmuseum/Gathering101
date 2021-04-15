/* Includes for board system 
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
*/

#define DEF_NORMAL  0 /* No forced change, but default (any string)   */
#define DEF_INCLUDE 1 /* 'names' MUST be included (only ONE name!)    */
#define DEF_EXCLUDE 2 /* 'names' must NOT be included (one name only) */

#define MAX_BOARD 9

#define DEFAULT_BOARD 0 /* default board is board #0 in the board_table */
                        /* It should be readable by everyone!!          */
                        
#define MAX_LINE_LENGTH 75 /* enforce a max length of 75 on text lines, reject longer lines */
			   /* This only applies in the Body of the note */                        
						   
#define MAX_NOTE_TEXT (MAX_STRING_LENGTH - 1000)
						
			/* The reason to the -1000 is so we can sprintf
			   the WHOLE note *including* To: From: Expire into a
			   buffer and then show it, and also have space
			   enough to add a (Continued in next message...) line
			   when splitting a message AND the
			   * Originally posted on <Board Name>
			   message when archiving notes.
			*/						   


#define NOTE_DIR	"../area/notes/" /* directory in which to store note files */

#define BOARD_NOTFOUND -1 /* Error code from board_lookup() and board_number */

#define ARCHIVE_BOARD "Archive" /* Name of the board to archive notes to */

/* Data about a board */
struct board_data
{
	char *short_name; /* Max 8 chars */
	char *long_name;  /* Explanatory text, should be no more than 40 ? chars */
	
	sh_int read_level; /* minimum level to see board */
	sh_int write_level;/* minimum level to post notes */

	char *names;       /* Default recipient */
	sh_int force_type; /* Default action (DEF_XXX) */
	
	sh_int purge_days; /* Default expiration */

	NOTE_DATA *note_first;		
	bool changed; /* currently unused */
		
};

typedef struct board_data BOARD_DATA;


/* External variables */

BOARD_DATA boards[MAX_BOARD]; /* Define */


/* Prototypes */

void finish_note (BOARD_DATA *board, NOTE_DATA *note); /* attach a note to a board */
void free_note   (NOTE_DATA *note); /* deallocate memory used by a note */
void load_boards (void); /* load all boards */
int board_lookup (char *name); /* Find a board with that name */
bool is_note_to (CHAR_DATA *ch, NOTE_DATA *note); /* is tha note to ch? */

/* Commands */

DECLARE_DO_FUN (do_note		);
DECLARE_DO_FUN (do_board	);
