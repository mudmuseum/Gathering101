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

void do_highlander (CHAR_DATA *ch, char *argument)
{

    if (IS_NPC(ch)) return;

    if ( !IS_SET(ch->immune, IMM_HIGHLANDER) )
    {
	SET_BIT(ch->immune, IMM_HIGHLANDER);
	if ( IS_CLASS(ch, CLASS_HIGHLANDER) )
	    send_to_char("You will now allow other clans to hire you as a mercenary.\r\n", ch);
	else
	    send_to_char("You now have the chance to become a highlander.\r\n", ch);
    }
    else
    {
	REMOVE_BIT(ch->immune, IMM_HIGHLANDER);
	if ( IS_CLASS(ch, CLASS_HIGHLANDER) )
	    send_to_char("You are no longer available as a mercenary.\r\n", ch);
	else
	    send_to_char("You don't want the chance to become a highlander.\r\n", ch);
    }

    return;
}

void create_highlander( CHAR_DATA *victim )
{
    int rolled, chance;
    char buf[MAX_STRING_LENGTH];
    char log_buf[MAX_STRING_LENGTH];
    OBJ_DATA *obj;

    if (IS_NPC(victim)) 			return;
    if (IS_CLASS(victim, CLASS_HIGHLANDER) )	return;
    if (!IS_IMMUNE(victim, IMM_HIGHLANDER) ) 	return;
    if (victim->pcdata->timer[TIMER_DEATH] > 0) return;
    if (victim->level >= LEVEL_ARCHMAGE) 	return;
    if (victim->trust >= LEVEL_ARCHMAGE)	return;
    if ((IS_CLASS(victim, CLASS_VAMPIRE) || IS_CLASS(victim, CLASS_WEREWOLF)
	|| IS_CLASS(victim, CLASS_DEMON) ) 
	&& victim->pcdata->stats[UNI_GEN] < 3)	return;

    rolled = number_percent();

    /* Change the chance calculation to your liking... */
    chance = UMAX(1, number_range(1,60));
    chance = UMIN ( chance, 45 );

/*    sprintf(buf, "Chance: %d, Rolled: %d", chance, rolled);
    log_string(buf, 0); */
    
    if ( rolled < chance)
    {
	save_char_obj_backup(victim); 
	clear_stats(victim);
	unclass(victim);
	victim->class = CLASS_HIGHLANDER;
	victim->level = LEVEL_AVATAR;
	victim->trust = 0;

        sprintf(buf,"Fate rescues %s from mortal death!",victim->name);
        sprintf(log_buf, "%s becomes highlanderrific.", victim->name);

        send_to_char("Fate rescues you from mortality, you are a highlander!\n\r", victim);
        log_string(log_buf, victim->in_room->vnum);
        do_info(victim, buf);

        obj = create_object( get_obj_index( OBJ_VNUM_KATANA ), 0 );
        free_string(obj->name);
        sprintf(buf, "%s highlander katana", victim->name);
        obj->name = str_dup(buf);
        free_string(obj->short_descr);
        sprintf(buf,"%s's gleaming katana",victim->name);
        obj->short_descr = str_dup(buf);
	free_string(obj->questowner);
        sprintf(buf, "%s", victim->name);
        obj->questowner = str_dup(buf);
        obj_to_char(obj, victim);

	save_char_obj(victim);

    } 


}

