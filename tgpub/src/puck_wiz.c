/***************************************************************************
 *  Original Diku Mud copyright (C) 1990, 1991 by Sebastian Hammer,	   *
 *  Michael Seifert, Hans Henrik St{rfeldt, Tom Madsen, and Katja Nyboe.   *
 *									   *
 *  Merc Diku Mud improvments copyright (C) 1992, 1993 by Michael	   *
 *  Chastain, Michael Quan, and Mitchell Tse.				   *
 *									   *
 *  In order to use any part of this Merc Diku Mud, you must comply with   *
 *  both the original Diku license in 'license.doc' as well the Merc	   *
 *  license in 'license.txt'.  In particular, you may not remove either of *
 *  these copyright notices.						   *
 *									   *
 *  Much time and thought has gone into this software and you are	   *
 *  benefitting.  We hope that you share your changes too.  What goes	   *
 *  around, comes around.						   *
 ***************************************************************************/

#if defined(macintosh)
#include <types.h>
#else
#include <sys/types.h>
#endif
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>


#include "merc.h"


const	sh_int	reverse_dir		[]		=
{
    2, 3, 0, 1, 5, 4
};


void do_set (CHAR_DATA *ch, char *argument)
{
    char buf [MAX_STRING_LENGTH];
    char arg1[MAX_STRING_LENGTH];
    char arg2[MAX_STRING_LENGTH];
    CHAR_DATA *victim;
    int newgen = 4;

    argument = one_argument(argument, arg1);
    argument = one_argument(argument, arg2);

    if (arg1[0] == '\0')
    {
	send_to_char("Syntax: set <victim> <field> <value>.\r\n", ch);
	send_to_char("Valid fields: gen, clan, lord.\r\n", ch);
	return;
    }

    if (( victim = get_char_world(ch, arg1)) == NULL)
    {
	send_to_char("They aren't here.\r\n", ch);
        return;
    }
    if (IS_NPC(victim))
    {
        send_to_char("Not on an NPC.\r\n", ch);
        return;
    }
    if (arg2[0] == '\0')
    {
	send_to_char("Set their what?\r\n", ch);
	return;
    }

    if (!str_cmp(arg2, "gen"))
    {
	if ( IS_CLASS(victim, CLASS_MAGE) )
	{
            send_to_char("Mages don't use gens.\r\n", ch);
            return;
    	}
        if ( IS_CLASS(victim, CLASS_HIGHLANDER) )
        {
	    send_to_char("Highlanders don't use gens.\r\n", ch);
            return;
        }

    	if ( !is_number(argument))
    	{
            send_to_char("Gen must be a numeric value, 0 through 10.\r\n", ch);
	    return;
    	}
    	newgen = atoi(argument);
    	sprintf(buf, "%s gen set from %d to %d by %s.", victim->name,
            victim->pcdata->stats[UNI_GEN], newgen, ch->name);
    	    victim->pcdata->stats[UNI_GEN] = newgen;
	log_string( buf, LOG_SET );
	send_to_char("Ok.\r\n", ch);
	return;
    }
    else if (!str_cmp(arg2, "clan"))
    {
    	if ( strlen(argument) > 20 )
    	{
            send_to_char("Clan names cannot be that long.\r\n", ch);
            return;
    	}

    	else if ( IS_CLASS(victim, CLASS_MAGE) )
    	{
	    if ( !str_cmp(argument, "purple") )
            	victim->pcdata->powers[MPOWER_RUNE0] = 0;
            else if ( !str_cmp(argument, "red") )
            	victim->pcdata->powers[MPOWER_RUNE0] = 1;
            else if ( !str_cmp(argument, "blue") )
                victim->pcdata->powers[MPOWER_RUNE0] = 2;
            else if ( !str_cmp(argument, "green") )
                victim->pcdata->powers[MPOWER_RUNE0] = 3;
            else if ( !str_cmp(argument, "yellow") )
            	victim->pcdata->powers[MPOWER_RUNE0] = 4;
            else
            {
            	send_to_char("Mage clans can be purple, red, blue, green, or yellow.\r\n", ch);
            	return;
	    }
        }
	else
	{
	    free_string(victim->clan);
	    victim->clan = strdup(argument);
	    send_to_char("Ok.\r\n", ch);
	    return;
	}
    }
    else if (!str_cmp(arg2, "lord"))
    {
	free_string(victim->lord);
	victim->lord = strdup(argument);
	send_to_char("Ok.\r\n", ch);
	return;
    }
    else
    {
	send_to_char("Syntax: set <victim> <field> <value>.\r\n", ch);
	send_to_char("Valid fields: gen, clan, lord.\r\n", ch);
	return;
    }
}

/*
 * Displays class-specific powers (totems/discs/etc), immunities,
 * stance/weapon/spell levels, and timer data.
 */
