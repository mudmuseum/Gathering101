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


void do_bite( CHAR_DATA *ch, char *argument )
{
    CHAR_DATA *victim;
    char      arg [MAX_INPUT_LENGTH];
    char      buf [MAX_INPUT_LENGTH];
    int       clancount;
    bool      can_sire = FALSE;
    bool      outcast = FALSE;

    argument = one_argument( argument, arg );

    if (IS_NPC(ch)) return;

    if (!IS_CLASS(ch, CLASS_VAMPIRE) || (ch->pcdata->stats[UNI_GEN] < 1))
    {
	send_to_char("Huh?\n\r",ch);
	return;
    }

    if (ch->pcdata->stats[UNI_CURRENT] == -1) 
	ch->pcdata->stats[UNI_CURRENT] = ch->pcdata->stats[UNI_AFF]; 

/*
    if (IS_SET(ch->special,SPC_PRINCE)) can_sire = TRUE;
    if (IS_SET(ch->special,SPC_SIRE)) can_sire = TRUE;
    if (ch->pcdata->stats[UNI_GEN] < 3 || ch->pcdata->stats[UNI_GEN] > 6) can_sire = FALSE;

    if (ch->pcdata->stats[UNI_GEN] > 6) can_sire = FALSE;
    else can_sire = TRUE;
*/
   if (IS_SET(ch->special,SPC_PRINCE)) can_sire = TRUE;
    if (IS_SET(ch->special,SPC_SIRE)) can_sire = TRUE;
    if (ch->pcdata->stats[UNI_GEN] == 1 || ch->pcdata->stats[UNI_GEN] == 2) 
	can_sire = TRUE;

    if (!can_sire)
    {
	send_to_char("You are not able to create any childer.\n\r",ch);
	return;
    }

    if (!str_cmp(ch->clan,"") && ch->pcdata->stats[UNI_GEN] == 2)
    {
	send_to_char( "First you need to found a clan.\n\r", ch );
	return;
    }

    clancount = 0;
    if (IS_VAMPPASS(ch,VAM_PROTEAN))       clancount = clancount + 1;
    if (IS_VAMPPASS(ch,VAM_CELERITY))      clancount = clancount + 1;
    if (IS_VAMPPASS(ch,VAM_FORTITUDE))     clancount = clancount + 1;
    if (IS_VAMPPASS(ch,VAM_POTENCE))       clancount = clancount + 1;
    if (IS_VAMPPASS(ch,VAM_OBFUSCATE))     clancount = clancount + 1;
    if (IS_VAMPPASS(ch,VAM_OBTENEBRATION)) clancount = clancount + 1;
    if (IS_VAMPPASS(ch,VAM_SERPENTIS))     clancount = clancount + 1;
    if (IS_VAMPPASS(ch,VAM_AUSPEX))        clancount = clancount + 1;
    if (IS_VAMPPASS(ch,VAM_DOMINATE))      clancount = clancount + 1;
    if (IS_VAMPPASS(ch,VAM_PRESENCE))      clancount = clancount + 1;

    if ( clancount < 3 )
    {
	send_to_char( "First you need to master 3 disciplines.\n\r", ch );
	return;
    }

    if ( arg[0] == '\0' )
    {
	send_to_char( "Bite whom?\n\r", ch );
	return;
    }

    if ( ( victim = get_char_room( ch, arg ) ) == NULL )
    {
	send_to_char( "They aren't here.\n\r", ch );
	return;
    }

    if ( IS_NPC(victim) )
    {
	send_to_char( "Not on NPC's.\n\r", ch );
	return;
    }

    if ( IS_IMMORTAL(victim) )
    {
	send_to_char( "Not on Immortal's.\n\r", ch );
	return;
    }

    if ( ch == victim )
    {
	send_to_char( "You cannot bite yourself.\n\r", ch );
	return;
    }

    if (IS_CLASS(victim, CLASS_MAGE))
    {
	send_to_char( "You cannot bite mages.\n\r", ch );
	return;
    }

    if ( victim->level != LEVEL_AVATAR && !IS_IMMORTAL(victim) )
    {
	send_to_char( "You can only bite avatars.\n\r", ch );
	return;
    }

    if (IS_CLASS(victim, CLASS_WEREWOLF))
    {
	send_to_char( "You are unable to create werevamps!\n\r", ch );
	return;
    }

    if (IS_CLASS(victim, CLASS_VAMPIRE) && ch->beast != 100)
    {
	send_to_char( "But they are already a vampire!\n\r", ch );
	return;
    }

    if (IS_CLASS(victim, CLASS_DEMON))
    {
	send_to_char( "But they have no soul!\n\r", ch );
	return;
    }


    if (IS_CLASS(victim, CLASS_HIGHLANDER))
    {
	send_to_char( "You cannot turn highlanders into vampires.\n\r", ch );
	return;
    }

    if (!IS_IMMUNE(victim,IMM_VAMPIRE) && ch->beast != 100)
    {
	send_to_char( "You cannot bite an unwilling person.\n\r", ch );
	return;
    }

    if (!IS_VAMPAFF(ch,VAM_FANGS) && ch->beast != 100)
    {
	send_to_char("First you better get your fangs out!\n\r",ch);
	return;
    }

    if (IS_VAMPAFF(ch,VAM_DISGUISED) && ch->beast != 100)
    {
	send_to_char("You must reveal your true nature to bite someone.\n\r",ch);
	return;
    }

    if (ch->exp < 1000 && ch->beast != 100)
    {
	send_to_char("You cannot afford the 1000 exp to create a childe.\n\r",ch);
	return;
    }

    if (ch->beast == 100 || ch->pcdata->stats[UNI_RAGE] > 0)
    {
    	if (!IS_VAMPAFF(ch,VAM_FANGS) ) do_fangs(ch,"");
    	act("Your jaw opens wide and you leap hungrily at $N.", ch, NULL, victim, TO_CHAR);
    	act("$n's jaw opens wide and $e leaps hungrily at $N.", ch, NULL, victim, TO_NOTVICT);
    	act("$n's jaw opens wide and $e leaps hungrily at you.", ch, NULL, victim, TO_VICT);
	one_hit( ch, victim, -1, 0 );
	return;
    }

    if (ch->beast > 0)
	ch->beast += 1;
    ch->exp = ch->exp - 1000;
    act("You sink your teeth into $N.", ch, NULL, victim, TO_CHAR);
    act("$n sinks $s teeth into $N.", ch, NULL, victim, TO_NOTVICT);
    act("$n sinks $s teeth into your neck.", ch, NULL, victim, TO_VICT);
    victim->class=CLASS_VAMPIRE;
/*
    if ( victim->pcdata->stats[UNI_GEN] != 0 && (victim->pcdata->stats[UNI_GEN] <= ( ch->pcdata->stats[UNI_GEN] + 1 ) ) ) */
    if ( victim->pcdata->stats[UNI_GEN] != 0 )
    {
	save_char_obj(ch);
	save_char_obj(victim);
	send_to_char( "Your vampiric status has been restored.\n\r", victim );
	return;
    }
    if (IS_SET(ch->special,SPC_SIRE) || ch->pcdata->stats[UNI_GEN] < 3 || IS_SET(ch->special,SPC_PRINCE))
    {
	if (IS_SET(ch->special,SPC_SIRE))
	    REMOVE_BIT(ch->special,SPC_SIRE);
	if (IS_SET(ch->special, SPC_ANARCH) && strlen(ch->clan) < 2)
	    outcast = TRUE;
    }
    else outcast = TRUE;
    send_to_char( "You are now a vampire.\n\r", victim );    
    victim->pcdata->stats[UNI_GEN] = ch->pcdata->stats[UNI_GEN] + 1;
/*
    clan_table_bite(victim);
*/
    free_string(victim->lord);
    if (ch->pcdata->stats[UNI_GEN] == 1)
	victim->lord=str_dup(ch->name);
    else
    {
	sprintf(buf,"%s %s",ch->lord,ch->name);
	victim->lord=str_dup(buf);
    }
    if (ch->pcdata->stats[UNI_GEN] != 1)
    {
    	if (victim->pcdata->stats[UNI_CURRENT] == -1) 
		victim->pcdata->stats[UNI_CURRENT] = victim->pcdata->stats[UNI_AFF];

	/* Remove hp bonus from fortitude */
	if (IS_VAMPPASS(victim,VAM_FORTITUDE) && !IS_VAMPAFF(victim,VAM_FORTITUDE))
	{
	    victim->max_hit = victim->max_hit - 50;
	    victim->hit = victim->hit - 50;
	    if (victim->hit < 1) victim->hit = 1;
	}

	/* Remove any old powers they might have */
	if (IS_VAMPPASS(victim,VAM_PROTEAN))
	{REMOVE_BIT(victim->pcdata->stats[UNI_CURRENT], VAM_PROTEAN);
	 REMOVE_BIT(victim->pcdata->stats[UNI_AFF], VAM_PROTEAN);}
	if (IS_VAMPPASS(victim,VAM_CELERITY))
	{REMOVE_BIT(victim->pcdata->stats[UNI_CURRENT], VAM_CELERITY);
	 REMOVE_BIT(victim->pcdata->stats[UNI_AFF], VAM_CELERITY);}
	if (IS_VAMPPASS(victim,VAM_FORTITUDE))
	{REMOVE_BIT(victim->pcdata->stats[UNI_CURRENT], VAM_FORTITUDE);
	 REMOVE_BIT(victim->pcdata->stats[UNI_AFF], VAM_FORTITUDE);}
	if (IS_VAMPPASS(victim,VAM_POTENCE))
	{REMOVE_BIT(victim->pcdata->stats[UNI_CURRENT], VAM_POTENCE);
	 REMOVE_BIT(victim->pcdata->stats[UNI_AFF], VAM_POTENCE);}
	if (IS_VAMPPASS(victim,VAM_OBFUSCATE))
	{REMOVE_BIT(victim->pcdata->stats[UNI_CURRENT], VAM_OBFUSCATE);
	 REMOVE_BIT(victim->pcdata->stats[UNI_AFF], VAM_OBFUSCATE);}
	if (IS_VAMPPASS(victim,VAM_OBTENEBRATION))
	{REMOVE_BIT(victim->pcdata->stats[UNI_CURRENT], VAM_OBTENEBRATION);
	 REMOVE_BIT(victim->pcdata->stats[UNI_AFF], VAM_OBTENEBRATION);}
	if (IS_VAMPPASS(victim,VAM_SERPENTIS))
	{REMOVE_BIT(victim->pcdata->stats[UNI_CURRENT], VAM_SERPENTIS);
	 REMOVE_BIT(victim->pcdata->stats[UNI_AFF], VAM_SERPENTIS);}
	if (IS_VAMPPASS(victim,VAM_AUSPEX))
	{REMOVE_BIT(victim->pcdata->stats[UNI_CURRENT], VAM_AUSPEX);
	 REMOVE_BIT(victim->pcdata->stats[UNI_AFF], VAM_AUSPEX);}
	if (IS_VAMPPASS(victim,VAM_DOMINATE))
	{REMOVE_BIT(victim->pcdata->stats[UNI_CURRENT], VAM_DOMINATE);
	 REMOVE_BIT(victim->pcdata->stats[UNI_AFF], VAM_DOMINATE);}
	if (IS_VAMPPASS(victim,VAM_PRESENCE))
	{REMOVE_BIT(victim->pcdata->stats[UNI_CURRENT], VAM_PRESENCE);
	 REMOVE_BIT(victim->pcdata->stats[UNI_AFF], VAM_PRESENCE);}
	if (!outcast)
	{
	    free_string(victim->clan);
	    victim->clan=str_dup(ch->clan);
	}
	/* Give the vampire the base powers of their sire */
	if (IS_VAMPPASS(ch,VAM_FORTITUDE) && !IS_VAMPAFF(victim,VAM_FORTITUDE))
	{
	    victim->max_hit = victim->max_hit + 50;
	    victim->hit = victim->hit + 50;
	}
	if (IS_VAMPPASS(ch,VAM_PROTEAN))
	{SET_BIT(victim->pcdata->stats[UNI_CURRENT], VAM_PROTEAN);
	 SET_BIT(victim->pcdata->stats[UNI_AFF], VAM_PROTEAN);}
	if (IS_VAMPPASS(ch,VAM_CELERITY))
	{SET_BIT(victim->pcdata->stats[UNI_CURRENT], VAM_CELERITY);
	 SET_BIT(victim->pcdata->stats[UNI_AFF], VAM_CELERITY);}
	if (IS_VAMPPASS(ch,VAM_FORTITUDE))
	{SET_BIT(victim->pcdata->stats[UNI_CURRENT], VAM_FORTITUDE);
	 SET_BIT(victim->pcdata->stats[UNI_AFF], VAM_FORTITUDE);}
	if (IS_VAMPPASS(ch,VAM_POTENCE))
	{SET_BIT(victim->pcdata->stats[UNI_CURRENT], VAM_POTENCE);
	 SET_BIT(victim->pcdata->stats[UNI_AFF], VAM_POTENCE);}
	if (IS_VAMPPASS(ch,VAM_OBFUSCATE))
	{SET_BIT(victim->pcdata->stats[UNI_CURRENT], VAM_OBFUSCATE);
	 SET_BIT(victim->pcdata->stats[UNI_AFF], VAM_OBFUSCATE);}
	if (IS_VAMPPASS(ch,VAM_OBTENEBRATION))
	{SET_BIT(victim->pcdata->stats[UNI_CURRENT], VAM_OBTENEBRATION);
	 SET_BIT(victim->pcdata->stats[UNI_AFF], VAM_OBTENEBRATION);}
	if (IS_VAMPPASS(ch,VAM_SERPENTIS))
	{SET_BIT(victim->pcdata->stats[UNI_CURRENT], VAM_SERPENTIS);
	 SET_BIT(victim->pcdata->stats[UNI_AFF], VAM_SERPENTIS);}
	if (IS_VAMPPASS(ch,VAM_AUSPEX))
	{SET_BIT(victim->pcdata->stats[UNI_CURRENT], VAM_AUSPEX);
	 SET_BIT(victim->pcdata->stats[UNI_AFF], VAM_AUSPEX);}
	if (IS_VAMPPASS(ch,VAM_DOMINATE))
	{SET_BIT(victim->pcdata->stats[UNI_CURRENT], VAM_DOMINATE);
	 SET_BIT(victim->pcdata->stats[UNI_AFF], VAM_DOMINATE);}
	if (IS_VAMPPASS(ch,VAM_PRESENCE))
	{SET_BIT(victim->pcdata->stats[UNI_CURRENT], VAM_PRESENCE);
	 SET_BIT(victim->pcdata->stats[UNI_AFF], VAM_PRESENCE);}
    }
    save_char_obj(ch);
    save_char_obj(victim);
    return;
}

