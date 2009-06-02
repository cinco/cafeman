#ifndef CCLFOX_H
#define CCLFOX_H

#ifdef HAVE_GETTEXT
# include <libintl.h>
# include <locale.h>
# define _(String) gettext(String)
# define N_(String) (String)
#else
# define _(String) (String)
# define N_(String) (String)
#endif

#define USERSTOP	(1<<16)	// The session was stoped by the client?
#define ALLOWUSERLOGIN	(1<<17)	// Allow the user to start the session?
#define CLIENT_DELETED	(1<<18) // This client was deleted
#define ALLOWMEMBERLOGIN (1<<19)// Allow members to start the session?

#define NOTPAID		0
#define PAID		(1<<0)
#define CANCELED	(1<<1)
#define WITH_DISCOUNT	(1<<2)

#endif
