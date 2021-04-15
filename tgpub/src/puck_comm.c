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

void subscribe	args( ( CHAR_DATA *ch, char *argument) );

void do_talk (CHAR_DATA *ch, char *argument)
{
    int chan, req;
    DESCRIPTOR_DATA *d;
    char buf[MAX_STRING_LENGTH];

    if (IS_NPC(ch)) return;
    if (ch->pcdata->legend < 3)
    {
	send_to_char("Huh?\r\n", ch);
	return;
    }

    chan = ch->pcdata->powers[TALK_CHANNEL];
    req  = ch->pcdata->powers[TALK_REQUESTED];

    if (!chan && !req)
    {
	send_to_char("You haven't subscribed to a channel yet.\r\n", ch);
	return;
    }

    if ( req )
    {
	sprintf(buf, "You haven't been granted access to channel %d.\r\n", req);
	send_to_char(buf, ch);
	return;
    }

    if ( argument[0] == '\0' )
    {
	send_to_char("Talk what?\r\n", ch);
	return;
    }

    for ( d = descriptor_list; d != NULL; d = d->next )
    {
        CHAR_DATA *vch = d->character;

        if ( d->connected == CON_PLAYING && vch != ch && !IS_NPC(vch) && 
	     chan == vch->pcdata->powers[TALK_CHANNEL])
	{
	    sprintf(buf, "#g%s talks '%s'.#n\r\n", 
		ch->name, argument);
	    send_to_char(buf, vch);
	}
    }

    sprintf(buf, "#gYou talk '%s'.#n\r\n", argument);
    send_to_char(buf, ch);

    return;
}

void do_talkchannels (CHAR_DATA *ch, char *argument)
{
    DESCRIPTOR_DATA *d;
    bool found = FALSE;
    char buf[MAX_STRING_LENGTH * 2];
    char arg[MAX_INPUT_LENGTH];

    one_argument(argument,arg);

    if (arg[0] == '\0')
    {

    for ( d = descriptor_list; d != NULL; d = d->next )
    {
	CHAR_DATA *vch = d->original ? d->original : d->character;
	if (vch == NULL) continue;

	if ( d->connected == CON_PLAYING && !IS_NPC(vch) &&
	     vch->pcdata->powers[TALK_CHANNEL] != 0 && !IS_IMMORTAL(vch))
	{
	    found = TRUE;
	    sprintf(buf, "%-20s %2d\r\n",
		vch->name, vch->pcdata->powers[TALK_CHANNEL]);
	    send_to_char(buf, ch);
	}
    }
    if (!found)
	send_to_char("No one is using talk channels.\r\n", ch);
    }
    else
    {
	subscribe(ch, arg);
    }

    return;
}

void do_grant (CHAR_DATA *ch, char *argument)
{
    CHAR_DATA *vch;
    char buf[MAX_STRING_LENGTH];
    char arg[MAX_INPUT_LENGTH];

    if (IS_NPC(ch)) return;
    if (ch->pcdata->legend < 3)
    {
	send_to_char("Huh?\r\n", ch);
	return;
    }

    one_argument(argument, arg);

    if ( (vch = get_char_world(ch, arg) ) == NULL)
    {
	send_to_char("They aren't here.\r\n", ch);
	return;
    }

    if (IS_NPC(vch) || 
	vch->pcdata->powers[TALK_REQUESTED] == 0 ||
	vch->pcdata->powers[TALK_REQUESTED] != ch->pcdata->powers[TALK_CHANNEL])
    {
	send_to_char("They aren't requesting your channel.\r\n", ch);
	return;
    }

    if ( vch->pcdata->legend < 3 )
    {
	send_to_char("They can't use talk channels.\r\n", ch);
	return;
    }

    vch->pcdata->powers[TALK_CHANNEL] = vch->pcdata->powers[TALK_REQUESTED];
    vch->pcdata->powers[TALK_REQUESTED] = 0;

    send_to_char("Ok.\r\n", ch);
    sprintf(buf, "%s grants you access to channel %d.\r\n", ch->name,
	vch->pcdata->powers[TALK_CHANNEL]);
    send_to_char(buf, vch);

    return;
}

