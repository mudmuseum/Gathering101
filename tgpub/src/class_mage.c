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
void 	area_scry		args( ( CHAR_DATA *ch, char *argument) );
void 	readmagic		args( ( CHAR_DATA *ch, char *argument) );
void	flame_room		args( ( CHAR_DATA *ch) );

void do_teach( CHAR_DATA *ch, char *argument )
{
    CHAR_DATA *victim;
    char      arg [MAX_INPUT_LENGTH];

    argument = one_argument( argument, arg );

    if (IS_NPC(ch)) return;
    if (ch->level == LEVEL_APPRENTICE)
    {
	send_to_char("You don't know enough to teach another.\n\r",ch);
	return;
    }

    if ( arg[0] == '\0' )
    {
	send_to_char( "Teach whom?\n\r", ch );
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
	send_to_char( "Not on Immortals's.\n\r", ch );
	return;
    }

    if ( ch == victim )
    {
	send_to_char( "You cannot teach yourself.\n\r", ch );
	return;
    }

    if ( victim->level < LEVEL_AVATAR )
    {
	send_to_char( "You can only teach avatars.\n\r", ch );
	return;
    }

    if (IS_CLASS(victim, CLASS_VAMPIRE) || IS_SET(victim->pcdata->stats[UNI_AFF], VAM_MORTAL))
    {
	send_to_char( "You are unable to teach vampires!\n\r", ch );
	return;
    }

    if (IS_CLASS(victim, CLASS_WEREWOLF))
    {
	send_to_char( "You are unable to teach werewolves!\n\r", ch );
	return;
    }

    if (IS_CLASS(victim, CLASS_DEMON))
    {
	send_to_char( "You are unable to teach demons!\n\r", ch );
	return;
    }

    if (IS_CLASS(victim, CLASS_HIGHLANDER))
    {
	send_to_char( "You are unable to teach highlanders.\n\r", ch );
	return;
    }

    if (!IS_IMMUNE(victim,IMM_VAMPIRE))
    {
	send_to_char( "You cannot teach an unwilling person.\n\r", ch );
	return;
    }

    if (victim->level == LEVEL_AVATAR)
    {
	if (ch->exp < 100000)
	{
	    send_to_char("You cannot afford the 100,000 experience required to teach them.\n\r",ch);
	    return;
        }

	if (victim->exp < 100000)
	{
	    send_to_char("They cannot afford the 100,000 exp required to learn from you.\n\r",ch);
	    return;
	}
	ch->exp -= 100000;
	victim->exp -= 100000;

    	act("You teach $N the basics of magic.", ch, NULL, victim, TO_CHAR);
    	act("$n teaches $N the basics of magic.", ch, NULL, victim, TO_NOTVICT);
    	act("$n teaches you the basics of magic.", ch, NULL, victim, TO_VICT);
    	victim->level = LEVEL_APPRENTICE;
    	victim->trust = LEVEL_APPRENTICE;
    	send_to_char( "You are now an apprentice.\n\r", victim );
   	free_string(victim->lord);
    	victim->lord = str_dup(ch->name);
    	victim->pcdata->powers[MPOWER_RUNE0] = ch->pcdata->powers[MPOWER_RUNE0];
    	victim->class    = CLASS_MAGE;
    	save_char_obj(ch);
    	save_char_obj(victim);
   	return;
    }
    else if (victim->level == LEVEL_APPRENTICE)
    {
	if (ch->pcdata->powers[MPOWER_RUNE0] != victim->pcdata->powers[MPOWER_RUNE0])
	{
	    send_to_char("They are not an apprentice of your school of magic.\r\n", ch);
	    return;
	}
	if (ch->exp < 1000000)
	{
	    send_to_char("You need 1,000,000 experience to teach an apprentice.\r\n", ch);
	    return;
	}
	if (victim->exp < 10000000)
	{
	    send_to_char("Your apprentice needs 10,000,000 experience to become a full mage.\r\n", ch);
	    return;
	}
	if (   (victim->pcdata->powers[MPOWER_LRUNES] < 2047)
	    || (victim->pcdata->powers[MPOWER_LGLYPHS] < 1023)
	    || (victim->pcdata->powers[MPOWER_LSIGILS] < 15) )
	{
	    send_to_char("Your apprentice has not yet learned all the runes, glyphs and sigils.\r\n", ch);
	    return;
	}
	if (   victim->spl[0] < 240 || victim->spl[1] < 240 
	    || victim->spl[2] < 240 || victim->spl[3] < 240 
	    || victim->spl[4] < 240)
	{
	    send_to_char("Your apprentice has not yet mastered their magickal colors.\r\n", ch);
	    return;
	}
	ch->exp -= 1000000;
	victim->exp -= 10000000;

    	act("You teach $N the advanced concepts of magic.", ch, NULL, victim, TO_CHAR);
    	act("$n teaches $N the advanced concepts of magic.", ch, NULL, victim, TO_NOTVICT);
    	act("$n teaches you the advanced concepts of magic.", ch, NULL, victim, TO_VICT);
    	victim->level = LEVEL_MAGE;
    	victim->trust = LEVEL_MAGE;
    	send_to_char( "You are now a full mage.\n\r", victim );
	save_char_obj(ch);
	save_char_obj(victim);
	return;
    }
}

