/*
 * signals.c - signal name handling 
 *
 * Copyright (C) 1993-2002 Werner Almesberger
 * Copyright (C) 2002-2005 Craig Small
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */




#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <signal.h>

#include "i18n.h"
#include "signals.h"


typedef struct
{
  int number;
  const char *name;
}
SIGNAME;


static SIGNAME signals[] = {
#include "signames.h"
  {0, NULL}
};


void
list_signals (void)
{
  SIGNAME *walk;
  int col;

  col = 0;
  for (walk = signals; walk->name; walk++)
    {
      if (col + strlen (walk->name) + 1 > 80)
	{
	  putchar ('\n');
	  col = 0;
	}
      printf ("%s%s", col ? " " : "", walk->name);
      col += strlen (walk->name) + 1;
    }
  putchar ('\n');
}


int
get_signal (char *name, const char *cmd)
{
  SIGNAME *walk;

  if (isdigit (*name))
    return atoi (name);
  if (!strncmp("SIG", name, 3))
    name += 3;
  for (walk = signals; walk->name; walk++)
    if (!strcmp (walk->name, name))
      break;
  if (walk->name)
    return walk->number;
  fprintf (stderr, _("%s: unknown signal; %s -l lists signals.\n"), name, cmd);
  exit (1);
}
