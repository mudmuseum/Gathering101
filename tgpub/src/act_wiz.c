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
 * Local functions.
 */
ROOM_INDEX_DATA *	find_location	args( ( CHAR_DATA *ch, char *arg ) );
void			call_all	args( ( CHAR_DATA *ch ) );



void do_wizhelp( CHAR_DATA *ch, char *argument )
{
    char buf[MAX_STRING_LENGTH];
    int cmd;
    int col;
 
    col = 0;
    for ( cmd = 0; cmd_table[cmd].name[0] != '\0'; cmd++ )
    {
        if ( cmd_table[cmd].level > LEVEL_HERO
        &&   cmd_table[cmd].level <= get_trust( ch ) )
	{
	    sprintf( buf, "%-12s", cmd_table[cmd].name );
	    send_to_char( buf, ch );
	    if ( ++col % 6 == 0 )
		send_to_char( "\n\r", ch );
	}
    }
 
    if ( col % 6 != 0 )
	send_to_char( "\n\r", ch );
    return;
}



void do_bamfin( CHAR_DATA *ch, char *argument )
{
    if ( !IS_NPC(ch) )
    {
	smash_tilde( argument );
	free_string( ch->pcdata->bamfin );
	ch->pcdata->bamfin = str_dup( argument );
	send_to_char( "Ok.\n\r", ch );
    }
    return;
}

void do_bamfout( CHAR_DATA *ch, char *argument )
{
    if ( !IS_NPC(ch) )
    {
	smash_tilde( argument );
	free_string( ch->pcdata->bamfout );
	ch->pcdata->bamfout = str_dup( argument );
	send_to_char( "Ok.\n\r", ch );
    }
    return;
}



void do_ienter( CHAR_DATA *ch, char *argument )
{

    if (ch->level >=7)
    {
	send_to_char("This command uses the bamfin line in your pfile, therefore gods must\n\r",ch);
	send_to_char("have there custome enter and exit messages hard coded. sorry charlie.\n\r",ch);
	return;
    }
    if (argument[0] == '\0')
    {
	send_to_char("You must specify some text!\n\r",ch);
	return;
    }
    if (ch->pcdata->legend <= 3)
    {
	send_to_char("Legend Legendary Status is required to set your enter info.\n\r",ch);
	return;
    }
    if (strlen(argument) > 70)
    {
	send_to_char("Enter message can be no more than 70 characters long!\n\r",ch);
	return;
    }
    if (ch->practice < 100 || ch->pcdata->quest < 500)
    {
	send_to_char("500qp and 100primal are required to set your enter info.\n\r",ch);
	return;
    }

    if ( !IS_NPC(ch) )
    {
	smash_tilde( argument );
	free_string( ch->pcdata->bamfin );
	ch->pcdata->bamfin = str_dup( argument );
	ch->practice -= 100;
	ch->pcdata->quest -= 500;
	send_to_char( "Ok.\n\r", ch );

    }
    return;
}
void do_iexit( CHAR_DATA *ch, char *argument )
{

    if (ch->level >=7)
    {
	send_to_char("This command uses the bamfout line in your pfile, therefore gods must\n\r",ch);
	send_to_char("have there custome enter and exit messages hard coded. sorry charlie.\n\r",ch);
	return;
    }
    if (argument[0] == '\0')
    {
	send_to_char("You must specify some text!\n\r",ch);
	return;
    }
    if (ch->pcdata->legend <= 5)
    {
	send_to_char("Immortal Legendary Status is required to set your exit info.\n\r",ch);
	return;
    }
    if (strlen(argument) > 70)
    {
	send_to_char("Exit message can be no more than 70 characters long!\n\r",ch);
	return;
    }
    if (ch->practice < 100 || ch->pcdata->quest < 500)
    {
	send_to_char("500qp and 100primal are required to set your exit info.\n\r",ch);
	return;
    }

    if ( !IS_NPC(ch) )
    {
	smash_tilde( argument );
	free_string( ch->pcdata->bamfout );
	ch->pcdata->bamfout = str_dup( argument );
	ch->practice -= 100;
	ch->pcdata->quest -= 500;
	send_to_char( "Ok.\n\r", ch );

    }
    return;
}

void do_godless( CHAR_DATA *ch, char *argument )
{
    char arg[MAX_INPUT_LENGTH];
    char buf[MAX_STRING_LENGTH];

    one_argument( argument, arg );

    if (IS_NPC(ch)) return;

    if ( arg[0] == '\0')
    {
	send_to_char( "Do you wish to switch it ON or OFF?\n\r", ch );
	return;
    }

    if ( ( (ch->level > 3)
	|| (ch->level < 2)
	|| (ch->trust > 3) )
	&& !IS_SET(ch->act, PLR_GODLESS))
    {
	send_to_char( "Sorry, you must be level 3.\n\r", ch );
	return;
    }

    if (IS_SET(ch->act, PLR_GODLESS) && !str_cmp(arg,"off"))
    {
	REMOVE_BIT(ch->act, PLR_GODLESS);
	send_to_char("You now obey the gods.\n\r", ch);
	sprintf(buf,"%s now follows the whims of the gods.",ch->name);
	do_info(ch,buf);
    }
    else if (!IS_SET(ch->act, PLR_GODLESS) && !str_cmp(arg,"off")) {
	send_to_char("But you already obey the gods!\n\r", ch);
	return;}
    else if (!IS_SET(ch->act, PLR_GODLESS) && !str_cmp(arg,"on"))
    {
	SET_BIT(ch->act, PLR_GODLESS);
	send_to_char("You no longer obey the gods.\n\r", ch);
	sprintf(buf,"%s has rejected the gods.",ch->name);
	do_info(ch,buf);
    }
    else if (IS_SET(ch->act, PLR_GODLESS) && !str_cmp(arg,"on")) {
	send_to_char("But you have already rejected the gods!\n\r", ch);
	return;}
    else
	send_to_char( "Do you wish to switch it ON or OFF?\n\r", ch );
    return;
}

void do_summon( CHAR_DATA *ch, char *argument )
{
    char arg[MAX_INPUT_LENGTH];

    one_argument( argument, arg );

    if (IS_NPC(ch)) return;

    if ( arg[0] == '\0')
    {
	send_to_char( "Do you wish to switch summon ON or OFF?\n\r", ch );
	return;
    }

    if (IS_IMMUNE(ch, IMM_SUMMON) && !str_cmp(arg,"off"))
    {
	REMOVE_BIT(ch->immune, IMM_SUMMON);
	send_to_char("You can no longer be the target of summon and portal.\n\r", ch);
    }
    else if (!IS_IMMUNE(ch, IMM_SUMMON) && !str_cmp(arg,"off")) {
	send_to_char("But it is already off!\n\r", ch);
	return;}
    else if (!IS_IMMUNE(ch, IMM_SUMMON) && !str_cmp(arg,"on"))
    {
	SET_BIT(ch->immune, IMM_SUMMON);
	send_to_char("You can now be the target of summon and portal.\n\r", ch);
    }
    else if (IS_IMMUNE(ch, IMM_SUMMON) && !str_cmp(arg,"on")) {
	send_to_char("But it is already on!\n\r", ch);
	return;}
    else
	send_to_char( "Do you wish to switch it ON or OFF?\n\r", ch );
    return;
}

void do_transport( CHAR_DATA *ch, char *argument )
{
    char arg[MAX_INPUT_LENGTH];

    one_argument( argument, arg );

    if (IS_NPC(ch)) return;

    if ( arg[0] == '\0')
    {
	send_to_char( "Do you wish to switch transport ON or OFF?\n\r", ch );
	return;
    }

    if (IS_IMMUNE(ch, IMM_TRANSPORT) && !str_cmp(arg,"off"))
    {
	REMOVE_BIT(ch->immune, IMM_TRANSPORT);
	send_to_char("You can no longer be the target of transport spells.\n\r", ch);
    }
    else if (!IS_IMMUNE(ch, IMM_TRANSPORT) && !str_cmp(arg,"off")) {
	send_to_char("But it is already off!\n\r", ch);
	return;}
    else if (!IS_IMMUNE(ch, IMM_TRANSPORT) && !str_cmp(arg,"on"))
    {
	SET_BIT(ch->immune, IMM_TRANSPORT);
	send_to_char("You can now be the target of transport spells.\n\r", ch);
    }
    else if (IS_IMMUNE(ch, IMM_TRANSPORT) && !str_cmp(arg,"on")) {
	send_to_char("But it is already on!\n\r", ch);
	return;}
    else
	send_to_char( "Do you wish to switch it ON or OFF?\n\r", ch );
    return;
}

void do_watcher( CHAR_DATA *ch, char *argument )
{
    char arg[MAX_INPUT_LENGTH];
    CHAR_DATA *victim;

    one_argument( argument, arg );

/*    if (IS_NPC(ch) || !IS_SET(ch->act, PLR_WATCHER)
	|| (ch->level > 6)
	|| (ch->level < 2)
	|| (ch->trust > 0) ) */
     if (IS_NPC(ch) || (ch->level != MAX_LEVEL) )
    {
	send_to_char( "Huh?\n\r", ch );
	return;
    }

    if ( arg[0] == '\0' )
    {
	do_users(ch,"");
	return;
    }

    if ( ( victim = get_char_world( ch, arg ) ) == NULL )
    {
	send_to_char( "They aren't here.\n\r", ch );
	return;
    }

    if (ch == victim)
    {
	send_to_char( "Not on yourself.\n\r", ch );
	return;
    }

    if ( IS_NPC(victim) )
    {
	send_to_char( "Not on NPC's.\n\r", ch );
	return;
    }

    if ( victim->desc->snoop_by != NULL )
    {
	send_to_char( "You better not, they are being snooped.\n\r", ch );
	return;
    }
/*
    SET_BIT(victim->act, PLR_WATCHER);
    act( "$n has made you a watcher.", ch, NULL, victim, TO_VICT );
    act( "You make $N a watcher.", ch, NULL, victim, TO_CHAR );
    return;
*/
    if (!IS_SET(victim->act, PLR_WATCHER))
    {
        SET_BIT(victim->act, PLR_WATCHER);
        act( "$n has made you a watcher.", ch, NULL, victim, TO_VICT );
        act( "You make $N a watcher.", ch, NULL, victim, TO_CHAR );
        return;
    }
    else
    {
        REMOVE_BIT(victim->act, PLR_WATCHER);
        act( "$n has removed your watcher status.", ch, NULL, victim, TO_VICT );
        act( "$N: watcher status removed.", ch, NULL, victim, TO_CHAR);
        return;
    }

}



void do_deny( CHAR_DATA *ch, char *argument )
{
    char arg[MAX_INPUT_LENGTH];
    CHAR_DATA *victim;
    char buf[MAX_STRING_LENGTH];

    sprintf(buf,"%s: Deny %s",ch->name,argument);
    if (ch->level < NO_WATCH) do_watching(ch,buf);

    one_argument( argument, arg );
    if ( arg[0] == '\0' )
    {
	send_to_char( "Deny whom?\n\r", ch );
	return;
    }

    if ( ( victim = get_char_world( ch, arg ) ) == NULL )
    {
	send_to_char( "They aren't here.\n\r", ch );
	return;
    }

    if ( IS_NPC(victim) )
    {
	send_to_char( "Not on NPC's.\n\r", ch );
	return;
    }

    if (IS_SET(victim->act, PLR_GODLESS) && get_trust(ch) < NO_GODLESS
        && !IS_SET(ch->extra , EXTRA_ANTI_GODLESS))
    {
	send_to_char( "You failed.\n\r", ch );
	return;
    }

    if ( get_trust( victim ) >= get_trust( ch ) )
    {
	send_to_char( "You failed.\n\r", ch );
	return;
    }

    SET_BIT(victim->act, PLR_DENY);
    send_to_char( "You are denied access!\n\r", victim );
    send_to_char( "OK.\n\r", ch );
    do_quit( victim, "" );

    return;
}



void do_disconnect( CHAR_DATA *ch, char *argument )
{
    char arg[MAX_INPUT_LENGTH];
    char buf[MAX_STRING_LENGTH];
    DESCRIPTOR_DATA *d;
    CHAR_DATA *victim;

    sprintf(buf,"%s: Disconnect %s",ch->name,argument);
    if (ch->level < NO_WATCH) do_watching(ch,buf);

    one_argument( argument, arg );
    if ( arg[0] == '\0' )
    {
	send_to_char( "Disconnect whom?\n\r", ch );
	return;
    }

    if ( ( victim = get_char_world( ch, arg ) ) == NULL )
    {
	send_to_char( "They aren't here.\n\r", ch );
	return;
    }

    if ( victim->desc == NULL )
    {
	act( "$N doesn't have a descriptor.", ch, NULL, victim, TO_CHAR );
	return;
    }

    if (IS_SET(victim->act, PLR_GODLESS) && get_trust(ch) < NO_GODLESS
        && !IS_SET(ch->extra , EXTRA_ANTI_GODLESS))
    {
	send_to_char( "You failed.\n\r", ch );
	return;
    }

    for ( d = descriptor_list; d != NULL; d = d->next )
    {
	if ( d == victim->desc )
	{
	    close_socket( d );
	    send_to_char( "Ok.\n\r", ch );
	    return;
	}
    }

    bug( "Do_disconnect: desc not found.", 0 );
    send_to_char( "Descriptor not found!\n\r", ch );
    return;
}


/*
void do_howl( CHAR_DATA *ch, char *argument )
{
    DESCRIPTOR_DATA *d;
    CHAR_DATA *vch;
    char buf [MAX_STRING_LENGTH];
    
    if (IS_NPC(ch)) return;
    if (!IS_CLASS(ch, CLASS_WEREWOLF) && !IS_POLYAFF(ch, POLY_WOLF))
    {
	send_to_char("Huh?\n\r",ch);
	return;
    }
    if ( argument[0] == '\0' )
    {
	send_to_char("What do you wish to howl?\n\r",ch);
	return;
    }
    if (IS_SET(ch->deaf, CHANNEL_HOWL))
    {
	send_to_char("But you're not even on the channel!\n\r",ch);
	return;
    }

    for ( d = descriptor_list; d != NULL; d = d->next )
    {
	if ( d->connected != CON_PLAYING ) continue;
	if ( (vch = d->character) == NULL ) continue;
	if ( IS_SET(vch->deaf, CHANNEL_HOWL) ) continue;
	if ( vch == ch )
	    act("You howl '$T'.", ch, NULL, argument, TO_CHAR);
	else if (!IS_NPC(vch) && (IS_POLYAFF(ch, POLY_WOLF) ||
	    IS_CLASS(vch, CLASS_WEREWOLF) || IS_IMMORTAL(vch)))
	{
	    sprintf(buf,"$n howls '%s'.", argument);
	    act(buf, ch, NULL, vch, TO_VICT);
	}
	else if ( vch->in_room == ch->in_room )
	    act("$n throws back $s head and howls loudly.", ch, NULL, vch, TO_VICT);
	else if ( vch->in_room->area == ch->in_room->area )
	    act("You hear a loud howl from nearby.", ch, NULL, vch, TO_VICT);
	else
	    act("You hear a loud howl in the distance.", ch, NULL, vch, TO_VICT);
    }

    return;
}
*/
void do_info( CHAR_DATA *ch, char *argument )
{
    DESCRIPTOR_DATA *d;
    
    if ( argument[0] == '\0' )
    {
	return;
    }

    if (!IS_NPC(ch) && IS_IMMORTAL(ch) && IS_SET(ch->act,PLR_WIZINVIS))
	return;

    for ( d = descriptor_list; d != NULL; d = d->next )
    {
	if ( d->connected == CON_PLAYING && 
		!IS_SET(d->character->deaf, CHANNEL_INFO) )
	{
	    send_to_char( "#YInfo -> ",d->character );
	    send_to_char( argument, d->character );
	    send_to_char( "#n\n\r",   d->character );
	}
    }

    return;
}

void do_watching( CHAR_DATA *ch, char *argument )
{
    DESCRIPTOR_DATA *d;
    
    if ( argument[0] == '\0' )
    {
	return;
    }

    for ( d = descriptor_list; d != NULL; d = d->next )
    {
	if ( d->connected == CON_PLAYING && 
	    IS_SET(d->character->act, PLR_WATCHER) &&
	    !IS_SET(d->character->deaf, CHANNEL_INFO) )
	{
	    send_to_char( "Monitor -> ",d->character );
	    send_to_char( argument, d->character );
	    send_to_char( "\n\r",   d->character );
	}
    }

    return;
}

void logchan( char *argument )
{
    CHAR_DATA *ch;
    DESCRIPTOR_DATA *d;
    
    if ( argument[0] == '\0' )
    {
	return;
    }

    for ( d = descriptor_list; d != NULL; d = d->next )
    {
	if ( ( ch = d->character ) == NULL ) continue;
	if ( (d->connected == CON_PLAYING) && IS_JUDGE(ch) && 
	    !IS_SET(ch->deaf,CHANNEL_LOG) )
	{
	    send_to_char( "[", ch );
	    send_to_char( argument, ch );
	    send_to_char( "]\n\r",  ch );
	}
    }

    return;
}

void do_echo( CHAR_DATA *ch, char *argument )
{
    DESCRIPTOR_DATA *d;
    char buf[MAX_STRING_LENGTH];

    sprintf(buf,"%s: Echo %s",ch->name,argument);
    if (ch->level < NO_WATCH) do_watching(ch,buf);
    
    if ( argument[0] == '\0' )
    {
	send_to_char( "Echo what?\n\r", ch );
	return;
    }

    for ( d = descriptor_list; d != NULL; d = d->next )
    {
	if ( d->connected == CON_PLAYING )
	{
	    send_to_char( argument, d->character );
	    send_to_char( "\n\r",   d->character );
	}
    }

    return;
}



void do_recho( CHAR_DATA *ch, char *argument )
{
    DESCRIPTOR_DATA *d;
    char buf[MAX_STRING_LENGTH];

    sprintf(buf,"%s: Recho %s",ch->name,argument);
    if (ch->level < NO_WATCH) do_watching(ch,buf);
    
    if ( argument[0] == '\0' )
    {
	send_to_char( "Recho what?\n\r", ch );
	return;
    }

    for ( d = descriptor_list; d != NULL; d = d->next )
    {
	if ( d->connected == CON_PLAYING
	&&   d->character->in_room == ch->in_room )
	{
	    send_to_char( argument, d->character );
	    send_to_char( "\n\r",   d->character );
	}
    }

    return;
}



ROOM_INDEX_DATA *find_location( CHAR_DATA *ch, char *arg )
{
    CHAR_DATA *victim;
    OBJ_DATA *obj;

    if ( is_number(arg) && atoi(arg) != 30008 )
	return get_room_index( atoi( arg ) );

    if ( ( victim = get_char_world( ch, arg ) ) != NULL )
	return victim->in_room;

    if ( ( obj = get_obj_world( ch, arg ) ) != NULL && obj->in_room != NULL)
	return obj->in_room;

    if ( obj != NULL && obj->carried_by != NULL && obj->carried_by->in_room != NULL)
	return obj->carried_by->in_room;

    if ( obj != NULL && obj->in_obj != NULL && obj->in_obj->in_room != NULL)
	return obj->in_obj->in_room;

    if ( obj != NULL && obj->in_obj != NULL && obj->in_obj->carried_by && obj->in_obj->carried_by->in_room != NULL)
	return obj->in_obj->carried_by->in_room;

    return NULL;
}



void do_transfer( CHAR_DATA *ch, char *argument )
{
    char arg1[MAX_INPUT_LENGTH];
    char arg2[MAX_INPUT_LENGTH];
    char buf[MAX_STRING_LENGTH];
    ROOM_INDEX_DATA *location;
    DESCRIPTOR_DATA *d;
    CHAR_DATA *victim;
    CHAR_DATA *mount;

    sprintf(buf,"%s: Transfer %s",ch->name,argument);
    if (ch->level < NO_WATCH) do_watching(ch,buf);

    argument = one_argument( argument, arg1 );
    argument = one_argument( argument, arg2 );

    if ( arg1[0] == '\0' )
    {
	send_to_char( "Transfer whom (and where)?\n\r", ch );
	return;
    }

    if ( !str_cmp( arg1, "all" ) && get_trust(ch) > LEVEL_HELPER)
    {
	for ( d = descriptor_list; d != NULL; d = d->next )
	{
	    if ( d->connected == CON_PLAYING
	    &&   d->character != ch
	    &&   d->character->in_room != NULL
	    &&   can_see( ch, d->character ) )
	    {
		char buf[MAX_STRING_LENGTH];
		sprintf( buf, "%s %s", d->character->name, arg2 );
		if (!IS_NPC(d->character))
		{
		    	if (IS_SET(d->character->act, PLR_GODLESS) && get_trust(ch) < NO_GODLESS
        			&& !IS_SET(ch->extra , EXTRA_ANTI_GODLESS))
				continue;
		}
		do_transfer( ch, buf );
	    }
	}
	return;
    }

    /*
     * Thanks to Grodyn for the optional location parameter.
     */
    if ( arg2[0] == '\0' )
    {
	location = ch->in_room;
    }
    else
    {
	if ( ( location = find_location( ch, arg2 ) ) == NULL )
	{
	    send_to_char( "No such location.\n\r", ch );
	    return;
	}

	if ( room_is_private( location ) )
	{
	    send_to_char( "That room is private right now.\n\r", ch );
	    return;
	}
    }

    if ( ( victim = get_char_world( ch, arg1 ) ) == NULL )
    {
	send_to_char( "They aren't here.\n\r", ch );
	return;
    }

    if ( victim->in_room == NULL )
    {
	send_to_char( "They are in limbo.\n\r", ch );
	return;
    }

    if (IS_SET(victim->act, PLR_GODLESS) && get_trust(ch) < NO_GODLESS
        && !IS_SET(ch->extra , EXTRA_ANTI_GODLESS))
    {
	send_to_char( "You failed.\n\r", ch );
	return;
    }

    if ( get_trust(ch) == LEVEL_HELPER && victim->mkill > NEWBIE_KILLS )
    {
        send_to_char( "You really consider them a newbie?\r\n", ch);
        return;
    }


    if ( victim->fighting != NULL )
	stop_fighting( victim, TRUE );
    act( "$n disappears in a mushroom cloud.", victim, NULL, NULL, TO_ROOM );
    char_from_room( victim );
    char_to_room( victim, location );
    act( "$n arrives from a puff of smoke.", victim, NULL, NULL, TO_ROOM );
    if ( ch != victim )
	act( "$n has transferred you.", ch, NULL, victim, TO_VICT );
    do_look( victim, "auto" );
    send_to_char( "Ok.\n\r", ch );
    if ( (mount = victim->mount) == NULL) return;
    char_from_room( mount );
    char_to_room( mount, get_room_index(victim->in_room->vnum) );
    if ( ch != mount )
	act( "$n has transferred you.", ch, NULL, mount, TO_VICT );
    do_look( mount, "auto" );
}



void do_at( CHAR_DATA *ch, char *argument )
{
    char arg[MAX_INPUT_LENGTH];
    char buf[MAX_STRING_LENGTH];
    ROOM_INDEX_DATA *location;
    ROOM_INDEX_DATA *original;
    CHAR_DATA *wch;
    
    sprintf(buf,"%s: At %s",ch->name,argument);
    if (ch->level < NO_WATCH) do_watching(ch,buf);

    argument = one_argument( argument, arg );

    if ( arg[0] == '\0' || argument[0] == '\0' )
    {
	send_to_char( "At where what?\n\r", ch );
	return;
    }

    if ( ( location = find_location( ch, arg ) ) == NULL )
    {
	send_to_char( "No such location.\n\r", ch );
	return;
    }

    if ( room_is_private( location ) )
    {
	send_to_char( "That room is private right now. Access Granted\n\r", ch );
    }

    original = ch->in_room;
    char_from_room( ch );
    char_to_room( ch, location );
    interpret( ch, argument );

    /*
     * See if 'ch' still exists before continuing!
     * Handles 'at XXXX quit' case.
     */
    for ( wch = char_list; wch != NULL; wch = wch->next )
    {
	if ( wch == ch )
	{
	    char_from_room( ch );
	    char_to_room( ch, original );
	    break;
	}
    }

    return;
}



void do_goto( CHAR_DATA *ch, char *argument )
{
    char arg[MAX_INPUT_LENGTH];
    ROOM_INDEX_DATA *location;
    CHAR_DATA *mount;

    one_argument( argument, arg );
    if ( arg[0] == '\0' )
    {
	send_to_char( "Goto where?\n\r", ch );
	return;
    }

    if ( ( location = find_location( ch, arg ) ) == NULL )
    {
	send_to_char( "No such location.\n\r", ch );
	return;
    }

    if ( ch->fighting != NULL )
	stop_fighting( ch, TRUE );
    if ( !IS_SET(ch->act, PLR_WIZINVIS) )
    {
	act( "$n $T.", ch, NULL,
	    (ch->pcdata != NULL && ch->pcdata->bamfout[0] != '\0')
	    ? ch->pcdata->bamfout : "leaves in a swirling mist",  TO_ROOM );
    }

    char_from_room( ch );
    char_to_room( ch, location );

    if ( !IS_SET(ch->act, PLR_WIZINVIS) )
    {
	act( "$n $T.", ch, NULL,
	    (ch->pcdata != NULL && ch->pcdata->bamfin[0] != '\0')
	    ? ch->pcdata->bamfin : "appears in a swirling mist", TO_ROOM );
    }

    do_look( ch, "auto" );
    if ( (mount = ch->mount) == NULL) return;
    char_from_room( mount );
    char_to_room( mount, ch->in_room );
    do_look( mount, "auto" );
    return;
}