void do_learn( CHAR_DATA *ch, char *argument )
{
    char arg1 [MAX_INPUT_LENGTH];
    char arg2 [MAX_INPUT_LENGTH];
    char buf [MAX_STRING_LENGTH];
    int cost = 0;
    int count = 0;
    
    smash_tilde( argument );
    argument = one_argument( argument, arg1 );
    argument = one_argument( argument, arg2 );
    
    if (arg1[0] == '\0')
    {
	send_to_char("-=-=-=-= Runes Glyphs and Sigils =-=-=-=-\r\n", ch);
	send_to_char("Runes : ", ch);
        if (IS_SET(ch->pcdata->powers[MPOWER_LRUNES], RUNE_MASTER))
	    send_to_char("MASTERY ", ch); else send_to_char("mastery ", ch);
        if (IS_SET(ch->pcdata->powers[MPOWER_LRUNES], RUNE_SPIRIT))
	    send_to_char("SPIRIT ", ch); else send_to_char("spirit ", ch);
        if (IS_SET(ch->pcdata->powers[MPOWER_LRUNES], RUNE_MIND))
	    send_to_char("MIND ", ch); else send_to_char("mind ", ch);
        if (IS_SET(ch->pcdata->powers[MPOWER_LRUNES], RUNE_LIFE))
	    send_to_char("LIFE ", ch); else send_to_char("life ", ch);
        if (IS_SET(ch->pcdata->powers[MPOWER_LRUNES], RUNE_DEATH)) 
	    send_to_char("DEATH ", ch); else send_to_char("death ", ch);
	send_to_char("\r\n        ", ch);
        if (IS_SET(ch->pcdata->powers[MPOWER_LRUNES], RUNE_LIGHT))
	    send_to_char("LIGHT ", ch); else send_to_char("light ", ch);
        if (IS_SET(ch->pcdata->powers[MPOWER_LRUNES], RUNE_DARK))
	    send_to_char("DARK ", ch); else send_to_char("dark ", ch);
        if (IS_SET(ch->pcdata->powers[MPOWER_LRUNES], RUNE_EARTH))
	    send_to_char("EARTH ", ch); else send_to_char("earth ", ch);
        if (IS_SET(ch->pcdata->powers[MPOWER_LRUNES], RUNE_AIR))
	    send_to_char("AIR ", ch); else send_to_char("air ", ch);
        if (IS_SET(ch->pcdata->powers[MPOWER_LRUNES], RUNE_FIRE)) 
	    send_to_char("FIRE ", ch); else send_to_char("fire ", ch);
	if (IS_SET(ch->pcdata->powers[MPOWER_LRUNES], RUNE_WATER))
	    send_to_char("WATER ", ch); else send_to_char("water ", ch);
	send_to_char("\r\nGlyphs: ", ch);
	if (IS_SET(ch->pcdata->powers[MPOWER_LGLYPHS], GLYPH_PROTECTION))
	    send_to_char("PROTECTION ", ch); else send_to_char("protection ", ch);
	if (IS_SET(ch->pcdata->powers[MPOWER_LGLYPHS], GLYPH_REDUCTION))
	    send_to_char("REDUCTION ", ch); else send_to_char("reduction ", ch);
	if (IS_SET(ch->pcdata->powers[MPOWER_LGLYPHS], GLYPH_CONTROL))
	    send_to_char("CONTROL ", ch); else send_to_char("control ", ch);
	if (IS_SET(ch->pcdata->powers[MPOWER_LGLYPHS], GLYPH_DESTRUCTION))
	    send_to_char("DESTRUCTION ", ch); else send_to_char("destruction ", ch);
	send_to_char("\r\n        ", ch);
	if (IS_SET(ch->pcdata->powers[MPOWER_LGLYPHS], GLYPH_TRANSPORTATION))
	    send_to_char("TRANSPORTATION ", ch); else send_to_char("transportation ", ch);
	if (IS_SET(ch->pcdata->powers[MPOWER_LGLYPHS], GLYPH_TRANSFORMATION))
	    send_to_char("TRANSFORMATION ", ch); else send_to_char("transformation ", ch);
	if (IS_SET(ch->pcdata->powers[MPOWER_LGLYPHS], GLYPH_CREATION))
	    send_to_char("CREATION ", ch); else send_to_char("creation ", ch);
	send_to_char("\r\n        ", ch);
	if (IS_SET(ch->pcdata->powers[MPOWER_LGLYPHS], GLYPH_INFORMATION))
	    send_to_char("INFORMATION ", ch); else send_to_char("information ", ch);
	if (IS_SET(ch->pcdata->powers[MPOWER_LGLYPHS], GLYPH_SUMMONING))
	    send_to_char("SUMMONING ", ch); else send_to_char("summoning ", ch);
	if (IS_SET(ch->pcdata->powers[MPOWER_LGLYPHS], GLYPH_ENHANCEMENT))
	    send_to_char("ENHANCEMENT ", ch); else send_to_char("enhancement ", ch);
	send_to_char("\r\nSigils: ", ch);
	if (IS_SET(ch->pcdata->powers[MPOWER_LSIGILS], SIGIL_AREA))
	    send_to_char("AREA ", ch); else send_to_char("area ", ch);
	if (IS_SET(ch->pcdata->powers[MPOWER_LSIGILS], SIGIL_OBJECT))
	    send_to_char("OBJECT ", ch); else send_to_char("object ", ch);
	if (IS_SET(ch->pcdata->powers[MPOWER_LSIGILS], SIGIL_SELF))
	    send_to_char("SELF ", ch); else send_to_char("self ", ch);
	if (IS_SET(ch->pcdata->powers[MPOWER_LSIGILS], SIGIL_TARGETING))
	    send_to_char("TARGETING ", ch); else send_to_char("targeting ", ch);
	send_to_char("\r\n=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=\r\n", ch);
	return;
    }
    if (!strcmp(arg1, "rune"))
    {
	if (IS_SET(ch->pcdata->powers[MPOWER_LRUNES], RUNE_MASTER)) count += 1;
	if (IS_SET(ch->pcdata->powers[MPOWER_LRUNES], RUNE_SPIRIT)) count += 1;
	if (IS_SET(ch->pcdata->powers[MPOWER_LRUNES], RUNE_MIND)) count += 1;
	if (IS_SET(ch->pcdata->powers[MPOWER_LRUNES], RUNE_LIFE)) count += 1;
	if (IS_SET(ch->pcdata->powers[MPOWER_LRUNES], RUNE_DEATH)) count += 1;
	if (IS_SET(ch->pcdata->powers[MPOWER_LRUNES], RUNE_LIGHT)) count += 1;
	if (IS_SET(ch->pcdata->powers[MPOWER_LRUNES], RUNE_DARK)) count += 1;
	if (IS_SET(ch->pcdata->powers[MPOWER_LRUNES], RUNE_EARTH)) count += 1;
	if (IS_SET(ch->pcdata->powers[MPOWER_LRUNES], RUNE_AIR)) count += 1;
	if (IS_SET(ch->pcdata->powers[MPOWER_LRUNES], RUNE_FIRE)) count += 1;
	if (IS_SET(ch->pcdata->powers[MPOWER_LRUNES], RUNE_WATER)) count += 1;
	cost = 10 + count * 10; if (cost > 80) cost = 80;
	if (count == 11) 
	{
	    send_to_char("You have mastered all known runes.\r\n", ch);
	    return;
	}

	if (ch->practice < cost) 
	{
	    sprintf(buf, "You need %d primal to learn any runes.\r\n", cost);
	    send_to_char(buf, ch);
	    return;
	}

	if (!strcmp(arg2, "mastery"))
	{
	    if (IS_SET(ch->pcdata->powers[MPOWER_LRUNES], RUNE_MASTER))
		send_to_char("You already know the rune of mastery.\r\n", ch);
	    else
	    {	
		SET_BIT(ch->pcdata->powers[MPOWER_LRUNES], RUNE_MASTER);
		ch->practice -= cost;
		send_to_char("Ok.\r\n", ch);
	    }
	    return;
	}
	if (!strcmp(arg2, "spirit"))
	{
	    if (IS_SET(ch->pcdata->powers[MPOWER_LRUNES], RUNE_SPIRIT))
		send_to_char("You already know the rune of spirit.\r\n", ch);
	    else
	    {
		SET_BIT(ch->pcdata->powers[MPOWER_LRUNES], RUNE_SPIRIT);
		ch->practice -= cost;
		send_to_char("Ok.\r\n", ch);
	    }
	    return;
	}
	if (!strcmp(arg2, "mind"))
	{
	    if (IS_SET(ch->pcdata->powers[MPOWER_LRUNES], RUNE_MIND))
		send_to_char("You already know the rune of mind.\r\n", ch);
	    else
	    {
		SET_BIT(ch->pcdata->powers[MPOWER_LRUNES], RUNE_MIND);
		ch->practice -= cost;
		send_to_char("Ok.\r\n", ch);
	    }
	    return;
	}
	if (!strcmp(arg2, "life"))
	{
	    if (IS_SET(ch->pcdata->powers[MPOWER_LRUNES], RUNE_LIFE))
		send_to_char("You already know the rune of life.\r\n", ch);
	    else
	    {
		SET_BIT(ch->pcdata->powers[MPOWER_LRUNES], RUNE_LIFE);
		ch->practice -= cost;
		send_to_char("Ok.\r\n", ch);
	    }
	    return;
	}
	if (!strcmp(arg2, "death"))
	{
	    if (IS_SET(ch->pcdata->powers[MPOWER_LRUNES], RUNE_DEATH))
		send_to_char("You already know the rune of death.\r\n",ch);
	    else
	    {
		SET_BIT(ch->pcdata->powers[MPOWER_LRUNES], RUNE_DEATH);
		ch->practice -= cost;
		send_to_char("Ok.\r\n", ch);
	    }
	    return;
	}
	if (!strcmp(arg2, "light"))
	{
	    if (IS_SET(ch->pcdata->powers[MPOWER_LRUNES], RUNE_LIGHT))
		send_to_char("You already know the rune of light.\r\n",ch);
	    else
	    {
		SET_BIT(ch->pcdata->powers[MPOWER_LRUNES], RUNE_LIGHT);
		ch->practice -= cost;
		send_to_char("Ok.\r\n", ch);
	    }
	    return;
	}
	if (!strcmp(arg2, "dark"))
	{
	    if (IS_SET(ch->pcdata->powers[MPOWER_LRUNES], RUNE_DARK))
		send_to_char("You already know the rune of dark.\r\n",ch);
	    else
	    {
		SET_BIT(ch->pcdata->powers[MPOWER_LRUNES], RUNE_DARK);
		ch->practice -= cost;
		send_to_char("Ok.\r\n", ch);
	    }
	    return;
	}
	if (!strcmp(arg2, "earth"))
	{
	    if (IS_SET(ch->pcdata->powers[MPOWER_LRUNES], RUNE_EARTH))
		send_to_char("You already know the rune of earth.\r\n",ch);
	    else
	    {
		SET_BIT(ch->pcdata->powers[MPOWER_LRUNES], RUNE_EARTH);
		ch->practice -= cost;
		send_to_char("Ok.\r\n", ch);
	    }
	    return;
	}
	if (!strcmp(arg2, "air"))
	{
	    if (IS_SET(ch->pcdata->powers[MPOWER_LRUNES], RUNE_AIR))
		send_to_char("You already know the rune of air.\r\n", ch);
	    else
	    {
		SET_BIT(ch->pcdata->powers[MPOWER_LRUNES], RUNE_AIR);
		ch->practice -= cost;
		send_to_char("Ok.\r\n", ch);
	    }
	    return;
	}
	if (!strcmp(arg2, "fire"))
	{
	    if (IS_SET(ch->pcdata->powers[MPOWER_LRUNES], RUNE_FIRE))
		send_to_char("You already know the rune of fire.\r\n",ch);
	    else
	    {
		SET_BIT(ch->pcdata->powers[MPOWER_LRUNES], RUNE_FIRE);
		ch->practice -= cost;
		send_to_char("Ok.\r\n", ch);
	    }
	    return;
	}
	if (!strcmp(arg2, "water"))
	{
	    if (IS_SET(ch->pcdata->powers[MPOWER_LRUNES], RUNE_WATER))
		send_to_char("You already know the rune of water.\r\n", ch);
	    else
	    {
		SET_BIT(ch->pcdata->powers[MPOWER_LRUNES], RUNE_WATER);
		ch->practice -= cost;
		send_to_char("Ok.\r\n", ch);
	    }
	    return;
	}
    }
    else if (!strcmp(arg1, "glyph"))
    {
	if (IS_SET(ch->pcdata->powers[MPOWER_LGLYPHS], GLYPH_PROTECTION)) count += 1;
	if (IS_SET(ch->pcdata->powers[MPOWER_LGLYPHS], GLYPH_REDUCTION)) count += 1;
	if (IS_SET(ch->pcdata->powers[MPOWER_LGLYPHS], GLYPH_CONTROL)) count += 1;
	if (IS_SET(ch->pcdata->powers[MPOWER_LGLYPHS], GLYPH_DESTRUCTION)) count += 1;
	if (IS_SET(ch->pcdata->powers[MPOWER_LGLYPHS], GLYPH_TRANSPORTATION)) count += 1; 
	if (IS_SET(ch->pcdata->powers[MPOWER_LGLYPHS], GLYPH_TRANSFORMATION)) count += 1;
	if (IS_SET(ch->pcdata->powers[MPOWER_LGLYPHS], GLYPH_CREATION)) count += 1;
	if (IS_SET(ch->pcdata->powers[MPOWER_LGLYPHS], GLYPH_INFORMATION)) count += 1;
	if (IS_SET(ch->pcdata->powers[MPOWER_LGLYPHS], GLYPH_SUMMONING)) count += 1;
	if (IS_SET(ch->pcdata->powers[MPOWER_LGLYPHS], GLYPH_ENHANCEMENT)) count += 1;
	cost = count * 10; if (cost > 50) cost = 50;
	if (count == 10) 
	{
	    send_to_char("You have mastered all known glyphs.\r\n", ch);
	    return;
	}
	if (ch->practice < cost) 
	{
	    sprintf(buf, "You need %d primal to learn any glyphs.\r\n", cost);
	    send_to_char(buf, ch);
	    return;
	}

	if (!strcmp(arg2, "protection"))
	{
	    if (IS_SET(ch->pcdata->powers[MPOWER_LGLYPHS],GLYPH_PROTECTION))
		send_to_char("You already know the glyph of protection.\r\n", ch);
	    else
	    {
		SET_BIT(ch->pcdata->powers[MPOWER_LGLYPHS], GLYPH_PROTECTION);
		ch->practice -= cost;
		send_to_char("Ok.\r\n", ch);
	    }
	    return;
	}
	if (!strcmp(arg2, "reduction"))
	{
	    if (IS_SET(ch->pcdata->powers[MPOWER_LGLYPHS], GLYPH_REDUCTION))
		send_to_char("You already know the glyph of reduction.\r\n", ch);
	    else
	    {
		SET_BIT(ch->pcdata->powers[MPOWER_LGLYPHS], GLYPH_REDUCTION);
		ch->practice -= cost;
		send_to_char("Ok.\r\n", ch);
	    }
	    return;
	}
	if (!strcmp(arg2, "control"))
	{
	    if (IS_SET(ch->pcdata->powers[MPOWER_LGLYPHS], GLYPH_CONTROL))
		send_to_char("You already know the glyph of control.\r\n", ch);
	    else
	    {
		SET_BIT(ch->pcdata->powers[MPOWER_LGLYPHS], GLYPH_CONTROL);
		ch->practice -= cost;
		send_to_char("Ok.\r\n", ch);
	    }
	    return;
	}
	if (!strcmp(arg2, "destruction"))
	{
	    if (IS_SET(ch->pcdata->powers[MPOWER_LGLYPHS], GLYPH_DESTRUCTION))
		send_to_char("You already know the glyph of destruction.\r\n", ch);
	    else
	    { 
		SET_BIT(ch->pcdata->powers[MPOWER_LGLYPHS], GLYPH_DESTRUCTION);
		ch->practice -= cost;
		send_to_char("Ok.\r\n", ch);
	    }
	    return;
	}
	if (!strcmp(arg2, "transportation"))
	{
	    if (IS_SET(ch->pcdata->powers[MPOWER_LGLYPHS], GLYPH_TRANSPORTATION))
		send_to_char("You already know the glyph of transportation.\r\n", ch);
	    else
	    {
		SET_BIT(ch->pcdata->powers[MPOWER_LGLYPHS], GLYPH_TRANSPORTATION);
		ch->practice -= cost;
		send_to_char("Ok.\r\n", ch);
	    }
	    return;
	}
	if (!strcmp(arg2, "transformation"))
	{
	    if (IS_SET(ch->pcdata->powers[MPOWER_LGLYPHS], GLYPH_TRANSFORMATION))
		send_to_char("You already know the glyph of transformation.\r\n", ch);
	    else
	    {
		SET_BIT(ch->pcdata->powers[MPOWER_LGLYPHS], GLYPH_TRANSFORMATION);
		ch->practice -= cost;
		send_to_char("Ok.\r\n", ch);
	    }
	    return;
	}
	if (!strcmp(arg2, "creation"))
	{
	    if (IS_SET(ch->pcdata->powers[MPOWER_LGLYPHS], GLYPH_CREATION))
		send_to_char("You already know the glyph of creation.\r\n", ch);
	    else
	    {
		SET_BIT(ch->pcdata->powers[MPOWER_LGLYPHS], GLYPH_CREATION);
		ch->practice -= cost;
		send_to_char("Ok.\r\n", ch);
	    }
	    return;
	}
	if (!strcmp(arg2, "information"))
	{
	    if (IS_SET(ch->pcdata->powers[MPOWER_LGLYPHS], GLYPH_INFORMATION))
		send_to_char("You already know the glyph of information.\r\n", ch);
	    else
	    {
		SET_BIT(ch->pcdata->powers[MPOWER_LGLYPHS], GLYPH_INFORMATION);
		ch->practice -= cost;
		send_to_char("Ok.\r\n", ch);
	    }
	    return;
	}
	if (!strcmp(arg2, "summoning"))
	{
	    if (IS_SET(ch->pcdata->powers[MPOWER_LGLYPHS], GLYPH_SUMMONING))
		send_to_char("You already know the glyph of summoning.\r\n", ch);
	    else
	    {
		SET_BIT(ch->pcdata->powers[MPOWER_LGLYPHS], GLYPH_SUMMONING);
		ch->practice -= cost;
		send_to_char("Ok.\r\n", ch);
	    }
	    return;
	}
	if (!strcmp(arg2, "enhancement"))
	{
	    if (IS_SET(ch->pcdata->powers[MPOWER_LGLYPHS], GLYPH_ENHANCEMENT))
		send_to_char("You already know the glyph of enhancement.\r\n", ch);
	    else
	    {
		SET_BIT(ch->pcdata->powers[MPOWER_LGLYPHS], GLYPH_ENHANCEMENT);
		ch->practice -= cost;
		send_to_char("Ok.\r\n", ch);
	    }
	    return;
	}
    }
    else if (!strcmp(arg1, "sigil"))
    { 
	if (IS_SET(ch->pcdata->powers[MPOWER_LSIGILS], SIGIL_AREA)) count += 1;
	if (IS_SET(ch->pcdata->powers[MPOWER_LSIGILS], SIGIL_OBJECT)) count += 1;
	if (IS_SET(ch->pcdata->powers[MPOWER_LSIGILS], SIGIL_SELF)) count += 1;
	if (IS_SET(ch->pcdata->powers[MPOWER_LSIGILS], SIGIL_TARGETING)) count += 1;
	cost = 30;
	if (count == 4) 
	{
	    send_to_char("You have mastered all known sigils.\r\n", ch);
	    return;
	}
	if (ch->practice < cost) 
	{
	    sprintf(buf, "You need %d primal to learn any sigils.\r\n", cost);
	    send_to_char(buf, ch);
	    return;
	}

	if (!strcmp(arg2, "area"))
	{
	    if (IS_SET(ch->pcdata->powers[MPOWER_LSIGILS], SIGIL_AREA))
		send_to_char("You already know the sigil of area.\r\n", ch);
	    else
	    {
		SET_BIT(ch->pcdata->powers[MPOWER_LSIGILS], SIGIL_AREA);
		ch->practice -= cost;
		send_to_char("Ok.\r\n", ch);
	    }
	    return;
	}
	if (!strcmp(arg2, "object"))
	{
	    if (IS_SET(ch->pcdata->powers[MPOWER_LSIGILS], SIGIL_OBJECT))
		send_to_char("You already know the sigil of object.\r\n", ch);
	    else
	    {
		SET_BIT(ch->pcdata->powers[MPOWER_LSIGILS], SIGIL_OBJECT);
		ch->practice -= cost;
		send_to_char("Ok.\r\n", ch);
	    }
	    return;
	}
	if (!strcmp(arg2, "self"))
	{
	    if (IS_SET(ch->pcdata->powers[MPOWER_LSIGILS], SIGIL_SELF))
		send_to_char("You already know the sigil of self.\r\n", ch);
	    else
	    {
		SET_BIT(ch->pcdata->powers[MPOWER_LSIGILS], SIGIL_SELF);
		ch->practice -= cost;
		send_to_char("Ok.\r\n", ch);
	    }
	    return;
	}
	if (!strcmp(arg2, "targeting"))
	{
	    if (IS_SET(ch->pcdata->powers[MPOWER_LSIGILS], SIGIL_TARGETING))
		send_to_char("You already know the sigil of targeting.\r\n", ch);
	    else
	    {
		SET_BIT(ch->pcdata->powers[MPOWER_LSIGILS], SIGIL_TARGETING);
		ch->practice -= cost;
		send_to_char("Ok.\r\n", ch);
	    }
	    return;
	}
    }
    send_to_char("Learn what?\r\n", ch);
    return;
}