void do_clandisc( CHAR_DATA *ch, char *argument )
{
    char arg [MAX_INPUT_LENGTH];
    char buf [MAX_STRING_LENGTH];
    int clancount = 0;
    int clanmax = 10;
    int cost = 0;
    argument = one_argument( argument, arg );

    if (IS_NPC(ch)) return;

    if ( !IS_CLASS(ch,CLASS_VAMPIRE) )
    {
	send_to_char( "Huh?\n\r", ch );
	return;
    }

    if (ch->pcdata->stats[UNI_CURRENT] == -1) 
	ch->pcdata->stats[UNI_CURRENT] = ch->pcdata->stats[UNI_AFF];

    if (IS_VAMPAFF(ch,VAM_PROTEAN) || IS_VAMPPASS(ch,VAM_PROTEAN))
	clancount = clancount + 1;
    if (IS_VAMPAFF(ch,VAM_CELERITY) || IS_VAMPPASS(ch,VAM_CELERITY))
	clancount = clancount + 1;
    if (IS_VAMPAFF(ch,VAM_FORTITUDE) || IS_VAMPPASS(ch,VAM_FORTITUDE))
	clancount = clancount + 1;
    if (IS_VAMPAFF(ch,VAM_POTENCE) || IS_VAMPPASS(ch,VAM_POTENCE))
	clancount = clancount + 1;
    if (IS_VAMPAFF(ch,VAM_OBFUSCATE) || IS_VAMPPASS(ch,VAM_OBFUSCATE))
	clancount = clancount + 1;
    if (IS_VAMPAFF(ch,VAM_OBTENEBRATION) || IS_VAMPPASS(ch,VAM_OBTENEBRATION))
	clancount = clancount + 1;
    if (IS_VAMPAFF(ch,VAM_SERPENTIS) || IS_VAMPPASS(ch,VAM_SERPENTIS))
	clancount = clancount + 1;
    if (IS_VAMPAFF(ch,VAM_AUSPEX) || IS_VAMPPASS(ch,VAM_AUSPEX))
	clancount = clancount + 1;
    if (IS_VAMPAFF(ch,VAM_DOMINATE) || IS_VAMPPASS(ch,VAM_DOMINATE))
	clancount = clancount + 1;
    if (IS_VAMPAFF(ch,VAM_PRESENCE) || IS_VAMPPASS(ch,VAM_PRESENCE))
	clancount = clancount + 1;

    if (clancount < 3) cost = 0;
	else cost = (clancount - 2) * 10;
    if (cost > 50) cost = 50;

    if ( arg[0] == '\0' )
    {
	send_to_char("Current powers:",ch);
	if (IS_VAMPAFF(ch,VAM_PROTEAN) && !IS_VAMPPASS(ch,VAM_PROTEAN))
	    send_to_char(" Protean",ch);
	else if (IS_VAMPAFF(ch,VAM_PROTEAN))
	    send_to_char(" PROTEAN",ch);
	if (IS_VAMPAFF(ch,VAM_CELERITY) && !IS_VAMPPASS(ch,VAM_CELERITY))
	    send_to_char(" Celerity",ch);
	else if (IS_VAMPAFF(ch,VAM_CELERITY))
	    send_to_char(" CELERITY",ch);
	if (IS_VAMPAFF(ch,VAM_FORTITUDE) && !IS_VAMPPASS(ch,VAM_FORTITUDE))
	    send_to_char(" Fortitude",ch);
	else if (IS_VAMPAFF(ch,VAM_FORTITUDE))
	    send_to_char(" FORTITUDE",ch);
	if (IS_VAMPAFF(ch,VAM_POTENCE) && !IS_VAMPPASS(ch,VAM_POTENCE))
	    send_to_char(" Potence",ch);
	else if (IS_VAMPAFF(ch,VAM_POTENCE))
	    send_to_char(" POTENCE",ch);
	if (IS_VAMPAFF(ch,VAM_OBFUSCATE) && !IS_VAMPPASS(ch,VAM_OBFUSCATE))
	    send_to_char(" Obfuscate",ch);
	else if (IS_VAMPAFF(ch,VAM_OBFUSCATE))
	    send_to_char(" OBFUSCATE",ch);
	if (IS_VAMPAFF(ch,VAM_OBTENEBRATION) && !IS_VAMPPASS(ch,VAM_OBTENEBRATION))
	    send_to_char(" Obtenebration",ch);
	else if (IS_VAMPAFF(ch,VAM_OBTENEBRATION))
	    send_to_char(" OBTENEBRATION",ch);
	if (IS_VAMPAFF(ch,VAM_SERPENTIS) && !IS_VAMPPASS(ch,VAM_SERPENTIS))
	    send_to_char(" Serpentis",ch);
	else if (IS_VAMPAFF(ch,VAM_SERPENTIS))
	    send_to_char(" SERPENTIS",ch);
	if (IS_VAMPAFF(ch,VAM_AUSPEX) && !IS_VAMPPASS(ch,VAM_AUSPEX))
	    send_to_char(" Auspex",ch);
	else if (IS_VAMPAFF(ch,VAM_AUSPEX))
	    send_to_char(" AUSPEX",ch);
	if (IS_VAMPAFF(ch,VAM_DOMINATE) && !IS_VAMPPASS(ch,VAM_DOMINATE))
	    send_to_char(" Dominate",ch);
	else if (IS_VAMPAFF(ch,VAM_DOMINATE))
	    send_to_char(" DOMINATE",ch);
	if (IS_VAMPAFF(ch,VAM_PRESENCE) && !IS_VAMPPASS(ch,VAM_PRESENCE))
	    send_to_char(" Presence",ch);
	else if (IS_VAMPAFF(ch,VAM_PRESENCE))
	    send_to_char(" PRESENCE",ch);
    	if (!IS_VAMPAFF(ch,VAM_PROTEAN) && !IS_VAMPAFF(ch,VAM_CELERITY) && 
	    !IS_VAMPAFF(ch,VAM_FORTITUDE) && !IS_VAMPAFF(ch,VAM_POTENCE) &&
	    !IS_VAMPAFF(ch,VAM_OBFUSCATE) && !IS_VAMPAFF(ch,VAM_AUSPEX) &&
	    !IS_VAMPAFF(ch,VAM_OBTENEBRATION) && !IS_VAMPAFF(ch,VAM_SERPENTIS) &&
    	    !IS_VAMPPASS(ch,VAM_PROTEAN) && !IS_VAMPPASS(ch,VAM_CELERITY) && 
	    !IS_VAMPPASS(ch,VAM_FORTITUDE) && !IS_VAMPPASS(ch,VAM_POTENCE) &&
	    !IS_VAMPPASS(ch,VAM_OBFUSCATE) && !IS_VAMPPASS(ch,VAM_AUSPEX) &&
	    !IS_VAMPPASS(ch,VAM_DOMINATE) && !IS_VAMPPASS(ch,VAM_PRESENCE) &&
	    !IS_VAMPPASS(ch,VAM_OBTENEBRATION) && !IS_VAMPPASS(ch,VAM_SERPENTIS))
	    send_to_char(" None",ch);
	send_to_char(".\n\r",ch);
	if (clancount < clanmax)
	{
	    sprintf(buf,"It will cost you %d primal to gain another discipline.\n\r",cost);
	    send_to_char(buf,ch);
	}
	if (cost <= ch->practice) send_to_char("Select from:",ch); else return;
	if (!IS_VAMPAFF(ch,VAM_PROTEAN)) send_to_char(" Protean",ch);
	if (!IS_VAMPAFF(ch,VAM_CELERITY)) send_to_char(" Celerity",ch);
	if (!IS_VAMPAFF(ch,VAM_FORTITUDE)) send_to_char(" Fortitude",ch);
	if (!IS_VAMPAFF(ch,VAM_POTENCE)) send_to_char(" Potence",ch);
	if (!IS_VAMPAFF(ch,VAM_OBFUSCATE)) send_to_char(" Obfuscate",ch);
	if (!IS_VAMPAFF(ch,VAM_OBTENEBRATION)) send_to_char(" Obtenebration",ch);
	if (!IS_VAMPAFF(ch,VAM_SERPENTIS)) send_to_char(" Serpentis",ch);
	if (!IS_VAMPAFF(ch,VAM_AUSPEX)) send_to_char(" Auspex",ch);
	if (!IS_VAMPAFF(ch,VAM_DOMINATE)) send_to_char(" Dominate",ch);
	if (!IS_VAMPAFF(ch,VAM_PRESENCE)) send_to_char(" Presence",ch);
	send_to_char(".\n\r",ch);
	return;
    }
    if ( clancount >= clanmax )
    {
	if ( !str_cmp(arg,"protean") && (IS_VAMPAFF(ch,VAM_PROTEAN) || IS_VAMPPASS(ch,VAM_PROTEAN)))
	    send_to_char( "Powers: Nightsight, Claws, Change.\n\r", ch );
	else if ( !str_cmp(arg,"celerity") && (IS_VAMPAFF(ch,VAM_CELERITY) || IS_VAMPPASS(ch,VAM_CELERITY)) )
	    send_to_char( "Powers: An extra attack, Reduced move cost for spells.\n\r", ch );
	else if ( !str_cmp(arg,"fortitude") && (IS_VAMPAFF(ch,VAM_FORTITUDE) || IS_VAMPPASS(ch,VAM_FORTITUDE)) )
	    send_to_char( "Powers: 1-100% damage reduction, +50 one time hp bonus.\n\r", ch );
	else if ( !str_cmp(arg,"potence") && (IS_VAMPAFF(ch,VAM_POTENCE) || IS_VAMPPASS(ch,VAM_POTENCE)) )
	    send_to_char( "Powers: 150% normal damage in combat.\n\r", ch );
	else if ( !str_cmp(arg,"obfuscate") && (IS_VAMPAFF(ch,VAM_OBFUSCATE) || IS_VAMPPASS(ch,VAM_OBFUSCATE)) )
	    send_to_char( "Powers: Mask, Mortal, Shield.\n\r", ch );
	else if ( !str_cmp(arg,"obtenebration") && (IS_VAMPAFF(ch,VAM_OBTENEBRATION) || IS_VAMPPASS(ch,VAM_OBTENEBRATION)) )
	    send_to_char( "Powers: Shadowplane, Shadowsight, Nightsight.\n\r", ch );
	else if ( !str_cmp(arg,"serpentis") && (IS_VAMPAFF(ch,VAM_SERPENTIS) || IS_VAMPPASS(ch,VAM_SERPENTIS)) )
	    send_to_char( "Powers: Darkheart, Serpent, Poison, Nightsight.\n\r", ch );
	else if ( !str_cmp(arg,"auspex") && (IS_VAMPAFF(ch,VAM_AUSPEX) || IS_VAMPPASS(ch,VAM_AUSPEX)) )
	    send_to_char( "Powers: Truesight, Scry, Readaura.\n\r", ch );
	else if ( !str_cmp(arg,"dominate") && (IS_VAMPAFF(ch,VAM_DOMINATE) || IS_VAMPPASS(ch,VAM_DOMINATE)) )
	    send_to_char( "Powers: Evileye, Command, Shield, Ghoul.\n\r", ch );
	else if ( !str_cmp(arg,"presence") && (IS_VAMPAFF(ch,VAM_PRESENCE) || IS_VAMPPASS(ch,VAM_PRESENCE)) )
	    send_to_char( "Powers: Majesty.\n\r", ch );
	else
	    send_to_char( "You don't know any such Discipline.\n\r", ch );
	return;
    }
    if ( !str_cmp(arg,"protean") )
    {
	if (IS_VAMPAFF(ch,VAM_PROTEAN) || IS_VAMPPASS(ch,VAM_PROTEAN)
	    || cost > ch->practice)
	{
	    send_to_char( "Powers: Nightsight, Claws, Change.\n\r", ch );
	    return;
	}
	send_to_char( "You master the discipline of Protean.\n\r", ch );
	if (clancount < 3) SET_BIT(ch->pcdata->stats[UNI_CURRENT],VAM_PROTEAN);
	SET_BIT(ch->pcdata->stats[UNI_AFF],VAM_PROTEAN);
	ch->practice -= cost;
	return;
    }
    else if ( !str_cmp(arg,"celerity") )
    {
	if (IS_VAMPAFF(ch,VAM_CELERITY) || IS_VAMPPASS(ch,VAM_CELERITY)
	    || cost > ch->practice)
	{
	    send_to_char( "Powers: An extra attack, Reduced move cost for spells.\n\r", ch );
	    return;
	}
	send_to_char( "You master the discipline of Celerity.\n\r", ch );
	if (clancount < 3) SET_BIT(ch->pcdata->stats[UNI_CURRENT],VAM_CELERITY);
	SET_BIT(ch->pcdata->stats[UNI_AFF],VAM_CELERITY);
	ch->practice -= cost;
	return;
    }
    else if ( !str_cmp(arg,"fortitude") )
    {
	if (IS_VAMPAFF(ch,VAM_FORTITUDE) || IS_VAMPPASS(ch,VAM_FORTITUDE)
	    || cost > ch->practice)
	{
	    send_to_char( "Powers: 1-100% damage reduction, +50 one time hp bonus.\n\r", ch );
	    return;
	}
	send_to_char( "You master the discipline of Fortitude.\n\r", ch );
	if (clancount < 3) SET_BIT(ch->pcdata->stats[UNI_CURRENT],VAM_FORTITUDE);
	SET_BIT(ch->pcdata->stats[UNI_AFF],VAM_FORTITUDE);
	ch->practice -= cost;
	ch->max_hit = ch->max_hit + 50;
	ch->hit = ch->hit + 50;
	return;
    }
    else if ( !str_cmp(arg,"potence") )
    {
	if (IS_VAMPAFF(ch,VAM_POTENCE) || IS_VAMPPASS(ch,VAM_POTENCE)
	    || cost > ch->practice)
	{
	    send_to_char( "Powers: 150% normal damage in combat.\n\r", ch );
	    return;
	}
	send_to_char( "You master the discipline of Potence.\n\r", ch );
	if (clancount < 3) SET_BIT(ch->pcdata->stats[UNI_CURRENT],VAM_POTENCE);
	SET_BIT(ch->pcdata->stats[UNI_AFF],VAM_POTENCE);
	ch->practice -= cost;
	return;
    }
    else if ( !str_cmp(arg,"obfuscate") )
    {
	if (IS_VAMPAFF(ch,VAM_OBFUSCATE) || IS_VAMPPASS(ch,VAM_OBFUSCATE)
	    || cost > ch->practice)
	{
	    send_to_char( "Powers: Mask, Mortal, Shield.\n\r", ch );
	    return;
	}
	send_to_char( "You master the discipline of Obfuscate.\n\r", ch );
	if (clancount < 3) SET_BIT(ch->pcdata->stats[UNI_CURRENT],VAM_OBFUSCATE);
	SET_BIT(ch->pcdata->stats[UNI_AFF],VAM_OBFUSCATE);
	ch->practice -= cost;
	return;
    }
    else if ( !str_cmp(arg,"obtenebration") )
    {
	if (IS_VAMPAFF(ch,VAM_OBTENEBRATION) || IS_VAMPPASS(ch,VAM_OBTENEBRATION)
	    || cost > ch->practice)
	{
	    send_to_char( "Powers: Shadowplane, Shadowsight, Nightsight.\n\r", ch );
	    return;
	}
	send_to_char( "You master the discipline of Obtenebration.\n\r", ch );
	if (clancount < 3) SET_BIT(ch->pcdata->stats[UNI_CURRENT],VAM_OBTENEBRATION); 
	SET_BIT(ch->pcdata->stats[UNI_AFF],VAM_OBTENEBRATION);
	ch->practice -= cost;
	return;
    }
    else if ( !str_cmp(arg,"serpentis") )
    {
	if (IS_VAMPAFF(ch,VAM_SERPENTIS) || IS_VAMPPASS(ch,VAM_SERPENTIS)
	    || cost > ch->practice)
	{
	    send_to_char( "Powers: Darkheart, Serpent, Poison, Nightsight.\n\r", ch );
	    return;
	}
	send_to_char( "You master the discipline of Serpentis.\n\r", ch );
	if (clancount < 3) SET_BIT(ch->pcdata->stats[UNI_CURRENT],VAM_SERPENTIS);
	SET_BIT(ch->pcdata->stats[UNI_AFF],VAM_SERPENTIS);
	ch->practice -= cost;
	return;
    }
    else if ( !str_cmp(arg,"auspex") )
    {
	if (IS_VAMPAFF(ch,VAM_AUSPEX) || IS_VAMPPASS(ch,VAM_AUSPEX)
	    || cost > ch->practice)
	{
	    send_to_char( "Powers: Truesight, Scry, Readaura.\n\r", ch );
	    return;
	}
	send_to_char( "You master the discipline of Auspex.\n\r", ch );
	if (clancount < 3) SET_BIT(ch->pcdata->stats[UNI_CURRENT],VAM_AUSPEX);
	SET_BIT(ch->pcdata->stats[UNI_AFF],VAM_AUSPEX);
	ch->practice -= cost;
	return;
    }
    else if ( !str_cmp(arg,"dominate") )
    {
	if (IS_VAMPAFF(ch,VAM_DOMINATE) || IS_VAMPPASS(ch,VAM_DOMINATE)
	    || cost > ch->practice)
	{
	    send_to_char( "Powers: Evileye, Command, Shield, Ghoul.\n\r", ch );
	    return;
	}
	send_to_char( "You master the discipline of Dominate.\n\r", ch );
	if (clancount < 3) SET_BIT(ch->pcdata->stats[UNI_CURRENT],VAM_DOMINATE);
	SET_BIT(ch->pcdata->stats[UNI_AFF],VAM_DOMINATE);
	ch->practice -= cost;
	return;
    }
    else if ( !str_cmp(arg,"presence") )
    {
	if (IS_VAMPAFF(ch,VAM_PRESENCE) || IS_VAMPPASS(ch,VAM_PRESENCE)
	    || cost > ch->practice)
	{
	    send_to_char( "Powers: Majesty.\n\r", ch );
	    return;
	}
	send_to_char( "You master the discipline of Presence.\n\r", ch );
	if (clancount < 3) SET_BIT(ch->pcdata->stats[UNI_CURRENT],VAM_PRESENCE);
	SET_BIT(ch->pcdata->stats[UNI_AFF],VAM_PRESENCE);
	ch->practice -= cost;
	return;
    }
    else
	send_to_char( "No such discipline.\n\r", ch );
    return;
}