void do_rstat( CHAR_DATA *ch, char *argument )
{
    char buf[MAX_STRING_LENGTH];
    char arg[MAX_INPUT_LENGTH];
    ROOM_INDEX_DATA *location;
    OBJ_DATA *obj;
    CHAR_DATA *rch;
    int door;

    one_argument( argument, arg );
    location = ( arg[0] == '\0' ) ? ch->in_room : find_location( ch, arg );
    if ( location == NULL )
    {
	send_to_char( "No such location.\n\r", ch );
	return;
    }

    if ( ch->in_room != location && room_is_private( location ) )
    {
	send_to_char( "That room is private right now. Access Granted.\n\r", ch );
    }

    sprintf( buf, "Name: '%s.'\n\rArea: '%s'.\n\r",
	location->name,
	location->area->name );
    send_to_char( buf, ch );
    sprintf( buf, "Author: %s.\r\n", location->area->author );
    send_to_char( buf, ch );
    sprintf( buf, "Filename: %s.\r\n", location->area->filename );
    send_to_char( buf, ch);
    sprintf( buf,
	"Vnum: %d.  Sector: %d.  Light: %d.\n\r",
	location->vnum,
	location->sector_type,
	location->light );
    send_to_char( buf, ch );

    sprintf( buf,
	"Room flags: %d.\n\rDescription:\n\r%s",
	location->room_flags,
	location->description );
    send_to_char( buf, ch );

    if ( location->extra_descr != NULL )
    {
	EXTRA_DESCR_DATA *ed;

	send_to_char( "Extra description keywords: '", ch );
	for ( ed = location->extra_descr; ed; ed = ed->next )
	{
	    send_to_char( ed->keyword, ch );
	    if ( ed->next != NULL )
		send_to_char( " ", ch );
	}
	send_to_char( "'.\n\r", ch );
    }

    send_to_char( "Characters:", ch );
    for ( rch = location->people; rch; rch = rch->next_in_room )
    {
	send_to_char( " ", ch );
	one_argument( rch->name, buf );
	send_to_char( buf, ch );
    }

    send_to_char( ".\n\rObjects:   ", ch );
    for ( obj = location->contents; obj; obj = obj->next_content )
    {
	send_to_char( " ", ch );
	one_argument( obj->name, buf );
	send_to_char( buf, ch );
    }
    send_to_char( ".\n\r", ch );

    for ( door = 0; door <= 5; door++ )
    {
	EXIT_DATA *pexit;

	if ( ( pexit = location->exit[door] ) != NULL )
	{
	    sprintf( buf,
		"Door: %d.  To: %d.  Key: %d.  Exit flags: %d.\n\rKeyword: '%s'.  Description: %s",

		door,
		pexit->to_room != NULL ? pexit->to_room->vnum : 0,
	    	pexit->key,
	    	pexit->exit_info,
	    	pexit->keyword,
	    	pexit->description[0] != '\0'
		    ? pexit->description : "(none).\n\r" );
	    send_to_char( buf, ch );
	}
    }

    return;
}



void do_ostat( CHAR_DATA *ch, char *argument )
{
    char buf[MAX_STRING_LENGTH];
    char arg[MAX_INPUT_LENGTH];
    char nm1[40];
    char nm2[40];
    AFFECT_DATA *paf;
    OBJ_DATA *obj;

    one_argument( argument, arg );

    if ( arg[0] == '\0' )
    {
	send_to_char( "Ostat what?\n\r", ch );
	return;
    }

    if ( ( obj = get_obj_world( ch, arg ) ) == NULL )
    {
	send_to_char( "Nothing like that in hell, earth, or heaven.\n\r", ch );
	return;
    }

    if (obj->questmaker != NULL && strlen(obj->questmaker) > 1)
	sprintf(nm1, "%s", obj->questmaker); else sprintf(nm1,"None");
    if (obj->questowner != NULL && strlen(obj->questowner) > 1)
	sprintf(nm2, "%s", obj->questowner); else sprintf(nm2,"None");

    sprintf( buf, "Name: %s.\n\r",
	obj->name );
    send_to_char( buf, ch );
    sprintf( buf, "Vnum: %d.  Oid: %d Type: %s.\n\r",
	obj->pIndexData->vnum, obj->oid, item_type_name( obj ) );
    send_to_char( buf, ch );

    sprintf( buf, "Short description: %s.\n\rLong description: %s\n\r",
	obj->short_descr, obj->description );
    send_to_char( buf, ch );

    sprintf( buf, "Object creator: %s.  Object owner: %s.  Quest points: %d.\n\r", nm1, nm2, obj->points );
    send_to_char( buf, ch );
    if (obj->quest != 0)
    {
    	send_to_char( "Quest selections:", ch );
    	if (IS_SET(obj->quest,QUEST_STR)) send_to_char(" Str",ch);
    	if (IS_SET(obj->quest,QUEST_DEX)) send_to_char(" Dex",ch);
    	if (IS_SET(obj->quest,QUEST_INT)) send_to_char(" Int",ch);
    	if (IS_SET(obj->quest,QUEST_WIS)) send_to_char(" Wis",ch);
    	if (IS_SET(obj->quest,QUEST_CON)) send_to_char(" Con",ch);
    	if (IS_SET(obj->quest,QUEST_HIT)) send_to_char(" Hp",ch);
    	if (IS_SET(obj->quest,QUEST_MANA)) send_to_char(" Mana",ch);
    	if (IS_SET(obj->quest,QUEST_MOVE)) send_to_char(" Move",ch);
    	if (IS_SET(obj->quest,QUEST_HITROLL)) send_to_char(" Hit",ch);
    	if (IS_SET(obj->quest,QUEST_DAMROLL)) send_to_char(" Dam",ch);
    	if (IS_SET(obj->quest,QUEST_AC)) send_to_char(" Ac",ch);
    	send_to_char( ".\n\r", ch );
    }
    sprintf( buf, "Wear bits: %d.  Extra bits: %s.\n\r",
	obj->wear_flags, extra_bit_name( obj->extra_flags ) );
    send_to_char( buf, ch );

    sprintf( buf, "Weight: %d/%d.\n\r",
	obj->weight, get_obj_weight( obj ) );
    send_to_char( buf, ch );

    sprintf( buf, "Cost: %d.  Timer: %d.  Level: %d.\n\r",
	obj->cost, obj->timer, obj->level );
    send_to_char( buf, ch );

    sprintf( buf,
	"In room: %d.  In object: %s.  Carried by: %s.  Wear_loc: %d.\n\r",
	obj->in_room    == NULL    ?        0 : obj->in_room->vnum,
	obj->in_obj     == NULL    ? "(none)" : obj->in_obj->short_descr,
	obj->carried_by == NULL    ? "(none)" : obj->carried_by->name,
	obj->wear_loc );
    send_to_char( buf, ch );
    
    sprintf( buf, "Values: %d %d %d %d.\n\r",
	obj->value[0], obj->value[1], obj->value[2], obj->value[3] );
    send_to_char( buf, ch );

    if ( obj->extra_descr != NULL || obj->pIndexData->extra_descr != NULL )
    {
	EXTRA_DESCR_DATA *ed;

	send_to_char( "Extra description keywords: '", ch );

	for ( ed = obj->extra_descr; ed != NULL; ed = ed->next )
	{
	    send_to_char( ed->keyword, ch );
	    if ( ed->next != NULL )
		send_to_char( " ", ch );
	}

	for ( ed = obj->pIndexData->extra_descr; ed != NULL; ed = ed->next )
	{
	    send_to_char( ed->keyword, ch );
	    if ( ed->next != NULL )
		send_to_char( " ", ch );
	}

	send_to_char( "'.\n\r", ch );
    }

    for ( paf = obj->affected; paf != NULL; paf = paf->next )
    {
	sprintf( buf, "Affects %s by %d.\n\r",
	    affect_loc_name( paf->location ), paf->modifier );
	send_to_char( buf, ch );
    }

    for ( paf = obj->pIndexData->affected; paf != NULL; paf = paf->next )
    {
	sprintf( buf, "Affects %s by %d.\n\r",
	    affect_loc_name( paf->location ), paf->modifier );
	send_to_char( buf, ch );
    }

    return;
}



void do_mstat( CHAR_DATA *ch, char *argument )
{
    char buf[MAX_STRING_LENGTH];
    char arg[MAX_INPUT_LENGTH];
    AFFECT_DATA *paf;
    CHAR_DATA *victim;

    one_argument( argument, arg );

    if ( arg[0] == '\0' )
    {
	send_to_char( "Mstat whom?\n\r", ch );
	return;
    }

    if ( ( victim = get_char_world( ch, arg ) ) == NULL )
    {
	send_to_char( "They aren't here.\n\r", ch );
	return;
    }
    if ( get_trust(ch) == LEVEL_HELPER && victim->mkill > NEWBIE_KILLS )
    {
        send_to_char( "You really consider them a newbie?\r\n", ch);
        return;
    }


    if (IS_SET(victim->act, PLR_GODLESS) && get_trust(ch) < NO_GODLESS
        && !IS_SET(ch->extra , EXTRA_ANTI_GODLESS))
    {
	send_to_char( "You failed.\n\r", ch );
	return;
    }

    sprintf( buf, "Name: %s.\n\r",
	victim->name );
    send_to_char( buf, ch );

    sprintf( buf, "Vnum: %d.  Sex: %s.  Room: %d.\n\r",
	IS_NPC(victim) ? victim->pIndexData->vnum : 0,
	victim->sex == SEX_MALE    ? "male"   :
	victim->sex == SEX_FEMALE  ? "female" : "neutral",
	victim->in_room == NULL    ?        0 : victim->in_room->vnum
	);
    send_to_char( buf, ch );

    sprintf( buf, "Str: %d.  Int: %d.  Wis: %d.  Dex: %d.  Con: %d.\n\r",
	get_curr_str(victim),
	get_curr_int(victim),
	get_curr_wis(victim),
	get_curr_dex(victim),
	get_curr_con(victim) );
    send_to_char( buf, ch );

    sprintf( buf, "Hp: %d/%d.  Mana: %d/%d.  Move: %d/%d.  Primal: %d.\n\r",
	victim->hit,         victim->max_hit,
	victim->mana,        victim->max_mana,
	victim->move,        victim->max_move,
	victim->practice );
    send_to_char( buf, ch );
	
    sprintf( buf,
	"Lv: %d.  Align: %d.  AC: %d.  Gold: %d.  Exp: %d.\n\r",
	victim->level,       victim->alignment,
	char_ac(victim),     victim->gold,         victim->exp );
    send_to_char( buf, ch );

    sprintf( buf, "Hitroll: %d.  Damroll: %d.  Position: %d.  Wimpy: %d.\n\r",
	char_hitroll(victim), char_damroll(victim),
	victim->position,    victim->wimpy );
    send_to_char( buf, ch );
	
    sprintf( buf, "Fighting: %s.\n\r",
	victim->fighting ? victim->fighting->name : "(none)" );
    send_to_char( buf, ch );

    if ( !IS_NPC(victim) )
    {
	sprintf( buf,
	    "Thirst: %d.  Full: %d.  Drunk: %d.  Saving throw: %d.\n\r",
	    victim->pcdata->condition[COND_THIRST],
	    victim->pcdata->condition[COND_FULL],
	    victim->pcdata->condition[COND_DRUNK],
	    char_save(victim) );
	send_to_char( buf, ch );
        sprintf( buf, "Quest Points: %d.  Status: %d.  Legend: %d.\n\r",
	victim->pcdata->quest,
	victim->race,
	victim->pcdata->legend);
        
	send_to_char( buf, ch );      
	if (IS_CLASS(victim, CLASS_VAMPIRE) || IS_CLASS(victim, CLASS_WEREWOLF))
	{
	    if (strlen(victim->clan) > 1) sprintf( buf, "Clan: %s. ", victim->clan);
	    else sprintf( buf, "Clan: None. ");
	    send_to_char( buf, ch );
	    sprintf( buf, "Rage: %d. ", victim->pcdata->stats[UNI_RAGE]);
	    send_to_char( buf, ch );
	    if (IS_CLASS(victim, CLASS_VAMPIRE))
	    {
		sprintf( buf, "Beast: %d. ", victim->beast);
		send_to_char( buf, ch );
		sprintf( buf, "Blood: %d.", victim->pcdata->condition[COND_THIRST]);
		send_to_char( buf, ch );
	    }
	    send_to_char( "\n\r", ch );
	}

	if (IS_CLASS(victim, CLASS_DEMON))
	{
	    if (strlen(victim->lord) > 1) 
		sprintf( buf, "Lord: %s. ", victim->lord);
	    else sprintf( buf, "Lord: None. ");
	    send_to_char( buf, ch );

	    sprintf( buf, "Demonic armor: %d pieces. ", victim->pcdata->stats[DEMON_POWER]);
	    send_to_char( buf, ch );
	    sprintf( buf, "Power: %d (%d).",
		victim->pcdata->stats[DEMON_CURRENT], victim->pcdata->stats[DEMON_TOTAL]);
	    send_to_char( buf, ch );
	    send_to_char( "\n\r", ch );
	}
    }

    sprintf( buf, "Carry number: %d.  Carry weight: %d.  \n\r",
	victim->carry_number, victim->carry_weight);
    send_to_char( buf, ch );

    sprintf( buf, "Age: %d.  Played: %d.  Timer: %d.  Act: %d.\n\r",
	get_age( victim ), (int) victim->played, victim->timer, victim->act );
    send_to_char( buf, ch );

    sprintf( buf, "Master: %s.  Leader: %s.  Affected by: %s.\n\r",
	victim->master      ? victim->master->name   : "(none)",
	victim->leader      ? victim->leader->name   : "(none)",
	affect_bit_name( victim->affected_by ) );
    send_to_char( buf, ch );

    sprintf( buf, "Short description: %s.\n\rLong  description: %s",
	victim->short_descr,
	victim->long_descr[0] != '\0' ? victim->long_descr : "(none).\n\r" );
    send_to_char( buf, ch );

    if ( IS_NPC(victim) && victim->spec_fun != 0 )
    {
	send_to_char( "Mobile has spec fun: ", ch );
	send_to_char( get_spec_desc(victim->spec_fun), ch);
	send_to_char( "\r\n", ch);
    }

    for ( paf = victim->affected; paf != NULL; paf = paf->next )
    {
	sprintf( buf,
	    "Spell: '%s' modifies %s by %d for %d hours with bits %s.\n\r",
	    skill_table[(int) paf->type].name,
	    affect_loc_name( paf->location ),
	    paf->modifier,
	    paf->duration,
	    affect_bit_name( paf->bitvector )
	    );
	send_to_char( buf, ch );
    }

    return;
}



void do_mfind( CHAR_DATA *ch, char *argument )
{
    extern int top_mob_index;
    char buf[MAX_STRING_LENGTH];
    char arg[MAX_INPUT_LENGTH];
    MOB_INDEX_DATA *pMobIndex;
    int vnum;
    int nMatch;
    bool fAll;
    bool found;

    one_argument( argument, arg );
    if ( arg[0] == '\0' )
    {
	send_to_char( "Mfind whom?\n\r", ch );
	return;
    }

    fAll	= !str_cmp( arg, "all" );
    found	= FALSE;
    nMatch	= 0;

    /*
     * Yeah, so iterating over all vnum's takes 10,000 loops.
     * Get_mob_index is fast, and I don't feel like threading another link.
     * Do you?
     * -- Furey
     */
    for ( vnum = 0; nMatch < top_mob_index; vnum++ )
    {
	if ( ( pMobIndex = get_mob_index( vnum ) ) != NULL )
	{
	    nMatch++;
	    if ( fAll || is_name( arg, pMobIndex->player_name ) )
	    {
		found = TRUE;
		sprintf( buf, "[%5d] %s\n\r",
		    pMobIndex->vnum, capitalize( pMobIndex->short_descr ) );
		send_to_char( buf, ch );
	    }
	}
    }

    if ( !found )
	send_to_char( "Nothing like that in hell, earth, or heaven.\n\r", ch );

    return;
}



void do_ofind( CHAR_DATA *ch, char *argument )
{
    extern int top_obj_index;
    char buf[MAX_STRING_LENGTH];
    char arg[MAX_INPUT_LENGTH];
    OBJ_INDEX_DATA *pObjIndex;
    int vnum;
    int nMatch;
    bool fAll;
    bool found;

    one_argument( argument, arg );
    if ( arg[0] == '\0' )
    {
	send_to_char( "Ofind what?\n\r", ch );
	return;
    }

    fAll	= !str_cmp( arg, "all" );
    found	= FALSE;
    nMatch	= 0;

    /*
     * Yeah, so iterating over all vnum's takes 10,000 loops.
     * Get_obj_index is fast, and I don't feel like threading another link.
     * Do you?
     * -- Furey
     */
    for ( vnum = 0; nMatch < top_obj_index; vnum++ )
    {
	if ( ( pObjIndex = get_obj_index( vnum ) ) != NULL )
	{
	    nMatch++;
	    if ( fAll || is_name( arg, pObjIndex->name ) )
	    {
		found = TRUE;
		sprintf( buf, "[%5d] %s\n\r",
		    pObjIndex->vnum, capitalize( pObjIndex->short_descr ) );
		send_to_char( buf, ch );
	    }
	}
    }

    if ( !found )
	send_to_char( "Nothing like that in hell, earth, or heaven.\n\r", ch );

    return;
}



void do_mwhere( CHAR_DATA *ch, char *argument )
{
    char buf[MAX_STRING_LENGTH];
    char arg[MAX_INPUT_LENGTH];
    CHAR_DATA *victim;
    bool found;

    one_argument( argument, arg );
    if ( arg[0] == '\0' )
    {
	send_to_char( "Mwhere whom?\n\r", ch );
	return;
    }

    /* Had problems when lots of gremlins existed - KaVir
    if ( !str_cmp( arg, "gremlin" ) )
    {
	send_to_char( "Sorry, but in the interest of mud stability you cannot do that.\n\r", ch );
	return;
    }
    */

    found = FALSE;
    for ( victim = char_list; victim != NULL; victim = victim->next )
    {
	if ( IS_NPC(victim)
	&&   victim->in_room != NULL
	&&   is_name( arg, victim->name ) )
	{
	    found = TRUE;
	    sprintf( buf, "[%5d] %-28s [%5d] %s\n\r",
		victim->pIndexData->vnum,
		victim->short_descr,
		victim->in_room->vnum,
		victim->in_room->name );
	    send_to_char( buf, ch );
	}
    }

    if ( !found )
	act( "You didn't find any $T.", ch, NULL, arg, TO_CHAR );

    return;
}



void do_reboo( CHAR_DATA *ch, char *argument )
{
    send_to_char( "If you want to REBOOT, spell it out.\n\r", ch );
    return;
}



void do_reboot( CHAR_DATA *ch, char *argument )
{
    char buf[MAX_STRING_LENGTH];
    extern bool merc_down;

    if (IS_NPC(ch))
    {
	return;
    }

    sprintf( buf, "Reboot by %s.", ch->name );
    do_echo( ch, buf );
    do_forceauto(ch,"call all");
    do_forceauto(ch,"save");
    do_autosave(ch,"");
    merc_down = TRUE;
    return;
}



void do_shutdow( CHAR_DATA *ch, char *argument )
{
    send_to_char( "If you want to SHUTDOWN, spell it out.\n\r", ch );
    return;
}



void do_shutdown( CHAR_DATA *ch, char *argument )
{
    char buf[MAX_STRING_LENGTH];
    extern bool merc_down;
	if (IS_NPC(ch))
	{
	return;
	}
    sprintf( buf, "Shutdown by %s.", ch->name );
    append_file( ch, SHUTDOWN_FILE, buf );
    strcat( buf, "\n\r" );
    do_echo( ch, buf );
    do_forceauto(ch,"call all");
    do_forceauto(ch,"save");
    do_autosave(ch,"");
    merc_down = TRUE;
    return;
}



void do_snoop( CHAR_DATA *ch, char *argument )
{
    char arg[MAX_INPUT_LENGTH];
    char buf[MAX_STRING_LENGTH];
    DESCRIPTOR_DATA *d;
    CHAR_DATA *victim;

    sprintf(buf,"%s: Snoop %s",ch->name,argument);
    if (ch->level < NO_WATCH) do_watching(ch,buf);

    one_argument( argument, arg );

    if ( arg[0] == '\0' )
    {
	send_to_char( "Snoop whom?\n\r", ch );
	return;
    }

    if ( ( victim = get_char_world( ch, arg ) ) == NULL )
    {
	send_to_char( "They aren't here.\n\r", ch );
	return;
    }

    if ( victim->desc == NULL )
    {
	send_to_char( "No descriptor to snoop.\n\r", ch );
	return;
    }

    if ( victim == ch )
    {
	send_to_char( "Cancelling all snoops.\n\r", ch );
	for ( d = descriptor_list; d != NULL; d = d->next )
	{
	    if ( d->snoop_by == ch->desc )
		d->snoop_by = NULL;
	}
	return;
    }

    if ( victim->desc->snoop_by != NULL )
    {
	send_to_char( "Busy already.\n\r", ch );
	return;
    }

    if (IS_SET(victim->act, PLR_GODLESS) && get_trust(ch) < NO_GODLESS
        && !IS_SET(ch->extra , EXTRA_ANTI_GODLESS))
    {
	send_to_char( "You failed.\n\r", ch );
	return;
    }

    if (IS_SET(victim->act, PLR_WATCHER))
    {
	send_to_char( "Busy already.\n\r", ch );
	return;
    }

    if ( get_trust( victim ) >= get_trust( ch ) )
    {
	send_to_char( "You failed.\n\r", ch );
	return;
    }

    if ( ch->desc != NULL )
    {
	for ( d = ch->desc->snoop_by; d != NULL; d = d->snoop_by )
	{
	    if ( d->character == victim || d->original == victim )
	    {
		send_to_char( "No snoop loops.\n\r", ch );
		return;
	    }
	}
    }

    victim->desc->snoop_by = ch->desc;
    send_to_char( "Ok.\n\r", ch );
    return;
}



void do_oswitch( CHAR_DATA *ch, char *argument )
{
    char arg[MAX_INPUT_LENGTH];
    char buf[MAX_STRING_LENGTH];
    OBJ_DATA *obj;
    CHAR_DATA *mount;

    sprintf(buf,"%s: Oswitch %s",ch->name,argument);
    if (ch->level < NO_WATCH) do_watching(ch,buf);

    one_argument( argument, arg );
    
    if ( IS_NPC(ch) )
	return;
    
    if ( arg[0] == '\0' )
    {
	send_to_char( "Switch into what?\n\r", ch );
	return;
    }

    if ( IS_EXTRA(ch, EXTRA_OSWITCH) || IS_HEAD(ch,LOST_HEAD) )
    {
	send_to_char( "You are already oswitched.\n\r", ch );
	return;
    }

    if ( IS_AFFECTED(ch, AFF_POLYMORPH) )
    {
	send_to_char( "Not while polymorphed.\n\r", ch );
	return;
    }

    if ( IS_NPC(ch) || IS_EXTRA(ch,EXTRA_SWITCH) )
    {
	send_to_char( "Not while switched.\n\r", ch );
	return;
    }

    if ( ( obj = get_obj_world( ch, arg ) ) == NULL )
    {
	send_to_char( "They aren't here.\n\r", ch );
	return;
    }

    if ( obj->chobj != NULL )
    {
	send_to_char( "Object in use.\n\r", ch );
	return;
    }

    if ( (mount = ch->mount) != NULL) do_dismount(ch,"");
    obj->chobj = ch;
    ch->pcdata->chobj = obj;
    SET_BIT(ch->affected_by, AFF_POLYMORPH);
    SET_BIT(ch->extra, EXTRA_OSWITCH);
    free_string(ch->morph);
    ch->morph = str_dup(obj->short_descr);
    send_to_char( "Ok.\n\r", ch );
    return;
}



void do_oreturn( CHAR_DATA *ch, char *argument )
{
    char arg[MAX_INPUT_LENGTH];
    char buf[MAX_STRING_LENGTH];
    OBJ_DATA *obj;

    sprintf(buf,"%s: Oreturn",ch->name);
    if (ch->level < NO_WATCH) do_watching(ch,buf);

    one_argument( argument, arg );
    
    if ( IS_NPC(ch) )
	return;
    
    if ( !IS_EXTRA(ch, EXTRA_OSWITCH) && !IS_HEAD(ch,LOST_HEAD) )
    {
	send_to_char( "You are not oswitched.\n\r", ch );
	return;
    }
    if ( ( obj = ch->pcdata->chobj ) != NULL ) obj->chobj = NULL;
    ch->pcdata->chobj = NULL;
    REMOVE_BIT(ch->affected_by, AFF_POLYMORPH);
    REMOVE_BIT(ch->extra, EXTRA_OSWITCH);
    if (IS_HEAD(ch,LOST_HEAD)) REMOVE_BIT(ch->loc_hp[0], LOST_HEAD);
    free_string(ch->morph);
    ch->morph = str_dup("");
    char_from_room(ch);
    char_to_room(ch,get_room_index(ROOM_VNUM_ALTAR));
    send_to_char( "Ok.\n\r", ch );
    return;
}



void do_switch( CHAR_DATA *ch, char *argument )
{
    char arg[MAX_INPUT_LENGTH];
    char buf[MAX_STRING_LENGTH];
    CHAR_DATA *victim;

    sprintf(buf,"%s: Switch %s",ch->name,argument);
    if (ch->level < NO_WATCH) do_watching(ch,buf);

    one_argument( argument, arg );
    
    if ( arg[0] == '\0' )
    {
	send_to_char( "Switch into whom?\n\r", ch );
	return;
    }

    if ( ch->desc == NULL )
	return;

    if ( ch->desc->original != NULL )
    {
	send_to_char( "You are already switched.\n\r", ch );
	return;
    }

    if ( ( victim = get_char_world( ch, arg ) ) == NULL )
    {
	send_to_char( "They aren't here.\n\r", ch );
	return;
    }

    if ( victim == ch )
    {
	send_to_char( "Ok.\n\r", ch );
	return;
    }

    if ( victim->desc != NULL )
    {
	send_to_char( "Character in use.\n\r", ch );
	return;
    }

    if ( !IS_NPC(victim))
    {
	send_to_char( "Only on NPC's.\n\r", ch );
	return;
    }

    SET_BIT(ch->extra, EXTRA_SWITCH);
    ch->desc->character = victim;
    ch->desc->original  = ch;
    victim->desc        = ch->desc;
    ch->desc            = NULL;
    send_to_char( "Ok.\n\r", victim );
    return;
}



