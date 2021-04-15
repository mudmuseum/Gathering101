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

void do_claw( CHAR_DATA *ch, char *argument )
{
    CHAR_DATA *victim;
    char      arg [MAX_INPUT_LENGTH];
    char      buf [MAX_INPUT_LENGTH];
    bool      can_sire = FALSE;

    argument = one_argument( argument, arg );

    if (IS_NPC(ch)) return;

    if (!IS_CLASS(ch, CLASS_WEREWOLF) || (ch->pcdata->stats[UNI_GEN] < 1))
    {
	do_claws(ch,"");
	return;
    }

    if (IS_SET(ch->special,SPC_PRINCE)) 	can_sire = TRUE;
    else if (IS_SET(ch->special,SPC_SIRE)) 	can_sire = TRUE;
    else if (ch->pcdata->stats[UNI_GEN] == 1 || 
	ch->pcdata->stats[UNI_GEN] == 2) 	can_sire = TRUE;
    if (ch->pcdata->stats[UNI_GEN] > 4) 	can_sire = FALSE;

    if (!can_sire)
    {
	send_to_char("You are unable to spread your gift.\n\r",ch);
	return;
    }

    if (strlen(ch->clan) < 2 && ch->pcdata->stats[UNI_GEN] != 1 )
    {
	send_to_char( "First you need to create a tribe.\n\r", ch );
	return;
    }

    if ( arg[0] == '\0' )
    {
	send_to_char( "Claw whom?\n\r", ch );
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
	send_to_char( "You cannot claw yourself.\n\r", ch );
	return;
    }

    if (IS_CLASS(victim, CLASS_MAGE))
    {
	send_to_char( "You cannot bite mages.\n\r", ch );
	return;
    }

    if ( victim->level != LEVEL_AVATAR && !IS_IMMORTAL(victim) )
    {
	send_to_char( "You can only claw avatars.\n\r", ch );
	return;
    }

    if (IS_CLASS(victim, CLASS_VAMPIRE) || IS_SET(victim->pcdata->stats[UNI_AFF], VAM_MORTAL))
    {
	send_to_char( "You are unable to create werevamps!\n\r", ch );
	return;
    }

    if (IS_CLASS(victim, CLASS_DEMON))
    {
	send_to_char( "But they have no soul!\n\r", ch );
	return;
    }

    if (IS_CLASS(victim, CLASS_HIGHLANDER))
    {
	send_to_char( "You cannot turn highlanders into werewolves.\n\r", ch );
	return;
    }

    if (IS_CLASS(victim, CLASS_WEREWOLF))
    {
	send_to_char( "But they are already a werewolf!\n\r", ch );
	return;
    }

    if (!IS_IMMUNE(victim,IMM_VAMPIRE))
    {
	send_to_char( "You cannot claw an unwilling person.\n\r", ch );
	return;
    }

    if (!IS_VAMPAFF(ch,VAM_CLAWS))
    {
	send_to_char("First you better get your claws out!\n\r",ch);
	return;
    }

    if (ch->exp < 10000)
    {
	send_to_char("You cannot afford the 10000 exp to pass on the gift.\n\r",ch);
	return;
    }

    ch->exp = ch->exp - 10000;
    if (ch->pcdata->stats[UNI_CURRENT] < 1) ch->pcdata->stats[UNI_CURRENT] = 1;
    else ch->pcdata->stats[UNI_CURRENT] += 1;
    if (IS_SET(ch->special,SPC_SIRE)) REMOVE_BIT(ch->special,SPC_SIRE);
    act("You plunge your claws into $N.", ch, NULL, victim, TO_CHAR);
    act("$n plunges $s claws into $N.", ch, NULL, victim, TO_NOTVICT);
    act("$n plunges $s claws into your chest.", ch, NULL, victim, TO_VICT);
    victim->class=CLASS_WEREWOLF;
    send_to_char( "You are now a werewolf.\n\r", victim );
    victim->pcdata->stats[UNI_GEN] = ch->pcdata->stats[UNI_GEN] + 1;
    free_string(victim->lord);
    if (ch->pcdata->stats[UNI_GEN] == 1)
	victim->lord=str_dup(ch->name);
    else
    {
	sprintf(buf,"%s %s",ch->lord,ch->name);
	victim->lord=str_dup(buf);
    }
    free_string(victim->clan);
    victim->clan=str_dup(ch->clan);
    victim->pcdata->stats[UNI_AFF] = 0;
    victim->pcdata->stats[UNI_CURRENT] = 0;
    victim->pcdata->stats[WOLF_AUSPICE] = time_info.lunar_day;
    save_char_obj(ch);
    save_char_obj(victim);
    return;
}