void do_pstat(CHAR_DATA *ch, char *argument)
{
    char arg[MAX_INPUT_LENGTH];
    char buf[MAX_STRING_LENGTH];
    CHAR_DATA *victim;

    one_argument(argument, arg);

    if (arg[0] == '\0')
    {
        send_to_char("Pstat who?\r\n", ch);
        return;
    }

    if (( victim = get_char_world(ch, arg)) == NULL)
    {
        send_to_char("They aren't here.\r\n", ch);
        return;
    }

    if (IS_NPC(victim))
    {
        send_to_char("Not on an NPC.\r\n", ch);
        return;
    }

    if ( get_trust(ch) == LEVEL_HELPER && victim->mkill > NEWBIE_KILLS )
    {
        send_to_char( "You really consider them a newbie?\r\n", ch);
        return;
    }

    sprintf(buf, "#WName: #Y%s#n    #WMorphname: #y%s#n\r\n", 
	victim->name, IS_AFFECTED(victim, AFF_POLYMORPH)? victim->morph: "(None)");
    send_to_char(buf, ch);
    sprintf(buf,   
"#WWeapon level:#n                 Bite  : %4d Pound : %4d Crush: %4d\r\n", 
	victim->wpn[10],victim->wpn[7],victim->wpn[8]);
    send_to_char(buf, ch);
    sprintf(buf, 
" Unarmed : %4d Slash  : %4d Slice : %4d Whip  : %4d Grep : %4d\r\n",
	victim->wpn[0],victim->wpn[3],victim->wpn[1], victim->wpn[4],
	victim->wpn[9]);
    send_to_char(buf, ch);
	sprintf(buf,
" Claw    : %4d Pierce : %4d Stab  : %4d Blast : %4d Suck : %4d\r\n",
	victim->wpn[5],victim->wpn[11],victim->wpn[2],
	victim->wpn[6],victim->wpn[12]);
    send_to_char(buf, ch);
	sprintf(buf,
"#WStances:#n
 Viper : %3d Mongoose: %3d Crab  : %3d Bull   : %3d  Crane :  %3d\r\n",
	victim->stance[1],victim->stance[4],victim->stance[3],
	victim->stance[5],victim->stance[2]);
    send_to_char(buf, ch);
    sprintf(buf,
" Mantis: %3d Tiger   : %3d Dragon: %3d Swallow: %3d  Monkey: %3d\r\n", 
	victim->stance[6],victim->stance[8],victim->stance[7],
	victim->stance[10],victim->stance[9]);
    send_to_char(buf, ch);
    sprintf( buf,
"#WSpells:#n
 Purple: %3d Red   : %3d Blue  : %3d Green : %3d  Yellow: %3d\r\n",
	victim->spl[0], victim->spl[1], victim->spl[2], victim->spl[3],
	victim->spl[4]);
    send_to_char(buf, ch);
    sprintf(buf, 
"
#WTimer data:#n
 Idle: %4d  Last Death: %4d  Outcast: %4d  Silenced: %4d\r\n",
	victim->timer, victim->pcdata->timer[TIMER_DEATH],
	victim->pcdata->timer[TIMER_OUTCAST],
	victim->pcdata->timer[TIMER_SILENCED]);
    send_to_char(buf, ch);

    send_to_char("\r\n", ch);
    sprintf(buf, "Home: %d   Paradox: %d\r\n", victim->home, victim->paradox[1]);
    send_to_char(buf, ch);
    switch (victim->class)
    {
	case CLASS_DEMON:
	    send_to_char("#WDemonic inparts:#n\r\n", ch);
	    break;
	case CLASS_WEREWOLF:
	    send_to_char("#WTotems:#n\r\n", ch);
	    break;
	case CLASS_VAMPIRE: 
	    send_to_char("#WDisciplines:#n\r\n", ch);
	    break;
	case CLASS_HIGHLANDER:
	    send_to_char("#WHighlander:#n\r\n", ch);
	    break;
    }

/* Inserted by Xodin 6/20 10:23 CST 
 * Only Demon and Werewolf so far
 * two hardest ones i think.  Already
 * tested on Test mud--full approval
 */

    if ( IS_CLASS( victim, CLASS_DEMON ) )
    {
    	if ( IS_DEMPOWER(victim,DEM_FANGS) )
	    send_to_char("Fangs (X), ",ch);
      else
          send_to_char("Fangs ( ), ",ch);

    	if ( IS_DEMPOWER(victim,DEM_CLAWS) )
	    send_to_char("Claws (X), ",ch);
      else
          send_to_char("Claws ( ), ",ch);

    	if ( IS_DEMPOWER(victim,DEM_HORNS) )
	    send_to_char("Horns (X), ",ch);
      else
          send_to_char("Horns ( ), ",ch);

    	if ( IS_DEMPOWER(victim,DEM_TAIL) )
	    send_to_char("Tail (X)\n\r",ch);
      else
          send_to_char("Tail ( )\n\r",ch);

    	if ( IS_DEMPOWER(victim,DEM_HOOVES) )
	    send_to_char("Hooves (X), ",ch);
      else
          send_to_char("Hooves ( ), ",ch);

    	if ( IS_DEMPOWER(victim,DEM_EYES) )
	    send_to_char("Nightsight (X), ",ch);
      else
          send_to_char("Nightsight ( ), ",ch);

    	if ( IS_DEMPOWER(victim,DEM_WINGS) )
	    send_to_char("Wings (X), ",ch);
      else
          send_to_char("Wings ( ), ",ch);

    	if ( IS_DEMPOWER(victim,DEM_MIGHT) )
	    send_to_char("Might (X)\n\r",ch);
      else
          send_to_char("Might ( )\n\r",ch);

    	if ( IS_DEMPOWER(victim,DEM_TOUGH) )
	    send_to_char("Toughness (X), ",ch);
      else
          send_to_char("Toughness ( ), ",ch);

    	if ( IS_DEMPOWER(victim,DEM_SPEED) )
	    send_to_char("Speed (X), ",ch);
      else
          send_to_char("Speed ( ), ",ch);

    	if ( IS_DEMPOWER(victim,DEM_TRAVEL) )
	    send_to_char("Travel (X), ",ch);
      else
          send_to_char("Travel ( ), ",ch);

    	if ( IS_DEMPOWER(victim,DEM_SCRY) )
	    send_to_char("Scry (X)\n\r",ch);
      else
          send_to_char("Scry ( )\r\n ",ch);

    	if ( IS_DEMPOWER(victim,DEM_SHADOWSIGHT) )
	    send_to_char("Shadowsight (X), ",ch);
      else
          send_to_char("Shadowsight ( ), ",ch);
	if ( IS_DEMPOWER(victim, DEM_REIGN) )
	   send_to_char("Reign (X).\r\n", ch);
	else 
	    send_to_char("Reign ( ).\r\n", ch);

    }


   else if (IS_CLASS(victim, CLASS_WEREWOLF))
    {
 	sprintf(buf,"Totems: Bear (%d), Lynx (%d), Boar (%d), Owl (%d), Spider (%d), Wolf (%d),\n\r        Hawk (%d), Mantis (%d), Stag (%d).\n\r", 	
          victim->pcdata->powers[WPOWER_BEAR], 
          victim->pcdata->powers[WPOWER_LYNX], 	
          victim->pcdata->powers[WPOWER_BOAR], 
          victim->pcdata->powers[WPOWER_OWL], 	
          victim->pcdata->powers[WPOWER_SPIDER], 
          victim->pcdata->powers[WPOWER_WOLF], 	
          victim->pcdata->powers[WPOWER_HAWK],
          victim->pcdata->powers[WPOWER_MANTIS], 	
          victim->pcdata->powers[WPOWER_STAG] );

 	send_to_char(buf,ch); 
    }

   else if (IS_CLASS(victim, CLASS_VAMPIRE))
   {
        if (IS_VAMPAFF(victim,VAM_PROTEAN) || IS_VAMPPASS(victim,VAM_PROTEAN))
            send_to_char(" Protean (X)\n\r",ch);
        else
            send_to_char(" Protean ( )\n\r",ch);
        if (IS_VAMPAFF(victim,VAM_CELERITY) || IS_VAMPPASS(victim,VAM_CELERITY))
            send_to_char(" Celerity (X)\n\r",ch);
        else
            send_to_char(" Celerity ( )\n\r",ch);
        if (IS_VAMPAFF(victim,VAM_FORTITUDE) || IS_VAMPPASS(victim,VAM_FORTITUDE))
            send_to_char(" Fortitude (X)\n\r",ch);
        else
            send_to_char(" Fortitude ( )\n\r",ch);
        if (IS_VAMPAFF(victim,VAM_POTENCE) || IS_VAMPPASS(victim,VAM_POTENCE))
            send_to_char(" Potence (X)\n\r",ch);
        else
            send_to_char(" Potence ( )\n\r",ch);
        if (IS_VAMPAFF(victim,VAM_OBFUSCATE) || IS_VAMPPASS(victim,VAM_OBFUSCATE))
            send_to_char(" Obfuscate (X)\n\r",ch);
        else
            send_to_char(" Obfuscate ( )\n\r",ch);
        if (IS_VAMPAFF(victim,VAM_OBTENEBRATION) || IS_VAMPPASS(victim,VAM_OBTENEBRATION))
            send_to_char(" Obtenebration (X)\n\r",ch);
        else
            send_to_char(" Obtenebration ( )\n\r",ch);
        if (IS_VAMPAFF(victim,VAM_SERPENTIS) || IS_VAMPPASS(victim,VAM_SERPENTIS))
            send_to_char(" Serpentis (X)\n\r",ch);
        else
            send_to_char(" Serpentis ( )\n\r",ch);
        if (IS_VAMPAFF(victim,VAM_AUSPEX) || IS_VAMPPASS(victim,VAM_AUSPEX))
            send_to_char(" Auspex (X)\n\r",ch);
        else
            send_to_char(" Auspex ( )\n\r",ch);
        if (IS_VAMPAFF(victim,VAM_DOMINATE) || IS_VAMPPASS(victim,VAM_DOMINATE))
            send_to_char(" Dominate (X)\n\r",ch);
        else
            send_to_char(" Dominate ( )\n\r",ch);
        if (IS_VAMPAFF(victim,VAM_PRESENCE) || IS_VAMPPASS(victim,VAM_PRESENCE))
            send_to_char(" Presence (X)\n\r",ch);
        else
            send_to_char(" Presence ( )\n\r",ch);
   }

   if ( IS_CLASS(victim, CLASS_MAGE) )
   {
	send_to_char("-=-=-=-= Runes Glyphs and Sigils =-=-=-=-\r\n", ch);
	send_to_char("Runes : ", ch);
        if (IS_SET(victim->pcdata->powers[MPOWER_LRUNES], RUNE_MASTER))
	    send_to_char("MASTERY ", ch); else send_to_char("mastery ", ch);
        if (IS_SET(victim->pcdata->powers[MPOWER_LRUNES], RUNE_SPIRIT))
	    send_to_char("SPIRIT ", ch); else send_to_char("spirit ", ch);
        if (IS_SET(victim->pcdata->powers[MPOWER_LRUNES], RUNE_MIND))
	    send_to_char("MIND ", ch); else send_to_char("mind ", ch);
        if (IS_SET(victim->pcdata->powers[MPOWER_LRUNES], RUNE_LIFE))
	    send_to_char("LIFE ", ch); else send_to_char("life ", ch);
        if (IS_SET(victim->pcdata->powers[MPOWER_LRUNES], RUNE_DEATH)) 
	    send_to_char("DEATH ", ch); else send_to_char("death ", ch);
	send_to_char("\r\n        ", ch);
        if (IS_SET(victim->pcdata->powers[MPOWER_LRUNES], RUNE_LIGHT))
	    send_to_char("LIGHT ", ch); else send_to_char("light ", ch);
        if (IS_SET(victim->pcdata->powers[MPOWER_LRUNES], RUNE_DARK))
	    send_to_char("DARK ", ch); else send_to_char("dark ", ch);
        if (IS_SET(victim->pcdata->powers[MPOWER_LRUNES], RUNE_EARTH))
	    send_to_char("EARTH ", ch); else send_to_char("earth ", ch);
        if (IS_SET(victim->pcdata->powers[MPOWER_LRUNES], RUNE_AIR))
	    send_to_char("AIR ", ch); else send_to_char("air ", ch);
        if (IS_SET(victim->pcdata->powers[MPOWER_LRUNES], RUNE_FIRE)) 
	    send_to_char("FIRE ", ch); else send_to_char("fire ", ch);
	if (IS_SET(victim->pcdata->powers[MPOWER_LRUNES], RUNE_WATER))
	    send_to_char("WATER ", ch); else send_to_char("water ", ch);
	send_to_char("\r\nGlyphs: ", ch);
	if (IS_SET(victim->pcdata->powers[MPOWER_LGLYPHS], GLYPH_PROTECTION))
	    send_to_char("PROTECTION ", ch); else send_to_char("protection ", ch);
	if (IS_SET(victim->pcdata->powers[MPOWER_LGLYPHS], GLYPH_REDUCTION))
	    send_to_char("REDUCTION ", ch); else send_to_char("reduction ", ch);
	if (IS_SET(victim->pcdata->powers[MPOWER_LGLYPHS], GLYPH_CONTROL))
	    send_to_char("CONTROL ", ch); else send_to_char("control ", ch);
	if (IS_SET(victim->pcdata->powers[MPOWER_LGLYPHS], GLYPH_DESTRUCTION))
	    send_to_char("DESTRUCTION ", ch); else send_to_char("destruction ", ch);
	send_to_char("\r\n        ", ch);
	if (IS_SET(victim->pcdata->powers[MPOWER_LGLYPHS], GLYPH_TRANSPORTATION))
	    send_to_char("TRANSPORTATION ", ch); else send_to_char("transportation ", ch);
	if (IS_SET(victim->pcdata->powers[MPOWER_LGLYPHS], GLYPH_TRANSFORMATION))
	    send_to_char("TRANSFORMATION ", ch); else send_to_char("transformation ", ch);
	if (IS_SET(victim->pcdata->powers[MPOWER_LGLYPHS], GLYPH_CREATION))
	    send_to_char("CREATION ", ch); else send_to_char("creation ", ch);
	send_to_char("\r\n        ", ch);
	if (IS_SET(victim->pcdata->powers[MPOWER_LGLYPHS], GLYPH_INFORMATION))
	    send_to_char("INFORMATION ", ch); else send_to_char("information ", ch);
	if (IS_SET(victim->pcdata->powers[MPOWER_LGLYPHS], GLYPH_SUMMONING))
	    send_to_char("SUMMONING ", ch); else send_to_char("summoning ", ch);
	if (IS_SET(victim->pcdata->powers[MPOWER_LGLYPHS], GLYPH_ENHANCEMENT))
	    send_to_char("ENHANCEMENT ", ch); else send_to_char("enhancement ", ch);
	send_to_char("\r\nSigils: ", ch);
	if (IS_SET(victim->pcdata->powers[MPOWER_LSIGILS], SIGIL_AREA))
	    send_to_char("AREA ", ch); else send_to_char("area ", ch);
	if (IS_SET(victim->pcdata->powers[MPOWER_LSIGILS], SIGIL_OBJECT))
	    send_to_char("OBJECT ", ch); else send_to_char("object ", ch);
	if (IS_SET(victim->pcdata->powers[MPOWER_LSIGILS], SIGIL_SELF))
	    send_to_char("SELF ", ch); else send_to_char("self ", ch);
	if (IS_SET(victim->pcdata->powers[MPOWER_LSIGILS], SIGIL_TARGETING))
	    send_to_char("TARGETING ", ch); else send_to_char("targeting ", ch);
	send_to_char("\r\n=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=\r\n", ch);
   }

   if ( IS_CLASS(victim, CLASS_HIGHLANDER) )
   {
      sprintf( buf, "Weapons Mastered: %d    Training Level: %d\n\r\n\r", victim->pcdata->powers[HPOWER_MASTERED], victim->pcdata->powers[HPOWER_TRAINED] );
      send_to_char( buf, ch );
	sprintf(buf, "Magical resistance: %d\r\n", char_save(ch) );
	send_to_char(buf, ch);
   }

/* End of Insert */
    return;
}