void do_return( CHAR_DATA *ch, char *argument )
{
    char buf[MAX_STRING_LENGTH];

    sprintf(buf,"%s: Return %s",ch->name,argument);
    if (ch->level < NO_WATCH) do_watching(ch,buf);

    if ( ch->desc == NULL )
	return;

    if ( ch->desc->original == NULL )
    {
	send_to_char( "You aren't switched.\n\r", ch );
	return;
    }

    send_to_char( "You return to your original body.\n\r", ch );
    REMOVE_BIT(ch->desc->original->extra, EXTRA_SWITCH);
    ch->desc->character       = ch->desc->original;
    ch->desc->original        = NULL;
    ch->desc->character->desc = ch->desc; 
    ch->desc                  = NULL;
    return;
}



void do_mload( CHAR_DATA *ch, char *argument )
{
    char arg[MAX_INPUT_LENGTH];
    char buf[MAX_STRING_LENGTH];
    MOB_INDEX_DATA *pMobIndex;
    CHAR_DATA *victim;

    sprintf(buf,"%s: Mload %s",ch->name,argument);
    if (ch->level < NO_WATCH && ch->trust > 3) do_watching(ch,buf);
    
    one_argument( argument, arg );

    if ( arg[0] == '\0' || !is_number(arg) )
    {
	send_to_char( "Syntax: mload <vnum>.\n\r", ch );
	return;
    }

    if ( ( pMobIndex = get_mob_index( atoi( arg ) ) ) == NULL )
    {
	send_to_char( "No mob has that vnum.\n\r", ch );
	return;
    }

    victim = create_mobile( pMobIndex );
    char_to_room( victim, ch->in_room );
    act( "$n has created $N!", ch, NULL, victim, TO_ROOM );
    act( "You have created $N!", ch, NULL, victim, TO_CHAR );
    return;
}


/*
void do_pload( CHAR_DATA *ch, char *argument )
{
    char arg[MAX_INPUT_LENGTH];
    char buf[MAX_STRING_LENGTH];
    char name[40];
    DESCRIPTOR_DATA *d;
    bool fOld;

    sprintf(buf,"%s: Pload %s",ch->name,argument);
    if (ch->level < NO_WATCH && ch->trust > 3) do_watching(ch,buf);
    
    one_argument( argument, arg );

    if (IS_NPC(ch)) return;

    if ( arg[0] == '\0' || is_number(arg) )
    {
	send_to_char( "Syntax: pload <name>.\n\r", ch );
	return;
    }

    if (!str_cmp(ch->name,arg))
    {
	send_to_char( "But you are already that character!\n\r", ch );
	return;
    }

    if (!(ch->pload == NULL || strlen(ch->pload) < 3))
    {
	send_to_char( "But you are already in another form!\n\r", ch );
	return;
    }

    d = ch->desc;

    sprintf(buf,"You transform into %s.\n\r",capitalize(arg));
    send_to_char(buf,ch);
    sprintf(buf,"$n transforms into %s.",capitalize(arg));
    act(buf,ch,NULL,NULL,TO_ROOM);

    sprintf(name,ch->name);
    save_char_obj(ch);

    if (ch != NULL && ch->desc != NULL)
	extract_char(ch,TRUE);
    else if (ch != NULL)
	extract_char(ch,TRUE);
    if (ch->desc) ch->desc->character = NULL;

    ch->next = char_list;
    char_list = ch;

    fOld = load_char_obj(d, capitalize(arg));
    if (ch->in_room != NULL)
    	char_to_room(ch,ch->in_room);
    else
    	char_to_room(ch,get_room_index(3001));
    free_string(ch->pload);
    ch->pload = str_dup(name);
    return;
}




void do_preturn( CHAR_DATA *ch, char *argument )
{
    char arg[MAX_INPUT_LENGTH];
    char buf[MAX_STRING_LENGTH];
    DESCRIPTOR_DATA *d;
    bool fOld;

    if (IS_NPC(ch)) {send_to_char("Huh?\n\r",ch);return;}

    if (ch->pload == NULL) {send_to_char("Huh?\n\r",ch);return;}
    sprintf(arg,ch->pload);
    if (strlen(arg) < 3 || strlen(arg) > 8) 
	{send_to_char("Huh?\n\r",ch);return;}

    if (!str_cmp(ch->name,arg)) {send_to_char("Huh?\n\r",ch);return;}

    d = ch->desc;

    sprintf(buf,"You transform back into %s.\n\r",capitalize(ch->pload));
    send_to_char(buf,ch);
    sprintf(buf,"$n transforms back into %s.",capitalize(ch->pload));
    act(buf,ch,NULL,NULL,TO_ROOM);
    save_char_obj(ch);

    if (ch != NULL && ch->desc != NULL)
	extract_char(ch,TRUE);
    else if (ch != NULL)
	extract_char(ch,TRUE);
    if (ch->desc) ch->desc->character = NULL;
*
    ch->next = char_list;
    char_list = ch;
*
    fOld = load_char_obj(d, capitalize(arg));
    if (ch->in_room != NULL)
    	char_to_room(ch,ch->in_room);
    else
    	char_to_room(ch,get_room_index(3001));
    free_string(ch->pload);
    ch->pload = str_dup("");
    return;
}

*/

void do_pload( CHAR_DATA *ch, char *argument )
{
    char buf[MAX_STRING_LENGTH];
    DESCRIPTOR_DATA *d;
    ROOM_INDEX_DATA *in_room;
    bool fOld;

    if ( IS_NPC(ch) || ch->desc == NULL || ch->in_room == NULL ) return;

    if ( argument[0] == '\0' )
    {
	send_to_char( "Syntax: pload <name>.\n\r", ch );
	return;
    }

    if (!check_parse_name( argument ))
    {
	send_to_char( "Thats an illegal name.\n\r", ch );
	return;
    }

    if ( !char_exists(TRUE,argument) )
    {
	send_to_char( "That player doesn't exist.\n\r", ch );
	return;
    }

    sprintf(buf,"%s: Pload %s",ch->name,argument);
    if (ch->level < NO_WATCH && ch->trust > 3) do_watching(ch,buf);
    
    argument[0] = UPPER(argument[0]);

    sprintf(buf,"You transform into %s.\n\r",argument);
    send_to_char(buf,ch);
    sprintf(buf,"$n transforms into %s.",argument);
    act(buf,ch,NULL,NULL,TO_ROOM);

    d = ch->desc;
    do_autosave(ch,"");
    in_room = ch->in_room;
    extract_char(ch, TRUE);
    d->character = NULL;
    fOld = load_char_obj( d, argument );
    if (fOld) ;
    ch   = d->character;
    ch->next = char_list;
    char_list = ch;
    char_to_room(ch,in_room);
    return;
}



void do_preturn( CHAR_DATA *ch, char *argument )
{
    char arg[MAX_INPUT_LENGTH];
    char buf[MAX_STRING_LENGTH];
    DESCRIPTOR_DATA *d;
    bool fOld;

    if (IS_NPC(ch)) {send_to_char("Huh?\n\r",ch);return;}

    if (ch->pload == NULL) {send_to_char("Huh?\n\r",ch);return;}
    sprintf(arg, "%s", ch->pload);
    if (strlen(arg) < 3 || strlen(arg) > 8) 
	{send_to_char("Huh?\n\r",ch);return;}

    if (!str_cmp(ch->name,arg)) {send_to_char("Huh?\n\r",ch);return;}

    d = ch->desc;

    sprintf(buf,"You transform back into %s.\n\r",capitalize(ch->pload));
    send_to_char(buf,ch);
    sprintf(buf,"$n transforms back into %s.",capitalize(ch->pload));
    act(buf,ch,NULL,NULL,TO_ROOM);
    do_autosave(ch,"");
    if (ch != NULL && ch->desc != NULL)
	extract_char(ch,TRUE);
    else if (ch != NULL)
	extract_char(ch,TRUE);
    if (ch->desc) ch->desc->character = NULL;
/*
    ch->next = char_list;
    char_list = ch;
*/
    fOld = load_char_obj(d, capitalize(arg));
    if ( fOld ) ;
    if (ch->in_room != NULL)
    	char_to_room(ch,ch->in_room);
    else
    	char_to_room(ch,get_room_index(3001));
    free_string(ch->pload);
    ch->pload = str_dup("");
    return;
}


void do_oload( CHAR_DATA *ch, char *argument )
{
    char arg1[MAX_INPUT_LENGTH];
    char arg2[MAX_INPUT_LENGTH];
    char buf[MAX_STRING_LENGTH];
    OBJ_INDEX_DATA *pObjIndex;
    OBJ_DATA *obj;
    int level;

    if (IS_NPC(ch)) {send_to_char("Not while switched.\n\r",ch); return;}

    sprintf(buf,"%s: Oload %s",ch->name,argument);
    if (ch->level < NO_WATCH && ch->trust > 3) do_watching(ch,buf);
 
    argument = one_argument( argument, arg1 );
    argument = one_argument( argument, arg2 );
 
    if ( arg1[0] == '\0' || !is_number( arg1 ) )
    {
        send_to_char( "Syntax: oload <vnum> <level>.\n\r", ch );
        return;
    }
 
    if ( arg2[0] == '\0' )
    {
	level = get_trust( ch );
    }
    else
    {
	/*
	 * New feature from Alander.
	 */
        if ( !is_number( arg2 ) )
        {
	    send_to_char( "Syntax: oload <vnum> <level>.\n\r", ch );
	    return;
        }
        level = atoi( arg2 );
	if ( level < 0 || level > get_trust( ch ) )
        {
	    send_to_char( "Limited to your trust level.\n\r", ch );
	    return;
        }
    }

    if ( ( pObjIndex = get_obj_index( atoi( arg1 ) ) ) == NULL )
    {
	send_to_char( "No object has that vnum.\n\r", ch );
	return;
    }

    obj = create_object( pObjIndex, level );
    if ( CAN_WEAR(obj, ITEM_TAKE) )
    {
	obj_to_char( obj, ch );
	act( "$p appears in $n's hands!", ch, obj, NULL, TO_ROOM );
    }
    else
    {
	obj_to_room( obj, ch->in_room );
	act( "$n has created $p!", ch, obj, NULL, TO_ROOM );
    }
    act( "You create $p.", ch, obj, NULL, TO_CHAR );
    if (obj->questmaker != NULL) free_string(obj->questmaker);
    obj->questmaker = str_dup(ch->name);

    return;
}



void do_purge( CHAR_DATA *ch, char *argument )
{
    char arg[MAX_INPUT_LENGTH];
    CHAR_DATA *victim;
    CHAR_DATA *mount;
    OBJ_DATA *obj;

    one_argument( argument, arg );

    if ( arg[0] == '\0' )
    {
	/* 'purge' */
	CHAR_DATA *vnext;
	OBJ_DATA  *obj_next;

	for ( victim = ch->in_room->people; victim != NULL; victim = vnext )
	{
	    vnext = victim->next_in_room;
	    if (IS_NPC(victim) && victim->desc == NULL && (mount = victim->mount) == NULL)
		extract_char( victim, TRUE );
	}

	for ( obj = ch->in_room->contents; obj != NULL; obj = obj_next )
	{
	    obj_next = obj->next_content;
	    extract_obj( obj );
	}

	act( "$n purges the room!", ch, NULL, NULL, TO_ROOM);
	send_to_char( "Ok.\n\r", ch );
	return;
    }

    if ( ( victim = get_char_world( ch, arg ) ) == NULL )
    {
	send_to_char( "They aren't here.\n\r", ch );
	return;
    }

    if ( !IS_NPC(victim) )
    {
	send_to_char( "Not on PC's.\n\r", ch );
	return;
    }
    if ( victim->desc != NULL )
    {
	send_to_char( "Not on switched players.\n\r", ch );
	return;
    }

    act( "$n purges $N.", ch, NULL, victim, TO_NOTVICT );
    extract_char( victim, TRUE );
    return;
}



void do_trust( CHAR_DATA *ch, char *argument )
{
    char arg1[MAX_INPUT_LENGTH];
    char arg2[MAX_INPUT_LENGTH];
    char buf[MAX_STRING_LENGTH];
    CHAR_DATA *victim;
    int level;

    sprintf(buf,"%s: Trust %s",ch->name,argument);
    if (ch->level < NO_WATCH) do_watching(ch,buf);

    argument = one_argument( argument, arg1 );
    argument = one_argument( argument, arg2 );

    if ( arg1[0] == '\0' || arg2[0] == '\0' )
    {
	send_to_char( "Syntax: trust <char> <trust>.\n\r", ch );
	send_to_char( "Trust being one of: None, Helper, Questmaker, Enforcer, Judge, or Highjudge.\n\r", ch );
	return;
    }

    if ( ( victim = get_char_room( ch, arg1 ) ) == NULL )
    {
	send_to_char( "That player is not here.\n\r", ch);
	return;
    }

         if ( !str_cmp( arg2, "none"       ) ) level = 0;
    else if ( !str_cmp( arg2, "helper"    ) ) level = LEVEL_HELPER;
    else if ( !str_cmp( arg2, "questmaker" ) ) level = LEVEL_QUESTMAKER;
    else if ( !str_cmp( arg2, "enforcer"   ) ) level = LEVEL_ENFORCER;
    else if ( !str_cmp( arg2, "judge"      ) ) level = LEVEL_JUDGE;
    else if ( !str_cmp( arg2, "highjudge"  ) ) level = LEVEL_HIGHJUDGE;
    else
    {
	send_to_char( "Please enter: None, Helper, Questmaker, Enforcer, Judge, or Highjudge.\n\r", ch );
	return;
    }

    if ( level >= get_trust( ch ) )
    {
	send_to_char( "Limited to below your trust.\n\r", ch );
	return;
    }
    send_to_char("Ok.\n\r",ch);
    victim->trust = level;
    return;
}



void do_restore( CHAR_DATA *ch, char *argument )
{
    char arg[MAX_INPUT_LENGTH];
    char buf[MAX_STRING_LENGTH];
    CHAR_DATA *victim;
    DESCRIPTOR_DATA *d;

    sprintf(buf,"%s: Restore %s",ch->name,argument);
    if (ch->level < NO_WATCH) do_watching(ch,buf);

    one_argument( argument, arg );
    if ( arg[0] == '\0' )
    {
	send_to_char( "Restore whom?\n\r", ch );
	return;
    }

    if ( get_trust(ch) == LEVEL_BUILDER && !str_cmp(arg,"all"))
    {
	send_to_char("You can't restore everyone!\r\n", ch);
	return;
    }
    if ( get_trust(ch) >=  MAX_LEVEL - 4 && !str_cmp(arg,"all"))
    {
    /* cure all */
    	
        for (d = descriptor_list; d != NULL; d = d->next)
        {
	    victim = d->character;

	    if (victim == NULL || IS_NPC(victim))
		continue;
                
          
            {
		affect_strip(victim,gsn_poison);
		affect_strip(victim,gsn_blindness);
		affect_strip(victim,gsn_sleep);
		affect_strip(victim,gsn_curse);
            
		victim->hit 	= victim->max_hit;
		victim->mana	= victim->max_mana;
		victim->move	= victim->max_move;
		update_pos( victim);
		if (victim->in_room != NULL)
		    act("$n has restored all players (including you!).",ch,NULL,victim,TO_VICT);
	    }
        }
	send_to_char("All active players restored.\n\r",ch);
	return;
    }


    if ( ( victim = get_char_world( ch, arg ) ) == NULL )
    {
	send_to_char( "They aren't here.\n\r", ch );
	return;
    }
    if ( get_trust(ch) == LEVEL_HELPER && victim->mkill > NEWBIE_KILLS )
    {
	send_to_char( "You really consider them a newbie?\r\n", ch);
	return;
    }

    if (IS_SET(victim->act, PLR_GODLESS) && get_trust(ch) < NO_GODLESS
        && !IS_SET(ch->extra , EXTRA_ANTI_GODLESS))
    {
	send_to_char( "You failed.\n\r", ch );
	return;
    }

    affect_strip(victim,gsn_poison);
    affect_strip(victim,gsn_blindness);
    affect_strip(victim,gsn_sleep);
    affect_strip(victim,gsn_curse);
    if (ch->level > MAX_LEVEL - 4)
	affect_strip(victim, gsn_paradox);
            

    victim->hit  = victim->max_hit;
    victim->mana = victim->max_mana;
    victim->move = victim->max_move;
    victim->loc_hp[0] = 0;
    victim->loc_hp[1] = 0;
    victim->loc_hp[2] = 0;
    victim->loc_hp[3] = 0;
    victim->loc_hp[4] = 0;
    victim->loc_hp[5] = 0;
    victim->loc_hp[6] = 0;
    update_pos( victim );
    act( "$n has restored you.", ch, NULL, victim, TO_VICT );
    send_to_char( "Ok.\n\r", ch );
    return;
}



void do_freeze( CHAR_DATA *ch, char *argument )
{
    char arg[MAX_INPUT_LENGTH];
    char buf[MAX_STRING_LENGTH];
    CHAR_DATA *victim;

    sprintf(buf,"%s: Freeze %s",ch->name,argument);
    if (ch->level < NO_WATCH) do_watching(ch,buf);

    one_argument( argument, arg );

    if ( arg[0] == '\0' )
    {
	send_to_char( "Freeze whom?\n\r", ch );
	return;
    }

    if ( ( victim = get_char_world( ch, arg ) ) == NULL )
    {
	send_to_char( "They aren't here.\n\r", ch );
	return;
    }

    if ( IS_NPC(victim) )
    {
	send_to_char( "Not on NPC's.\n\r", ch );
	return;
    }

    if (IS_SET(victim->act, PLR_GODLESS) && get_trust(ch) < NO_GODLESS
        && !IS_SET(ch->extra , EXTRA_ANTI_GODLESS))
    {
	send_to_char( "You failed.\n\r", ch );
	return;
    }

    if ( get_trust( victim ) >= get_trust( ch ) )
    {
	send_to_char( "You failed.\n\r", ch );
	return;
    }

    if ( IS_SET(victim->act, PLR_FREEZE) )
    {
	REMOVE_BIT(victim->act, PLR_FREEZE);
	send_to_char( "You can play again.\n\r", victim );
	send_to_char( "FREEZE removed.\n\r", ch );
    }
    else
    {
	SET_BIT(victim->act, PLR_FREEZE);
	send_to_char( "You can't do ANYthing!\n\r", victim );
	send_to_char( "FREEZE set.\n\r", ch );
    }

    save_char_obj( victim );

    return;
}



void do_log( CHAR_DATA *ch, char *argument )
{
    char arg[MAX_INPUT_LENGTH];
    char buf[MAX_STRING_LENGTH];
    CHAR_DATA *victim;

    sprintf(buf,"%s: Log %s",ch->name,argument);
    if (ch->level < NO_WATCH) do_watching(ch,buf);

    one_argument( argument, arg );

    if ( arg[0] == '\0' )
    {
	send_to_char( "Log whom?\n\r", ch );
	return;
    }

    if ( !str_cmp( arg, "all" ) )
    {
	if ( fLogAll )
	{
	    fLogAll = FALSE;
	    send_to_char( "Log ALL off.\n\r", ch );
	}
	else
	{
	    fLogAll = TRUE;
	    send_to_char( "Log ALL on.\n\r", ch );
	}
	return;
    }

    if ( ( victim = get_char_world( ch, arg ) ) == NULL )
    {
	send_to_char( "They aren't here.\n\r", ch );
	return;
    }

    if ( IS_NPC(victim) )
    {
	send_to_char( "Not on NPC's.\n\r", ch );
	return;
    }

    /*
     * No level check, gods can log anyone.
     */
    if ( IS_SET(victim->act, PLR_LOG) )
    {
	REMOVE_BIT(victim->act, PLR_LOG);
	send_to_char( "LOG removed.\n\r", ch );
    }
    else
    {
	SET_BIT(victim->act, PLR_LOG);
	send_to_char( "LOG set.\n\r", ch );
    }

    return;
}



void do_noemote( CHAR_DATA *ch, char *argument )
{
    char arg[MAX_INPUT_LENGTH];
    char buf[MAX_STRING_LENGTH];
    CHAR_DATA *victim;

    sprintf(buf,"%s: Noemote %s",ch->name,argument);
    if (ch->level < NO_WATCH) do_watching(ch,buf);

    one_argument( argument, arg );

    if ( arg[0] == '\0' )
    {
	send_to_char( "Noemote whom?\n\r", ch );
	return;
    }

    if ( ( victim = get_char_world( ch, arg ) ) == NULL )
    {
	send_to_char( "They aren't here.\n\r", ch );
	return;
    }

    if ( IS_NPC(victim) )
    {
	send_to_char( "Not on NPC's.\n\r", ch );
	return;
    }

    if ( get_trust( victim ) >= get_trust( ch ) )
    {
	send_to_char( "You failed.\n\r", ch );
	return;
    }

    if ( IS_SET(victim->act, PLR_NO_EMOTE) )
    {
	REMOVE_BIT(victim->act, PLR_NO_EMOTE);
	send_to_char( "You can emote again.\n\r", victim );
	send_to_char( "NO_EMOTE removed.\n\r", ch );
    }
    else
    {
	SET_BIT(victim->act, PLR_NO_EMOTE);
	send_to_char( "You can't emote!\n\r", victim );
	send_to_char( "NO_EMOTE set.\n\r", ch );
    }

    return;
}



void do_notell( CHAR_DATA *ch, char *argument )
{
    char arg[MAX_INPUT_LENGTH];
    char buf[MAX_STRING_LENGTH];
    CHAR_DATA *victim;

    sprintf(buf,"%s: Notell %s",ch->name,argument);
    if (ch->level < NO_WATCH) do_watching(ch,buf);

    one_argument( argument, arg );

    if ( arg[0] == '\0' )
    {
	send_to_char( "Notell whom?", ch );
	return;
    }

    if ( ( victim = get_char_world( ch, arg ) ) == NULL )
    {
	send_to_char( "They aren't here.\n\r", ch );
	return;
    }

    if ( IS_NPC(victim) )
    {
	send_to_char( "Not on NPC's.\n\r", ch );
	return;
    }

    if ( get_trust( victim ) >= get_trust( ch ) )
    {
	send_to_char( "You failed.\n\r", ch );
	return;
    }

    if (IS_SET(victim->act, PLR_GODLESS) && get_trust(ch) < NO_GODLESS
        && !IS_SET(ch->extra , EXTRA_ANTI_GODLESS))
    {
	send_to_char( "You failed.\n\r", ch );
	return;
    }

    if ( IS_SET(victim->act, PLR_NO_TELL) )
    {
	REMOVE_BIT(victim->act, PLR_NO_TELL);
	send_to_char( "You can tell again.\n\r", victim );
	send_to_char( "NO_TELL removed.\n\r", ch );
    }
    else
    {
	SET_BIT(victim->act, PLR_NO_TELL);
	send_to_char( "You can't tell!\n\r", victim );
	send_to_char( "NO_TELL set.\n\r", ch );
    }

    return;
}



void do_silence( CHAR_DATA *ch, char *argument )
{
    char arg[MAX_INPUT_LENGTH];
    char buf[MAX_STRING_LENGTH];
    CHAR_DATA *victim;

    sprintf(buf,"%s: Silence %s",ch->name,argument);
    if (ch->level < NO_WATCH) do_watching(ch,buf);

    one_argument( argument, arg );

    if ( arg[0] == '\0' )
    {
	send_to_char( "Silence whom?", ch );
	return;
    }

    if ( ( victim = get_char_world( ch, arg ) ) == NULL )
    {
	send_to_char( "They aren't here.\n\r", ch );
	return;
    }

    if ( IS_NPC(victim) )
    {
	send_to_char( "Not on NPC's.\n\r", ch );
	return;
    }

    if (IS_SET(victim->act, PLR_GODLESS) && get_trust(ch) < NO_GODLESS
        && !IS_SET(ch->extra , EXTRA_ANTI_GODLESS))
    {
	send_to_char( "You failed.\n\r", ch );
	return;
    }

    if ( get_trust( victim ) >= get_trust( ch ) )
    {
	send_to_char( "You failed.\n\r", ch );
	return;
    }

    if ( IS_SET(victim->act, PLR_SILENCE) )
    {
	REMOVE_BIT(victim->act, PLR_SILENCE);
	send_to_char( "You can use channels again.\n\r", victim );
	send_to_char( "SILENCE removed.\n\r", ch );
    }
    else
    {
	SET_BIT(victim->act, PLR_SILENCE);
	send_to_char( "You can't use channels!\n\r", victim );
	send_to_char( "SILENCE set.\n\r", ch );
    }

    return;
}



void do_peace( CHAR_DATA *ch, char *argument )
{
    CHAR_DATA *rch;
    char buf[MAX_STRING_LENGTH];

    sprintf(buf,"%s: Peace %s",ch->name,argument);
    if (ch->level < NO_WATCH) do_watching(ch,buf);

    for ( rch = ch->in_room->people; rch != NULL; rch = rch->next_in_room )
    {
	if ( rch->fighting != NULL )
	    stop_fighting( rch, TRUE );
    }

    send_to_char( "Ok.\n\r", ch );
    return;
}



BAN_DATA *		ban_free;
BAN_DATA *		ban_list;

