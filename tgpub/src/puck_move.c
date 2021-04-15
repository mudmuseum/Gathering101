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
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "merc.h"

/* 
 * This is a variation of the recall command.  Costs more to do and set.
 * Inspired by Mortal Realms "gohome" command.
 */
void do_gohome ( CHAR_DATA *ch, char* argument )
{
    char arg[MAX_INPUT_LENGTH];
    CHAR_DATA *mount;
    ROOM_INDEX_DATA *location;
    sh_int move_cost = 0;
    sh_int primal_cost = 15;

    if (IS_NPC(ch)) return;

    if (ch->pcdata->legend < 5)
    {
	send_to_char("Huh?\r\n", ch);
	return;
    }

    one_argument(argument, arg);

    if ( !str_cmp(arg, "set") )
    {
	if ( ch->pcdata->quest < 500 )
	{
	    send_to_char( "You need 500 qps to set your gohome location.\r\n", ch);	
	    return;
	}
	if ( ch->in_room->vnum == ch->home2 )
	{
            send_to_char( "You already go home to this location.\n\r", ch );
            return;
	}

	if ( IS_SET(ch->in_room->room_flags, ROOM_NO_RECALL) ||
             IS_SET(ch->in_room->room_flags, ROOM_SAFE) )
	{
            send_to_char("You can't set your home here.\n\r", ch);
            return;
	}

	if ( ch->in_room->sector_type == SECT_HQ && 
		!char_can_hqedit(ch))
	{
	    /* 
	     * char_can_hqedit() compares the area name to the ch's class.
	     * This function is located in puck_wiz.c
	     */
	    send_to_char("You can't set your home here.\r\n", ch);
	    return;
	}

	send_to_char("You set your home here.\r\n", ch);
	ch->home2 = ch->in_room->vnum;
	ch->pcdata->quest -= 500;
	return;
    }

    move_cost = UMIN(1000, ch->move/10);

    if (ch->move < move_cost)
    {
	send_to_char("You're too fatigued to go home.\n\r",ch);
	return;
    }
    if (ch->practice < primal_cost)
    {
	send_to_char("You don't have the primal energy to go home.\r\n", ch);
	return;
    }

    act( "$n's body is consumed by blue flames.", ch, NULL, NULL, TO_ROOM );
    act( "Your body is consumed by blue flames.", ch, NULL, NULL, TO_CHAR );

    if ( ( location = get_room_index( ch->home2 ) ) == NULL )
    {
	send_to_char( "You are completely lost.\n\r", ch );
	return;
    }

    if ( ch->in_room == location )
	return;

    if ( IS_SET(ch->in_room->room_flags, ROOM_NO_RECALL)
    ||   IS_AFFECTED(ch, AFF_CURSE) )
    {
	send_to_char( "You are unable to go home.\n\r", ch );
	return;
    }

    if ( ch->position <= POS_FIGHTING )
    {
	send_to_char( "You can't go home right now.\r\n", ch);
	return;
    }

    ch->move -= move_cost;
    ch->practice -= primal_cost;

    act( "$n disappears in a puff of smoke.", ch, NULL, NULL, TO_ROOM );
    char_from_room( ch );
    char_to_room( ch, location );
    act( "$n materializes from a burst of blue flames.", ch, NULL, NULL, TO_ROOM );
    do_look( ch, "auto" );
    if ( ( mount = ch->mount ) == NULL ) return;
    char_from_room( mount );
    char_to_room( mount, ch->in_room );
    return;
}