void highlander_regen( CHAR_DATA *ch, int heal_factor )
{

    int level = 23 - ch->pcdata->powers[HPOWER_TRAINED] - heal_factor;

    if (!IS_HERO(ch))
    {
	return;
    }

    if ( ch->hit >= ch->max_hit && ch->mana >= ch->max_mana &&
	 ch->move >= ch->max_move )
	return;
  
    if ( ch->hit < 1 )
    {
	ch->hit = ch->hit + number_range(2,4);
	update_pos(ch);
    }
    else if (ch->position == POS_SLEEPING)
    {
	ch->hit = UMIN ( ch->hit + (ch->max_hit - ch->hit) / level, 
	    ch->max_hit);

	ch->mana = UMIN (ch->mana + (ch->max_mana - ch->mana) / level,
	    ch->max_mana);

	ch->move = UMIN (ch->move + (ch->max_move - ch->move) / level, 
	    ch->max_move);

	if ( ch->hit >= ch->max_hit && ch->mana >= ch->max_mana && ch->move >= ch->max_move )
	    send_to_char("Your body has completely regenerated itself.\n\r",ch);
    }
    else if ( (ch->hit / ch->max_hit) < 0.15 )
    {
	werewolf_regen(ch, 3);
    }
    else
	werewolf_regen(ch, 2);
 
    return;

}

void do_hire ( CHAR_DATA *ch, char *argument )
{
    CHAR_DATA *victim;
    bool can_hire = FALSE;
    char arg[MAX_INPUT_LENGTH];

    if (IS_NPC(ch)) return;

    if ( ch->pcdata->stats[UNI_GEN] == 2 ) 	can_hire = TRUE;
    else if ( IS_SET(ch->special, SPC_PRINCE) )	can_hire = TRUE;
    else if ( IS_CLASS(ch, CLASS_DEMON) &&
	( ch->pcdata->stats[UNI_GEN] == 3 ) )	can_hire = TRUE;

    if ( strlen(ch->clan) < 2 )			can_hire = FALSE;

    if (!can_hire)
    {
	send_to_char("Huh?\r\n", ch);
	return;
    }

    one_argument(argument, arg);

    if (arg[0] == '\0')
    {
	send_to_char("Whom do you wish to hire?\r\n", ch);
	return;
    }

    if ( ( victim = get_char_world( ch, arg ) ) == NULL )
    {
	send_to_char( "They aren't here.\n\r", ch );
	return;
    }

    if ( !IS_CLASS(victim, CLASS_HIGHLANDER) )
    {
	send_to_char( "You can only hire highlanders as mercenaries.\r\n", ch);
	return;

    }

    if ( strlen(victim->clan) > 1 )
    {
	send_to_char( "They appear to be employed already.\r\n", ch);
	return;
    }

    if ( !IS_IMMUNE(victim, IMM_HIGHLANDER) )
    {
	send_to_char( "They are not available for employment.\r\n", ch);
	return;
    }

    free_string(victim->clan);
    victim->clan = str_dup (ch->clan);

    act("$N hires you as a mercenary for $s clan.", ch, NULL, victim, TO_VICT);
    act("You hire $N as a mercenary for your clan.", ch, NULL, victim, TO_CHAR);

    return;
}

void do_fire ( CHAR_DATA *ch, char *argument )
{
    char arg[MAX_INPUT_LENGTH];
    CHAR_DATA *victim;

    if (IS_CLASS(ch, CLASS_HIGHLANDER) )
    {
	send_to_char("Use RETIRE to leave your clan.\r\n", ch);
	return;
    }

    if (IS_CLASS(ch, CLASS_VAMPIRE) && (ch->pcdata->stats[UNI_GEN] > 2 ||
	!IS_SET(ch->special, SPC_PRINCE) ) )
    {
	send_to_char("Only clan leaders can fire mercenaries.\r\n", ch);
	return;
    }

    if (IS_CLASS(ch, CLASS_WEREWOLF) && (ch->pcdata->stats[UNI_GEN] > 2 ||
	!IS_SET(ch->special, SPC_PRINCE) ) )
    {
	send_to_char("Only clan leaders can fire mercenaries.\r\n", ch);
	return;
    }

    if (IS_CLASS(ch, CLASS_DEMON) && ch->pcdata->stats[UNI_GEN] > 3)
    {
	send_to_char("Only clan leaders can fire mercenaries.\r\n", ch);
	return;
    }

    one_argument(argument, arg);

    if ( ( victim = get_char_world( ch, arg ) ) == NULL )
    {
	send_to_char( "They aren't here.\n\r", ch );
	return;
    }

    if (str_cmp(ch->clan, victim->clan))
    {
	send_to_char("You can only fire your clan's mercenaries.\r\n", ch);
	return;
    }

    free_string(victim->clan);
    victim->clan = str_dup("");

    act("$N fires you from your mercenary duties.", ch, NULL, victim, TO_VICT);
    act("You fire $n from their mercenary duties.", ch, NULL, victim, TO_CHAR);

    if ( get_room_index(victim->home) == NULL ||
        get_room_index(victim->home)->sector_type == SECT_HQ )
        victim->home = ROOM_VNUM_TEMPLE;

    if (victim->in_room->sector_type == SECT_HQ)
	do_recall(victim, "");

    return;
}