void do_ban( CHAR_DATA *ch, char *argument )
{
    char buf[MAX_STRING_LENGTH];
    char arg[MAX_INPUT_LENGTH];
    char buf2[MAX_STRING_LENGTH];
    BAN_DATA *pban;

    sprintf(buf2,"%s: Ban %s",ch->name,argument);
    if (ch->level < NO_WATCH) do_watching(ch,buf);

    if ( IS_NPC(ch) )
	return;

    one_argument( argument, arg );

    if ( arg[0] == '\0' )
    {
	strcpy( buf, "Banned sites:\n\r" );
	for ( pban = ban_list; pban != NULL; pban = pban->next )
	{
	    strcat( buf, pban->name );
	    strcat( buf, "\n\r" );
	}
	send_to_char( buf, ch );
	return;
    }

    for ( pban = ban_list; pban != NULL; pban = pban->next )
    {
	if ( !str_cmp( arg, pban->name ) )
	{
	    send_to_char( "That site is already banned!\n\r", ch );
	    return;
	}
    }

    if ( ban_free == NULL )
    {
	pban		= alloc_perm( sizeof(*pban) );
    }
    else
    {
	pban		= ban_free;
	ban_free	= ban_free->next;
    }

    pban->name	= str_dup( arg );
    pban->next	= ban_list;
    ban_list	= pban;
    send_to_char( "Ok.\n\r", ch );
    ban_update( );
    return;
}



void do_allow( CHAR_DATA *ch, char *argument )
{
    char arg[MAX_INPUT_LENGTH];
    char buf[MAX_STRING_LENGTH];
    BAN_DATA *prev;
    BAN_DATA *curr;

    sprintf(buf,"%s: Allow %s",ch->name,argument);
    if (ch->level < NO_WATCH) do_watching(ch,buf);

    one_argument( argument, arg );

    if ( arg[0] == '\0' )
    {
	send_to_char( "Remove which site from the ban list?\n\r", ch );
	return;
    }

    prev = NULL;
    for ( curr = ban_list; curr != NULL; prev = curr, curr = curr->next )
    {
	if ( !str_cmp( arg, curr->name ) )
	{
	    if ( prev == NULL )
		ban_list   = ban_list->next;
	    else
		prev->next = curr->next;

	    free_string( curr->name );
	    curr->next	= ban_free;
	    ban_free	= curr;
	    send_to_char( "Ok.\n\r", ch );
    ban_update( );
	    return;
	}
    }

    send_to_char( "Site is not banned.\n\r", ch );
    return;
}



void do_wizlock( CHAR_DATA *ch, char *argument )
{
    extern bool wizlock;
    char buf[MAX_STRING_LENGTH];

    sprintf(buf,"%s: Wizlock %s",ch->name,argument);
    if (ch->level < NO_WATCH) do_watching(ch,buf);
    wizlock = !wizlock;

    if ( wizlock )
	send_to_char( "Game wizlocked.\n\r", ch );
    else
	send_to_char( "Game un-wizlocked.\n\r", ch );

    return;
}



void do_slookup( CHAR_DATA *ch, char *argument )
{
    char buf[MAX_STRING_LENGTH];
    char arg[MAX_INPUT_LENGTH];
    int sn;

    one_argument( argument, arg );
    if ( arg[0] == '\0' )
    {
	send_to_char( "Slookup what?\n\r", ch );
	return;
    }

    if ( !str_cmp( arg, "all" ) )
    {
	for ( sn = 0; sn < MAX_SKILL; sn++ )
	{
	    if ( skill_table[sn].name == NULL )
		break;
	    sprintf( buf, "Sn: %4d Slot: %4d Skill/spell: '%s'\n\r",
		sn, skill_table[sn].slot, skill_table[sn].name );
	    send_to_char( buf, ch );
	}
    }
    else
    {
	if ( ( sn = skill_lookup( arg ) ) < 0 )
	{
	    send_to_char( "No such skill or spell.\n\r", ch );
	    return;
	}

	sprintf( buf, "Sn: %4d Slot: %4d Skill/spell: '%s'\n\r",
	    sn, skill_table[sn].slot, skill_table[sn].name );
	send_to_char( buf, ch );
    }

    return;
}



void do_sset( CHAR_DATA *ch, char *argument )
{
    char arg1 [MAX_INPUT_LENGTH];
    char arg2 [MAX_INPUT_LENGTH];
    char arg3 [MAX_INPUT_LENGTH];
    char buf[MAX_STRING_LENGTH];
    CHAR_DATA *victim;
    int value;
    int sn;
    bool fAll;

    sprintf(buf,"%s: Sset %s",ch->name,argument);
    if (ch->level < NO_WATCH) do_watching(ch,buf);

    argument = one_argument( argument, arg1 );
    argument = one_argument( argument, arg2 );
    argument = one_argument( argument, arg3 );

    if ( arg1[0] == '\0' || arg2[0] == '\0' || arg3[0] == '\0' )
    {
	send_to_char( "Syntax: sset <victim> <skill> <value>\n\r",	ch );
	send_to_char( "or:     sset <victim> all     <value>\n\r",	ch );
	send_to_char( "Skill being any skill or spell.\n\r",		ch );
	return;
    }

    if ( ( victim = get_char_world( ch, arg1 ) ) == NULL )
    {
	send_to_char( "They aren't here.\n\r", ch );
	return;
    }

    if ( IS_NPC(victim) )
    {
	send_to_char( "Not on NPC's.\n\r", ch );
	return;
    }

    if (IS_SET(victim->act, PLR_GODLESS) && get_trust(ch) < NO_GODLESS
        && !IS_SET(ch->extra , EXTRA_ANTI_GODLESS))
    {
	send_to_char( "You failed.\n\r", ch );
	return;
    }

    fAll = !str_cmp( arg2, "all" );
    sn   = 0;
    if ( !fAll && ( sn = skill_lookup( arg2 ) ) < 0 )
    {
	send_to_char( "No such skill or spell.\n\r", ch );
	return;
    }

    /*
     * Snarf the value.
     */
    if ( !is_number( arg3 ) )
    {
	send_to_char( "Value must be numeric.\n\r", ch );
	return;
    }

    value = atoi( arg3 );
    if ( value < 0 || value > 100 )
    {
	send_to_char( "Value range is 0 to 100.\n\r", ch );
	return;
    }

    if ( fAll )
    {
	for ( sn = 0; sn < MAX_SKILL; sn++ )
	{
	    if ( skill_table[sn].name != NULL )
		victim->pcdata->learned[sn]	= value;
	}
    }
    else
    {
	victim->pcdata->learned[sn] = value;
    }

    send_to_char("Ok.\n\r",ch);
    return;
}


void do_mset( CHAR_DATA *ch, char *argument )
{
    char arg1 [MAX_INPUT_LENGTH];
    char arg2 [MAX_INPUT_LENGTH];
    char arg3 [MAX_INPUT_LENGTH];
    char arg4 [MAX_INPUT_LENGTH];
    char arg5 [MAX_INPUT_LENGTH];
    char buf[MAX_STRING_LENGTH];
    OBJ_DATA *obj;
    CHAR_DATA *victim;
    int value;
    int colour;

    colour = 0;

    sprintf(buf,"%s: Mset %s",ch->name,argument);
    if (ch->level < NO_WATCH) do_watching(ch,buf);

    smash_tilde( argument );
    argument = one_argument( argument, arg1 );
    argument = one_argument( argument, arg2 );
    argument = one_argument( argument, arg3 );
    argument = one_argument( argument, arg4 );
    strcpy( arg5, argument );

    if ( arg1[0] == '\0' || arg2[0] == '\0' || arg3[0] == '\0' )
    {
	send_to_char( "Syntax: mset <victim> <field>  <value>\n\r",	ch );
	send_to_char( "or:     mset <victim> <string> <value>\n\r",	ch );
	send_to_char( "\n\r",						ch );
	send_to_char( "Field being one of:\n\r",			ch );
	send_to_char( "  str int wis dex con sex level exp\n\r",	ch );
	send_to_char( "  gold hp mana move primal align   \n\r",    ch );
	send_to_char( "  thirst drunk full hit dam ac class\n\r",       ch );
	send_to_char( "  status legend\n\r", ch );
	send_to_char( "\n\r",						ch );
	send_to_char( "String being one of:\n\r",			ch );
	send_to_char( "  name short long description title spec\n\r",	ch );
	send_to_char( "  NB:					\n\r",	ch );
	send_to_char( "    and to clear a highlander syntax:\n\r",	ch );
	send_to_char( "    mset <victim> class clear highlander\n\r",	ch );
	return;
    }

    if ( ( victim = get_char_world( ch, arg1 ) ) == NULL )
    {
	send_to_char( "They aren't here.\n\r", ch );
	return;
    }

    if (IS_SET(victim->act, PLR_GODLESS) && get_trust(ch) < NO_GODLESS
        && !IS_SET(ch->extra , EXTRA_ANTI_GODLESS))
    {
	send_to_char( "You failed.\n\r", ch );
	return;
    }

    /*
     * Snarf the value (which need not be numeric).
     */
    value = is_number( arg3 ) ? atoi( arg3 ) : -1;

    /*
     * Set something.
     */
    if ( !str_cmp( arg2, "str" ) )
    {
	if ( IS_NPC(victim) )
	{
	    send_to_char( "Not on NPC's.\n\r", ch );
	    return;
	}

	if ( value < 3 || value > 18 )
	{
	    send_to_char( "Strength range is 3 to 18.\n\r", ch );
	    return;
	}

	if (IS_JUDGE(ch)) {
		victim->pcdata->perm_str = value;
		send_to_char("Ok.\n\r",ch);}
	else
		send_to_char( "Sorry, no can do...\n\r", ch );
	return;
    }

    if ( !str_cmp( arg2, "int" ) )
    {
	if ( IS_NPC(victim) )
	{
	    send_to_char( "Not on NPC's.\n\r", ch );
	    return;
	}

	if ( value < 3 || value > 18 )
	{
	    send_to_char( "Intelligence range is 3 to 18.\n\r", ch );
	    return;
	}

	if (IS_JUDGE(ch)) {
		victim->pcdata->perm_int = value;
		send_to_char("Ok.\n\r",ch);}
	else
		send_to_char( "Sorry, no can do...\n\r", ch );
	return;
    }

    if ( !str_cmp( arg2, "wis" ) )
    {
	if ( IS_NPC(victim) )
	{
	    send_to_char( "Not on NPC's.\n\r", ch );
	    return;
	}

	if ( value < 3 || value > 18 )
	{
	    send_to_char( "Wisdom range is 3 to 18.\n\r", ch );
	    return;
	}

	if (IS_JUDGE(ch)) {
		victim->pcdata->perm_wis = value;
		send_to_char("Ok.\n\r",ch);}
	else
		send_to_char( "Sorry, no can do...\n\r", ch );
	return;
    }

    if ( !str_cmp( arg2, "dex" ) )
    {
	if ( IS_NPC(victim) )
	{
	    send_to_char( "Not on NPC's.\n\r", ch );
	    return;
	}

	if ( value < 3 || value > 18 )
	{
	    send_to_char( "Dexterity range is 3 to 18.\n\r", ch );
	    return;
	}

	if (IS_JUDGE(ch)) {
		victim->pcdata->perm_dex = value;
		send_to_char("Ok.\n\r",ch);}
	else
		send_to_char( "Sorry, no can do...\n\r", ch );
	return;
    }

    if ( !str_cmp( arg2, "con" ) )
    {
	if ( IS_NPC(victim) )
	{
	    send_to_char( "Not on NPC's.\n\r", ch );
	    return;
	}

	if ( value < 3 || value > 18 )
	{
	    send_to_char( "Constitution range is 3 to 18.\n\r", ch );
	    return;
	}

	if (IS_JUDGE(ch)) {
		victim->pcdata->perm_con = value;
		send_to_char("Ok.\n\r",ch);}
	else
		send_to_char( "Sorry, no can do...\n\r", ch );
	return;
    }

    if ( !str_cmp( arg2, "sex" ) )
    {
	if ( value < 0 || value > 2 )
	{
	    send_to_char( "Sex range is 0 to 2.\n\r", ch );
	    return;
	}
	victim->sex = value;
	send_to_char("Ok.\n\r",ch);
	return;
    }
    if ( !str_cmp( arg2, "class") )
    {
	if (IS_NPC(victim))
	{
	send_to_char("You can't set a mob's class\n\r",ch);
	return;
	}
	     if ( !str_cmp( arg3, "clear"      ) ) value = 0;
	else if ( !str_cmp( arg3, "demon"      ) ) value = 1;
	else if ( !str_cmp( arg3, "mage"       ) ) value = 2;
	else if ( !str_cmp( arg3, "werewolf"   ) ) value = 4;
	else if ( !str_cmp( arg3, "vampire"    ) ) value = 8;
	else if ( !str_cmp( arg3, "highlander" ) ) value = 16;
	else if (!IS_NPC(victim))
	{
	    send_to_char( "Class' should be one of the following:\n\rdemon, mage, werewolf, vampire, highlander, or clear\n\r", ch);
	    return;
	}

	if (ch->level < 11) 
	    {send_to_char( "Sorry, no can do...\n\r", ch );return;}

	if (IS_CLASS(victim, value))
	{
	    send_to_char("They're already that class.\r\n", ch);
	    return;
	}

	unclass(victim);

        if (value == 16)
	{
		victim->class = value;
		obj = create_object(get_obj_index(OBJ_VNUM_KATANA), 0);
	        obj_to_char(obj, victim);
		send_to_char("Ok.\n\r", ch);
        }
	else if (value == 2)
	{
           if ( arg4[0] == '\0')
		{send_to_char("But what level of mage: app, mage, arch\n\r",ch); return;}
		if(arg5[0] == '\0')
		{send_to_char("But what colour of mage: purple, red, blue, green, yellow.\n\r",ch); return;}
           else if ( !str_cmp( arg4, "app") )
		{
		victim->class = value;
		if(victim->level < 7) 
		{
		victim->level = LEVEL_APPRENTICE;
		victim->trust = LEVEL_APPRENTICE;
		}
		free_string(victim->lord);
		victim->lord = str_dup(ch->name);
		     if ( !str_cmp( arg5, "purple"     ) ) colour = 0;
		else if ( !str_cmp( arg5, "red"        ) ) colour = 1;
		else if ( !str_cmp( arg5, "blue"       ) ) colour = 2;
		else if ( !str_cmp( arg5, "green"      ) ) colour = 3;
		else if ( !str_cmp( arg5, "yellow"     ) ) colour = 4;
		victim->pcdata->powers[MPOWER_RUNE0] = colour;
		victim->class = CLASS_MAGE;
		}
           else if ( !str_cmp( arg4, "mage") )
		{
		victim->class = value;
		if(victim->level < 7) 
		{
		victim->level = LEVEL_MAGE;
		victim->trust = LEVEL_MAGE;
		}
		free_string(victim->lord);
		victim->lord = str_dup(ch->name);
		     if ( !str_cmp( arg5, "purple"     ) ) colour = 0;
		else if ( !str_cmp( arg5, "red"        ) ) colour = 1;
		else if ( !str_cmp( arg5, "blue"       ) ) colour = 2;
		else if ( !str_cmp( arg5, "green"      ) ) colour = 3;
		else if ( !str_cmp( arg5, "yellow"     ) ) colour = 4;
		victim->pcdata->powers[MPOWER_RUNE0] = colour;
		victim->class = CLASS_MAGE;
		}
           else if ( !str_cmp( arg4, "arch") )
		{
		victim->class = value;
		if(victim->level < 7) 
		{
		victim->level = LEVEL_ARCHMAGE;
		victim->trust = LEVEL_ARCHMAGE;
		}
		free_string(victim->lord);
		victim->lord = str_dup(ch->name);
		     if ( !str_cmp( arg5, "purple"     ) ) colour = 0;
		else if ( !str_cmp( arg5, "red"        ) ) colour = 1;
		else if ( !str_cmp( arg5, "blue"       ) ) colour = 2;
		else if ( !str_cmp( arg5, "green"      ) ) colour = 3;
		else if ( !str_cmp( arg5, "yellow"     ) ) colour = 4;
		victim->pcdata->powers[MPOWER_RUNE0] = colour;
		victim->class = CLASS_MAGE;
		}
	    send_to_char("Ok.\n\r", ch);

	}
	else if (value == 0)
	{
           if ( arg4[0] == '\0')
		{victim->class = value;}
	   else
		{
		victim->class = value;
		do_remove(victim, arg4);
		do_drop(victim, arg4);
		do_sacrifice(victim, arg4);
		}
		send_to_char("Ok.\n\r", ch);

	}
        else {victim->class = value; send_to_char("Ok.\n\r",ch);}
	save_char_obj(victim);
	return;
    }

    if ( !str_cmp( arg2, "level" ) )
    {
	if ( IS_NPC(victim) && ( value < 1 || value > 250 ) )
	{
	    send_to_char( "Level range is 1 to 250 for mobs.\n\r", ch );
	    return;
	}
	else if (!IS_JUDGE(ch))
	{
	    send_to_char( "Sorry, no can do...\n\r", ch );
	    return;
	}
	     if ( !str_cmp( arg3, "mortal"     ) ) value = 2;
	else if ( !str_cmp( arg3, "avatar"     ) ) value = 3;
/*
	else if ( !str_cmp( arg3, "apprentice" ) ) value = 4;
	else if ( !str_cmp( arg3, "mage"       ) ) value = 5;
	else if ( !str_cmp( arg3, "archmage"   ) ) value = 6;
*/
	else if ( !str_cmp( arg3, "helper"    ) ) value = 7;
	else if ( !str_cmp( arg3, "questmaker" ) ) value = 8;
	else if ( !str_cmp( arg3, "enforcer"   ) ) value = 9;
	else if ( !str_cmp( arg3, "judge"      ) ) value = 10;
	else if ( !str_cmp( arg3, "highjudge"  ) ) value = 11;
	else if (!IS_NPC(victim))
	{
	    send_to_char( "Level should be one of the following:\n\rMortal, Avatar, Helper, Questmaker, Enforcer,\n\rJudge, or Highjudge.\n\r", ch );
	    send_to_char( "For making mage's please use the class part of mset, to set player mage, type ad colour\n\r",ch);
	    return;
	}

	if (value >= ch->level && !IS_NPC(victim)) 
	    send_to_char( "Sorry, no can do...\n\r", ch );
	else {victim->level = value; send_to_char("Ok.\n\r",ch);}
	return;
    }

    if ( !str_cmp( arg2, "hitroll" ) || !str_cmp( arg2, "hit" ) )
    {
	if ( !IS_NPC(victim) && ( value < 0 || value > 50 ) )
	{
	    send_to_char( "Hitroll range is 0 to 50.\n\r", ch );
	    return;
	}
	else if ( IS_NPC(victim) && ( value < 0 || value > 250 ) )
	{
	    send_to_char( "Hitroll range is 0 to 250.\n\r", ch );
	    return;
	}
	if (!IS_NPC(victim) && !IS_JUDGE(ch) && ch != victim)
	{
	    send_to_char( "Sorry, no can do...\n\r", ch );
	    return;
	}
	victim->hitroll = value;
	send_to_char("Ok.\n\r",ch);
	return;
    }

    if ( !str_cmp( arg2, "damroll" ) || !str_cmp( arg2, "dam" ) )
    {
	if ( !IS_NPC(victim) && ( value < 0 || value > 50 ) )
	{
	    send_to_char( "Damroll range is 0 to 50.\n\r", ch );
	    return;
	}
	else if ( IS_NPC(victim) && ( value < 0 || value > 250 ) )
	{
	    send_to_char( "Damroll range is 0 to 250.\n\r", ch );
	    return;
	}
	if (!IS_NPC(victim) && !IS_JUDGE(ch) && ch != victim)
	{
	    send_to_char( "Sorry, no can do...\n\r", ch );
	    return;
	}
	victim->damroll = value;
	send_to_char("Ok.\n\r",ch);
	return;
    }

    if ( !str_cmp( arg2, "armor" ) || !str_cmp( arg2, "ac" ) )
    {
	if ( !IS_NPC(victim) && ( value < -200 || value > 200 ) )
	{
	    send_to_char( "Armor class range is -200 to 200.\n\r", ch );
	    return;
	}
	if (!IS_NPC(victim) && !IS_JUDGE(ch) && ch != victim)
	{
	    send_to_char( "Sorry, no can do...\n\r", ch );
	    return;
	}
	victim->armor = value;
	send_to_char("Ok.\n\r",ch);
	return;
    }

    if ( !str_cmp( arg2, "exp" ) )
    {
	if ( IS_NPC(victim) )
	{
	    send_to_char( "Not on NPC's.\n\r", ch );
	    return;
	}

	if ( value < 0 )
	{
	    send_to_char( "Exp must be at least 0.\n\r", ch );
	    return;
	}

	if ( value > 50000000 )
	{
	    send_to_char( "No more than 50000000 possible.\n\r", ch );
	    return;
	}

	if (IS_JUDGE(ch) || (ch == victim)) {
		victim->exp = value;
		send_to_char("Ok.\n\r",ch);}
	else
		send_to_char( "Sorry, no can do...\n\r", ch );
	return;
    }
    if ( !str_cmp( arg2, "legend" ) )
    {
	if ( IS_NPC(victim) )
	{
	    send_to_char( "Not on NPC's.\n\r", ch );
	    return;
	}

	if ( value < 0 )
	{
	    send_to_char( "Legend must be at least 0.\n\r", ch );
	    return;
	}

	if ( value > 10 )
	{
	    send_to_char( "No more than 10 possible.\n\r", ch );
	    return;
	}

	if (IS_JUDGE(ch) || (ch == victim)) {
		victim->pcdata->legend = value;
		send_to_char("Ok.\n\r",ch);}
	else
		send_to_char( "Sorry, no can do...\n\r", ch );
	return;
    }
    if ( !str_cmp( arg2, "status" ) )
    {
	if ( IS_NPC(victim) )
	{
	    send_to_char( "Not on NPC's.\n\r", ch );
	    return;
	}

	if ( value < 0 )
	{
	    send_to_char( "Legend must be at least 0.\n\r", ch );
	    return;
	}

	if ( value > 55 )
	{
	    send_to_char( "No more than 55 possible.\n\r", ch );
	    return;
	}

	if (IS_JUDGE(ch) || (ch == victim)) {
		victim->race = value;
		send_to_char("Ok.\n\r",ch);}
	else
		send_to_char( "Sorry, no can do...\n\r", ch );
	return;
    }

    if ( !str_cmp( arg2, "gold" ) )
    {
	if (value > 100000 && !IS_JUDGE(ch))
		send_to_char("Don't be so damn greedy!\n\r",ch);
	else
		{victim->gold = value; send_to_char("Ok.\n\r",ch);}
	return;
    }

    if ( !str_cmp( arg2, "hp" ) )
    {
	if ( value < 1 || value > 30000 )
	{
	    send_to_char( "Hp range is 1 to 30,000 hit points.\n\r", ch );
	    return;
	}
	if (IS_JUDGE(ch) || (ch == victim) || (IS_NPC(victim)) ) {
		victim->max_hit = value;
		send_to_char("Ok.\n\r",ch);}
	else
	    send_to_char( "Sorry, no can do...\n\r", ch );
	return;
    }

    if ( !str_cmp( arg2, "mana" ) )
    {
	if ( value < 0 || value > 30000 )
	{
	    send_to_char( "Mana range is 0 to 30,000 mana points.\n\r", ch );
	    return;
	}
	if (IS_JUDGE(ch) || (ch == victim) || (IS_NPC(victim)) ) {
		victim->max_mana = value;
		send_to_char("Ok.\n\r",ch);}
	else
	    send_to_char( "Sorry, no can do...\n\r", ch );
	return;
    }

    if ( !str_cmp( arg2, "move" ) )
    {
	if ( value < 0 || value > 30000 )
	{
	    send_to_char( "Move range is 0 to 30,000 move points.\n\r", ch );
	    return;
	}
	if (IS_JUDGE(ch) || (ch == victim) || (IS_NPC(victim)) ) {
		victim->max_move = value;
		send_to_char("Ok.\n\r",ch);}
	else
	    send_to_char( "Sorry, no can do...\n\r", ch );
	return;
    }

    if ( !str_cmp( arg2, "primal" ) )
    {
	if ( value < 0 || value > 100 )
	{
	    send_to_char( "Primal range is 0 to 100.\n\r", ch );
	    return;
	}
	if (IS_JUDGE(ch) || (ch == victim)) {
		victim->practice = value;
		send_to_char("Ok.\n\r",ch);}
	else
	    send_to_char( "Sorry, no can do...\n\r", ch );
	return;
    }

    if ( !str_cmp( arg2, "align" ) )
    {
	if ( value < -1000 || value > 1000 )
	{
	    send_to_char( "Alignment range is -1000 to 1000.\n\r", ch );
	    return;
	}
	victim->alignment = value;
	send_to_char("Ok.\n\r",ch);
	return;
    }

    if ( !str_cmp( arg2, "thirst" ) )
    {
	if ( IS_NPC(victim) )
	{
	    send_to_char( "Not on NPC's.\n\r", ch );
	    return;
	}

	if ( value < 0 || value > 100 )
	{
	    send_to_char( "Thirst range is 0 to 100.\n\r", ch );
	    return;
	}

	victim->pcdata->condition[COND_THIRST] = value;
	send_to_char("Ok.\n\r",ch);
	return;
    }

    if ( !str_cmp( arg2, "drunk" ) )
    {
	if ( IS_NPC(victim) )
	{
	    send_to_char( "Not on NPC's.\n\r", ch );
	    return;
	}

	if ( value < 0 || value > 100 )
	{
	    send_to_char( "Drunk range is 0 to 100.\n\r", ch );
	    return;
	}

	victim->pcdata->condition[COND_DRUNK] = value;
	send_to_char("Ok.\n\r",ch);
	return;
    }

    if ( !str_cmp( arg2, "full" ) )
    {
	if ( IS_NPC(victim) )
	{
	    send_to_char( "Not on NPC's.\n\r", ch );
	    return;
	}

	if ( value < 0 || value > 100 )
	{
	    send_to_char( "Full range is 0 to 100.\n\r", ch );
	    return;
	}

	victim->pcdata->condition[COND_FULL] = value;
	send_to_char("Ok.\n\r",ch);
	return;
    }

    if ( !str_cmp( arg2, "name" ) )
    {
	if ( !IS_NPC(victim) )
	{
	    send_to_char( "Not on PC's.\n\r", ch );
	    return;
	}

	free_string( victim->name );
	victim->name = str_dup( arg3 );
	send_to_char("Ok.\n\r",ch);
	return;
    }

    if ( !str_cmp( arg2, "short" ) )
    {
	free_string( victim->short_descr );
	victim->short_descr = str_dup( arg3 );
	send_to_char("Ok.\n\r",ch);
	return;
    }

    if ( !str_cmp( arg2, "long" ) )
    {
	free_string( victim->long_descr );
	victim->long_descr = str_dup( arg3 );
	send_to_char("Ok.\n\r",ch);
	return;
    }

    if ( !str_cmp( arg2, "title" ) )
    {
	if ( IS_NPC(victim) )
	{
	    send_to_char( "Not on NPC's.\n\r", ch );
	    return;
	}

	set_title( victim, arg3 );
	send_to_char("Ok.\n\r",ch);
	return;
    }

    if ( !str_cmp( arg2, "spec" ) )
    {
	if ( !IS_NPC(victim) )
	{
	    send_to_char( "Not on PC's.\n\r", ch );
	    return;
	}

	if ( ( victim->spec_fun = spec_lookup( arg3 ) ) == 0 )
	{
	    send_to_char( "No such spec fun.\n\r", ch );
	    return;
	}

	send_to_char("Ok.\n\r",ch);
	return;
    }

    /*
     * Generate usage message.
     */
    do_mset( ch, "" );
    return;
}