void do_decline (CHAR_DATA *ch, char *argument)
{
    CHAR_DATA *vch;
    char buf[MAX_STRING_LENGTH];
    char arg[MAX_INPUT_LENGTH];

    if (IS_NPC(ch)) return;
    if (ch->pcdata->legend < 3)
    {
        send_to_char("Huh?\r\n", ch);
        return;
    }

    one_argument(argument, arg);

    if ( (vch = get_char_world(ch, arg) ) == NULL)
    {
        send_to_char("They aren't here.\r\n", ch);
        return;
    }
    if (IS_NPC(vch))
    {
	send_to_char("Not on NPCs.\r\n", ch);
	return;
    }

    if (ch->pcdata->powers[TALK_CHANNEL] == 0)
    {
	send_to_char("You aren't even on a channel yourself.\r\n", ch);
	return;
    }

    if (vch->pcdata->powers[TALK_CHANNEL] == 0 && 
	vch->pcdata->powers[TALK_REQUESTED] == 0)
    {
	send_to_char("They aren't on or requesting a channel.\r\n", ch);
	return;
    }

    if (vch->pcdata->powers[TALK_CHANNEL] == ch->pcdata->powers[TALK_CHANNEL] ||
	vch->pcdata->powers[TALK_REQUESTED] == ch->pcdata->powers[TALK_REQUESTED])

    {
    	if (IS_IMMORTAL(vch))
    	{
	    send_to_char("You can't decline an immortal.\r\n", ch);
	    return;
    	}

    	send_to_char("You decline them access.\r\n", ch);

    	sprintf(buf,"You are declined access to channel %d.\r\n", 
	    ch->pcdata->powers[TALK_CHANNEL]);
    	send_to_char(buf, vch);
    	vch->pcdata->powers[TALK_REQUESTED] = 0;
	if (vch->pcdata->powers[TALK_CHANNEL] == 
		ch->pcdata->powers[TALK_CHANNEL])
	    vch->pcdata->powers[TALK_CHANNEL] = 0;
    }
    else
    {
	send_to_char("You can only decline people who are on your channel or requesting your channel.\r\n", ch);
    }
	

    return;
}

void subscribe (CHAR_DATA *ch, char *argument)
{
    DESCRIPTOR_DATA *d;
    int chan = 0, req = 0;    
    bool found = FALSE;
    char buf[MAX_STRING_LENGTH];
    char arg[MAX_INPUT_LENGTH];

    if (IS_NPC(ch)) return;
    if (ch->pcdata->legend < 3)
    {
        send_to_char("Huh?\r\n", ch);
        return;
    }

    one_argument(argument, arg);
    if (is_number(arg)) req = atoi(arg);

    if (arg[0] == '\0' || req < 0 || req > 20)
    {
	send_to_char("Syntax: subscribe <0-20>\r\n", ch);
	return;
    }

    chan = ch->pcdata->powers[TALK_CHANNEL];

    if (req == chan)
    {
	send_to_char("You're already subscribed to that talk channel.\r\n", ch);
	return;
    }

    if (req == 0)
    {
	ch->pcdata->powers[TALK_CHANNEL] = 0;
	send_to_char("Ok.\r\n", ch);
	return;
    }

    if (IS_IMMORTAL(ch))
    {
	ch->pcdata->powers[TALK_CHANNEL] = req;
	send_to_char("Ok.\r\n", ch);
	return;
    }

    ch->pcdata->powers[TALK_REQUESTED] = req;

    for ( d = descriptor_list; d != NULL; d = d->next )
    {
        CHAR_DATA *vch = d->original ? d->original : d->character;

        if ( d->connected == CON_PLAYING && vch != ch && !IS_NPC(vch) &&
             !IS_IMMORTAL(vch) && req == vch->pcdata->powers[TALK_CHANNEL])
        {
	    found = TRUE;
	    sprintf(buf, "%s requests access to your channel (Use grant/decline).\r\n", ch->name);
	    send_to_char(buf, vch);
        }
    }

    if (found)
    {
	send_to_char("Waiting for clearance...\r\n", ch);
    }
    else
    {
	ch->pcdata->powers[TALK_CHANNEL] = req;
	ch->pcdata->powers[TALK_REQUESTED] = 0;
	send_to_char("Ok.\r\n", ch);
    }

    return;
}

    