void do_totems( CHAR_DATA *ch, char *argument )
{
    char arg1[MAX_INPUT_LENGTH];
    char arg2[MAX_INPUT_LENGTH];
    char buf[MAX_STRING_LENGTH];

    argument = one_argument( argument, arg1 );
    argument = one_argument( argument, arg2 );

    if (IS_NPC(ch)) return;

    if (!IS_CLASS(ch, CLASS_WEREWOLF))
    {
	send_to_char("Huh?\n\r",ch);
	return;
    }

    if (arg1[0] == '\0' && arg2[0] == '\0')
    {
	sprintf(buf,"Totems: Bear (%d), Lynx (%d), Boar (%d), Owl (%d), Spider (%d), Wolf (%d),\n\r        Hawk (%d), Mantis (%d), Stag (%d).\n\r",
	ch->pcdata->powers[WPOWER_BEAR], ch->pcdata->powers[WPOWER_LYNX],
	ch->pcdata->powers[WPOWER_BOAR], ch->pcdata->powers[WPOWER_OWL],
	ch->pcdata->powers[WPOWER_SPIDER], ch->pcdata->powers[WPOWER_WOLF],
	ch->pcdata->powers[WPOWER_HAWK],ch->pcdata->powers[WPOWER_MANTIS],
	ch->pcdata->powers[WPOWER_STAG]);
	send_to_char(buf,ch);
	return;
    }
    if (arg2[0] == '\0')
    {
	if (!str_cmp(arg1,"bear"))
	{
	    send_to_char("Bear: The totem of strength and aggression.\n\r",ch);
	    if (ch->pcdata->powers[WPOWER_BEAR] < 1) 
		send_to_char("You have none of the Bear totem powers.\n\r",ch);
	    if (ch->pcdata->powers[WPOWER_BEAR] > 0) 
		send_to_char("FLEX: You strength is so great that no ropes can hold you.\n\r",ch);
	    if (ch->pcdata->powers[WPOWER_BEAR] > 1) 
		send_to_char("RAGE: You are able to build yourself up a rage at will.\n\r",ch);
	    if (ch->pcdata->powers[WPOWER_BEAR] > 2) 
		send_to_char("Steel claws: Your claws are so tough that they can parry weapons.\n\r",ch);
	    if (ch->pcdata->powers[WPOWER_BEAR] > 3) 
		send_to_char("Hibernation: Your wounds heal at amazing speeds when you sleep.\n\r",ch);
	    return;
	}
	else if (!str_cmp(arg1,"boar"))
	{
	    send_to_char("Boar: The totem of toughness and perserverance.\n\r",ch);
	    if (ch->pcdata->powers[WPOWER_BOAR] < 1) 
		send_to_char("You have none of the Boar totem powers.\n\r",ch);
	    if (ch->pcdata->powers[WPOWER_BOAR] > 0) 
		send_to_char("Shatter: No door is sturdy enough to resist you.\n\r",ch);
	    if (ch->pcdata->powers[WPOWER_BOAR] > 1) 
		send_to_char("CHARGE: Your first blow in combat has a +50 damage bonus.\n\r",ch);
	    if (ch->pcdata->powers[WPOWER_BOAR] > 2) 
		send_to_char("Toughness: Your skin is extremely tough. You take half damage in combat.\n\r",ch);
	    if (ch->pcdata->powers[WPOWER_BOAR] > 3) 
		send_to_char("Immovability: You are able to shrug off blows that would knock out most people.\n\r",ch);
	    if (ch->pcdata->powers[WPOWER_BOAR] > 4) 
		send_to_char("Might of Fenris:  You have the chance of dealing far more damage in combat.\n\r",ch);
	    return;
	}
	else if (!str_cmp(arg1,"lynx"))
	{
	    send_to_char("Lynx: The totem of speed and agility.\n\r",ch);
	    if (ch->pcdata->powers[WPOWER_LYNX] < 1) 
		send_to_char("You have none of the Lynx totem powers.\n\r",ch);
	    if (ch->pcdata->powers[WPOWER_LYNX] > 0) 
		send_to_char("Light footed: You move so lightly that you leave no tracks behind you.\n\r",ch);
	    if (ch->pcdata->powers[WPOWER_LYNX] > 1) 
		send_to_char("Stalker: You are able hunt people with much greater speed than normal.\n\r",ch);
	    if (ch->pcdata->powers[WPOWER_LYNX] > 2) 
		send_to_char("Combat speed: You have an extra attack in combat.\n\r",ch);
	    if (ch->pcdata->powers[WPOWER_LYNX] > 3) 
		send_to_char("Lightning Claws: Yours claws parry blows with lightning fast speed.\n\r",ch);
	    if (ch->pcdata->powers[WPOWER_LYNX] > 4) 
		send_to_char("Combat Skill: You can gain an additional skill in unarmed combat.\n\r",ch);
	    return;
	}
	else if (!str_cmp(arg1,"Owl"))
	{
	    send_to_char("Owl: The totem of thought and spiritualism.\n\r",ch);
	    if (ch->pcdata->powers[WPOWER_OWL] < 1) 
		send_to_char("You have none of the Owl totem powers.\n\r",ch);
	    if (ch->pcdata->powers[WPOWER_OWL] > 0) 
		send_to_char("VANISH: You are able to conceal yourself from all but the most perceptive.\n\r",ch);
	    if (ch->pcdata->powers[WPOWER_OWL] > 1) 
		send_to_char("SHIELD: You are able to shield your mind from scrying and aura-reading.\n\r",ch);
	    if (ch->pcdata->powers[WPOWER_OWL] > 2) 
		send_to_char("SHADOWPLANE: You are able to enter the shadow plane.\n\r",ch);
	    if (ch->pcdata->powers[WPOWER_OWL] > 3) 
		send_to_char("Magical Control: You are able to fully control your magic in crinos form.\n\r",ch);
	    return;
	}
	else if (!str_cmp(arg1,"Spider"))
	{
	    send_to_char("Spider: The totem of ambush and cunning.\n\r",ch);
	    if (ch->pcdata->powers[WPOWER_SPIDER] < 1) 
		send_to_char("You have none of the Spider totem powers.\n\r",ch);
	    if (ch->pcdata->powers[WPOWER_SPIDER] > 0) 
		send_to_char("Poisonous bite: Your bite injects your opponents with a deadly venom.\n\r",ch);
	    if (ch->pcdata->powers[WPOWER_SPIDER] > 1) 
		send_to_char("WEB: You are able to shoot a web at your opponents to entrap them.\n\r",ch);
	    if (ch->pcdata->powers[WPOWER_SPIDER] > 2) 
		send_to_char("Immunity to poison: Poisons have no affect upon you.\n\r",ch);
	    return;
	}
	else if (!str_cmp(arg1,"Wolf"))
	{
	    send_to_char("Wolf: Controlling your innate wolf powers.\n\r",ch);
	    if (ch->pcdata->powers[WPOWER_WOLF] < 1) 
		send_to_char("You have none of the Wolf totem powers.\n\r",ch);
	    if (ch->pcdata->powers[WPOWER_WOLF] > 0) 
		send_to_char("CLAWS: You can extend or retract your claws at will.\n\r",ch);
	    if (ch->pcdata->powers[WPOWER_WOLF] > 1) 
		send_to_char("FANGS: You can extend or retract your fangs at will.\n\r",ch);
	    if (ch->pcdata->powers[WPOWER_WOLF] > 2) 
		send_to_char("CALM: You are able to repress your inner beast at will.\n\r",ch);
	    if (ch->pcdata->powers[WPOWER_WOLF] > 3) 
		send_to_char("Spirit of Fenris: You are able to enter rage faster than normal.\n\r",ch);
	    return;
	}
	else if (!str_cmp(arg1,"Hawk"))
	{
	    send_to_char("Hawk: The totem of vision and perception.\n\r",ch);
	    if (ch->pcdata->powers[WPOWER_HAWK] < 1) 
		send_to_char("You have none of the Wolf totem powers.\n\r",ch);
	    if (ch->pcdata->powers[WPOWER_HAWK] > 0) 
		send_to_char("NIGHTSIGHT: You can see perfectly well in the dark.\n\r",ch);
	    if (ch->pcdata->powers[WPOWER_HAWK] > 1) 
		send_to_char("SHADOWSIGHT: You can see into the plane of shadows.\n\r",ch);
	    if (ch->pcdata->powers[WPOWER_HAWK] > 2) 
		send_to_char("TRUESIGHT: You have perfect vision.\n\r",ch);
	    return;
	}
	else if (!str_cmp(arg1,"Mantis"))
	{
	    send_to_char("Mantis: The totem of dexterity and reflexes.\n\r",ch);
	    if (ch->pcdata->powers[WPOWER_MANTIS] < 1) 
		send_to_char("You have none of the Mantis totem powers.\n\r",ch);
	    if (ch->pcdata->powers[WPOWER_MANTIS] > 4) 
		send_to_char("Incredibly fast attacks: Your opponents get -25 to parry and -50 to dodge.\n\r",ch);
	    if (ch->pcdata->powers[WPOWER_MANTIS] > 3) 
		send_to_char("Incredibly fast attacks: Your opponents get -20 to parry and -40 to dodge.\n\r",ch);
	    else if (ch->pcdata->powers[WPOWER_MANTIS] > 2) 
		send_to_char("Extremely fast attacks: Your opponents get -15 to parry and -30 to dodge.\n\r",ch);
	    else if (ch->pcdata->powers[WPOWER_MANTIS] > 1) 
		send_to_char("Very fast attacks: Your opponents get -10 to parry and -20 to dodge.\n\r",ch);
	    else if (ch->pcdata->powers[WPOWER_MANTIS] > 0) 
		send_to_char("Fast attacks: Your opponents get -5 to parry and -10 to dodge.\n\r",ch);
	    return;
	}
	else if (!str_cmp(arg1, "Stag"))
	{
	    send_to_char("Stag: The totem of grace and spirit.\r\n", ch);
	    if (ch->pcdata->powers[WPOWER_STAG] < 1)
	    	send_to_char("You have none of the Stag totem powers.\r\n", ch);
	    if (ch->pcdata->powers[WPOWER_STAG] > 0)
		send_to_char("GNOSIS: You regenerate incredibly fast when concentrating.\r\n", ch);
	    if (ch->pcdata->powers[WPOWER_STAG] > 1)
		send_to_char("FETISH: You can create objects with spirits bound in them.\r\n", ch);
	    if (ch->pcdata->powers[WPOWER_STAG] > 2)
		send_to_char("BRIDGE: You can create a moon bridge - the ability to travel to other Garou.\r\n", ch);
	    return;
	}

	sprintf(buf,"Totems: Bear (%d), Lynx (%d), Boar (%d), Owl (%d), Spider (%d), Wolf (%d),\n\r        Hawk (%d), Mantis (%d), Stag (%d).\n\r",
	ch->pcdata->powers[WPOWER_BEAR], ch->pcdata->powers[WPOWER_LYNX],
	ch->pcdata->powers[WPOWER_BOAR], ch->pcdata->powers[WPOWER_OWL],
	ch->pcdata->powers[WPOWER_SPIDER], ch->pcdata->powers[WPOWER_WOLF],
	ch->pcdata->powers[WPOWER_HAWK],ch->pcdata->powers[WPOWER_MANTIS],
	ch->pcdata->powers[WPOWER_STAG]);
	send_to_char(buf,ch);
	return;
    }
 
    if (!str_cmp(arg2,"improve"))
    {
	int improve;
	int cost;
	int max;

	     if (!str_cmp(arg1,"bear"   )) {improve = WPOWER_BEAR;   max=4;}
	else if (!str_cmp(arg1,"boar"   )) {improve = WPOWER_BOAR;   max=5;}
	else if (!str_cmp(arg1,"lynx"   )) {improve = WPOWER_LYNX;   max=5;}
	else if (!str_cmp(arg1,"owl"    )) {improve = WPOWER_OWL;    max=4;}
	else if (!str_cmp(arg1,"spider" )) {improve = WPOWER_SPIDER; max=3;}
	else if (!str_cmp(arg1,"wolf"   )) {improve = WPOWER_WOLF;   max=4;}
	else if (!str_cmp(arg1,"hawk"   )) {improve = WPOWER_HAWK;   max=3;}
	else if (!str_cmp(arg1,"mantis" )) {improve = WPOWER_MANTIS; max=5;}
	else if (!str_cmp(arg1,"stag"	)) {improve = WPOWER_STAG;   max=3;}
	else
	{
	    send_to_char("You can improve: Bear, Boar, Lynx, Owl, Spider, Wolf, Hawk, Mantis or Stag.\n\r",ch);
	    return;
	}
	cost = (ch->pcdata->powers[improve]+1) * 10;
	arg1[0] = UPPER(arg1[0]);
	if ( ch->pcdata->powers[improve] >= max )
	{
	    sprintf(buf,"You have already gained all the powers of the %s totem.\n\r", arg1);
	    send_to_char(buf,ch);
	    return;
	}
	if ( cost > ch->practice )
	{
	    sprintf(buf,"It costs you %d primal to improve your %s totem.\n\r", cost, arg1);
	    send_to_char(buf,ch);
	    return;
	}
	ch->pcdata->powers[improve] += 1;
	ch->practice -= cost;
	sprintf(buf,"You improve your ability in the %s totem.\n\r", arg1);
	send_to_char(buf,ch);
    }
    else send_to_char("To improve a totem, type: Totem <totem type> improve.\n\r",ch);
    return;
}