void do_vclan( CHAR_DATA *ch, char *argument )
{
    char buf[MAX_STRING_LENGTH];
    char arg[MAX_INPUT_LENGTH];
    CHAR_DATA *gch;

    one_argument( argument, arg );

    if (IS_NPC(ch)) return;
    if (!IS_CLASS(ch, CLASS_VAMPIRE) )
    {
	send_to_char("Huh?\n\r",ch);
	return;
    }

    if (strlen(ch->clan) < 2 || IS_SET(ch->special, SPC_ANARCH) )
    {
	send_to_char("But you don't belong to any clan!\n\r",ch);
	return;
    }

    sprintf( buf, "The %s clan:\n\r", ch->clan );
    send_to_char( buf, ch );
    send_to_char("[      Name      ] [ Gen ] [ Hits  % ] [ Mana  % ] [ Move  % ] [  Exp  ] [Blood]\n\r", ch );
    for ( gch = char_list; gch != NULL; gch = gch->next )
    {
	if ( IS_NPC(gch) ) continue;
	if ( !IS_CLASS(gch, CLASS_VAMPIRE) ) continue;
	if ( !str_cmp(ch->clan,gch->clan) )
	{
	    sprintf( buf,
	    "[%-16s] [  %d  ] [%-6d%3d] [%-6d%3d] [%-6d%3d] [%7d] [ %3d ]\n\r",
		capitalize( gch->name ),
		gch->pcdata->stats[UNI_GEN],
		gch->hit,  (gch->hit  * 100 / gch->max_hit ),
		gch->mana, (gch->mana * 100 / gch->max_mana),
		gch->move, (gch->move * 100 / gch->max_move),
		gch->exp, gch->pcdata->condition[COND_THIRST] );
		send_to_char( buf, ch );
	}
    }
    return;
}