void tie ( CHAR_DATA *ch, int duration)
{
   AFFECT_DATA af;

   af.type = gsn_tied;
   af.duration = duration;
   af.location = APPLY_NONE;
   af.modifier = 0;
   af.bitvector = AFF_TIED;
   affect_join(ch, &af);

}






/* HQ EDIT -- Xodin */

/* This is a very long function I created
 * for the main interaction for the creation
 * to an HQ.  It is divided up into sections
 * of ROOMS/OBJECTS/PORTALS/EXITS...then is
 * later broken down into the features each
 * one has as an option.
 */

void do_hqedit( CHAR_DATA *ch, char *argument )
{
  char arg1[MAX_INPUT_LENGTH];
  char arg2[MAX_INPUT_LENGTH];
  char arg3[MAX_INPUT_LENGTH];
  char arg4[MAX_INPUT_LENGTH];
  char arg5[MAX_INPUT_LENGTH];
  char arg6[MAX_INPUT_LENGTH];
  int  tempnum;
  sh_int  exit_dir;
  argument = one_argument( argument, arg1 );
  argument = one_argument( argument, arg2 );

 /*
  * Make sure hqinfo is initialized and memory is assigned
  * just have to make sure memory isn't assigned twice
  * and if the target isn't a portal then initialize hqinfo
  * if is portal then we need to keep the hqinfo intact for
  * target use. -- Xodin
  */

    if (IS_NPC(ch))
	return;

 /* 
  * Check if CL or IMM status
  */
  if ( IS_CLASS( ch, CLASS_DEMON ) && IS_GEN( ch, 2 ) ) {}
  else if ( IS_CLASS( ch, CLASS_VAMPIRE ) && IS_GEN( ch, 2 ) ) {}
  else if ( IS_CLASS( ch, CLASS_WEREWOLF ) && IS_GEN( ch, 2 ) ) {}
  else if ( IS_CLASS( ch, CLASS_MAGE ) && ch->level == LEVEL_ARCHMAGE ) {}
  else if ( IS_JUDGE( ch ) ) {}
  else
  {
    send_to_char( "Huh?\n\r", ch );
    return;
  }


  if ( ch->pcdata->hqinfo == NULL )
  {
    ch->pcdata->hqinfo = alloc_perm( sizeof(*ch->pcdata->hqinfo) );
    ch->pcdata->hqinfo->obj_specpower = 0;
  }

  else if ( ch->pcdata->hqinfo->store_vnum == 0 && 
	ch->pcdata->hqinfo->obj_spectype == 0 )
  {
    ch->pcdata->hqinfo = (struct hq_data *) 0;
    ch->pcdata->hqinfo = alloc_perm( sizeof(*ch->pcdata->hqinfo) );
  }

 /*
  * Check to see if CL is in hq, unless they are continuing
  * a portal.  If immortal, bypass. -- Xodin
  */

  if ( !str_cmp( arg1, "clear") )
  {
	ch->pcdata->hqinfo->vnum 		= 0;
	ch->pcdata->hqinfo->store_vnum 		= 0;
	ch->pcdata->hqinfo->obj_specpower 	= 0;
	ch->pcdata->hqinfo->obj_spectype 	= 0;
	send_to_char("Clearing HQEDIT information.\r\n", ch);
	return;
  }

  if ( ch->pcdata->hqinfo->store_vnum != 0 && !str_cmp( arg1, "portal" ) ) {}
  else if ( ch->pcdata->hqinfo->store_vnum != 0 && !str_cmp( arg1, "object" ) ) {}
  else if ( char_can_hqedit(ch) ) {}
  else 
  {
    send_to_char( "You must be in your HQ to edit your HQ, unless you are designating the target of a portal.\n\r", ch );
    return;
  }

  if ( arg1[0] == '\0' )
  {
    send_to_char( "Syntax: hqedit <room/object/portal/exit/clear> <value> <value>...\n\r", ch );
    return;
  }

 /*
  * Interface
  */
  if ( !str_cmp( arg1, "room" ) )
  {
    if ( arg2[0] == '\0' )
    {
      send_to_char( "Syntax: hqedit room <create/name/description/flag>\n\r", ch );
      return;
    }

    if ( !str_cmp( arg2, "create" ) )
    {
      argument = one_argument( argument, arg3 );
      argument = one_argument( argument, arg4 );
      argument = one_argument( argument, arg5 );
      argument = one_argument( argument, arg6 );

      if ( arg3[0] == '\0' )
      {
        send_to_char( "Syntax: hqedit room create <n/s/e/w/u/d>\n\r", ch );
	return;
      }

      send_to_char( "Rooms cost 1500qp and 50 primal.\n\r", ch );

      if ( ch->pcdata->quest < 1500 )
      {
        send_to_char( "You don't have enough quest points to create a new room.\n\r", ch );
        return;
      }
      if ( ch->practice < 50 )
      {
        send_to_char( "You don't have enough primal to create a new room.\n\r", ch );
        return;
      }
      switch( arg3[0] )
      {
        default:
          send_to_char( "Syntax: hqedit room create <n/s/e/w/u/d>\n\r", ch );
          return;
          break;
        case 'n': exit_dir = DIR_NORTH;
          break;
        case 's': exit_dir = DIR_SOUTH;
          break;
        case 'e': exit_dir = DIR_EAST;
          break;
        case 'w': exit_dir = DIR_WEST;
          break;
        case 'u': exit_dir = DIR_UP;
          break;
        case 'd': exit_dir = DIR_DOWN;
          break;
      }
      if ( ch->in_room->exit[exit_dir] != NULL )
      {
        send_to_char( "Exit already in use!\n\r", ch );
        return;
      }

      send_to_char( "Generating room...\n\r", ch );
      generate_room( ch );
      send_to_char( "Done.\n\r", ch );

     switch (arg3[0])
     {
	default:
          send_to_char( "Syntax: hqedit room create <n/s/e/w/u/d>\n\r", ch );
          return;
          break;
        case 'n':
          ch->pcdata->hqinfo->vnum = reverse_dir[0];
          break;
        case 's':
          ch->pcdata->hqinfo->vnum = reverse_dir[2];
          break;
        case 'e':
          ch->pcdata->hqinfo->vnum = reverse_dir[1];
          break;
        case 'w':
          ch->pcdata->hqinfo->vnum = reverse_dir[3];
          break;
        case 'u':
          ch->pcdata->hqinfo->vnum = reverse_dir[4];
          break;
        case 'd':
          ch->pcdata->hqinfo->vnum = reverse_dir[5];
          break;
      }
      ch->pcdata->hqinfo->obj_specpower = ch->in_room->vnum;
      tempnum = free_hq_vnum( ch, "room" ) - 1;
      char_from_room( ch );
      char_to_room( ch, get_room_index( tempnum ) );

      send_to_char( "Creating exits...\n\r", ch );
      generate_exit( ch );
      ch->pcdata->hqinfo->obj_specpower = ch->in_room->vnum;
      tempnum = ch->in_room->exit[ch->pcdata->hqinfo->vnum]->to_room->vnum;
      char_from_room( ch );
      char_to_room( ch, get_room_index( tempnum ) );
      ch->pcdata->hqinfo->vnum = reverse_dir[ch->pcdata->hqinfo->vnum]; 
      generate_exit( ch );
      send_to_char( "Done.\n\r", ch );

      send_to_char( "Saving...\n\r", ch );
      save_new_creation( ch );
      send_to_char( "Purchases are final - NO refunds, so I hope you got it right.\n\r", ch );
      send_to_char( "Ok.\n\r", ch );
      ch->pcdata->quest -= 1500;
      ch->practice -= 50;
    }
  
    else if ( !str_cmp( arg2, "name" ) )
    {
      if ( argument[0] == '\0' )
      {
        send_to_char( "Syntax: hqedit room name <value ... ..>\n\r", ch );
        return;
      }

      if ( strlen( argument ) < 5 )
      {
        send_to_char( "Name must be at least 5 characters long.\n\r", ch );
        return;
      }

      send_to_char( "Room names cost 150qp and 25 primal.\n\r", ch );

      if ( ch->pcdata->quest < 150 )
      {
        send_to_char( "You don't have enough quest points to change a description.\n\r", ch );
        return;
      }
      if ( ch->practice < 25 )
      {
        send_to_char( "You don't have enough primal to change a description.\n\r", ch );
        return;
      }
      smash_colors( argument );

      send_to_char( "Editing room...\n\r", ch );
      free_string(ch->in_room->name);
      ch->in_room->name = str_dup( argument );
      send_to_char( "Done.\n\r", ch );

      send_to_char( "Saving...\n\r", ch );
      save_new_creation( ch );
      send_to_char( "Purchases are final - NO refunds, so I hope you got it right.\n\r", ch );
      send_to_char( "Ok.\n\r", ch );
      ch->pcdata->quest -= 150;
      ch->practice -= 25;
    }

    else if ( !str_cmp( arg2, "description" ) || !str_cmp( arg2, "desc") )
    {
      send_to_char( "Room descriptions cost 150qp and 25 primal.\n\r", ch );

      if ( ch->pcdata->quest < 150 )
      {
        send_to_char( "You don't have enough quest points to change a description.\n\r", ch );
        return;
      }
      if ( ch->practice < 25 )
      {
        send_to_char( "You don't have enough primal to change a description.\n\r", ch );
        return;
      }      
      send_to_char( "Enter room description ( type \'END\' when done ):\n\r", ch );
      send_to_char( "============================================================\n\r", ch );
      ch->desc->connected = CON_HQ_PART2;
      return;
    }

    else if ( !str_cmp( arg2, "flag" ) )
    {
      argument = one_argument( argument, arg3 );
      
      if ( arg3[0] == '\0' )
      {
        send_to_char( "Syntax: hqedit room flag <safe/dark/saveeq>\n\r", ch );
        return;
      }

      if ( !str_cmp( arg3, "safe" ) )
      {
        send_to_char( "The saferoom flag costs 250qp and 50 primal to toggle.\n\r", ch );

        if ( ch->pcdata->quest < 250 )
        {
          send_to_char( "You don't have enough quest points to toggle this flag.\n\r", ch );
          return;
        }
        if ( ch->practice < 50 )
        {
          send_to_char( "You don't have enough primal to toggle this flag.\n\r", ch );
          return;
        }
        send_to_char( "Editing room...\n\r", ch );
        if (IS_SET(ch->in_room->room_flags, ROOM_SAFE) )
	    REMOVE_BIT( ch->in_room->room_flags, ROOM_SAFE );
	else 
	    SET_BIT(ch->in_room->room_flags, ROOM_SAFE);
        send_to_char( "Saferoom flag toggled.\n\r", ch );

        send_to_char( "Saving...\n\r", ch );
        save_new_creation( ch );
        send_to_char( "Purchases are final - NO refunds, so I hope you got it right.\n\r", ch );
        send_to_char( "Ok.\n\r", ch );
        ch->pcdata->quest -= 250;
        ch->practice -= 50;
      }

      else if ( !str_cmp( arg3, "dark" ) )
      {
        send_to_char( "The darkness flag costs 200qp and 25 primal to toggle.\n\r", ch );

        if ( ch->pcdata->quest < 200 )
        {
          send_to_char( "You don't have enough quest points to toggle this flag.\n\r", ch );
          return;
        }
        if ( ch->practice < 25 )
        {
          send_to_char( "You don't have enough primal to toggle this flag.\n\r", ch );
          return;
        }
        send_to_char( "Editing room...\n\r", ch );
	if ( IS_SET( ch->in_room->room_flags, ROOM_DARK) )
	    REMOVE_BIT(ch->in_room->room_flags, ROOM_DARK);
	else 
            SET_BIT( ch->in_room->room_flags, ROOM_DARK );
        send_to_char( "Darkness flag toggled.\n\r", ch );
        send_to_char( "Saving...\n\r", ch );
        save_new_creation( ch );
        send_to_char( "Purchases are final - NO refunds, so I hope you got it right.\n\r", ch );
        send_to_char( "Ok.\n\r", ch );
        ch->pcdata->quest -= 200;
        ch->practice -= 25;
      }

      else if ( !str_cmp( arg3, "saveeq" ) )
      {
        send_to_char( "The save eq flag costs 2500qp and 50 primal.\n\r", ch );

        if ( ch->pcdata->quest < 2500 )
        {
          send_to_char( "You don't have enough quest points to add this flag.\n\r", ch );
          return;
        }
        if ( ch->practice < 50 )
        {
          send_to_char( "You don't have enough primal to add this flag.\n\r", ch );
          return;
        }
        send_to_char( "Editing room...\n\r", ch );
        SET_BIT( ch->in_room->room_flags, ROOM_SAVE_EQ );
        send_to_char( "Save eq flag added.\n\r", ch );
        send_to_char( "Saving...\n\r", ch );
        save_new_creation( ch );
        send_to_char( "Purchases are final - NO refunds, so I hope you got it right.\n\r", ch );
        send_to_char( "Ok.\n\r", ch );
        ch->pcdata->quest -= 2500;
        ch->practice -= 50;
      }
      else
      {
        send_to_char( "Syntax: hqedit room flag <safe/dark/saveeq>\n\r", ch );
      }

      return;
    } 	/* End of ROOM FLAG options */

    else
	send_to_char( "Syntax: hqedit room <create/name/description/flag> <value>\r\n", ch);

  }	/* End of ROOM options */

  

  else if ( !str_cmp( arg1, "object" ) )
  {
    OBJ_DATA *obj;

    if ( arg2[0] == '\0' )
    {
      send_to_char( "Syntax: hqedit object <create/longname/action/destination> <value>...\n\r", ch );
      return;
    }

    if ( !str_cmp( arg2, "create" ) )
    {
      send_to_char( "New objects cost 500qp and 50 primal.\n\r", ch );

      if ( ch->pcdata->quest < 500 )
      {
        send_to_char( "You don't have enough quest points to add this object.\n\r", ch );
        return;
      }
      if ( ch->practice < 50 )
      {
        send_to_char( "You don't have enough primal to add this object.\n\r", ch );
        return;
      }

      send_to_char( "Adding object...\n\r", ch );
      generate_object( ch );
      send_to_char( "Done.\n\r", ch );

      send_to_char( "Saving...\n\r", ch );
      save_new_creation( ch );
      send_to_char( "Purchases are final - NO refunds, so I hope you got it right.\n\r", ch );
      send_to_char( "Ok.\n\r", ch );
      ch->pcdata->quest -= 500;
      ch->practice -= 50;
    }

    else if ( !str_cmp( arg2, "longname" ) )
    {
      argument = one_argument( argument, arg3 );

      if ( ( obj = get_obj_list( ch, arg3, ch->in_room->contents ) ) == NULL )
      {
        send_to_char( "You can't find it.\n\r", ch );
        return;
      }

      if ( obj->pIndexData->area_from != ch->in_room->area )
      {
        send_to_char( "You can only edit items originating in your hq.\n\r", ch );
        return;
      }

      send_to_char( "Renaming an object costs 150qp and 25 primal.\n\r", ch );

      if ( ch->pcdata->quest < 150 )
      {
        send_to_char( "You don't have enough quest points to change this name.\n\r", ch );
        return;
      }
      if ( ch->practice < 25 )
      {
        send_to_char( "You don't have enough primal to change this name.\n\r", ch );
        return;
      }

      send_to_char( "Editing object...\n\r", ch );
      free_string(obj->pIndexData->description);
      obj->pIndexData->description = str_dup( argument );
      free_string(obj->description);
      obj->description = str_dup( argument );
      send_to_char( "Done.\n\r", ch );

      send_to_char( "Saving...\n\r", ch );
      save_new_creation( ch );
      send_to_char( "Purchases are final - NO refunds, so I hope you got it right.\n\r", ch );
      send_to_char( "Ok.\n\r", ch );
      ch->pcdata->quest -= 150;
      ch->practice -= 25;
      return; 
    }

    else if ( !str_cmp( arg2, "action" ) )
    {
      argument = one_argument( argument, arg3 );
      argument = one_argument( argument, arg4 );

      if ( arg3[0] == '\0' )
      {
        send_to_char( "Syntax: hqedit object action <activate/twist/press/pull> <objname>\n\r", ch );
        return;
      }

      if ( arg4[0] == '\0' )
      {
        send_to_char( "Syntax: hqedit object action <activate/twist/press/pull> <objname>\n\r", ch );
        return;
      }

      if ( ch->pcdata->hqinfo->obj_spectype != 0 )
      {
        send_to_char( "You have already created the action, you must then choose a destination.\n\r", ch);
        return;
      }

      if ( ( obj = get_obj_list( ch, arg4, ch->in_room->contents ) ) == NULL )
      {
        send_to_char( "You can't find it.\n\r", ch );
        return;
      }

      if ( obj->pIndexData->area_from != ch->in_room->area )
      {
        send_to_char( "You can only edit items originating in your hq.\n\r", ch );
        return;
      }

      send_to_char( "That costs 500qp and 50 primal.\n\r", ch );

      if ( ch->pcdata->quest < 500 )
      {
        send_to_char( "You don't have enough quest points to change this object.\n\r", ch );
        return;
      }
      if ( ch->practice < 50 )
      {
        send_to_char( "You don't have enough primal to change this object.\n\r", ch );
        return;
      }

      if ( !str_cmp( arg3, "activate" ) )
      {
        SET_BIT( ch->pcdata->hqinfo->obj_spectype, SITEM_ACTIVATE );
        SET_BIT( ch->pcdata->hqinfo->obj_spectype, SITEM_TELEPORTER );
      }
      else if ( !str_cmp( arg3, "twist" ) )
      {
        SET_BIT( ch->pcdata->hqinfo->obj_spectype, SITEM_TWIST );
        SET_BIT( ch->pcdata->hqinfo->obj_spectype, SITEM_TELEPORTER );
      }
      else if ( !str_cmp( arg3, "press" ) )
      {
        SET_BIT( ch->pcdata->hqinfo->obj_spectype, SITEM_PRESS );
        SET_BIT( ch->pcdata->hqinfo->obj_spectype, SITEM_TELEPORTER );
      }
      else if ( !str_cmp( arg3, "pull" ) )
      {
        SET_BIT( ch->pcdata->hqinfo->obj_spectype, SITEM_PULL );
        SET_BIT( ch->pcdata->hqinfo->obj_spectype, SITEM_TELEPORTER );
      }
      else
      {
        send_to_char( "Syntax: hqedit object action <activate/twist/press/pull> <obj>\n\r", ch );
        return;
      }

      send_to_char( "Ok.\n\r", ch );
      ch->pcdata->hqinfo->store_vnum = ch->in_room->vnum;
      ch->pcdata->hqinfo->obj_name = str_dup( obj->name );
      send_to_char( "Now go to the target transporter place and type:\n\r", ch );
      send_to_char( "  hqedit object destination\n\r", ch );
      return;
    }
    else if ( !str_cmp( arg2, "destination" ) )
    {
      if ( ch->pcdata->hqinfo->obj_spectype == 0 && !str_cmp( arg2, "destination" ) )
      {
        send_to_char( "You can't set the destination yet, you must use the create option first.\n\r", ch );
        ch->pcdata->hqinfo->obj_specpower = 0;
        ch->pcdata->hqinfo->store_vnum = 0;
        ch->pcdata->hqinfo->obj_spectype = 0;
        return;
      }

      send_to_char( "That costs 500qp and 50 primal.\n\r", ch );

      if ( ch->pcdata->quest < 500 )
      {
        send_to_char( "You don't have enough quest points to change this object.\n\r", ch );
        return;
      }
      if ( ch->practice < 50 )
      {
        send_to_char( "You don't have enough primal to change this object.\n\r", ch );
        return;
      }
     
      if ( ch->in_room->area != 
		get_room_index(ch->pcdata->hqinfo->store_vnum)->area)
      {
        send_to_char( "You can only make an object's destination inside your HQ.\n\r", ch );
        return;
      }

      ch->pcdata->hqinfo->obj_specpower = ch->in_room->vnum;

      char_from_room( ch );
      char_to_room(ch, get_room_index( ch->pcdata->hqinfo->store_vnum ) );

      if ( ( obj = get_obj_list( ch, ch->pcdata->hqinfo->obj_name, ch->in_room->contents ) ) == NULL )
      {
        send_to_char( "Where did it go, George?\n\r", ch );
        return;
      }

      send_to_char( "Editing object...\n\r", ch );
      obj->pIndexData->spectype = obj->spectype = 
			ch->pcdata->hqinfo->obj_spectype;
      obj->pIndexData->specpower = obj->specpower = 
			ch->pcdata->hqinfo->obj_specpower;
      send_to_char( "Done.\n\r", ch );

      send_to_char( "Saving...\n\r", ch );
      save_new_creation( ch );
      send_to_char( "Purchases are final - NO refunds, so I hope you got it right.\n\r", ch );
      send_to_char( "Ok.\n\r", ch );
      ch->pcdata->quest -= 500;
      ch->practice -= 50;

      ch->pcdata->hqinfo->obj_specpower = 0;
      ch->pcdata->hqinfo->store_vnum = 0;
      ch->pcdata->hqinfo->obj_spectype = 0;
      return; 
    }
      
    else
    {
      send_to_char( "Syntax: hqedit object <create/longname/action/destination> <value>...\n\r", ch );
      return;
    }
  }	/* End of OBJECT options */


  else if ( !str_cmp( arg1, "portal" ) )
  {
    argument = one_argument( argument, arg3 );

    if ( arg2[0] == '\0' )
    {
      send_to_char( "Syntax: hqedit portal <create/destination> <value>\n\r", ch );
      return;
    }

    if ( !str_cmp( arg2, "create" ) && arg3[0] == '\0' )
    {
      send_to_char( "Syntax: hqedit portal create <n/s/e/w/u/d>\n\r", ch );
      return;
    }

    send_to_char( "That costs 5000qp and 100 primal.\n\r", ch );

    if ( ch->pcdata->quest < 5000 )
    {
      send_to_char( "You dont have enough quest points to add a portal.\n\r", ch );
      return;
    }

    if ( ch->practice < 100 )
    {
      send_to_char( "You dont have enough primal to add a portal.\n\r", ch );
      return;
    }

    if ( ch->pcdata->hqinfo->store_vnum != 0 && !str_cmp( arg2, "create" ) )
    {
      send_to_char( "You have already created it, you must choose a destination.\n\r", ch );
      return;
    }

    if ( ch->pcdata->hqinfo->store_vnum == 0 && !str_cmp( arg2, "destination" ) )
    {
      send_to_char( "You can't set the destination yet, you must choose a create first.\n\r", ch );
      return;
    }

    if ( !str_cmp( arg2, "create" ) )
    {
      switch ( arg3[0] )
      {
        case 'n':
          ch->pcdata->hqinfo->vnum = DIR_NORTH;
          break;
        case 's':
          ch->pcdata->hqinfo->vnum = DIR_SOUTH;
          break;
        case 'e':
          ch->pcdata->hqinfo->vnum = DIR_EAST;
          break;
        case 'w':
          ch->pcdata->hqinfo->vnum = DIR_WEST;
          break;
	case 'u':
	  ch->pcdata->hqinfo->vnum = DIR_UP;
	  break;
	case 'd':
	  ch->pcdata->hqinfo->vnum = DIR_DOWN;
	  break;
        default:
          send_to_char( "You didn't choose a <n/s/e/w/u/d> direction.\n\r", ch );
          return;
      }

      if ( ch->in_room->exit[ch->pcdata->hqinfo->vnum] != NULL )
      {
        send_to_char( "Exit already in use!\n\r", ch );
        return;
      }
      ch->pcdata->hqinfo->store_vnum = ch->in_room->vnum;
      send_to_char( "Ok. Origin set - go to destination and type hqedit portal destination.\n\r", ch );
      return;
    }
    else if ( !str_cmp( arg2, "destination" ) )
    {    
      if (!valid_portal_destination(ch->in_room))
      {
	send_to_char( "You can't create a portal to this room.\r\n", ch);
	return;
      }
      if ( ch->in_room->sector_type == SECT_HQ )
      {
        send_to_char( "You cannot make a portal to inside an hq, if its your own hq use hqedit exit.\n\r", ch );
        return;
      }

      ch->pcdata->hqinfo->obj_specpower = ch->in_room->vnum;
      char_from_room( ch );
      char_to_room( ch, get_room_index( ch->pcdata->hqinfo->store_vnum ) );
      send_to_char( "Adding portal...\n\r", ch );
      generate_exit( ch );
      send_to_char( "Done.\n\r", ch );
      send_to_char( "Saving...\n\r", ch );
      save_new_creation( ch );
      send_to_char( "Purchases are final - NO refunds, so I hope you got it right.\n\r", ch );
      send_to_char( "Ok.\n\r", ch );
      ch->pcdata->quest -= 5000;
      ch->practice -= 100;
      ch->pcdata->hqinfo->obj_specpower = 0;
      ch->pcdata->hqinfo->vnum = 0;
      ch->pcdata->hqinfo->store_vnum = 0;
      return;
    }
    else
    {
      send_to_char( "Syntax: hqedit portal <create/destination> <value>\n\r", ch );
      return;
    }  
  }	/* End of PORTAL options */


  else if ( !str_cmp( arg1, "exit" ) )
  {
	/* EXIT needs some work... */
    send_to_char("Not fully working.  Please try again later.\r\n", ch);
    return;

    argument = one_argument( argument, arg3 );

    if ( arg2[0] == '\0' )
    {
      send_to_char( "Syntax: hqedit exit <create/destination> <value>\n\r", ch );
      return;
    }

    if ( !str_cmp( arg2, "create" ) && arg3[0] == '\0' )
    {
      send_to_char( "Syntax: hqedit exit create <n/s/e/w/u/d>\n\r", ch );
      return;
    }

    send_to_char( "That costs 200qp and 25 primal.\n\r", ch );

    if ( ch->pcdata->quest < 200 )
    {
      send_to_char( "You dont have enough quest points to add a portal.\n\r", ch );
      return;
    }

    if ( ch->practice < 25 )
    {
      send_to_char( "You dont have enough primal to add a portal.\n\r", ch );
      return;
    }

    if ( ch->pcdata->hqinfo->store_vnum != 0 && !str_cmp( arg2, "create" ) )
    {
      send_to_char( "You have already created it, you must choose a destination.\n\r", ch );
      return;
    }

    if ( ch->pcdata->hqinfo->store_vnum == 0 && !str_cmp( arg2, "destination" ) )
    {
      send_to_char( "You can't set the destination yet, you must choose a create first.\n\r", ch );
      return;
    }

    if ( !str_cmp( arg2, "create" ) )
    {
      switch ( arg3[0] )
      {
        case 'n':
          ch->pcdata->hqinfo->vnum = DIR_NORTH;
          break;
        case 's':
          ch->pcdata->hqinfo->vnum = DIR_SOUTH;
          break;
        case 'e':
          ch->pcdata->hqinfo->vnum = DIR_EAST;
          break;
        case 'w':
          ch->pcdata->hqinfo->vnum = DIR_WEST;
          break;
        default:
          send_to_char( "You didn't choose a (n/s/e/w/u/d) direction.\n\r", ch );
          return;
      }

      if ( ch->in_room->exit[ch->pcdata->hqinfo->vnum] != NULL )
      {
        send_to_char( "Exit already in use!\n\r", ch );
        return;
      }
      ch->pcdata->hqinfo->store_vnum = ch->in_room->vnum;
      send_to_char(" Ok. Origin set - go to destination and type hqedit exit destination.\r\n", ch);
      return;
    }
    else if ( !str_cmp( arg2, "destination" ) )
    {    
      if ( !IS_SET( ch->in_room->sector_type, SECT_HQ ) || str_cmp( ch->in_room->area->name, ch->clan ) )
      {
        send_to_char( "The exit option is for exits inside your HQ.  Use the portal option.\n\r", ch );
        ch->pcdata->hqinfo->obj_specpower = 0;
        ch->pcdata->hqinfo->vnum = 0;
        return;
      }
      ch->pcdata->hqinfo->obj_specpower = ch->pcdata->hqinfo->store_vnum;
      ch->pcdata->hqinfo->vnum = reverse_dir[ch->pcdata->hqinfo->vnum];
      generate_exit( ch );
      ch->pcdata->hqinfo->obj_specpower = ch->in_room->vnum;
      char_from_room( ch );
      char_to_room( ch, get_room_index( ch->pcdata->hqinfo->store_vnum ) );
      ch->pcdata->hqinfo->vnum = reverse_dir[ch->pcdata->hqinfo->vnum];

      send_to_char( "Adding exit...\n\r", ch );
      generate_exit( ch );
      send_to_char( "Done.\n\r", ch );

      send_to_char( "Saving...\n\r", ch );
      save_new_creation( ch );
      send_to_char( "Purchases are final - NO refunds, so I hope you got it right.\n\r", ch );
      send_to_char( "Ok.\n\r", ch );
      ch->pcdata->quest -= 200;
      ch->practice -= 25;
      ch->pcdata->hqinfo->obj_specpower = 0;
      ch->pcdata->hqinfo->vnum = 0;
      ch->pcdata->hqinfo->store_vnum = 0;
      return;
    }
    else
    {
      send_to_char( "Syntax: hqedit exit <create/destination> <value>\n\r", ch );
      return;
    }    
  }	/* End of EXIT options */

  else
  {
    send_to_char( "Syntax: hqedit <room/object/portal/exit/clear> <value> <value>\n\r", ch );
    return;
  }
}


