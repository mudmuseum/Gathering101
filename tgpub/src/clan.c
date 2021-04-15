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

void	adv_spell_damage	args( ( CHAR_DATA *ch, OBJ_DATA *book, 
				OBJ_DATA *page, char *argument) );
void	adv_spell_affect	args( ( CHAR_DATA *ch, OBJ_DATA *book, 
				OBJ_DATA *page, char *argument) );
void	adv_spell_action	args( ( CHAR_DATA *ch, OBJ_DATA *book, 
				OBJ_DATA *page, char *argument) );
void	birth_write		args( ( CHAR_DATA *ch, char *argument ) );
bool	birth_ok		args( ( CHAR_DATA *ch, char *argument ) );


void unclass ( CHAR_DATA *ch )
{
    int i;

    if ( IS_CLASS(ch,CLASS_DEMON) )
    {
	if (IS_DEMAFF(ch,DEM_TAIL) )
	    REMOVE_BIT(ch->pcdata->powers[DPOWER_CURRENT], DEM_TAIL);

	if (IS_DEMAFF(ch,DEM_HORNS) )
	    REMOVE_BIT(ch->pcdata->powers[DPOWER_CURRENT], DEM_HORNS);
	
	if (IS_DEMAFF(ch,DEM_HOOVES) )
	    REMOVE_BIT(ch->pcdata->powers[DPOWER_CURRENT], DEM_HOOVES);

	if (IS_DEMAFF(ch,DEM_WINGS) )
	{
	    if (IS_DEMAFF(ch,DEM_UNFOLDED) )
	        REMOVE_BIT(ch->pcdata->powers[DPOWER_CURRENT], DEM_UNFOLDED);
	    REMOVE_BIT(ch->pcdata->powers[DPOWER_CURRENT], DEM_WINGS);
	}
    }

    else if ( IS_CLASS(ch,CLASS_VAMPIRE) && IS_CLASS(ch, CLASS_VAMPIRE) )   
    {
	ch->pcdata->condition[COND_THIRST] = 10000;
	
	if ( IS_VAMPAFF(ch,VAM_DISGUISED) ) do_mask(ch,"self");
	if ( IS_POLYAFF(ch,POLY_SERPENT) ) do_serpent(ch,"");

	if ( IS_SET(ch->special, SPC_INCONNU) )
	    REMOVE_BIT(ch->special, SPC_INCONNU);

	ch->max_hit-=50;
	if (ch->max_hit < 1) ch->max_hit = 1;

	if (ch->beast > 99) ch->beast = 99;
    }

    else if ( IS_CLASS(ch, CLASS_WEREWOLF) && IS_CLASS(ch, CLASS_WEREWOLF) )
    {
	do_unwerewolf(ch,"");
	do_shift(ch,"homid");
    }

    if (IS_AFFECTED(ch, AFF_ETHEREAL))
    {
	send_to_char("Your form shimmers and solidifies.\r\n", ch);
	act("$N's form shimmers and solidifies.\r\n", ch, NULL, ch, TO_ROOM);
	REMOVE_BIT(ch->affected_by, AFF_ETHEREAL);
    }

    if (IS_AFFECTED(ch,AFF_SHADOWPLANE) )
    {
	send_to_char("You fade back into the real world.\n\r",ch);
	act("The shadows flicker and $N fades into existance.",ch,NULL,ch,TO_ROOM);
	REMOVE_BIT(ch->affected_by,AFF_SHADOWPLANE);
	do_look(ch,"auto");
    }

    if (IS_AFFECTED(ch,AFF_SHADOWSIGHT) )
    {
	 send_to_char("You can no longer see between planes.\n\r",ch);
	 REMOVE_BIT(ch->affected_by, AFF_SHADOWSIGHT);
    }
    if (IS_VAMPAFF(ch,VAM_FANGS) )
    {
	send_to_char("Your fangs slide back into your gums.\n\r",ch);
	act("$N's fangs slide back into $s gums.", ch, NULL, ch, TO_ROOM);
	REMOVE_BIT(ch->pcdata->stats[UNI_AFF], VAM_FANGS);
    }

    if (IS_VAMPAFF(ch,VAM_CLAWS) ) 
    {
	send_to_char("Your claws slide back under your nails.\n\r",ch);
	act("$N's claws slide back under $s nails.", ch, NULL, ch, TO_ROOM);
	REMOVE_BIT(ch->pcdata->stats[UNI_AFF], VAM_CLAWS);
    }

    if (IS_VAMPAFF(ch,VAM_NIGHTSIGHT) )
    {
	 send_to_char("The red glow in your eyes fades.\n\r",ch);
	 act("The red glow in $N's eyes fades.", ch, NULL, ch, TO_ROOM);
	 REMOVE_BIT(ch->pcdata->stats[UNI_AFF], VAM_NIGHTSIGHT);
    }

    if (IS_IMMUNE(ch,IMM_SHIELDED) )
    {
	send_to_char("You stop shielding your aura.\n\r",ch);
	REMOVE_BIT(ch->immune, IMM_SHIELDED);
    }
	
    if (IS_SET(ch->act, PLR_HOLYLIGHT) && !IS_IMMORTAL(ch) )
    {
	REMOVE_BIT(ch->act, PLR_HOLYLIGHT);
	send_to_char( "Your senses return to normal.\n\r", ch );
    }
    
    if (IS_SET(ch->act, PLR_WIZINVIS) && !IS_IMMORTAL(ch) )
    {
	REMOVE_BIT(ch->act, PLR_WIZINVIS);
	send_to_char("You fade into existence.\r\n", ch);
    }

    if (ch->level > LEVEL_AVATAR && !IS_IMMORTAL(ch) )
    {
	ch->level = LEVEL_AVATAR;
	ch->trust = 0;
    }

    ch->class = 0;
    free_string(ch->lord);
    ch->lord     = str_dup( "" );
    free_string(ch->clan);
    ch->clan     = str_dup( "" );

    for (i = 0; i < 20; i++)
	ch->pcdata->powers[i] = 0;
    for (i = 0; i < 12; i++)
	ch->pcdata->stats[i] = 0;

    check_stats(ch);

    ch->special = 0;

    ch->pcdata->timer[TIMER_OUTCAST] = 120;

    if ( get_room_index(ch->home) == NULL || 
	get_room_index(ch->home)->sector_type == SECT_HQ )
        ch->home = ROOM_VNUM_TEMPLE;

    return;
}

void do_vampire( CHAR_DATA *ch, char *argument )
{
    char      arg [MAX_INPUT_LENGTH];
    argument = one_argument( argument, arg );

    if (IS_NPC(ch)) return;

    if (!IS_IMMUNE(ch, IMM_VAMPIRE))
    {
	send_to_char("You will now allow vampires to bite you.\n\r",ch);
	SET_BIT(ch->immune, IMM_VAMPIRE);
	return;
    }
    send_to_char("You will no longer allow vampires to bite you.\n\r",ch);
    REMOVE_BIT(ch->immune, IMM_VAMPIRE);
    return;
}

void do_fangs( CHAR_DATA *ch, char *argument )
{
    char      arg [MAX_INPUT_LENGTH];

    argument = one_argument( argument, arg );

    if (IS_NPC(ch)) return;

    if (IS_CLASS(ch, CLASS_DEMON))
    {
	if (!IS_DEMPOWER( ch, DEM_FANGS))
	{
	    send_to_char("You haven't been granted the gift of fangs.\n\r",ch);
	    return;
	}
    }
    else if (IS_CLASS(ch, CLASS_WEREWOLF))
    {
	if (ch->pcdata->powers[WPOWER_WOLF] < 2)
	{
	    send_to_char("Huh?\n\r",ch);
	    return;
	}
    }
    else if (!IS_CLASS(ch, CLASS_VAMPIRE) )
    {
	send_to_char("Huh?\n\r",ch);
	return;
    }
    if (IS_CLASS(ch, CLASS_VAMPIRE) && ch->pcdata->stats[UNI_RAGE] > 0)
    {
	send_to_char("Your beast won't let you retract your fangs.\n\r",ch);
	return;
    }

    if (IS_VAMPAFF(ch,VAM_FANGS) )
    {
	send_to_char("Your fangs slide back into your gums.\n\r",ch);
	act("$n's fangs slide back into $s gums.", ch, NULL, NULL, TO_ROOM);
	REMOVE_BIT(ch->pcdata->stats[UNI_AFF], VAM_FANGS);
	return;
    }
    send_to_char("Your fangs extend out of your gums.\n\r",ch);
    act("A pair of razor sharp fangs extend from $n's mouth.", ch, NULL, NULL, TO_ROOM);
    SET_BIT(ch->pcdata->stats[UNI_AFF], VAM_FANGS);
    return;
}

void do_shift( CHAR_DATA *ch, char *argument )
{
    char arg [MAX_INPUT_LENGTH];
    int  toform = 0;

    argument = one_argument( argument, arg );

    if (IS_NPC(ch)) return;

    if (!IS_CLASS(ch, CLASS_WEREWOLF))
    {
	send_to_char("Huh?\n\r",ch);
	return;
    }

    if ( arg[0] == '\0' )
    {
	send_to_char( "You can shift between the following forms: Homid, Glabro, Crinos, Hispo, Lupus.\n\r", ch );
	return;
    }
    if      ( !str_cmp(arg,"homid" ) ) toform = 1;
    else if ( !str_cmp(arg,"glabro") ) toform = 2;
    else if ( !str_cmp(arg,"crinos") ) toform = 3;
    else if ( !str_cmp(arg,"hispo" ) ) toform = 4;
    else if ( !str_cmp(arg,"lupus" ) ) toform = 5;
    else
    {
	send_to_char( "You can shift between the following forms: Homid, Glabro, Crinos, Hispo, Lupus.\n\r", ch );
	return;
    }
    if ( ch->pcdata->stats[UNI_FORM0] < 1 || ch->pcdata->stats[UNI_FORM0] > 5) 
	ch->pcdata->stats[UNI_FORM0] = 1;
    if ( ch->pcdata->stats[UNI_FORM0] == toform )
    {
	send_to_char( "You are already in that form.\n\r", ch );
	return;
    }
    ch->pcdata->stats[UNI_FORM1] = toform;
    return;
}

void do_claws( CHAR_DATA *ch, char *argument )
{
    char      arg [MAX_INPUT_LENGTH];

    argument = one_argument( argument, arg );

    if (IS_NPC(ch)) return;

    if (IS_CLASS(ch, CLASS_DEMON))
    {
	if (!IS_DEMPOWER( ch, DEM_CLAWS))
	{
	    send_to_char("You haven't been granted the gift of claws.\n\r",ch);
	    return;
	}
    }
    else if (IS_CLASS(ch, CLASS_WEREWOLF))
    {
	if (ch->pcdata->powers[WPOWER_WOLF] < 1)
	{
	    send_to_char("Huh?\n\r",ch);
	    return;
	}
    }
    else if (!IS_CLASS(ch, CLASS_VAMPIRE) )
    {
	send_to_char("Huh?\n\r",ch);
	return;
    }
    if (IS_CLASS(ch, CLASS_VAMPIRE) && !IS_VAMPAFF(ch,VAM_PROTEAN) )
    {
	send_to_char("You are not trained in the Protean discipline.\n\r",ch);
	return;
    }

    if (IS_CLASS(ch, CLASS_VAMPIRE) && ch->pcdata->stats[UNI_RAGE] > 0)
    {
	send_to_char("Your beast won't let you retract your claws.\n\r",ch);
	return;
    }
    if (IS_CLASS(ch, CLASS_WEREWOLF) && ch->pcdata->stats[UNI_RAGE] > 0)
    {
	send_to_char("Your rage won't let you retract your claws.\n\r",ch);
	return;
    }

    if (IS_VAMPAFF(ch,VAM_CLAWS) )
    {
	if (IS_CLASS(ch, CLASS_VAMPIRE)) 
	{
	    send_to_char("Your claws slide back under your nails.\n\r",ch);
	    act("$n's claws slide back under $s nails.", ch, NULL, NULL, TO_ROOM);
	}
	else
	{
	    send_to_char("Your talons slide back into your fingers.\n\r",ch);
	    act("$n's talons slide back into $s fingers.",ch,NULL,NULL,TO_ROOM);
	}
	REMOVE_BIT(ch->pcdata->stats[UNI_AFF], VAM_CLAWS);
	return;
    }

    if (IS_CLASS(ch, CLASS_VAMPIRE)) 
    {
	send_to_char("Sharp claws extend from under your finger nails.\n\r",ch);
	act("Sharp claws extend from under $n's finger nails.", ch, NULL, NULL, TO_ROOM);
    }
    else
    {
	send_to_char("Razor sharp talons extend from your fingers.\n\r",ch);
	act("Razor sharp talons extend from $n's fingers.",ch,NULL,NULL,TO_ROOM);
    }
    SET_BIT(ch->pcdata->stats[UNI_AFF], VAM_CLAWS);
    return;
}

