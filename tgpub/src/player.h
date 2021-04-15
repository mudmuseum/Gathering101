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

/* Special Bits */

#define SPC_CHAMPION	1 /* PLR_CHAMPION 4 */
#define SPC_DEMON_LORD	2 /* OLD_DEMON Flag */
#define SPC_WOLFMAN	4 /* old PLR_WOLFMAN */
#define SPC_PRINCE      8 /* old EXTRA_PRINCE */
#define SPC_SIRE	16 /* Old EXTRA_SIRE */
#define SPC_ANARCH      32 /* old extra_anarch */
#define SPC_INCONNU     64 /* old extra_inconnu */

/* Class Bits */
#define CLASS_DEMON	 1 /* PLR_DEMON 2 */
#define CLASS_MAGE	 2 
#define CLASS_WEREWOLF	 4 /* PLR_WEREWOLF */
#define CLASS_VAMPIRE	 8 
#define CLASS_HIGHLANDER 16 /* EXTRA_HIGHLANDER */

/* 
 * Bits For Highlanders
 */
#define HPOWER_WPNSKILL		0

#define HPOWER_ENLIGHTENED	0
#define HPOWER_MASTERED 	1
#define HPOWER_TRAINED  	2

/* 
 * Bits For Mages
 */
#define MPOWER_RUNE0	0
#define MPOWER_RUNE1	1
#define MPOWER_RUNE2	2
#define MPOWER_RUNE3	3
#define MPOWER_RUNE4	4
#define MPOWER_LRUNES   1
#define MPOWER_LGLYPHS  2
#define MPOWER_LSIGILS  3

/*
 * Bits for Werewolves 
 */
/* Stats */

#define MAGE_CHAIN         6
#define UNI_GEN        	0 /* vampgen */
#define UNI_AFF		1 /* vampaff */
#define UNI_CURRENT	2 /* vamppass */
#define UNI_RAGE	3 /* ch->wolf = how big rage they are in*/
#define UNI_FORM0      	4/* wolfform[0] */
#define UNI_FORM1      	5 /* wolfform[1] */
#define	WOLF_POLYAFF	6
#define WOLF_AUSPICE    7
/* 
 * Powers 
 * Totems for werewolves.
 */

#define WPOWER_MANTIS	      0
#define WPOWER_BEAR	      1
#define WPOWER_LYNX	      2
#define WPOWER_BOAR	      3
#define WPOWER_OWL	      4
#define WPOWER_SPIDER	      5
#define WPOWER_WOLF	      6
#define WPOWER_HAWK	      7
#define WPOWER_STAG	      8
#define WPOWER_SILVER	     10


/*
 * Bits for Demonic Champions.
 */

/*Stats*/

#define DEMON_CURRENT		      8	/* POWER_CURRENT 0*/
#define DEMON_TOTAL		      9	/* POWER_TOTAL   1*/
#define DEMON_POWER		     10 /* TOTAL ARMOUR BOUNS */

/*Powers*/
#define DPOWER_FLAGS		      0 /* C_POWERS  1 */
#define DPOWER_CURRENT		      1 /* C_CURRENT 2 */
#define DPOWER_HEAD		      2 /* C_HEAD    3 */
#define DPOWER_TAIL		      3 /* C_TAIL    4 */
#define DPOWER_OBJ_VNUM		      4 /* DISC[10]  5 */

#define DEM_UNFOLDED		      1

#define DEM_FANGS		      1
#define DEM_CLAWS		      2
#define DEM_HORNS		      4
#define DEM_HOOVES		      8
#define DEM_EYES		     16
#define DEM_WINGS		     32
#define DEM_MIGHT		     64
#define DEM_TOUGH		    128
#define DEM_SPEED		    256
#define DEM_TRAVEL		    512
#define DEM_SCRY		   1024
#define DEM_SHADOWSIGHT		   2048
/* Object Powers */
#define DEM_MOVE		   4096   /* 1 Can roll as an object */
#define DEM_LEAP		   8192   /* 2 Can leap out of someone's hands */
#define DEM_MAGIC		  16384   /* 4 Can cast spells as an object */
#define DEM_LIFESPAN		  32768   /* 8 Can change lifespan */
#define DEM_TAIL                  65536
#define DEM_REIGN		 131072	  /* Mobs aren't aggressive in hell */

#define HEAD_NORMAL		      0
#define HEAD_WOLF		      1
#define HEAD_EAGLE		      2
#define HEAD_LION		      4
#define HEAD_SNAKE		      8

#define TAIL_NONE		      0
#define TAIL_POISONOUS		      1
#define TAIL_SCORPION		      2

/* Timer Bits */
#define TIMER_DEATH		      0
#define TIMER_OUTCAST		      1
#define TIMER_SILENCED		      2
#define TIMER_UNUSED_3		      3
#define TIMER_UNUSED_4		      4