void do_tradition( CHAR_DATA *ch, char *argument )
{
    char      arg1 [MAX_INPUT_LENGTH];
    char      arg2 [MAX_INPUT_LENGTH];
    char      arg3 [MAX_INPUT_LENGTH];
    char      arg4 [MAX_INPUT_LENGTH];
    char      arg5 [MAX_INPUT_LENGTH];
    char      arg6 [MAX_INPUT_LENGTH];
    char      buf  [MAX_STRING_LENGTH];
    char      buf2 [MAX_INPUT_LENGTH];
    char      buf3 [MAX_INPUT_LENGTH];

    argument = one_argument( argument, arg1 );
    argument = one_argument( argument, arg2 );
    argument = one_argument( argument, arg3 );
    argument = one_argument( argument, arg4 );
    argument = one_argument( argument, arg5 );
    argument = one_argument( argument, arg6 );

    if (IS_NPC(ch)) return;

    if (arg1 != '\0') arg1[0] = UPPER(arg1[0]);
    if (arg2 != '\0') arg2[0] = UPPER(arg2[0]);
    if (arg3 != '\0') arg3[0] = UPPER(arg3[0]);
    if (arg4 != '\0') arg4[0] = UPPER(arg4[0]);
    if (arg5 != '\0') arg5[0] = UPPER(arg5[0]);
    if (arg6 != '\0') arg6[0] = UPPER(arg6[0]);

    if (!str_cmp(arg1,"kavir")) strcpy(arg1,"KaVir");

    if (!IS_CLASS(ch, CLASS_VAMPIRE) || (ch->pcdata->stats[UNI_GEN] < 1) || (ch->pcdata->stats[UNI_GEN] > 7))
    {
	send_to_char("Huh?\n\r",ch);
	return;
    }
/*
    if ( !str_cmp(ch->clan,"") && ch->pcdata->stats[UNI_GEN] == 2 )
    {
	send_to_char( "Not until you've created your own clan!\n\r", ch );
	return;
    }
*/
    if      (ch->pcdata->stats[UNI_GEN] == 2)                   sprintf(buf3,"Antediluvian");
    else if (ch->pcdata->rank == AGE_NEONATE)    sprintf(buf3,"Neonate");
    else if (ch->pcdata->rank == AGE_ANCILLA)    sprintf(buf3,"Ancilla");
    else if (ch->pcdata->rank == AGE_ELDER)      sprintf(buf3,"Elder");
    else if (ch->pcdata->rank == AGE_METHUSELAH) sprintf(buf3,"Methuselah");
    else                                         sprintf(buf3,"Childe");

    if      (ch->pcdata->stats[UNI_GEN] == 7) sprintf(buf2,"Seventh");
    else if (ch->pcdata->stats[UNI_GEN] == 6) sprintf(buf2,"Sixth");
    else if (ch->pcdata->stats[UNI_GEN] == 5) sprintf(buf2,"Fifth");
    else if (ch->pcdata->stats[UNI_GEN] == 4) sprintf(buf2,"Fourth");
    else if (ch->pcdata->stats[UNI_GEN] == 3) sprintf(buf2,"Third");
    else if (ch->pcdata->stats[UNI_GEN] == 2) sprintf(buf2,"Second");
    if (ch->pcdata->stats[UNI_GEN] == 1)
	sprintf(buf,"As is the tradition, I recite the lineage of %s, Sire of all Kindred.",ch->name);
    else
	sprintf(buf,"As is the tradition, I recite the lineage of %s, %s of the %s Generation.",ch->name,buf3,buf2);
    do_say(ch,buf);
    if ( ch->pcdata->stats[UNI_GEN] != 1 )
    {
	if      (ch->pcdata->stats[UNI_GEN] == 7) sprintf(buf2,"%s",arg6);
	else if (ch->pcdata->stats[UNI_GEN] == 6) sprintf(buf2,"%s",arg5);
	else if (ch->pcdata->stats[UNI_GEN] == 5) sprintf(buf2,"%s",arg4);
	else if (ch->pcdata->stats[UNI_GEN] == 4) sprintf(buf2,"%s",arg3);
	else if (ch->pcdata->stats[UNI_GEN] == 3) sprintf(buf2,"%s",arg2);
	else if (ch->pcdata->stats[UNI_GEN] == 2) sprintf(buf2,"%s",arg1);

	if (IS_SET(ch->special, SPC_ANARCH) || strlen(ch->clan) < 2)
	    sprintf(buf,"My name is %s.  I am of no clan.  My sire is %s.", ch->name,buf2);
	else if (ch->pcdata->stats[UNI_GEN] == 2)
	    sprintf(buf,"My name is %s. I founded %s.  My sire is %s.", ch->name,ch->clan,buf2);
	else
	    sprintf(buf,"My name is %s.  I am of %s.  My sire is %s.", ch->name,ch->clan,buf2);
	do_say(ch,buf);
    }
    if ( arg6[0] != '\0' )
    {
	sprintf(buf,"My name is %s.  My sire is %s.",
	arg6,arg5);
	do_say(ch,buf);
    }
    if ( arg5[0] != '\0' )
    {
	sprintf(buf,"My name is %s.  My sire is %s.",
	arg5,arg4);
	do_say(ch,buf);
    }
    if ( arg4[0] != '\0' )
    {
	sprintf(buf,"My name is %s.  My sire is %s.",
	arg4,arg3);
	do_say(ch,buf);
    }
    if ( arg3[0] != '\0' )
    {
	sprintf(buf,"My name is %s.  My sire is %s.",
	arg3,arg2);
	do_say(ch,buf);
    }
    if ( arg2[0] != '\0' )
    {
	sprintf(buf,"My name is %s.  My sire is %s.",arg2,arg1);
	do_say(ch,buf);
    }
    if ( ch->pcdata->stats[UNI_GEN] == 1 )
	sprintf(buf,"My name is %s.  All Kindred are my childer.",ch->name);
    else
	sprintf(buf,"My name is %s.  All Kindred are my childer.",arg1);
    do_say(ch,buf);
    if ( ch->pcdata->stats[UNI_GEN] == 7 ) sprintf(buf,"My name is %s, childe of %s, childe of %s, childe of %s, childe of %s, childe of %s, childe of %s.  Recognize my lineage.",ch->name,arg6,arg5,arg4,arg3,arg2,arg1);
    if ( ch->pcdata->stats[UNI_GEN] == 6 ) sprintf(buf,"My name is %s, childe of %s, childe of %s, childe of %s, childe of %s, childe of %s.  Recognize my lineage.",ch->name,arg5,arg4,arg3,arg2,arg1);
    if ( ch->pcdata->stats[UNI_GEN] == 5 ) sprintf(buf,"My name is %s, childe of %s, childe of %s, childe of %s, childe of %s.  Recognize my lineage.",ch->name,arg4,arg3,arg2,arg1);
    if ( ch->pcdata->stats[UNI_GEN] == 4 ) sprintf(buf,"My name is %s, childe of %s, childe of %s, childe of %s.  Recognize my lineage.",ch->name,arg3,arg2,arg1);
    if ( ch->pcdata->stats[UNI_GEN] == 3 ) sprintf(buf,"My name is %s, childe of %s, childe of %s.  Recognize my lineage.",ch->name,arg2,arg1);
    if ( ch->pcdata->stats[UNI_GEN] == 2 ) sprintf(buf,"My name is %s, childe of %s.  Recognize my lineage.",ch->name,arg1);
    if ( ch->pcdata->stats[UNI_GEN] == 1 ) sprintf(buf,"My name is %s.  Recognize my lineage.",ch->name);
    do_say(ch,buf);
    return;
}

void do_anarch( CHAR_DATA *ch, char *argument )
{
    char      arg [MAX_INPUT_LENGTH];
    char      buf [MAX_INPUT_LENGTH];

    argument = one_argument( argument, arg );

    if (IS_NPC(ch)) return;

    if (!IS_CLASS(ch, CLASS_VAMPIRE) )
    {
	send_to_char("Huh?\n\r",ch);
	return;
    }

    if (strlen(ch->clan) > 1)
    {
	send_to_char("But you are already in a clan!\n\r",ch);
	return;
    }

    if (IS_SET(ch->special, SPC_INCONNU))
    {
	send_to_char("But you are already an Inconnu!\n\r",ch);
	return;
    }

    if ( IS_SET(ch->special, SPC_ANARCH) )
    {
	send_to_char("You are no longer an Anarch.\n\r",ch);
	sprintf(buf,"%s is no longer an Anarch!",ch->name);
	do_info(ch,buf);
	REMOVE_BIT(ch->special, SPC_ANARCH);
	return;
    }
    send_to_char("You are now an Anarch.\n\r",ch);
    sprintf(buf,"%s is now an Anarch!",ch->name);
    do_info(ch,buf);
    SET_BIT(ch->special, SPC_ANARCH);
    return;
}