/*
 * Kinda an important command to accidently type
 * so might as well make it so they HAVE to want
 * to do that. -- Xodin
 */
void do_hqedi( CHAR_DATA *ch, char *argument )
{
  if ( IS_CLASS( ch, CLASS_MAGE ) && ch->level == 6 )
  {
    send_to_char( "If you want to edit your HQ spell it out HQEDIT.\n\r", ch );
    return;
  }

  if ( IS_CLASS( ch, CLASS_DEMON ) && IS_GEN(ch, 2) )
  {
    send_to_char( "If you want to edit your HQ spell it out HQEDIT.\n\r", ch );
    return;
  }

  if ( IS_CLASS( ch, CLASS_VAMPIRE ) && IS_GEN(ch, 2) )
  {
    send_to_char( "If you want to edit your HQ spell it out HQEDIT.\n\r", ch );
    return;
  }

  if ( IS_CLASS( ch, CLASS_WEREWOLF ) && IS_GEN(ch, 2) )
  {
    send_to_char( "If you want to edit your HQ spell it out HQEDIT.\n\r", ch );
    return;
  }

  send_to_char( "Huh?\n\r", ch );

}

/*
 * Easy way to find the next free vnum in an hq
 * also by using this...if you delete a room
 * and its vnum, it will use the deleted one
 * instead of a new one, so you will always
 * be able to have 50 rooms no matter what -- Xodin
 */