void do_nightsight( CHAR_DATA *ch, char *argument )
{
    char      arg [MAX_INPUT_LENGTH];

    argument = one_argument( argument, arg );

    if (IS_NPC(ch)) return;

    if (IS_CLASS(ch, CLASS_DEMON))
    {
	if (!IS_DEMPOWER( ch, DEM_EYES))
	{
	    send_to_char("You haven't been granted the gift of nightsight.\n\r",ch);
	    return;
	}
    }
    else if (IS_CLASS(ch, CLASS_WEREWOLF))
    {
	if (ch->pcdata->powers[WPOWER_HAWK] < 1)
	{
	    send_to_char("Huh?\n\r",ch);
	    return;
	}
    }
    else if (!IS_CLASS(ch, CLASS_VAMPIRE) )
    {
	send_to_char("Huh?\n\r",ch);
	return;
    }
    if (!IS_VAMPAFF(ch,VAM_PROTEAN) && !IS_VAMPAFF(ch,VAM_OBTENEBRATION)
	&& !IS_VAMPAFF(ch,VAM_SERPENTIS) && IS_CLASS(ch, CLASS_VAMPIRE))
    {
	send_to_char("You are not trained in the correct disciplines.\n\r",ch);
	return;
    }
    if (IS_CLASS(ch, CLASS_VAMPIRE) && ch->pcdata->stats[UNI_RAGE] > 0)
    {
	send_to_char("Not while your beast is in control.\n\r",ch);
	return;
    }

    if (IS_VAMPAFF(ch,VAM_NIGHTSIGHT) )
    {
	send_to_char("The red glow in your eyes fades.\n\r",ch);
	act("The red glow in $n's eyes fades.", ch, NULL, NULL, TO_ROOM);
	REMOVE_BIT(ch->pcdata->stats[UNI_AFF], VAM_NIGHTSIGHT);
	return;
    }
    send_to_char("Your eyes start glowing red.\n\r",ch);
    act("$n's eyes start glowing red.", ch, NULL, NULL, TO_ROOM);
    SET_BIT(ch->pcdata->stats[UNI_AFF], VAM_NIGHTSIGHT);
    return;
}

void do_shadowsight( CHAR_DATA *ch, char *argument )
{
    char      arg [MAX_INPUT_LENGTH];

    argument = one_argument( argument, arg );

    if (IS_NPC(ch)) return;

    if (IS_CLASS(ch, CLASS_DEMON))
    {
	if (!IS_DEMPOWER( ch, DEM_SHADOWSIGHT))
	{
	    send_to_char("You haven't been granted the gift of shadowsight.\n\r",ch);
	    return;
	}
    }
    else if (!IS_CLASS(ch, CLASS_WEREWOLF) || ch->pcdata->powers[WPOWER_HAWK] < 2)
    {
	if (!IS_CLASS(ch, CLASS_VAMPIRE))
	{
	    send_to_char("Huh?\n\r",ch);
	    return;
	}
	if (!IS_VAMPAFF(ch,VAM_OBTENEBRATION) )
	{
	    send_to_char("You are not trained in the Obtenebration discipline.\n\r",ch);
	    return;
	}
    }
    if (IS_AFFECTED(ch,AFF_SHADOWSIGHT) )
    {
	send_to_char("You can no longer see between planes.\n\r",ch);
	REMOVE_BIT(ch->affected_by, AFF_SHADOWSIGHT);
	return;
    }
    if (IS_CLASS(ch, CLASS_VAMPIRE) && ch->pcdata->condition[COND_THIRST] < 10)
    {
	send_to_char("You have insufficient blood.\n\r",ch);
	return;
    }
    if (IS_CLASS(ch, CLASS_VAMPIRE))
	ch->pcdata->condition[COND_THIRST] -= number_range(5,10);
    send_to_char("You can now see between planes.\n\r",ch);
    SET_BIT(ch->affected_by, AFF_SHADOWSIGHT);
    return;
}

void do_clanname( CHAR_DATA *ch, char *argument )
{
    CHAR_DATA *victim;
    char      arg [MAX_INPUT_LENGTH];
    argument = one_argument( argument, arg );
    if ( IS_NPC(ch) ) return;

    if ( (!IS_CLASS(ch,CLASS_VAMPIRE) || ch->pcdata->stats[UNI_GEN] != 1) && 
	(!IS_CLASS(ch,CLASS_WEREWOLF) || ch->pcdata->stats[UNI_GEN] != 1))
    {
	send_to_char( "Huh?\n\r", ch );
	return;
    }

    if ( arg[0] == '\0' )
    {
	if (IS_CLASS(ch, CLASS_VAMPIRE))
	    send_to_char( "Who's clan do you wish to name?\n\r", ch );
	else
	    send_to_char( "Who do you wish to give a tribe to?\n\r", ch );
	return;
    }

    if ( ( victim = get_char_room( ch, arg ) ) == NULL )
    {
	send_to_char( "They aren't here.\n\r", ch );
	return;
    }
    if ( IS_NPC(victim) ) return;
    if ( IS_CLASS(ch, CLASS_WEREWOLF))
    {
	if ( victim->pcdata->stats[UNI_GEN] != 2)
	{
	    send_to_char( "Only greater werewolves may own a tribe.\n\r", ch );
	    return;
	}
    }
    else if ( victim->pcdata->stats[UNI_GEN] != 2 )
    {
	if (IS_CLASS(ch, CLASS_VAMPIRE))
	    send_to_char( "Only the Antediluvians may have clans.\n\r", ch );
	return;
    }
    if ( str_cmp(victim->clan,"") )
    {
	send_to_char( "But they already have a clan!\n\r", ch );
	return;
    }
    smash_tilde( argument );
    if ( strlen(argument) < 3 || strlen(argument) > 13 )
    {
	send_to_char( "Clan name should be between 3 and 13 letters long.\n\r", ch );
	return;
    }
    free_string( victim->clan );
    victim->clan = str_dup( argument );
    if (IS_CLASS(ch, CLASS_VAMPIRE))
    {
    	/*clan_table_namefill(victim);*/
    	send_to_char( "Clan name set.\n\r", ch );
    }
    else
    	send_to_char( "Tribe name set.\n\r", ch );
    return;
}

void do_shadowplane( CHAR_DATA *ch, char *argument )
{
    OBJ_DATA *obj;
    char      arg [MAX_INPUT_LENGTH];
    argument = one_argument( argument, arg );

    if (IS_NPC(ch)) return;

    if (!IS_CLASS(ch, CLASS_WEREWOLF) || ch->pcdata->powers[WPOWER_OWL] < 3)
    {
	if (!IS_CLASS(ch, CLASS_VAMPIRE))
	{
	    send_to_char("Huh?\n\r",ch);
	    return;
	}
	if (!IS_VAMPAFF(ch,VAM_OBTENEBRATION) )
	{
	    send_to_char("You are not trained in the Obtenebration discipline.\n\r",ch);
	    return;
	}
    }
    if (IS_CLASS(ch, CLASS_VAMPIRE) && ch->pcdata->condition[COND_THIRST] < 75)
    {
	send_to_char("You have insufficient blood.\n\r",ch);
	return;
    }
    
    if (IS_CLASS(ch, CLASS_VAMPIRE))
	ch->pcdata->condition[COND_THIRST] -= number_range(65,75);

    if ( arg[0] == '\0' )
    {
    	if (!IS_AFFECTED(ch, AFF_SHADOWPLANE))
    	{
	    send_to_char("You fade into the plane of shadows.\n\r",ch);
	    act("The shadows flicker and swallow up $n.",ch,NULL,NULL,TO_ROOM);
	    SET_BIT(ch->affected_by, AFF_SHADOWPLANE);
	    do_look(ch,"auto");
	    return;
    	}
    	REMOVE_BIT(ch->affected_by, AFF_SHADOWPLANE);
    	send_to_char("You fade back into the real world.\n\r",ch);
	act("The shadows flicker and $n fades into existance.",ch,NULL,NULL,TO_ROOM);
	do_look(ch,"auto");
	return;
    }

    if ( ( obj = get_obj_here( ch, arg ) ) == NULL )
    {
	send_to_char( "What do you wish to toss into the shadow plane?\n\r", ch );
	return;
    }

    if (IS_AFFECTED(ch, AFF_SHADOWPLANE))
    	send_to_char( "You toss it to the ground and it vanishes.\n\r", ch );
    else
    	send_to_char( "You toss it into a shadow and it vanishes.\n\r", ch );
    return;
}

void do_introduce( CHAR_DATA *ch, char *argument )
{
    char      arg [MAX_INPUT_LENGTH];
    argument = one_argument( argument, arg );

    if (IS_NPC(ch)) return;

    if (IS_CLASS(ch, CLASS_VAMPIRE) && ch->pcdata->stats[UNI_GEN] > 0 && ch->pcdata->stats[UNI_GEN] < 8)
	do_tradition(ch,ch->lord);
    else if (IS_CLASS(ch, CLASS_WEREWOLF) && IS_HERO(ch) &&
	ch->pcdata->stats[UNI_GEN] > 0)
	do_bloodline(ch,ch->lord);
    else send_to_char("Huh?\n\r",ch);
    return;
}

void do_truesight( CHAR_DATA *ch, char *argument )
{
    if ( IS_NPC(ch) )
	return;

    if (!IS_CLASS(ch, CLASS_WEREWOLF) && !IS_CLASS(ch, CLASS_VAMPIRE) &&
	!IS_CLASS(ch, CLASS_HIGHLANDER) )
    {
	send_to_char("Huh?\r\n", ch);
	return;
    }

    if (IS_CLASS(ch, CLASS_WEREWOLF) && ch->pcdata->powers[WPOWER_HAWK] < 3)
    {
	send_to_char("You are not that skilled in the Hawk totem.\r\n", ch);
	return;
    }

    if (IS_CLASS(ch, CLASS_VAMPIRE) && !IS_VAMPAFF(ch,VAM_AUSPEX) )
    {
	send_to_char("You are not trained in the Auspex discipline.\n\r",ch);
	return;
    }

    if (IS_CLASS(ch, CLASS_HIGHLANDER) && ch->pcdata->powers[HPOWER_TRAINED] < 1)
    {
	send_to_char("You are not that skilled yet.\r\n", ch);
	return;
    }
  
    if ( IS_SET(ch->act, PLR_HOLYLIGHT) )
    {
	REMOVE_BIT(ch->act, PLR_HOLYLIGHT);
	send_to_char( "Your senses return to normal.\n\r", ch );
    }
    else
    {
	SET_BIT(ch->act, PLR_HOLYLIGHT);
	send_to_char( "Your senses increase to incredible proportions.\n\r", ch );
    }

    return;
}

void do_scry( CHAR_DATA *ch, char *argument )
{
    CHAR_DATA *victim;
    ROOM_INDEX_DATA *chroom;
    ROOM_INDEX_DATA *victimroom;
    char      arg [MAX_INPUT_LENGTH];

    argument = one_argument( argument, arg );

    if (IS_NPC(ch)) return;

    if (IS_CLASS(ch, CLASS_DEMON))
    {
	if (!IS_DEMPOWER(ch, DEM_SCRY))
	{
	    send_to_char("You haven't been granted the gift of scry.\n\r",ch);
	    return;
	}
    }
    else if (!IS_CLASS(ch, CLASS_VAMPIRE) && !IS_ITEMAFF(ch, ITEMA_VISION))
    {
	send_to_char( "Huh?\n\r", ch );
	return;
    }

    if (!IS_CLASS(ch, CLASS_DEMON) && !IS_VAMPAFF(ch,VAM_AUSPEX) 
	&& !IS_ITEMAFF(ch, ITEMA_VISION))
    {
	send_to_char("You are not trained in the Auspex discipline.\n\r",ch);
	return;
    }

    if ( arg[0] == '\0' )
    {
	send_to_char( "Scry on whom?\n\r", ch );
	return;
    }

    if ( ( victim = get_char_world( ch, arg ) ) == NULL )
    {
	send_to_char( "They aren't here.\n\r", ch );
	return;
    }

    if (!IS_CLASS(ch, CLASS_DEMON) && 
	ch->pcdata->condition[COND_THIRST] < 25 && !IS_ITEMAFF(ch, ITEMA_VISION))
    {
	send_to_char("You have insufficient blood.\n\r",ch);
	return;
    }
    if (!IS_ITEMAFF(ch, ITEMA_VISION) && IS_CLASS(ch, CLASS_VAMPIRE))
	ch->pcdata->condition[COND_THIRST] -= number_range(15,25);

    	if (!IS_NPC(victim) 
	&& IS_IMMUNE(victim,IMM_SHIELDED) 
	&& !IS_ITEMAFF(ch, ITEMA_VISION) 
	&& (IS_CLASS(victim, CLASS_VAMPIRE) || IS_CLASS(victim, CLASS_WEREWOLF)))
    {
	send_to_char("You are unable to locate them.\n\r",ch);
	return;
    }

    if (IS_NPC(victim) && victim->in_room->sector_type == SECT_HQ)
    {
	send_to_char("They are protected under Clan Headquarter law.\r\n", ch);
	return;
    }

    chroom = ch->in_room;
    victimroom = victim->in_room;

    char_from_room(ch);
    char_to_room(ch,victimroom);
    if (IS_AFFECTED(ch, AFF_SHADOWPLANE) && (!IS_AFFECTED(victim, AFF_SHADOWPLANE)))
    {
	REMOVE_BIT(ch->affected_by, AFF_SHADOWPLANE);
	do_look(ch,"auto");
	SET_BIT(ch->affected_by, AFF_SHADOWPLANE);
    }
    else if (!IS_AFFECTED(ch, AFF_SHADOWPLANE) && (IS_AFFECTED(victim, AFF_SHADOWPLANE)))
    {
	SET_BIT(ch->affected_by, AFF_SHADOWPLANE);
	do_look(ch,"auto");
	REMOVE_BIT(ch->affected_by, AFF_SHADOWPLANE);
    }
    else
	do_look(ch,"auto");
    char_from_room(ch);
    char_to_room(ch,chroom);

    return;
}