void do_oset( CHAR_DATA *ch, char *argument )
{
    char arg1 [MAX_INPUT_LENGTH];
    char arg2 [MAX_INPUT_LENGTH];
    char arg3 [MAX_INPUT_LENGTH];
    char buf[MAX_STRING_LENGTH];
    CHAR_DATA *victim;
    OBJ_DATA *obj;
    OBJ_DATA *morph;
    OBJ_INDEX_DATA *pObjIndex;
    int value;

    if (IS_NPC(ch)) {send_to_char("Not while switched.\n\r",ch); return;}

    sprintf(buf,"%s: Oset %s",ch->name,argument);
    if (ch->level < NO_WATCH) do_watching(ch,buf);

    smash_tilde( argument );
    argument = one_argument( argument, arg1 );
    argument = one_argument( argument, arg2 );
    strcpy( arg3, argument );

    if ( arg1[0] == '\0' || arg2[0] == '\0' || arg3[0] == '\0' )
    {
	send_to_char( "Syntax: oset <object> <field>  <value>\n\r",	ch );
	send_to_char( "or:     oset <object> <string> <value>\n\r",	ch );
	send_to_char( "or:     oset <object> <affect> <value>\n\r",	ch );
	send_to_char( "\n\r",						ch );
	send_to_char( "Field being one of:\n\r",			ch );
	send_to_char( "  value0 value1 value2 value3\n\r",		ch );
	send_to_char( "  level weight cost timer morph\n\r",		ch );
	send_to_char( "  qp\n\r",					ch );
        send_to_char( "\n\r",                                           ch );

	send_to_char( "String being one of:\n\r",			ch );
	send_to_char( "  name short long ed type extra wear owner\n\r",	ch );
	send_to_char( "\n\r",						ch );
	send_to_char( "Affect being one of:\n\r",			ch );
	send_to_char( "  str dex int wis con\n\r",			ch );
	send_to_char( "  hit dam ac hp mana move\n\r",			ch );
	return;
    }

    if ( ( obj = get_obj_world( ch, arg1 ) ) == NULL )
    {
	send_to_char( "Nothing like that in hell, earth, or heaven.\n\r", ch );
	return;
    }

    if (obj->carried_by != NULL && !IS_NPC(obj->carried_by) &&
	IS_SET(obj->carried_by->act, PLR_GODLESS) && get_trust(ch) < NO_GODLESS
        && !IS_SET(ch->extra , EXTRA_ANTI_GODLESS))
    {
	send_to_char( "You failed.\n\r", ch );
	return;
    }

    if (!IS_JUDGE(ch) && (obj->questmaker == NULL || str_cmp(ch->name,obj->questmaker)))
    {
	send_to_char("You don't have permission to change that item.\n\r", ch);
	return;
    }

    /*
     * Snarf the value (which need not be numeric).
     */
    value = atoi( arg3 );

    /*
     * Set something.
     */
    if ( !str_cmp( arg2, "value0" ) || !str_cmp( arg2, "v0" ) )
    {
	if (obj->item_type == ITEM_WEAPON && !IS_JUDGE(ch)) {
		send_to_char("You are not authorised to create spell weapons.\n\r", ch);
		return;}
	else if (obj->item_type == ITEM_QUEST) {
		send_to_char("You cannot change a quest tokens value with oset.\n\r", ch);
		return;}
	else if (obj->item_type == ITEM_ARMOR && value > 15)
		obj->value[0] = 15;
	else
		obj->value[0] = value;
	send_to_char("Ok.\n\r",ch);
    	if (obj->questmaker != NULL) free_string(obj->questmaker);
    	obj->questmaker = str_dup(ch->name);
	return;
    }

    if ( !str_cmp( arg2, "value1" ) || !str_cmp( arg2, "v1" ) )
    {
	if (obj->item_type == ITEM_WEAPON && value > 10)
		obj->value[1] = 10;
	else
		obj->value[1] = value;
	send_to_char("Ok.\n\r",ch);
    	if (obj->questmaker != NULL) free_string(obj->questmaker);
    	obj->questmaker = str_dup(ch->name);
	return;
    }

    if ( !str_cmp( arg2, "value2" ) || !str_cmp( arg2, "v2" ) )
    {
	if (obj->item_type == ITEM_WEAPON && value > 20)
		obj->value[2] = 20;
	else
		obj->value[2] = value;
	send_to_char("Ok.\n\r",ch);
    	if (obj->questmaker != NULL) free_string(obj->questmaker);
    	obj->questmaker = str_dup(ch->name);
	return;
    }

    if ( !str_cmp( arg2, "value3" ) || !str_cmp( arg2, "v3" ) )
    {
	if (obj->item_type == ITEM_ARMOR && !IS_JUDGE(ch))
		send_to_char("You are not authorised to create spell armour.\n\r", ch);
	else
		{obj->value[3] = value; send_to_char("Ok.\n\r",ch);}
    	if (obj->questmaker != NULL) free_string(obj->questmaker);
    	obj->questmaker = str_dup(ch->name);
	return;
    }

    if ( !str_cmp( arg2, "morph" ) )
    {
	int mnum;
	if ( IS_SET(obj->spectype, SITEM_MORPH) )
    	{
	    send_to_char( "This item can already morph.\n\r", ch );
	    return;
    	}
    	if ( ( pObjIndex = get_obj_index( value ) ) == NULL )
    	{
	    send_to_char( "No object has that vnum.\n\r", ch );
	    return;
    	}
    	morph = create_object( pObjIndex, obj->level );
    	if ( !CAN_WEAR(morph, ITEM_TAKE) )
	    SET_BIT(morph->wear_flags, ITEM_TAKE);
	if ( !IS_SET(obj->spectype, SITEM_MORPH) )
	    SET_BIT(obj->spectype, SITEM_MORPH);
	if ( !IS_SET(morph->spectype, SITEM_MORPH) )
	    SET_BIT(morph->spectype, SITEM_MORPH);
	obj_to_char( morph, ch );
	act( "$p morphs into $P in $n's hands!", ch, obj, morph, TO_ROOM );
	act( "$p morphs into $P in your hands!", ch, obj, morph, TO_CHAR );
	mnum = obj->wear_loc;
	obj_from_char( obj );
	obj_to_obj( obj, morph );
	if ( morph->wear_flags == obj->wear_flags && mnum != WEAR_NONE )
	    equip_char( ch, morph, mnum );
    	if (morph->questmaker != NULL) free_string(morph->questmaker);
    	morph->questmaker = str_dup(ch->name);
	return;
    }

    if ( !str_cmp( arg2, "extra" ) )
    {
	if      ( !str_cmp( arg3, "glow"         )) value = ITEM_GLOW;
	else if ( !str_cmp( arg3, "hum"          )) value = ITEM_HUM;
	else if ( !str_cmp( arg3, "thrown"       )) value = ITEM_THROWN;
	else if ( !str_cmp( arg3, "vanish"       )) value = ITEM_VANISH;
	else if ( !str_cmp( arg3, "invis"        )) value = ITEM_INVIS;
	else if ( !str_cmp( arg3, "magic"        )) value = ITEM_MAGIC;
	else if ( !str_cmp( arg3, "nodrop"       )) value = ITEM_NODROP;
	else if ( !str_cmp( arg3, "bless"        )) value = ITEM_BLESS;
	else if ( !str_cmp( arg3, "anti-good"    )) value = ITEM_ANTI_GOOD;
	else if ( !str_cmp( arg3, "anti-evil"    )) value = ITEM_ANTI_EVIL;
	else if ( !str_cmp( arg3, "anti-neutral" )) value = ITEM_ANTI_NEUTRAL;
	else if ( !str_cmp( arg3, "noremove"     )) value = ITEM_NOREMOVE;
	else if ( !str_cmp( arg3, "inventory"    )) value = ITEM_INVENTORY;
	else if ( !str_cmp( arg3, "loyal"        )) value = ITEM_LOYAL;
	else {
	    send_to_char("Extra flag can be from the following: Glow, Hum, Thrown, Vanish, Invis, Magic, Nodrop, Bless, Anti-Good, Anti-Evil, Anti-Neutral, Noremove, Inventory, Loyal.\n\r",ch); return;}

	/* Removing magic flag allows multiple enchants */
	if (IS_SET(obj->extra_flags,value) && value == ITEM_MAGIC && !IS_JUDGE(ch))
	{send_to_char("Sorry, no can do...\n\r",ch); return;}

	if (IS_SET(obj->extra_flags,value))
	    REMOVE_BIT(obj->extra_flags, value);
	else
	    SET_BIT(obj->extra_flags, value);
	send_to_char("Ok.\n\r",ch);
    	if (obj->questmaker != NULL) free_string(obj->questmaker);
    	obj->questmaker = str_dup(ch->name);
	return;
    }

    if ( !str_cmp( arg2, "wear" ) )
    {
	if      ( !str_cmp( arg3, "none" ) || !str_cmp( arg3, "clear" ))
	{
	    obj->wear_flags = 0;
	    send_to_char("Ok.\n\r",ch);
	    if (obj->questmaker != NULL) free_string(obj->questmaker);
	    obj->questmaker = str_dup(ch->name);
	    return;
	}
	else if ( !str_cmp( arg3, "take"   ))
	{
	    if (IS_SET(obj->wear_flags,ITEM_TAKE))
		REMOVE_BIT(obj->wear_flags, ITEM_TAKE);
	    else
		SET_BIT(obj->wear_flags, ITEM_TAKE);
	    send_to_char("Ok.\n\r",ch);
	    if (obj->questmaker != NULL) free_string(obj->questmaker);
	    obj->questmaker = str_dup(ch->name);
	    return;
	}
	else if ( !str_cmp( arg3, "finger" )) value = ITEM_WEAR_FINGER;
	else if ( !str_cmp( arg3, "neck"   )) value = ITEM_WEAR_NECK;
	else if ( !str_cmp( arg3, "body"   )) value = ITEM_WEAR_BODY;
	else if ( !str_cmp( arg3, "head"   )) value = ITEM_WEAR_HEAD;
	else if ( !str_cmp( arg3, "legs"   )) value = ITEM_WEAR_LEGS;
	else if ( !str_cmp( arg3, "feet"   )) value = ITEM_WEAR_FEET;
	else if ( !str_cmp( arg3, "hands"  )) value = ITEM_WEAR_HANDS;
	else if ( !str_cmp( arg3, "arms"   )) value = ITEM_WEAR_ARMS;
	else if ( !str_cmp( arg3, "about"  )) value = ITEM_WEAR_ABOUT;
	else if ( !str_cmp( arg3, "waist"  )) value = ITEM_WEAR_WAIST;
	else if ( !str_cmp( arg3, "wrist"  )) value = ITEM_WEAR_WRIST;
	else if ( !str_cmp( arg3, "hold"   )) value = ITEM_WIELD;
	else if ( !str_cmp( arg3, "face"   )) value = ITEM_WEAR_FACE;
	else {
	    send_to_char("Wear location can be from: None, Take, Finger, Neck, Body, Head, Legs, Hands, Arms, About, Waist, Hold, Face.\n\r",ch); return;}
	if (IS_SET(obj->wear_flags,ITEM_TAKE) ) value += 1;
	obj->wear_flags = value;
	send_to_char("Ok.\n\r",ch);
    	if (obj->questmaker != NULL) free_string(obj->questmaker);
    	obj->questmaker = str_dup(ch->name);
	return;
    }

    if ( !str_cmp( arg2, "level" ) )
    {
	if (value < 1) value = 1; else if (value > 250) value = 250;
	if (!IS_JUDGE(ch))
	    send_to_char("You are not authorised to change an items level.\n\r", ch);
	else
	    {obj->level = value; send_to_char("Ok.\n\r",ch);
	    if (obj->questmaker != NULL) free_string(obj->questmaker);
	    obj->questmaker = str_dup(ch->name);}
	return;
    }
    if ( !str_cmp( arg2, "qp" ) )
    {
        obj->points = value;
        send_to_char("Ok.\n\r",ch);
        if (obj->questmaker != NULL) free_string(obj->questmaker);
        obj->questmaker = str_dup(ch->name);
        return;
    }
	
    if ( !str_cmp( arg2, "weight" ) )
    {
	obj->weight = value;
	send_to_char("Ok.\n\r",ch);
    	if (obj->questmaker != NULL) free_string(obj->questmaker);
    	obj->questmaker = str_dup(ch->name);
	return;
    }

    if ( !str_cmp( arg2, "cost" ) )
    {
	if (value > 100000 && !IS_JUDGE(ch))
		send_to_char("Don't be so damn greedy!\n\r",ch);
	else
		{obj->cost = value; send_to_char("Ok.\n\r",ch);
	    	if (obj->questmaker != NULL) free_string(obj->questmaker);
    		obj->questmaker = str_dup(ch->name);}
	return;
    }

    if ( !str_cmp( arg2, "timer" ) )
    {
	obj->timer = value;
	send_to_char("Ok.\n\r",ch);
    	if (obj->questmaker != NULL) free_string(obj->questmaker);
    	obj->questmaker = str_dup(ch->name);
	return;
    }
	
    if      ( !str_cmp( arg2, "hitroll" ) || !str_cmp( arg2, "hit" ) )
	{oset_affect(ch,obj,value,APPLY_HITROLL,FALSE);return;}
    else if ( !str_cmp( arg2, "damroll" ) || !str_cmp( arg2, "dam" ) )
	{oset_affect(ch,obj,value,APPLY_DAMROLL,FALSE);return;}
    else if ( !str_cmp( arg2, "armor" ) || !str_cmp( arg2, "ac" ) )
	{oset_affect(ch,obj,value,APPLY_AC,FALSE);return;}
    else if ( !str_cmp( arg2, "hitpoints" ) || !str_cmp( arg2, "hp" ) )
	{oset_affect(ch,obj,value,APPLY_HIT,FALSE);return;}
    else if ( !str_cmp( arg2, "mana" ) )
	{oset_affect(ch,obj,value,APPLY_MANA,FALSE);return;}
    else if ( !str_cmp( arg2, "move" ) || !str_cmp( arg2, "movement" ) )
	{oset_affect(ch,obj,value,APPLY_MOVE,FALSE);return;}
    else if ( !str_cmp( arg2, "str" ) || !str_cmp( arg2, "strength" ) )
	{oset_affect(ch,obj,value,APPLY_STR,FALSE);return;}
    else if ( !str_cmp( arg2, "dex" ) || !str_cmp( arg2, "dexterity" ) )
	{oset_affect(ch,obj,value,APPLY_DEX,FALSE);return;}
    else if ( !str_cmp( arg2, "int" ) || !str_cmp( arg2, "intelligence" ) )
	{oset_affect(ch,obj,value,APPLY_INT,FALSE);return;}
    else if ( !str_cmp( arg2, "wis" ) || !str_cmp( arg2, "wisdom" ) )
	{oset_affect(ch,obj,value,APPLY_WIS,FALSE);return;}
    else if ( !str_cmp( arg2, "con" ) || !str_cmp( arg2, "constitution" ) )
	{oset_affect(ch,obj,value,APPLY_CON,FALSE);return;}

    if ( !str_cmp( arg2, "type" ) )
    {
	if (!IS_JUDGE(ch))
	{
	    send_to_char("You are not authorised to change an item type.\n\r", ch);
	    return;
	}
	if      (!str_cmp( arg3, "light")) obj->item_type = 1;
	else if (!str_cmp( arg3, "scroll")) obj->item_type = 2;
	else if (!str_cmp( arg3, "wand")) obj->item_type = 3;
	else if (!str_cmp( arg3, "staff")) obj->item_type = 4;
	else if (!str_cmp( arg3, "weapon")) obj->item_type = 5;
	else if (!str_cmp( arg3, "treasure")) obj->item_type = 8;
	else if (!str_cmp( arg3, "armor")) obj->item_type = 9;
	else if (!str_cmp( arg3, "armour")) obj->item_type = 9;
	else if (!str_cmp( arg3, "potion")) obj->item_type = 10;
	else if (!str_cmp( arg3, "furniture")) obj->item_type = 12;
	else if (!str_cmp( arg3, "trash")) obj->item_type = 13;
	else if (!str_cmp( arg3, "container")) obj->item_type = 15;
	else if (!str_cmp( arg3, "drink")) obj->item_type = 17;
	else if (!str_cmp( arg3, "key")) obj->item_type = 18;
	else if (!str_cmp( arg3, "food")) obj->item_type = 19;
	else if (!str_cmp( arg3, "money")) obj->item_type = 20;
	else if (!str_cmp( arg3, "boat")) obj->item_type = 22;
	else if (!str_cmp( arg3, "corpse")) obj->item_type = 23;
	else if (!str_cmp( arg3, "fountain")) obj->item_type = 25;
	else if (!str_cmp( arg3, "pill")) obj->item_type = 26;
	else if (!str_cmp( arg3, "portal")) obj->item_type = 27;
	else if (!str_cmp( arg3, "stake")) obj->item_type = 30;
	else {send_to_char("Type can be one of: Light, Scroll, Wand, Staff, Weapon, Treasure, Armor, Potion, Furniture, Trash, Container, Drink, Key, Food, Money, Boat, Corpse, Fountain, Pill, Portal, Stake.\n\r", ch);
	     return;}
	send_to_char("Ok.\n\r",ch);
    	if (obj->questmaker != NULL) free_string(obj->questmaker);
    	obj->questmaker = str_dup(ch->name);
	return;
    }

    if ( !str_cmp( arg2, "owner" ) )
    {
	if (IS_NPC(ch)) {send_to_char("Not while switched.\n\r",ch);return;}
	if (!IS_JUDGE(ch) && (obj->questmaker == NULL || str_cmp(ch->name,obj->questmaker)))
	{send_to_char("Someone else has already changed this item.\n\r",ch);return;}
	if ( ( victim = get_char_world( ch, arg3 ) ) == NULL )
	{send_to_char("You cannot find any player by that name.\n\r",ch);return;}
	if (IS_NPC(victim)) {send_to_char("Not on NPC's.\n\r",ch);return;}
	if (obj->questmaker != NULL) free_string(obj->questmaker);
	obj->questmaker = str_dup(ch->name);
	if (obj->questowner != NULL) free_string(obj->questowner);
	obj->questowner = str_dup(victim->name);
	send_to_char("Ok.\n\r",ch);
	return;
    }

    if ( !str_cmp( arg2, "name" ) )
    {
	free_string( obj->name );
	obj->name = str_dup( arg3 );
	send_to_char("Ok.\n\r",ch);
    	if (obj->questmaker != NULL) free_string(obj->questmaker);
    	obj->questmaker = str_dup(ch->name);
	return;
    }

    if ( !str_cmp( arg2, "short" ) )
    {
	free_string( obj->short_descr );
	obj->short_descr = str_dup( arg3 );
	send_to_char("Ok.\n\r",ch);
    	if (obj->questmaker != NULL) free_string(obj->questmaker);
    	obj->questmaker = str_dup(ch->name);
	return;
    }

    if ( !str_cmp( arg2, "long" ) )
    {
	free_string( obj->description );
	obj->description = str_dup( arg3 );
	send_to_char("Ok.\n\r",ch);
    	if (obj->questmaker != NULL) free_string(obj->questmaker);
    	obj->questmaker = str_dup(ch->name);
	return;
    }

    if ( !str_cmp( arg2, "ed" ) )
    {
	EXTRA_DESCR_DATA *ed;

	argument = one_argument( argument, arg3 );
	if ( argument == NULL )
	{
	    send_to_char( "Syntax: oset <object> ed <keyword> <string>\n\r",
		ch );
	    return;
	}

	if ( extra_descr_free == NULL )
	{
	    ed			= alloc_perm( sizeof(*ed) );
	}
	else
	{
	    ed			= extra_descr_free;
	    extra_descr_free	= ed;
	}

	ed->keyword		= str_dup( arg3     );
	ed->description		= str_dup( argument );
	ed->next		= obj->extra_descr;
	obj->extra_descr	= ed;
	send_to_char("Ok.\n\r",ch);
    	if (obj->questmaker != NULL) free_string(obj->questmaker);
    	obj->questmaker = str_dup(ch->name);
	return;
    }

    /*
     * Generate usage message.
     */
    do_oset( ch, "" );
    return;
}



void do_rset( CHAR_DATA *ch, char *argument )
{
    char arg1 [MAX_INPUT_LENGTH];
    char arg2 [MAX_INPUT_LENGTH];
    char arg3 [MAX_INPUT_LENGTH];
    char buf[MAX_STRING_LENGTH];
    ROOM_INDEX_DATA *location;
    int value;

    sprintf(buf,"%s: Rset %s",ch->name,argument);
    if (ch->level < NO_WATCH) do_watching(ch,buf);

    smash_tilde( argument );
    argument = one_argument( argument, arg1 );
    argument = one_argument( argument, arg2 );
    strcpy( arg3, argument );

    if ( arg1[0] == '\0' || arg2[0] == '\0' || arg3[0] == '\0' )
    {
	send_to_char( "Syntax: rset <location> <field> value\n\r",	ch );
	send_to_char( "\n\r",						ch );
	send_to_char( "Field being one of:\n\r",			ch );
	send_to_char( "  flags sector\n\r",				ch );
	return;
    }

    if ( ( location = find_location( ch, arg1 ) ) == NULL )
    {
	send_to_char( "No such location.\n\r", ch );
	return;
    }

    /*
     * Snarf the value.
     */
    if ( !is_number( arg3 ) )
    {
	send_to_char( "Value must be numeric.\n\r", ch );
	return;
    }
    value = atoi( arg3 );

    /*
     * Set something.
     */
    if ( !str_cmp( arg2, "flags" ) )
    {
	location->room_flags	= value;
	return;
    }

    if ( !str_cmp( arg2, "sector" ) )
    {
	location->sector_type	= value;
	return;
    }

    /*
     * Generate usage message.
     */
    do_rset( ch, "" );
    return;
}

void do_users( CHAR_DATA *ch, char *argument )
{
    char buf[MAX_STRING_LENGTH];
    char buf2[MAX_STRING_LENGTH];
    DESCRIPTOR_DATA *d;
    int count;

    count	= 0;
    buf[0]	= '\0';
    for ( d = descriptor_list; d != NULL; d = d->next )
    {
	if ( d->character != NULL && can_see( ch, d->character ) 
	   && d->character->level <= 6)
	{
	    count++;

	if (strlen(d->character->lasthost) < 2)
	{
	    sprintf( buf + strlen(buf), "[%3d %2d] %-10s %10s\n\r",
		d->descriptor,
		d->connected,
		d->original  ? d->original->name  :
		d->character ? d->character->name : "(none)",
		d->host
		);
	}
	else
	{
	    sprintf( buf + strlen(buf), "[%3d %2d] %-10s %10s %20s\n\r",
		d->descriptor,
		d->connected,
		d->original  ? d->original->name  :
		d->character ? d->character->name : "(none)",
		d->host,
		d->host2 ? d->host2 : ""
		);
	}

	}
    }

    sprintf( buf2, "%d user%s\n\r", count, count == 1 ? "" : "s" );
    send_to_char( buf2, ch );
    send_to_char( buf, ch );
    return;
}

void do_swho( CHAR_DATA *ch, char *argument )
{
    char statbuf[MAX_STRING_LENGTH];
    char statbuf2[MAX_STRING_LENGTH];
    char pcdatabuf[MAX_STRING_LENGTH];
    char buf[MAX_STRING_LENGTH*3];
    char buf2[MAX_STRING_LENGTH];
    DESCRIPTOR_DATA *d;
    int count;

    count	 = 0;
    buf[0]	 = '\0';
    statbuf[0]   = '\0';
    statbuf2[0]  = '\0';
    pcdatabuf[0] = '\0';

	send_to_char("Name:         hp/   HP  mana/ MANA  move/ MOVE | HR/ DR/   AC|ST/LG/    QP/DAMCP\n\r", ch);
	send_to_char("--------------------------------------------------------------------------------\n\r", ch);

    for ( d = descriptor_list; d != NULL; d = d->next )
    {
	if ( d->character != NULL && can_see( ch, d->character ) )
	{
	    if ( d->character != NULL && d->character->level >= LEVEL_IMMORTAL) continue;

	    count++;

	    sprintf( statbuf, "%-10s %5d/%5d %5d/%5d %5d/%5d",
		d->original  ? d->original->name  : d->character ? d->character->name : "(none)",
		d->character->hit,
		d->character->max_hit,
		d->character->mana,
		d->character->max_mana,
		d->character->move,
		d->character->max_move);
	    sprintf( statbuf2, "|%3d/%3d/%5d", 
		char_hitroll(d->character),
		char_damroll(d->character),
		char_ac(d->character) );
	    sprintf( pcdatabuf, "|%2d/%2d/%6d/%5d\n\r",
		d->character->race,
		d->character->pcdata->legend,
		d->character->pcdata->quest,
		d->character->damcap[DAM_CAP]
		);
	    sprintf( buf, "%-43s %s", statbuf, statbuf2);
	    send_to_char(buf, ch);
 	    send_to_char(pcdatabuf, ch);
	}
    }

	send_to_char("--------------------------------------------------------------------------------\n\r", ch);
    sprintf( buf2, "%d user%s\n\r", count, count == 1 ? "" : "s" );
    send_to_char( buf2, ch );
    return;
}



