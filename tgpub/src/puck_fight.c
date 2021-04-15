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


void do_divert (CHAR_DATA *ch, char *argument)
{
    CHAR_DATA *victim;
    char arg [MAX_INPUT_LENGTH];
    CHAR_DATA *opponent;

    one_argument(argument, arg);

    if ( arg[0] == '\0' )
    {
	send_to_char("Divert <target> to divert the attack to that person.\r\n", ch);
	return;
    }

    if ( IS_NPC(ch) ) return;

    if ( ch->pcdata->legend < 6 )
    {
	send_to_char("Huh?\r\n", ch);
	return;
    }

    if (( victim = get_char_room(ch, arg)) == NULL )
    {
	send_to_char("They aren't here.\r\n", ch);
	return;
    }

    if ( IS_NPC(victim) )
    {
	send_to_char("Only to another member of your group.\r\n", ch);
	return;
    }

    if (!is_same_group(ch, victim) )
    {
	send_to_char("Only to another member of your group.\r\n", ch);
	return;
    }
    
    if ( victim == ch )
    {
	send_to_char("That would be pointless...\r\n", ch);
	return;
    }

    if (ch->position != POS_FIGHTING || ch->fighting == NULL)
    {
	send_to_char("What are you trying to divert? Attention?\r\n", ch);
	return;
    }

    if (victim->position < POS_FIGHTING)
    {
	send_to_char("They aren't ready for that.\r\n", ch);
	return;
    }

    opponent = ch->fighting;

    if (opponent == NULL)
    {
	bug("Divert: opponent NULL", 0);
	return;
    }

    if ( ch->in_room->vnum != victim->in_room->vnum )
    {
	send_to_char("The gang's not all here.\r\n", ch);
	return;
    }

    if ( IS_AFFECTED(opponent, AFF_ETHEREAL) && 
	!IS_AFFECTED(victim, AFF_ETHEREAL) )
    {
        send_to_char( "Ethereal opponents can only be diverted to other ethereal people.\n\r", ch );
        return;
    }

    if ( !IS_AFFECTED(opponent, AFF_ETHEREAL) &&
        IS_AFFECTED(victim, AFF_ETHEREAL) )
    {
        send_to_char( "Ethereal opponents can only be diverted to other ethereal people.\n\r", ch );
        return;
    }

    if ( IS_AFFECTED(opponent, AFF_SHADOWPLANE) && !IS_AFFECTED(victim, AFF_SHADOWPLANE) )
    {
        send_to_char( "They are too insubstantial!\r\n", ch);
        return;
    }
    if ( !IS_AFFECTED(opponent, AFF_SHADOWPLANE) && IS_AFFECTED(victim, AFF_SHADOWPLANE) )
    {
        send_to_char( "They are too insubstantial!\r\n", ch);
        return;

    }

    if ( IS_SET(ch->in_room->room_flags,ROOM_SAFE) )
    {
        send_to_char( "You cannot fight in a safe room.\n\r", ch );
        return;
    }


    one_hit(victim, opponent, TYPE_UNDEFINED, 1);
    
    opponent->fighting = victim;

    one_hit( opponent, victim, TYPE_UNDEFINED, 1);

    WAIT_STATE(ch, 12);

    act("$n diverts the attack to you!", ch, NULL, victim, TO_VICT);
    act("You divert the attack to $N!", ch, NULL, victim, TO_CHAR);
    act("$n diverts the attack to $N!", ch, NULL, victim, TO_NOTVICT);

    return;
}

/*
 * Handles deflected damage from the ITEMA_DEFLECT affect.  Called from
 * multi_hit().  We want a random percent of the damage done to a random
 * number of people in the same room as victim.  First, decide how many
 * "pieces" it will break off into.  Then randomly hit people in the room
 * until all pieces have landed.  Some pieces may hit victim, some may hit
 * ch, and it is valid to be hit multiple times.
 */
void deflect_attack (CHAR_DATA *ch, CHAR_DATA *victim, int dam, int dt)
{
}