void do_readaura( CHAR_DATA *ch, char *argument )
{
    CHAR_DATA *victim;
    OBJ_DATA  *obj;
    char      arg [MAX_INPUT_LENGTH];
    char      buf [MAX_INPUT_LENGTH];

    argument = one_argument( argument, arg );

    if (IS_NPC(ch)) return;

    if (!IS_CLASS(ch, CLASS_VAMPIRE) && !IS_ITEMAFF(ch, ITEMA_VISION))
    {
	send_to_char( "Huh?\n\r", ch );
	return;
    }

    if (!IS_VAMPAFF(ch,VAM_AUSPEX) && !IS_ITEMAFF(ch, ITEMA_VISION))
    {
	send_to_char("You are not trained in the Auspex discipline.\n\r",ch);
	return;
    }

    if ( arg[0] == '\0' )
    {
	send_to_char( "Read the aura on what?\n\r", ch );
	return;
    }

    if ( ( victim = get_char_room( ch, arg ) ) == NULL )
    {
        if ( ( obj = get_obj_carry( ch, arg ) ) == NULL )
	{
	    send_to_char( "Read the aura on what?\n\r", ch );
	    return;
	}
    	if ( ch->pcdata->condition[COND_THIRST] < 50 && !IS_ITEMAFF(ch, ITEMA_VISION))
    	{
	    send_to_char("You have insufficient blood.\n\r",ch);
	    return;
    	}
	if (!IS_ITEMAFF(ch, ITEMA_VISION))
	    ch->pcdata->condition[COND_THIRST] -= number_range(40,50);
	act("$n examines $p intently.",ch,obj,NULL,TO_ROOM);
	spell_identify( skill_lookup( "identify" ), ch->level, ch, obj );
	return;
    }

    if (ch->pcdata->condition[COND_THIRST] < 50 && !IS_ITEMAFF(ch,ITEMA_VISION))
    {
	send_to_char("You have insufficient blood.\n\r",ch);
	return;
    }
    if (!IS_ITEMAFF(ch, ITEMA_VISION))
	ch->pcdata->condition[COND_THIRST] -= number_range(40,50);

    	if (!IS_NPC(victim) 
	&& IS_IMMUNE(victim,IMM_SHIELDED) 
	&& !IS_ITEMAFF(ch, ITEMA_VISION) 
	&& (IS_CLASS(victim, CLASS_VAMPIRE) || IS_CLASS(victim, CLASS_WEREWOLF)))

    {
	send_to_char("You are unable to read their aura.\n\r",ch);
	return;
    }

    act("$n examines $N intently.",ch,NULL,victim,TO_NOTVICT);
    act("$n examines you intently.",ch,NULL,victim,TO_VICT);
    if (IS_NPC(victim)) sprintf(buf, "%s is an NPC.\n\r",victim->short_descr);
    else 
    {
	if      (victim->level == 12) sprintf(buf, "%s is an Implementor.\n\r", victim->name);
	else if (victim->level == 11) sprintf(buf, "%s is a High Judge.\n\r", victim->name);
	else if (victim->level == 10) sprintf(buf, "%s is a Judge.\n\r", victim->name);
	else if (victim->level == 9 ) sprintf(buf, "%s is an Enforcer.\n\r", victim->name);
	else if (victim->level == 8 ) sprintf(buf, "%s is a Quest Maker.\n\r", victim->name);
	else if (victim->level == 7 ) sprintf(buf, "%s is a Newbie Helper.\n\r", victim->name);
	else if (victim->level >= 3 ) sprintf(buf, "%s is an Avatar.\n\r", victim->name);
	else sprintf(buf, "%s is a Mortal.\n\r", victim->name);
    }
    send_to_char(buf,ch);
    if (!IS_NPC(victim))
    {
	sprintf(buf,"Str:%d, Int:%d, Wis:%d, Dex:%d, Con:%d.\n\r",get_curr_str(victim),get_curr_int(victim),get_curr_wis(victim),get_curr_dex(victim),get_curr_con(victim));
	send_to_char(buf,ch);
    }
    sprintf(buf,"Hp:%d/%d, Mana:%d/%d, Move:%d/%d.\n\r",victim->hit,victim->max_hit,victim->mana,victim->max_mana,victim->move,victim->max_move);
    send_to_char(buf,ch);
    if (!IS_NPC(victim)) sprintf(buf,"Hitroll:%d, Damroll:%d, AC:%d.\n\r",char_hitroll(victim),char_damroll(victim),char_ac(victim));
    else sprintf(buf,"AC:%d.\n\r",char_ac(victim));
    send_to_char(buf,ch);
    if (!IS_NPC(victim))
    {
	sprintf(buf,"Status:%d, ",victim->race);
	send_to_char(buf,ch);
	if (IS_CLASS(victim, CLASS_VAMPIRE))    
	{
	    sprintf(buf,"Blood:%d, ",victim->pcdata->condition[COND_THIRST]);
	    send_to_char(buf,ch);
	}
    }
    sprintf(buf,"Alignment:%d.\n\r",victim->alignment);
    send_to_char(buf,ch);
    if (!IS_NPC(victim) && IS_EXTRA(victim, EXTRA_PREGNANT))
	act("$N is pregnant.",ch,NULL,victim,TO_CHAR);
    if (!IS_NPC(victim) && IS_CLASS(victim, CLASS_VAMPIRE))    
    {
	send_to_char("Disciplines:",ch);
	if (IS_VAMPAFF(victim, VAM_PROTEAN)) send_to_char(" Protean",ch);
	if (IS_VAMPAFF(victim, VAM_CELERITY)) send_to_char(" Celerity",ch);
	if (IS_VAMPAFF(victim, VAM_FORTITUDE)) send_to_char(" Fortitude",ch);
	if (IS_VAMPAFF(victim, VAM_POTENCE)) send_to_char(" Potence",ch);
	if (IS_VAMPAFF(victim, VAM_OBFUSCATE)) send_to_char(" Obfuscate",ch);
	if (IS_VAMPAFF(victim, VAM_OBTENEBRATION)) send_to_char(" Obtenebration",ch);
	if (IS_VAMPAFF(victim, VAM_SERPENTIS)) send_to_char(" Serpentis",ch);
	if (IS_VAMPAFF(victim, VAM_AUSPEX)) send_to_char(" Auspex",ch);
	if (IS_VAMPAFF(victim, VAM_DOMINATE)) send_to_char(" Dominate",ch);
	if (IS_VAMPAFF(victim, VAM_PRESENCE)) send_to_char(" Presence",ch);
	send_to_char(".\n\r",ch);
    }
    return;
}

void do_shield( CHAR_DATA *ch, char *argument )
{
    char      arg [MAX_INPUT_LENGTH];

    argument = one_argument( argument, arg );

    if (IS_NPC(ch)) return;
    if (!IS_CLASS(ch, CLASS_WEREWOLF) || ch->pcdata->powers[WPOWER_OWL] < 2)
    {
	if (!IS_CLASS(ch, CLASS_VAMPIRE))
	{
	    send_to_char("Huh?\n\r",ch);
	    return;
	}
    }
    if (!IS_VAMPAFF(ch,VAM_OBFUSCATE) && !IS_VAMPAFF(ch,VAM_DOMINATE) && 
	!IS_CLASS(ch, CLASS_WEREWOLF) )
    {
   	send_to_char("You are not trained in the Obfuscate or Dominate disciplines.\n\r",ch);
	return;
    }
    if (IS_CLASS(ch, CLASS_VAMPIRE) && ch->pcdata->condition[COND_THIRST] < 60)
    {
	send_to_char("You have insufficient blood.\n\r",ch);
	return;
    }
    if (IS_CLASS(ch, CLASS_VAMPIRE))
	ch->pcdata->condition[COND_THIRST] -= number_range(50,60);
    if (!IS_IMMUNE(ch,IMM_SHIELDED) )
    {
    	send_to_char("You shield your aura from those around you.\n\r",ch);
    	SET_BIT(ch->immune, IMM_SHIELDED);
	return;
    }
    send_to_char("You stop shielding your aura.\n\r",ch);
    REMOVE_BIT(ch->immune, IMM_SHIELDED);
    return;
}

/*
 * Standard class regen function, not just werewolves.
 */
void werewolf_regen( CHAR_DATA *ch, int multiplier )
{
    int min = 5;
    int max = 10;
    
    if (IS_NPC(ch)) return;

    if ( ch->hit >= ch->max_hit && ch->mana >= ch->max_mana &&
	 ch->move >= ch->max_move )
	return;
    
    if (multiplier < 1) multiplier = 1;

    if (ch->hit < 1 ) 
    {
	ch->hit = ch->hit + multiplier * number_range(1,3);
	update_pos(ch);
    }
    else
    {
	min += 10 - ch->pcdata->stats[UNI_GEN];
	max += 20 - (ch->pcdata->stats[UNI_GEN] * 2);
	if (IS_CLASS(ch,CLASS_DEMON) && ch->in_room->vnum == 30000)
	{
	min = min * 2.5;
	max = max * 2.5;
	}
    	ch->hit = UMIN
		(ch->hit + multiplier * number_range(min,max), ch->max_hit);
    	ch->mana = UMIN
		(ch->mana + multiplier * number_range(min,max), ch->max_mana);
    	ch->move = UMIN
		(ch->move + multiplier * number_range(min,max), ch->max_move);

	if ( ch->hit >= ch->max_hit && ch->mana >= ch->max_mana && ch->move >= ch->max_move )
	    send_to_char("Your body has completely regenerated itself.\n\r",ch);
    }
    return;
}