void do_inconn( CHAR_DATA *ch, char *argument )
{
    send_to_char( "If you want to INCONNU, spell it out.\n\r", ch );
    return;
}

void do_inconnu( CHAR_DATA *ch, char *argument )
{
    char      arg [MAX_INPUT_LENGTH];
    char      buf [MAX_INPUT_LENGTH];

    argument = one_argument( argument, arg );

    if (IS_NPC(ch)) return;

    if (!IS_CLASS(ch, CLASS_VAMPIRE) )
    {
	send_to_char("Huh?\n\r",ch);
	return;
    }

    if (strlen(ch->clan) > 1 && ch->pcdata->stats[UNI_GEN] < 3)
    {
	send_to_char("You cannot abandon your clan!\n\r",ch);
	return;
    }

    if (((get_age(ch) - 17) * 2 ) < 200 && ch->pcdata->stats[UNI_GEN] > 2)
    {
	send_to_char("You must be at least an Elder to become an Inconnu.\n\r",ch);
	return;
    }

    if (IS_SET(ch->special, SPC_INCONNU))
    {
	send_to_char("But you are already an Inconnu!\n\r",ch);
	return;
    }

    if (ch->exp < 1000000)
    {
	send_to_char("It costs 1000000 exp to become an Inconnu.\n\r",ch);
	return;
    }

    if (IS_SET(ch->special, SPC_ANARCH)) REMOVE_BIT(ch->special, SPC_ANARCH);
    free_string(ch->clan);
    ch->clan = str_dup( "" );
    ch->exp -= 1000000;
    send_to_char("You are now an Inconnu.\n\r",ch);
    sprintf(buf,"%s is now an Inconnu!",ch->name);
    do_info(ch,buf);
    SET_BIT(ch->special, SPC_INCONNU);
    return;
}

void do_stake( CHAR_DATA *ch, char *argument )
{
    CHAR_DATA *victim;
    OBJ_DATA  *stake;
    char      arg [MAX_INPUT_LENGTH];
    int       blood;

    argument = one_argument( argument, arg );
    if (IS_NPC( ch )) return;

    if ( arg[0] == '\0' )
    {
	send_to_char( "Stake whom?\n\r", ch );
	return;
    }

    if ( ( victim = get_char_room( ch, arg ) ) == NULL )
    {
	send_to_char( "They aren't here.\n\r", ch );
	return;
    }

    stake = get_eq_char( ch, WEAR_HOLD );
    if ( stake == NULL || stake->item_type != ITEM_STAKE )
    {
	stake = get_eq_char( ch, WEAR_WIELD );
	if ( stake == NULL || stake->item_type != ITEM_STAKE )
	{
	    send_to_char( "How can you stake someone down without holding a stake?\n\r", ch );
	    return;
	}
    }

    if ( IS_NPC(victim) )
    {
	send_to_char( "Not on NPC's.\n\r", ch );
	return;
    }

    if ( ch == victim )
    {
	send_to_char( "You cannot stake yourself.\n\r", ch );
	return;
    }

    if (IS_SET (ch->in_room->room_flags, ROOM_NO_DECAP))
    {
	send_to_char( "You cannot stake someone in a no decap room!\n\r",ch );
	return;
    }

    if (!IS_CLASS(victim, CLASS_VAMPIRE))
    {
	send_to_char( "You can only stake vampires.\n\r", ch );
	return;
    }

    if (victim->position > POS_MORTAL)
    {
	send_to_char( "You can only stake down a vampire who is mortally wounded.\n\r", ch );
	return;
    }

    act("You plunge $p into $N's heart.", ch, stake, victim, TO_CHAR);
    act("$n plunges $p into $N's heart.", ch, stake, victim, TO_NOTVICT);
    send_to_char( "You feel a stake plunged through your heart.\n\r", victim );
    if (IS_IMMUNE(victim,IMM_STAKE)) return;

    /* Have to make sure they have enough blood to change back */
    blood = victim->pcdata->condition[COND_THIRST];
    victim->pcdata->condition[COND_THIRST] = 666;

    /* To take care of vampires who have powers in affect. */
    if (IS_VAMPAFF(victim,VAM_DISGUISED) ) do_mask(victim,"self");
    if (IS_IMMUNE(victim,IMM_SHIELDED) ) do_shield(victim,"");
    if (IS_AFFECTED(victim,AFF_SHADOWPLANE) ) do_shadowplane(victim,"");
    if (IS_VAMPAFF(victim,VAM_FANGS) ) do_fangs(victim,"");
    if (IS_VAMPAFF(victim,VAM_CLAWS) ) do_claws(victim,"");
    if (IS_VAMPAFF(victim,VAM_NIGHTSIGHT) ) do_nightsight(victim,"");
    if (IS_AFFECTED(victim,AFF_SHADOWSIGHT) ) do_shadowsight(victim,"");
    if (IS_SET(victim->act,PLR_HOLYLIGHT) ) do_truesight(victim,"");
    if (IS_VAMPAFF(victim,VAM_CHANGED) ) do_change(victim,"human");
    if (IS_POLYAFF(victim,POLY_SERPENT) ) do_serpent(victim,"");
    victim->pcdata->stats[UNI_RAGE] = 0;
    victim->pcdata->condition[COND_THIRST] = blood;

    REMOVE_BIT(victim->special, SPC_ANARCH);
    REMOVE_BIT(victim->class, CLASS_VAMPIRE);
    obj_from_char(stake);
    obj_to_char(stake,victim);
    ch->exp = ch->exp + 1000;
    victim->home = 3001;
    return;
}

void do_mask( CHAR_DATA *ch, char *argument )
{
    CHAR_DATA *victim;
    char      arg [MAX_INPUT_LENGTH];
    char      buf [MAX_INPUT_LENGTH];

    argument = one_argument( argument, arg );

    if (IS_NPC(ch)) return;

    if (!IS_CLASS(ch, CLASS_VAMPIRE))
    {
	send_to_char( "Huh?\n\r", ch );
	return;
    }

    if (!IS_VAMPAFF(ch,VAM_OBFUSCATE) )
    {
	send_to_char("You are not trained in the Obfuscate discipline.\n\r",ch);
	return;
    }

    if ( arg[0] == '\0' )
    {
	send_to_char( "Change to look like whom?\n\r", ch );
	return;
    }

    if (IS_AFFECTED(ch,AFF_POLYMORPH) && !IS_VAMPAFF(ch,VAM_DISGUISED))
    {
	send_to_char( "Not while polymorphed.\n\r", ch );
	return;
    }

    if ( ( victim = get_char_room( ch, arg ) ) == NULL )
    {
	send_to_char( "They aren't here.\n\r", ch );
	return;
    }

    if ( IS_NPC(victim) )
    {
	send_to_char( "Not on NPC's.\n\r", ch );
	return;
    }

    if ( IS_IMMORTAL(victim) && victim != ch )
    {
	send_to_char( "You can only mask avatars or lower.\n\r", ch );
	return;
    }

    if ( ch->pcdata->condition[COND_THIRST] < 40 )
    {
	send_to_char("You have insufficient blood.\n\r",ch);
	return;
    }
    ch->pcdata->condition[COND_THIRST] -= number_range(30,40);

    if ( ch == victim )
    {
	if (!IS_AFFECTED(ch,AFF_POLYMORPH) && !IS_VAMPAFF(ch,VAM_DISGUISED))
	{
	    send_to_char( "You already look like yourself!\n\r", ch );
	    return;
	}
	sprintf(buf,"Your form shimmers and transforms into %s.",ch->name);
	act(buf,ch,NULL,victim,TO_CHAR);
	sprintf(buf,"%s's form shimmers and transforms into %s.",ch->morph,ch->name);
	act(buf,ch,NULL,victim,TO_ROOM);
	REMOVE_BIT(ch->affected_by, AFF_POLYMORPH);
	REMOVE_BIT(ch->pcdata->stats[UNI_AFF], VAM_DISGUISED);
    	free_string( ch->morph );
    	ch->morph = str_dup( "" );
	return;
    }
    if (IS_VAMPAFF(ch,VAM_DISGUISED))
    {
    	sprintf(buf,"Your form shimmers and transforms into a clone of %s.",victim->name);
    	act(buf,ch,NULL,victim,TO_CHAR);
    	sprintf(buf,"%s's form shimmers and transforms into a clone of %s.",ch->morph,victim->name);
    	act(buf,ch,NULL,victim,TO_NOTVICT);
    	sprintf(buf,"%s's form shimmers and transforms into a clone of you!",ch->morph);
    	act(buf,ch,NULL,victim,TO_VICT);
    	free_string( ch->morph );
    	ch->morph = str_dup( victim->name );
	return;
    }
    sprintf(buf,"Your form shimmers and transforms into a clone of %s.",victim->name);
    act(buf,ch,NULL,victim,TO_CHAR);
    sprintf(buf,"%s's form shimmers and transforms into a clone of %s.",ch->name,victim->name);
    act(buf,ch,NULL,victim,TO_NOTVICT);
    sprintf(buf,"%s's form shimmers and transforms into a clone of you!",ch->name);
    act(buf,ch,NULL,victim,TO_VICT);
    SET_BIT(ch->affected_by, AFF_POLYMORPH);
    SET_BIT(ch->pcdata->stats[UNI_AFF], VAM_DISGUISED);
    free_string( ch->morph );
    ch->morph = str_dup( victim->name );
    return;
}

