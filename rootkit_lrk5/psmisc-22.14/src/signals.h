/* signals.h - signal name handling */

/* Copyright 1993-1995 Werner Almesberger. See file COPYING for details. */


#ifndef SIGNALS_H
#define SIGNALS_H

void list_signals (void);

/* Lists all known signal names on standard output. */

int get_signal (char *name, const char *cmd);

/* Returns the signal number of NAME. If no such signal exists, an error
   message is displayed and the program is terminated. CMD is the name of the
   application. */

#endif