void do_tribe( CHAR_DATA *ch, char *argument )
{
    char buf[MAX_STRING_LENGTH];
    char arg[MAX_INPUT_LENGTH];
    char clan[MAX_INPUT_LENGTH];
    CHAR_DATA *gch;

    one_argument( argument, arg );

    if (IS_NPC(ch)) return;
    if (!IS_CLASS(ch, CLASS_WEREWOLF))
    {
	send_to_char("Huh?\n\r",ch);
	return;
    }
    if ( strlen(ch->clan) > 16)
    {
	send_to_char("You need no tribe, foul spawn of the wyrm.\n\r",ch);
	return;
    }
    send_to_char("[      Name      ] [    Tribe    ] [ Hits  % ] [ Mana  % ] [ Move  % ] [  Exp  ]\n\r", ch );
    for ( gch = char_list; gch != NULL; gch = gch->next )
    {
	if ( IS_NPC(gch) ) continue;
	if ( !IS_CLASS(gch, CLASS_WEREWOLF) ) continue;
	if ( strlen(gch->clan) > 16) continue;
	if ( gch->clan != NULL && strlen(gch->clan) > 1 )
	    sprintf(clan,gch->clan);
	else if ( gch->pcdata->stats[UNI_GEN] == 1 )
	    sprintf(clan,"All");
	else
	    sprintf(clan,"None");
	{
	    sprintf( buf,
	    "[%-16s] [%-13s] [%-6d%3d] [%-6d%3d] [%-6d%3d] [%7d]\n\r",
		capitalize( gch->name ), clan,
		gch->hit,  (gch->hit  * 100 / gch->max_hit ),
		gch->mana, (gch->mana * 100 / gch->max_mana),
		gch->move, (gch->move * 100 / gch->max_move),
		gch->exp);
		send_to_char( buf, ch );
	}
    }
    return;
}