int free_hq_vnum( CHAR_DATA *ch, char *argument )
{
  ROOM_INDEX_DATA *room;
  MOB_INDEX_DATA *mob;
  OBJ_INDEX_DATA *obj;
  int first_vnum = 0;
  int last_vnum = 0;
  int iScan;
 
  for ( iScan = 1; iScan < 32000; iScan++ )
  {
    if ( !str_cmp( argument, "room" ) )
    {
      if ( ( room = get_room_index( iScan ) ) == NULL )
        continue;
      if (  room->area != ch->in_room->area )
        continue;
      if ( first_vnum == 0 )
        first_vnum = iScan;
      last_vnum = iScan;
      continue;
    }
    else if ( !str_cmp( argument, "object" ) )
    {
      if ( ( obj = get_obj_index( iScan ) ) == NULL )
        continue;
      if (  obj->area_from != ch->in_room->area )
        continue;
      if ( first_vnum == 0 )
        first_vnum = iScan;
      last_vnum = iScan;
      continue;
    }
    else if ( !str_cmp( argument, "mobile" ) )
    {
      if ( ( mob = get_mob_index( iScan ) ) == NULL )
        continue;
      if (  mob->area_from != ch->in_room->area )
        continue;
      if ( first_vnum == 0 )
        first_vnum = iScan;
      last_vnum = iScan;
      continue;
    }
    else return -1;
  }
  if ( !str_cmp( argument, "mobile" ) )
  {
    if ( ( get_mob_index( last_vnum + 1 ) ) != NULL )
    {
      bug( "Mob Vnums Full!!!", 0);
      send_to_char( "Mob Vnums Full -- Please inform Puck or Dalren.\n\r", ch );
      return -1;
    }
  }
  else if ( !str_cmp( argument, "object" ) )
  {
    if ( ( get_obj_index( last_vnum + 1 ) ) != NULL )
    {
      bug( "Object Vnums Full!!!", 0);
      send_to_char( "Object Vnums Full -- Please inform Puck or Dalren.\n\r", ch );
      return -1;
    }
  }
  else if ( !str_cmp( argument, "room" ) )
  {
    if ( ( get_room_index( last_vnum + 1 ) ) != NULL )
    {
      bug( "Room Vnums Full!!!", 0);
      send_to_char( "Room Vnums Full -- Please inform Puck or Dalren.\n\r", ch );
      return -1;
    }
  }


  return last_vnum + 1;     
  
}

