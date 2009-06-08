#ifndef CCLWin_H
#define CCLWin_H

#include "update.h"

enum ListTypeEnum {  LV_ICONLIST, LV_DETAILLIST };

class CCLWin : public FXMainWindow
{
FXDECLARE(CCLWin)
friend void onEventCallback(int client,FXuint cmd,void *data,FXuint size);
protected:
  // App Icons
  FXIcon	*bigicon;
  FXIcon	*miniicon;
  FXSplitter	*vsplitter;
  FXShutter	*rshutter;
  // Clients area
  FXToolBar	*ctoolbar;
  FXIcon	*playicon;
  FXIcon	*pauseicon;
  FXIcon	*stopicon;
  FXIcon	*cancelicon;
  FXIcon	*unstopicon;
  FXIcon	*swapicon;
  FXIcon	*timeicon;
  FXIcon        *msgicon;
  FXIcon        *emplogicon;
  FXIcon        *delicon;
  FXIcon        *newicon;
  FXButton	*playbutton;
  FXButton	*stopbutton;
  FXButton	*unstopbutton;
  FXButton	*cancelbutton;
  FXButton	*pausebutton;
  FXButton	*swapbutton;
  FXButton	*timebutton;
  FXButton	*newbutton;
  FXButton	*delbutton;
  FXButton	*passbutton;
  FXButton      *loginbutton;
  FXButton      *msgbutton;
  FXIconList	*clientslist;
  FXFoldingList	*clientslist2;
  FXGIFIcon	*bpcicons[4];
  FXGIFIcon	*disconicon;
  // Client Info
  FXFont	*i_fonthandle;
  FX7Segment	*i_time;
  FX7Segment	*i_terminal;
  FX7Segment	*i_products;
  FX7Segment	*i_owes;
  // Client menu
  FXMenuPane	*clmenu;
  FXMenuPane	*clsmenu;
  FXMenuCaption	*clmenu_caption;
  FXMenuCaption	*clsmenu_caption;
  FXMenuCheck	*clmenu_allowuserlogin_check;
  FXMenuCheck	*clmenu_allowmemberlogin_check;
  FXMenuCheck	*clmenu_enableassist_check;
  FXMenuCheck	*clsmenu_allowuserlogin_check;
  FXMenuCheck	*clsmenu_allowmemberlogin_check;
  FXMenuCheck	*clsmenu_enableassist_check;
  //progress
  //FXProgressDialog *prgrs;
  FXProgressBar *prgrs;
protected:
  int		 toSwap;    // Client selected to be swaped with another
  int            curempid;
  int            loginstat;
  FILE          *upfp;
  UpdateInfo     upinfo;
  enum ListTypeEnum  ListType;
public:
  int            curUpdClient;
  int            updFileCount;
  FILE          *upfpx;
  char           flname[128];
protected:
  CCLWin(){}
public:
  CCLWin(FXApp *app);
  virtual void create();
  virtual ~CCLWin();
public:
  void loadClients();
  int appendClient(int client);
  void deleteClient(int client);
  void setClientDisconnected(int client,FXbool disconnected = TRUE);
  void setClientMember(int client);
  void setEmployeeID(int empid);
  int getEmployeeID();
  FXbool auth(int id,FXuchar * digest);
  FXbool authemp(FXuchar *,FXuchar * digest);
  void updateInfo(int client);
  void updateClientIcon(int client);
  int getSelectedClient();
  void showCashing();
  void showProducts();
  void drawPasswordBox(FXEvent* event);
  void clearPasswordBox();
  void setPerms(unsigned long perm);
  void send_cmd_to_all(long cmd, char *arg, int arglen);
  void setAllClientMember(int);
  long unBlockClient(int client);
  long blockClient(int client);
  long updateClient(int client);
  long updateAllClients();
  long getUpdateFileName(char *fname, int *buf);
  uint getClientIndex(int client);
  uint getPageCount(char *cupstr, int *client);
  void updateSummaryInfo();
  void getClientInfoStr(int client, char *clbuf, int len);

public:
  long onCheckEvents(FXObject*,FXSelector,void*);
  long onCommand(FXObject*,FXSelector sel,void*);
  long onSwap(FXObject*,FXSelector,void*);
  long onTime(FXObject*,FXSelector,void*);
  long onAllowUserLogin(FXObject*,FXSelector,void*);
  long onAllowMemberLogin(FXObject*,FXSelector,void*);
  long onAbout(FXObject*,FXSelector,void*);
  long onNewClient(FXObject*,FXSelector,void*);
  long onDelClient(FXObject*,FXSelector,void*);
  long onNewProduct(FXObject*,FXSelector,void*);
  long onDelProduct(FXObject*,FXSelector,void*);
  long onEditProduct(FXObject*,FXSelector,void*);
  long onSellProduct(FXObject*,FXSelector,void*);
  long onClientSelected(FXObject*,FXSelector,void* ptr);
  long onClientSelected2(FXObject*,FXSelector,void* ptr);
  long onShowClientMenu(FXObject*,FXSelector,void* ptr);
  long onShowClientMenu2(FXObject*,FXSelector,void* ptr);
  long onTimerTick(FXObject*,FXSelector,void*);
  long onProductAdd(FXObject*,FXSelector,void* ptr);
  long onProductRemove(FXObject*,FXSelector,void* ptr);
  long onEmpLogin(FXObject*,FXSelector,void* ptr);
  long onMsgClient(FXObject*,FXSelector, void* ptr);
  long onMsgServer(FXObject*,FXSelector, void* ptr);
  long onCallAssist(FXObject*,FXSelector,void* ptr);
  long onEnableAssist(FXObject*,FXSelector,void* ptr);
  long onEnableAllAssist(FXObject*,FXSelector,void* ptr);
  long onAllAllowMemberLogin(FXObject*,FXSelector,void* ptr);
  long onAllAllowUserLogin(FXObject*,FXSelector,void* ptr);
  long onListShutter(FXObject*,FXSelector,void* ptr);
  FXbool employeeLogin(FXObject*, FXSelector, void*ptr);
  void employeeLogout(FXObject*, FXSelector, void*ptr);
  FXbool clientHelpIsUp(int client);
  bool confirmAll();
  long sendUpdateChunk(int, long);
  int  startUpdateClient(int client, char *fname);
  int  doNextUpdateFile(int client);
  int  getCurrentClient();
  long setAllClientPass();
public:
  enum {
    ID_START = FXMainWindow::ID_LAST,ID_STOP,ID_UNSTOP,ID_SWAP,
    ID_TIME,ID_MONITOROFF,ID_REBOOT,ID_POWEROFF,ID_PAUSE,ID_ABOUT,
    ID_ALLOWUSERLOGIN,ID_NEWCLIENT,ID_DELCLIENT,ID_SETPASS, ID_SETMEMBER,
    ID_CLIENTSLIST,ID_TIMERTICK,ID_QUITCLIENT,ID_CHECKEVENTS,
    ID_ALLOWMEMBERLOGIN, ID_EMPLOGIN, ID_MSGCLIENT, ID_ENABLEASSIST,
    ID_ALLSETMEMBER, ID_ALLPOWEROFF, ID_ALLREBOOT, ID_ALLMONITOROFF,
    ID_ALLALLOWUSERLOGIN, ID_ALLALLOWMEMBERLOGIN, ID_ALLENABLEASSIST,
    ID_UPDATECLIENT, ID_ALLUPDATECLIENT,ID_TOGGLELIST, ID_CLIENTSLIST2,
    ID_SHUTTER, ID_SHUTTER2, ID_ALLADMINPASS, ID_LAST
  };
};

void updateClientStatus(int client);
void onEventCallback(int client,FXuint cmd,void *data,FXuint size,
		     void *userdata);
void onConnectCallback(int client,void *userdata);
void onDisconnectCallback(int client,void *userdata);



#endif