void do_chant( CHAR_DATA *ch, char *argument )
{
    CHAR_DATA *victim = (CHAR_DATA *) 0;
    OBJ_DATA *book;
    OBJ_DATA *page;
    OBJ_DATA *obj = (OBJ_DATA *) 0;
    char arg[MAX_INPUT_LENGTH];
    bool victim_target = FALSE;
    bool object_target = FALSE;
    bool global_target = FALSE; /* Target object/victim may be anywhere */
    int spellno = 1;
    int spellcount = 0;
    int spelltype;
    int sn = 0;
    int level;

    one_argument( argument, arg );

    if ( ( book = get_eq_char(ch, WEAR_WIELD) ) == NULL || 
	book->item_type != ITEM_BOOK)
    {
	if ( ( book = get_eq_char(ch, WEAR_HOLD) ) == NULL || 
	    book->item_type != ITEM_BOOK)
	{
	    send_to_char( "First you must hold a spellbook.\n\r", ch );
	    return;
	}
    }
    if (IS_SET(book->value[1], CONT_CLOSED))
    {
	send_to_char( "First you better open the book.\n\r", ch );
	return;
    }
    if (book->value[2] < 1)
    {
	send_to_char( "There are no spells on the index page!\n\r", ch );
	return;
    }
    if ( ( page = get_page( book, book->value[2] ) ) == NULL )
    {
	send_to_char( "The current page seems to have been torn out!\n\r", ch );
	return;
    }
    spellcount = ((page->value[1] * 10000) + (page->value[2] * 10) + 
	page->value[3]);
		ch->pcdata->powers[MAGE_CHAIN] = 0;
    act("You chant the arcane words from $p.",ch,book,NULL,TO_CHAR);
    act("$n chants some arcane words from $p.",ch,book,NULL,TO_ROOM);
    if (IS_SET(page->quest, QUEST_MASTER_RUNE))
    {
	ch->spectype = 0;
	if (IS_SET(page->spectype, ADV_FAILED) || 
	    !IS_SET(page->spectype, ADV_FINISHED) || page->points < 1)
	    send_to_char( "The spell failed.\n\r", ch );
	else if (IS_SET(page->spectype, ADV_DAMAGE))
	    adv_spell_damage(ch,book,page,argument);
	else if (IS_SET(page->spectype, ADV_AFFECT))
	    adv_spell_affect(ch,book,page,argument);
	else if (IS_SET(page->spectype, ADV_ACTION))
	    adv_spell_action(ch,book,page,argument);
	else send_to_char( "The spell failed.\n\r", ch );
	return;
    }
    switch ( spellcount ) /* Runespells - additions by Puck */
    {
    default:
	send_to_char( "Nothing happens.\n\r", ch );
	return;
    case 10022:         /* FIRE + DESTRUCTION + TARGETING  = Double Fireball */
	sn = skill_lookup( "fireball" );
	victim_target = TRUE;
	spellno = 2;
	break;
    case 10024:         /* FIRE + DESTRUCTION + AREA = Flame Bolts */
	flame_room(ch);
        return;
    case 10328:         /* FIRE + ENHANCEMENT + OBJECT = Resistance */
        sn = skill_lookup( "resistance" );
        object_target = TRUE;
        break;
    case 20022:         /* AIR + DESTRUCTION + TARGETING = Global chill touch */
	sn = skill_lookup ( "chill touch" );
	global_target = TRUE;
	victim_target = TRUE;
	break;
    case 20082:         /* AIR + TRANSFORMATION + TARGETING = Energy drain */
	sn = skill_lookup( "energy drain" );
	victim_target = TRUE;
	break;
    case 20024:         /* AIR + DESTRUCTION + AREA = Double call lightning */
        sn = skill_lookup("call lightning");
        spellno = 2;
        break;
    case 20322:         /* AIR + ENHANCEMENT + AREA = Fly */
        sn = skill_lookup("fly");
        victim_target = TRUE;
        break;
    case 20328:         /* AIR + ENHANCEMENT + OBJECT = Spellproof */
        sn = skill_lookup("spellproof");
        object_target = TRUE;
        break;
    case 40024:         /* EARTH + DESTRUCTION + AREA = Double earthquake */
        sn = skill_lookup("earthquake");
        spellno = 2;
        break;
    case 40081:         /* EARTH + ENHANCEMENT + SELF = Stone skin */
        sn = skill_lookup("stone skin");
        break;
    case 40322:         /* EARTH + ENHANCEMENT + TARGETING = Global giant str */
        sn = skill_lookup("giant strength");
        victim_target = TRUE;
        global_target = TRUE;
        break;
    case 40328:         /* EARTH + ENHANECMENT + OBJECT = Repair */
        sn = skill_lookup("repair");
        break;
    case 80018:         /* WATER + CREATION + OBJECT = Create spring */
        sn = skill_lookup("create spring");
        break;
    case 80024:         /* WATER + DESTRUCTION + AREA = Double gas breath */
        sn = skill_lookup("gas breath");
        spellno = 2;
        break;
    case 80321:         /* WATER + ENHANCEMENT + SELF = Double mana */
	sn = skill_lookup( "mana" );
	victim_target = TRUE;
        spellno = 2;
	break;
    case 81282:         /* WATER + CONTROL + TARGETING = Global jail of water */
        return;
    case 160022:        /* DARK + DESTRUCTION + TARGETING = Double harm */
	sn = skill_lookup( "harm" );
        spellno = 2;
	victim_target = TRUE;
	break;
    case 160162:        /* DARK + TRANSPORTATION + TARGETING = Global curse */
        sn = skill_lookup("curse");
        victim_target = TRUE;
        global_target = TRUE;
        break;
    case 160321:        /* DARK + ENHANCEMENT + SELF = Frenzy */
        sn = skill_lookup("frenzy");
        victim_target = TRUE;
        break;
    case 160322:        /* DARK + ENHANCEMENT + TARGETING = Global darkblessing */
        sn = skill_lookup("darkblessing");
        victim_target = TRUE;
        global_target = TRUE;
        break;
    case 160642:        /* DARK + REDUCTION + TARGETING = Global poison */
        sn = skill_lookup("poison");
        victim_target = TRUE;
        global_target = TRUE;
        break;
    case 161284:        /* DARK + CONTROL + AREA = Sleep */
        sn = skill_lookup("sleep");
        victim_target = TRUE;
        break;
    case 320018:        /* LIGHT + CREATION + OBJECT = Continual light */
        sn = skill_lookup("continual light");
        break;
    case 320024:        /* LIGHT + DESTRUCTION + AREA = Triple colour spray */
        sn = skill_lookup("colour spray");
        victim_target = TRUE;
        spellno = 3;
        break;
    case 320321:        /* LIGHT + ENHANCEMENT + SELF = Sanctuary */
        sn = skill_lookup("sanctuary");
        victim_target = TRUE;
        break;
    case 320322:        /* LIGHT + ENHANCEMENT + TARGETING = Global bless */
        sn = skill_lookup("bless");
        victim_target = TRUE;
        global_target = TRUE;
        break;
    case 322561:        /* LIGHT + PROTECTION + SELF = Protection */
        sn = skill_lookup("protection");
        victim_target = TRUE;
        break;
    case 322562:        /* LIGHT + PROTECTION + TARGETING = Armor */
        sn = skill_lookup("armor");
        victim_target = TRUE;
        break;
    case 322564:        /* LIGHT + PROTECTOIN + AREA = Global shield */
        sn = skill_lookup("shield");
        victim_target = TRUE;
        global_target = TRUE;
        break;
    case 640018:        /* LIFE + CREATION + OBJECT = Triple create food */
        sn = skill_lookup("create food");
        spellno = 3;
        break;
    case 640022:        /* LIFE + DESTRUCTION + TARGETING = Double global dispel evil */
        sn = skill_lookup("dispel evil");
        victim_target = TRUE;
        global_target = TRUE;
        spellno = 2;
        break;
    case 640321:        /* LIFE + ENHANCEMENT + SELF = Triple refresh */
        sn = skill_lookup("refresh");
        victim_target = TRUE;
        spellno = 3;
        break;
    case 640322:        /* LIFE + ENHANCEMENT + TARGETING = Double heal */
	sn = skill_lookup( "heal" );
	victim_target = TRUE;
	spellno = 2;
	break;
    case 640324:        /* LIFE + ENHANCEMENT + AREA = Global heal */
        sn = skill_lookup("heal");
        victim_target = TRUE;
        global_target = TRUE;
        break;
    case 640328:        /* LIFE + ENHANCEMENT + OBJECT = Enhance armor */
        sn = skill_lookup("enhance armor");
        object_target = TRUE;
        break;
    case 640642:        /* LIFE + REDUCTION + TARGETING = Double global drain life */
        sn = skill_lookup("energy drain");
        victim_target = TRUE;
        global_target = TRUE;
        spellno = 2;
        break;
    case 645122:        /* LIFE + INFORMATION + TARGETING = Global know aligment */
        sn = skill_lookup("know alignment");
        victim_target = TRUE;
        global_target = TRUE;
        break;
    case 1280044:       /* DEATH + SUMMONING + AREA = Triple guardian */
	sn = skill_lookup( "guardian" );
	spellno = 3;
	break;
    case 1280328:       /* DEATH + ENHANCEMENT + OBJECT = Preserve */
        sn = skill_lookup("preserve");
        object_target = TRUE;
        break;
    case 1280642:       /* DEATH + REDUCTION + TARGETING = Weaken */
        sn = skill_lookup("weaken");
        victim_target = TRUE;
        break;
    case 2560321:       /* MIND + ENHANCEMENT + SELF = Truesight */
	sn = skill_lookup("detect magic");
	break;
    case 2560642:       /* MIND + REDUCTION + TARGETING = Global dispel magic */
        sn = skill_lookup("dispel magic");
        victim_target = TRUE;
        global_target = TRUE;
        break;
    case 2565122:       /* MIND + INFORMATION + TARGETING = Global readaura */
	readmagic(ch, argument);
	return;
    case 2565128:       /* MIND + INFORMATION + OBJECT = Global identify */
	sn = skill_lookup( "identify" );
	object_target = TRUE;
	global_target = TRUE;
	break;
    case 2565124:	/* MIND + INFORMATION + AREA = Area scry */
	area_scry(ch, argument);
	return;
    case 5120018:       /* SPIRIT + CREATION + OBJECT = Double soulblade */
        sn = skill_lookup("soulblade");
        spellno = 2;
        break;
    case 5120044:       /* SPIRIT + SUMMONING + AREA = Summon */
        sn = skill_lookup("summon");
        victim_target = TRUE;
        break;
    case 5120161:       /* SPIRIT + TRANSPORTATION + SELF = Teleport */
        sn = skill_lookup("teleport");
        break;
    case 5120321:       /* SPIRIT + ENHANCEMENT + SELF = Pass door */
        sn = skill_lookup("pass door");
        break;
    case 5120328:       /* SPIRIT + ENHANCEMENT + OBJECT = Enchant weapon */
        sn = skill_lookup("enchant weapon");
        object_target = TRUE;
        break;
    case 5120642:       /* SPIRIT + REDUCTION + TARGETING = Global faerie fire */
        sn = skill_lookup("faerie fire");
        victim_target = TRUE;
        global_target = TRUE;
        break;
    case 5121284:       /* SPIRIT + CONTROL + AREA = Faerie fog */
        sn = skill_lookup("faerie fog");
        break;
    case 5122561:       /* SPIRIT + PROTECTION + SELF = No-magic aura */
        return;
    }
    if ( arg[0] == '\0' && (victim_target == TRUE || object_target == TRUE))
    {
	send_to_char( "Please specify a target.\n\r", ch );
	return;
    }
    if (victim_target && sn > 0)
    {
	if ( !global_target && ( victim = get_char_room( ch, arg ) ) == NULL )
	{
	    send_to_char( "They are not here.\n\r", ch );
	    return;
	}
	else if ( global_target && ( victim = get_char_world( ch, arg ) ) == NULL )
	{
	    send_to_char( "They are not here.\n\r", ch );
	    return;
	}
	if (IS_ITEMAFF(victim, ITEMA_REFLECT))
	{
	    send_to_char( "You are unable to focus your spell on them.\n\r", ch );
	    return;
	}
/*
        if (!IS_NPC(victim) && IS_DROW( victim ))
        {
            if (ch == victim )
            {
                send_to_char("You lower your magical resistance....\n\r", ch);
            }
            else if (number_percent ( )  <= victim->drow_magic)
            {
               send_to_char("Your spell does not affect them.\n\r", ch);
               return;
            }
        }
*/
	spelltype = (skill_table[sn].target);
	level = ch->spl[spelltype] * 0.25;
	(*skill_table[sn].spell_fun) ( sn, level, ch, victim );
	if (spellno > 1) (*skill_table[sn].spell_fun) ( sn, level, ch, victim );
	if (spellno > 2) (*skill_table[sn].spell_fun) ( sn, level, ch, victim );
        if (!IS_IMMORTAL( ch ))
		WAIT_STATE(ch, skill_table[sn].beats);
    }
    else if (object_target && sn > 0)
    {
	if ( !global_target && ( obj = get_obj_carry( ch, arg ) ) == NULL )
	{
	    send_to_char( "You are not carrying that object.\n\r", ch );
	    return;
	}
	else if ( global_target && ( obj = get_obj_world( ch, arg ) ) == NULL )
	{
	    send_to_char( "You cannot find any object like that.\n\r", ch );
	    return;
	}
	spelltype = (skill_table[sn].target);
	level = ch->spl[spelltype] * 0.25;
	(*skill_table[sn].spell_fun) ( sn, level, ch, obj );
	if (spellno > 1) (*skill_table[sn].spell_fun) ( sn, level, ch, obj );
	if (spellno > 2) (*skill_table[sn].spell_fun) ( sn, level, ch, obj );
	if (!IS_IMMORTAL( ch))	
		WAIT_STATE(ch, skill_table[sn].beats);
    }
    else if (sn > 0)
    {
	spelltype = (skill_table[sn].target);
	if (spelltype == TAR_OBJ_INV)
	{
	    send_to_char( "Nothing happens.\n\r", ch );
	    return;
	}
	level = ch->spl[spelltype] * 0.25;
	(*skill_table[sn].spell_fun) ( sn, level, ch, ch );
	if (spellno > 1) (*skill_table[sn].spell_fun) ( sn, level, ch, ch );
	if (spellno > 2) (*skill_table[sn].spell_fun) ( sn, level, ch, ch );
	if (!IS_IMMORTAL( ch ))
		WAIT_STATE(ch, skill_table[sn].beats);
    }
    else send_to_char( "Nothing happens.\n\r", ch );
    return;
}