void reg_mend( CHAR_DATA *ch )
{
    int ribs = 0;
    int teeth = 0;

    if ((ch->loc_hp[0] + ch->loc_hp[1] + ch->loc_hp[2] + ch->loc_hp[3] + 
	    ch->loc_hp[4] + ch->loc_hp[5]) == 0) 
	return;

    if (IS_BODY(ch,BROKEN_RIBS_1 )) ribs += 1;
    if (IS_BODY(ch,BROKEN_RIBS_2 )) ribs += 2;
    if (IS_BODY(ch,BROKEN_RIBS_4 )) ribs += 4;
    if (IS_BODY(ch,BROKEN_RIBS_8 )) ribs += 8;
    if (IS_BODY(ch,BROKEN_RIBS_16)) ribs += 16;
    if (IS_HEAD(ch,LOST_TOOTH_1  )) teeth += 1;
    if (IS_HEAD(ch,LOST_TOOTH_2  )) teeth += 2;
    if (IS_HEAD(ch,LOST_TOOTH_4  )) teeth += 4;
    if (IS_HEAD(ch,LOST_TOOTH_8  )) teeth += 8;
    if (IS_HEAD(ch,LOST_TOOTH_16 )) teeth += 16;

    if (ribs > 0)
    {
    	if (IS_BODY(ch,BROKEN_RIBS_1 ))
	    REMOVE_BIT(ch->loc_hp[1],BROKEN_RIBS_1);
    	if (IS_BODY(ch,BROKEN_RIBS_2 ))
	    REMOVE_BIT(ch->loc_hp[1],BROKEN_RIBS_2);
	if (IS_BODY(ch,BROKEN_RIBS_4 ))
	    REMOVE_BIT(ch->loc_hp[1],BROKEN_RIBS_4);
	if (IS_BODY(ch,BROKEN_RIBS_8 ))
	    REMOVE_BIT(ch->loc_hp[1],BROKEN_RIBS_8);
	if (IS_BODY(ch,BROKEN_RIBS_16))
	    REMOVE_BIT(ch->loc_hp[1],BROKEN_RIBS_16);
	ribs -= 1;
	if (ribs >= 16) {ribs -= 16;
	    SET_BIT(ch->loc_hp[1],BROKEN_RIBS_16);}
	if (ribs >= 8 ) {ribs -= 8;
	    SET_BIT(ch->loc_hp[1],BROKEN_RIBS_8);}
	if (ribs >= 4 ) {ribs -= 4;
	    SET_BIT(ch->loc_hp[1],BROKEN_RIBS_4);}
	if (ribs >= 2 ) {ribs -= 2;
	    SET_BIT(ch->loc_hp[1],BROKEN_RIBS_2);}
	if (ribs >= 1 ) {ribs -= 1;
	    SET_BIT(ch->loc_hp[1],BROKEN_RIBS_1);}
	act("One of $n's ribs snap back into place.",ch,NULL,NULL,TO_ROOM);
	act("One of your ribs snap back into place.",ch,NULL,NULL,TO_CHAR);
    }
    else if (IS_HEAD(ch,LOST_EYE_L))
    {
	act("An eyeball appears in $n's left eye socket.",ch,NULL,NULL,TO_ROOM);
	act("An eyeball appears in your left eye socket.",ch,NULL,NULL,TO_CHAR);
	REMOVE_BIT(ch->loc_hp[LOC_HEAD],LOST_EYE_L);
    }
    else if (IS_HEAD(ch,LOST_EYE_R))
    {
	act("An eyeball appears in $n's right eye socket.",ch,NULL,NULL,TO_ROOM);
	act("An eyeball appears in your right eye socket.",ch,NULL,NULL,TO_CHAR);
	REMOVE_BIT(ch->loc_hp[LOC_HEAD],LOST_EYE_R);
    }
    else if (IS_HEAD(ch,LOST_EAR_L))
    {
	act("An ear grows on the left side of $n's head.",ch,NULL,NULL,TO_ROOM);
	act("An ear grows on the left side of your head.",ch,NULL,NULL,TO_CHAR);
	REMOVE_BIT(ch->loc_hp[LOC_HEAD],LOST_EAR_L);
    }
    else if (IS_HEAD(ch,LOST_EAR_R))
    {
	act("An ear grows on the right side of $n's head.",ch,NULL,NULL,TO_ROOM);
	act("An ear grows on the right side of your head.",ch,NULL,NULL,TO_CHAR);
	REMOVE_BIT(ch->loc_hp[LOC_HEAD],LOST_EAR_R);
    }
    else if (IS_HEAD(ch,LOST_NOSE))
    {
	act("A nose grows on the front of $n's face.",ch,NULL,NULL,TO_ROOM);
	act("A nose grows on the front of your face.",ch,NULL,NULL,TO_CHAR);
	REMOVE_BIT(ch->loc_hp[LOC_HEAD],LOST_NOSE);
	REMOVE_BIT(ch->loc_hp[LOC_HEAD],BROKEN_NOSE);
    }
    else if (teeth > 0)
    {
    	if (IS_HEAD(ch,LOST_TOOTH_1 ))
	    REMOVE_BIT(ch->loc_hp[LOC_HEAD],LOST_TOOTH_1);
    	if (IS_HEAD(ch,LOST_TOOTH_2 ))
	    REMOVE_BIT(ch->loc_hp[LOC_HEAD],LOST_TOOTH_2);
    	if (IS_HEAD(ch,LOST_TOOTH_4 ))
	    REMOVE_BIT(ch->loc_hp[LOC_HEAD],LOST_TOOTH_4);
    	if (IS_HEAD(ch,LOST_TOOTH_8 ))
	    REMOVE_BIT(ch->loc_hp[LOC_HEAD],LOST_TOOTH_8);
    	if (IS_HEAD(ch,LOST_TOOTH_16))
	    REMOVE_BIT(ch->loc_hp[LOC_HEAD],LOST_TOOTH_16);
	teeth -= 1;
	if (teeth >= 16) {teeth -= 16;
	    SET_BIT(ch->loc_hp[LOC_HEAD],LOST_TOOTH_16);}
	if (teeth >= 8 ) {teeth -= 8;
	    SET_BIT(ch->loc_hp[LOC_HEAD],LOST_TOOTH_8);}
	if (teeth >= 4 ) {teeth -= 4;
	    SET_BIT(ch->loc_hp[LOC_HEAD],LOST_TOOTH_4);}
	if (teeth >= 2 ) {teeth -= 2;
	    SET_BIT(ch->loc_hp[LOC_HEAD],LOST_TOOTH_2);}
	if (teeth >= 1 ) {teeth -= 1;
	    SET_BIT(ch->loc_hp[LOC_HEAD],LOST_TOOTH_1);}
	act("A missing tooth grows in your mouth.",ch,NULL,NULL,TO_CHAR);
	act("A missing tooth grows in $n's mouth.",ch,NULL,NULL,TO_ROOM);
    }
    else if (IS_HEAD(ch,BROKEN_NOSE))
    {
	act("$n's nose snaps back into place.",ch,NULL,NULL,TO_ROOM);
	act("Your nose snaps back into place.",ch,NULL,NULL,TO_CHAR);
	REMOVE_BIT(ch->loc_hp[LOC_HEAD],BROKEN_NOSE);
    }
    else if (IS_HEAD(ch,BROKEN_JAW))
    {
	act("$n's jaw snaps back into place.",ch,NULL,NULL,TO_ROOM);
	act("Your jaw snaps back into place.",ch,NULL,NULL,TO_CHAR);
	REMOVE_BIT(ch->loc_hp[LOC_HEAD],BROKEN_JAW);
    }
    else if (IS_HEAD(ch,BROKEN_SKULL))
    {
	act("$n's skull knits itself back together.",ch,NULL,NULL,TO_ROOM);
	act("Your skull knits itself back together.",ch,NULL,NULL,TO_CHAR);
	REMOVE_BIT(ch->loc_hp[LOC_HEAD],BROKEN_SKULL);
    }
    else if (IS_BODY(ch,BROKEN_SPINE))
    {
	act("$n's spine knits itself back together.",ch,NULL,NULL,TO_ROOM);
	act("Your spine knits itself back together.",ch,NULL,NULL,TO_CHAR);
	REMOVE_BIT(ch->loc_hp[LOC_BODY],BROKEN_SPINE);
    }
    else if (IS_BODY(ch,BROKEN_NECK))
    {
	act("$n's neck snaps back into place.",ch,NULL,NULL,TO_ROOM);
	act("Your neck snaps back into place.",ch,NULL,NULL,TO_CHAR);
	REMOVE_BIT(ch->loc_hp[LOC_BODY],BROKEN_NECK);
    }
    else if (IS_ARM_L(ch,LOST_ARM))
    {
	act("An arm grows from the stump of $n's left shoulder.",ch,NULL,NULL,TO_ROOM);
	act("An arm grows from the stump of your left shoulder.",ch,NULL,NULL,TO_CHAR);
	REMOVE_BIT(ch->loc_hp[LOC_ARM_L],LOST_ARM);
	REMOVE_BIT(ch->loc_hp[LOC_ARM_L],BROKEN_ARM);
	SET_BIT(ch->loc_hp[LOC_ARM_L],LOST_HAND);
    }
    else if (IS_ARM_R(ch,LOST_ARM))
    {
	act("An arm grows from the stump of $n's right shoulder.",ch,NULL,NULL,TO_ROOM);
	act("An arm grows from the stump of your right shoulder.",ch,NULL,NULL,TO_CHAR);
	REMOVE_BIT(ch->loc_hp[LOC_ARM_R],LOST_ARM);
	REMOVE_BIT(ch->loc_hp[LOC_ARM_R],BROKEN_ARM);
	SET_BIT(ch->loc_hp[LOC_ARM_R],LOST_HAND);
    }
    else if (IS_LEG_L(ch,LOST_LEG))
    {
	act("A leg grows from the stump of $n's left hip.",ch,NULL,NULL,TO_ROOM);
	act("A leg grows from the stump of your left hip.",ch,NULL,NULL,TO_CHAR);
	REMOVE_BIT(ch->loc_hp[LOC_LEG_L],LOST_LEG);
	REMOVE_BIT(ch->loc_hp[LOC_LEG_L],BROKEN_LEG);
	SET_BIT(ch->loc_hp[LOC_LEG_L],LOST_FOOT);
    }
    else if (IS_LEG_R(ch,LOST_LEG))
    {
	act("A leg grows from the stump of $n's right hip.",ch,NULL,NULL,TO_ROOM);
	act("A leg grows from the stump of your right hip.",ch,NULL,NULL,TO_CHAR);
	REMOVE_BIT(ch->loc_hp[LOC_LEG_R],LOST_LEG);
	REMOVE_BIT(ch->loc_hp[LOC_LEG_R],BROKEN_LEG);
	SET_BIT(ch->loc_hp[LOC_LEG_R],LOST_FOOT);
    }
    else if (IS_ARM_L(ch,BROKEN_ARM))
    {
	act("$n's left arm snaps back into place.",ch,NULL,NULL,TO_ROOM);
	act("Your left arm snaps back into place.",ch,NULL,NULL,TO_CHAR);
	REMOVE_BIT(ch->loc_hp[LOC_ARM_L],BROKEN_ARM);
    }
    else if (IS_ARM_R(ch,BROKEN_ARM))
    {
	act("$n's right arm snaps back into place.",ch,NULL,NULL,TO_ROOM);
	act("Your right arm snaps back into place.",ch,NULL,NULL,TO_CHAR);
	REMOVE_BIT(ch->loc_hp[LOC_ARM_R],BROKEN_ARM);
    }
    else if (IS_LEG_L(ch,BROKEN_LEG))
    {
	act("$n's left leg snaps back into place.",ch,NULL,NULL,TO_ROOM);
	act("Your left leg snaps back into place.",ch,NULL,NULL,TO_CHAR);
	REMOVE_BIT(ch->loc_hp[LOC_LEG_L],BROKEN_LEG);
    }
    else if (IS_LEG_R(ch,BROKEN_LEG))
    {
	act("$n's right leg snaps back into place.",ch,NULL,NULL,TO_ROOM);
	act("Your right leg snaps back into place.",ch,NULL,NULL,TO_CHAR);
	REMOVE_BIT(ch->loc_hp[LOC_LEG_R],BROKEN_LEG);
    }
    else if (IS_ARM_L(ch,LOST_HAND))
    {
	act("A hand grows from the stump of $n's left wrist.",ch,NULL,NULL,TO_ROOM);
	act("A hand grows from the stump of your left wrist.",ch,NULL,NULL,TO_CHAR);
	REMOVE_BIT(ch->loc_hp[LOC_ARM_L],LOST_HAND);
	SET_BIT(ch->loc_hp[LOC_ARM_L],LOST_THUMB);
	SET_BIT(ch->loc_hp[LOC_ARM_L],LOST_FINGER_I);
	SET_BIT(ch->loc_hp[LOC_ARM_L],LOST_FINGER_M);
	SET_BIT(ch->loc_hp[LOC_ARM_L],LOST_FINGER_R);
	SET_BIT(ch->loc_hp[LOC_ARM_L],LOST_FINGER_L);
    }
    else if (IS_ARM_R(ch,LOST_HAND))
    {
	act("A hand grows from the stump of $n's right wrist.",ch,NULL,NULL,TO_ROOM);
	act("A hand grows from the stump of your right wrist.",ch,NULL,NULL,TO_CHAR);
	REMOVE_BIT(ch->loc_hp[LOC_ARM_R],LOST_HAND);
	SET_BIT(ch->loc_hp[LOC_ARM_R],LOST_THUMB);
	SET_BIT(ch->loc_hp[LOC_ARM_R],LOST_FINGER_I);
	SET_BIT(ch->loc_hp[LOC_ARM_R],LOST_FINGER_M);
	SET_BIT(ch->loc_hp[LOC_ARM_R],LOST_FINGER_R);
	SET_BIT(ch->loc_hp[LOC_ARM_R],LOST_FINGER_L);
    }
    else if (IS_LEG_L(ch,LOST_FOOT))
    {
	act("A foot grows from the stump of $n's left ankle.",ch,NULL,NULL,TO_ROOM);
	act("A foot grows from the stump of your left ankle.",ch,NULL,NULL,TO_CHAR);
	REMOVE_BIT(ch->loc_hp[LOC_LEG_L],LOST_FOOT);
    }
    else if (IS_LEG_R(ch,LOST_FOOT))
    {
	act("A foot grows from the stump of $n's right ankle.",ch,NULL,NULL,TO_ROOM);
	act("A foot grows from the stump of your right ankle.",ch,NULL,NULL,TO_CHAR);
	REMOVE_BIT(ch->loc_hp[LOC_LEG_R],LOST_FOOT);
    }
    else if (IS_ARM_L(ch,LOST_THUMB))
    {
	act("A thumb slides out of $n's left hand.",ch,NULL,NULL,TO_ROOM);
	act("A thumb slides out of your left hand.",ch,NULL,NULL,TO_CHAR);
	REMOVE_BIT(ch->loc_hp[LOC_ARM_L],LOST_THUMB);
	REMOVE_BIT(ch->loc_hp[LOC_ARM_L],BROKEN_THUMB);
    }
    else if (IS_ARM_L(ch,BROKEN_THUMB))
    {
	act("$n's left thumb snaps back into place.",ch,NULL,NULL,TO_ROOM);
	act("Your left thumb snaps back into place.",ch,NULL,NULL,TO_CHAR);
	REMOVE_BIT(ch->loc_hp[LOC_ARM_L],BROKEN_THUMB);
    }
    else if (IS_ARM_L(ch,LOST_FINGER_I))
    {
	act("An index finger slides out of $n's left hand.",ch,NULL,NULL,TO_ROOM);
	act("An index finger slides out of your left hand.",ch,NULL,NULL,TO_CHAR);
	REMOVE_BIT(ch->loc_hp[LOC_ARM_L],LOST_FINGER_I);
	REMOVE_BIT(ch->loc_hp[LOC_ARM_L],BROKEN_FINGER_I);
    }
    else if (IS_ARM_L(ch,BROKEN_FINGER_I))
    {
	act("$n's left index finger snaps back into place.",ch,NULL,NULL,TO_ROOM);
	act("Your left index finger snaps back into place.",ch,NULL,NULL,TO_CHAR);
	REMOVE_BIT(ch->loc_hp[LOC_ARM_L],BROKEN_FINGER_I);
    }
    else if (IS_ARM_L(ch,LOST_FINGER_M))
    {
	act("A middle finger slides out of $n's left hand.",ch,NULL,NULL,TO_ROOM);
	act("A middle finger slides out of your left hand.",ch,NULL,NULL,TO_CHAR);
	REMOVE_BIT(ch->loc_hp[LOC_ARM_L],LOST_FINGER_M);
	REMOVE_BIT(ch->loc_hp[LOC_ARM_L],BROKEN_FINGER_M);
    }
    else if (IS_ARM_L(ch,BROKEN_FINGER_M))
    {
	act("$n's left middle finger snaps back into place.",ch,NULL,NULL,TO_ROOM);
	act("Your left middle finger snaps back into place.",ch,NULL,NULL,TO_CHAR);
	REMOVE_BIT(ch->loc_hp[LOC_ARM_L],BROKEN_FINGER_M);
    }
    else if (IS_ARM_L(ch,LOST_FINGER_R))
    {
	act("A ring finger slides out of $n's left hand.",ch,NULL,NULL,TO_ROOM);
	act("A ring finger slides out of your left hand.",ch,NULL,NULL,TO_CHAR);
	REMOVE_BIT(ch->loc_hp[LOC_ARM_L],LOST_FINGER_R);
	REMOVE_BIT(ch->loc_hp[LOC_ARM_L],BROKEN_FINGER_R);
    }
    else if (IS_ARM_L(ch,BROKEN_FINGER_R))
    {
	act("$n's left ring finger snaps back into place.",ch,NULL,NULL,TO_ROOM);
	act("Your left ring finger snaps back into place.",ch,NULL,NULL,TO_CHAR);
	REMOVE_BIT(ch->loc_hp[LOC_ARM_L],BROKEN_FINGER_R);
    }
    else if (IS_ARM_L(ch,LOST_FINGER_L))
    {
	act("A little finger slides out of $n's left hand.",ch,NULL,NULL,TO_ROOM);
	act("A little finger slides out of your left hand.",ch,NULL,NULL,TO_CHAR);
	REMOVE_BIT(ch->loc_hp[LOC_ARM_L],LOST_FINGER_L);
	REMOVE_BIT(ch->loc_hp[LOC_ARM_L],BROKEN_FINGER_L);
    }
    else if (IS_ARM_L(ch,BROKEN_FINGER_L))
    {
	act("$n's left little finger snaps back into place.",ch,NULL,NULL,TO_ROOM);
	act("Your left little finger snaps back into place.",ch,NULL,NULL,TO_CHAR);
	REMOVE_BIT(ch->loc_hp[LOC_ARM_L],BROKEN_FINGER_L);
    }
    else if (IS_ARM_R(ch,LOST_THUMB))
    {
	act("A thumb slides out of $n's right hand.",ch,NULL,NULL,TO_ROOM);
	act("A thumb slides out of your right hand.",ch,NULL,NULL,TO_CHAR);
	REMOVE_BIT(ch->loc_hp[LOC_ARM_R],LOST_THUMB);
	REMOVE_BIT(ch->loc_hp[LOC_ARM_R],BROKEN_THUMB);
    }
    else if (IS_ARM_R(ch,BROKEN_THUMB))
    {
	act("$n's right thumb snaps back into place.",ch,NULL,NULL,TO_ROOM);
	act("Your right thumb snaps back into place.",ch,NULL,NULL,TO_CHAR);
	REMOVE_BIT(ch->loc_hp[LOC_ARM_R],BROKEN_THUMB);
    }
    else if (IS_ARM_R(ch,LOST_FINGER_I))
    {
	act("An index finger slides out of $n's right hand.",ch,NULL,NULL,TO_ROOM);
	act("An index finger slides out of your right hand.",ch,NULL,NULL,TO_CHAR);
	REMOVE_BIT(ch->loc_hp[LOC_ARM_R],LOST_FINGER_I);
	REMOVE_BIT(ch->loc_hp[LOC_ARM_R],BROKEN_FINGER_I);
    }
    else if (IS_ARM_R(ch,BROKEN_FINGER_I))
    {
	act("$n's right index finger snaps back into place.",ch,NULL,NULL,TO_ROOM);
	act("Your right index finger snaps back into place.",ch,NULL,NULL,TO_CHAR);
	REMOVE_BIT(ch->loc_hp[LOC_ARM_R],BROKEN_FINGER_I);
    }
    else if (IS_ARM_R(ch,LOST_FINGER_M))
    {
	act("A middle finger slides out of $n's right hand.",ch,NULL,NULL,TO_ROOM);
	act("A middle finger slides out of your right hand.",ch,NULL,NULL,TO_CHAR);
	REMOVE_BIT(ch->loc_hp[LOC_ARM_R],LOST_FINGER_M);
	REMOVE_BIT(ch->loc_hp[LOC_ARM_R],BROKEN_FINGER_M);
    }
    else if (IS_ARM_R(ch,BROKEN_FINGER_M))
    {
	act("$n's right middle finger snaps back into place.",ch,NULL,NULL,TO_ROOM);
	act("Your right middle finger snaps back into place.",ch,NULL,NULL,TO_CHAR);
	REMOVE_BIT(ch->loc_hp[LOC_ARM_R],BROKEN_FINGER_M);
    }
    else if (IS_ARM_R(ch,LOST_FINGER_R))
    {
	act("A ring finger slides out of $n's right hand.",ch,NULL,NULL,TO_ROOM);
	act("A ring finger slides out of your right hand.",ch,NULL,NULL,TO_CHAR);
	REMOVE_BIT(ch->loc_hp[LOC_ARM_R],LOST_FINGER_R);
	REMOVE_BIT(ch->loc_hp[LOC_ARM_R],BROKEN_FINGER_R);
    }
    else if (IS_ARM_R(ch,BROKEN_FINGER_R))
    {
	act("$n's right ring finger snaps back into place.",ch,NULL,NULL,TO_ROOM);
	act("Your right ring finger snaps back into place.",ch,NULL,NULL,TO_CHAR);
	REMOVE_BIT(ch->loc_hp[LOC_ARM_R],BROKEN_FINGER_R);
    }
    else if ( IS_ARM_R(ch,LOST_FINGER_L))
    {
	act("A little finger slides out of $n's right hand.",ch,NULL,NULL,TO_ROOM);
	act("A little finger slides out of your right hand.",ch,NULL,NULL,TO_CHAR);
	REMOVE_BIT(ch->loc_hp[LOC_ARM_R],LOST_FINGER_L);
	REMOVE_BIT(ch->loc_hp[LOC_ARM_R],BROKEN_FINGER_L);
    }
    else if ( IS_ARM_R(ch,BROKEN_FINGER_L))
    {
	act("$n's right little finger snaps back into place.",ch,NULL,NULL,TO_ROOM);
	act("Your right little finger snaps back into place.",ch,NULL,NULL,TO_CHAR);
	REMOVE_BIT(ch->loc_hp[LOC_ARM_R],BROKEN_FINGER_L);
    }
    else if (IS_BODY(ch,CUT_THROAT))
    {
    	if (IS_SET(ch->loc_hp[6], BLEEDING_THROAT)) return;
	act("The wound in $n's throat closes up.",ch,NULL,NULL,TO_ROOM);
	act("The wound in your throat closes up.",ch,NULL,NULL,TO_CHAR);
	REMOVE_BIT(ch->loc_hp[LOC_BODY],CUT_THROAT);
    }
    return;
}