/*
 * Checks to see if ch can edit in his current room.
 */
bool char_can_hqedit( CHAR_DATA *ch )
{
  char mage_col[20];

  if ( IS_NPC(ch) )
    return FALSE;
  if ( IS_JUDGE( ch ) )
    return TRUE;
  else if ( !IS_CLASS(ch, CLASS_MAGE) && strlen(ch->clan) < 2)
    return FALSE;
  else if ( IS_CLASS( ch, CLASS_MAGE ) )
  {
    if (ch->pcdata->powers[MPOWER_RUNE0] == RED_MAGIC)
      strcpy( mage_col, "Red" );
    else if (ch->pcdata->powers[MPOWER_RUNE0] == BLUE_MAGIC)
      strcpy( mage_col, "Blue" );
    else if (ch->pcdata->powers[MPOWER_RUNE0] == GREEN_MAGIC)
      strcpy( mage_col, "Green" );
    else if (ch->pcdata->powers[MPOWER_RUNE0] == YELLOW_MAGIC)
      strcpy( mage_col, "Yellow" );
    else strcpy( mage_col, "Purple" );
      
    if ( !strncmp( ch->in_room->area->name, mage_col, strlen(mage_col) ) )
      return TRUE;
  }
  else if ( !strncmp( ch->in_room->area->name, ch->clan, strlen(ch->clan)) )
    return TRUE;

  return FALSE;
}