void do_change( CHAR_DATA *ch, char *argument )
{
    char arg [MAX_INPUT_LENGTH]; 
    char buf [MAX_STRING_LENGTH]; 
    argument = one_argument( argument, arg ); 

    if (IS_NPC(ch)) return;

    if (!IS_CLASS(ch, CLASS_VAMPIRE))
    {
	send_to_char("Huh?\n\r",ch);
	return;
    }

    if (!IS_VAMPAFF(ch,VAM_PROTEAN) )
    {
	send_to_char("You are not trained in the Protean discipline.\n\r",ch);
	return;
    }

    if (IS_AFFECTED(ch, AFF_PARADOX))
    {
	send_to_char("Your soul is stuck in paradox!\r\n", ch);
	return;
    }

    if ( arg[0] == '\0' )
    {
	send_to_char( "You can change between 'human', 'bat', 'wolf' and 'mist' forms.\n\r", ch );
	return;
    }

    if ( !str_cmp(arg,"bat") )
    {
	if (IS_AFFECTED(ch, AFF_POLYMORPH))
	{
	    send_to_char( "You can only polymorph from human form.\n\r", ch );
	    return;
	}
    	if ( ch->pcdata->condition[COND_THIRST] < 50 )
    	{
	    send_to_char("You have insufficient blood.\n\r",ch);
	    return;
    	}
	if (ch->stance[0] != -1) do_stance(ch,"");
	if (ch->mounted == IS_RIDING) do_dismount(ch,"");
    	ch->pcdata->condition[COND_THIRST] -= number_range(40,50);
	clear_stats(ch);
	act( "You transform into bat form.", ch, NULL, NULL, TO_CHAR );
	act( "$n transforms into a bat.", ch, NULL, NULL, TO_ROOM );
	SET_BIT(ch->pcdata->stats[UNI_AFF], VAM_FLYING);
	SET_BIT(ch->pcdata->stats[UNI_AFF], VAM_SONIC);
	SET_BIT(ch->pcdata->powers[WOLF_POLYAFF], POLY_BAT);
	SET_BIT(ch->pcdata->stats[UNI_AFF], VAM_CHANGED);
	SET_BIT(ch->affected_by, AFF_POLYMORPH);
	sprintf(buf, "%s the vampire bat", ch->name);
    	free_string( ch->morph );
    	ch->morph = str_dup( buf );
	return;
    }
    else if ( !str_cmp(arg,"wolf") )
    {
	if (IS_AFFECTED(ch, AFF_POLYMORPH))
	{
	    send_to_char( "You can only polymorph from human form.\n\r", ch );
	    return;
	}
    	if ( ch->pcdata->condition[COND_THIRST] < 50 )
    	{
	    send_to_char("You have insufficient blood.\n\r",ch);
	    return;
    	}
	if (ch->stance[0] != -1) do_stance(ch,"");
	if (ch->mounted == IS_RIDING) do_dismount(ch,"");
    	ch->pcdata->condition[COND_THIRST] -= number_range(40,50);
	act( "You transform into wolf form.", ch, NULL, NULL, TO_CHAR );
	act( "$n transforms into a dire wolf.", ch, NULL, NULL, TO_ROOM );
	clear_stats(ch);
    	if (ch->wpn[0] > 0)
    	{
	    ch->hitroll += (ch->wpn[0]);
	    ch->damroll += (ch->wpn[0]);
	    ch->armor   -= (ch->wpn[0] * 3);
    	}
    	ch->pcdata->mod_str = 10;
	SET_BIT(ch->pcdata->powers[WOLF_POLYAFF], POLY_WOLF);
	SET_BIT(ch->affected_by, AFF_POLYMORPH);
	SET_BIT(ch->pcdata->stats[UNI_AFF], VAM_CHANGED);
	sprintf(buf, "%s the dire wolf", ch->name);
    	free_string( ch->morph );
    	ch->morph = str_dup( buf );
	return;
    }
    else if ( !str_cmp(arg,"mist") )
    {
	if (IS_AFFECTED(ch, AFF_POLYMORPH))
	{
	    send_to_char( "You can only polymorph from human form.\n\r", ch );
	    return;
	}
    	if ( ch->pcdata->condition[COND_THIRST] < 50 )
    	{
	    send_to_char("You have insufficient blood.\n\r",ch);
	    return;
    	}
	if (ch->stance[0] != -1) do_stance(ch,"");
	if (ch->mounted == IS_RIDING) do_dismount(ch,"");
    	ch->pcdata->condition[COND_THIRST] -= number_range(40,50);
	act( "You transform into mist form.", ch, NULL, NULL, TO_CHAR );
	act( "$n transforms into a white mist.", ch, NULL, NULL, TO_ROOM );

	if (IS_AFFECTED(ch, AFF_TIED))
	{
	    act("The ropes binding you fall through your ethereal form.",ch,NULL,NULL,TO_CHAR);
	    act("The ropes binding $n fall through $s ethereal form.",ch,NULL,NULL,TO_ROOM);
	    affect_strip(ch, gsn_tied);
	    REMOVE_BIT(ch->extra, GAGGED);
	    REMOVE_BIT(ch->extra, BLINDFOLDED);
	}
	if (is_affected(ch, gsn_web))
	{
	    act("The webbing entrapping $n falls through $s ethereal form.",ch,NULL,NULL,TO_ROOM);
	    send_to_char("The webbing entrapping you falls through your ethereal form.\n\r",ch);
	    affect_strip(ch, gsn_web);
	}
	clear_stats(ch);
	SET_BIT(ch->pcdata->powers[WOLF_POLYAFF], POLY_MIST);
	SET_BIT(ch->pcdata->stats[UNI_AFF], VAM_CHANGED);
	SET_BIT(ch->affected_by, AFF_POLYMORPH);
	SET_BIT(ch->affected_by, AFF_ETHEREAL);
	sprintf(buf, "%s the white mist", ch->name);
    	free_string( ch->morph );
    	ch->morph = str_dup( buf );
	return;
    }
    else if ( !str_cmp(arg,"human") )
    {
	if (!IS_AFFECTED(ch, AFF_POLYMORPH))
	{
	    send_to_char( "You are already in human form.\n\r", ch );
	    return;
	}
	if (IS_VAMPAFF(ch, VAM_CHANGED) && IS_POLYAFF(ch, POLY_BAT))
	{
	    REMOVE_BIT(ch->pcdata->stats[UNI_AFF], VAM_FLYING);
	    REMOVE_BIT(ch->pcdata->stats[UNI_AFF], VAM_SONIC);
	    REMOVE_BIT(ch->pcdata->powers[WOLF_POLYAFF], POLY_BAT);
	}
	else if (IS_VAMPAFF(ch, VAM_CHANGED) && IS_POLYAFF(ch, POLY_WOLF))
	{
	    REMOVE_BIT(ch->pcdata->powers[WOLF_POLYAFF], POLY_WOLF);
	}
	else if (IS_VAMPAFF(ch, VAM_CHANGED) && IS_POLYAFF(ch, POLY_MIST))
	{
	    REMOVE_BIT(ch->pcdata->powers[WOLF_POLYAFF], POLY_MIST);
	    REMOVE_BIT(ch->affected_by, AFF_ETHEREAL);
	}
	else
	{
	    /* In case they try to change to human from a non-vamp form */
	    send_to_char( "You struggle to controll your body, mind, and soul.\n\r", ch );
	}
	act( "You transform into human form.", ch, NULL, NULL, TO_CHAR );
	act( "$n transforms into human form.", ch, NULL, NULL, TO_ROOM );
	REMOVE_BIT(ch->affected_by, AFF_POLYMORPH);
	REMOVE_BIT(ch->pcdata->stats[UNI_AFF], VAM_CHANGED);
	REMOVE_BIT(ch->affected_by, AFF_ETHEREAL);
	clear_stats(ch);
  	free_string( ch->morph );
    	ch->morph = str_dup( "" );
	return;
    }
    else
	send_to_char( "You can change between 'human', 'bat', 'wolf' and 'mist' forms.\n\r", ch );
    return;
}

void do_darkheart( CHAR_DATA *ch, char *argument )
{
    char      arg [MAX_INPUT_LENGTH];

    argument = one_argument( argument, arg );

    if (IS_NPC(ch)) return;

    if (!IS_CLASS(ch, CLASS_VAMPIRE) )
    {
	send_to_char("Huh?\n\r",ch);
	return;
    }
    if (!IS_VAMPAFF(ch,VAM_SERPENTIS) )
    {
	send_to_char("You are not trained in the Serpentis discipline.\n\r",ch);
	return;
    }
    if (IS_IMMUNE(ch,IMM_STAKE) )
    {
	send_to_char("But you've already torn your heart out!\n\r",ch);
	return;
    }
    if ( ch->pcdata->condition[COND_THIRST] < 100 )
    {
	send_to_char("You have insufficient blood.\n\r",ch);
	return;
    }
    ch->pcdata->condition[COND_THIRST] -= 100;
    send_to_char("You rip your heart from your body and toss it to the ground.\n\r",ch);
    act("$n rips $s heart out and tosses it to the ground.", ch, NULL, NULL, TO_ROOM);
    make_part( ch, "heart" );
    ch->hit = ch->hit - number_range(10,20);
    update_pos(ch);
    if (ch->position == POS_DEAD && !IS_HERO(ch))
    {
	send_to_char( "You have been KILLED!!\n\r\n\r", ch );
	raw_kill(ch);
	return;
    }
    SET_BIT(ch->immune, IMM_STAKE);
    return;
}