void do_bloodline( CHAR_DATA *ch, char *argument )
{
    char      gen1 [MAX_INPUT_LENGTH];
    char      gen2 [MAX_INPUT_LENGTH];
    char      gen3 [MAX_INPUT_LENGTH];
    char      gen4 [MAX_INPUT_LENGTH];
    char      buf  [MAX_STRING_LENGTH];
    char      buf2 [MAX_STRING_LENGTH];
    char   auspice [MAX_STRING_LENGTH];

    argument = one_argument( argument, gen1 );
    argument = one_argument( argument, gen2 );
    argument = one_argument( argument, gen3 );
    argument = one_argument( argument, gen4 );

    if (IS_NPC(ch)) return;

    if (gen1 != '\0') gen1[0] = UPPER(gen1[0]);
    if (gen2 != '\0') gen2[0] = UPPER(gen2[0]);
    if (gen3 != '\0') gen3[0] = UPPER(gen3[0]);
    if (gen4 != '\0') gen4[0] = UPPER(gen4[0]);

    switch ( moon_phase(ch->pcdata->stats[WOLF_AUSPICE]) )
    {
    	case PHASE_NEW: sprintf(auspice, "Ragabash"); break;
	case PHASE_CRESCENT: sprintf(auspice, "Theurge"); break;
	case PHASE_HALF: sprintf(auspice, "Philodox"); break;
	case PHASE_GIBBOUS: sprintf(auspice, "Galliard"); break;
	case PHASE_FULL: sprintf(auspice, "Ahroun"); break;
	default: sprintf(auspice, "Unkown auspice"); break;
    }

    if ( !str_cmp(ch->clan,"") && ch->pcdata->stats[UNI_GEN] != 1 )
    {
	strcpy(buf,"In the name of Gaia, I announce my Garou heritage.");
	do_say(ch,buf);
	sprintf(buf,"I am the %s %s, I am a Ronin of no tribe.", 
	auspice, ch->name);
	do_say(ch,buf);
	return;
    }

    strcpy(buf,"In the name of Gaia, I announce my Garou heritage.");
    do_say(ch,buf);

    if (ch->pcdata->stats[UNI_GEN] == 1)
	{do_say(ch, "I am the Chosen Champion of Gaia."); return;}
    else if (ch->pcdata->stats[UNI_GEN] == 2)
	sprintf(buf,"My name is %s, %s Chieftain of the %s tribe.", 
	    ch->name,auspice,ch->clan);
    else if (IS_SET(ch->special, SPC_PRINCE))
	sprintf(buf,"My name is %s, %s Shaman of the %s tribe.", 
	    ch->name, auspice, ch->clan);
    else
	sprintf(buf,"My name is %s, %s of the %s tribe.", 
	    ch->name, auspice, ch->clan);

    do_say(ch, buf);

    strcpy(buf2, "I am the ");

    switch (ch->pcdata->stats[UNI_GEN])
    {
	default:
	case 5: sprintf(buf2 + strlen(buf2), "pup of %s, ", gen4);
	case 4: sprintf(buf2 + strlen(buf2), "pup of %s, ", gen3);
	case 3: sprintf(buf2 + strlen(buf2), "pup of %s, ", gen2);
	case 2: sprintf(buf2 + strlen(buf2), "pup of %s, ", gen1);
	case 1: sprintf(buf2 + strlen(buf2), "Chosen Champion of Gaia.");
    }

    do_say(ch, buf2);
    return;
}

