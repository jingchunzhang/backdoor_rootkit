/* i18n.h - common i18n declarations for psmisc programs.  */

#ifndef I18N_H
#define I18N_H

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#ifdef ENABLE_NLS
#include <locale.h>
#include <libintl.h>
#define _(String) gettext (String)
#else
#define _(String) (String)
#endif

#endif

#ifndef HAVE_RPMATCH
#define rpmatch(line) \
	( (line == NULL)? -1 : \
	  (*line == 'y' || *line == 'Y')? 1 : \
	  (*line == 'n' || *line == 'N')? 0 : \
	  -1 )
#endif