void do_majesty( CHAR_DATA *ch, char *argument )
{
    char arg1[MAX_INPUT_LENGTH];
    char arg2[MAX_INPUT_LENGTH];
    char buf[MAX_STRING_LENGTH];
    int value;

    smash_tilde( argument );
    argument = one_argument( argument, arg1 );
    strcpy( arg2, argument );

    if ( IS_NPC(ch) )
	return;

    if (!IS_CLASS(ch, CLASS_VAMPIRE) )
    {
	send_to_char("Huh?\n\r",ch);
	return;
    }
    if (!IS_VAMPAFF(ch,VAM_PRESENCE) )
    {
	send_to_char("You are not trained in the Presence discipline.\n\r",ch);
	return;
    }

    if (arg1[0] != '\0' && !str_cmp(arg1,"on"))
    {
	if (IS_EXTRA(ch, EXTRA_FAKE_CON))
	{send_to_char("You already have Majesty on.\n\r",ch); return;}
	SET_BIT(ch->extra, EXTRA_FAKE_CON);
	send_to_char("Your Majesty is now ON.\n\r",ch);
	return;
    }
    if (arg1[0] != '\0' && !str_cmp(arg1,"off"))
    {
	if (!IS_EXTRA(ch, EXTRA_FAKE_CON))
	{send_to_char("You already have Majesty off.\n\r",ch); return;}
	REMOVE_BIT(ch->extra, EXTRA_FAKE_CON);
	send_to_char("Your Majesty is now OFF.\n\r",ch);
	return;
    }
    if ( arg1[0] == '\0' || arg2[0] == '\0' )
    {
	send_to_char("You have the following stats:\n\r",ch);
	sprintf(buf,"Hitroll: %d, Actual: %d.\n\r",
		ch->pcdata->fake_hit, char_hitroll(ch));
	send_to_char(buf, ch);
	sprintf(buf,"Damroll: %d, Actual: %d.\n\r",
		ch->pcdata->fake_dam, char_damroll(ch));
	send_to_char(buf, ch);
	sprintf(buf,"Armour: %d, Actual: %d.\n\r",
		ch->pcdata->fake_ac, char_ac(ch));
	send_to_char(buf, ch);
	sprintf(buf,"Hp: %d, Actual: %d.\n\r",
		ch->pcdata->fake_hp, ch->hit);
	send_to_char(buf, ch);
	sprintf(buf,"Mana: %d, Actual: %d.\n\r",
		ch->pcdata->fake_mana, ch->mana);
	send_to_char(buf, ch);
	sprintf(buf,"Move: %d, Actual: %d.\n\r",
		ch->pcdata->fake_move, ch->move);
	send_to_char(buf, ch);
	return;
    }
    value = is_number( arg2 ) ? atoi( arg2 ) : -10000;
    if (!str_cmp(arg1,"hit") || !str_cmp(arg1,"hitroll"))
    {
	if (value < 0 || value > 1000)
	{
	    send_to_char("Please enter a value between 0 and 1000.\n\r",ch);
	    return;
	}
	ch->pcdata->fake_hit = value;
	send_to_char("Ok.\n\r",ch);
	return;
    }
    if (!str_cmp(arg1,"dam") || !str_cmp(arg1,"damroll"))
    {
	if (value < 0 || value > 1000)
	{
	    send_to_char("Please enter a value between 0 and 1000.\n\r",ch);
	    return;
	}
	ch->pcdata->fake_dam = value;
	send_to_char("Ok.\n\r",ch);
	return;
    }
    if (!str_cmp(arg1,"ac") || !str_cmp(arg1,"armour") || !str_cmp(arg1,"armor"))
    {
	if (value < -1000 || value > 100)
	{
	    send_to_char("Please enter a value between -1000 and 100.\n\r",ch);
	    return;
	}
	ch->pcdata->fake_ac = value;
	send_to_char("Ok.\n\r",ch);
	return;
    }
    if (!str_cmp(arg1,"hp") || !str_cmp(arg1,"hitpoints"))
    {
	if (value < 1 || value > 30000)
	{
	    send_to_char("Please enter a value between 1 and 30000.\n\r",ch);
	    return;
	}
	ch->pcdata->fake_hp = value;
	send_to_char("Ok.\n\r",ch);
	return;
    }
    if (!str_cmp(arg1,"mana"))
    {
	if (value < 1 || value > 30000)
	{
	    send_to_char("Please enter a value between 1 and 30000.\n\r",ch);
	    return;
	}
	ch->pcdata->fake_mana = value;
	send_to_char("Ok.\n\r",ch);
	return;
    }
    if (!str_cmp(arg1,"move"))
    {
	if (value < 1 || value > 30000)
	{
	    send_to_char("Please enter a value between 1 and 30000.\n\r",ch);
	    return;
	}
	ch->pcdata->fake_move = value;
	send_to_char("Ok.\n\r",ch);
	return;
    }
    send_to_char("You can set: Hit, Dam, Ac, Hp, Mana, Move.\n\r",ch);
    return;
}

void do_mortal( CHAR_DATA *ch, char *argument )
{
    char      arg [MAX_INPUT_LENGTH];
    int       blood;

    argument = one_argument( argument, arg );

    if (IS_NPC(ch)) return;
    if (!IS_CLASS(ch, CLASS_VAMPIRE) && !IS_VAMPAFF(ch, VAM_MORTAL))
    {
	send_to_char("Huh?\n\r",ch);
	return;
    }
    if (!IS_VAMPAFF(ch,VAM_OBFUSCATE) )
    {
	send_to_char("You are not trained in the Obfuscate discipline.\n\r",ch);
	return;
    }
    if (IS_CLASS(ch,CLASS_VAMPIRE) )
    {
        if ( ch->pcdata->condition[COND_THIRST] < 100 )
        {
	    send_to_char("You must be at full blood to use this power.\n\r",ch);
	    return;
    	}
    	/* Have to make sure they have enough blood to change back */
    	blood = ch->pcdata->condition[COND_THIRST];
    	ch->pcdata->condition[COND_THIRST] = 666;

	/* Remove physical vampire attributes when you take mortal form */
    	if (IS_VAMPAFF(ch,VAM_DISGUISED) ) do_mask(ch,"self");
    	if (IS_IMMUNE(ch,IMM_SHIELDED) ) do_shield(ch,"");
    	if (IS_AFFECTED(ch,AFF_SHADOWPLANE) ) do_shadowplane(ch,"");
    	if (IS_VAMPAFF(ch,VAM_FANGS) ) do_fangs(ch,"");
    	if (IS_VAMPAFF(ch,VAM_CLAWS) ) do_claws(ch,"");
    	if (IS_VAMPAFF(ch,VAM_NIGHTSIGHT) ) do_nightsight(ch,"");
    	if (IS_AFFECTED(ch,AFF_SHADOWSIGHT) ) do_shadowsight(ch,"");
    	if (IS_SET(ch->act,PLR_HOLYLIGHT) ) do_truesight(ch,"");
    	if (IS_VAMPAFF(ch,VAM_CHANGED) ) do_change(ch,"human");
    	if (IS_POLYAFF(ch,POLY_SERPENT) ) do_serpent(ch,"");
	ch->pcdata->stats[UNI_RAGE] = 0;
    	ch->pcdata->condition[COND_THIRST] = blood;

    	send_to_char("Colour returns to your skin and you warm up a little.\n\r",ch);
    	act("Colour returns to $n's skin.", ch, NULL, NULL, TO_ROOM);

    	REMOVE_BIT(ch->class, CLASS_VAMPIRE);
    	SET_BIT(ch->pcdata->stats[UNI_AFF], VAM_MORTAL);
	return;
    }
    send_to_char("You skin pales and cools.\n\r",ch);
    act("$n's skin pales slightly.", ch, NULL, NULL, TO_ROOM);
    ch->class=CLASS_VAMPIRE;
    REMOVE_BIT(ch->pcdata->stats[UNI_AFF], VAM_MORTAL);
    return;
}

void do_mortalvamp( CHAR_DATA *ch, char *argument )
{
    char      arg [MAX_INPUT_LENGTH];
    int       blood;

    argument = one_argument( argument, arg );

    if (IS_NPC(ch)) return;
    if (IS_CLASS(ch,CLASS_VAMPIRE) )
    {
    	/* Have to make sure they have enough blood to change back */
    	blood = ch->pcdata->condition[COND_THIRST];
    	ch->pcdata->condition[COND_THIRST] = 666;

	/* Remove physical vampire attributes when you take mortal form */
    	if (IS_VAMPAFF(ch,VAM_DISGUISED) ) do_mask(ch,"self");
    	if (IS_IMMUNE(ch,IMM_SHIELDED) ) do_shield(ch,"");
    	if (IS_AFFECTED(ch,AFF_SHADOWPLANE) ) do_shadowplane(ch,"");
    	if (IS_VAMPAFF(ch,VAM_FANGS) ) do_fangs(ch,"");
    	if (IS_VAMPAFF(ch,VAM_CLAWS) ) do_claws(ch,"");
    	if (IS_VAMPAFF(ch,VAM_NIGHTSIGHT) ) do_nightsight(ch,"");
    	if (IS_AFFECTED(ch,AFF_SHADOWSIGHT) ) do_shadowsight(ch,"");
    	if (IS_SET(ch->act,PLR_HOLYLIGHT) ) do_truesight(ch,"");
    	if (IS_VAMPAFF(ch,VAM_CHANGED) ) do_change(ch,"human");
    	if (IS_POLYAFF(ch,POLY_SERPENT) ) do_serpent(ch,"");
	ch->pcdata->stats[UNI_RAGE] = 0;
    	ch->pcdata->condition[COND_THIRST] = blood;

    	send_to_char("You loose your vampire powers.\n\r",ch);

    	REMOVE_BIT(ch->class, CLASS_VAMPIRE);
    	SET_BIT(ch->pcdata->stats[UNI_AFF], VAM_MORTAL);
	return;
    }
    send_to_char("You regain your vampire powers.\n\r",ch);
    ch->class= CLASS_VAMPIRE;
    REMOVE_BIT(ch->pcdata->stats[UNI_AFF], VAM_MORTAL);
    return;
}

void do_serpent( CHAR_DATA *ch, char *argument )
{
    char arg [MAX_INPUT_LENGTH]; 
    char buf [MAX_STRING_LENGTH]; 
    argument = one_argument( argument, arg ); 

    if (IS_NPC(ch)) return;

    if (!IS_CLASS(ch, CLASS_VAMPIRE))
    {
	send_to_char("Huh?\n\r",ch);
	return;
    }

    if (!IS_VAMPAFF(ch,VAM_SERPENTIS) )
    {
	send_to_char("You are not trained in the Serpentis discipline.\n\r",ch);
	return;
    }

    if (IS_AFFECTED(ch, AFF_POLYMORPH))
    {
	if (!IS_POLYAFF(ch,POLY_SERPENT))
	{
	    send_to_char( "You cannot polymorph from this form.\n\r", ch );
	    return;
	}
	act( "You transform back into human.", ch, NULL, NULL, TO_CHAR );
	act( "$n transform into human form.", ch, NULL, NULL, TO_ROOM );
	REMOVE_BIT(ch->polyaff, POLY_SERPENT);
	REMOVE_BIT(ch->affected_by, AFF_POLYMORPH);
	clear_stats(ch);
	free_string( ch->morph );
	ch->morph = str_dup( "" );
	ch->max_hit = ch->max_hit - 250;
	ch->hit = ch->hit - 250;
	if (ch->hit < 1) ch->hit = 1;
	ch->max_mana = ch->max_mana + 50;
	return;
    }
    if ( ch->pcdata->condition[COND_THIRST] < 50 )
    {
	send_to_char("You have insufficient blood.\n\r",ch);
	return;
    }
    ch->pcdata->condition[COND_THIRST] -= number_range(40,50);
    clear_stats(ch);
    if (ch->wpn[0] > 0)
    {
    	ch->hitroll += ch->wpn[0] * 0.75;
    	ch->damroll += ch->wpn[0] * 0.75;
    	ch->armor   -= ch->wpn[0] * 3;
    }
    if (ch->stance[0] != -1) do_stance(ch,"");
    if (ch->mounted == IS_RIDING) do_dismount(ch,"");
    ch->pcdata->mod_str = 10;
    act( "You transform into a huge serpent.", ch, NULL, NULL, TO_CHAR );
    act( "$n transforms into a huge serpent.", ch, NULL, NULL, TO_ROOM );
    SET_BIT(ch->polyaff, POLY_SERPENT);
    SET_BIT(ch->affected_by, AFF_POLYMORPH);
    sprintf(buf, "%s the huge serpent", ch->name);
    free_string( ch->morph );
    ch->morph = str_dup( buf );
    ch->max_hit = ch->max_hit + 250;
    ch->hit = ch->hit + 250;
    ch->max_mana = ch->max_mana - 50;
    return;
}

