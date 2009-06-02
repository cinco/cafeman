#ifndef CCLWin_H
#define CCLWin_H

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
  FXIconList	*clientslist;
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
  FXMenuCaption	*clmenu_caption;
  FXMenuCheck	*clmenu_allowuserlogin_check;
  FXMenuCheck	*clmenu_allowmemberlogin_check;
protected:
  int		 toSwap;    // Client selected to be swaped with another
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
  FXbool auth(int id,FXuchar * digest);
  void updateInfo(int client);
  void updateClientIcon(int client);
  int getSelectedClient();
  void showCashing();
  void showProducts();
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
  long onShowClientMenu(FXObject*,FXSelector,void* ptr);
  long onTimerTick(FXObject*,FXSelector,void*);
  long onProductAdd(FXObject*,FXSelector,void* ptr);
  long onProductRemove(FXObject*,FXSelector,void* ptr);
public:
  enum {
    ID_START = FXMainWindow::ID_LAST,ID_STOP,ID_UNSTOP,ID_SWAP,
    ID_TIME,ID_MONITOROFF,ID_REBOOT,ID_POWEROFF,ID_PAUSE,ID_ABOUT,
    ID_ALLOWUSERLOGIN,ID_NEWCLIENT,ID_DELCLIENT,ID_SETPASS, ID_SETMEMBER,
    ID_CLIENTSLIST,ID_TIMERTICK,ID_QUITCLIENT,ID_CHECKEVENTS,
    ID_ALLOWMEMBERLOGIN,
    ID_LAST
  };
};

void updateClientStatus(int client);
void onEventCallback(int client,FXuint cmd,void *data,FXuint size,
		     void *userdata);
void onConnectCallback(int client,void *userdata);
void onDisconnectCallback(int client,void *userdata);

#endif