void do_werewolf( CHAR_DATA *ch, char *argument )
{
    char       buf[MAX_INPUT_LENGTH];
    char       arg[MAX_INPUT_LENGTH];
    CHAR_DATA *vch;
    CHAR_DATA *vch_next;
    OBJ_DATA  *obj;

    argument = one_argument( argument, arg );

    if (IS_NPC(ch)) return;
    if (!IS_CLASS(ch, CLASS_WEREWOLF)) return;
    if (IS_SET(ch->special, SPC_WOLFMAN)) return;
    SET_BIT(ch->special, SPC_WOLFMAN);

    send_to_char("You throw back your head and howl with rage!\n\r",ch);
    act("$n throws back $s head and howls with rage!.", ch, NULL, NULL, TO_ROOM);
    send_to_char("Coarse dark hair sprouts from your body.\n\r",ch);
    act("Coarse dark hair sprouts from $n's body.",ch,NULL,NULL,TO_ROOM);
    if (!IS_VAMPAFF(ch,VAM_NIGHTSIGHT))
    {
	send_to_char("Your eyes start glowing red.\n\r",ch);
	act("$n's eyes start glowing red.",ch,NULL,NULL,TO_ROOM);
	SET_BIT(ch->pcdata->stats[UNI_AFF], VAM_NIGHTSIGHT);
    }
    if (!IS_VAMPAFF(ch,VAM_FANGS))
    {
	send_to_char("A pair of long fangs extend from your mouth.\n\r",ch);
	act("A pair of long fangs extend from $n's mouth.",ch,NULL,NULL,TO_ROOM);
	SET_BIT(ch->pcdata->stats[UNI_AFF], VAM_FANGS);
    }
    if (!IS_VAMPAFF(ch,VAM_CLAWS))
    {
	send_to_char("Razor sharp talons extend from your fingers.\n\r",ch);
	act("Razor sharp talons extend from $n's fingers.",ch,NULL,NULL,TO_ROOM);
	SET_BIT(ch->pcdata->stats[UNI_AFF], VAM_CLAWS);
    }
    if ((obj = get_eq_char(ch,WEAR_WIELD)) != NULL && !IS_SET(obj->spectype, SITEM_WOLFWEAPON))
    {
	act("$p drops from your right hand.",ch,obj,NULL,TO_CHAR);
	act("$p drops from $n's right hand.",ch,obj,NULL,TO_ROOM);
	obj_from_char(obj);
	obj_to_char(obj, ch);
    }
    if ((obj = get_eq_char(ch,WEAR_HOLD)) != NULL && !IS_SET(obj->spectype, SITEM_WOLFWEAPON))
    {
	act("$p drops from your left hand.",ch,obj,NULL,TO_CHAR);
	act("$p drops from $n's left hand.",ch,obj,NULL,TO_ROOM);
	obj_from_char(obj);
	obj_to_char(obj, ch);
    }
    SET_BIT(ch->affected_by, AFF_POLYMORPH);
    SET_BIT(ch->pcdata->stats[UNI_AFF], VAM_DISGUISED);
    sprintf(buf,"%s the werewolf",ch->name);
    free_string(ch->morph);
    ch->morph = str_dup(buf);
    ch->pcdata->stats[UNI_RAGE] += 25;
    if (ch->pcdata->powers[WPOWER_WOLF] > 3) ch->pcdata->stats[UNI_RAGE] += 100;
    if (ch->pcdata->stats[UNI_RAGE] > 300) 
	ch->pcdata->stats[UNI_RAGE] = 300;

    if (moon_phase(time_info.lunar_day) == 
	 moon_phase(ch->pcdata->stats[WOLF_AUSPICE]) )
	ch->pcdata->stats[UNI_RAGE] = 350;

    for ( vch = char_list; vch != NULL; vch = vch_next )
    {
	vch_next	= vch->next;
	if ( vch->in_room == NULL )
	    continue;
	if ( ch == vch )
	{
	    act("You throw back your head and howl with rage!", ch, NULL, NULL, TO_CHAR);
	    continue;
	}
	if ( !IS_NPC(vch) && vch->pcdata->chobj != NULL )
	    continue;
	if (!IS_NPC(vch))
	{
	    if (vch->in_room == ch->in_room)
		act("$n throws back $s head and howls with rage!", ch, NULL, vch, TO_VICT);
	    else if (vch->in_room->area == ch->in_room->area)
		send_to_char("You hear a fearsome howl close by!\n\r", vch);
	    else
/*		send_to_char("You hear a fearsome howl far off in the 
   distance!\n\r", vch);*/
	    if (!CAN_PK(vch)) continue;
	}
	if ( vch->in_room == ch->in_room && can_see( ch, vch) )
	{
	    multi_hit( ch, vch, TYPE_UNDEFINED );
	    if (vch == NULL || vch->position <= POS_STUNNED) continue;
	    multi_hit( ch, vch, TYPE_UNDEFINED );
	    if (vch == NULL || vch->position <= POS_STUNNED) continue;
	    multi_hit( ch, vch, TYPE_UNDEFINED );
	}
    }
    return;
}