void do_upkeep( CHAR_DATA *ch, char *argument )
{
    char      arg [MAX_INPUT_LENGTH];
    char      buf [MAX_INPUT_LENGTH];
    char      buf2 [MAX_INPUT_LENGTH];

    argument = one_argument( argument, arg );

    if (IS_NPC(ch)) return;
    if (IS_CLASS(ch, CLASS_DEMON))
    {
	send_to_char("--------------------------------------------------------------------------------\n\r",ch);
	send_to_char("                              -= Demonic powers =-\n\r",ch);
	send_to_char("--------------------------------------------------------------------------------\n\r",ch);

	if (ch->pcdata->powers[DPOWER_FLAGS] < 1)
	    send_to_char("You have no demonic powers.\n\r",ch);
    	if ( IS_DEMPOWER(ch,DEM_FANGS) )
	{
	    if (IS_VAMPAFF(ch, VAM_FANGS)) send_to_char("You have a pair of long pointed fangs extending from your gums.\n\r",ch);
	    else send_to_char("You have a pair of long pointed fangs, but they are not currently extended.\n\r",ch);
	}
    	if ( IS_DEMPOWER(ch,DEM_CLAWS) )
	{
	    if (IS_VAMPAFF(ch, VAM_CLAWS)) send_to_char("You have a pair of razor sharp claws extending from your fingers.\n\r",ch);
	    else send_to_char("You have a pair of razor sharp claws, but they are not currently extended.\n\r",ch);
	}
    	if ( IS_DEMPOWER(ch,DEM_HORNS) )
	{
	    if (IS_DEMAFF(ch, DEM_HORNS)) send_to_char("You have a pair of curved horns extending from your forehead.\n\r",ch);
	    else send_to_char("You have a pair of curved horns, but they are not currently extended.\n\r",ch);
	}
    	if ( IS_DEMPOWER(ch,DEM_TAIL) )
	{
	    if (IS_DEMAFF(ch, DEM_TAIL)) send_to_char("You have a big tail coming out of your arse.\n\r",ch);
	    else send_to_char("You have a tail but its currently inside your arse.\n\r",ch);
	}
    	if ( IS_DEMPOWER(ch,DEM_HOOVES) )
	{
	    if (IS_DEMAFF(ch, DEM_HOOVES)) send_to_char("You have hooves instead of feet.\n\r",ch);
	    else send_to_char("You are able to transform your feet into hooves at will.\n\r",ch);
	}
    	if ( IS_DEMPOWER(ch,DEM_EYES) )
	{
	    if (IS_VAMPAFF(ch, VAM_NIGHTSIGHT)) send_to_char("Your eyes are glowing bright red, allowing you to see in the dark.\n\r",ch);
	    else send_to_char("You are able to see in the dark, although that power is not currently activated.\n\r",ch);
	}
    	if ( IS_DEMPOWER(ch,DEM_WINGS) )
	{
	    if (!IS_DEMAFF(ch, DEM_WINGS)) send_to_char("You have the ability to extend wings from your back.\n\r",ch);
	    else if (IS_DEMAFF(ch, DEM_UNFOLDED)) send_to_char("You have a pair of large leathery wings unfolded behind your back.\n\r",ch);
	    else send_to_char("You have a pair of large leathery wings folded behind your back.\n\r",ch);
	}
    	if ( IS_DEMPOWER(ch,DEM_MIGHT) )
	    send_to_char("Your muscles ripple with supernatural strength.\n\r",ch);
    	if ( IS_DEMPOWER(ch,DEM_TOUGH) )
	    send_to_char("Your skin reflects blows with supernatural toughness.\n\r",ch);
    	if ( IS_DEMPOWER(ch,DEM_SPEED) )
	    send_to_char("You move with supernatural speed and grace.\n\r",ch);
    	if ( IS_DEMPOWER(ch,DEM_TRAVEL) )
	    send_to_char("You are able to travel to other demons at will.\n\r",ch);
    	if ( IS_DEMPOWER(ch,DEM_SCRY) )
	    send_to_char("You are able to scry over great distances at will.\n\r",ch);
    	if ( IS_DEMPOWER(ch,DEM_SHADOWSIGHT) )
	{
	    if (IS_AFFECTED(ch, AFF_SHADOWSIGHT)) send_to_char("You are able see things in the shadowplane.\n\r",ch);
	    else send_to_char("You are able to view the shadowplane, although you are not currently doing so.\n\r",ch);
	}

	if ( IS_DEMPOWER(ch, DEM_REIGN) )
	{
	  send_to_char("You have complete reign over lesser beings in Hell.\r\n", ch);
	}
    }
    else if (IS_CLASS(ch, CLASS_WEREWOLF))
    {
	send_to_char("--------------------------------------------------------------------------------\n\r",ch);
	send_to_char("                              -= Werewolf powers =-\n\r",ch);
	send_to_char("--------------------------------------------------------------------------------\n\r",ch);

	    if (ch->pcdata->powers[WPOWER_BEAR] > 0) 
		send_to_char("Your strength is so great that no ropes can hold you.\n\r",ch);
	    if (ch->pcdata->powers[WPOWER_BEAR] > 2) 
		send_to_char("Your claws are so tough that they can parry weapons.\n\r",ch);
	    if (ch->pcdata->powers[WPOWER_BEAR] > 3) 
		send_to_char("Your wounds heal at amazing speeds when you sleep.\n\r",ch);
	    if (ch->pcdata->powers[WPOWER_BOAR] > 0) 
		send_to_char("No door is sturdy enough to resist you.\n\r",ch);
	    if (ch->pcdata->powers[WPOWER_BOAR] > 1) 
		send_to_char("Your first blow in combat has a +50 damage bonus.\n\r",ch);
	    if (ch->pcdata->powers[WPOWER_BOAR] > 2) 
		send_to_char("Your skin is extremely tough. You take half damage in combat.\n\r",ch);
	    if (ch->pcdata->powers[WPOWER_BOAR] > 3) 
		send_to_char("You are able to shrug off blows that would knock out most people.\n\r",ch);
	    if (ch->pcdata->powers[WPOWER_BOAR] > 4) 
		send_to_char("Your might is so great, you may deal double damage.\n\r",ch);
	    if (ch->pcdata->powers[WPOWER_LYNX] > 0) 
		send_to_char("You move so lightly that you leave no tracks behind you.\n\r",ch);
	    if (ch->pcdata->powers[WPOWER_LYNX] > 1) 
		send_to_char("You are able hunt people with much greater speed than normal.\n\r",ch);
	    if (ch->pcdata->powers[WPOWER_LYNX] > 2) 
		send_to_char("You have an extra attack in combat.\n\r",ch);
	    if (ch->pcdata->powers[WPOWER_LYNX] > 3) 
		send_to_char("Yours claws parry blows with lightning fast speed.\n\r",ch);
	    if (ch->pcdata->powers[WPOWER_OWL] > 3) 
		send_to_char("You are able to fully control your magic in crinos form.\n\r",ch);
	    if (ch->pcdata->powers[WPOWER_SPIDER] > 0) 
		send_to_char("Your bite injects your opponents with a deadly venom.\n\r",ch);
	    if (ch->pcdata->powers[WPOWER_SPIDER] > 1) 
		send_to_char("You are able to shoot a web at your opponents to entrap them.\n\r",ch);
	    if (ch->pcdata->powers[WPOWER_SPIDER] > 2) 
		send_to_char("Poisons have no affect upon you.\n\r",ch);
	    if (ch->pcdata->powers[WPOWER_MANTIS] > 4) 
		send_to_char("Your opponents get -25 to parry and -50 to dodge.\n\r",ch);
	    else if (ch->pcdata->powers[WPOWER_MANTIS] > 3) 
		send_to_char("Your opponents get -20 to parry and -40 to dodge.\n\r",ch);
	    else if (ch->pcdata->powers[WPOWER_MANTIS] > 2) 
		send_to_char("Your opponents get -15 to parry and -30 to dodge.\n\r",ch);
	    else if (ch->pcdata->powers[WPOWER_MANTIS] > 1) 
		send_to_char("Your opponents get -10 to parry and -20 to dodge.\n\r",ch);
	    else if (ch->pcdata->powers[WPOWER_MANTIS] > 0) 
		send_to_char("Your opponents get -5 to parry and -10 to dodge.\n\r",ch);
    	if (IS_IMMUNE(ch,IMM_SHIELDED) )
	    send_to_char("You are shielded\n\r",ch);
    	if (IS_AFFECTED(ch,AFF_SHADOWPLANE) )
	    send_to_char("You are in the shadowplane\n\r",ch);
	if (IS_VAMPAFF(ch, VAM_FANGS)) 
		send_to_char("Your fangs are extended.\n\r",ch);
	if (IS_VAMPAFF(ch, VAM_CLAWS)) 
		send_to_char("Your claws are extended\n\r",ch);
    	if (IS_SET(ch->act,PLR_HOLYLIGHT) )
	    send_to_char("You have truesight\n\r",ch);
    	if (IS_VAMPAFF(ch,VAM_NIGHTSIGHT) )
	    send_to_char("You have nightsight\n\r",ch);
    	if (IS_AFFECTED(ch,AFF_SHADOWSIGHT) )
	    send_to_char("You have shadowsight\n\r",ch);
    }
    else if (IS_CLASS(ch, CLASS_VAMPIRE))
    {
	send_to_char("--------------------------------------------------------------------------------\n\r",ch);
	send_to_char("                              -= Vampire upkeep =-\n\r",ch);
	send_to_char("--------------------------------------------------------------------------------\n\r",ch);
	send_to_char("Staying alive...upkeep 1.\n\r",ch);

    	if (IS_VAMPAFF(ch,VAM_DISGUISED) )
	{
	    if      (ch->beast == 0  ) sprintf(buf,"You are disguised as %s...no upkeep.\n\r",ch->morph);
	    else if (ch->beast == 100) sprintf(buf,"You are disguised as %s...upkeep 10-20.\n\r",ch->morph);
	    else sprintf(buf,"You are disguised as %s...upkeep 5-10.\n\r",ch->morph);
	    send_to_char(buf,ch);
	}
    	if (IS_IMMUNE(ch,IMM_SHIELDED) )
	{
	    if      (ch->beast == 0  ) sprintf(buf,"You are shielded...no upkeep.\n\r");
	    else if (ch->beast == 100) sprintf(buf,"You are shielded...upkeep 2-6.\n\r");
	    else sprintf(buf,"You are shielded...upkeep 1-3.\n\r");
	    send_to_char(buf,ch);
	}
    	if (IS_AFFECTED(ch,AFF_SHADOWPLANE) )
	{
	    if      (ch->beast == 0  ) sprintf(buf,"You are in the shadowplane...no upkeep.\n\r");
	    else if (ch->beast == 100) sprintf(buf,"You are in the shadowplane...no upkeep.\n\r");
	    else sprintf(buf,"You are in the shadowplane...no upkeep.\n\r");
	    send_to_char(buf,ch);
	}
    	if (IS_VAMPAFF(ch,VAM_FANGS) )
	{
	    if      (ch->beast == 0  ) sprintf(buf,"You have your fangs out...no upkeep.\n\r");
	    else if (ch->beast == 100) sprintf(buf,"You have your fangs out...upkeep 2.\n\r");
	    else sprintf(buf,"You have your fangs out...upkeep 1.\n\r");
	    send_to_char(buf,ch);
	}
    	if (IS_VAMPAFF(ch,VAM_CLAWS) )
	{
	    if      (ch->beast == 0  ) sprintf(buf,"You have your claws out...no upkeep.\n\r");
	    else if (ch->beast == 100) sprintf(buf,"You have your claws out...upkeep 2-6.\n\r");
	    else sprintf(buf,"You have your claws out...upkeep 1-3.\n\r");
	    send_to_char(buf,ch);
	}
    	if (IS_VAMPAFF(ch,VAM_NIGHTSIGHT) )
	{
	    if      (ch->beast == 0  ) sprintf(buf,"You have nightsight...no upkeep.\n\r");
	    else if (ch->beast == 100) sprintf(buf,"You have nightsight...upkeep 2.\n\r");
	    else sprintf(buf,"You have nightsight...upkeep 1.\n\r");
	    send_to_char(buf,ch);
	}
    	if (IS_AFFECTED(ch,AFF_SHADOWSIGHT) )
	{
	    if      (ch->beast == 0  ) sprintf(buf,"You have shadowsight...no upkeep.\n\r");
	    else if (ch->beast == 100) sprintf(buf,"You are shadowsight...upkeep 2-6.\n\r");
	    else sprintf(buf,"You are shadowsight...upkeep 1-3.\n\r");
	    send_to_char(buf,ch);
	}
    	if (IS_SET(ch->act,PLR_HOLYLIGHT) )
	{
	    if      (ch->beast == 0  ) sprintf(buf,"You have truesight...no upkeep.\n\r");
	    else if (ch->beast == 100) sprintf(buf,"You have truesight...upkeep 2-10.\n\r");
	    else sprintf(buf,"You have truesight...upkeep 1-5.\n\r");
	    send_to_char(buf,ch);
	}
    	if (IS_VAMPAFF(ch,VAM_CHANGED) )
	{
	    if      (IS_POLYAFF(ch,POLY_BAT))  sprintf(buf2,"bat" );
	    else if (IS_POLYAFF(ch,POLY_WOLF)) sprintf(buf2,"wolf");
	    else                               sprintf(buf2,"mist");
	    if      (ch->beast == 0  ) sprintf(buf,"You have changed into %s form...no upkeep.\n\r",buf2);
	    else if (ch->beast == 100) sprintf(buf,"You have changed into %s form...upkeep 10-20.\n\r",buf2);
	    else sprintf(buf,"You have changed into %s form...upkeep 5-10.\n\r",buf2);
	    send_to_char(buf,ch);
	}
    	if (IS_POLYAFF(ch,POLY_SERPENT) )
	{
	    if      (ch->beast == 0  ) sprintf(buf,"You are in serpent form...no upkeep.\n\r");
	    else if (ch->beast == 100) sprintf(buf,"You are in serpent form...upkeep 6-8.\n\r");
	    else sprintf(buf,"You are in serpent form...upkeep 1-3.\n\r");
	    send_to_char(buf,ch);
	}
    }
    else
    {
	send_to_char("Huh?\n\r",ch);
	return;
    }
    send_to_char("--------------------------------------------------------------------------------\n\r",ch);
    return;
}