/* 
 * Be sure to check valid portal exits
 * created by Puck.
 */
int valid_portal_destination ( ROOM_INDEX_DATA *room )
{
    switch (room->vnum)
    {
        default: 
	    break;
	case ROOM_VNUM_LIMBO: case 1:
	case ROOM_VNUM_HELL:
	case ROOM_VNUM_CHAT:
	case ROOM_VNUM_PUNISHMENT:
	case 29500:
	case 29501: 
	case 29502:
	case 10258:
	case 30002: 
	case 30003: 
	case 30004: 
	case 30005:
	case 30006: 
	case 30007: 
	case 30008: 
	    return 0;
    }

    if (IS_SET(room->room_flags, ROOM_NO_DECAP))
	return 0;

    return 1;
}



/*
 * Unless you are Puck or Xodin, or fucking
 * around....do not edit beyond this point
 * took forever to get to work :P -- Xodin 
 */

void save_new_creation( CHAR_DATA *ch  )
{ 
  FILE *fp;
  char buf[MAX_INPUT_LENGTH];

    
  sprintf( buf, "cp ../area/%s ../area/backup/%s.%d ", 
	ch->in_room->area->filename, ch->in_room->area->filename,
	(int)current_time);

  system( buf );

  sprintf( buf, "../area/%s", ch->in_room->area->filename );

  if ( ( fp = fopen( buf, "w" ) ) == NULL )
  {
    bug("NULL fp in save_new_creation.", 0);
    return;
  }

  if ( !write_area( ch, fp ) )
  {
    bug( "Write_area incomplete.", 0);
    fclose( fp );
    return;
  }

  fclose( fp );

  sprintf( buf, "../area/dos2unix ../area/%s", ch->in_room->area->filename);
  system( buf );
}

bool write_area( CHAR_DATA *ch, FILE *fp )
{
  char buf[MAX_INPUT_LENGTH];

  sprintf( buf, "#AREA %s %s~\n\n\n", ch->in_room->area->author, 
					ch->in_room->area->name);
  fprintf( fp, buf);
  fprintf( fp, "#MOBILES\n" );
  write_mobiles( ch, fp );
  fprintf( fp, "#0\n\n" );
  fprintf( fp, "#OBJECTS\n" );
  write_objects( ch, fp );
  fprintf( fp, "#0\n\n" );
  fprintf( fp, "#ROOMS\n" );
  write_rooms( ch, fp );
  fprintf( fp, "#0\n\n" );
  fprintf( fp, "#RESETS\n" );
  write_resets( ch, fp );
  fprintf( fp, "S\n\n" );
  fprintf( fp, "#SPECIALS\n" );
  write_specials( ch, fp );
  fprintf( fp, "S\n\n" );
  fprintf( fp, "#$\n" );

  return TRUE;
}

void write_mobiles( CHAR_DATA *ch, FILE *fp )
{
  MOB_INDEX_DATA *scanMob;
  int iScan;

  for ( iScan = 1; iScan < 32000; iScan++ )
  {
    if ( ( scanMob = get_mob_index( iScan ) ) == NULL )
      continue;
    if ( scanMob->area_from != ch->in_room->area ) 
      continue;
    fprintf( fp, "#%d\n", scanMob->vnum );
    fprintf( fp, "%s~\n", scanMob->player_name );
    fprintf( fp, "%s~\n", scanMob->short_descr );
    fprintf( fp, "%s~\n", scanMob->long_descr );
    fprintf( fp, "%s~\n", scanMob->description );
    fprintf( fp, "%d %d %d S\n", scanMob->act, scanMob->affected_by, scanMob->alignment );
    fprintf( fp, "%d %d %d %dd%d+%d %dd%d+%d\n", 
       scanMob->level, scanMob->hitroll, scanMob->ac, scanMob->hitnodice, scanMob->hitsizedice, 
       scanMob->hitplus, scanMob->damnodice, scanMob->damsizedice, scanMob->damplus );
    fprintf( fp, "%d 0\n", scanMob->gold );
    fprintf( fp, "0 0 %d\n", scanMob->sex );
  }

}
     