void do_retire ( CHAR_DATA *ch, char *argument )
{
    char arg[MAX_INPUT_LENGTH];

    if (!IS_CLASS(ch, CLASS_HIGHLANDER))
    {
	send_to_char("Huh?\r\n", ch);
	return;
    }

    if (ch->clan[0] == '\0')
    {
	send_to_char("You aren't even a mercenary!\r\n", ch);
	return;
    }

    one_argument(argument, arg);

    if (str_cmp(ch->clan, arg) || arg[0] == '\0')
    {
	send_to_char("Syntax is: retire <clan>\r\n", ch);
	return;
    }

    free_string(ch->clan);
    ch->clan = str_dup("");

    act("You retire from your mercenary duties.", ch, NULL, NULL, TO_CHAR);

    if ( get_room_index(ch->home) == NULL ||
        get_room_index(ch->home)->sector_type == SECT_HQ )
        ch->home = ROOM_VNUM_TEMPLE;

    if (ch->in_room->sector_type == SECT_HQ)
	do_recall(ch, "");

    return;
}

void do_highskills ( CHAR_DATA *ch, char *argument )
{
    char buf [MAX_STRING_LENGTH];
    char arg [MAX_INPUT_LENGTH];

    if (IS_NPC(ch)) return;

    if (!IS_CLASS(ch, CLASS_HIGHLANDER) )
    {
	send_to_char("Huh?\r\n", ch);
	return;
    }

    one_argument(argument, arg);

    if (!str_cmp(arg,"improve"))
    {
	if (ch->practice < 100)
 	{
	    send_to_char("It costs 100 primal to improve your skills.\r\n", ch);
	    return;
	}

	if (ch->pcdata->powers[HPOWER_TRAINED] >= 
	    ch->pcdata->powers[HPOWER_MASTERED] )
	{
	    send_to_char("You are not ready to advance your training.\r\n", ch);
	    return;
	}
	
	ch->pcdata->powers[HPOWER_TRAINED]++;
	ch->practice -= 100;
	send_to_char("Ok.\r\n", ch);	
	return;
    }
    else
    {
	send_to_char("		-= Highlander Skills =-\r\n", ch);
	send_to_char("Type \"highskills improve\" to advance.\r\n", ch);

	sprintf(buf, "Magical resistance: %d\r\n", char_save(ch) );
	send_to_char(buf, ch);

	if (ch->pcdata->powers[HPOWER_TRAINED] >= 1)
	{
	    send_to_char("TRUESIGHT: Your vision is greatly enhanced.\r\n", ch);
	}
	else if (ch->pcdata->powers[HPOWER_MASTERED] >= 1)
 	{
	    send_to_char("Truesight.\r\n", ch);
	}

	if (ch->pcdata->powers[HPOWER_TRAINED] >= 2)
	{
	    send_to_char("TRUESENSE: You can sense the presence of nearby immortals.\r\n", ch);
	}
	else if (ch->pcdata->powers[HPOWER_MASTERED] >= 2)
	{
	    send_to_char("Truesense.\r\n", ch);
	}

	if (ch->pcdata->powers[HPOWER_TRAINED] >= 3)
	{
	    send_to_char("STRENGTH: Your skill enables you to hit with deadly accuracy.\r\n", ch);
	}
	else if (ch->pcdata->powers[HPOWER_MASTERED] >= 3)
	{
	    send_to_char("Strength.\r\n", ch);
	}
	if (ch->pcdata->powers[HPOWER_TRAINED] >= 4)
	{
	    send_to_char("ONENESS: You feel a oneness with all life, soothing their hostility.\r\n", ch);
	}
	else if (ch->pcdata->powers[HPOWER_MASTERED] >= 4)
	{
	    send_to_char(" Oneness.\r\n", ch);
	}
	if (ch->pcdata->powers[HPOWER_TRAINED] >= 5)
	{
	    send_to_char("FORGE: Weapons of any type can become powerful in your hands.\r\n", ch);
	}
	else if (ch->pcdata->powers[HPOWER_MASTERED] >= 5)
	{
	    send_to_char("Forge.\r\n", ch);
	}

	if (ch->pcdata->powers[HPOWER_TRAINED] >= 6)
	{
	    send_to_char("DISARM: You can disarm opponents even if they are immune.\r\n", ch);
	}
	else if (ch->pcdata->powers[HPOWER_MASTERED] >= 6)
	{
	    send_to_char("Disarm.\r\n", ch);
	}

	if (ch->pcdata->powers[HPOWER_TRAINED] >= 7)
	{
	    send_to_char("AGILITY: Increased ability to dodge blows and kicks.\r\n",ch);
	}
	else if (ch->pcdata->powers[HPOWER_MASTERED] >= 7)
	{
	    send_to_char("Agility.\r\n", ch);
	}

	if (ch->pcdata->powers[HPOWER_TRAINED] >= 8)
	{
	    send_to_char("Under construction.\r\n", ch);
	}
	if (ch->pcdata->powers[HPOWER_TRAINED] >= 9)
	{
	    sprintf(buf,"Under construction.\r\n");
	    send_to_char(buf, ch);
	}
	if (ch->pcdata->powers[HPOWER_TRAINED] >= 10)
	{
	    sprintf(buf,"Under construction.\r\n");
	    send_to_char(buf, ch);
	}
	if (ch->pcdata->powers[HPOWER_TRAINED] >= 11)
	{
	    sprintf(buf,"Under construction.\r\n");
	    send_to_char(buf, ch);
	}
	if (ch->pcdata->powers[HPOWER_TRAINED] >= 12)
	{
	    sprintf(buf,"Under construction.\r\n");
	    send_to_char(buf, ch);
	}
	if (ch->pcdata->powers[HPOWER_TRAINED] >= 13)
	{
	    sprintf(buf,"Under construction.\r\n");
	    send_to_char(buf, ch);
	}

    }

    return;
}