void do_favour( CHAR_DATA *ch, char *argument )
{
    CHAR_DATA *victim;
    char      arg1 [MAX_INPUT_LENGTH];
    char      arg2 [MAX_INPUT_LENGTH];

    argument = one_argument( argument, arg1 );
    argument = one_argument( argument, arg2 );

    if ( IS_NPC(ch) ) return;
    if (!IS_CLASS(ch,CLASS_VAMPIRE) && !IS_CLASS(ch,CLASS_WEREWOLF))
    {
        send_to_char( "Huh?\n\r", ch );
        return;
    }

    if (!IS_SET(ch->special,SPC_PRINCE) && ch->pcdata->stats[UNI_GEN] != 2)
    {
        send_to_char( "Huh?\n\r", ch );
        return;
    }

    if ( arg1[0] == '\0' || arg2[0] == '\0' )
    {
	send_to_char( "Syntax is: favour <target> <prince/sire/induct/outcast>\n\r", ch );
	return;
    }

    if ( ( victim = get_char_room( ch, arg1 ) ) == NULL )
    {
	send_to_char( "They aren't here.\n\r", ch );
	return;
    }

    if ( IS_NPC(victim) )
    {
	send_to_char( "Not on NPC's.\n\r", ch );
	return;
    }

    if ( ch == victim )
    {
	send_to_char( "Not on yourself!\n\r", ch );
	return;
    }

    if (victim->class != ch->class && !IS_CLASS(victim, CLASS_HIGHLANDER) )
    {
	send_to_char("They are not of your kind!\r\n", ch);
	return;
    }

    if ( str_cmp(victim->clan,ch->clan) && str_cmp(arg2,"induct") )
    {
	send_to_char( "You can only grant your favour to someone in your clan.\n\r", ch );
	return;
    }

    if ( !str_cmp(arg2,"prince") && ch->pcdata->stats[UNI_GEN] == 2)
    {
	if (IS_SET(victim->special,SPC_PRINCE)) {
	act("You remove $N's prince privileges!",ch,NULL,victim,TO_CHAR);
	act("$n removes $N's prince privileges!",ch,NULL,victim,TO_NOTVICT);
	act("$n removes your prince privileges!",ch,NULL,victim,TO_VICT);
	if (IS_SET(victim->special,SPC_SIRE)) REMOVE_BIT(victim->special,SPC_SIRE);
	REMOVE_BIT(victim->special,SPC_PRINCE);return;}
	act("You make $N a prince!",ch,NULL,victim,TO_CHAR);
	act("$n has made $N a prince!",ch,NULL,victim,TO_NOTVICT);
	act("$n has made you a prince!",ch,NULL,victim,TO_VICT);
	SET_BIT(victim->special,SPC_PRINCE);
	if (IS_SET(victim->special,SPC_SIRE)) REMOVE_BIT(victim->special,SPC_SIRE);
	return;
    }
    else if ( !str_cmp(arg2,"sire") && (ch->pcdata->stats[UNI_GEN] == 2 || IS_SET(ch->special,SPC_PRINCE)))
    {
	if (IS_SET(victim->special,SPC_SIRE)) {
	act("You remove $N's permission to sire a childe!",ch,NULL,victim,TO_CHAR);
	act("$n has removed $N's permission to sire a childe!",ch,NULL,victim,TO_NOTVICT);
	act("$n has remove your permission to sire a childe!",ch,NULL,victim,TO_VICT);
	REMOVE_BIT(victim->special,SPC_SIRE);return;}
	act("You grant $N permission to sire a childe!",ch,NULL,victim,TO_CHAR);
	act("$n has granted $N permission to sire a childe!",ch,NULL,victim,TO_NOTVICT);
	act("$n has granted you permission to sire a childe!",ch,NULL,victim,TO_VICT);
	SET_BIT(victim->special,SPC_SIRE);
	return;
    }
    else if ( !str_cmp(arg2,"outcast") && victim->pcdata->stats[UNI_GEN] > 2 && 
	ch->pcdata->stats[UNI_GEN] == 2)
    {
	act("You outcast $N!",ch,NULL,victim,TO_CHAR);
	act("$n has outcasted $N!",ch,NULL,victim,TO_NOTVICT);
	act("$n has outcasted you!",ch,NULL,victim,TO_VICT);
	free_string(victim->clan);
	victim->clan = str_dup( "" );
	return;
    }
    else if ( !str_cmp(arg2,"outcast") && victim->pcdata->stats[UNI_GEN] > 2 && 
	!IS_SET(victim->special, SPC_PRINCE) && IS_SET(ch->special,SPC_PRINCE)
	&& !str_cmp(ch->clan, victim->clan) )
    {
	act("You outcast $N!",ch,NULL,victim,TO_CHAR);
	act("$n has outcasted $N!",ch,NULL,victim,TO_NOTVICT);
	act("$n has outcasted $N!",ch,NULL,victim,TO_VICT);
	free_string(victim->clan);
	victim->clan = str_dup( "" );
	return;
    }
    else if ( !str_cmp(arg2,"induct") && victim->pcdata->stats[UNI_GEN] > 2 && 
	strlen(victim->clan) < 2)
    {
	if (IS_SET(victim->special, SPC_ANARCH))
	{
	    send_to_char("You cannot induct an Anarch!\n\r",ch);
	    return;
	}
	if (IS_CLASS(victim, CLASS_VAMPIRE) && IS_SET(victim->special, SPC_INCONNU))
	{
	    send_to_char("You cannot induct an Inconnu!\r\n", ch);
	    return;
	}
    if (ch->exp < 500000)
    {
	send_to_char("You cannot afford the 500000 exp required to induct them into your clan.\n\r",ch);
	return;
    }

    if (victim->exp < 500000)
    {
	send_to_char("They cannot afford the 500000 exp required to be inducted into your clan.\n\r",ch);
	return;
    }

    ch->exp -= 500000;
    victim->exp -= 500000;

	act("You induct $N into your clan!",ch,NULL,victim,TO_CHAR);
	act("$n inducts $N into $s clan!",ch,NULL,victim,TO_NOTVICT);
	act("$n inducts you into $s clan!",ch,NULL,victim,TO_VICT);
	free_string(victim->clan);
	victim->clan = str_dup( ch->clan );
	return;
    }
    else if ( !str_cmp(arg2,"induct") && victim->pcdata->stats[UNI_GEN] > 2 && 
	!IS_SET(victim->special, SPC_PRINCE) && IS_SET(ch->special,SPC_PRINCE) &&
	strlen(victim->clan) < 2)
    {
	if (IS_SET(victim->special, SPC_ANARCH))
	{
	    send_to_char("You cannot induct an Anarch!\n\r",ch);
	    return;
	}
    if (ch->exp < 500000)
    {
	send_to_char("You cannot afford the 500000 exp required to induct them into your clan.\n\r",ch);
	return;
    }

    if (victim->exp < 500000)
    {
	send_to_char("They cannot afford the 500000 exp required to be inducted into your clan.\n\r",ch);
	return;
    }

    ch->exp -= 500000;
    victim->exp -= 500000;

	act("You induct $N into your clan!",ch,NULL,victim,TO_CHAR);
	act("$n inducts $N into $s clan!",ch,NULL,victim,TO_NOTVICT);
	act("$n inducts you into $s clan!",ch,NULL,victim,TO_VICT);
	free_string(victim->clan);
	victim->clan = str_dup( ch->clan );
	return;
    }
    else if ( !str_cmp(arg2,"accept") &&
	(ch->pcdata->stats[UNI_GEN] == 2 || IS_SET(ch->special,SPC_PRINCE)))
    {
/*	if ( victim->pcdata->rank > AGE_CHILDE)
	{ send_to_char("But they are not a childe!\n\r",ch); return; }
	act("You accept $N into the clan!",ch,NULL,victim,TO_CHAR);
	act("$n has accepted $N into $s clan!",ch,NULL,victim,TO_NOTVICT);
	act("$n accepted you into $s clan!",ch,NULL,victim,TO_VICT);
	victim->pcdata->rank = AGE_NEONATE;*/
	return;
    }
    else send_to_char( "You are unable to grant that sort of favour.\n\r", ch );
    return;
}