void do_poison( CHAR_DATA *ch, char *argument )
{
    OBJ_DATA  *obj;
    char       arg [MAX_INPUT_LENGTH];

    argument = one_argument( argument, arg );

    if (IS_NPC(ch)) return;

    if (!IS_CLASS(ch, CLASS_VAMPIRE))
    {
	send_to_char("Huh?\n\r",ch);
	return;
    }

    if (!IS_VAMPAFF(ch,VAM_SERPENTIS) )
    {
	send_to_char("You are not trained in the Serpentis discipline.\n\r",ch);
	return;
    }

    if ( ( ( obj = get_eq_char( ch, WEAR_WIELD ) ) == NULL )
    &&   ( ( obj = get_eq_char( ch, WEAR_HOLD ) ) == NULL ) )
    {
	send_to_char( "You must wield the weapon you wish to poison.\n\r", ch );
	return;
    }

    if ( obj->value[0] != 0 )
    {
	send_to_char( "This weapon cannot be poisoned.\n\r", ch );
	return;
    }

    if ( ch->pcdata->condition[COND_THIRST] < 15 )
    {
	send_to_char("You have insufficient blood.\n\r",ch);
	return;
    }
    ch->pcdata->condition[COND_THIRST] -= number_range(5,15);
    act("You run your tongue along $p, poisoning it.",ch,obj,NULL,TO_CHAR);
    act("$n runs $s tongue along $p, poisoning it.",ch,obj,NULL,TO_ROOM);
    obj->value[0] = 53;
    obj->timer = number_range(10,20);

    return;
}

void do_regenerate( CHAR_DATA *ch, char *argument )
{
    char       arg [MAX_INPUT_LENGTH];
    int min = 5;
    int max = 10;

    argument = one_argument( argument, arg );

    if (IS_NPC(ch)) return;

    if (!IS_CLASS(ch, CLASS_VAMPIRE))
    {
	send_to_char("Huh?\n\r",ch);
	return;
    }

    if (ch->pcdata->stats[UNI_RAGE] > 0)
    {
	send_to_char("You cannot control your regenerative powers while the beast is so strong.\n\r",ch);
	return;
    }

    if ( ch->position == POS_FIGHTING )
    {
	send_to_char("You cannot regenerate while fighting.\n\r",ch);
	return;
    }

    if ( ch->hit >= ch->max_hit && ch->mana >= ch->max_mana && ch->move >= ch->max_move )
    {
	send_to_char("But you are already completely regenerated!\n\r",ch);
	return;
    }

    if ( ch->pcdata->condition[COND_THIRST] < 5 )
    {
	send_to_char("You have insufficient blood.\n\r",ch);
	return;
    }

    ch->pcdata->condition[COND_THIRST] -= number_range(2,5);
    if ( ch->hit >= ch->max_hit && ch->mana >= ch->max_mana && ch->move >= ch->max_move )
    {
    	send_to_char("Your body has completely regenerated.\n\r",ch);
    	act("$n's body completely regenerates itself.",ch,NULL,NULL,TO_ROOM);
    }
    else
    	send_to_char("Your body slowly regenerates itself.\n\r",ch);
    if (ch->hit < 1 )
    {
	ch->hit = ch->hit + 1;
	update_pos(ch);
	WAIT_STATE( ch, 24 );
    }
    else
    {
	min = ch->pcdata->condition[COND_THIRST] / 2;
	max = ch->pcdata->condition[COND_THIRST];
	min += ch->pcdata->legend * 10;
	max += ch->pcdata->legend * 20;
    	ch->hit  = UMIN(ch->hit + number_range(min,max), ch->max_hit);
    	ch->mana = UMIN(ch->mana + number_range(min,max), ch->max_mana);
    	ch->move = UMIN(ch->move + number_range(min,max), ch->max_move);
	update_pos(ch);
	WAIT_STATE( ch, 2);
    }
    return;
}


void vamp_rage( CHAR_DATA *ch )
{
    if (IS_NPC(ch)) return;

    send_to_char("You scream with rage as the beast within consumes you!\n\r",ch);
    act("$n screams with rage as $s inner beast consumes $m!.", ch, NULL, NULL, TO_ROOM);
    do_beastlike(ch,"");
    do_rage(ch,"");
    return;
}

void do_humanity( CHAR_DATA *ch, char *argument )
{
    char       arg [MAX_INPUT_LENGTH];

    argument = one_argument( argument, arg );

    if (IS_NPC( ch )) return;

    if (!IS_NPC(ch) && IS_CLASS(ch,CLASS_VAMPIRE) && ch->beast > 0
	&& ch->beast < 100 && number_range(1, 500) <= ch->beast)
    {
	if (ch->beast == 1)
	{
	    send_to_char("You have attained Golconda!\n\r", ch);
	    ch->exp += 1000000;
	    if (!IS_IMMUNE(ch, IMM_SUNLIGHT))
		SET_BIT(ch->immune, IMM_SUNLIGHT);
	}
	else
	    send_to_char("You feel slightly more in control of your beast.\n\r", ch);
	ch->beast -= 1;
    }
    return;
}

void do_beastlike( CHAR_DATA *ch, char *argument )
{
    char       arg [MAX_INPUT_LENGTH];
    int blood;

    argument = one_argument( argument, arg );

    if (IS_NPC( ch )) return;

    if (!IS_NPC(ch) && IS_CLASS(ch,CLASS_VAMPIRE) && ch->beast < 100
	&& ch->beast > 0 && number_range(1, 500) <= ch->beast)
    {
	if (ch->beast == 0 || ch->beast == 100) return;
	if (ch->beast < 99)
	    send_to_char("You feel your beast take more control over your actions.\n\r", ch);
	else
	    send_to_char("Your beast has fully taken over control of your actions!\n\r", ch);
	ch->beast += 1;
    	blood = ch->pcdata->condition[COND_THIRST];
    	ch->pcdata->condition[COND_THIRST] = 666;
    	if ((IS_VAMPAFF(ch,VAM_PROTEAN) || (IS_VAMPAFF(ch,VAM_OBTENEBRATION)))
		&& !IS_VAMPAFF(ch,VAM_NIGHTSIGHT) ) do_nightsight(ch,"");
    	if (!IS_VAMPAFF(ch,VAM_FANGS) ) do_fangs(ch,"");
    	if (IS_VAMPAFF(ch,VAM_PROTEAN) &&
		!IS_VAMPAFF(ch,VAM_CLAWS) ) do_claws(ch,"");
    	ch->pcdata->condition[COND_THIRST] = blood;
    }
    return;
}

void do_feed( CHAR_DATA *ch, char *argument )
{
    CHAR_DATA *victim;
    char      arg [MAX_INPUT_LENGTH];
    char      buf [MAX_STRING_LENGTH];
    char      bufch [MAX_INPUT_LENGTH];
    char      bufvi [MAX_INPUT_LENGTH];
    int       blood;

    argument = one_argument( argument, arg );

    if (IS_NPC(ch)) return;
    if (!IS_CLASS(ch, CLASS_VAMPIRE))
    {
	send_to_char("Huh?\n\r",ch);
	return;
    }
    if ( ( victim = get_char_room( ch, arg ) ) == NULL )
    {
	send_to_char( "They aren't here.\n\r", ch );
	return;
    }
    if (IS_NPC(victim))
    {
	send_to_char("Not on NPC's.\n\r",ch);
	return;
    }
    if (!IS_CLASS(victim, CLASS_VAMPIRE))
    {
	send_to_char("Blood does them no good at all.\n\r",ch);
	return;
    }
    if (!IS_IMMUNE(victim, IMM_VAMPIRE))
    {
	send_to_char("They refuse to drink your blood.\n\r",ch);
	return;
    }
    if ( ch->pcdata->condition[COND_THIRST] < 20 )
    {
	send_to_char("You don't have enough blood.\n\r",ch);
	return;
    }

    blood = number_range(5,10);
    ch->pcdata->condition[COND_THIRST] -= (blood * 2);
    victim->pcdata->condition[COND_THIRST] += blood;

    if (IS_AFFECTED(ch,AFF_POLYMORPH)) sprintf(bufch, "%s", ch->morph);
    else sprintf(bufch, "%s", ch->name);
    if (IS_AFFECTED(victim,AFF_POLYMORPH)) sprintf(bufvi, "%s", victim->morph);
    else sprintf(bufvi, "%s", victim->name);
    sprintf(buf,"You cut open your wrist and feed some blood to %s.",bufvi);
    act(buf,ch,NULL,victim,TO_CHAR);
    sprintf(buf,"%s cuts open $s wrist and feeds some blood to %s.",bufch,bufvi);
    act(buf, ch, NULL, victim, TO_NOTVICT);
    if (victim->position < POS_RESTING)
	send_to_char("You feel some blood poured down your throat.\n\r",victim);
    else
    {
	sprintf(buf,"%s cuts open $s wrist and feeds you some blood.",bufch);
	act(buf, ch, NULL, victim, TO_VICT);
    }
    return;
}

void do_ghoul( CHAR_DATA *ch, char *argument )
{
    CHAR_DATA *victim;
    CHAR_DATA *familiar;
    char arg[MAX_INPUT_LENGTH];
    char buf[MAX_STRING_LENGTH];

    one_argument( argument, arg );

    if ( IS_NPC(ch) )
	return;

    if (!IS_CLASS(ch, CLASS_VAMPIRE) )
    {
	send_to_char("Huh?\n\r",ch);
	return;
    }

    if (!IS_VAMPAFF(ch,VAM_DOMINATE) )
    {
	send_to_char("You are not trained in the Dominate discipline.\n\r",ch);
	return;
    }

    if ( ( familiar = ch->pcdata->familiar ) != NULL)
    {
	sprintf(buf,"You break your hold over %s.\n\r",familiar->short_descr);
	send_to_char( buf, ch );
	familiar->wizard = NULL;
	ch->pcdata->familiar = NULL;
	return;
    }

    if ( arg[0] == '\0' )
    {
	send_to_char( "What do you wish to make your ghoul?\n\r", ch );
	return;
    }

    if ( ( victim = get_char_room( ch, arg ) ) == NULL )
    {
	send_to_char( "They aren't here.\n\r", ch );
	return;
    }

    if ( ch == victim )
    {
	send_to_char( "Become your own ghoul?\n\r", ch );
	return;
    }

    if ( !IS_NPC(victim) )
    {
	send_to_char( "Not on players.\n\r", ch );
	return;
    }

    if (victim->wizard != NULL)
    {
	send_to_char( "You are unable to make them a ghoul.\n\r", ch );
	return;
    }

    if (victim->level > (ch->spl[RED_MAGIC] * 0.25))
    {
	send_to_char( "They are too powerful.\n\r", ch );
	return;
    }

    if ( ch->pcdata->condition[COND_THIRST] < 50 )
    {
	send_to_char("You have insufficient blood.\n\r",ch);
	return;
    }

    ch->pcdata->condition[COND_THIRST] -= 50;
    ch->pcdata->familiar = victim;
    victim->wizard = ch;
    act("You cut open your wrist and feed $N some blood.",ch,NULL,victim,TO_CHAR);
    act("$n cuts open $s wrist and feeds you some blood.",ch,NULL,victim,TO_VICT);
    act("$n cuts open $s wrist and feeds $N some blood.",ch,NULL,victim,TO_NOTVICT);
    return;
}