void write_objects( CHAR_DATA *ch, FILE *fp )
{
  OBJ_INDEX_DATA *scanObj;
  AFFECT_DATA *paf;
  EXTRA_DESCR_DATA *ed;
  int iScan;

  for ( iScan = 1; iScan < 32000; iScan++ )
  {
    if ( ( scanObj = get_obj_index( iScan ) ) == NULL )
      continue;
    if ( scanObj->area_from != ch->in_room->area) 
      continue;
    fprintf( fp, "#%d\n", scanObj->vnum );
    fprintf( fp, "%s~\n", scanObj->name );
    fprintf( fp, "%s~\n", scanObj->short_descr );
    fprintf( fp, "%s~\n", scanObj->description );
    fprintf( fp, "~\n" );
    fprintf( fp, "%d %d %d\n", scanObj->item_type, scanObj->extra_flags, scanObj->wear_flags );
    fprintf( fp, "%d %d %d %d\n", scanObj->value[0], scanObj->value[1], scanObj->value[2], scanObj->value[3] );
    fprintf( fp, "%d %d 0\n", scanObj->weight, scanObj->cost );
    if ( scanObj->affected != NULL )
    {
      paf = scanObj->affected;
      for ( ; ; )
      {
        if ( paf == NULL )
          break;
        fprintf( fp, "A\n" );
        fprintf( fp, "%d %d\n", paf->location, paf->modifier );
        paf = paf->next;
      }
    }
    if ( scanObj->extra_descr )
    {
      ed = scanObj->extra_descr;
      for( ; ; )
      {
        if ( ed == NULL )
          break;
        fprintf( fp, "E\n" );
        fprintf( fp, "%s~\n", ed->keyword );
        fprintf( fp, "%s~\n", ed->description );
        ed = ed->next;
      }
    }
    if ( scanObj->specpower != 0 || scanObj->spectype != 0 )
    {
      fprintf( fp, "Q\n" );
      fprintf( fp, "%s~\n", scanObj->chpoweron );
      fprintf( fp, "%s~\n", scanObj->chpoweroff );
      fprintf( fp, "%s~\n", scanObj->chpoweruse );
      fprintf( fp, "%s~\n", scanObj->victpoweron );
      fprintf( fp, "%s~\n", scanObj->victpoweroff );
      fprintf( fp, "%s~\n", scanObj->victpoweruse );
      fprintf( fp, "%d %d\n", scanObj->spectype, scanObj->specpower );
    }
  }
}

void write_rooms( CHAR_DATA *ch, FILE *fp )
{
  ROOM_INDEX_DATA *scanRoom;
  EXTRA_DESCR_DATA *ed;
  int iScan;
  int door;

  for ( iScan = 1; iScan < 32000; iScan++ )
  {
    if ( ( scanRoom = get_room_index( iScan ) ) == NULL )
      continue;
    if ( scanRoom->area != ch->in_room->area) 
      continue;

    fprintf( fp, "#%d\n", scanRoom->vnum );
    fprintf( fp, "%s~\n", scanRoom->name );
    fprintf( fp, "%s~\n", scanRoom->description );
    fprintf( fp, "0 %d %d\n", scanRoom->room_flags, scanRoom->sector_type );

    for ( door = 0; door <= 5; door++ )
    {
      if ( scanRoom->exit[door] != NULL )
      {
        fprintf( fp, "D%d\n", door );
        fprintf( fp, "%s~\n", scanRoom->exit[door]->description );
        fprintf( fp, "%s~\n", scanRoom->exit[door]->keyword );
        if ( IS_SET( scanRoom->exit[door]->exit_info, EX_ISDOOR ) && 
             IS_SET( scanRoom->exit[door]->exit_info, EX_PICKPROOF ) )
          fprintf( fp, "2 " );
        else if ( IS_SET( scanRoom->exit[door]->exit_info, EX_ISDOOR ) )
          fprintf( fp, "1 " );
        else
          fprintf( fp, "0 " );
        fprintf( fp, "%d %d\n", scanRoom->exit[door]->key, scanRoom->exit[door]->vnum );
      }
    } 
    if ( scanRoom->extra_descr )
    {
      ed = scanRoom->extra_descr;
      for( ; ; )
      {
        if ( ed == NULL )
          break;
        fprintf( fp, "E\n" );
        fprintf( fp, "%s~\n", ed->keyword );
        fprintf( fp, "%s~\n", ed->description );
        ed = ed->next;
      }
    }
    fprintf( fp, "S\n" );
  }
}

void write_resets( CHAR_DATA *ch, FILE *fp )
{
  RESET_DATA *reset;

  if ( ch->in_room->area->reset_first == NULL )
    return;

  for ( reset = ch->in_room->area->reset_first; ; reset = reset->next )
  {
    fprintf( fp, "%c 1 %d %d %d\n*\n", reset->command, reset->arg1, reset->arg2, reset->arg3 );
    if ( reset == ch->in_room->area->reset_last )
      break;
  }
}

void write_specials( CHAR_DATA *ch, FILE *fp )
{
  MOB_INDEX_DATA *scanMob;
  int iScan;

  for ( iScan = 1; iScan < 32000; iScan++ )
  {
    if ( ( scanMob = get_mob_index( iScan ) ) == NULL )
      continue;
    if ( scanMob->area_from != ch->in_room->area) 
      continue;
    if ( scanMob->spec_fun != 0 )
      fprintf( fp, "M %d %s\n*\n", scanMob->vnum,
				get_spec_desc(scanMob->spec_fun) );
  }
}

void do_newbiepack( CHAR_DATA *ch, char *argument )
{
    char buf[MAX_STRING_LENGTH];

    do_oload(ch,"10312");
    do_oload(ch,"30333");
    do_oload(ch,"30334");
    do_oload(ch,"30335");
    do_oload(ch,"30336");
    do_oload(ch,"30337");
    do_oload(ch,"30338");
    do_oload(ch,"30339");
    do_oload(ch,"30339");
    do_oload(ch,"30340");
    do_oload(ch,"30340");
    do_oload(ch,"30342");
    do_oload(ch,"30342");
    do_oload(ch,"30343");
    do_oload(ch,"30343");
    do_oload(ch,"2622");
    sprintf(buf,"all.black newbiepack");
    do_put(ch, buf);
    sprintf(buf,"golden newbiepack");
    do_put(ch,buf);
    send_to_char("You now have a newbie pack!\n\r", ch);
}

void do_undeny( CHAR_DATA *ch, char *argument )
{
    char arg[MAX_INPUT_LENGTH];
    CHAR_DATA *victim;
    char buf[MAX_STRING_LENGTH];

    if (str_cmp(ch->name, "Coal") && str_cmp(ch->name, "Puck"))
    {
	send_to_char("Huh?\r\n", ch);
	return;
    }
    
    one_argument( argument, arg );
    if ( arg[0] == '\0' )
    {
        send_to_char( "Undeny whom?\n\r", ch );
        return;
    }

    if ( ( victim = get_char_world( ch, arg ) ) == NULL )
    {
        send_to_char( "They aren't here.\n\r", ch );
        return;
    }

    sprintf(buf, "%s: UnDeny %s",ch->name,argument);
    if (ch->level < NO_WATCH) do_watching(ch,buf);

    if ( IS_NPC(victim) )
    {
        send_to_char( "Not on NPC's.\n\r", ch );
        return;
    }

    if (!IS_SET(victim->act, PLR_DENY))
    {
	send_to_char( "They aren't denied in the first place.\r\n", ch);
	return;
    }

    REMOVE_BIT(victim->act, PLR_DENY);
    send_to_char( "You are undenied!\n\r", victim );
    send_to_char( "OK.\n\r", ch );

    return;
}

void do_gethost ( CHAR_DATA *ch, char *argument )
{
    char arg[MAX_INPUT_LENGTH];
    CHAR_DATA *victim;
    char buf[MAX_STRING_LENGTH];
    unsigned long addr;
    struct hostent *hp;

    one_argument(argument, arg);

    if ( (victim = get_char_world(ch, arg) ) == NULL)
    {
	send_to_char("They aren't here.\r\n", ch);
	return;
    }

    if (victim->desc == NULL)
    {
	send_to_char("They are linklost.\r\n", ch);
	return;
    }

    if (victim->desc->host == NULL)
    {
	send_to_char("Null descriptor.\r\n", ch);
	return;
    }

    if (victim->desc->host2 != NULL)
    {
	sprintf(buf, "%s connected from %s.\r\n", victim->name, victim->desc->host2);
	send_to_char(buf, ch);
	return;
    }
    else if ((int)(addr = inet_addr(victim->desc->host)) == -1) 
    {
        send_to_char("IP-address must be of the form a.b.c.d\r\n", ch);
	return;
    }

    hp = gethostbyaddr((char *)&addr, sizeof (addr), AF_INET);
    if (hp == NULL) 
    {
            send_to_char("Address not found.\r\n", ch);
	    victim->desc->host2 = str_dup( "(N/A)");
            return;
    }

    victim->desc->host2 = str_dup(hp->h_name);

    sprintf(buf, "%s connected from %s.\r\n", victim->name, hp->h_name);
    send_to_char(buf,ch);
    return;
}
