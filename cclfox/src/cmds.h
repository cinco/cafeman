/* 
 * cmds.h
 * commands sent by the server to the client from client to server
 */

#ifndef CMDS_H
#define CMDS_H
/* Commands sent by the server */
#define CS_STOP		  1UL   /* End session */
#define CS_START	  2UL   /* Start session */
#define CS_PAUSE	  3UL   /* Pause session */
#define CS_RESUME	  4UL   /* Resume last session */
#define CS_MONITOROFF	  5UL   /* Turn off client's monitor */
#define CS_SETTIMEOUT	  6UL   /* Set the clients timeout */
#define CS_SHUTDOWN	  7UL   /* Shutdown client's computer */
#define CS_REBOOT	  8UL   /* Reboot client's computer */
#define CS_SETTIME	  9UL   /* Set client's time */
#define CS_SETOWED	  10UL  /* Set client's owed */
#define CS_SETADDITIONAL  11UL  /* Set client's additional owed */
#define CS_QUITCLIENT	  12UL  /* Tells the client to close itself */
#define CS_DISPLAYMESSAGE 13UL  /* Shows a message */
#define CS_UNLOCKSCREEN	  14UL	/* Unlock the screen */
#define CS_LOCKSCREEN	  15UL	/* Lock the screen */
#define CS_ENABLEPASSWORDEDIT 16UL/* Enable password edit */
#define CS_ALLOWMEMBERLOGIN 17UL/* Enable member login */
#define CS_ALLOWUSERLOGIN 18UL	/* Enable user login */

/* Commands sent by the client */
#define CC_USEREXIT	  1UL	/* The user ended the session */
#define CC_USERLOGIN	  2UL	/* The user wants to start a new session */
#define CC_GETSTATUS	  4UL	/* Request the status (time, owed, etc) */
#define CC_GETTIME	  5UL	/* Request the used time */
#define CC_GETOWED	  6UL	/* Request the amount owed by the user */
#define CC_GETTIMEOUT	  7UL	/* Request the timeout */
#define CC_MEMBERLOGIN	  8UL	/* Login with member id */
#define CC_SETMEMBERPASSWORD 9UL/* Change the password for this member */
#define CC_MEMBERLOGINWITHNAME 10UL /* Login with member name */

#endif /* ifndef CMDS_H */