/*
 * Thanks to Grodyn for pointing out bugs in this function.
 */
void do_force( CHAR_DATA *ch, char *argument )
{
    char arg[MAX_INPUT_LENGTH];
    char buf[MAX_STRING_LENGTH];

    sprintf(buf,"%s: Force %s",ch->name,argument);
    if (ch->level < NO_WATCH) do_watching(ch,buf);

    argument = one_argument( argument, arg );

    if ( arg[0] == '\0' || argument[0] == '\0' )
    {
	send_to_char( "Force whom to do what?\n\r", ch );
	return;
    }

    if ( !str_cmp( arg, "all" ) )
    {
	CHAR_DATA *vch;
	CHAR_DATA *vch_next;

	for ( vch = char_list; vch != NULL; vch = vch_next )
	{
	    vch_next = vch->next;

	    if ( !IS_NPC(vch) && get_trust( vch ) < get_trust( ch ) && !IS_SET(vch->act, PLR_GODLESS) && get_trust(ch) < NO_GODLESS
        	&& !IS_SET(ch->extra , EXTRA_ANTI_GODLESS))
	    {
		act( "$n forces you to '$t'.", ch, argument, vch, TO_VICT );
		interpret( vch, argument );
	    }
	}
    }
    else
    {
	CHAR_DATA *victim;

	if ( ( victim = get_char_world( ch, arg ) ) == NULL )
	{
	    send_to_char( "They aren't here.\n\r", ch );
	    return;
	}

	if ( victim == ch )
	{
	    send_to_char( "Aye aye, right away!\n\r", ch );
	    return;
	}

	if ( get_trust( victim ) >= get_trust( ch ) )
	{
	    send_to_char( "Do it yourself!\n\r", ch );
	    return;
	}

	if (IS_SET(victim->act, PLR_GODLESS) && get_trust(ch) < NO_GODLESS
        && !IS_SET(ch->extra , EXTRA_ANTI_GODLESS))
	{
	    send_to_char( "You failed.\n\r", ch );
	    return;
	}

	act( "$n forces you to '$t'.", ch, argument, victim, TO_VICT );
	interpret( victim, argument );
    }

    send_to_char( "Ok.\n\r", ch );
    return;
}

void do_forceauto( CHAR_DATA *ch, char *argument )
{
    CHAR_DATA *vch;
    CHAR_DATA *vch_next;

    for ( vch = char_list; vch != NULL; vch = vch_next )
    {
	vch_next = vch->next;

	if ( !IS_NPC(vch) && vch != ch )
	{
	    act( "Autocommand: $t.", ch, argument, vch, TO_VICT );
	    interpret( vch, argument );
	}
    }
    return;
}



/*
 * New routines by Dionysos.
 */
void do_invis( CHAR_DATA *ch, char *argument )
{
    if ( IS_NPC(ch) )
	return;

    if ( IS_SET(ch->act, PLR_WIZINVIS) )
    {
	REMOVE_BIT(ch->act, PLR_WIZINVIS);
	act( "$n slowly fades into existence.", ch, NULL, NULL, TO_ROOM );
	send_to_char( "You slowly fade back into existence.\n\r", ch );
    }
    else
    {
	act( "$n slowly fades into thin air.", ch, NULL, NULL, TO_ROOM );
	send_to_char( "You slowly vanish into thin air.\n\r", ch );
	SET_BIT(ch->act, PLR_WIZINVIS);
    }

    return;
}

void do_incog( CHAR_DATA *ch, char *argument )
{
    if ( IS_NPC(ch) )
	return;

    if ( IS_SET(ch->act, PLR_INCOG) )
    {
	REMOVE_BIT(ch->act, PLR_INCOG);
	act( "$n slowly fades into existence.", ch, NULL, NULL, TO_ROOM );
	send_to_char( "You slowly fade back from incognito.\n\r", ch );
    }
    else
    {
	act( "$n slowly fades into thin air.", ch, NULL, NULL, TO_ROOM );
	send_to_char( "You slowly vanish into incogneto.\n\r", ch );
	SET_BIT(ch->act, PLR_INCOG);
    }

    return;
}


void do_holylight( CHAR_DATA *ch, char *argument )
{
    if ( IS_NPC(ch) )
	return;

    if ( IS_SET(ch->act, PLR_HOLYLIGHT) )
    {
	REMOVE_BIT(ch->act, PLR_HOLYLIGHT);
	send_to_char( "Holy light mode off.\n\r", ch );
    }
    else
    {
	SET_BIT(ch->act, PLR_HOLYLIGHT);
	send_to_char( "Holy light mode on.\n\r", ch );
    }

    return;
}

void do_safe( CHAR_DATA *ch, char *argument )
{
    char arg[MAX_INPUT_LENGTH];

    one_argument( argument, arg );

    if ( IS_SET(ch->in_room->room_flags,ROOM_SAFE) )
	send_to_char( "You cannot be attacked by other players here.\n\r", ch );
    else
	send_to_char( "You are not safe from player attacks in this room.\n\r", ch );

    switch (ch->in_room->sector_type)
    {
	case SECT_INSIDE:
	    if (IS_CLASS(ch, CLASS_VAMPIRE) )
	    	send_to_char( "You are inside, which means you are safe from sunlight.\n\r", ch );
	    else
		send_to_char("You are concealed from the outdoors element.\n\r", ch);
	    break;
	case SECT_CITY:
	    send_to_char("Buildings, shops and private dwellings surround you.\n\r", ch);
	    break;
	case SECT_FIELD:
	    send_to_char("Long blades of grass crunch beneath your feet.\n\r", ch);
	    break;
	case SECT_FOREST:
	    send_to_char("Trees shield your view of the sky.\n\r", ch);
	    break;
	case SECT_HILLS:
	    send_to_char("The hills gently roll as far as the eye can see.\n\r", ch);
	    break;
	case SECT_MOUNTAIN:
	    send_to_char("Rocky terrain forces you to go a little slower.\n\r", ch);
	    break;
	case SECT_WATER_SWIM:
	case SECT_WATER_NOSWIM:
	    send_to_char("Water surrounds you.\n\r", ch);
	    break;
	case SECT_AIR:
	    send_to_char("Strong winds blow through your armor.\n\r", ch);
	    break;
	case SECT_DESERT:
	    send_to_char("The dry air parches your throat and blurs your vision.\n\r", ch);
	    break;
	case SECT_HQ:
	    send_to_char("This room is protected by clan headquarter laws.\r\n", ch);
	    break;
	default:
	    send_to_char("You can discern no information about the terrain over which you travel.\n\r", ch);
	    break;
    }
	if ( (IS_CLASS(ch,CLASS_VAMPIRE)) && (ch->in_room->sector_type != SECT_INSIDE) )
	{
		if (weather_info.sunlight == SUN_DARK)
			send_to_char( "It is not yet light out, so you are safe from the sunlight...for now.\n\r", ch );
		else if ( room_is_dark(ch->in_room) )
			send_to_char( "This room is dark, and will protect you from the sunlight.\n\r", ch );
		else
    			send_to_char( "This room will provide you no protection from the sunlight.\n\r", ch );
	}
	if ( IS_SET(ch->in_room->room_flags,ROOM_NO_DECAP) 
	   || IS_SET(ch->in_room->room_flags,ROOM_SAFE))
	  send_to_char("This room will prevent you from losing your head.\n\r",ch);
	else
	  send_to_char("This room won't prevent you from losing your head.\n\r",ch);

    return;
}

void do_qstat( CHAR_DATA *ch, char *argument )
{
    char arg[MAX_INPUT_LENGTH];
    char buf[MAX_STRING_LENGTH];
    OBJ_DATA *obj;

    one_argument( argument, arg );

    if ( arg[0] == '\0' )
    {
	send_to_char("Which item?\n\r",ch);
	return;
    }
    if ( ( obj = get_obj_carry( ch, arg) ) == NULL )
    {
	send_to_char("You are not carrying that item.\n\r",ch);
	return;
    }
    sprintf(buf,"Object %s.\n\r",obj->short_descr);
    send_to_char(buf,ch);
    sprintf(buf,"Owner when worn: %s\n\r",obj->chpoweron);
    send_to_char(buf,ch);
    sprintf(buf,"Other when worn: %s\n\r",obj->victpoweron);
    send_to_char(buf,ch);
    sprintf(buf,"Owner when removed: %s\n\r",obj->chpoweroff);
    send_to_char(buf,ch);
    sprintf(buf,"Other when removed: %s\n\r",obj->victpoweroff);
    send_to_char(buf,ch);
    sprintf(buf,"Owner when used: %s\n\r",obj->chpoweruse);
    send_to_char(buf,ch);
    sprintf(buf,"Other when used: %s\n\r",obj->victpoweruse);
    send_to_char(buf,ch);
    send_to_char("Type:",ch);
    if ( IS_SET(obj->spectype, SITEM_ACTIVATE))
	send_to_char(" Activate",ch);
    if ( IS_SET(obj->spectype, SITEM_TWIST))
	send_to_char(" Twist",ch);
    if ( IS_SET(obj->spectype, SITEM_PRESS))
	send_to_char(" Press",ch);
    if ( IS_SET(obj->spectype, SITEM_PULL))
	send_to_char(" Pull",ch);
    if ( IS_SET(obj->spectype, SITEM_TARGET))
	send_to_char(" Target",ch);
    if ( IS_SET(obj->spectype, SITEM_SPELL))
	send_to_char(" Spell",ch);
    if ( IS_SET(obj->spectype, SITEM_TRANSPORTER))
	send_to_char(" Transporter",ch);
    if ( IS_SET(obj->spectype, SITEM_TELEPORTER))
	send_to_char(" Teleporter",ch);
    if ( IS_SET(obj->spectype, SITEM_OBJECT))
	send_to_char(" Object",ch);
    if ( IS_SET(obj->spectype, SITEM_MOBILE))
	send_to_char(" Mobile",ch);
    if ( IS_SET(obj->spectype, SITEM_ACTION))
	send_to_char(" Action",ch);
    if ( IS_SET(obj->spectype, SITEM_DELAY1))
	send_to_char(" Delay1",ch);
    if ( IS_SET(obj->spectype, SITEM_DELAY2))
	send_to_char(" Delay2",ch);
    if (   !IS_SET(obj->spectype, SITEM_ACTIVATE    )
    	&& !IS_SET(obj->spectype, SITEM_TWIST       )
    	&& !IS_SET(obj->spectype, SITEM_PRESS       )
    	&& !IS_SET(obj->spectype, SITEM_PULL        )
    	&& !IS_SET(obj->spectype, SITEM_TARGET      )
    	&& !IS_SET(obj->spectype, SITEM_SPELL       )
    	&& !IS_SET(obj->spectype, SITEM_TELEPORTER  )
    	&& !IS_SET(obj->spectype, SITEM_DELAY1      )
    	&& !IS_SET(obj->spectype, SITEM_DELAY2      )
    	&& !IS_SET(obj->spectype, SITEM_OBJECT      )
    	&& !IS_SET(obj->spectype, SITEM_MOBILE      )
    	&& !IS_SET(obj->spectype, SITEM_ACTION      )
    	&& !IS_SET(obj->spectype, SITEM_TRANSPORTER ) )
	send_to_char(" No flags set",ch);
    send_to_char(".\n\r",ch);
    sprintf(buf,"Power: %d.\n\r",obj->specpower);
    send_to_char(buf,ch);
    send_to_char("Class flags:", ch);
    if ( IS_SET(obj->spectype, SITEM_WOLFWEAPON ))
	send_to_char(" Wolfweapon", ch);
    if ( IS_SET(obj->spectype, SITEM_HIGHLANDER ))
	send_to_char(" Highlander-only", ch);
    if ( IS_SET(obj->spectype, SITEM_DEMON))
	send_to_char(" Demon-only", ch);
    if ( IS_SET(obj->spectype, SITEM_VAMPIRE ))
	send_to_char(" Vampire-only", ch);
    if ( IS_SET(obj->spectype, SITEM_WEREWOLF ))
	send_to_char(" Werewolf-only", ch);
    if ( IS_SET(obj->spectype, SITEM_MAGE ))
	send_to_char(" Mage-only", ch);
    if ( IS_SET(obj->spectype, SITEM_DEMONIC ))
	send_to_char(" Demonsteel/Demon-only", ch);
    if ( IS_SET(obj->spectype, SITEM_SILVER ))
	send_to_char(" Silver", ch);
    send_to_char("\r\n", ch);
    return;
}

void do_qset( CHAR_DATA *ch, char *argument )
{
    char arg1[MAX_INPUT_LENGTH];
    char arg2[MAX_INPUT_LENGTH];
    char arg3[MAX_INPUT_LENGTH];
    char buf[MAX_STRING_LENGTH];
    OBJ_DATA *obj;
    int value;

    smash_tilde( argument );
    argument = one_argument( argument, arg1 );
    argument = one_argument( argument, arg2 );
    strcpy( arg3, argument );

    if ( arg1[0] == '\0' || arg2[0] == '\0' || arg3[0] == '0' )
    {
    	send_to_char("You can change the following fields...\n\r", ch);
	send_to_char("chwear   = Message to owner when item is worn.\n\r",ch);
	send_to_char("chrem    = Message to owner when item is removed.\n\r",ch);
	send_to_char("chuse    = Message to owner when item is used.\n\r",ch);
	send_to_char("victwear = Message to others in room when item is worn.\n\r",ch);
	send_to_char("victrem  = Message to others in room when item is removed.\n\r",ch);
	send_to_char("victuse  = Message to others in room when item is used.\n\r",ch);
	send_to_char("type       activate/twist/press/pull\n\r",ch);
	send_to_char("           target       = Item can target people (for spell, etc).\n\r",ch);
	send_to_char("           transporter  = Owner can transport freely between two locations.\n\r",ch);
	send_to_char("           teleporter   = Owner can transport to target location at will.\n\r",ch);
	send_to_char("           spell        = Item can cast spells.\n\r",ch);
	send_to_char("           object       = Owner can create the specified object.\n\r",ch);
	send_to_char("           mobile       = Owner can create the specified mobile.\n\r",ch);
        send_to_char("           unique       = Makes it a unique item.\n\r",ch);
	send_to_char("           action       = Target must perform an action.\n\r",ch);
	send_to_char("           delay1/delay2= Sets a half/whole round delay on action.\n\r",ch);
	send_to_char("power      <value>      = Spell number/transporter room number.\n\r",ch);
	send_to_char("extra	 vampire      = Vampires only.\n\r", ch);
	send_to_char("		 highlander   = Highlanders only & boosts hit/dam/ac.\n\r", ch);
	send_to_char("		 werewolf     = Werewolf only.\n\r", ch);
	send_to_char("		 wolfweapon   = Wieldable by raging werewolf.\n\r", ch);
	send_to_char("		 mage	      = Mage only.\n\r", ch);
	send_to_char("		 demon	      = Demon only.\n\r", ch);
	send_to_char("		 demonsteel   = Demonsteel & demon only.\n\r", ch);
	return;
    }
    if ( ( obj = get_obj_carry( ch, arg1) ) == NULL )
    {
	send_to_char("You are not carrying that item.\n\r",ch);
	return;
    }
    value = is_number( arg3 ) ? atoi( arg3 ) : -1;
    if (!str_cmp(arg2,"chwear") )
    {
	if (obj->chpoweron != NULL) strcpy(buf,obj->chpoweron);
	if (!str_cmp(arg3,"clear"))
	{
	    free_string( obj->chpoweron );
	    obj->chpoweron = str_dup( "(null)" );
	}
	else if (obj->chpoweron != NULL && buf[0] != '\0' && str_cmp(buf,"(null)"))
	{
	    if (strlen(buf)+strlen(arg3) >= MAX_STRING_LENGTH-4)
	    {
		send_to_char("Line too long.\n\r",ch);
		return;
	    }
	    else
	    {
	    	free_string( obj->chpoweron );
	    	strcat( buf, "\n\r" );
	    	strcat( buf, arg3 );
	    	obj->chpoweron = str_dup( buf );
	    }
	}
	else
	{
	    free_string( obj->chpoweron );
	    obj->chpoweron = str_dup( arg3 );
	}
    }
    else if (!str_cmp(arg2,"chrem") )
    {
	if (obj->chpoweroff != NULL) strcpy(buf,obj->chpoweroff);
	if (!str_cmp(arg3,"clear"))
	{
	    free_string( obj->chpoweroff );
	    obj->chpoweroff = str_dup( "(null)" );
	}
	else if (obj->chpoweroff != NULL && buf[0] != '\0' && str_cmp(buf,"(null)"))
	{
	    if (strlen(buf)+strlen(arg3) >= MAX_STRING_LENGTH-4)
	    {
		send_to_char("Line too long.\n\r",ch);
		return;
	    }
	    else
	    {
	    	free_string( obj->chpoweroff );
	    	strcat( buf, "\n\r" );
	    	strcat( buf, arg3 );
	    	obj->chpoweroff = str_dup( buf );
	    }
	}
	else
	{
	    free_string( obj->chpoweroff );
	    obj->chpoweroff = str_dup( arg3 );
	}
    }
    else if (!str_cmp(arg2,"chuse") )
    {
	if (obj->chpoweruse != NULL) strcpy(buf,obj->chpoweruse);
	if (!str_cmp(arg3,"clear"))
	{
	    free_string( obj->chpoweruse );
	    obj->chpoweruse = str_dup( "(null)" );
	}
	else if (obj->chpoweruse != NULL && buf[0] != '\0' && str_cmp(buf,"(null)"))
	{
	    if (strlen(buf)+strlen(arg3) >= MAX_STRING_LENGTH-4)
	    {
		send_to_char("Line too long.\n\r",ch);
		return;
	    }
	    else
	    {
	    	free_string( obj->chpoweruse );
	    	strcat( buf, "\n\r" );
	    	strcat( buf, arg3 );
	    	obj->chpoweruse = str_dup( buf );
	    }
	}
	else
	{
	    free_string( obj->chpoweruse );
	    obj->chpoweruse = str_dup( arg3 );
	}
    }
    else if (!str_cmp(arg2,"victwear") )
    {
	if (obj->victpoweron != NULL) strcpy(buf,obj->victpoweron);
	if (!str_cmp(arg3,"clear"))
	{
	    free_string( obj->victpoweron );
	    obj->victpoweron = str_dup( "(null)" );
	}
	else if (obj->victpoweron != NULL && buf[0] != '\0' && str_cmp(buf,"(null)"))
	{
	    if (strlen(buf)+strlen(arg3) >= MAX_STRING_LENGTH-4)
	    {
		send_to_char("Line too long.\n\r",ch);
		return;
	    }
	    else
	    {
	    	free_string( obj->victpoweron );
	    	strcat( buf, "\n\r" );
	    	strcat( buf, arg3 );
	    	obj->victpoweron = str_dup( buf );
	    }
	}
	else
	{
	    free_string( obj->victpoweron );
	    obj->victpoweron = str_dup( arg3 );
	}
    }
    else if (!str_cmp(arg2,"victrem") )
    {
	if (obj->victpoweroff != NULL) strcpy(buf,obj->victpoweroff);
	if (!str_cmp(arg3,"clear"))
	{
	    free_string( obj->victpoweroff );
	    obj->victpoweroff = str_dup( "(null)" );
	}
	else if (obj->victpoweroff != NULL && buf[0] != '\0' && str_cmp(buf,"(null)"))
	{
	    if (strlen(buf)+strlen(arg3) >= MAX_STRING_LENGTH-4)
	    {
		send_to_char("Line too long.\n\r",ch);
		return;
	    }
	    else
	    {
	    	free_string( obj->victpoweroff );
	    	strcat( buf, "\n\r" );
	    	strcat( buf, arg3 );
	    	obj->victpoweroff = str_dup( buf );
	    }
	}
	else
	{
	    free_string( obj->victpoweroff );
	    obj->victpoweroff = str_dup( arg3 );
	}
    }
    else if (!str_cmp(arg2,"victuse") )
    {
	if (obj->victpoweruse != NULL) strcpy(buf,obj->victpoweruse);
	if (!str_cmp(arg3,"clear"))
	{
	    free_string( obj->victpoweruse );
	    obj->victpoweruse = str_dup( "(null)" );
	}
	else if (obj->victpoweruse != NULL && buf[0] != '\0' && str_cmp(buf,"(null)"))
	{
	    if (strlen(buf)+strlen(arg3) >= MAX_STRING_LENGTH-4)
	    {
		send_to_char("Line too long.\n\r",ch);
		return;
	    }
	    else
	    {
	    	free_string( obj->victpoweruse );
	    	strcat( buf, "\n\r" );
	    	strcat( buf, arg3 );
	    	obj->victpoweruse = str_dup( buf );
	    }
	}
	else
	{
	    free_string( obj->victpoweruse );
	    obj->victpoweruse = str_dup( arg3 );
	}
    }
    else if ( !str_cmp( arg2, "type" ) )
    {
	if ( !str_cmp( arg3, "activate" ) && IS_SET(obj->spectype, SITEM_ACTIVATE) )
	    REMOVE_BIT(obj->spectype, SITEM_ACTIVATE);
	else if ( !str_cmp( arg3, "activate" ) && !IS_SET(obj->spectype, SITEM_ACTIVATE) )
	    SET_BIT(obj->spectype, SITEM_ACTIVATE);
	else if ( !str_cmp( arg3, "twist" ) && IS_SET(obj->spectype, SITEM_TWIST) )
	    REMOVE_BIT(obj->spectype, SITEM_TWIST);
	else if ( !str_cmp( arg3, "twist" ) && !IS_SET(obj->spectype, SITEM_TWIST) )
	    SET_BIT(obj->spectype, SITEM_TWIST);
        else if ( !str_cmp( arg3, "unique" ) && !IS_SET(obj->quest, QUEST_UNIQUE) )
	    SET_BIT(obj->quest, QUEST_UNIQUE);
        else if ( !str_cmp( arg3, "unique" ) && IS_SET(obj->quest, QUEST_UNIQUE) )
	    REMOVE_BIT(obj->quest, QUEST_UNIQUE);
	else if ( !str_cmp( arg3, "press" ) && IS_SET(obj->spectype, SITEM_PRESS) )
	    REMOVE_BIT(obj->spectype, SITEM_PRESS);
	else if ( !str_cmp( arg3, "press" ) && !IS_SET(obj->spectype, SITEM_PRESS) )
	    SET_BIT(obj->spectype, SITEM_PRESS);
	else if ( !str_cmp( arg3, "pull" ) && IS_SET(obj->spectype, SITEM_PULL) )
	    REMOVE_BIT(obj->spectype, SITEM_PULL);
	else if ( !str_cmp( arg3, "pull" ) && !IS_SET(obj->spectype, SITEM_PULL) )
	    SET_BIT(obj->spectype, SITEM_PULL);
	else if ( !str_cmp( arg3, "target" ) && IS_SET(obj->spectype, SITEM_TARGET) )
	    REMOVE_BIT(obj->spectype, SITEM_TARGET);
	else if ( !str_cmp( arg3, "target" ) && !IS_SET(obj->spectype, SITEM_TARGET) )
	    SET_BIT(obj->spectype, SITEM_TARGET);
	else if ( !str_cmp( arg3, "spell" ) && IS_SET(obj->spectype, SITEM_SPELL) )
	    REMOVE_BIT(obj->spectype, SITEM_SPELL);
	else if ( !str_cmp( arg3, "spell" ) && !IS_SET(obj->spectype, SITEM_SPELL) )
	    SET_BIT(obj->spectype, SITEM_SPELL);
	else if ( !str_cmp( arg3, "transporter" ) && IS_SET(obj->spectype, SITEM_TRANSPORTER) )
	    REMOVE_BIT(obj->spectype, SITEM_TRANSPORTER);
	else if ( !str_cmp( arg3, "transporter" ) && !IS_SET(obj->spectype, SITEM_TRANSPORTER) )
	    SET_BIT(obj->spectype, SITEM_TRANSPORTER);
	else if ( !str_cmp( arg3, "teleporter" ) && IS_SET(obj->spectype, SITEM_TELEPORTER) )
	    REMOVE_BIT(obj->spectype, SITEM_TELEPORTER);
	else if ( !str_cmp( arg3, "teleporter" ) && !IS_SET(obj->spectype, SITEM_TELEPORTER) )
	    SET_BIT(obj->spectype, SITEM_TELEPORTER);
	else if ( !str_cmp( arg3, "object" ) && IS_SET(obj->spectype, SITEM_OBJECT) )
	    REMOVE_BIT(obj->spectype, SITEM_OBJECT);
	else if ( !str_cmp( arg3, "object" ) && !IS_SET(obj->spectype, SITEM_OBJECT) )
	    SET_BIT(obj->spectype, SITEM_OBJECT);
	else if ( !str_cmp( arg3, "mobile" ) && IS_SET(obj->spectype, SITEM_MOBILE) )
	    REMOVE_BIT(obj->spectype, SITEM_MOBILE);
	else if ( !str_cmp( arg3, "mobile" ) && !IS_SET(obj->spectype, SITEM_MOBILE) )
	    SET_BIT(obj->spectype, SITEM_MOBILE);
	else if ( !str_cmp( arg3, "action" ) && IS_SET(obj->spectype, SITEM_ACTION) )
	    REMOVE_BIT(obj->spectype, SITEM_ACTION);
	else if ( !str_cmp( arg3, "action" ) && !IS_SET(obj->spectype, SITEM_ACTION) )
	    SET_BIT(obj->spectype, SITEM_ACTION);
	else if ( !str_cmp( arg3, "delay1" ) && IS_SET(obj->spectype, SITEM_DELAY1) )
	    REMOVE_BIT(obj->spectype, SITEM_DELAY1);
	else if ( !str_cmp( arg3, "delay1" ) && !IS_SET(obj->spectype, SITEM_DELAY1) )
	    SET_BIT(obj->spectype, SITEM_DELAY1);
	else if ( !str_cmp( arg3, "delay2" ) && IS_SET(obj->spectype, SITEM_DELAY2) )
	    REMOVE_BIT(obj->spectype, SITEM_DELAY2);
	else if ( !str_cmp( arg3, "delay2" ) && !IS_SET(obj->spectype, SITEM_DELAY2) )
	    SET_BIT(obj->spectype, SITEM_DELAY2);
	else
	{
	    send_to_char("No such flag to set.\n\r",ch);
	    return;
	}
	sprintf(buf,"%s flag toggled.\n\r",capitalize(arg3));
	send_to_char(buf,ch);
	return;
    }
    else if ( !str_cmp( arg2, "power" ) )
	obj->specpower = value;
    else if ( !str_cmp( arg2, "extra" ) )
    {
	if ( !str_cmp( arg3, "vampire" ) && IS_SET(obj->spectype, SITEM_VAMPIRE) )
	    REMOVE_BIT(obj->spectype, SITEM_VAMPIRE);
	else if ( !str_cmp( arg3, "vampire" ) && !IS_SET(obj->spectype, SITEM_VAMPIRE) )
	    SET_BIT(obj->spectype, SITEM_VAMPIRE);
	else if ( !str_cmp( arg3, "demon" ) && IS_SET(obj->spectype, SITEM_DEMON) )
	    REMOVE_BIT(obj->spectype, SITEM_DEMON);
	else if ( !str_cmp( arg3, "demon" ) && !IS_SET(obj->spectype, SITEM_DEMON) )
	    SET_BIT(obj->spectype, SITEM_DEMON);
	else if ( !str_cmp( arg3, "demonsteel" ) && IS_SET(obj->spectype, SITEM_DEMONIC) )
	    REMOVE_BIT(obj->spectype, SITEM_DEMONIC);
	else if ( !str_cmp( arg3, "demonsteel" ) && !IS_SET(obj->spectype, SITEM_DEMONIC) )
	    SET_BIT(obj->spectype, SITEM_DEMONIC);
	else if ( !str_cmp( arg3, "highlander" ) && IS_SET(obj->spectype, SITEM_HIGHLANDER) )
	    REMOVE_BIT(obj->spectype, SITEM_HIGHLANDER);
	else if ( !str_cmp( arg3, "highlander" ) && !IS_SET(obj->spectype, SITEM_HIGHLANDER) )
	    SET_BIT(obj->spectype, SITEM_HIGHLANDER);
	else if ( !str_cmp( arg3, "werewolf" ) && IS_SET(obj->spectype, SITEM_WEREWOLF) )
	    REMOVE_BIT(obj->spectype, SITEM_WEREWOLF);
	else if ( !str_cmp( arg3, "werewolf" ) && !IS_SET(obj->spectype, SITEM_WEREWOLF) )
	    SET_BIT(obj->spectype, SITEM_WEREWOLF);
	else if ( !str_cmp( arg3, "wolfweapon" ) && IS_SET(obj->spectype, SITEM_WOLFWEAPON) )
	    REMOVE_BIT(obj->spectype, SITEM_WOLFWEAPON);
	else if ( !str_cmp( arg3, "wolfweapon" ) && !IS_SET(obj->spectype, SITEM_WOLFWEAPON) )
	    SET_BIT(obj->spectype, SITEM_WOLFWEAPON);
	else if ( !str_cmp( arg3, "silver" ) && IS_SET(obj->spectype, SITEM_SILVER))
	    REMOVE_BIT(obj->spectype, SITEM_SILVER);
	else if ( !str_cmp( arg3, "silver" ) && !IS_SET(obj->spectype, SITEM_SILVER))
	    SET_BIT(obj->spectype, SITEM_SILVER);
	else if ( !str_cmp( arg3, "mage" ) && IS_SET(obj->spectype, SITEM_MAGE) )
	    REMOVE_BIT(obj->spectype, SITEM_MAGE);
	else if ( !str_cmp( arg3, "mage" ) && !IS_SET(obj->spectype, SITEM_MAGE) )
	    SET_BIT(obj->spectype, SITEM_MAGE);
	else 
	{
	    send_to_char("No such extra flag.\n\r", ch);
	    return;
	}
	sprintf(buf,"%s extra flag toggled.\n\r", capitalize(arg3));
	send_to_char(buf, ch);
	return;
    }

    else
    {
	send_to_char("No such flag.\n\r",ch);
	return;
    }

    send_to_char("Ok.\n\r",ch);
    return;
}