void do_familiar( CHAR_DATA *ch, char *argument )
{
    CHAR_DATA *victim;
    CHAR_DATA *familiar;
    char arg[MAX_INPUT_LENGTH];

    one_argument( argument, arg );

    return;

    if ( arg[0] == '\0' )
    {
	send_to_char( "What do you wish to make your familiar?\n\r", ch );
	return;
    }

    if ( ( victim = get_char_room( ch, arg ) ) == NULL )
    {
	send_to_char( "They aren't here.\n\r", ch );
	return;
    }

    if ( ch == victim )
    {
	send_to_char( "Become your own familiar?\n\r", ch );
	return;
    }

    if ( !IS_NPC(victim) )
    {
	send_to_char( "Not on players.\n\r", ch );
	return;
    }

    if ( ( familiar = ch->pcdata->familiar ) != NULL ) 
	familiar->wizard = NULL;
    ch->pcdata->familiar = victim;
    victim->wizard = ch;
    send_to_char("Ok.\n\r",ch);

    return;
}

void do_fcommand( CHAR_DATA *ch, char *argument )
{
    CHAR_DATA *victim;

    if (IS_NPC(ch)) return;

    if (!IS_CLASS(ch, CLASS_VAMPIRE) && ch->level < LEVEL_APPRENTICE)
    {
	send_to_char("Huh?\n\r",ch);
	return;
    }

    if (IS_CLASS(ch, CLASS_VAMPIRE) && !IS_VAMPAFF(ch,VAM_DOMINATE) )
    {
	send_to_char("You are not trained in the Dominate discipline.\n\r",ch);
	return;
    }

    if ( ( victim = ch->pcdata->familiar ) == NULL )
    {
	send_to_char( "But you don't have a familiar!\n\r", ch );
	return;
    }

    if ( argument[0] == '\0' )
    {
	send_to_char( "What do you wish to make your familiar do?\n\r", ch );
	return;
    }
    interpret(victim,argument);
    return;
}

void do_flex( CHAR_DATA *ch, char *argument )
{
    act("You flex your bulging muscles.",ch,NULL,NULL,TO_CHAR);
    act("$n flexes $s bulging muscles.",ch,NULL,NULL,TO_ROOM);
    if (IS_NPC(ch)) return;

    if (IS_CLASS(ch, CLASS_DEMON))
    {
	if (!IS_DEMPOWER(ch, DEM_MIGHT))
	    return;
    }
    else if (!IS_CLASS(ch, CLASS_WEREWOLF) || ch->pcdata->powers[WPOWER_BEAR] < 1)
	return;

    if (IS_AFFECTED(ch, AFF_PARADOX))
    {
	send_to_char("Your soul is stuck in paradox!\r\n", ch);
	return;
    }

    if ( is_affected(ch, gsn_tied ))
    {
	act("The ropes restraining you snap.",ch,NULL,NULL,TO_CHAR);
	act("The ropes restraining $n snap.",ch,NULL,NULL,TO_ROOM);
	affect_strip(ch, gsn_tied);
    }
    if (is_affected(ch, gsn_web))
    {
	act("The webbing entrapping $n breaks away.",ch,NULL,NULL,TO_ROOM);
	send_to_char("The webbing entrapping you breaks away.\n\r",ch);
	affect_strip(ch, gsn_web);
    }
    if (IS_AFFECTED(ch, AFF_WEBBED))
    {
	act("The webbing entrapping $n breaks away.",ch,NULL,NULL,TO_ROOM);
	send_to_char("The webbing entrapping you breaks away.\n\r",ch);
	REMOVE_BIT(ch->affected_by, AFF_WEBBED);
    }
    WAIT_STATE(ch,12);
    return;
}

void do_rage( CHAR_DATA *ch, char *argument )
{
    if ( IS_NPC(ch) )
	return;

    if (IS_CLASS(ch, CLASS_VAMPIRE))
    {
	if ( ch->pcdata->stats[UNI_RAGE] >= 100 )
	{
	    send_to_char("Your beast is already controlling you.\n\r",ch);
	    return;
	}
	if (!IS_VAMPAFF(ch, VAM_NIGHTSIGHT)) do_nightsight(ch,"");
	if (!IS_VAMPAFF(ch, VAM_FANGS)) do_fangs(ch,"");
	if (!IS_VAMPAFF(ch, VAM_CLAWS)) do_claws(ch,"");
	send_to_char("You bare yours fangs and growl as your inner beast consumes you.\n\r",ch);
	act("$n bares $s fangs and growls as $s inner beast consumes $m.",ch,NULL,NULL,TO_ROOM);
	ch->pcdata->stats[UNI_RAGE] += number_range(10,20);
	if (ch->beast > 0) do_beastlike(ch,"");
	WAIT_STATE(ch,12);
	return;
    }
    if (!IS_CLASS(ch, CLASS_WEREWOLF) || ch->pcdata->powers[WPOWER_BEAR] < 2)
    {
	send_to_char("Huh?\n\r",ch);
	return;
    }

    if (!IS_SET(ch->special, SPC_WOLFMAN))
    {
	send_to_char("You start snarling angrily.\n\r",ch);
	act("$n starts snarling angrily.",ch,NULL,NULL,TO_ROOM);
	ch->pcdata->stats[UNI_RAGE] += number_range(10,20);
	if (ch->pcdata->stats[UNI_RAGE] >= 100) do_werewolf(ch,"");
	WAIT_STATE(ch,12);
	return;
    }
    send_to_char("But you are already in a rage!\n\r",ch);
    return;
}

void do_calm( CHAR_DATA *ch, char *argument )
{
    if ( IS_NPC(ch) )
	return;

    if (IS_CLASS(ch, CLASS_VAMPIRE) && ch->beast < 1)
    {
	if ( ch->pcdata->stats[UNI_RAGE] < 1 )
	{
	    send_to_char("Your beast doesn't control your actions.\n\r",ch);
	    return;
	}
	send_to_char("You take a deep breath and force back your inner beast.\n\r",ch);
	act("$n takes a deep breath and forces back $s inner beast.",ch,NULL,NULL,TO_ROOM);
	ch->pcdata->stats[UNI_RAGE] = 0;
	if (IS_VAMPAFF(ch, VAM_NIGHTSIGHT)) do_nightsight(ch,"");
	if (IS_VAMPAFF(ch, VAM_FANGS)) do_fangs(ch,"");
	if (IS_VAMPAFF(ch, VAM_CLAWS)) do_claws(ch,"");
	WAIT_STATE(ch,12);
	return;
    }
    if (!IS_CLASS(ch, CLASS_WEREWOLF) || ch->pcdata->powers[WPOWER_WOLF] < 3)
    {
	send_to_char("Huh?\n\r",ch);
	return;
    }

    if (IS_SET(ch->special, SPC_WOLFMAN))
    {
	send_to_char("You take a deep breath and calm yourself.\n\r",ch);
	act("$n takes a deep breath and tries to calm $mself.",ch,NULL,NULL,TO_ROOM);
	ch->pcdata->stats[UNI_RAGE] -= number_range(10,20);
	if (ch->pcdata->stats[UNI_RAGE] < 100) do_unwerewolf(ch,"");
	WAIT_STATE(ch,12);
	return;
    }

    if (ch->pcdata->stats[UNI_RAGE]  > 0)
    {
	send_to_char("You take a deep breath and calm yourself.\r\n", ch);
	act("$n takes a deep breath and tries to calm $mself.",ch,NULL,NULL,TO_ROOM);
	ch->pcdata->stats[UNI_RAGE] -= number_range(10,20);
	if (ch->pcdata->stats[UNI_RAGE] < 0)
	{
	    ch->pcdata->stats[UNI_RAGE] = 0;
	    send_to_char("You are now totally calm.\r\n", ch);
	    WAIT_STATE(ch,12);
	}
    }
    else
	send_to_char("You are totally calm.\r\n", ch);
    return;
}

