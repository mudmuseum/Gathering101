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
#include "player.h"

/*
 * Local functions.
 */
bool is_lord	args ( (CHAR_DATA *ch, char *argument) );

void do_inpart( CHAR_DATA *ch, char *argument )
{
    CHAR_DATA *victim;
    char      arg1 [MAX_INPUT_LENGTH];
    char      arg2 [MAX_INPUT_LENGTH];
    int       inpart = 0;
    int       cost = 0;

    smash_tilde(argument);
    argument = one_argument( argument, arg1 );
    argument = one_argument( argument, arg2 );

    if (IS_NPC(ch)) return;

    if (!IS_CLASS(ch, CLASS_DEMON))
    {
	send_to_char("Huh?\n\r",ch);
	return;
    }

    if ( arg1[0] == '\0' || arg2[0] == '\0' )
    {
	send_to_char( "Syntax: Inpart <person> <power>\n\r", ch );
	send_to_char( "Fangs (2500), Claws (2500), Horns (2500), Hooves (1500), Tail (7500)\r\n",ch);
	send_to_char( "Wings (1000), Might (7500), Toughness (7500), Speed (7500), Travel (1500),\n\r",ch);
	send_to_char( "Nightsight (3000), Shadowsight (7500), Scry (7500) , Move (500), Leap (500)\n\r",ch);
	send_to_char( "Magic (1000), Lifespan (100), Longsword (0), Shortsword (0), Reign (6000).\r\n",ch);
	if (ch->pcdata->stats[UNI_GEN] < 4)
	{
	    send_to_char("    Demonic leader powers:\r\n", ch);
	    send_to_char("Outcast (0)", ch);
	    if (ch->pcdata->stats[UNI_GEN] < 3)
	    	send_to_char(", Prince (0)", ch);
	    send_to_char(".\r\n", ch);
	}
	return;
    }

    if ( ( victim = get_char_world( ch, arg1 ) ) == NULL )
    {
	send_to_char( "Nobody by that name.\n\r", ch );
	return;
    }

    if ( IS_NPC(victim) )
    {
	send_to_char( "Not on NPC's.\n\r", ch );
	return;
    }
    if ( !IS_CLASS(victim, CLASS_DEMON) )
    {
	send_to_char("Only demons may be inparted gifts.\r\n", ch);
	return;
    }

    if (ch->pcdata->stats[UNI_GEN] > 3 && victim != ch)
    {
	send_to_char("You must be a lord or prince to inpart gifts to others.\r\n", ch);
	return;
    }
    if (str_cmp(ch->clan, victim->clan) && str_cmp(arg2, "induct"))
    {
	send_to_char( "They are not of your demonic kin.\n\r", ch );
	return;
    }

    if (!str_cmp(arg2,"induct"))
    {
	if (ch->pcdata->stats[UNI_GEN] > 3)
	{
	    send_to_char("Only high-ranking demons can induct rogue demons.\r\n", ch);
	    return;
	}
	if (strlen(victim->clan) > 2)
	{
	    send_to_char("You can only induct rogue demons.\r\n", ch);
	    return;
	}
	send_to_char("Pact rogue demons instead.\r\n", ch);
	return;
    }
    
    if (!str_cmp(arg2, "outcast"))
    {
	if (ch->pcdata->stats[UNI_GEN] > 3)
	{
	    send_to_char("Only high-ranking demons have that power.\r\n", ch);
	    return;
	}

	if (str_cmp(ch->clan, victim->clan) || ch->pcdata->stats[UNI_GEN] > victim->pcdata->stats[UNI_GEN])
	{
	    send_to_char("You have no power over them.\r\n", ch);
	    return;
	}

	act("You outcast $N.",ch,NULL,victim,TO_CHAR);
	act("$n has outcasted $N!",ch,NULL,victim,TO_NOTVICT);
	act("$n has outcasted $N!",ch,NULL,victim,TO_VICT);
	free_string(victim->clan);
	victim->clan = str_dup( "" );
	free_string(victim->lord);
	victim->lord = str_dup( "" );
	victim->pcdata->stats[UNI_GEN] = 4;
	victim->home = ROOM_VNUM_TEMPLE;
	return;
   }

    if (!str_cmp(arg2,"prince"))
    {
	if (victim == ch)
	{
	    send_to_char("Not on yourself!\n\r",ch);
	    return;
	}
	if (!IS_CLASS(ch, CLASS_DEMON))
	{
	    send_to_char("Only a demon lord has the power to make princes.\n\r",ch);
	    return;
	}
        if (ch->pcdata->stats[UNI_GEN] > 2)
        {
	    send_to_char("Only a demon lord has the power to make princes.\n\r",ch);
	    return;
        }
	if (victim->pcdata->stats[UNI_GEN] == 3)
	{
	    send_to_char("You have lost your princehood!\n\r",victim);
	    send_to_char("You remove their princehood.\n\r",ch);
	    victim->pcdata->stats[UNI_GEN] = 4;
	}
	else if (victim->pcdata->stats[UNI_GEN] == 4)
	{
	    send_to_char("You have been made a prince!\n\r",victim);
	    send_to_char("You make them a prince.\n\r",ch);
	    victim->pcdata->stats[UNI_GEN] = 3;
	    free_string( victim->lord );
	    victim->lord = str_dup( ch->name );
	}
	save_char_obj(victim);
	return;
    }

    if (!str_cmp(arg2,"longsword"))
    {
	send_to_char("You now have the power to transform into a demonic longsword!\n\r",victim);
	victim->pcdata->powers[DPOWER_OBJ_VNUM] = 29662;
	save_char_obj(victim);
	return;
    }

    if (!str_cmp(arg2,"shortsword"))
    {
	send_to_char("You now have the power to transform into a demonic shortsword!\n\r",victim);
	victim->pcdata->powers[DPOWER_OBJ_VNUM] = 29663;
	save_char_obj(victim);
	return;
    }

    if (!str_cmp(arg2,"fangs")) 
	{inpart = DEM_FANGS; cost = 2500;}
    else if (!str_cmp(arg2,"claws")) 
	{inpart = DEM_CLAWS; cost = 2500;}
    else if (!str_cmp(arg2,"horns")) 
	{inpart = DEM_HORNS; cost = 2500;}
    else if (!str_cmp(arg2,"hooves")) 
	{inpart = DEM_HOOVES; cost = 1500;}
    else if (!str_cmp(arg2,"nightsight")) 
	{inpart = DEM_EYES; cost = 3000;}
    else if (!str_cmp(arg2,"wings")) 
	{inpart = DEM_WINGS; cost = 1000;}
    else if (!str_cmp(arg2,"might")) 
	{inpart = DEM_MIGHT; cost = 7500;}
    else if (!str_cmp(arg2,"toughness")) 
	{inpart = DEM_TOUGH; cost = 7500;}
    else if (!str_cmp(arg2,"speed"))
	{inpart = DEM_SPEED; cost = 7500;}
    else if (!str_cmp(arg2,"travel")) 
	{inpart = DEM_TRAVEL; cost = 1500;}
    else if (!str_cmp(arg2,"scry")) 
	{inpart = DEM_SCRY; cost = 7500;}
    else if (!str_cmp(arg2,"shadowsight")) 
	{inpart = DEM_SHADOWSIGHT; cost = 3000;}
    else if (!str_cmp(arg2,"move")) 
	{inpart = DEM_MOVE; cost = 500;}
    else if (!str_cmp(arg2,"leap")) 
	{inpart = DEM_LEAP; cost = 500;}
    else if (!str_cmp(arg2,"magic")) 
	{inpart = DEM_MAGIC; cost = 1000;}
    else if (!str_cmp(arg2,"lifespan")) 
	{inpart = DEM_LIFESPAN; cost = 100;}
    else if (!str_cmp(arg2,"tail"))
        {inpart = DEM_TAIL; cost = 7500;}
    else if ( !str_cmp(arg2,"reign"))
	{inpart = DEM_REIGN; cost = 6000;}
    else
    {
	send_to_char("Please select a power from:\n\r",ch);
	send_to_char( "Fangs (2500), Claws (2500), Horns (2500), Hooves (1500), Tail (7500)\r\n",ch);
	send_to_char( "Wings (1000), Might (7500), Toughness (7500), Speed (7500), Travel (1500),\n\r",ch);
	send_to_char( "Nightsight (3000), Shadowsight (7500), Scry (7500) , Move (500), Leap (500)\n\r",ch);
	send_to_char( "Magic (1000), Lifespan (100), Longsword (0), Shortsword (0), Reign (6000).\r\n",ch);
	if (ch->pcdata->stats[UNI_GEN] < 4)
	{
	    send_to_char("    Demonic leader powers:\r\n", ch);
	    send_to_char("Outcast (0), Induct (1000)", ch);
	    if (ch->pcdata->stats[UNI_GEN] < 3)
	    	send_to_char(", Prince (0)", ch);
	    send_to_char(".\r\n", ch);
	}
	return;
    }
    if (IS_DEMPOWER(victim, inpart))
    {
	send_to_char("They have already got that power.\n\r",ch);
	return;
    }
    if (ch->pcdata->stats[DEMON_TOTAL] < cost || 
	ch->pcdata->stats[DEMON_CURRENT] < cost)
    {
	send_to_char("You have insufficient power to inpart that gift.\n\r",ch);
	return;
    }
    SET_BIT(victim->pcdata->powers[DPOWER_FLAGS], inpart);
    ch->pcdata->stats[DEMON_TOTAL]   -= cost;
    ch->pcdata->stats[DEMON_CURRENT] -= cost;
    if (victim != ch) 
	send_to_char("You have been granted a demonic gift from your patron!\n\r",victim);
    send_to_char("Ok.\n\r",ch);
    if (victim != ch) save_char_obj(ch);
    save_char_obj(victim);
    return;
}