void do_unwerewolf( CHAR_DATA *ch, char *argument )
{
    char       arg[MAX_INPUT_LENGTH];

    argument = one_argument( argument, arg );

    if (IS_NPC(ch)) return;
    if (!IS_CLASS(ch, CLASS_WEREWOLF)) return;
    if (!IS_SET(ch->special, SPC_WOLFMAN)) return;
    REMOVE_BIT(ch->special, SPC_WOLFMAN);
    REMOVE_BIT(ch->affected_by, AFF_POLYMORPH);
    REMOVE_BIT(ch->pcdata->stats[UNI_AFF], VAM_DISGUISED);
    free_string(ch->morph);
    ch->morph = str_dup("");
    if (IS_VAMPAFF(ch,VAM_CLAWS))
    {
	send_to_char("Your talons slide back into your fingers.\n\r",ch);
	act("$n's talons slide back into $s fingers.",ch,NULL,NULL,TO_ROOM);
	REMOVE_BIT(ch->pcdata->stats[UNI_AFF], VAM_CLAWS);
    }
    if (IS_VAMPAFF(ch,VAM_FANGS))
    {
	send_to_char("Your fangs slide back into your mouth.\n\r",ch);
	act("$n's fangs slide back into $s mouth.",ch,NULL,NULL,TO_ROOM);
	REMOVE_BIT(ch->pcdata->stats[UNI_AFF], VAM_FANGS);
    }
    if (IS_VAMPAFF(ch,VAM_NIGHTSIGHT))
    {
	send_to_char("The red glow in your eyes fades.\n\r",ch);
	act("The red glow in $n's eyes fades.",ch,NULL,NULL,TO_ROOM);
	REMOVE_BIT(ch->pcdata->stats[UNI_AFF], VAM_NIGHTSIGHT);
    }
    send_to_char("Your coarse hair shrinks back into your body.\n\r",ch);
    act("$n's coarse hair shrinks back into $s body.",ch,NULL,NULL,TO_ROOM);
    ch->pcdata->stats[UNI_RAGE] -= 25;
    if (ch->pcdata->stats[UNI_RAGE] < 0) ch->pcdata->stats[UNI_RAGE] = 0;
    return;
}