void do_forge (CHAR_DATA *ch, char *argument)
{
    char arg[MAX_INPUT_LENGTH];
    OBJ_DATA *obj;

    one_argument(argument, arg);

    if (!IS_CLASS(ch, CLASS_HIGHLANDER) )
    {
	send_to_char("Huh?\r\n", ch);
	return;
    }


    if (ch->pcdata->powers[HPOWER_TRAINED] < 5)
    {
	send_to_char("You are not that skilled.\r\n", ch);
	return;
    }

    if (!IS_HERO(ch))
    {
	send_to_char("Huh?\r\n", ch);
	return;
    }

    if ( (obj = get_obj_carry(ch, arg) ) == NULL )
    {
	send_to_char("You aren't carrying that.\r\n", ch);
	return;
    }

    if ( obj->item_type != ITEM_WEAPON )
    {
	send_to_char("You can only forge your soul into weapons.\r\n", ch);
	return;
    }

    if ( IS_SET(obj->spectype, SITEM_HIGHLANDER) )
    {
	send_to_char("A part of someone's soul is already forged into it.\r\n", ch);
	return;
    }

    if ( str_cmp(ch->name, obj->questowner) )
    {
	send_to_char("Only on something you own.\r\n", ch);
	return;
    }

    SET_BIT(obj->spectype, SITEM_HIGHLANDER);
    send_to_char("Ok.\r\n", ch);
    return;
}