void do_demonarmour( CHAR_DATA *ch, char *argument )
{
    OBJ_INDEX_DATA *pObjIndex;
    OBJ_DATA *obj;
    char arg1[MAX_INPUT_LENGTH];
    char arg2[MAX_INPUT_LENGTH];
    char buf[MAX_STRING_LENGTH];
    int vnum = 0;
    int dps_cost = 5000;
    sh_int primal_cost = 0;

    argument = one_argument( argument, arg1 );
    argument = one_argument( argument, arg2 );

    if (IS_NPC(ch)) return;

    if (!IS_CLASS(ch, CLASS_DEMON))
    {
        send_to_char("Huh?\n\r",ch);
        return;
}

    if (arg1[0] == '\0')
    {
        send_to_char("Please specify what color of demonic armor you wish to make: \r\nBlack, Jade, or Red.\r\n", ch);
        return;
    }

    if (!str_cmp(arg1, "black"))
    {
        if      (!str_cmp(arg2,"ring"      )) vnum = 29650;
        else if (!str_cmp(arg2,"collar"    )) vnum = 29651;
        else if (!str_cmp(arg2,"plate"     )) vnum = 29652;
        else if (!str_cmp(arg2,"helmet"    )) vnum = 29653;
        else if (!str_cmp(arg2,"leggings"  )) vnum = 29654;
        else if (!str_cmp(arg2,"boots"     )) vnum = 29655;
        else if (!str_cmp(arg2,"gauntlets" )) vnum = 29656;
	else if (!str_cmp(arg2,"sleeves"   )) vnum = 29657;
        else if (!str_cmp(arg2,"cape"      )) vnum = 29658;
        else if (!str_cmp(arg2,"belt"      )) vnum = 29659;
        else if (!str_cmp(arg2,"bracer"    )) vnum = 29660;
        else if (!str_cmp(arg2,"visor"     )) vnum = 29661;
        else if (!str_cmp(arg2,"shortsword")) vnum = 29697;
        else if (!str_cmp(arg2,"longsword" )) vnum = 29696;
        else
        {
            send_to_char("Please specify which piece of black demonic armor you wish to make:
Ring Collar Plate Helmet Leggings Boots Gauntlets Sleeves 
Cape Belt Bracer Visor Shortsword Longsword\n\r",ch);
            return;
        }
    }
    else if (!str_cmp(arg1, "jade"))
    {        if      (!str_cmp(arg2, "plate"    )) vnum = 29612;
        else if (!str_cmp(arg2, "helmet"        )) vnum = 29613;
        else if (!str_cmp(arg2, "leggings"      )) vnum = 29614;
        else if (!str_cmp(arg2, "boots"         )) vnum = 29615;
        else if (!str_cmp(arg2, "gauntlets"     )) vnum = 29616;
        else if (!str_cmp(arg2, "sleeves"       )) vnum = 29617;
        else if (!str_cmp(arg2, "cape"          )) vnum = 29618;
        else if (!str_cmp(arg2, "belt"          )) vnum = 29619;
        else if (!str_cmp(arg2, "visor"         )) vnum = 29620;
/*      else if (!str_cmp(arg2, "shortsword"    )) vnum = 29621;
        else if (!str_cmp(arg2, "longsword"     )) vnum = 29622; */
        else
        {
            send_to_char("Please specify which piece of jade demonic armor you wish to make:\r\nPlate Helmet Leggings Boots Gauntlets Sleeves Cape Belt Visor.\r\n", ch);
            return;
        }
        dps_cost = 20000;
        primal_cost = 25;
    }
    else if (!str_cmp(arg1, "red"))
    {
        if      (!str_cmp(arg2, "ring"          )) vnum = 29630;
        else if (!str_cmp(arg2, "collar"        )) vnum = 29631;
        else if (!str_cmp(arg2, "plate"         )) vnum = 29632;
        else if (!str_cmp(arg2, "helmet"        )) vnum = 29633;
        else if (!str_cmp(arg2, "leggings"      )) vnum = 29634;
        else if (!str_cmp(arg2, "boots"         )) vnum = 29635;
        else if (!str_cmp(arg2, "gauntlets"     )) vnum = 29636;
        else if (!str_cmp(arg2, "sleeves"       )) vnum = 29637;
        else if (!str_cmp(arg2, "cape"          )) vnum = 29638;
        else if (!str_cmp(arg2, "belt"          )) vnum = 29639;
        else if (!str_cmp(arg2, "bracer"        )) vnum = 29640;
        else if (!str_cmp(arg2, "visor"         )) vnum = 29641;
/*      else if (!str_cmp(arg2, "shortsword"    )) vnum = 29642;
	else if (!str_cmp(arg2, "longsword"     )) vnum = 29643; */
        else
        {
            send_to_char("Please specify which piece of red demonic armor you wish to make:
Ring Collar Plate Helmet Leggings Boots 
Gauntlets Sleeves Cape Belt Bracer Visor.\r\n", ch);
            return;
        }
        dps_cost = 25000;
        primal_cost = 50;
    }
    else
    {
        send_to_char("Please specify what color of demonic armor you wish to make: \r\nBlack, Jade, or Red.\r\n", ch);
        return;
    }

    if (ch->level < MAX_LEVEL)
    {
        if ( ch->pcdata->stats[DEMON_TOTAL] < dps_cost || ch->pcdata->stats[DEMON_CURRENT] < dps_cost)
        {
            sprintf(buf, "It costs %d points of power to create a piece of %s demonic armor.\n\r",dps_cost, arg1);
            send_to_char(buf, ch);
            return;
        }
        if (ch->practice < primal_cost)
        {
            sprintf(buf, "It costs %d primal energy to create a piece of %s demonic armor.\r\n", primal_cost, arg1);
            send_to_char(buf, ch);
            return;
        }
    }
    if ( vnum == 0 || (pObjIndex = get_obj_index( vnum )) == NULL)
    {
        send_to_char("Missing object, please inform an immortal.\n\r",ch);
        return;
    }
    if (ch->level < MAX_LEVEL)
    {
        ch->pcdata->stats[DEMON_TOTAL]   -= dps_cost;
        ch->pcdata->stats[DEMON_CURRENT] -= dps_cost;
        ch->practice -= primal_cost;
    }
    obj = create_object(pObjIndex, 50);
    obj_to_char(obj, ch);
    act("$p appears in your hands in a blast of flames.",ch,obj,NULL,TO_CHAR);
    act("$p appears in $n's hands in a blast of flames.",ch,obj,NULL,TO_ROOM);
    return;
}

void do_travel( CHAR_DATA *ch, char *argument )
{
    char      arg [MAX_INPUT_LENGTH];
    CHAR_DATA *victim;

    argument = one_argument( argument, arg );

    if (IS_NPC(ch)) return;

    if (IS_CLASS(ch, CLASS_DEMON))
    {
	if (!IS_DEMPOWER( ch, DEM_TRAVEL))
	{
	    send_to_char("You haven't been granted the gift of travel.\n\r",ch);
	    return;
	}
    }
    else
    {
	send_to_char("Huh?\n\r",ch);
	return;
    }

    if ( ( victim = get_char_world( ch, arg ) ) == NULL )
    {
	send_to_char( "Nobody by that name.\n\r", ch );
	return;
    }

    if ( IS_NPC(victim) )
    {
	send_to_char( "Not on NPC's.\n\r", ch );
	return;
    }

    if (!IS_CLASS(victim, CLASS_DEMON))
    {
	send_to_char( "You can only travel to one of your own demonic kin.\n\r", ch );
	return;
    }

    if ( str_cmp(ch->clan, victim->clan))
    {
	send_to_char( "You can only travel to one of your own demonic kin.\n\r", ch );
	return;
    }

    if (victim->in_room == NULL)
    {
	send_to_char( "Nothing happens.\n\r", ch );
	return;
    }

    if ( victim == ch )
    {
	send_to_char( "How can you travel to yourself?\n\r", ch );
	return;
    }

    if ( victim->position != POS_STANDING )
    {
	send_to_char( "You are unable to focus on their location.\n\r", ch );
	return;
    }

    if ( IS_SET(ch->in_room->room_flags, ROOM_NO_RECALL)
    ||   IS_SET(victim->in_room->room_flags, ROOM_PRIVATE)
    ||   IS_SET(victim->in_room->room_flags, ROOM_SOLITARY)
    ||   IS_SET(victim->in_room->room_flags, ROOM_NO_RECALL)
    ||   victim->in_room->vnum == ch->in_room->vnum)
    {
        send_to_char( "You failed.\n\r", ch );
        return;
    }

    send_to_char("You sink into the ground.\n\r",ch);
    act("$n sinks into the ground.",ch,NULL,NULL,TO_ROOM);
    char_from_room(ch);
    char_to_room(ch,victim->in_room);
    do_look(ch,"");
    send_to_char("You rise up out of the ground.\n\r",ch);
    act("$n rises up out of the ground.",ch,NULL,NULL,TO_ROOM);
    return;
}
void do_tail( CHAR_DATA *ch, char *argument )
{
    char      arg [MAX_INPUT_LENGTH];

    argument = one_argument( argument, arg );

    if (IS_NPC(ch)) return;

    if (!IS_CLASS(ch, CLASS_DEMON))
    {
	send_to_char("Huh?\n\r",ch);
	return;
    }
    if (!IS_DEMPOWER( ch, DEM_TAIL))
    {
	send_to_char("You haven't been granted the gift of tail.\n\r",ch);
	return;
    }

    if (IS_DEMAFF(ch,DEM_TAIL) )
    {
	send_to_char("Your tail shrinks back into your arse.\n\r",ch);
	act("$n's tail shrinks back into $s arse.", ch, NULL, NULL, TO_ROOM);
	REMOVE_BIT(ch->pcdata->powers[DPOWER_CURRENT], DEM_TAIL);
	return;
    }
    send_to_char("Your tail comes shooting out of your arse.\n\r",ch);
    act("A tail comes shooting out of $n's arse.", ch, NULL, NULL, TO_ROOM);
    SET_BIT(ch->pcdata->powers[DPOWER_CURRENT], DEM_TAIL);
    return;
}
void do_horns(CHAR_DATA *ch, char *argument )
{
    char      arg [MAX_INPUT_LENGTH];

    argument = one_argument( argument, arg );

    if (IS_NPC(ch)) return;

    if (!IS_CLASS(ch, CLASS_DEMON))
    {
	send_to_char("Huh?\n\r",ch);
	return;
    }
    if (!IS_DEMPOWER( ch, DEM_HORNS))
    {
	send_to_char("You haven't been granted the gift of horns.\n\r",ch);
	return;
    }

    if (IS_DEMAFF(ch,DEM_HORNS) )
    {
	send_to_char("Your horns slide back into your head.\n\r",ch);
	act("$n's horns slide back into $s head.", ch, NULL, NULL, TO_ROOM);
	REMOVE_BIT(ch->pcdata->powers[DPOWER_CURRENT], DEM_HORNS);
	return;
    }
    send_to_char("Your horns extend out of your head.\n\r",ch);
    act("A pair of pointed horns extend from $n's head.", ch, NULL, NULL, TO_ROOM);
    SET_BIT(ch->pcdata->powers[DPOWER_CURRENT], DEM_HORNS);
    return;
}

void do_hooves( CHAR_DATA *ch, char *argument )
{
    char      arg [MAX_INPUT_LENGTH];

    argument = one_argument( argument, arg );

    if (IS_NPC(ch)) return;

    if (!IS_CLASS(ch, CLASS_DEMON))
    {
	send_to_char("Huh?\n\r",ch);
	return;
    }
    if (!IS_DEMPOWER( ch, DEM_HOOVES))
    {
	send_to_char("You haven't been granted the gift of horns.\n\r",ch);
	return;
    }

    if (IS_DEMAFF(ch,DEM_HOOVES) )
    {
	send_to_char("Your hooves transform into feet.\n\r",ch);
	act("$n's hooves transform back into $s feet.", ch, NULL,NULL,TO_ROOM);
	REMOVE_BIT(ch->pcdata->powers[DPOWER_CURRENT], DEM_HOOVES);
	return;
    }
    send_to_char("Your feet transform into hooves.\n\r",ch);
    act("$n's feet transform into hooves.", ch, NULL, NULL, TO_ROOM);
    SET_BIT(ch->pcdata->powers[DPOWER_CURRENT], DEM_HOOVES);
    return;
}

void do_wings( CHAR_DATA *ch, char *argument )
{
    char      arg [MAX_INPUT_LENGTH];

    argument = one_argument( argument, arg );

    if (IS_NPC(ch)) return;

    if (!IS_CLASS(ch, CLASS_DEMON))
    {
	send_to_char("Huh?\n\r",ch);
	return;
    }
    if (!IS_DEMPOWER( ch, DEM_WINGS))
    {
	send_to_char("You haven't been granted the gift of wings.\n\r",ch);
	return;
    }

    if (arg[0] != '\0')
    {
	if (!IS_DEMAFF(ch,DEM_WINGS) )
	{
	    send_to_char("First you better get your wings out!\n\r",ch);
	    return;
	}
	if (!str_cmp(arg,"unfold") || !str_cmp(arg,"u"))
	{
	    if (IS_DEMAFF(ch,DEM_UNFOLDED) )
	    {
		send_to_char("But your wings are already unfolded!\n\r",ch);
		return;
	    }
	    send_to_char("Your wings unfold from behind your back.\n\r",ch);
	    act("$n's wings unfold from behind $s back.", ch, NULL, NULL, TO_ROOM);
	    SET_BIT(ch->pcdata->powers[DPOWER_CURRENT], DEM_UNFOLDED);
	    return;
	}
	else if (!str_cmp(arg,"fold") || !str_cmp(arg,"f"))
	{
	    if (!IS_DEMAFF(ch,DEM_UNFOLDED) )
	    {
		send_to_char("But your wings are already folded!\n\r",ch);
		return;
	    }
	    send_to_char("Your wings fold up behind your back.\n\r",ch);
	    act("$n's wings fold up behind $s back.", ch, NULL, NULL, TO_ROOM);
	    REMOVE_BIT(ch->pcdata->powers[DPOWER_CURRENT], DEM_UNFOLDED);
	    return;
	}
	else
	{
	    send_to_char("Do you want to FOLD or UNFOLD your wings?\n\r",ch);
	    return;
	}
    }

    if (IS_DEMAFF(ch,DEM_WINGS) )
    {
	if (IS_DEMAFF(ch,DEM_UNFOLDED) )
	{
	    send_to_char("Your wings fold up behind your back.\n\r",ch);
	    act("$n's wings fold up behind $s back.", ch, NULL, NULL, TO_ROOM);
	    REMOVE_BIT(ch->pcdata->powers[DPOWER_CURRENT], DEM_UNFOLDED);
	}
	send_to_char("Your wings slide into your back.\n\r",ch);
	act("$n's wings slide into $s back.", ch, NULL, NULL, TO_ROOM);
	REMOVE_BIT(ch->pcdata->powers[DPOWER_CURRENT], DEM_WINGS);
	return;
    }
    send_to_char("Your wings extend from your back.\n\r",ch);
    act("A pair of wings extend from $n's back.", ch, NULL, NULL, TO_ROOM);
    SET_BIT(ch->pcdata->powers[DPOWER_CURRENT], DEM_WINGS);
    return;
}

void do_lifespan( CHAR_DATA *ch, char *argument )
{
    char      arg [MAX_INPUT_LENGTH];
    OBJ_DATA *obj;

    argument = one_argument( argument, arg );

    if (IS_NPC(ch)) return;

    if (!IS_CLASS(ch,CLASS_DEMON))
    {
	send_to_char("Huh?\n\r",ch);
	return;
    }

    if (!IS_DEMPOWER( ch, DEM_LIFESPAN))
    {
	send_to_char("You haven't been granted the gift of lifespan.\n\r",ch);
	return;
    }

    if ( ( obj = ch->pcdata->chobj ) == NULL )
    {
	send_to_char("Huh?\n\r",ch);
	return;
    }

    if ( obj->chobj == NULL || obj->chobj != ch )
    {
	send_to_char("Huh?\n\r",ch);
	return;
    }

    if (!IS_HEAD(ch,LOST_HEAD))
    {
	send_to_char("You cannot change your lifespan in this form.\n\r",ch);
	return;
    }

         if (!str_cmp(arg,"l") || !str_cmp(arg,"long" )) obj->timer = 0;
    else if (!str_cmp(arg,"s") || !str_cmp(arg,"short")) obj->timer = 1;
    else
    {
	send_to_char("Do you wish to have a long or short lifespan?\n\r",ch);
	return;
    }
    send_to_char("Ok.\n\r",ch);

    return;
}

void do_pact( CHAR_DATA *ch, char *argument )
{
    CHAR_DATA *victim;
    char      arg [MAX_INPUT_LENGTH];
    char      buf [MAX_STRING_LENGTH];
    bool      can_sire = FALSE;

    argument = one_argument( argument, arg );

    if (IS_NPC(ch)) return;

    if (!IS_CLASS(ch, CLASS_DEMON))
    {
	send_to_char("Huh?\n\r",ch);
	return;
    }

    if (ch->pcdata->stats[UNI_GEN] <= 3) can_sire = TRUE;

    if (!can_sire)
    {
	send_to_char("You are not able to make a pact.\n\r",ch);
	return;
    }

    if (!str_cmp(ch->lord,"") && !IS_CLASS(ch, CLASS_DEMON))
    {
	send_to_char( "First you must find a demon lord.\n\r", ch );
	return;
    }

    if ( arg[0] == '\0' )
    {
	send_to_char( "Make a pact with whom?\n\r", ch );
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

    if ( ch == victim )
    {
	send_to_char( "You cannot make a pact with yourself.\n\r", ch );
	return;
    }

    if (IS_CLASS(victim, CLASS_MAGE))
    {
	send_to_char( "You cannot make a pact with a mage.\n\r", ch );
	return;
    }
    if (IS_CLASS(victim, CLASS_VAMPIRE))
    {
	send_to_char( "You cannot make a pact with a vampire.\n\r", ch );
	return;
    }
    if (IS_CLASS(victim, CLASS_WEREWOLF))
    {
	send_to_char( "You cannot make a pact with a werewolf.\n\r", ch );
	return;
    }

    if ( victim->level != LEVEL_AVATAR && !IS_IMMORTAL(victim) )
    {
	send_to_char( "You can only make pacts with avatars.\n\r", ch );
	return;
    }

    if (!IS_IMMUNE(victim,IMM_DEMON))
    {
	send_to_char( "You cannot make a pact with an unwilling person.\n\r", ch );
	return;
    }

    if (IS_CLASS(victim, CLASS_DEMON) && strlen(victim->clan) > 1)
    {
	send_to_char("They have no soul to make a pact with!\n\r",ch);
	return;
    }

    if ( IS_CLASS(victim, CLASS_HIGHLANDER) )
    {
	send_to_char( "You cannot make a pact with a highlander.\n\r", ch );
	return;
    }

    if (ch->exp < 666)
    {
	send_to_char("You cannot afford the 666 exp to make a pact.\n\r",ch);
	return;
    }

    ch->exp = ch->exp - 666;
    act("You make $N a demonic champion!", ch, NULL, victim, TO_CHAR);
    act("$n makes $N a demonic champion!", ch, NULL, victim, TO_NOTVICT);
    act("$n makes you a demonic champion!", ch, NULL, victim, TO_VICT);
    victim->class = CLASS_DEMON;
    victim->special = 0;
    victim->pcdata->stats[UNI_GEN] = 4;

    if (IS_CLASS(victim, CLASS_VAMPIRE)) do_mortalvamp(victim,"");
    REMOVE_BIT(victim->act, PLR_HOLYLIGHT);
    REMOVE_BIT(victim->act, PLR_WIZINVIS);
    REMOVE_BIT(victim->special, SPC_SIRE);
    REMOVE_BIT(victim->special, SPC_ANARCH);
    victim->pcdata->stats[UNI_RAGE] = 0;

    free_string(victim->morph);
    victim->morph=str_dup("");

    free_string(victim->clan);
    victim->clan=str_dup(ch->clan);

    free_string(victim->lord);
    
    if (ch->pcdata->stats[UNI_GEN] < 3)
	sprintf(buf, "%s", ch->name);
    else
        sprintf(buf,"%s %s",ch->lord,ch->name);

    victim->lord=str_dup(buf);

    save_char_obj(ch);
    save_char_obj(victim);
    return;
}

void do_offersoul( CHAR_DATA *ch, char *argument )
{
    char      arg [MAX_INPUT_LENGTH];
    argument = one_argument( argument, arg );

    if (IS_NPC(ch)) return;

    if (!IS_IMMUNE(ch, IMM_DEMON))
    {
/*
	send_to_char("That would be a very bad idea...\n\r",ch);
	return;
*/
	send_to_char("You will now allow demons to buy your soul.\n\r",ch);
	SET_BIT(ch->immune, IMM_DEMON);
	return;
  
    }
    send_to_char("You will no longer allow demons to buy your soul.\n\r",ch);
    REMOVE_BIT(ch->immune, IMM_DEMON);
    return;
}

void do_weaponform( CHAR_DATA *ch, char *argument )
{
    OBJ_DATA *obj;
    if (IS_NPC(ch)) return;
    if (!IS_CLASS(ch, CLASS_DEMON))
    {
	send_to_char( "Huh?\n\r", ch);
	return;
    }
    else if (IS_AFFECTED(ch,AFF_POLYMORPH))
    {
	send_to_char( "You cannot do this while polymorphed.\n\r", ch);
	return;
    }
    if (ch->pcdata->powers[DPOWER_OBJ_VNUM] < 1)
    {
	send_to_char( "You don't have the ability to change into a weapon.\n\r", ch);
	return;
    }
    if ((obj = create_object(get_obj_index(ch->pcdata->powers[DPOWER_OBJ_VNUM]),60)) == NULL)
    {
	send_to_char( "You don't have the ability to change into a weapon.\n\r", ch);
	return;
    }
    if (IS_AFFECTED(ch, AFF_CURSE)) 
    {
	send_to_char( "Something prevents you from assuming a weapon form!\n\r", ch);
	return;
    }
    if (IS_AFFECTED(ch,AFF_WEBBED))
    {
	send_to_char( "Not with all this sticky webbing on.\n\r", ch);
	return;
    }
    obj_to_room(obj,ch->in_room);
    act("$n transforms into $p and falls to the ground.",ch,obj,NULL,TO_ROOM);
    act("You transform into $p and fall to the ground.",ch,obj,NULL,TO_CHAR);
    ch->pcdata->obj_vnum = ch->pcdata->powers[DPOWER_OBJ_VNUM];
    obj->chobj = ch;
    ch->pcdata->chobj = obj;
    SET_BIT(ch->affected_by, AFF_POLYMORPH);
    SET_BIT(ch->extra, EXTRA_OSWITCH);
    free_string(ch->morph);
    ch->morph = str_dup(obj->short_descr);
    return;
}

void do_humanform( CHAR_DATA *ch, char *argument )
{
    OBJ_DATA *obj;

    if (IS_NPC(ch)) return;
    if ( ( obj = ch->pcdata->chobj ) == NULL )
    {
	send_to_char("You are already in human form.\n\r",ch);
	return;
    }

    ch->pcdata->obj_vnum = 0;
    obj->chobj = NULL;
    ch->pcdata->chobj = NULL;
    REMOVE_BIT(ch->affected_by, AFF_POLYMORPH);
    REMOVE_BIT(ch->extra, EXTRA_OSWITCH);
    free_string(ch->morph);
    ch->morph = str_dup("");
    act("$p transforms into $n.",ch,obj,NULL,TO_ROOM);
    act("Your reform your human body.",ch,obj,NULL,TO_CHAR);
    extract_obj(obj);
    if (ch->in_room->vnum == ROOM_VNUM_IN_OBJECT)
    {
	char_from_room(ch);
	char_to_room(ch,get_room_index(ROOM_VNUM_HELL));
    }
    return;
}

void do_champions( CHAR_DATA *ch, char *argument )
{
    char buf[MAX_STRING_LENGTH];
    char arg[MAX_INPUT_LENGTH];
    char lord[MAX_INPUT_LENGTH];
    CHAR_DATA *gch;

    one_argument( argument, arg );

    if (IS_NPC(ch)) return;
    if (!IS_CLASS(ch, CLASS_DEMON))
    {
	send_to_char("Huh?\n\r",ch);
	return;
    }

    if (strlen(ch->lord) < 2 && !IS_CLASS(ch, CLASS_DEMON) )
    {
	send_to_char("But you don't follow any demon!\n\r",ch);
	return;
    }

    if (IS_CLASS(ch, CLASS_DEMON)) strcpy(lord,ch->name);
	else strcpy(lord,ch->lord);
    sprintf( buf, "The champions of %s:\n\r", lord );
    send_to_char( buf, ch );
    send_to_char("[      Name      ] [ Hits ] [ Mana ] [ Move ] [  Exp  ] [       Power        ]\n\r", ch );
    for ( gch = char_list; gch != NULL; gch = gch->next )
    {
	if ( IS_NPC(gch) ) continue;
	if ( !IS_CLASS(gch, CLASS_DEMON) ) 
	    continue;
	if ( !str_cmp(ch->lord,lord) || !str_cmp(ch->name,lord))
	{
	    sprintf( buf,
	    "[%-16s] [%-6d] [%-6d] [%-6d] [%7d] [ %-9d%9d ]\n\r",
		capitalize( gch->name ),
		gch->hit,gch->mana,gch->move,
		gch->exp, gch->pcdata->stats[DEMON_CURRENT], gch->pcdata->stats[DEMON_TOTAL]);
		send_to_char( buf, ch );
	}
    }
    return;
}

void do_eyespy( CHAR_DATA *ch, char *argument )
{
    CHAR_DATA *victim;
    CHAR_DATA *familiar;

    if (IS_HEAD(ch,LOST_EYE_L) && IS_HEAD(ch,LOST_EYE_R))
    {
	send_to_char( "But you don't have any more eyes to pluck out!\n\r", ch );
	return;
    }
    if (!IS_HEAD(ch,LOST_EYE_L) && number_range(1,2) == 1)
    {
	act( "You pluck out your left eyeball and throw it to the ground.", ch, NULL, NULL, TO_CHAR );
	act( "$n plucks out $s left eyeball and throws it to the ground.", ch, NULL, NULL, TO_ROOM );
    }
    else if (!IS_HEAD(ch,LOST_EYE_R))
    {
	act( "You pluck out your right eyeball and throw it to the ground.", ch, NULL, NULL, TO_CHAR );
	act( "$n plucks out $s right eyeball and throws it to the ground.", ch, NULL, NULL, TO_ROOM );
    }
    else
    {
	act( "You pluck out your left eyeball and throw it to the ground.", ch, NULL, NULL, TO_CHAR );
	act( "$n plucks out $s left eyeball and throws it to the ground.", ch, NULL, NULL, TO_ROOM );
    }
    if ( ( familiar = ch->pcdata->familiar ) != NULL ) 
    {
	make_part(ch,"eyeball");
	return;
    }

    victim = create_mobile( get_mob_index( MOB_VNUM_EYE ) );
    if (victim == NULL)
    {send_to_char("Error - please inform KaVir.\n\r",ch); return;}

    char_to_room( victim, ch->in_room );

    ch->pcdata->familiar = victim;
    victim->wizard = ch;
    return;
}

bool is_lord( CHAR_DATA *ch, char *argument )
{
    char arg1[MAX_STRING_LENGTH];
    char arg2[MAX_STRING_LENGTH];
    char arg3[MAX_STRING_LENGTH];

    argument = one_argument(arg1, argument);
    argument = one_argument(arg2, argument);
    argument = one_argument(arg3, argument);

    if (!str_cmp(ch->name, arg1)) 
	return TRUE;
    else if (!str_cmp(ch->name, arg2)) 
	return TRUE;
    else if (!str_cmp(ch->name, arg3)) 
	return TRUE;
    else
	return FALSE;
}
