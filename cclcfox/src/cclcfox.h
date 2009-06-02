// cclcfox.h
//

#ifndef CCLCFOX_H
#define CCLCFOX_H

#include <time.h>
#include "cmds.h"

#include <cclc.h>

#ifdef HAVE_GETTEXT
# include <libintl.h>
# include <locale.h>
# define _(String) gettext(String)
# define N_(String) (String)
#else
# define _(String) (String)
# define N_(String) (String)
#endif

class CCLCFox : public FXObject
{
FXDECLARE(CCLCFox)
private:
  static CCLCFox   *cclc;
protected:
  FXbool	    active;
  FXbool	    networking;
  time_t	    stime;
  int		    timeout;
public:
  CCLCFox();
  ~CCLCFox();
public:
  void initCCLC();
  FXbool initCCLC(const char *cafile, const char *certfile,
		  const char * certpass = NULL);
  FXbool initNetworking(const char *sname = NULL,FXushort sport = 0,
			const char *myname = NULL);
  FXbool shutdownNetworking();
  FXbool isLocked();
  FXbool isInfoShown();
  void showInfo();
  void hideInfo();
  void start();
  void stop();
  void userExit();
  void userStart();
  void resume();
  void lockScreen();
  void unlockScreen();
  void shutdownSystem();
  void rebootSystem();
  void turnOffMonitor();
  void setOwed(FXuint owed);
  void setProducts(FXuint owed);
  void showMessage(char *message);
  void execCommand(FXuint cmd,const void *data,FXuint datasize);
  void unlockWithPass(int id,FXString password);
  void unlockWithPass(FXString login,FXString password);
  void exitProgram();
  void setPassword(FXuchar * digest);
public:
  long onTimer(FXObject*,FXSelector,void*);
  long onCheckEvents(FXObject*,FXSelector,void*);
public:
  enum {
    ID_TIMER = 0,ID_CHECKEVENTS,
    ID_LAST
  };
};
#endif