OBJ_DATA *get_page( OBJ_DATA *book, int page_num )
{
    OBJ_DATA *page;
    OBJ_DATA *page_next;

    if (page_num < 1) return NULL;
    for ( page = book->contains; page != NULL; page = page_next )
    {
	page_next = page->next_content;
	if (page->value[0] == page_num) return page;
    }
    return NULL;
}

void adv_spell_damage( CHAR_DATA *ch, OBJ_DATA *book, OBJ_DATA *page, char *argument)
{
    char arg [MAX_STRING_LENGTH];
    char buf [MAX_STRING_LENGTH];
    char msg [50];
    OBJ_DATA *page_next;
    ROOM_INDEX_DATA *old_room;
    char next_par [MAX_INPUT_LENGTH];
    int mana_cost = page->points;
    int min = page->value[1];
    int max = page->value[2];
    int dam;
    int level;
    bool area_affect = FALSE;
    bool victim_target = FALSE;
    bool global_target = FALSE;
    bool next_page = FALSE;
    bool parameter = FALSE;
    bool not_caster = FALSE;
    bool no_players = FALSE;
    bool cast_message = FALSE;
    bool reversed = FALSE;
  
    if (ch->mana < mana_cost)
    {send_to_char("You have insufficient mana to chant this spell.\n\r",ch);return;}
    if (number_range(1,10) == 1)
    {
	send_to_char("You lose concentration, and the spell fizzles!\n\r",ch);
    	act( "$n loses concentration, and the spell fizzles!", ch, NULL, NULL, TO_ROOM );
	min *= 0.25;
	max *= 0.25;
	mana_cost *= .050;
    }
    if (ch->pcdata->powers[MAGE_CHAIN] >= 5)
    {send_to_char("The spell failed.\n\r",ch);return;}
    if (min < 1 || max < 1)
    {send_to_char("The spell failed.\n\r",ch);return;}
    if (min > 350)
    {send_to_char("The max min (hehe) is now 350.\n\r",ch);return;}
	if (min > max) return;

    if (IS_SET(page->spectype, ADV_NEXT_PAGE) &&
	IS_SET(page->spectype, ADV_SPELL_FIRST))
    {
    	if (strlen(page->chpoweroff) < 2 || !str_cmp(page->chpoweroff,"(null)"))
    	{send_to_char("The spell failed.\n\r",ch);return;}
	if (IS_SET(page->spectype, ADV_PARAMETER))
	{
	    if (!str_cmp(page->chpoweron,"(null)"))
		{send_to_char("The spell failed.\n\r",ch);return;}
	    else strcpy(next_par,page->chpoweron);
	}
	else strcpy(next_par,arg);
	if ( page->specpower < page->value[0] )
	{send_to_char( "The spell failed.\n\r", ch );return;}
	if ( ( page_next = get_page( book, page->specpower ) ) == NULL )
	{send_to_char( "The spell failed.\n\r", ch );return;}
    	if (IS_SET(page_next->quest, QUEST_MASTER_RUNE))
    	{
	    if      (IS_SET(page_next->spectype, ADV_DAMAGE))
	    	adv_spell_damage(ch,book,page_next,next_par);
	    else if (IS_SET(page_next->spectype, ADV_AFFECT))
	    	adv_spell_affect(ch,book,page_next,next_par);
	    else if (IS_SET(page_next->spectype, ADV_ACTION))
	    	adv_spell_action(ch,book,page_next,next_par);
	    else {send_to_char( "The spell failed.\n\r", ch );return;}
    	}
	else {send_to_char( "The spell failed.\n\r", ch );return;}
    }

    one_argument( argument, arg );

    if (strlen(page->victpoweron) > 0 || str_cmp(page->victpoweron,"(null)"))
    {
	if (strlen(page->victpoweroff) > 0 || str_cmp(page->victpoweroff,"(null)"))
	    cast_message = TRUE;
    }

    if (IS_SET(page->spectype, ADV_AREA_AFFECT  )) area_affect   = TRUE;
    if (IS_SET(page->spectype, ADV_VICTIM_TARGET)) victim_target = TRUE;
    if (IS_SET(page->spectype, ADV_GLOBAL_TARGET)) global_target = TRUE;
    if (IS_SET(page->spectype, ADV_NEXT_PAGE    )) next_page     = TRUE;
    if (IS_SET(page->spectype, ADV_PARAMETER    )) parameter     = TRUE;
    if (IS_SET(page->spectype, ADV_NOT_CASTER   )) not_caster    = TRUE;
    if (IS_SET(page->spectype, ADV_NO_PLAYERS   )) no_players    = TRUE;
    if (IS_SET(page->spectype, ADV_REVERSED     )) reversed      = TRUE;

/*	if (!reversed && ch->pcdata->powers[MPOWER_RUNE0] == RED_MAGIC)
		dam *= 1.5;
	if (reversed && ch->pcdata->powers[MPOWER_RUNE0] == BLUE_MAGIC)
		dam *= 1.5; */
	if (next_page)        ch->pcdata->powers[MAGE_CHAIN] += 1;
if (victim_target)
    {
	CHAR_DATA *victim = (CHAR_DATA *) 0;

	if ( !global_target && ( victim = get_char_room( ch, arg ) ) == NULL )
	{
	    send_to_char( "They are not here.\n\r", ch );
	    return;
	}
	else if ( global_target && ( victim = get_char_world( ch, arg ) ) == NULL )
	{
	    send_to_char( "They are not here.\n\r", ch );
	    return;
	}
       if (IS_SET(victim->in_room->room_flags, ROOM_NO_TELEPORT) &&
ch->in_room->vnum != victim->in_room->vnum)
        {
            send_to_char("You failed.\n\r",ch);
	return;
        }
	if (reversed)
	    level = ch->spl[BLUE_MAGIC];
	else
	    level = ch->spl[RED_MAGIC];
	if (ch->in_room == victim->in_room)
	    dam = number_range(min,max) + level;
	else
	    dam = number_range(min,max);
	if (ch->spectype < 1000)
	{
	    ch->spectype += dam;

if (ch->level == LEVEL_ARCHMAGE)
	dam = dam * 1.1;
if (ch->level == LEVEL_MAGE)
	dam = dam * 1.05;
	    if (!IS_NPC(victim) && IS_CLASS(victim, CLASS_VAMPIRE) && 
		IS_VAMPAFF(victim, VAM_FORTITUDE) && dam > 1 && !reversed)
		dam = number_range(1,dam);
	    if (IS_ITEMAFF(victim, ITEMA_RESISTANCE) && dam > 1 && !reversed)
		dam = number_range(1,dam);
	    else if (!IS_NPC(victim) && IS_CLASS(victim, CLASS_DEMON) && !reversed &&
		IS_SET(victim->pcdata->powers[DPOWER_FLAGS], DEM_TOUGH) && dam > 1)
		dam = number_range(1,dam);
	    else if (!IS_NPC(victim) && IS_CLASS(victim, CLASS_WEREWOLF) && 
		victim->pcdata->powers[WPOWER_BOAR] > 2 && dam > 1 && !reversed)
		dam *= 0.5;
	    if (!IS_NPC(victim) && IS_CLASS(victim,CLASS_DEMON) && victim->in_room != NULL 
		&& victim->in_room->vnum == ROOM_VNUM_HELL && !reversed)
	    { if (dam < 5) dam = 1; else dam *= 0.2; }
	}
	old_room = ch->in_room;
	if (victim->in_room != NULL && victim->in_room != ch->in_room)
	{
	    char_from_room(ch);
	    char_to_room(ch,victim->in_room);
	}
	if (!reversed)
	{
	    if (is_safe(ch,victim))
	    {
		char_from_room(ch);
		char_to_room(ch,old_room);
		return;
	    }
	}
	else if (IS_ITEMAFF(victim, ITEMA_REFLECT))
	{
	    send_to_char( "You are unable to focus your spell on them.\n\r", ch );
	    char_from_room(ch);
	    char_to_room(ch,old_room);
	    return;
	}
	char_from_room(ch);
	char_to_room(ch,old_room);
	if (cast_message)
	{
	    strcpy(buf,page->victpoweron);
	    act2(buf,ch,NULL,victim,TO_CHAR);
	    strcpy(buf,page->victpoweroff);
	    act2(buf,ch,NULL,victim,TO_ROOM);
	}
	if (reversed)
	{
	    victim->hit += dam;
	    if (victim->hit > victim->max_hit) victim->hit = victim->max_hit;
	}
	else
	{
	    if (dam <= 25) strcpy(msg,"like a wimp.");
	    else if (dam <=   50) strcpy(msg, "lightly.");
	    else if (dam <=  100) strcpy(msg, "hard.");
	    else if (dam <=  250) strcpy(msg, "very hard.");
	    else if (dam <=  500) strcpy(msg, "extremely hard!");
	    else if (dam <= 1000) strcpy(msg, "incredibly hard!");
	    else if (dam <= 1500) strcpy(msg, "insanely hard!");
	    else if (dam <= 2750) strcpy(msg, "unbelievebly hard!");
	    else if (dam <= 4000) strcpy(msg, "with ULTIMATE power!");
	    else strcpy(msg, "with UNGODLY vengance!");
	    adv_damage(ch,victim,dam);
	    sprintf(buf,"Your %s strikes $N %s (%d)",page->chpoweroff,msg, dam);
	    act2(buf,ch,NULL,victim,TO_CHAR);
	    sprintf(buf,"$n's %s strikes $N %s",page->chpoweroff, msg);
	    act2(buf,ch,NULL,victim,TO_NOTVICT);
	    sprintf(buf,"$n's %s strikes you %s [%d]",page->chpoweroff, msg, dam);
	    act2(buf,ch,NULL,victim,TO_VICT);
	 /*   if (!IS_NPC(victim) && number_percent() <= victim->pcdata->atm)
		act( "$n's spell is unable to penetrate your antimagic shield.", ch, NULL, victim, TO_VICT );
	    else*/
		hurt_person(ch,victim,dam);
	}
	if (!IS_IMMORTAL( ch ))
	{
		WAIT_STATE(ch, 12);
		ch->mana -= mana_cost;
	}
    }
    else if (area_affect)
    {
	CHAR_DATA *vch = ch;
	CHAR_DATA *vch_next;

	if (reversed)
	    level = ch->spl[BLUE_MAGIC];
	else
	    level = ch->spl[RED_MAGIC];
	dam = number_range(min,max) + (level * 0.5);
	if (ch->spectype < 1000)
	{
	    ch->spectype += dam;
	}
	else dam = 0;
	if (IS_SET(ch->in_room->room_flags, ROOM_SAFE) && !reversed)
	{
	    send_to_char("You cannot fight in a safe room.\n\r",ch);
	    return;
	}
	if (cast_message)
	{
	    strcpy(buf,page->victpoweron);
	    act2(buf,ch,NULL,vch,TO_CHAR);
	    strcpy(buf,page->victpoweroff);
	    act2(buf,ch,NULL,vch,TO_ROOM);
	}

	for ( vch = char_list; vch != NULL; vch = vch_next )
	{
	    vch_next	= vch->next;
	    if ( vch->in_room == NULL ) continue;
	    if ( ch == vch && not_caster ) continue;
	    if ( !IS_NPC(vch) && no_players ) continue;
	    if ( ch->in_room != vch->in_room ) continue;
	    if (!reversed)
	    {
		if (is_safe(ch,vch)) continue;
	    }
	    if (IS_ITEMAFF(vch, ITEMA_REFLECT))
	    {
		send_to_char( "You are unable to focus your spell on them.\n\r", ch );
		continue;
	    }
	    if ( vch->in_room == ch->in_room )
	    {
		if (reversed)
		{
		    vch->hit += dam;
		    if (vch->hit > vch->max_hit) vch->hit = vch->max_hit;
		}
		else
		{
	    if (dam <= 25) strcpy(msg,"like a wimp.");
	    else if (dam <=   50) strcpy(msg, "lightly.");
	    else if (dam <=  100) strcpy(msg, "hard.");
	    else if (dam <=  250) strcpy(msg, "very hard.");
	    else if (dam <=  500) strcpy(msg, "extremely hard!");
	    else if (dam <= 1000) strcpy(msg, "incredibly hard!");
	    else if (dam <= 1500) strcpy(msg, "insanely hard!");
	    else if (dam <= 2750) strcpy(msg, "unbelievebly hard!");
	    else if (dam <= 4000) strcpy(msg, "with ULTIMATE power!");
	    else strcpy(msg, "with UNGODLY vengance!");

		    adv_damage(ch,vch,dam);
		    sprintf(buf,"Your %s strikes $N %s 
(%d)",page->chpoweroff, msg, dam);
		    act2(buf,ch,NULL,vch,TO_CHAR);
		    sprintf(buf,"$n's %s strikes $N
%s",page->chpoweroff, msg);
		    act2(buf,ch,NULL,vch,TO_NOTVICT);
		    sprintf(buf,"$n's %s strikes you
%s [%d]",page->chpoweroff, msg, dam);
		    act2(buf,ch,NULL,vch,TO_VICT);
/*		    if (!IS_NPC(vch) && number_percent() <= vch->pcdata->atm)
			act( "$n's spell is unable to penetrate your antimagic shield.", ch, NULL, vch, TO_VICT );
		    else if (!IS_NPC(vch) && IS_SET(vch->act, PLR_VAMPIRE) && 
			IS_VAMPAFF(vch, VAM_FORTITUDE) && dam > 1)
			hurt_person(ch,vch,number_range(1,dam));*/
		    if (IS_ITEMAFF(vch, ITEMA_RESISTANCE) && dam > 1)
			hurt_person(ch,vch,number_range(1,dam));
/*		    else if (!IS_NPC(vch) && IS_SET(vch->act, PLR_WEREWOLF) && 
			vch->pcdata->disc[TOTEM_BOAR] > 2 && dam > 1)
			hurt_person(ch,vch,(dam*0.5));*/
		    else hurt_person(ch,vch,dam);
		}
		continue;
	    }
	}
	if (!IS_IMMORTAL(ch))
	{
		WAIT_STATE(ch, 12);
		ch->mana -= mana_cost;
	}
    }
    else {send_to_char("The spell failed.\n\r",ch);return;}
    if (IS_SET(page->spectype, ADV_NEXT_PAGE) &&
	!IS_SET(page->spectype, ADV_SPELL_FIRST))
    {
    	if (strlen(page->chpoweroff) < 2 || !str_cmp(page->chpoweroff,"(null)"))
    	{send_to_char("The spell failed.\n\r",ch);return;}
	if (IS_SET(page->spectype, ADV_PARAMETER))
	{
	    if (!str_cmp(page->chpoweron,"(null)"))
		{send_to_char("The spell failed.\n\r",ch);return;}
	    else strcpy(next_par,page->chpoweron);
	}
	else strcpy(next_par,arg);
	if ( page->specpower < page->value[0] )
	{send_to_char( "The spell failed.\n\r", ch );return;}
	if ( ( page_next = get_page( book, page->specpower ) ) == NULL )
	{send_to_char( "The spell failed.\n\r", ch );return;}
    	if (IS_SET(page_next->quest, QUEST_MASTER_RUNE))
    	{
	    if      (IS_SET(page_next->spectype, ADV_DAMAGE))
	    	adv_spell_damage(ch,book,page_next,next_par);
	    else if (IS_SET(page_next->spectype, ADV_AFFECT))
	    	adv_spell_affect(ch,book,page_next,next_par);
	    else if (IS_SET(page_next->spectype, ADV_ACTION))
	    	adv_spell_action(ch,book,page_next,next_par);
	    else {send_to_char( "The spell failed.\n\r", ch );return;}
    	}
	else {send_to_char( "The spell failed.\n\r", ch );return;}
    }
    return;
}

void adv_spell_affect( CHAR_DATA *ch, OBJ_DATA *book, OBJ_DATA *page, char *argument)
{
    char arg [MAX_STRING_LENGTH];
    char c_m [MAX_INPUT_LENGTH];
    char c_1 [MAX_INPUT_LENGTH];
    char c_2 [MAX_INPUT_LENGTH];
    OBJ_DATA *page_next;
    OBJ_DATA *obj = NULL;
    CHAR_DATA *victim = ch;
    char next_par [MAX_INPUT_LENGTH];
    int mana_cost = page->points;
    int apply_bit = page->value[1];
    int bonuses = page->value[2];
    int affect_bit = page->value[3];
    int sn;
    int level = page->level;
    bool any_affects = FALSE;
    bool area_affect = FALSE;
    bool victim_target = FALSE;
    bool object_target = FALSE;
    bool global_target = FALSE;
    bool next_page = FALSE;
    bool parameter = FALSE;
    bool not_caster = FALSE;
    bool no_players = FALSE;
    bool cast_message = FALSE;
    bool message_one = FALSE;
    bool message_two = FALSE;
    bool is_reversed = FALSE;

    if (ch->mana < mana_cost)
    {send_to_char("You have insufficient mana to chant this spell.\n\r",ch);return;}
if (ch->pcdata->powers[MAGE_CHAIN] >= 5)
{send_to_char("The spell failed.\n\r",ch);return;}
    if (IS_SET(page->spectype, ADV_NEXT_PAGE) &&
	IS_SET(page->spectype, ADV_SPELL_FIRST))
    {
    	if (strlen(page->chpoweroff) < 2 || !str_cmp(page->chpoweroff,"(null)"))
    	{send_to_char("The spell failed.\n\r",ch);return;}
	if (IS_SET(page->spectype, ADV_PARAMETER))
	{
	    if (!str_cmp(page->chpoweron,"(null)"))
		{send_to_char("The spell failed.\n\r",ch);return;}
	    else strcpy(next_par,page->chpoweron);
	}
	else strcpy(next_par,argument);
	if ( page->specpower < page->value[0] )
	{send_to_char( "The spell failed.\n\r", ch );return;}
	if ( ( page_next = get_page( book, page->specpower ) ) == NULL )
	{send_to_char( "The spell failed.\n\r", ch );return;}
    	if (IS_SET(page_next->quest, QUEST_MASTER_RUNE))
    	{
	    if      (IS_SET(page_next->spectype, ADV_DAMAGE))
	    	adv_spell_damage(ch,book,page_next,next_par);
	    else if (IS_SET(page_next->spectype, ADV_AFFECT))
	    	adv_spell_affect(ch,book,page_next,next_par);
	    else if (IS_SET(page_next->spectype, ADV_ACTION))
	    	adv_spell_action(ch,book,page_next,next_par);
	    else {send_to_char( "The spell failed.\n\r", ch );return;}
    	}
	else {send_to_char( "The spell failed.\n\r", ch );return;}
    }

    one_argument( argument, arg );

    if (strlen(page->chpoweroff) > 0 && str_cmp(page->chpoweroff,"(null)"))
	{strcpy(c_m,page->chpoweroff);cast_message = TRUE;}
    if (strlen(page->victpoweron) > 0 && str_cmp(page->victpoweron,"(null)"))
	{strcpy(c_1,page->victpoweron);message_one = TRUE;}
    if (strlen(page->victpoweroff) > 0 && str_cmp(page->victpoweroff,"(null)"))
	{strcpy(c_2,page->victpoweroff);message_two = TRUE;}

    if (IS_SET(page->spectype, ADV_AREA_AFFECT  )) area_affect   = TRUE;
    if (IS_SET(page->spectype, ADV_VICTIM_TARGET)) victim_target = TRUE;
    if (IS_SET(page->spectype, ADV_OBJECT_TARGET)) object_target = TRUE;
    if (IS_SET(page->spectype, ADV_GLOBAL_TARGET)) global_target = TRUE;
    if (IS_SET(page->spectype, ADV_NEXT_PAGE    )) next_page     = TRUE;
    if (IS_SET(page->spectype, ADV_PARAMETER    )) parameter     = TRUE;
    if (IS_SET(page->spectype, ADV_NOT_CASTER   )) not_caster    = TRUE;
    if (IS_SET(page->spectype, ADV_NO_PLAYERS   )) no_players    = TRUE;
    if (IS_SET(page->spectype, ADV_REVERSED     )) 
	{is_reversed = TRUE;bonuses = 0 - bonuses;}
        if (next_page)        ch->pcdata->powers[MAGE_CHAIN] += 1;
       if (IS_SET(ch->in_room->room_flags, ROOM_NO_TELEPORT) && IS_SET(page->spectype,
ADV_OBJECT_TARGET) && IS_SET(page->spectype, ADV_REVERSED))
        {
        send_to_char("You cannot use object spells in no teleport rooms.\n\r",ch);
        return;
        }
    if (victim_target && !area_affect && !global_target && !object_target)
    {
	if ( ( victim = get_char_room( ch, arg ) ) == NULL )
    	{send_to_char("The spell failed.\n\r",ch);return;}
	if (victim->in_room == NULL)
    	{send_to_char("The spell failed.\n\r",ch);return;}
    }
    else if (victim_target && area_affect && !global_target && !object_target)
    {
	if ( ( victim = get_char_world( ch, arg ) ) == NULL )
    	{send_to_char("The spell failed.\n\r",ch);return;}
	if (victim->in_room == NULL ||
	    victim->in_room->area != ch->in_room->area)
    	{send_to_char("The spell failed.\n\r",ch);return;}
    }
    else if (victim_target && global_target && !object_target)
    {
	if ( ( victim = get_char_world( ch, arg ) ) == NULL )
    	{send_to_char("The spell failed.\n\r",ch);return;}
	if (victim->in_room == NULL)
    	{send_to_char("The spell failed.\n\r",ch);return;}
    }
    else if (object_target && !area_affect && !global_target && !victim_target)
    {
	if ( ( obj = get_obj_carry( ch, arg ) ) == NULL )
    	{send_to_char("The spell failed.\n\r",ch);return;}
    }
    else if (object_target && area_affect && !global_target && !victim_target)
    {
	if ( ( obj = get_obj_here( ch, arg ) ) == NULL )
    	{send_to_char("The spell failed.\n\r",ch);return;}
    }
    else if (object_target && global_target && !victim_target)
    {
	if ( ( obj = get_obj_world( ch, arg ) ) == NULL )
    	{send_to_char("The spell failed.\n\r",ch);return;}
	if (obj->in_room == NULL)
    	{send_to_char("The spell failed.\n\r",ch);return;}
    }
	if (ch->pcdata->powers[MPOWER_RUNE0] == page->toughness)
	bonuses = bonuses * 1.5;
    if (page->toughness < PURPLE_MAGIC || page->toughness > YELLOW_MAGIC)
    	{send_to_char("The spell failed.\n\r",ch);return;}
    if (page->toughness == PURPLE_MAGIC)
    {
	if ( ( sn = skill_lookup( "purple sorcery" ) ) < 0 )
	{send_to_char("The spell failed.\n\r",ch);return;}
    }
    else if (page->toughness == RED_MAGIC)
    {
	if ( ( sn = skill_lookup( "red sorcery" ) ) < 0 )
	{send_to_char("The spell failed.\n\r",ch);return;}
    }
    else if (page->toughness == BLUE_MAGIC)
    {
	if ( ( sn = skill_lookup( "blue sorcery" ) ) < 0 )
	{send_to_char("The spell failed.\n\r",ch);return;}
    }
    else if (page->toughness == GREEN_MAGIC)
    {
	if ( ( sn = skill_lookup( "green sorcery" ) ) < 0 )
	{send_to_char("The spell failed.\n\r",ch);return;}
    }
    else if (page->toughness == YELLOW_MAGIC)
    {
	if ( ( sn = skill_lookup( "yellow sorcery" ) ) < 0 )
	{send_to_char("The spell failed.\n\r",ch);return;}
    }
    else {send_to_char("The spell failed.\n\r",ch);return;}

    if (!victim_target && victim != ch)
	{send_to_char("The spell failed.\n\r",ch);return;}
    if (not_caster && ch == victim)
	{send_to_char("The spell failed.\n\r",ch);return;}
    else if (no_players && !IS_NPC(victim))
	{send_to_char("The spell failed.\n\r",ch);return;}

    if ( is_affected( victim, sn ) )
    {
	send_to_char("They are already affected by a spell of that colour.\n\r",ch);
	return;
    }

    if ( apply_bit == 0 )
    {
	enhance_stat(sn,level,ch,victim,APPLY_NONE,bonuses,affect_bit);
	affect_bit = 0;
	any_affects = TRUE;
    }
    if ( IS_SET(apply_bit, ADV_STR) )
    {
	enhance_stat(sn,level,ch,victim,APPLY_STR,bonuses * 0.1,affect_bit);
	affect_bit = 0;
	any_affects = TRUE;
    }
    if ( IS_SET(apply_bit, ADV_DEX) )
    {
	enhance_stat(sn,level,ch,victim,APPLY_DEX,bonuses * 0.1,affect_bit);
	affect_bit = 0;
	any_affects = TRUE;
    }
    if ( IS_SET(apply_bit, ADV_INT) )
    {
	enhance_stat(sn,level,ch,victim,APPLY_INT,bonuses * 0.1,affect_bit);
	affect_bit = 0;
	any_affects = TRUE;
    }
    if ( IS_SET(apply_bit, ADV_WIS) )
    {
	enhance_stat(sn,level,ch,victim,APPLY_WIS,bonuses * 0.1,affect_bit);
	affect_bit = 0;
	any_affects = TRUE;
    }
    if ( IS_SET(apply_bit, ADV_CON) )
    {
	enhance_stat(sn,level,ch,victim,APPLY_CON,bonuses * 0.1,affect_bit);
	affect_bit = 0;
	any_affects = TRUE;
    }
    if ( IS_SET(apply_bit, ADV_MANA) )
    {
	enhance_stat(sn,level,ch,victim,APPLY_MANA,bonuses * 5,affect_bit);
	affect_bit = 0;
	any_affects = TRUE;
    }
    if ( IS_SET(apply_bit, ADV_HIT) )
    {
	enhance_stat(sn,level,ch,victim,APPLY_HIT,bonuses * 5,affect_bit);
	affect_bit = 0;
	any_affects = TRUE;
    }
    if ( IS_SET(apply_bit, ADV_MOVE) )
    {
	enhance_stat(sn,level,ch,victim,APPLY_MOVE,bonuses * 5,affect_bit);
	affect_bit = 0;
	any_affects = TRUE;
    }
    if ( IS_SET(apply_bit, ADV_AC) )
    {
	enhance_stat(sn,level,ch,victim,APPLY_AC,0 - (bonuses * 5),affect_bit);
	affect_bit = 0;
	any_affects = TRUE;
    }
    if ( IS_SET(apply_bit, ADV_HITROLL) )
    {
	enhance_stat(sn,level,ch,victim,APPLY_HITROLL,bonuses * 0.5,affect_bit);
	affect_bit = 0;
	any_affects = TRUE;
    }
    if ( IS_SET(apply_bit, ADV_DAMROLL) )
    {
	enhance_stat(sn,level,ch,victim,APPLY_DAMROLL,bonuses * 0.5,affect_bit);
	affect_bit = 0;
	any_affects = TRUE;
    }
    if ( IS_SET(apply_bit, ADV_SAVING_SPELL) )
    {

enhance_stat(sn,level,ch,victim,APPLY_SAVING_SPELL,0 
- bonuses*0.2,affect_bit);
	affect_bit = 0;
	any_affects = TRUE;
    }
    if (!any_affects) {send_to_char("The spell failed.\n\r",ch);return;}

    if (cast_message) act2(c_m,ch,NULL,victim,TO_CHAR);
    if (message_one)  act2(c_1,ch,NULL,victim,TO_VICT);
    if (message_two)  act2(c_2,ch,NULL,victim,TO_NOTVICT);
    if (!IS_IMMORTAL(ch))
    {
    	WAIT_STATE(ch,12);
    	ch->mana -= mana_cost;
    }

    if (IS_SET(page->spectype, ADV_NEXT_PAGE) &&
	!IS_SET(page->spectype, ADV_SPELL_FIRST))
    {
    	if (strlen(page->chpoweroff) < 2 || !str_cmp(page->chpoweroff,"(null)"))
    	{send_to_char("The spell failed.\n\r",ch);return;}
	if (IS_SET(page->spectype, ADV_PARAMETER))
	{
	    if (!str_cmp(page->chpoweron,"(null)"))
		{send_to_char("The spell failed.\n\r",ch);return;}
	    else strcpy(next_par,page->chpoweron);
	}
	else strcpy(next_par,argument);
	if ( page->specpower < page->value[0] )
	{send_to_char( "The spell failed.\n\r", ch );return;}
	if ( ( page_next = get_page( book, page->specpower ) ) == NULL )
	{send_to_char( "The spell failed.\n\r", ch );return;}
    	if (IS_SET(page_next->quest, QUEST_MASTER_RUNE))
    	{
	    if      (IS_SET(page_next->spectype, ADV_DAMAGE))
	    	adv_spell_damage(ch,book,page_next,next_par);
	    else if (IS_SET(page_next->spectype, ADV_AFFECT))
	    	adv_spell_affect(ch,book,page_next,next_par);
	    else if (IS_SET(page_next->spectype, ADV_ACTION))
	    	adv_spell_action(ch,book,page_next,next_par);
	    else {send_to_char( "The spell failed.\n\r", ch );return;}
    	}
	else {send_to_char( "The spell failed.\n\r", ch );return;}
    }
    return;
}

void adv_spell_action( CHAR_DATA *ch, OBJ_DATA *book, OBJ_DATA *page, char *argument)
{
    char arg1 [MAX_STRING_LENGTH];
    char arg2 [MAX_STRING_LENGTH];
    char c_m [MAX_INPUT_LENGTH];
    char c_1 [MAX_INPUT_LENGTH];
    char c_2 [MAX_INPUT_LENGTH];
    char buf [MAX_STRING_LENGTH];
    OBJ_DATA *page_next;
    OBJ_DATA *obj = NULL;
    OBJ_DATA *obj2 = NULL;
    OBJ_DATA *obj_within = NULL;
    CHAR_DATA *victim = NULL;
    CHAR_DATA *victim2 = NULL;
    ROOM_INDEX_DATA *old_room = ch->in_room;
    char next_par [MAX_INPUT_LENGTH];
    int mana_cost = page->points;
    int action_bit = page->value[1];
    int action_type = page->value[2];
    bool area_affect = FALSE;
    bool victim_target = FALSE;
    bool object_target = FALSE;
    bool second_victim = FALSE;
    bool second_object = FALSE;
    bool global_target = FALSE;
    bool next_page = FALSE;
    bool parameter = FALSE;
    bool not_caster = FALSE;
    bool no_players = FALSE;
    bool cast_message = FALSE;
    bool message_one = FALSE;
    bool message_two = FALSE;
    bool is_reversed = FALSE;

/*if (ch->pcdata->powers[MPOWER_RUNE0] == GREEN_MAGIC ||
ch->pcdata->powers[MPOWER_RUNE0] == YELLOW_MAGIC)*/
/*	mana_cost = 0;*/
    if (ch->mana < mana_cost)
    {send_to_char("You have insufficient mana to chant this spell.\n\r",ch);return;}
if (ch->pcdata->powers[MAGE_CHAIN] >= 5)
{send_to_char("The spell failed.\n\r",ch);return;}
    if (IS_SET(page->spectype, ADV_NEXT_PAGE) &&
	IS_SET(page->spectype, ADV_SPELL_FIRST))
    {
    	if (strlen(page->chpoweroff) < 2 || !str_cmp(page->chpoweroff,"(null)"))
    	{send_to_char("The spell failed.\n\r",ch);return;}
	if (IS_SET(page->spectype, ADV_PARAMETER))
	{
	    if (!str_cmp(page->chpoweron,"(null)"))
		{send_to_char("The spell failed.\n\r",ch);return;}
	    else strcpy(next_par,page->chpoweron);
	}
	else strcpy(next_par,argument);
	if ( page->specpower < page->value[0] )
	{send_to_char( "The spell failed.\n\r", ch );return;}
	if ( ( page_next = get_page( book, page->specpower ) ) == NULL )
	{send_to_char( "The spell failed.\n\r", ch );return;}
    	if (IS_SET(page_next->quest, QUEST_MASTER_RUNE))
    	{
	    if      (IS_SET(page_next->spectype, ADV_DAMAGE))
	    	adv_spell_damage(ch,book,page_next,next_par);
	    else if (IS_SET(page_next->spectype, ADV_AFFECT))
	    	adv_spell_affect(ch,book,page_next,next_par);
	    else if (IS_SET(page_next->spectype, ADV_ACTION))
	    	adv_spell_action(ch,book,page_next,next_par);
	    else {send_to_char( "The spell failed.\n\r", ch );return;}
    	}
	else {send_to_char( "The spell failed.\n\r", ch );return;}
    }

    argument = one_argument( argument, arg1 );
    argument = one_argument( argument, arg2 );

    if (strlen(page->chpoweroff) > 0 && str_cmp(page->chpoweroff,"(null)"))
	{strcpy(c_m,page->chpoweroff);cast_message = TRUE;}
    if (strlen(page->victpoweron) > 0 && str_cmp(page->victpoweron,"(null)"))
	{strcpy(c_1,page->victpoweron);message_one = TRUE;}
    if (strlen(page->victpoweroff) > 0 && str_cmp(page->victpoweroff,"(null)"))
	{strcpy(c_2,page->victpoweroff);message_two = TRUE;}

    if (IS_SET(page->spectype, ADV_AREA_AFFECT  )) area_affect   = TRUE;
    if (IS_SET(page->spectype, ADV_VICTIM_TARGET)) victim_target = TRUE;
    if (IS_SET(page->spectype, ADV_OBJECT_TARGET)) object_target = TRUE;
    if (IS_SET(page->spectype, ADV_GLOBAL_TARGET)) global_target = TRUE;
    if (IS_SET(page->spectype, ADV_NEXT_PAGE    )) next_page     = TRUE;
    if (IS_SET(page->spectype, ADV_PARAMETER    )) parameter     = TRUE;
    if (IS_SET(page->spectype, ADV_NOT_CASTER   )) not_caster    = TRUE;
    if (IS_SET(page->spectype, ADV_NO_PLAYERS   )) no_players    = TRUE;
    if (IS_SET(page->spectype, ADV_SECOND_VICTIM)) second_victim = TRUE;
    if (IS_SET(page->spectype, ADV_SECOND_OBJECT)) second_object = TRUE;
    if (IS_SET(page->spectype, ADV_REVERSED     )) is_reversed   = TRUE;

        if (next_page)        ch->pcdata->powers[MAGE_CHAIN] += 1;
    if (victim_target && !area_affect && !global_target && !object_target)
    {
	if ( ( victim = get_char_room( ch, arg1 ) ) == NULL )
    	{send_to_char("The spell failed.\n\r",ch);return;}
	if (victim->in_room == NULL)
    	{send_to_char("The spell failed.\n\r",ch);return;}
	if (!IS_NPC(victim) && !IS_IMMUNE(victim, IMM_SUMMON))
    	{send_to_char("The spell failed.\n\r",ch);return;}
	if (victim == ch)
    	{send_to_char("The spell failed.\n\r",ch);return;}
    }
    else if (victim_target && area_affect && !global_target && !object_target)
    {
	if ( ( victim = get_char_world( ch, arg1 ) ) == NULL )
    	{send_to_char("The spell failed.\n\r",ch);return;}
	if (victim->in_room == NULL ||
	    victim->in_room->area != ch->in_room->area)
    	{send_to_char("The spell failed.\n\r",ch);return;}
	if (!IS_NPC(victim) && !IS_IMMUNE(victim, IMM_SUMMON))
    	{send_to_char("The spell failed.\n\r",ch);return;}
	if (victim == ch)
    	{send_to_char("The spell failed.\n\r",ch);return;}
    }
    else if (victim_target && global_target && !object_target)
    {
	if ( ( victim = get_char_world( ch, arg1 ) ) == NULL )
    	{send_to_char("The spell failed.\n\r",ch);return;}
	if (victim->in_room == NULL)
    	{send_to_char("The spell failed.\n\r",ch);return;}
 	if (!IS_NPC(victim) && !IS_IMMUNE(victim, IMM_SUMMON))
    	{send_to_char("The spell failed.\n\r",ch);return;}
	if (victim == ch)
    	{send_to_char("The spell failed.\n\r",ch);return;}
    }
    else if (object_target && !area_affect && !global_target && !victim_target)
    {
	if ( ( obj = get_obj_carry( ch, arg1 ) ) == NULL )
    	{send_to_char("The spell failed.\n\r",ch);return;}
    }
    else if (object_target && area_affect && !global_target && !victim_target)
    {
	if ( ( obj = get_obj_here( ch, arg1 ) ) == NULL )
    	{send_to_char("The spell failed.\n\r",ch);return;}
    }
    else if (object_target && global_target && !victim_target)
    {
	if ( ( obj = get_obj_world( ch, arg1 ) ) == NULL )
    	{send_to_char("The spell failed.\n\r",ch);return;}
	if (obj->in_room == NULL)
    	{send_to_char("The spell failed.\n\r",ch);return;}
    }

    if ( arg2[0] == '\0' && (second_victim || second_object) )
    {
	send_to_char("Please specify a target.\n\r",ch);
	return;
    }
    else if (second_victim && victim_target && !area_affect && !global_target && !object_target)
    {
	if ( ( victim2 = get_char_room( ch, arg2 ) ) == NULL )
    	{send_to_char("The spell failed.\n\r",ch);return;}
	if (victim2->in_room == NULL)
    	{send_to_char("The spell failed.\n\r",ch);return;}
	if (victim == NULL || IS_NPC(victim) || IS_IMMUNE(victim, IMM_SUMMON))
    	{send_to_char("The spell failed.\n\r",ch);return;}
	if (!IS_NPC(victim2) && !IS_IMMUNE(victim2, IMM_SUMMON))
    	{send_to_char("The spell failed.\n\r",ch);return;}
	if (victim == victim2)
    	{send_to_char("The spell failed.\n\r",ch);return;}
    }
    else if (second_victim && victim_target && area_affect && !global_target && !object_target)
    {
	if ( ( victim2 = get_char_world( ch, arg2 ) ) == NULL )
    	{send_to_char("The spell failed.\n\r",ch);return;}
	if (victim2->in_room == NULL ||
	    victim2->in_room->area != ch->in_room->area)
    	{send_to_char("The spell failed.\n\r",ch);return;}
	if (victim == NULL || IS_NPC(victim) || IS_IMMUNE(victim, IMM_SUMMON))
    	{send_to_char("The spell failed.\n\r",ch);return;}
	if (!IS_NPC(victim2) && !IS_IMMUNE(victim2, IMM_SUMMON))
    	{send_to_char("The spell failed.\n\r",ch);return;}
	if (victim == victim2)
    	{send_to_char("The spell failed.\n\r",ch);return;}
    }
    else if (second_victim && victim_target && global_target && !object_target)
    {
	if ( ( victim2 = get_char_world( ch, arg2 ) ) == NULL )
    	{send_to_char("The spell failed.\n\r",ch);return;}
	if (victim2->in_room == NULL)
    	{send_to_char("The spell failed.\n\r",ch);return;}
	if (victim == NULL || IS_NPC(victim) || IS_IMMUNE(victim, IMM_SUMMON))
    	{send_to_char("The spell failed.\n\r",ch);return;}
	if (!IS_NPC(victim2) && !IS_IMMUNE(victim2, IMM_SUMMON))
    	{send_to_char("The spell failed.\n\r",ch);return;}
	if (victim == victim2)
    	{send_to_char("The spell failed.\n\r",ch);return;}
    }
    else if (second_object && object_target && !area_affect && !global_target && !victim_target)
    {
	if ( ( obj2 = get_obj_carry( ch, arg2 ) ) == NULL )
    	{send_to_char("The spell failed.\n\r",ch);return;}
    }
    else if (second_object && object_target && area_affect && !global_target && !victim_target)
    {
	if ( ( obj2 = get_obj_here( ch, arg2 ) ) == NULL )
    	{send_to_char("The spell failed.\n\r",ch);return;}
    }
    else if (second_object && object_target && global_target && !victim_target)
    {
	if ( ( obj2 = get_obj_world( ch, arg2 ) ) == NULL )
    	{send_to_char("The spell failed.\n\r",ch);return;}
    }

    if (victim != NULL)
/*    {
if (ch->pcdata->powers[MPOWER_RUNE0] == GREEN_MAGIC ||
ch->pcdata->powers[MPOWER_RUNE0] == YELLOW_MAGIC ||
ch->pcdata->powers[MPOWER_RUNE0] == BLUE_MAGIC ||
ch->pcdata->powers[MPOWER_RUNE0] == RED_MAGIC)*/
	{
	if (victim->level > ch->spl[PURPLE_MAGIC])
	{
	    send_to_char("The spell failed.\n\r",ch);
	    return;
	}
    }

    if (victim2 != NULL)
    {
	if (victim2->level > ch->spl[PURPLE_MAGIC])
	{
	    send_to_char("The spell failed.\n\r",ch);
	    return;
	}
    }

    switch ( action_bit )
    {
    default:
	send_to_char("The spell failed.\n\r",ch);
	return;
    case ACTION_MOVE:
	if (!victim_target && !second_victim && !object_target && !second_object)
	{
	    if (cast_message) act2(c_m,ch,NULL,NULL,TO_CHAR);
	    if (message_one) act2(c_1,ch,NULL,NULL,TO_ROOM);
	    char_from_room(ch);
	    char_to_room(ch,old_room);
	    if (message_two) act2(c_2,ch,NULL,NULL,TO_ROOM);
	}
	else if ( arg1[0] == '\0' )
	{
	    send_to_char("Please specify a target.\n\r",ch);
	    return;
	}
	else if (victim_target && !second_victim && !object_target && !second_object)
	{
	    if (victim == NULL || victim->in_room == NULL ||
		victim->in_room == ch->in_room || IS_SET (victim->in_room->room_flags,
ROOM_NO_RECALL) ||
		IS_SET (ch->in_room->room_flags, ROOM_NO_RECALL))
	    {send_to_char("The spell failed.\n\r",ch);return;}
	    if (is_reversed)
	    {
		if (victim->position == POS_FIGHTING)
		{send_to_char("The spell failed.\n\r",ch);return;}
		if (cast_message) act2(c_m,victim,NULL,NULL,TO_CHAR);
		if (message_one) act2(c_1,victim,NULL,NULL,TO_ROOM);
		char_from_room(victim);
		char_to_room(victim,ch->in_room);
		if (IS_NPC(victim)) {
		    if (IS_SET(victim->act, ACT_STAY_AREA))
			REMOVE_BIT(victim->act, ACT_STAY_AREA);
		    if (IS_SET(victim->act, ACT_SENTINEL))
			REMOVE_BIT(victim->act, ACT_SENTINEL);
		    if (IS_SET(victim->act, ACT_AGGRESSIVE))
			REMOVE_BIT(victim->act, ACT_AGGRESSIVE);}
		if (message_two) act2(c_2,victim,NULL,NULL,TO_ROOM);
		do_look(victim,"");
	    }
	    else
	    {
		if (ch->position == POS_FIGHTING)
		{send_to_char("The spell failed.\n\r",ch);return;}
		if (cast_message) act2(c_m,ch,NULL,NULL,TO_CHAR);
		if (message_one) act2(c_1,ch,NULL,NULL,TO_ROOM);
		char_from_room(ch);
		char_to_room(ch,victim->in_room);
		if (message_two) act2(c_2,ch,NULL,NULL,TO_ROOM);
		do_look(ch,"");
		sprintf(buf, "%s magechants to %s", ch->name,
			victim->name);
		log_string(buf, victim->in_room->vnum);

	    }
	}
	else if (!victim_target && !second_victim && object_target && !second_object)
	{
	    if (obj == NULL || obj->in_room == NULL ||
		obj->in_room == ch->in_room || 
		IS_SET (ch->in_room->room_flags, ROOM_NO_TELEPORT) ||
		obj->in_room->vnum == ROOM_VNUM_PUNISHMENT)
	    {send_to_char("The spell failed.\n\r",ch);return;}
                if (ch->position == POS_FIGHTING)
                {send_to_char("The spell failed.\n\r",ch);return;}
	if (( !CAN_WEAR(obj, ITEM_TAKE) ) && IS_SET(obj->in_room->room_flags, ROOM_NO_TELEPORT))
	{send_to_char("The spell failed.\n\r",ch);return;}
      if ( IS_SET( obj->in_room->room_flags, ROOM_SAVE_EQ ) ) 	{send_to_char("The spell failed.\n\r",ch);return;}
	    if (cast_message) act2(c_m,ch,obj,NULL,TO_CHAR);
	    if (message_one) act2(c_1,ch,obj,NULL,TO_ROOM);
	    if (is_reversed)
	    {
		if ( IS_SET(obj->quest, QUEST_ARTIFACT) )
		{
		    send_to_char("Not on artifacts or things that contain artifacts.", ch);
		    return;
		}
		for (obj_within = obj->contains; obj_within != NULL;
		    obj_within = obj_within->next_content)
		{
		    if (IS_SET(obj_within->quest, QUEST_ARTIFACT))
		    {
			send_to_char("Not on things that contain artifacts.\r\n", ch);
			return;
		    }
		}
		obj_from_room(obj);
		obj_to_room(obj,ch->in_room);
	    }
	    else
	    {
		char_from_room(ch);
		char_to_room(ch,obj->in_room);
		do_look(ch,"");
	    }
	    if (message_two) act2(c_2,ch,obj,NULL,TO_ROOM);
	}
	else if (victim_target && second_victim && !object_target && !second_object)
	{
	    if (victim == NULL || victim->in_room == NULL)
	    {send_to_char("The spell failed.\n\r",ch);return;}
	    if (victim2 == NULL || victim2->in_room == NULL ||
		victim2->in_room == victim->in_room)
	    {send_to_char("The spell failed.\n\r",ch);return;}
	    if (is_reversed)
	    {
		if (victim2->position == POS_FIGHTING)
		{send_to_char("The spell failed.\n\r",ch);return;}
		if (cast_message) act2(c_m,victim2,NULL,victim,TO_CHAR);
		if (message_one) act2(c_1,victim2,NULL,victim,TO_ROOM);
		char_from_room(victim2);
		char_to_room(victim2,victim->in_room);
		if (message_two) act2(c_2,victim2,NULL,victim,TO_ROOM);
		do_look(victim2,"");
	    }
	    else
	    {
		if (victim->position == POS_FIGHTING)
		{send_to_char("The spell failed.\n\r",ch);return;}
		if (cast_message) act2(c_m,victim,NULL,victim2,TO_CHAR);
		if (message_one) act2(c_1,victim,NULL,victim2,TO_ROOM);
		char_from_room(victim);
		char_to_room(victim,victim2->in_room);
		if (message_two) act2(c_2,victim,NULL,victim2,TO_ROOM);
		do_look(victim,"");
	    }
	}
	else if (victim_target && !second_victim && !object_target && second_object)
	{
	    if (victim == NULL || victim->in_room == NULL)
	    {send_to_char("The spell failed.\n\r",ch);return;}
	    if (obj2 == NULL || obj2->in_room == NULL ||
		obj2->in_room == victim->in_room)
	    {send_to_char("The spell failed.\n\r",ch);return;}
	    if (cast_message) act2(c_m,victim,NULL,NULL,TO_CHAR);
	    if (message_one) act2(c_1,victim,obj2,NULL,TO_ROOM);
	    if (is_reversed)
	    {
		obj_from_room(obj2);
		obj_to_room(obj2,victim->in_room);
	    }
	    else
	    {
		if (victim->position == POS_FIGHTING)
		{send_to_char("The spell failed.\n\r",ch);return;}
		char_from_room(victim);
		char_to_room(victim,obj2->in_room);
		do_look(victim,"");
	    }
	    if (message_two) act2(c_2,victim,obj2,NULL,TO_ROOM);
	}
	else if (!victim_target && !second_victim && object_target && second_object)
	{
	    if (obj == NULL || obj->in_room == NULL)
	    {send_to_char("The spell failed.\n\r",ch);return;}
	    if (obj2 == NULL || obj2->in_room == NULL ||
		obj2->in_room == obj->in_room)
	    {send_to_char("The spell failed.\n\r",ch);return;}
	    if (cast_message) act2(c_m,ch,obj,NULL,TO_CHAR);
	    if (is_reversed)
	    {
		old_room = ch->in_room;
		if (message_one) act2(c_1,ch,obj2,NULL,TO_ROOM);
		obj_from_room(obj2);
		obj_to_room(obj2,obj->in_room);
		char_from_room(ch);
		char_to_room(ch,obj->in_room);
		if (message_two) act2(c_2,ch,obj2,NULL,TO_ROOM);
		char_from_room(ch);
		char_to_room(ch,old_room);
	    }
	    else
	    {
		old_room = ch->in_room;
		if (message_one) act2(c_1,ch,obj,NULL,TO_ROOM);
		obj_from_room(obj);
		obj_to_room(obj,obj2->in_room);
		char_from_room(ch);
		char_to_room(ch,obj2->in_room);
		if (message_two) act2(c_2,ch,obj,NULL,TO_ROOM);
		char_from_room(ch);
		char_to_room(ch,old_room);
	    }
	}
	else if (!victim_target && second_victim && object_target && !second_object)
	{
	    if (victim2 == NULL || victim2->in_room == NULL)
	    {send_to_char("The spell failed.\n\r",ch);return;}
	    if (obj == NULL || obj->in_room == NULL ||
		obj->in_room == victim2->in_room)
	    {send_to_char("The spell failed.\n\r",ch);return;}
	    if (cast_message) act2(c_m,victim2,NULL,NULL,TO_CHAR);
	    if (message_one) act2(c_1,victim,obj2,NULL,TO_ROOM);
	    if (is_reversed)
	    {
		if (victim2->position == POS_FIGHTING)
		{send_to_char("The spell failed.\n\r",ch);return;}
		char_from_room(victim2);
		char_to_room(victim2,obj->in_room);
		do_look(victim2,"");
	    }
	    else
	    {
		obj_from_room(obj);
		obj_to_room(obj,victim2->in_room);
	    }
	    if (message_two) act2(c_2,victim2,obj,NULL,TO_ROOM);
	}
	else {send_to_char("The spell failed.\n\r",ch);return;}
	break;
    case ACTION_MOB:
	if ( action_type < 1)
	{send_to_char("The spell failed.\n\r",ch);return;}
	if (IS_NPC(ch) || ch->pcdata->followers > 4)
	{send_to_char("The spell failed.\n\r",ch);return;}
	if (( victim = create_mobile( get_mob_index( action_type ))) == NULL )
	{send_to_char("The spell failed.\n\r",ch);return;}
	if (cast_message) act2(c_m,ch,NULL,victim,TO_CHAR);
	if (message_one) act2(c_1,ch,NULL,victim,TO_ROOM);
	ch->pcdata->followers++;
	char_to_room( victim, ch->in_room );
	SET_BIT(victim->act, ACT_NOEXP);
	if (IS_SET(victim->act, ACT_STAY_AREA)) 
	    REMOVE_BIT(victim->act, ACT_STAY_AREA);
	if (IS_SET(victim->act, ACT_SENTINEL))
	    REMOVE_BIT(victim->act, ACT_SENTINEL);
	if (IS_SET(victim->act, ACT_AGGRESSIVE))
	    REMOVE_BIT(victim->act, ACT_AGGRESSIVE);
	free_string(victim->lord);
	victim->lord = str_dup(ch->name);
	if (victim->level > ch->spl[PURPLE_MAGIC])
	{
	    send_to_char("The spell failed.\n\r",ch);
	    extract_char(victim, TRUE);
	    return;
	}
	break;
    case ACTION_OBJECT:
       if (IS_SET(ch->in_room->room_flags, ROOM_NO_TELEPORT))
        {
        send_to_char("You cannot use object spells in no teleport rooms.\n\r",ch);
        return;
        }
	if ( action_type < 1)
	{
	    send_to_char("The spell failed.\n\r",ch);
	    return;
	}
// Puck
	if (( obj = create_object( get_obj_index( action_type ),0 )) == NULL )
	{
	    send_to_char("The spell failed.\n\r",ch);
	    return;
	}
	if (IS_SET(obj->quest, QUEST_RELIC) ||
	    IS_SET(obj->quest, QUEST_ARTIFACT) ||
	    obj->item_type == ITEM_QUEST || obj->item_type == ITEM_QUESTCARD)
	{
	    send_to_char("The spell failed.\n\r", ch);
	    return;
	}

	if (cast_message) 	act2(c_m,ch,obj,NULL,TO_CHAR);
	if (message_one) 	act2(c_1,ch,obj,NULL,TO_ROOM);
	free_string(obj->questmaker);
	obj->questmaker = str_dup(ch->name);
	obj_to_room( obj, ch->in_room );
        sprintf( log_buf, "%s mage creates %s", ch->name, obj->short_descr );
        log_string( log_buf, obj->oid );


	break;
    }
    if (!IS_IMMORTAL( ch ))
    {
    	WAIT_STATE(ch,12);
    	ch->mana -= mana_cost;
    }

    if (IS_SET(page->spectype, ADV_NEXT_PAGE) &&
	!IS_SET(page->spectype, ADV_SPELL_FIRST))
    {
    	if (strlen(page->chpoweroff) < 2 || !str_cmp(page->chpoweroff,"(null)"))
    	{send_to_char("The spell failed.\n\r",ch);return;}
	if (IS_SET(page->spectype, ADV_PARAMETER))
	{
	    if (!str_cmp(page->chpoweron,"(null)"))
		{send_to_char("The spell failed.\n\r",ch);return;}
	    else strcpy(next_par,page->chpoweron);
	}
	else strcpy(next_par,argument);
	if ( page->specpower < page->value[0] )
	{send_to_char( "The spell failed.\n\r", ch );return;}
	if ( ( page_next = get_page( book, page->specpower ) ) == NULL )
	{send_to_char( "The spell failed.\n\r", ch );return;}
    	if (IS_SET(page_next->quest, QUEST_MASTER_RUNE))
    	{
	    if      (IS_SET(page_next->spectype, ADV_DAMAGE))
	    	adv_spell_damage(ch,book,page_next,next_par);
	    else if (IS_SET(page_next->spectype, ADV_AFFECT))
	    	adv_spell_affect(ch,book,page_next,next_par);
	    else if (IS_SET(page_next->spectype, ADV_ACTION))
	    	adv_spell_action(ch,book,page_next,next_par);
	    else {send_to_char( "The spell failed.\n\r", ch );return;}
    	}
	else {send_to_char( "The spell failed.\n\r", ch );return;}
    }
    return;
}
void area_scry(CHAR_DATA *ch, char *argument)
{
    CHAR_DATA *victim;
    char buf [MAX_STRING_LENGTH];
    char arg [MAX_STRING_LENGTH];

    one_argument(argument, arg);

    if ( ( victim = get_char_world(ch, arg) ) == NULL )
    {
        send_to_char("The spell failed.\r\n", ch);
        return;
    }
    if ( !IS_NPC(victim) && IS_IMMUNE(victim, IMM_SHIELDED) ) 
    {
        send_to_char("The spell failed.\n\r", ch);
        return;
    }

    if (!IS_NPC(victim))
	sprintf(buf, "You locate %s in %s.\r\n", victim->name,victim->in_room->area->name);
    else
	sprintf(buf, "You locate %s in %s.\r\n", victim->short_descr, victim->in_room->area->name);
    send_to_char(buf, ch);
    return;
}

void readmagic ( CHAR_DATA *ch, char *argument)
{
    CHAR_DATA *victim;
    char arg [MAX_STRING_LENGTH];
    char buf [MAX_STRING_LENGTH];

    one_argument(argument, arg);

    if ( (victim = get_char_world(ch,arg) ) == NULL)
    {
	send_to_char("The spell failed.\r\n", ch);
	return;
    }

    if (!IS_NPC(victim) && IS_IMMUNE(victim, IMM_SHIELDED))
    {
	send_to_char("You have trouble reading their magical aura.\r\n", ch);
	return;
    }

    sprintf(buf, "\r\nYou read %s's magical aura:\r\n",
	IS_NPC(victim)?victim->short_descr:victim->name);
    send_to_char(buf,ch);
    sprintf(buf, "Hp:%d/%d, Mana:%d/%d, Move:%d/%d.\r\n", victim->hit,
	victim->max_hit, victim->mana, victim->max_mana, victim->move,
	victim->max_move);
    send_to_char(buf, ch);
    sprintf(buf,"Hitroll:%d, Damroll:%d, AC:%d Save:%d.\r\n",
	char_hitroll(victim), char_damroll(victim), char_ac(victim), char_save(victim));
    send_to_char(buf, ch);
    if (!IS_NPC(victim))
    {
    	sprintf(buf, "Red: %d Blue: %d Green: %d Purple: %d Yellow: %d.\r\n", 
		victim->spl[RED_MAGIC], victim->spl[BLUE_MAGIC],
		victim->spl[GREEN_MAGIC], victim->spl[PURPLE_MAGIC],
		victim->spl[YELLOW_MAGIC]);
	send_to_char(buf, ch);
    }

    return;

}

void flame_room (CHAR_DATA *ch)
{
    CHAR_DATA *vch;
    CHAR_DATA *vch_next;

    for ( vch = char_list; vch != NULL; vch = vch_next )
    {
	vch_next	= vch->next;
	if ( vch->in_room == NULL )
	    continue;
	if ( vch->in_room == ch->in_room )
	{
	    if ( vch == ch ) continue;
	    if (IS_AFFECTED(vch, AFF_FLAMING)) continue;
	    if (number_percent() > 80 && !IS_AFFECTED(vch, AFF_FLAMING))
	    {
		SET_BIT(vch->affected_by, AFF_FLAMING);
		act("A spark of magical energy sets you on fire!", vch, NULL, NULL, TO_CHAR);
		act("A spark of magical energy sets $n on fire!", vch, NULL, NULL, TO_ROOM);
	    }
	}
    }

    return;
}

void do_magearmour( CHAR_DATA *ch, char *argument )
{
    OBJ_INDEX_DATA *pObjIndex;
    OBJ_DATA *obj;
    char arg[MAX_INPUT_LENGTH];
    int vnum = 0;
    char buf[MAX_STRING_LENGTH];
			  
    argument = one_argument( argument, arg );
          
    if (IS_NPC(ch)) return;
          
    if (!IS_CLASS(ch, CLASS_MAGE))
    {
	send_to_char("Huh?\n\r",ch);
	return;
    }
          
    if (arg[0] == '\0')
    {
	send_to_char("Please specify which piece of mage armor you wish to create:\nCloak, Belt, Helmet or Plate.\n\r",ch);
	return;
    }
			  
    if      (!str_cmp(arg,"plate"    )) vnum = 29692;
    else if (!str_cmp(arg,"belt"     )) vnum = 29693;
    else if (!str_cmp(arg,"cloak"     )) vnum = 29694;
    else if (!str_cmp(arg,"helmet"     )) vnum = 29690;

    if ( vnum == 0 || (pObjIndex = get_obj_index( vnum )) == NULL)
    {
	send_to_char("Missing object, please inform an immortal.\n\r",ch);
	return;
    }
              
    if ( 1000000 > ch->exp )
    {
	sprintf(buf,"It costs you 1000000 exp to create a piece of mage equipment.\n\r");
	send_to_char(buf,ch);
	return;
    }
			  
    ch->exp -= 1000000;
    obj = create_object(pObjIndex, 50);
    obj_to_char(obj, ch);
    act("You chant some mystical words and $p appears in your hands.",ch,obj,NULL,TO_CHAR);
    act("$n chants some mystical words and $p appears in $s hands.",ch,obj,NULL,TO_ROOM);
    return;
}