void do_oclone( CHAR_DATA *ch, char *argument )
{
    char arg1[MAX_INPUT_LENGTH];
    OBJ_INDEX_DATA *pObjIndex;
    OBJ_DATA *obj;
    OBJ_DATA *obj2;
    AFFECT_DATA *paf;
    AFFECT_DATA *paf2;

    argument = one_argument( argument, arg1 );
 
    if ( arg1[0] == '\0' )
    {
        send_to_char( "Make a clone of what object?\n\r", ch );
        return;
    }
 
    if ( ( obj = get_obj_world( ch, arg1 ) ) == NULL )
    {
	send_to_char( "Nothing like that in hell, earth, or heaven.\n\r", ch );
	return;
    }

    if (!IS_JUDGE(ch) && (obj->questmaker == NULL || str_cmp(ch->name,obj->questmaker) || strlen(obj->questmaker) < 2))
    {
	send_to_char("You can only clone your own creations.\n\r", ch);
	return;
    }

    pObjIndex = get_obj_index( obj->pIndexData->vnum);
    obj2 = create_object( pObjIndex, obj->level );
    /* Copy any changed parts of the object. */
    free_string(obj2->name);
    obj2->name=str_dup(obj->name);
    free_string(obj2->short_descr);
    obj2->short_descr=str_dup(obj->short_descr);
    free_string(obj2->description);
    obj2->description=str_dup(obj->description);

    if (obj->questmaker != NULL && strlen(obj->questmaker) > 1)
    {
	free_string(obj2->questmaker);
	obj2->questmaker=str_dup(obj->questmaker);
    }

    if (obj->chpoweron != NULL)
    {
    	free_string(obj2->chpoweron);
    	obj2->chpoweron=str_dup(obj->chpoweron);
    }
    if (obj->chpoweroff != NULL)
    {
    	free_string(obj2->chpoweroff);
    	obj2->chpoweroff=str_dup(obj->chpoweroff);
    }
    if (obj->chpoweruse != NULL)
    {
    	free_string(obj2->chpoweruse);
    	obj2->chpoweruse=str_dup(obj->chpoweruse);
    }
    if (obj->victpoweron != NULL)
    {
    	free_string(obj2->victpoweron);
    	obj2->victpoweron=str_dup(obj->victpoweron);
    }
    if (obj->victpoweroff != NULL)
    {
    	free_string(obj2->victpoweroff);
    	obj2->victpoweroff=str_dup(obj->victpoweroff);
    }
    if (obj->victpoweruse != NULL)
    {
    	free_string(obj2->victpoweruse);
    	obj2->victpoweruse=str_dup(obj->victpoweruse);
    }
    obj2->item_type 	= obj->item_type;
    obj2->extra_flags 	= obj->extra_flags;
    obj2->wear_flags 	= obj->wear_flags;
    obj2->weight 	= obj->weight;
    obj2->spectype 	= obj->spectype;
    obj2->specpower 	= obj->specpower;
    obj2->condition 	= obj->condition;
    obj2->toughness 	= obj->toughness;
    obj2->resistance 	= obj->resistance;
    obj2->quest 	= obj->quest;
    obj2->points 	= obj->points;
    obj2->cost 		= obj->cost;
    obj2->value[0] 	= obj->value[0];
    obj2->value[1] 	= obj->value[1];
    obj2->value[2] 	= obj->value[2];
    obj2->value[3] 	= obj->value[3];
    /*****************************************/
    obj_to_char(obj2,ch);

    if (obj->affected != NULL)
    {
    	for ( paf = obj->affected; paf != NULL; paf = paf->next )
    	{
	    if (affect_free == NULL)
	        paf2 = alloc_perm( sizeof(*paf) );
	    else
	    {
		paf2 = affect_free;
		affect_free = affect_free->next;
	    }
	    paf2->type  	= 0;
	    paf2->duration	= paf->duration;
	    paf2->location	= paf->location;
	    paf2->modifier	= paf->modifier;
	    paf2->bitvector	= 0;
	    paf2->next  	= obj2->affected;
	    obj2->affected	= paf2;
    	}
    }

    act( "You create a clone of $p.", ch, obj, NULL, TO_CHAR );
    return;
}

void do_evileye( CHAR_DATA *ch, char *argument )
{
    char buf[MAX_STRING_LENGTH];
    char arg1[MAX_INPUT_LENGTH];
    char arg2[MAX_INPUT_LENGTH];
 
    smash_tilde( argument );
    argument = one_argument( argument, arg1 );
    strcpy( arg2, argument );
 
    if (IS_NPC(ch) || !IS_VAMPAFF(ch,VAM_DOMINATE)) {
	send_to_char("Huh?\n\r",ch);return;}
    if ( arg1[0] == '\0' || arg2[0] == '\0' )
    {
        send_to_char( "Format is: evileye <option> <value>\n\r", ch );
        send_to_char( "Option ACTION is a text string action performed by you or the viewer.\n\r", ch );
        send_to_char( "Option MESSAGE is a text string shown to the person looking at you.\n\r", ch );
        send_to_char( "Option TOGGLE has values: spell, self, other.\n\r\n\r", ch );
	if ( ch->poweraction != NULL) {
	    sprintf(buf,"Current action: %s.\n\r",ch->poweraction);
	    send_to_char(buf,ch);}
	if ( ch->powertype != NULL) {
	    sprintf(buf,"Current message: %s.\n\r",ch->powertype);
	    send_to_char(buf,ch);}
	send_to_char("Current flags:",ch);
	if (IS_SET(ch->spectype,EYE_SPELL)) send_to_char(" Spell",ch);
	if (IS_SET(ch->spectype,EYE_SELFACTION)) send_to_char(" Self",ch);
	if (IS_SET(ch->spectype,EYE_ACTION)) send_to_char(" Other",ch);
	if (!IS_SET(ch->spectype,EYE_SPELL) &&
	    !IS_SET(ch->spectype,EYE_SELFACTION) &&
	    !IS_SET(ch->spectype,EYE_ACTION)) send_to_char(" None",ch);
	send_to_char(".\n\r",ch);
        return;
    }
    if ( !str_cmp( arg1, "action" ) )
    {
	free_string( ch->poweraction );
	ch->poweraction = str_dup( arg2 );
	return;
    }
    else if ( !str_cmp( arg1, "message" ) )
    {
	free_string( ch->powertype );
	ch->powertype = str_dup( arg2 );
	return;
    }
    else if ( !str_cmp( arg1, "toggle" ) )
    {
	if ( !str_cmp( arg2, "spell" ) && IS_SET(ch->spectype,EYE_SPELL))
	    REMOVE_BIT(ch->spectype,EYE_SPELL);
	else if ( !str_cmp( arg2, "spell" ) && !IS_SET(ch->spectype,EYE_SPELL))
	    SET_BIT(ch->spectype,EYE_SPELL);
	else if ( !str_cmp( arg2, "self" ) && IS_SET(ch->spectype,EYE_SELFACTION))
	    REMOVE_BIT(ch->spectype,EYE_SELFACTION);
	else if ( !str_cmp( arg2, "self" ) && !IS_SET(ch->spectype,EYE_SELFACTION))
	    SET_BIT(ch->spectype,EYE_SELFACTION);
	else if ( !str_cmp( arg2, "other" ) && IS_SET(ch->spectype,EYE_ACTION))
	    REMOVE_BIT(ch->spectype,EYE_ACTION);
	else if ( !str_cmp( arg2, "other" ) && !IS_SET(ch->spectype,EYE_ACTION))
	    SET_BIT(ch->spectype,EYE_ACTION);
	else {
	    send_to_char("TOGGLE flag should be one of: spell, self, other.\n\r",ch);
	    return;}
	sprintf(buf,"%s flag toggled.\n\r",capitalize(arg2));
	send_to_char(buf,ch);
	return;
    }
    else
    {
        send_to_char( "Format is: evileye <option> <value>\n\r", ch );
        send_to_char( "Option ACTION is a text string action performed by you or the viewer.\n\r", ch );
        send_to_char( "Option MESSAGE is a text string shown to the person looking at you.\n\r", ch );
        send_to_char( "Option TOGGLE has values: spell, self, other.\n\r\n\r", ch );
	if ( ch->poweraction != NULL) {
	    sprintf(buf,"Current action: %s.\n\r",ch->poweraction);
	    send_to_char(buf,ch);}
	if ( ch->powertype != NULL) {
	    sprintf(buf,"Current message: %s.\n\r",ch->powertype);
	    send_to_char(buf,ch);}
	send_to_char("Current flags:",ch);
	if (IS_SET(ch->spectype,EYE_SPELL)) send_to_char(" Spell",ch);
	if (IS_SET(ch->spectype,EYE_SELFACTION)) send_to_char(" Self",ch);
	if (IS_SET(ch->spectype,EYE_ACTION)) send_to_char(" Other",ch);
	if (!IS_SET(ch->spectype,EYE_SPELL) &&
	    !IS_SET(ch->spectype,EYE_SELFACTION) &&
	    !IS_SET(ch->spectype,EYE_ACTION)) send_to_char(" None",ch);
            send_to_char(".\n\r",ch);

    }
    return;
}

void do_clearvam( CHAR_DATA *ch, char *argument )
{
    if ( ch->trust < 8 )
    	send_to_char( "Huh?\n\r", ch );
    else
    	send_to_char( "If you want to CLEARVAMP, spell it out.\n\r", ch );
    return;
}


void do_clearvamp( CHAR_DATA *ch, char *argument )
{
    CHAR_DATA *victim;
    char arg[MAX_INPUT_LENGTH];

    one_argument( argument, arg );

    if ( arg[0] == '\0' || is_number(arg) )
    {
	send_to_char( "Clear who's clan?\n\r", ch );
	return;
    }

    if ( ( victim = get_char_world( ch, arg ) ) == NULL )
    {
	send_to_char( "They aren't here.\n\r", ch );
	return;
    }

    if ( IS_NPC(victim) )
    {
	send_to_char( "Not on NPC's.\n\r", ch );
	return;
    }

    if (IS_SET(victim->act, PLR_GODLESS) && get_trust(ch) < NO_GODLESS
        && !IS_SET(ch->extra , EXTRA_ANTI_GODLESS))
    {
	send_to_char( "You failed.\n\r", ch );
	return;
    }

    if (IS_CLASS(victim,CLASS_VAMPIRE)) do_mortalvamp(victim,"");
    free_string(victim->lord);
    victim->lord     = str_dup( "" );
    free_string(victim->clan);
    victim->clan     = str_dup( "" );
    victim->pcdata->stats[UNI_GEN]     = 0;
    victim->pcdata->stats[UNI_AFF]     = 0;
    victim->pcdata->stats[UNI_CURRENT] = -1;

    REMOVE_BIT(victim->special, SPC_SIRE);
    REMOVE_BIT(victim->special, SPC_PRINCE);
    REMOVE_BIT(victim->special, SPC_ANARCH);
    victim->pcdata->stats[UNI_RAGE] = 0;
    victim->pcdata->rank = AGE_CHILDE;

    send_to_char( "Ok.\n\r", ch );
    return;
}

void do_artifact( CHAR_DATA *ch, char *argument )
{
    char buf[MAX_STRING_LENGTH];
    OBJ_DATA *obj;
    OBJ_DATA *in_obj;
    bool found;

    if (IS_NPC(ch)) {send_to_char("Not while switched.\n\r",ch); return;}

    found = FALSE;
    for ( obj = object_list; obj != NULL; obj = obj->next )
    {
	if ( !IS_SET(obj->quest, QUEST_ARTIFACT) ) continue;

	found = TRUE;

	for ( in_obj = obj; in_obj->in_obj != NULL; in_obj = in_obj->in_obj )
	    ;

	if ( in_obj->carried_by != NULL )
	{
	    sprintf( buf, "%s created by %s and carried by %s.\n\r",
		obj->short_descr, (obj->questmaker && strlen(obj->questmaker) > 1) 
		? obj->questmaker : "the Ancients",
		PERS(in_obj->carried_by, ch) );
	}
	else
	{
	    sprintf( buf, "%s created by %s and in %s.\n\r",
		obj->short_descr, (obj->questmaker && strlen(obj->questmaker) > 1) 
		? obj->questmaker : "the Ancients",
		in_obj->in_room == NULL ? "somewhere" : in_obj->in_room->name );
	}

	buf[0] = UPPER(buf[0]);
	send_to_char( buf, ch );
    }

    if ( !found )
	send_to_char( "There are no artifacts in the game.\n\r", ch );

    return;
}

void do_unique( CHAR_DATA *ch, char *argument )
{
    char buf[MAX_STRING_LENGTH];
    OBJ_DATA *obj;
    OBJ_DATA *in_obj;
    bool found;

    if (IS_NPC(ch)) {send_to_char("Not while switched.\n\r",ch); return;}

    found = FALSE;
    for ( obj = object_list; obj != NULL; obj = obj->next )
    {
	if ( !IS_SET(obj->quest, QUEST_UNIQUE) ) continue;

	found = TRUE;

	for ( in_obj = obj; in_obj->in_obj != NULL; in_obj = in_obj->in_obj )
	    ;

	if ( in_obj->carried_by != NULL )
	{
	    sprintf( buf, "%s created by %s and carried by %s.\n\r",
		obj->short_descr, obj->questmaker, 
		PERS(in_obj->carried_by, ch) );
	}
	else
	{
	    sprintf( buf, "%s created by %s and in %s.\n\r",
		obj->short_descr, obj->questmaker,
		in_obj->in_room == NULL ? "somewhere" : in_obj->in_room->name );
	}

	buf[0] = UPPER(buf[0]);
	send_to_char( buf, ch );
    }

    if ( !found )
	send_to_char( "There are no unique's in the game.\n\r", ch );

    return;
}

void do_locate( CHAR_DATA *ch, char *argument )
{
    char buf[MAX_STRING_LENGTH];
    OBJ_DATA *obj;
    OBJ_DATA *in_obj;
    bool found;

    if (IS_NPC(ch)) {send_to_char("Not while switched.\n\r",ch); return;}

    found = FALSE;
    for ( obj = object_list; obj != NULL; obj = obj->next )
    {
	if ( !can_see_obj( ch, obj ) || obj->questowner == NULL || 
	    strlen(obj->questowner) < 2 || str_cmp( ch->name, obj->questowner ))
	    continue;

	found = TRUE;

	for ( in_obj = obj; in_obj->in_obj != NULL; in_obj = in_obj->in_obj )
	    ;

	if ( in_obj->carried_by != NULL )
	{
	    sprintf( buf, "%s carried by %s.\n\r",
		obj->short_descr, PERS(in_obj->carried_by, ch) );
	}
	else
	{
	    sprintf( buf, "%s in %s.\n\r",
		obj->short_descr, in_obj->in_room == NULL
		    ? "somewhere" : in_obj->in_room->name );
	}

	buf[0] = UPPER(buf[0]);
	send_to_char( buf, ch );
    }

    if ( !found )
	send_to_char( "You cannot locate any items belonging to you.\n\r", ch );

    return;
}

void do_claim( CHAR_DATA *ch, char *argument )
{
    char arg[MAX_INPUT_LENGTH];
    OBJ_DATA *obj;

    one_argument( argument, arg );

    if (IS_NPC(ch)) {send_to_char("Not while switched.\n\r",ch); return;}

    if ( ch->exp < 10000 ) {send_to_char("It costs 10000 exp to claim ownership of an item.\n\r",ch); return;}

    if ( arg[0] == '\0' )
    {
	send_to_char( "What object do you wish to claim ownership of?\n\r", ch );
	return;
    }

    if ( ( obj = get_obj_carry(ch, arg) ) == NULL )
	{send_to_char("You are not carrying that item.\n\r",ch); return;}

    if (obj->item_type == ITEM_QUEST || obj->item_type == ITEM_AMMO ||
        obj->item_type == ITEM_EGG   || obj->item_type == ITEM_VOODOO ||
        obj->item_type == ITEM_MONEY || obj->item_type == ITEM_TREASURE ||
        obj->item_type == ITEM_PAGE  || IS_SET(obj->quest, QUEST_ARTIFACT) ||
	IS_SET(obj->quest, QUEST_UNIQUE) || obj->item_type == ITEM_CORPSE_NPC)
    {
	send_to_char( "You cannot claim that item.\n\r", ch );
	return;
    }
    else if (obj->chobj != NULL && !IS_NPC(obj->chobj) &&
	obj->chobj->pcdata->obj_vnum != 0)
    {
	send_to_char( "You cannot claim that item.\n\r", ch );
	return;
    }

    if ( obj->questowner != NULL && strlen(obj->questowner) > 1 )
    {
	if (!str_cmp(ch->name,obj->questowner))
	    send_to_char("But you already own it!\n\r",ch);
	else
	    send_to_char("Someone else has already claimed ownership to it.\n\r",ch);
	return;
    }

    ch->exp -= 10000;
    if (obj->questowner != NULL) free_string(obj->questowner);
    obj->questowner = str_dup(ch->name);
    act("You are now the owner of $p.",ch,obj,NULL,TO_CHAR);
    act("$n is now the owner of $p.",ch,obj,NULL,TO_ROOM);
    return;
}
void do_reclaim( CHAR_DATA *ch, char *argument )
{
    char arg[MAX_INPUT_LENGTH];
    char buf[MAX_INPUT_LENGTH];
    char logstring[MAX_STRING_LENGTH];
    OBJ_DATA *obj;
    int cost;

    one_argument( argument, arg );

    if (ch->pcdata->legend <=6)
    {
        send_to_char("This Legend Command requires Oracle Legend Status.\n\r",ch);
        return;
    }


    if (IS_NPC(ch)) {send_to_char("Not while switched.\n\r",ch); return;}

    if ( ch->exp < 1000000 ) {send_to_char("It costs 1000000 exp to reclaim ownership of an item.\n\r",ch); return;}

    if ( arg[0] == '\0' )
    {
	send_to_char( "What object do you wish to reclaim ownership of?\n\r", ch );
	return;
    }

    if ( ( obj = get_obj_carry(ch, arg) ) == NULL )
	{send_to_char("You are not carrying that item.\n\r",ch); return;}

    cost = obj->points * 0.75;

    if (cost < 250) cost = 250;

    if (ch->pcdata->quest < cost)
    {
        sprintf( buf, "That item requires %dqp to reclaim.", cost);
        act( buf, ch, obj, NULL, TO_CHAR );
	return;
    }	

    if (obj->item_type == ITEM_QUEST || obj->item_type == ITEM_AMMO ||
        obj->item_type == ITEM_EGG   || obj->item_type == ITEM_VOODOO ||
        obj->item_type == ITEM_MONEY || obj->item_type == ITEM_TREASURE ||
        obj->item_type == ITEM_PAGE  || IS_SET(obj->quest, QUEST_ARTIFACT) ||
	IS_SET(obj->quest, QUEST_UNIQUE) || obj->item_type == ITEM_CORPSE_NPC ||
	IS_SET(obj->spectype, SITEM_HIGHLANDER) )
    {
	send_to_char( "You cannot reclaim that item.\n\r", ch );
	return;
    }
    else if (obj->chobj != NULL && !IS_NPC(obj->chobj) &&
	obj->chobj->pcdata->obj_vnum != 0)
    {
	send_to_char( "You cannot reclaim that item.\n\r", ch );
	return;
    }

    if (!str_cmp(obj->questowner,"Coal") || !str_cmp(obj->questowner,"Puck") || !str_cmp(obj->questowner,"Coal"))
    {
	send_to_char("You cannot reclaim something that is owned by a god!\n\r",ch );
	return;
    }

    if ( obj->questowner == NULL)
    {
	send_to_char("This item isnt claimed!\n\r",ch);
	return;
    }

    if ( obj->questowner != NULL && strlen(obj->questowner) > 1 )
    {
	if (!str_cmp(ch->name,obj->questowner))
	{
	    send_to_char("But you already own it!\n\r",ch);
	    return;
	}
    }

    sprintf(logstring, "%s reclaims %s's %s.", ch->name, obj->questowner, obj->short_descr);
    ch->exp -= 1000000;
    ch->pcdata->quest -= cost;
    obj->points = UMIN(obj->points, cost);
    if (obj->questowner != NULL) free_string(obj->questowner);
    obj->questowner = str_dup(ch->name);
    act("You are now the new owner of $p.",ch,obj,NULL,TO_CHAR);
    act("$n is now the new owner of $p.",ch,obj,NULL,TO_ROOM);
    return;
}

