/* src/scrolls.c: scroll code

   Copyright (c) 1989-94 James E. Wilson, Robert A. Koeneke

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 3 of the License, or
   (at your option) any later version.
   
   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU Library General Public License for more details.
   
   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
*/

#include "config.h"
#include "constant.h"
#include "types.h"
#include "externs.h"

#include <string.h>

/* Scrolls for the reading				-RAK-	*/
void
read_scroll ()
{
  int j, k, item_val, y, x;
  int tmp[6], flag, used_up;
  bigvtype out_val, tmp_str;
  register int ident, l;
  register inven_type *i_ptr;
  register struct misc *m_ptr;

  free_turn_flag = TRUE;
  if (py.flags.blind > 0)
    msg_print ("You can't see to read the scroll.");
  else if (no_light ())
    msg_print ("You have no light to read by.");
  else if (py.flags.confused > 0)
    msg_print ("You are too confused to read a scroll.");
  else if (inven_ctr == 0)
    msg_print ("You are not carrying anything!");
  else if (!find_range (TV_SCROLL, TV_NEVER, &j, &k))
    msg_print ("You are not carrying any scrolls!");
  else if (get_item (&item_val, "Read which scroll?", j, k, NULL, NULL))
    {
      i_ptr = &inventory[item_val];
      free_turn_flag = FALSE;
      used_up = TRUE;
      ident = FALSE;

      j = object_list[i_ptr->index].effect_idx;

	  /* Scrolls.                   */
	  switch (j)
	    {
	    case 0:
	      i_ptr = &inventory[INVEN_WIELD];
	      if (i_ptr->tval != TV_NOTHING)
		{
		  objdes (tmp_str, sizeof (tmp_str), i_ptr, FALSE);
		  snprintf (out_val, sizeof (out_val),
			    "Your %s glows faintly!", tmp_str);
		  msg_print (out_val);
		  if (enchant (&i_ptr->tohit, 10))
		    {
		      i_ptr->flags &= ~TR_CURSED;
		      calc_bonuses ();
		    }
		  else
		    msg_print ("The enchantment fails.");
		  ident = TRUE;
		}
	      break;
	    case 1:
	      i_ptr = &inventory[INVEN_WIELD];
	      if (i_ptr->tval != TV_NOTHING)
		{
		  objdes (tmp_str, sizeof (tmp_str), i_ptr, FALSE);
		  snprintf (out_val, sizeof (out_val),
			    "Your %s glows faintly!", tmp_str);
		  msg_print (out_val);
		  if ((i_ptr->tval >= TV_HAFTED)
		      && (i_ptr->tval <= TV_DIGGING))
		    j = i_ptr->damage[0] * i_ptr->damage[1];
		  else		/* Bows' and arrows' enchantments should not be limited
				   by their low base damages */
		    j = 10;
		  if (enchant (&i_ptr->todam, j))
		    {
		      i_ptr->flags &= ~TR_CURSED;
		      calc_bonuses ();
		    }
		  else
		    msg_print ("The enchantment fails.");
		  ident = TRUE;
		}
	      break;
	    case 2:
	      k = 0;
	      l = 0;
	      if (inventory[INVEN_BODY].tval != TV_NOTHING)
		tmp[k++] = INVEN_BODY;
	      if (inventory[INVEN_ARM].tval != TV_NOTHING)
		tmp[k++] = INVEN_ARM;
	      if (inventory[INVEN_OUTER].tval != TV_NOTHING)
		tmp[k++] = INVEN_OUTER;
	      if (inventory[INVEN_HANDS].tval != TV_NOTHING)
		tmp[k++] = INVEN_HANDS;
	      if (inventory[INVEN_HEAD].tval != TV_NOTHING)
		tmp[k++] = INVEN_HEAD;
	      /* also enchant boots */
	      if (inventory[INVEN_FEET].tval != TV_NOTHING)
		tmp[k++] = INVEN_FEET;

	      if (k > 0)
		l = tmp[randint (k) - 1];
	      if (TR_CURSED & inventory[INVEN_BODY].flags)
		l = INVEN_BODY;
	      else if (TR_CURSED & inventory[INVEN_ARM].flags)
		l = INVEN_ARM;
	      else if (TR_CURSED & inventory[INVEN_OUTER].flags)
		l = INVEN_OUTER;
	      else if (TR_CURSED & inventory[INVEN_HEAD].flags)
		l = INVEN_HEAD;
	      else if (TR_CURSED & inventory[INVEN_HANDS].flags)
		l = INVEN_HANDS;
	      else if (TR_CURSED & inventory[INVEN_FEET].flags)
		l = INVEN_FEET;

	      if (l > 0)
		{
		  i_ptr = &inventory[l];
		  objdes (tmp_str, sizeof (tmp_str), i_ptr, FALSE);
		  snprintf (out_val, sizeof (out_val),
			    "Your %s glows faintly!", tmp_str);
		  msg_print (out_val);
		  if (enchant (&i_ptr->toac, 10))
		    {
		      i_ptr->flags &= ~TR_CURSED;
		      calc_bonuses ();
		    }
		  else
		    msg_print ("The enchantment fails.");
		  ident = TRUE;
		}
	      break;
	    case 3:
	      msg_print ("This is an identify scroll.");
	      ident = TRUE;
	      used_up = ident_spell ();

	      /* The identify may merge objects, causing the identify scroll
	         to move to a different place.  Check for that here.  It can
	         move arbitrarily far if an identify scroll was used on
	         another identify scroll, but it always moves down. */
	      while (i_ptr->tval != TV_SCROLL || 
                     object_list[i_ptr->index].effect_idx != 3)
		{
		  item_val--;
		  i_ptr = &inventory[item_val];
		}
	      break;
	    case 4:
	      if (remove_curse ())
		{
		  msg_print ("You feel as if someone is watching over you.");
		  ident = TRUE;
		}
	      break;
	    case 5:
	      ident = light_area (char_row, char_col);
	      break;
	    case 6:
	      for (k = 0; k < randint (3); k++)
		{
		  y = char_row;
		  x = char_col;
		  ident |= summon_monster (&y, &x, FALSE);
		}
	      break;
	    case 7:
	      teleport (10);
	      ident = TRUE;
	      break;
	    case 8:
	      teleport (100);
	      ident = TRUE;
	      break;
	    case 9:
	      dun_level += (-3) + 2 * randint (2);
	      if (dun_level < 1)
		dun_level = 1;
	      new_level_flag = TRUE;
	      ident = TRUE;
	      break;
	    case 10:
	      if (py.flags.confuse_monster == 0)
		{
		  msg_print ("Your hands begin to glow.");
		  py.flags.confuse_monster = TRUE;
		  ident = TRUE;
		}
	      break;
	    case 11:
	      ident = TRUE;
	      map_area ();
	      break;
	    case 12:
	      ident = sleep_monsters1 (char_row, char_col);
	      break;
	    case 13:
	      ident = TRUE;
	      warding_glyph ();
	      break;
	    case 14:
	      ident = detect_treasure ();
	      break;
	    case 15:
	      ident = detect_object ();
	      break;
	    case 16:
	      ident = detect_trap ();
	      break;
	    case 17:
	      ident = detect_sdoor ();
	      break;
	    case 18:
	      msg_print ("This is a mass genocide scroll.");
	      mass_genocide ();
	      ident = TRUE;
	      break;
	    case 19:
	      ident = detect_invisible ();
	      break;
	    case 20:
	      msg_print ("There is a high pitched humming noise.");
	      aggravate_monster (20);
	      ident = TRUE;
	      break;
	    case 21:
	      ident = trap_creation ();
	      break;
	    case 22:
	      ident = td_destroy ();
	      break;
	    case 23:
	      ident = door_creation ();
	      break;
	    case 24:
	      msg_print ("This is a Recharge-Item scroll.");
	      ident = TRUE;
	      used_up = recharge (60);
	      break;
	    case 25:
	      msg_print ("This is a genocide scroll.");
	      genocide ();
	      ident = TRUE;
	      break;
	    case 26:
	      ident = unlight_area (char_row, char_col);
	      break;
	    case 27:
	      ident = protect_evil ();
	      break;
	    case 28:
	      ident = TRUE;
	      create_food ();
	      break;
	    case 29:
	      ident = dispel_creature (CD_UNDEAD, 60);
	      break;
	    case 32:
	      i_ptr = &inventory[INVEN_WIELD];
	      if (i_ptr->tval != TV_NOTHING)
		{
		  objdes (tmp_str, sizeof (tmp_str), i_ptr, FALSE);
		  snprintf (out_val, sizeof (out_val),
			    "Your %s glows brightly!", tmp_str);
		  msg_print (out_val);
		  flag = FALSE;
		  for (k = 0; k < randint (2); k++)
		    if (enchant (&i_ptr->tohit, 10))
		      flag = TRUE;
		  if ((i_ptr->tval >= TV_HAFTED)
		      && (i_ptr->tval <= TV_DIGGING))
		    j = i_ptr->damage[0] * i_ptr->damage[1];
		  else		/* Bows' and arrows' enchantments should not be limited
				   by their low base damages */
		    j = 10;
		  for (k = 0; k < randint (2); k++)
		    if (enchant (&i_ptr->todam, j))
		      flag = TRUE;
		  if (flag)
		    {
		      i_ptr->flags &= ~TR_CURSED;
		      calc_bonuses ();
		    }
		  else
		    msg_print ("The enchantment fails.");
		  ident = TRUE;
		}
	      break;
	    case 33:
	      i_ptr = &inventory[INVEN_WIELD];
	      if (i_ptr->tval != TV_NOTHING)
		{
		  objdes (tmp_str, sizeof (tmp_str), i_ptr, FALSE);
		  snprintf (out_val, sizeof (out_val),
			    "Your %s glows black, fades.", tmp_str);
		  msg_print (out_val);
		  unmagic_name (i_ptr);
		  i_ptr->tohit = -randint (5) - randint (5);
		  i_ptr->todam = -randint (5) - randint (5);
		  i_ptr->toac = 0;
		  /* Must call py_bonuses() before set (clear) flags, and
		     must call calc_bonuses() after set (clear) flags, so that
		     all attributes will be properly turned off. */
		  py_bonuses (i_ptr, -1);
		  i_ptr->flags = TR_CURSED;
		  calc_bonuses ();
		  ident = TRUE;
		}
	      break;
	    case 34:
	      k = 0;
	      l = 0;
	      if (inventory[INVEN_BODY].tval != TV_NOTHING)
		tmp[k++] = INVEN_BODY;
	      if (inventory[INVEN_ARM].tval != TV_NOTHING)
		tmp[k++] = INVEN_ARM;
	      if (inventory[INVEN_OUTER].tval != TV_NOTHING)
		tmp[k++] = INVEN_OUTER;
	      if (inventory[INVEN_HANDS].tval != TV_NOTHING)
		tmp[k++] = INVEN_HANDS;
	      if (inventory[INVEN_HEAD].tval != TV_NOTHING)
		tmp[k++] = INVEN_HEAD;
	      /* also enchant boots */
	      if (inventory[INVEN_FEET].tval != TV_NOTHING)
		tmp[k++] = INVEN_FEET;

	      if (k > 0)
		l = tmp[randint (k) - 1];
	      if (TR_CURSED & inventory[INVEN_BODY].flags)
		l = INVEN_BODY;
	      else if (TR_CURSED & inventory[INVEN_ARM].flags)
		l = INVEN_ARM;
	      else if (TR_CURSED & inventory[INVEN_OUTER].flags)
		l = INVEN_OUTER;
	      else if (TR_CURSED & inventory[INVEN_HEAD].flags)
		l = INVEN_HEAD;
	      else if (TR_CURSED & inventory[INVEN_HANDS].flags)
		l = INVEN_HANDS;
	      else if (TR_CURSED & inventory[INVEN_FEET].flags)
		l = INVEN_FEET;

	      if (l > 0)
		{
		  i_ptr = &inventory[l];
		  objdes (tmp_str, sizeof (tmp_str), i_ptr, FALSE);
		  snprintf (out_val, sizeof (out_val),
			    "Your %s glows brightly!", tmp_str);
		  msg_print (out_val);
		  flag = FALSE;
		  for (k = 0; k < randint (2) + 1; k++)
		    if (enchant (&i_ptr->toac, 10))
		      flag = TRUE;
		  if (flag)
		    {
		      i_ptr->flags &= ~TR_CURSED;
		      calc_bonuses ();
		    }
		  else
		    msg_print ("The enchantment fails.");
		  ident = TRUE;
		}
	      break;
	    case 35:
	      if ((inventory[INVEN_BODY].tval != TV_NOTHING)
		  && (randint (4) == 1))
		k = INVEN_BODY;
	      else if ((inventory[INVEN_ARM].tval != TV_NOTHING)
		       && (randint (3) == 1))
		k = INVEN_ARM;
	      else if ((inventory[INVEN_OUTER].tval != TV_NOTHING)
		       && (randint (3) == 1))
		k = INVEN_OUTER;
	      else if ((inventory[INVEN_HEAD].tval != TV_NOTHING)
		       && (randint (3) == 1))
		k = INVEN_HEAD;
	      else if ((inventory[INVEN_HANDS].tval != TV_NOTHING)
		       && (randint (3) == 1))
		k = INVEN_HANDS;
	      else if ((inventory[INVEN_FEET].tval != TV_NOTHING)
		       && (randint (3) == 1))
		k = INVEN_FEET;
	      else if (inventory[INVEN_BODY].tval != TV_NOTHING)
		k = INVEN_BODY;
	      else if (inventory[INVEN_ARM].tval != TV_NOTHING)
		k = INVEN_ARM;
	      else if (inventory[INVEN_OUTER].tval != TV_NOTHING)
		k = INVEN_OUTER;
	      else if (inventory[INVEN_HEAD].tval != TV_NOTHING)
		k = INVEN_HEAD;
	      else if (inventory[INVEN_HANDS].tval != TV_NOTHING)
		k = INVEN_HANDS;
	      else if (inventory[INVEN_FEET].tval != TV_NOTHING)
		k = INVEN_FEET;
	      else
		k = 0;

	      if (k > 0)
		{
		  i_ptr = &inventory[k];
		  objdes (tmp_str, sizeof (tmp_str), i_ptr, FALSE);
		  snprintf (out_val, sizeof (out_val),
			    "Your %s glows black, fades.", tmp_str);
		  msg_print (out_val);
		  unmagic_name (i_ptr);
		  i_ptr->flags = TR_CURSED;
		  i_ptr->tohit = 0;
		  i_ptr->todam = 0;
		  i_ptr->toac = -randint (5) - randint (5);
		  calc_bonuses ();
		  ident = TRUE;
		}
	      break;
	    case 36:
	      ident = FALSE;
	      for (k = 0; k < randint (3); k++)
		{
		  y = char_row;
		  x = char_col;
		  ident |= summon_undead (&y, &x);
		}
	      break;
	    case 37:
	      ident = TRUE;
	      bless (randint (12) + 6);
	      break;
	    case 38:
	      ident = TRUE;
	      bless (randint (24) + 12);
	      break;
	    case 39:
	      ident = TRUE;
	      bless (randint (48) + 24);
	      break;
	    case 40:
	      ident = TRUE;
	      if (py.flags.word_recall == 0)
		{
		  py.flags.word_recall = 25 + randint (30);
		  msg_print ("The air about you becomes charged.");
		}
	      else
		{
		  py.flags.word_recall = 0;
		  msg_print ("A tension leaves the air around you.");
		}
	      break;
	    case 41:
	      destroy_area (char_row, char_col);
	      ident = TRUE;
	      break;
	    default:
	      msg_print ("Internal error in scroll()");
	      break;
	    }
	  /* End of Scrolls.                           */
      i_ptr = &inventory[item_val];
      if (ident)
	{
	  if (!known1_p (i_ptr))
	    {
	      m_ptr = &py.misc;
	      /* round half-way case up */
	      m_ptr->exp += (i_ptr->level + (m_ptr->lev >> 1)) / m_ptr->lev;
	      prt_experience ();

	      identify (&item_val);
	      i_ptr = &inventory[item_val];
	    }
	}
      else if (!known1_p (i_ptr))
	sample (i_ptr);
      if (used_up)
	{
	  desc_remain (item_val);
	  inven_destroy (item_val);
	}
    }
}