void do_vanish( CHAR_DATA *ch, char *argument )
{
    if ( IS_NPC(ch) )
	return;

    if (!IS_CLASS(ch, CLASS_WEREWOLF) || ch->pcdata->powers[WPOWER_OWL] < 1)
    {
	send_to_char("Huh?\n\r",ch);
	return;
    }

    if ( IS_SET(ch->act, PLR_WIZINVIS) )
    {
	REMOVE_BIT(ch->act, PLR_WIZINVIS);
	send_to_char( "You slowly fade into existance.\n\r", ch );
	act("$n slowly fades into existance.",ch,NULL,NULL,TO_ROOM);
    }
    else
    {
	send_to_char( "You slowly fade out of existance.\n\r", ch );
	act("$n slowly fades out of existance.",ch,NULL,NULL,TO_ROOM);
	SET_BIT(ch->act, PLR_WIZINVIS);
    }
    return;
}

void do_web( CHAR_DATA *ch, char *argument )
{
    CHAR_DATA *victim;
    char arg [MAX_INPUT_LENGTH];
    int sn;
    int level;
    int spelltype;
    int cost = (number_range(200,400));

    argument = one_argument( argument, arg );

    if (IS_NPC(ch)) return;

    if (!IS_CLASS(ch, CLASS_WEREWOLF) || ch->pcdata->powers[WPOWER_SPIDER] < 2)
    {
	send_to_char("Huh?\n\r",ch);
	return;
    }

    if (ch->move < cost)
    {
	send_to_char("You're too fatigued to try and web someone.\n\r",ch);
	return;
    }

    if ( ( victim = get_char_room( ch, arg ) ) == NULL )
    {
	send_to_char( "They aren't here.\n\r", ch );
	return;
    }

    if ( ch == victim )
    {
	send_to_char( "You cannot web yourself.\n\r", ch );
	return;
    }

    if ( IS_AFFECTED(victim, AFF_ETHEREAL) )
    {
	send_to_char( "You cannot web an ethereal person.\n\r", ch );
	return;
    }

    ch->move -= cost;
    if ( ( sn = skill_lookup( "web" ) ) < 0 ) return;
    spelltype = skill_table[sn].target;
    level = ch->spl[spelltype] * 0.25;
    (*skill_table[sn].spell_fun) ( sn, level, ch, victim );
    WAIT_STATE( ch, 12 );
    return;
}

void do_gnosis ( CHAR_DATA *ch, char *argument )
{
    if (IS_NPC(ch)) return;
    
    if (!IS_CLASS(ch, CLASS_WEREWOLF))
    {
	send_to_char("Huh?\r\n", ch);
	return;
    }

    if (ch->pcdata->powers[WPOWER_STAG] < 1)
    {
	send_to_char("Huh?\r\n", ch);
	return;
    }
  
    if (ch->position != POS_RESTING)
    {
	send_to_char("You cannot enter gnosis unless you are resting.\r\n", ch);
	return;
    }

    if (ch->pcdata->stats[UNI_RAGE] > 0)
    {
	send_to_char("You must be calm and at peace before entering the state of gnosis.\r\n", ch);
	return;
    }

    ch->position = POS_GNOSIS;
    act("You attune yourself to the spirit world and allow your body to regenerate.", ch, NULL, NULL, TO_CHAR);
    act("$n closes $s eyes and begins chanting in the Garou tongue.", ch, NULL, NULL, TO_ROOM);
    return;
}