void do_gift( CHAR_DATA *ch, char *argument )
{
    char arg1[MAX_INPUT_LENGTH];
    char arg2[MAX_INPUT_LENGTH];
    OBJ_DATA *obj;
    CHAR_DATA *victim;

    argument = one_argument( argument, arg1 );
    argument = one_argument( argument, arg2 );

    if (IS_NPC(ch)) {send_to_char("Not while switched.\n\r",ch); return;}

    if ( ch->exp < 500 ) {send_to_char("It costs 500 exp to make a gift of an item.\n\r",ch); return;}

    if ( arg1[0] == '\0' || arg2[0] == '\0' )
    {
	send_to_char( "Make a gift of which object to whom?\n\r", ch );
	return;
    }

    if ( ( obj = get_obj_carry(ch, arg1) ) == NULL )
	{send_to_char("You are not carrying that item.\n\r",ch); return;}
    if ( ( victim = get_char_room( ch, arg2 ) ) == NULL )
	{send_to_char("Nobody here by that name.\n\r",ch); return;}

    if ( IS_NPC(victim) )
    {
	send_to_char("Not on NPC's.\n\r",ch);
	return;
    }
    if ( obj->questowner == NULL || strlen(obj->questowner) < 2 )
    {
	send_to_char("That item has not yet been claimed.\n\r",ch);
	return;
    }
    if (str_cmp(ch->name,obj->questowner))
    {
	send_to_char("But you don't own it!\n\r",ch);
	return;
    }
    ch->exp -= 500;
    if (obj->questowner != NULL) free_string(obj->questowner);
    obj->questowner = str_dup(victim->name);
    act("You grant ownership of $p to $N.",ch,obj,victim,TO_CHAR);
    act("$n grants ownership of $p to $N.",ch,obj,victim,TO_NOTVICT);
    act("$n grants ownership of $p to you.",ch,obj,victim,TO_VICT);
    return;
}

void do_create( CHAR_DATA *ch, char *argument )
{
    char arg1[MAX_INPUT_LENGTH];
    char arg2[MAX_INPUT_LENGTH];
    OBJ_INDEX_DATA *pObjIndex;
    OBJ_DATA *obj;
    int itemtype = 13;
    int level;

    argument = one_argument( argument, arg1 );
    argument = one_argument( argument, arg2 );
 
    if ( arg1[0] == '\0' )               itemtype = ITEM_TRASH;
    else if (!str_cmp(arg1,"light"    )) itemtype = ITEM_LIGHT;
    else if (!str_cmp(arg1,"scroll"   )) itemtype = ITEM_SCROLL;
    else if (!str_cmp(arg1,"wand"     )) itemtype = ITEM_WAND;
    else if (!str_cmp(arg1,"staff"    )) itemtype = ITEM_STAFF;
    else if (!str_cmp(arg1,"weapon"   )) itemtype = ITEM_WEAPON;
    else if (!str_cmp(arg1,"treasure" )) itemtype = ITEM_TREASURE;
    else if (!str_cmp(arg1,"armor"    )) itemtype = ITEM_ARMOR;
    else if (!str_cmp(arg1,"armour"   )) itemtype = ITEM_ARMOR;
    else if (!str_cmp(arg1,"potion"   )) itemtype = ITEM_POTION;
    else if (!str_cmp(arg1,"furniture")) itemtype = ITEM_FURNITURE;
    else if (!str_cmp(arg1,"trash"    )) itemtype = ITEM_TRASH;
    else if (!str_cmp(arg1,"container")) itemtype = ITEM_CONTAINER;
    else if (!str_cmp(arg1,"drink"    )) itemtype = ITEM_DRINK_CON;
    else if (!str_cmp(arg1,"key"      )) itemtype = ITEM_KEY;
    else if (!str_cmp(arg1,"food"     )) itemtype = ITEM_FOOD;
    else if (!str_cmp(arg1,"money"    )) itemtype = ITEM_MONEY;
    else if (!str_cmp(arg1,"boat"     )) itemtype = ITEM_BOAT;
    else if (!str_cmp(arg1,"corpse"   )) itemtype = ITEM_CORPSE_NPC;
    else if (!str_cmp(arg1,"fountain" )) itemtype = ITEM_FOUNTAIN;
    else if (!str_cmp(arg1,"pill"     )) itemtype = ITEM_PILL;
    else if (!str_cmp(arg1,"portal"   )) itemtype = ITEM_PORTAL;
    else if (!str_cmp(arg1,"egg"      )) itemtype = ITEM_EGG;
    else if (!str_cmp(arg1,"stake"    )) itemtype = ITEM_STAKE;
    else if (!str_cmp(arg1,"missile"  )) itemtype = ITEM_MISSILE;
    else                                 itemtype = ITEM_TRASH;

    if ( arg2[0] == '\0' || !is_number( arg2 ) )
    {
	level = 0;
    }
    else
    {
        level = atoi( arg2 );
	if ( level < 1 || level > 50 )
        {
	    send_to_char( "Level should be within range 1 to 50.\n\r", ch );
	    return;
        }
    }

    if ( ( pObjIndex = get_obj_index( OBJ_VNUM_PROTOPLASM ) ) == NULL )
    {
	send_to_char( "Error...missing object, please inform KaVir.\n\r", ch );
	return;
    }

    obj = create_object( pObjIndex, level );
    obj->level = level;
    obj->item_type = itemtype;
    obj_to_char(obj,ch);
    if (obj->questmaker != NULL) free_string(obj->questmaker);
    obj->questmaker = str_dup(ch->name);

    act( "You reach up into the air and draw out a ball of protoplasm.", ch, obj, NULL, TO_CHAR );
    act( "$n reaches up into the air and draws out a ball of protoplasm.", ch, obj, NULL, TO_ROOM );
    return;
}

void do_token( CHAR_DATA *ch, char *argument )
{
    char arg1[MAX_INPUT_LENGTH];
    char arg2[MAX_INPUT_LENGTH];
    char buf[MAX_STRING_LENGTH];
    OBJ_INDEX_DATA *pObjIndex;
    OBJ_DATA *obj;
    CHAR_DATA *victim;
    int value;

    argument = one_argument( argument, arg1 );
    argument = one_argument( argument, arg2 );
 
    if ( IS_NPC(ch) || (ch->pcdata->quest < 1 && !IS_JUDGE(ch)) )
    {
	send_to_char("You are unable to make a quest token.\n\r",ch);
	return;
    }
    else if ( arg1[0] == '\0' || !is_number( arg1 ) )
    {
	send_to_char("Please specify a value for the quest token.\n\r",ch);
	return;
    }
    else
    {
        value = atoi( arg1 );
	if (( value < 1 || value > 500) && ch->level <= 6 )
        {
	    send_to_char( "Quest token should have a value between 1 and 500.\n\r", ch );
	    return;
        }
	else if ( value > ch->pcdata->quest && !IS_JUDGE(ch) )
        {
	    sprintf(buf,"You only have %d quest points left to put into tokens.\n\r",ch->pcdata->quest);
	    send_to_char( buf, ch );
	    return;
        }
    }

    victim = get_char_room( ch, arg2 );

    if ( ( pObjIndex = get_obj_index( OBJ_VNUM_PROTOPLASM ) ) == NULL )
    {
	send_to_char( "Error...missing object, please inform KaVir.\n\r", ch );
	return;
    }

    ch->pcdata->quest -= value;
    if (ch->pcdata->quest < 0) ch->pcdata->quest = 0;
    obj = create_object( pObjIndex, value );
    obj->value[0] = value;
    obj->level = 1;
    obj->cost = value*1000;
    obj->item_type = ITEM_QUEST;
    obj_to_char(obj,ch);
    if (obj->questmaker != NULL) free_string(obj->questmaker);
    obj->questmaker = str_dup(ch->name);
    free_string( obj->name );
    obj->name = str_dup( "quest token" );
    sprintf(buf,"a %d point quest token",value);
    free_string( obj->short_descr );
    obj->short_descr = str_dup( buf );
    sprintf(buf,"A %d point quest token lies on the floor.",value);
    free_string( obj->description );
    obj->description = str_dup( buf );
    if (victim != NULL && victim != ch)
    {
    	act( "You reach behind $N's ear and produce $p.", ch, obj, victim, TO_CHAR );
    	act( "$n reaches behind $N's ear and produces $p.", ch, obj, victim, TO_NOTVICT );
    	act( "$n reaches behind your ear and produces $p.", ch, obj, victim, TO_VICT );
    }
    else
    {
    	act( "You snap your fingers and reveal $p.", ch, obj, NULL, TO_CHAR );
    	act( "$n snaps $s fingers and reveals $p.", ch, obj, NULL, TO_ROOM );
    }
        sprintf( log_buf, "%s makes a %d point quest token.", ch->name, value );
        log_string( log_buf, obj->oid );
    return;
}

void do_qtrust( CHAR_DATA *ch, char *argument )
{
    char arg1[MAX_INPUT_LENGTH];
    char arg2[MAX_INPUT_LENGTH];
    CHAR_DATA *victim;

    argument = one_argument( argument, arg1 );
    argument = one_argument( argument, arg2 );

    if ( arg1[0] == '\0' )
    {
	send_to_char( "Syntax: qtrust <char> <on/off>.\n\r", ch );
	return;
    }

    if ( arg2[0] == '\0' )
    {
	send_to_char( "Do you wish to set qtrust ON or OFF?\n\r", ch );
	return;
    }

    if ( ( victim = get_char_room( ch, arg1 ) ) == NULL )
    {
	send_to_char( "That player is not here.\n\r", ch);
	return;
    }

    if (str_cmp(arg2,"on") && str_cmp(arg2,"off"))
    {
	send_to_char( "Do you want to set their qtrust ON or OFF?\n\r", ch);
	return;
    }
    if (!str_cmp(arg2,"off"))
    {
	if (!IS_EXTRA(victim, EXTRA_TRUSTED))
	{
	    send_to_char("Their qtrust is already off.\n\r",ch);
	    return;
	}
	REMOVE_BIT(victim->extra, EXTRA_TRUSTED);
	send_to_char("Quest trust OFF.\n\r",ch);
	send_to_char("You are no longer quest trusted.\n\r",victim);
	return;
    }
    else if (!str_cmp(arg2,"on"))
    {
	if (IS_EXTRA(victim, EXTRA_TRUSTED))
	{
	    send_to_char("Their qtrust is already on.\n\r",ch);
	    return;
	}
	SET_BIT(victim->extra, EXTRA_TRUSTED);
	send_to_char("Quest trust ON.\n\r",ch);
	send_to_char("You are now quest trusted.\n\r",victim);
	return;
    }
    return;
}

void do_ntrust( CHAR_DATA *ch, char *argument )
{
    char arg1[MAX_INPUT_LENGTH];
    char arg2[MAX_INPUT_LENGTH];
    CHAR_DATA *victim;

    argument = one_argument( argument, arg1 );
    argument = one_argument( argument, arg2 );

    if ( arg1[0] == '\0' )
    {
	send_to_char( "Syntax: ntrust <char> <on/off>.\n\r", ch );
	return;
    }

    if ( arg2[0] == '\0' )
    {
	send_to_char( "Do you wish to set ntrust ON or OFF?\n\r", ch );
	return;
    }

    if ( ( victim = get_char_room( ch, arg1 ) ) == NULL )
    {
	send_to_char( "That player is not here.\n\r", ch);
	return;
    }

    if (str_cmp(arg2,"on") && str_cmp(arg2,"off"))
    {
	send_to_char( "Do you want to set their ntrust ON or OFF?\n\r",ch);
	return;
    }
    if (!str_cmp(arg2,"off"))
    {
	if (!IS_EXTRA(victim, EXTRA_TRUSTED))
	{
	    send_to_char("Their ntrust is already off.\n\r",ch);
	    return;
	}
	REMOVE_BIT(victim->extra, EXTRA_NOTE_TRUST);
	send_to_char("Note trust OFF.\n\r",ch);
	send_to_char("You are no longer note trusted.\n\r",victim);
	return;
    }
    else if (!str_cmp(arg2,"on"))
    {
	if (IS_EXTRA(victim, EXTRA_NOTE_TRUST))
	{
	    send_to_char("Their ntrust is already on.\n\r",ch);
	    return;
	}
	SET_BIT(victim->extra, EXTRA_NOTE_TRUST);
	send_to_char("Note trust ON.\n\r",ch);
	send_to_char("You are now note trusted.\n\r",victim);
	return;
    }
    return;
}

void do_mclear( CHAR_DATA *ch, char *argument )
{
    OBJ_DATA *obj;
    OBJ_DATA *obj_next;
    char arg[MAX_INPUT_LENGTH];
    CHAR_DATA *victim;

    argument = one_argument( argument, arg );

    if ( arg[0] == '\0' )
    {
	send_to_char( "Syntax: mclear <char>.\n\r", ch );
	return;
    }

    if ( ( victim = get_char_room( ch, arg ) ) == NULL )
    {
	send_to_char( "That player is not here.\n\r", ch);
	return;
    }

    if (IS_NPC(victim))
    {
	send_to_char( "Not on NPC's.\n\r", ch);
	return;
    }

    if (IS_SET(victim->act, PLR_GODLESS) && get_trust(ch) < NO_GODLESS
        && !IS_SET(ch->extra , EXTRA_ANTI_GODLESS))
    {
	send_to_char( "You failed.\n\r", ch );
	return;
    }

    for ( obj = victim->carrying; obj != NULL; obj = obj_next )
    {
	obj_next = obj->next_content;
	if ( obj->wear_loc != WEAR_NONE ) 
	{obj_from_char(obj); obj_to_char(obj,victim);}
    }
    while ( victim->affected )
	affect_remove( victim, victim->affected );
    if (IS_AFFECTED(victim,AFF_POLYMORPH) && IS_AFFECTED(victim,AFF_ETHEREAL))
    {
    	victim->affected_by	= AFF_POLYMORPH + AFF_ETHEREAL;
    }
    else if (IS_AFFECTED(victim,AFF_POLYMORPH))
    	victim->affected_by	= AFF_POLYMORPH;
    else if (IS_AFFECTED(victim,AFF_ETHEREAL))
    	victim->affected_by	= AFF_ETHEREAL;
    else
    	victim->affected_by	= 0;
    victim->armor	 = 100;
    victim->hit		 = UMAX( 1, victim->hit  );
    victim->mana	 = UMAX( 1, victim->mana );
    victim->move	 = UMAX( 1, victim->move );
    victim->hitroll	 = 0;
    victim->damroll	 = 0;
    victim->saving_throw = 0;
    victim->pcdata->mod_str = 0;
    victim->pcdata->mod_int = 0;
    victim->pcdata->mod_wis = 0;
    victim->pcdata->mod_dex = 0;
    victim->pcdata->mod_con = 0;
    victim->pcdata->followers = 0;
    save_char_obj( victim );
    send_to_char("Your stats have been cleared.  Please rewear your equipment.\n\r",victim);
    send_to_char("Ok.\n\r",ch);
    return;
}

void do_clearstats( CHAR_DATA *ch, char *argument )
{
    OBJ_DATA *obj;
    OBJ_DATA *obj_next;
    char buf[MAX_STRING_LENGTH];

    if (IS_NPC(ch))
    {
	return;
    }

    for ( obj = ch->carrying; obj != NULL; obj = obj_next )
    {
	obj_next = obj->next_content;
	if ( obj->wear_loc != WEAR_NONE ) 
	{obj_from_char(obj); obj_to_char(obj,ch);}
    }
    while ( ch->affected )
	affect_remove( ch, ch->affected );
    if (IS_AFFECTED(ch,AFF_POLYMORPH) && IS_AFFECTED(ch,AFF_ETHEREAL))
    {
    	ch->affected_by	 = AFF_POLYMORPH + AFF_ETHEREAL;
    }
    else if (IS_AFFECTED(ch,AFF_POLYMORPH))
    	ch->affected_by	 = AFF_POLYMORPH;
    else if (IS_AFFECTED(ch,AFF_ETHEREAL))
    	ch->affected_by	 = AFF_ETHEREAL;
    else
    	ch->affected_by	 = 0;
    ch->armor		 = 100;
    ch->hit		 = UMAX( 1, ch->hit  );
    ch->mana		 = UMAX( 1, ch->mana );
    ch->move		 = UMAX( 1, ch->move );
    ch->hitroll		 = 0;
    ch->damroll		 = 0;
    ch->saving_throw	 = 0;
    ch->pcdata->mod_str	 = 0;
    ch->pcdata->mod_int	 = 0;
    ch->pcdata->mod_wis	 = 0;
    ch->pcdata->mod_dex	 = 0;
    ch->pcdata->mod_con	 = 0;
    ch->damcap[DAM_CAP]	 = 2000;
    ch->damcap[DAM_MOD]	 = 0;
    save_char_obj( ch );
    send_to_char("Your stats have been cleared.  Please rewear your equipment.\n\r",ch);
    if (ch->itemaffect != 0)
    {
	sprintf(buf, "Clearstats error on %s: Itemaffect !=0", ch->name);
	log_string(buf, ch->itemaffect);
	ch->itemaffect = 0;
    }
    return;
}

void do_otransfer( CHAR_DATA *ch, char *argument )
{
    char arg1[MAX_INPUT_LENGTH];
    char arg2[MAX_INPUT_LENGTH];
    OBJ_DATA *obj;
    CHAR_DATA *victim;
    ROOM_INDEX_DATA *chroom;
    ROOM_INDEX_DATA *objroom;

    argument = one_argument( argument, arg1 );
    argument = one_argument( argument, arg2 );
 
    if ( arg1[0] == '\0' )
    {
        send_to_char( "Otransfer which object?\n\r", ch );
        return;
    }

    if ( arg2[0] == '\0' ) victim = ch;
    else if ( ( victim = get_char_world( ch, arg2 ) ) == NULL )
    {
	send_to_char( "They aren't here.\n\r", ch );
	return;
    }

    if ( ( obj = get_obj_world( ch, arg1 ) ) == NULL )
    {
	send_to_char( "Nothing like that in hell, earth, or heaven.\n\r", ch );
	return;
    }

    if (!IS_JUDGE(ch) && (obj->questmaker == NULL || 
	str_cmp(ch->name,obj->questmaker) || strlen(obj->questmaker) < 2))
    {
	send_to_char("You don't have permission to otransfer that item.\n\r", ch);
	return;
    }

    if (obj->carried_by != NULL)
    {
	if (!IS_NPC(obj->carried_by) && IS_SET(obj->carried_by->act,PLR_GODLESS) && get_trust(ch) < NO_GODLESS
        && !IS_SET(ch->extra , EXTRA_ANTI_GODLESS))
	{
	    send_to_char("You failed.\n\r",ch);
	    act("$p flickers briefly with energy.",obj->carried_by,obj,NULL,TO_CHAR);
	    return;
	}
	act("$p vanishes from your hands in an explosion of energy.",obj->carried_by,obj,NULL,TO_CHAR);
	act("$p vanishes from $n's hands in an explosion of energy.",obj->carried_by,obj,NULL,TO_ROOM);
	obj_from_char(obj);
    }
    else if (obj->in_obj     != NULL) obj_from_obj(obj);
    else if (obj->in_room != NULL)
    {
    	chroom = ch->in_room;
    	objroom = obj->in_room;
    	char_from_room(ch);
    	char_to_room(ch,objroom);
    	act("$p vanishes from the ground in an explosion of energy.",ch,obj,NULL,TO_ROOM);
	if (chroom == objroom) act("$p vanishes from the ground in an explosion of energy.",ch,obj,NULL,TO_CHAR);
    	char_from_room(ch);
    	char_to_room(ch,chroom);
	obj_from_room(obj);
    }
    else
    {
	send_to_char( "You were unable to get it.\n\r", ch );
	return;
    }
    obj_to_char(obj,victim);
    act("$p appears in your hands in an explosion of energy.",victim,obj,NULL,TO_CHAR);
    act("$p appears in $n's hands in an explosion of energy.",victim,obj,NULL,TO_ROOM);
    return;
}

void bind_char( CHAR_DATA *ch )
{
    OBJ_DATA *obj;
    OBJ_INDEX_DATA *pObjIndex;
    ROOM_INDEX_DATA *location;

    if ( IS_NPC(ch) || ch->pcdata->obj_vnum < 1 )
	return;

    if ( ( pObjIndex = get_obj_index( ch->pcdata->obj_vnum ) ) == NULL )
	return;

    if ( ch->in_room == NULL || ch->in_room->vnum == ROOM_VNUM_IN_OBJECT )
    {
	location = get_room_index(ROOM_VNUM_ALTAR);
    	char_from_room(ch);
    	char_to_room(ch,location);
    }
    else location = ch->in_room;

    obj = create_object( pObjIndex, 50 );
    obj_to_room(obj,location);
    obj->chobj = ch;
    ch->pcdata->chobj = obj;
    SET_BIT(ch->affected_by, AFF_POLYMORPH);
    SET_BIT(ch->extra, EXTRA_OSWITCH);
    free_string(ch->morph);
    ch->morph = str_dup(obj->short_descr);
    send_to_char( "You reform yourself.\n\r", ch );
    act("$p fades into existance on the floor.",ch,obj,NULL,TO_ROOM);
    do_look(ch,"auto");
    return;
}

void do_bind( CHAR_DATA *ch, char *argument )
{
    char arg1[MAX_INPUT_LENGTH];
    char arg2[MAX_INPUT_LENGTH];
    OBJ_DATA *obj;
    CHAR_DATA *victim;

    argument = one_argument( argument, arg1 );
    argument = one_argument( argument, arg2 );

    if ( arg1[0] == '\0' || arg2[0] == '\0' )
    {
	send_to_char( "Syntax: bind <player> <object>\n\r", ch );
	return;
    }

    if ( ( victim = get_char_room( ch, arg1 ) ) == NULL )
    {
	send_to_char( "That player is not here.\n\r", ch);
	return;
    }
    if (ch == victim)
    {
	send_to_char( "You can't do this to yourself.\n\r", ch);
	return;
    }
    if (IS_NPC(victim))
    {
	send_to_char( "Not on NPC's.\n\r", ch);
	return;
    }
    else if (IS_AFFECTED(victim,AFF_POLYMORPH))
    {
	send_to_char( "You cannot do this while they are polymorphed.\n\r", ch);
	return;
    }
    else if (IS_IMMORTAL(victim))
    {
	send_to_char( "Only on mortals or avatars.\n\r", ch);
	return;
    }
    else if (IS_SET(victim->act, PLR_GODLESS) && get_trust(ch) < NO_GODLESS
        && !IS_SET(ch->extra , EXTRA_ANTI_GODLESS))
    {
	send_to_char( "You failed.\n\r", ch );
	return;
    }
    if ( ( obj = get_obj_carry( ch, arg2) ) == NULL )
    {
	send_to_char("You are not carrying that item.\n\r",ch);
	return;
    }
    if (obj->questmaker != NULL && strlen(obj->questmaker) > 1)
    {
	send_to_char("You cannot bind someone into a modified item.\n\r",ch);
	return;
    }
    if (obj->chobj != NULL)
    {
	send_to_char( "That item already has someone bound in it.\n\r", ch);
	return;
    }
    send_to_char("Ok.\n\r",ch);
    act("$n transforms into a white vapour and pours into $p.",victim,obj,NULL,TO_ROOM);
    act("You transform into a white vapour and pour into $p.",victim,obj,NULL,TO_CHAR);
    victim->pcdata->obj_vnum = obj->pIndexData->vnum;
    obj->chobj = victim;
    victim->pcdata->chobj = obj;
    SET_BIT(victim->affected_by, AFF_POLYMORPH);
    SET_BIT(victim->extra, EXTRA_OSWITCH);
    free_string(victim->morph);
    victim->morph = str_dup(obj->short_descr);
    return;
}

void do_release( CHAR_DATA *ch, char *argument )
{
    char arg[MAX_INPUT_LENGTH];
    OBJ_DATA *obj;
    CHAR_DATA *victim;

    argument = one_argument( argument, arg );

    if ( arg[0] == '\0' )
    {
	send_to_char( "Syntax: release <object>\n\r", ch );
	return;
    }

    if ( ( obj = get_obj_carry( ch, arg) ) == NULL )
    {
	send_to_char("You are not carrying that item.\n\r",ch);
	return;
    }
    if ( (victim = obj->chobj) == NULL)
    {
	send_to_char( "There is nobody bound in that item.\n\r", ch);
	return;
    }
    send_to_char("Ok.\n\r",ch);
    victim->pcdata->obj_vnum = 0;
    obj->chobj = NULL;
    victim->pcdata->chobj = NULL;
    REMOVE_BIT(victim->affected_by, AFF_POLYMORPH);
    REMOVE_BIT(victim->extra, EXTRA_OSWITCH);
    free_string(victim->morph);
    victim->morph = str_dup("");
    act("A white vapour pours out of $p and forms into $n.",victim,obj,NULL,TO_ROOM);
    act("Your spirit floats out of $p and reforms its body.",victim,obj,NULL,TO_CHAR);
    return;
}


void do_morph( CHAR_DATA *ch, char *argument )
{
    char arg1[MAX_INPUT_LENGTH];
    char arg2[MAX_INPUT_LENGTH];
    OBJ_DATA *obj;
    OBJ_DATA *morph;
    int mnum;

    argument = one_argument( argument, arg1 );
    argument = one_argument( argument, arg2 );

    if ( arg1[0] == '\0' )
    {
	send_to_char( "Syntax: morph <object> <form>\n\r", ch );
	return;
    }

    if ( ( obj = get_obj_wear( ch, arg1) ) == NULL )
    {
    	if ( ( obj = get_obj_carry( ch, arg1) ) == NULL )
	{
	    send_to_char("You are not carrying that item.\n\r",ch);
	    return;
	}
    }

    if ( !IS_SET(obj->spectype, SITEM_MORPH) )
    {
	send_to_char("That item cannot morph.\n\r",ch);
	return;
    }

    if ( arg2[0] == '\0' )
    {
	act("$p can morph into the following forms:",ch,obj,NULL,TO_CHAR);
	show_list_to_char( obj->contains, ch, TRUE, TRUE );
	return;
    }

    if ( ( morph = get_obj_list( ch, arg2, obj->contains ) ) == NULL )
    {
	send_to_char("It cannot assume that form.\n\r",ch);
	return;
    }
    obj_from_obj( morph );
    obj_to_char( morph, ch );
    act( "$p morphs into $P in $n's hands!", ch, obj, morph, TO_ROOM );
    act( "$p morphs into $P in your hands!", ch, obj, morph, TO_CHAR );
    mnum = obj->wear_loc;
    obj_from_char( obj );
    obj_to_obj( obj, morph );
    if ( morph->wear_flags == obj->wear_flags && mnum != WEAR_NONE )
	equip_char( ch, morph, mnum );
    return;
}