void do_birth( CHAR_DATA *ch, char *argument )
{
    char buf2[MAX_STRING_LENGTH];

    if (IS_NPC(ch)) return;

    if (!IS_EXTRA(ch, EXTRA_PREGNANT))
    {
	send_to_char("But you are not even pregnant!\n\r",ch);
	return;
    }

    if (!IS_EXTRA(ch, EXTRA_LABOUR))
    {
	send_to_char("You're not ready to give birth yet.\n\r",ch);
	return;
    }

    if ( argument[0] == '\0' )
    {
	if (ch->pcdata->genes[4] == SEX_MALE)
	    send_to_char( "What do you wish to name your little boy?\n\r", ch );
	else if (ch->pcdata->genes[4] == SEX_FEMALE)
	    send_to_char( "What do you wish to name your little girl?\n\r", ch );
	else
	    send_to_char( "What do you wish to name your child?\n\r", ch );
	return;
    }

    if (!check_parse_name( argument ))
    {
	send_to_char( "Thats an illegal name.\n\r", ch );
	return;
    }

    if ( char_exists(FALSE,argument) )
    {
	send_to_char( "That player already exists.\n\r", ch );
	return;
    }

    strcpy(buf2,ch->pcdata->cparents);
    strcat(buf2," ");
    strcat(buf2,argument);
    if (!birth_ok(ch, buf2))
    {
	send_to_char( "Bug - please inform KaVir.\n\r", ch );
	return;
    }
    argument[0] = UPPER(argument[0]);
    birth_write( ch, argument );
    ch->pcdata->genes[9] += 1;
    REMOVE_BIT(ch->extra, EXTRA_PREGNANT);
    REMOVE_BIT(ch->extra, EXTRA_LABOUR);
    save_char_obj(ch);
    return;
}

bool birth_ok( CHAR_DATA *ch, char *argument )
{
    char buf [MAX_STRING_LENGTH];
    char mum [MAX_INPUT_LENGTH];
    char dad [MAX_INPUT_LENGTH];
    char child [MAX_INPUT_LENGTH];

    argument = one_argument( argument, mum );
    argument = one_argument( argument, dad );
    argument = one_argument( argument, child );

    if (dad[0] == '\0')
    {
	send_to_char("You are unable to give birth - please inform KaVir.\n\r",ch);
	return FALSE;
    }
    if (child[0] == '\0')
    {
	send_to_char("You are unable to give birth - please inform KaVir.\n\r",ch);
	return FALSE;
    }
    dad[0] = UPPER(dad[0]);
    if (!str_cmp(dad,"Kavir")) strcpy(dad,"KaVir");
    child[0] = UPPER(child[0]);
    if (ch->pcdata->genes[4] == SEX_MALE)
    {
	send_to_char("You give birth to a little boy!\n\r",ch);
	sprintf(buf,"%s has given birth to %s's son, named %s!",ch->name,dad,child);
	do_info(ch,buf);
	return TRUE; 
    }
    else if (ch->pcdata->genes[4] == SEX_FEMALE)
    {
	send_to_char("You give birth to a little girl!\n\r",ch);
	sprintf(buf,"%s has given birth to %s's daughter, named %s!",ch->name,dad,child);
	do_info(ch,buf);
	return TRUE; 
    }
    return FALSE;
}

void birth_write( CHAR_DATA *ch, char *argument )
{
    FILE *fp;
    char buf [MAX_STRING_LENGTH];
    char *strtime;

    strtime = ctime( &current_time );
    strtime[strlen(strtime)-1] = '\0';

    sprintf( buf, "%s%s", PLAYER_DIR, capitalize( argument ) );
    if ( ( fp = fopen(buf, "w") ) != NULL) 
    {  
	fprintf( fp, "#PLAYER\n");
	fprintf( fp, "Name         %s~\n",capitalize( argument ));
	fprintf( fp, "ShortDescr   ~\n"				);
	fprintf( fp, "LongDescr    ~\n"				);
	fprintf( fp, "Description  ~\n"				);
	fprintf( fp, "Lord         ~\n"				);
	fprintf( fp, "Clan         ~\n"				);
	fprintf( fp, "Morph        ~\n"				);
	fprintf( fp, "Createtime   %s~\n",str_dup( strtime )	);
 	fprintf( fp, "Lasttime     ~\n"				);
	fprintf( fp, "Lasthost     ~\n"				);
	fprintf( fp, "Poweraction  ~\n"				);
	fprintf( fp, "Powertype    ~\n"				);
	fprintf( fp, "Prompt       ~\n"				);
	fprintf( fp, "Cprompt      ~\n"				);
	fprintf( fp, "Sex          %d\n",ch->pcdata->genes[4]	);
	fprintf( fp, "Race         0\n"				);
	fprintf( fp, "Immune       %d\n",ch->pcdata->genes[3]	);
	fprintf( fp, "Polyaff      0\n"				);
	fprintf( fp, "Itemaffect   0\n"				);
	fprintf( fp, "Vampaff      0\n"				);
	fprintf( fp, "Vamppass     0\n"				);
	fprintf( fp, "Form         32767\n"			);
	fprintf( fp, "Beast        15\n"			);
	fprintf( fp, "Vampgen      -1\n"			);
	fprintf( fp, "Spectype     0\n"				);
	fprintf( fp, "Specpower    0\n"				);
	fprintf( fp, "Home         3001\n"			);
	fprintf( fp, "Level        2\n"				);
	fprintf( fp, "Trust        0\n"				);
	fprintf( fp, "Played       0\n"				);
	fprintf( fp, "Room         %d\n",ch->in_room->vnum 	);
	fprintf( fp, "PkPdMkMd     0 0 0 0\n"			);
	fprintf( fp, "Weapons      0 0 0 0 0 0 0 0 0 0 0 0 0\n"	);
	fprintf( fp, "Spells       4 4 4 4 4\n"			);
	fprintf( fp, "Combat       0 0 0 0 0 0 0 0\n"		);
	fprintf( fp, "Stance       0 0 0 0 0 0 0 0 0 0 0\n"	);
	fprintf( fp, "Locationhp   0 0 0 0 0 0 0\n"		);
	fprintf( fp, "HpManaMove   %d %d %d %d %d %d\n",
	    (ch->pcdata->genes[0] > 10000 ? 10000 : ch->pcdata->genes[0]),
	    (ch->pcdata->genes[0] > 10000 ? 10000 : ch->pcdata->genes[0]),
	    (ch->pcdata->genes[1] > 10000 ? 10000 : ch->pcdata->genes[1]),
	    (ch->pcdata->genes[1] > 10000 ? 10000 : ch->pcdata->genes[1]),
	    (ch->pcdata->genes[2] > 10000 ? 10000 : ch->pcdata->genes[2]),
	    (ch->pcdata->genes[2] > 10000 ? 10000 : ch->pcdata->genes[2]));
	fprintf( fp, "Gold         0\n"				);
	fprintf( fp, "Exp          0\n"				);
	fprintf( fp, "Act          1600\n"   			);
	fprintf( fp, "Extra        32768\n"   			);
	fprintf( fp, "AffectedBy   0\n"				);
	fprintf( fp, "Position     7\n"				);
	fprintf( fp, "Practice     0\n"				);
	fprintf( fp, "SavingThrow  0\n"				);
	fprintf( fp, "Alignment    0\n"				);
	fprintf( fp, "Hitroll      0\n"				);
	fprintf( fp, "Damroll      0\n"				);
	fprintf( fp, "Armor        100\n"			);
	fprintf( fp, "Wimpy        0\n"				);
	fprintf( fp, "Deaf         0\n"				);
	fprintf( fp, "Password     %s~\n",ch->pcdata->pwd	);
	fprintf( fp, "Bamfin       ~\n"				);
	fprintf( fp, "Bamfout      ~\n"				);
	fprintf( fp, "Title         the mortal~\n"		);
	fprintf( fp, "Conception   ~\n"				);
	fprintf( fp, "Parents      %s~\n",ch->pcdata->cparents	);
	fprintf( fp, "Cparents     ~\n"				);
	fprintf( fp, "AttrPerm     %d %d %d %d %d\n",
	    ch->pcdata->perm_str,
	    ch->pcdata->perm_int,
	    ch->pcdata->perm_wis,
	    ch->pcdata->perm_dex,
	    ch->pcdata->perm_con );
	fprintf( fp, "AttrMod      0 0 0 0 0\n"			);
	fprintf( fp, "Quest        0\n"				);
	fprintf( fp, "Legend       0\n"				);
	fprintf( fp, "Wolf         0\n"				);
	fprintf( fp, "Rank         0\n"				);
	fprintf( fp, "Stage        0 0 0\n"			);
	fprintf( fp, "Wolfform     0 0\n"			);
	fprintf( fp, "Runes        0 0 0 0\n"			);
	fprintf( fp, "Disc         0 0 0 0 0 0 0 0 0 0 0\n"	);
	fprintf( fp, "Genes        0 0 0 0 0 0 0 0 0 0\n"	);
	fprintf( fp, "Power        0 0\n"			);
	fprintf( fp, "FakeCon      0 0 0 0 0 0 0 0\n"		);
	fprintf( fp, "Condition    0 48 48\n"			);
	fprintf( fp, "End\n\n" );
	fprintf( fp, "#END\n" );
	fclose( fp );
    }
    return;
}

void do_outcast( CHAR_DATA *ch, char *argument )
{
    CHAR_DATA *victim;
    char arg[MAX_INPUT_LENGTH];
    char outbuf[MAX_STRING_LENGTH];

    one_argument( argument, arg );

    if ( ch->pcdata->stats[UNI_GEN] != 2 && ch->trust < LEVEL_ARCHMAGE
	&& !IS_CLASS(ch, CLASS_HIGHLANDER) )
    {
	send_to_char("You are not a clan leader!\n\r", ch);
	return;
    }

    if ( arg[0] == '\0' )
    {
	send_to_char( "Outcast who?\n\r", ch );
	return;
    }

    if ( ( victim = get_char_world( ch, arg ) ) == NULL )
    {
	send_to_char( "They aren't here.\n\r", ch );
	return;
    }

    if ( victim == ch  && !IS_CLASS(ch, CLASS_HIGHLANDER) )
    {
	send_to_char("You cannot outcast youself!\n\r", ch);
        return;
    }

    if ( victim != ch && IS_CLASS(ch, CLASS_HIGHLANDER) )
    {
	send_to_char("You can only outcast yourself as a highlander.\r\n", ch);
	return;
    }

    if ( IS_NPC(victim) )
    {
	send_to_char( "Not on NPC's.\n\r", ch );
	return;
    }

    if ( (str_cmp(ch->clan, victim->clan) && !IS_CLASS(ch, CLASS_MAGE))
	|| (ch->class != victim->class) )
    {
	send_to_char("They are not of your clan.\r\n", ch);
	return;
    }

    if ( IS_CLASS(ch, CLASS_MAGE) && victim->trust < LEVEL_ARCHMAGE &&
	ch->pcdata->powers[MPOWER_RUNE0] != victim->pcdata->powers[MPOWER_RUNE0])
    {
	send_to_char("They belong to another circle of magic.\r\n", ch);
	return;
    }

    if ( IS_CLASS(ch,CLASS_DEMON) && IS_CLASS(victim, CLASS_DEMON))
    {
	sprintf(outbuf,"%s has been outcast from Hell.", victim->name);
	send_to_char("You have been outcast from Hell!\n\r", victim);
    }

    else if ( ( IS_CLASS(ch, CLASS_MAGE) && ch->trust >= LEVEL_ARCHMAGE )
	&& victim->trust < LEVEL_ARCHMAGE )
    {
  	sprintf(outbuf,"%s has been outcast from the Mages Guild.", victim->name);
	send_to_char("You have been outcast from the Mages Guild!\n\r", victim);
    }

    else if ( IS_CLASS(ch,CLASS_VAMPIRE) && IS_CLASS(victim, CLASS_VAMPIRE) )   
    {
  	sprintf(outbuf,"%s has been outcast from %s.", victim->name, victim->clan);

	send_to_char("You have been outcast from your clan!\r\n", victim);
    }

    else if ( IS_CLASS(ch, CLASS_WEREWOLF) && IS_CLASS(victim, CLASS_WEREWOLF) )
    {
  	sprintf(outbuf,"%s has been outcast from %s.", victim->name, victim->clan);
    }

    else if (IS_CLASS(ch, CLASS_HIGHLANDER)) /* Highlander outcast self */
    {
	if (ch->exp >= 100000)
	{
	    ch->exp -= 100000;
	    do_clearstats(ch, "");
	    sprintf(outbuf,"%s is no longer a Highlander!", victim->name);
            send_to_char("You reject the Highlander way of life.\n\r",ch);
	}
        else
	{
	    send_to_char("It costs 100,000 exp to reject the Highlander way of life.\n\r", ch);
	    return;
	}
    }

    unclass(victim);
    do_info(victim, outbuf);
    return;
}