void do_fetish( CHAR_DATA *ch, char *argument )
{
    OBJ_INDEX_DATA *pObjIndex;
    OBJ_DATA *obj;
    char arg1[MAX_INPUT_LENGTH];
    char arg2[MAX_INPUT_LENGTH];
    char buf[MAX_STRING_LENGTH];
    int vnum = 0;
    int cost = 50;

    
    if (IS_NPC(ch)) return;

    if (!IS_CLASS(ch, CLASS_WEREWOLF)) 
    {
	send_to_char("Huh?\r\n", ch);
    }

    if (ch->pcdata->powers[WPOWER_STAG] < 2) 
    {
	send_to_char("You must advance your Stag totem to level 2 to create fetish objects.\r\n", ch);
	return;
    }

    if (argument[0] == '\0')
    {
	send_to_char("You can make the following fetish objects:\r\nMoongem, Lesser Klaive, Greater Klaive.\r\n", ch);
	return;
    }

    argument = one_argument(argument, arg1);
    argument = one_argument(argument, arg2);

    if (!str_cmp(arg1, "moongem"))
    {
	vnum = 29675;
	cost = 25;
    }

    else if (!str_cmp(arg1, "lesser") && !str_cmp(arg2, "klaive"))
    {
	vnum = 29691;
	cost = 60;
    }
    else if (!str_cmp(arg1, "greater") && !str_cmp(arg2, "klaive"))
    {
	vnum = 29676;
	cost = 100;
    }
    else
    {
	send_to_char("You can make the following fetish objects:\r\nMoongem, Lesser Klaive, Greater Klaive.\r\n", ch);
	return;
    }

    if (ch->practice < cost)
    {
	sprintf(buf, "It costs %d to create that fetish object.\r\n", cost);
	send_to_char(buf, ch);
	return;
    }

    if ( vnum == 0 || (pObjIndex = get_obj_index( vnum )) == NULL)
    {
	send_to_char("Missing object, please inform a god.\n\r",ch);
	return;
    }

    ch->practice -= cost;
    obj = create_object(pObjIndex, 50);
    free_string(obj->questmaker);
    obj->questmaker = str_dup(ch->name);
    obj_to_char(obj, ch);
    act("$p appears in your hands in a blast of moon light.",ch,obj,NULL,TO_CHAR);
    act("$p appears in $n's hands in a blast of moon light.",ch,obj,NULL,TO_ROOM);
    return;
}

void do_bridge (CHAR_DATA *ch, char *argument)
{
    char arg[MAX_STRING_LENGTH];

    CHAR_DATA *victim;
    OBJ_DATA *obj1;
    OBJ_DATA *obj2;

    if (IS_NPC(ch)) return;

    if (!IS_CLASS(ch, CLASS_WEREWOLF))
    {
	send_to_char("Huh?\r\n", ch);
	return;
    }

    if (ch->pcdata->powers[WPOWER_STAG] < 3)
    {
	send_to_char("You must advance your Stag totem to open a moonbridge.\r\n", ch);
	return;
    }

    one_argument(argument, arg);

    if (arg[0] == '\0')
    {
	send_to_char("Open a moonbridge to whom?\r\n", ch);
	return;
    }

    if ( ( victim = get_char_world( ch, arg ) ) == NULL )
    {
	send_to_char( "They are nowhere to be found.\n\r", ch );
	return;
    }

    if ( ch == victim)
    {
	send_to_char("That would be a short bridge wouldn't it?\r\n", ch);
	return;
    }

    if (victim->in_room == NULL)
    {
	send_to_char( "The moonbridge fails to open.\n\r", ch );
	return;
    }


    if ( !IS_CLASS(victim, CLASS_WEREWOLF))
    {
	send_to_char("You can only open a moonbridge to another Garou.\r\n", ch);
	return;
    }

    if ( ( obj1 = get_obj_carry( ch, "moongem" ) ) == NULL )
    {
	send_to_char("A moongem is required to open a moonbridge.\r\n", ch);
	return;
    }

    if ( ( obj2 = get_obj_carry( victim, "moongem" ) ) == NULL )
    {
	send_to_char("That Garou has no moongem.\r\n", ch);
	return;
    }

    if ( IS_SET(ch->in_room->room_flags, ROOM_NO_RECALL)
    ||   IS_SET(victim->in_room->room_flags, ROOM_PRIVATE)
    ||   IS_SET(victim->in_room->room_flags, ROOM_SOLITARY)
    ||   IS_SET(victim->in_room->room_flags, ROOM_NO_RECALL)
    ||   victim->in_room->vnum == ch->in_room->vnum)
    {
        send_to_char( "The moonbridge fails to open.\n\r", ch );
        return;
    }

    if (victim->position == POS_FIGHTING)
    {
	send_to_char("The moonbridge fails to open.\r\n", ch);
	return;
    }

    if (number_range(1, 10) == 1)
    {
	send_to_char("Your moongem shines brightly then vanishes.\r\n", ch);
	extract_obj(obj1);
    }

    act("A moonbridge opens and swallows $n.", ch, NULL, NULL, TO_ROOM);
    act("A moonbridge opens above and swallows you.", ch, NULL, NULL, TO_CHAR);
    char_from_room(ch);
    char_to_room(ch, victim->in_room);
    act("$n steps out of a moonbridge.\r\n", ch, NULL, NULL, TO_ROOM);
    act("You step out of a moonbridge.\r\n", ch, NULL, NULL, TO_CHAR);
    do_look(ch, "auto");
    return;
}

int moon_phase (int lunar_day)
{
    switch (lunar_day)
    {
	case 1:  case 2:  case 26: case 27: case 28:
            return PHASE_NEW;
        case 3:  case 4:  case 5:  case 24: case 25:
            return PHASE_CRESCENT;
        case 6:  case 7:  case 8:  case 21: case 22: case 23:
            return PHASE_HALF;
        case 9:  case 10: case 11: case 18: case 19: case 20:
            return PHASE_GIBBOUS;
        case 12: case 13: case 14: case 15: case 16: case 17:
            return PHASE_FULL;
    }
    return PHASE_FULL;
}


