#include <ccls.h>
#include <fox-1.6/fx.h>
using namespace FX;
using namespace std;

#include "cclfox.h"
#include "ProductsFrame.h"
#include "CashingFrame.h"
#include "TarifFrame.h"
#include "MembersFrame.h"
#include "NotpaidFrame.h"
#include "LogFrame.h"
#include "CCLIconItem.h"
#include "CCLWin.h"
#include "cmds.h"
#include "icons.h"

FXDEFMAP(CCLWin) CCLWinMap[] =
{
  FXMAPFUNC(SEL_TIMEOUT,CCLWin::ID_CHECKEVENTS,CCLWin::onCheckEvents),
  FXMAPFUNC(SEL_COMMAND,CCLWin::ID_START,CCLWin::onCommand),
  FXMAPFUNC(SEL_COMMAND,CCLWin::ID_STOP,CCLWin::onCommand),
  FXMAPFUNC(SEL_COMMAND,CCLWin::ID_UNSTOP,CCLWin::onCommand),
  FXMAPFUNC(SEL_COMMAND,CCLWin::ID_SETMEMBER,CCLWin::onCommand),
  FXMAPFUNC(SEL_COMMAND,CCLWin::ID_MONITOROFF,CCLWin::onCommand),
  FXMAPFUNC(SEL_COMMAND,CCLWin::ID_REBOOT,CCLWin::onCommand),
  FXMAPFUNC(SEL_COMMAND,CCLWin::ID_POWEROFF,CCLWin::onCommand),
  FXMAPFUNC(SEL_COMMAND,CCLWin::ID_SWAP,CCLWin::onSwap),
  FXMAPFUNC(SEL_COMMAND,CCLWin::ID_TIME,CCLWin::onTime),
  FXMAPFUNC(SEL_COMMAND,CCLWin::ID_PAUSE,CCLWin::onCommand),
  FXMAPFUNC(SEL_COMMAND,CCLWin::ID_ALLOWUSERLOGIN,CCLWin::onAllowUserLogin),
  FXMAPFUNC(SEL_COMMAND,CCLWin::ID_ALLOWMEMBERLOGIN,CCLWin::onAllowMemberLogin),
  FXMAPFUNC(SEL_COMMAND,CCLWin::ID_QUITCLIENT,CCLWin::onCommand),
  FXMAPFUNC(SEL_COMMAND,CCLWin::ID_ABOUT,CCLWin::onAbout),
  FXMAPFUNC(SEL_COMMAND,CCLWin::ID_NEWCLIENT,CCLWin::onNewClient),
  FXMAPFUNC(SEL_COMMAND,CCLWin::ID_DELCLIENT,CCLWin::onDelClient),
  FXMAPFUNC(SEL_SELECTED,CCLWin::ID_CLIENTSLIST,CCLWin::onClientSelected),
  FXMAPFUNC(SEL_RIGHTBUTTONRELEASE,CCLWin::ID_CLIENTSLIST,
	    CCLWin::onShowClientMenu),
  FXMAPFUNC(SEL_TIMEOUT,CCLWin::ID_TIMERTICK,CCLWin::onTimerTick)
};

FXIMPLEMENT(CCLWin,FXMainWindow,CCLWinMap,ARRAYNUMBER(CCLWinMap))

extern FXSettings *passwords;
extern CCLWin *mainwin;
extern ProductsFrame *productsframe;
extern CashingFrame *cashingframe;
extern TarifFrame *tarifframe;
extern NotpaidFrame *notpaidframe;
extern LogFrame *logframe;
extern MembersFrame *membersframe;

int
citemSortFunc(const FXIconItem * l,const FXIconItem * r)
{
  const FXString lname = CCL_client_name_get((int) l->getData());
  const FXString rname = CCL_client_name_get((int) r->getData());

  return compare(lname,rname);
}

/****** CCLWin ******/
CCLWin::CCLWin(FXApp * app)
:FXMainWindow(app,"Cafe Con Leche - http://ccl.sf.net",NULL,NULL,
	      DECOR_ALL,0,50,800,550)
{
  toSwap = -1;
  // App Icons
  bigicon = new FXGIFIcon(getApp(),icon32_gif);
  miniicon = new FXGIFIcon(getApp(),icon16_gif);
  setIcon(bigicon);
  setMiniIcon(miniicon);
  //
  new FXToolTip(app,0);
  //
  FXHorizontalFrame *infoframe =
    new FXHorizontalFrame(this,LAYOUT_FILL_X|LAYOUT_SIDE_BOTTOM|FRAME_SUNKEN,
			  0,0,0,0,0,0,0,0,10,0);
  vsplitter = new FXSplitter(this,FRAME_RAISED|LAYOUT_FILL_X|LAYOUT_FILL_Y|
			     SPLITTER_REVERSED,0,0,0,0);
  FXShutter *lshutter =
    new FXShutter(vsplitter,NULL,0,LAYOUT_FILL_X|LAYOUT_FILL_Y,
		  0,0,0,0,0,0,0,0,0,0);
  FXShutterItem *litem1 = new FXShutterItem(lshutter,_("Clients"),NULL,
					    LAYOUT_FILL_X|LAYOUT_FILL_Y,
					    0,0,0,0,0,0,0,0);
  FXShutterItem *litem2 = new FXShutterItem(lshutter,_("Log"),NULL,
					    LAYOUT_FILL_X|LAYOUT_FILL_Y,
					    0,0,0,0,0,0,0,0);
  /********* Clients area**********/
  FXVerticalFrame *vframe =
    new FXVerticalFrame(litem1->getContent(),FRAME_SUNKEN|LAYOUT_FILL_X|
			LAYOUT_FILL_Y,0,0,0,0,0,0,0,0,0,0);

  // Toolbar
  ctoolbar = new FXToolBar(vframe,FRAME_RAISED|LAYOUT_TOP|LAYOUT_FILL_X,
			   0,0,0,0,0,0,0,0,0,0);
  playicon = new FXGIFIcon(getApp(),play_gif);
  pauseicon = new FXGIFIcon(getApp(),pause_gif);
  stopicon = new FXGIFIcon(getApp(),stop_gif);
  unstopicon = new FXGIFIcon(getApp(),continue_gif);
  swapicon = new FXGIFIcon(getApp(),swap_gif);
  timeicon = new FXGIFIcon(getApp(),time_gif);
  playbutton =
    new FXButton(ctoolbar,_("\tStart a new session"),playicon,this,ID_START,
		 BUTTON_TOOLBAR|FRAME_RAISED|LAYOUT_TOP|LAYOUT_LEFT,
		 0,0,0,0,0,0,0,0);
  pausebutton =
    new FXButton(ctoolbar,_("\tPause current session"),pauseicon,this,ID_PAUSE,
		 BUTTON_TOOLBAR|FRAME_RAISED|LAYOUT_TOP|LAYOUT_LEFT,
		 0,0,0,0,0,0,0,0);
  stopbutton =
    new FXButton(ctoolbar,_("\tStop current session"),stopicon,this,ID_STOP,
		 BUTTON_TOOLBAR|FRAME_RAISED|LAYOUT_TOP|LAYOUT_LEFT,
		 0,0,0,0,0,0,0,0);
  unstopbutton =
    new FXButton(ctoolbar,_("\tContinue session"),unstopicon,this,ID_UNSTOP,
		 BUTTON_TOOLBAR|FRAME_RAISED|LAYOUT_TOP|LAYOUT_LEFT,
		 0,0,0,0,0,0,0,0);
  swapbutton =
    new FXButton(ctoolbar,_("\tSwap sessions"),swapicon,this,ID_SWAP,
		 BUTTON_TOOLBAR|FRAME_RAISED|LAYOUT_TOP|LAYOUT_LEFT,
		 0,0,0,0,0,0,0,0);
  timebutton =
    new FXButton(ctoolbar,_("\tSet timeout"),timeicon,this,ID_TIME,
		 BUTTON_TOOLBAR|FRAME_RAISED|LAYOUT_TOP|LAYOUT_LEFT,
		 0,0,0,0,0,0,0,0);
  newbutton =
    new FXButton(ctoolbar,_("New\nclient"),NULL,this,ID_NEWCLIENT,
		 BUTTON_TOOLBAR|FRAME_RAISED|LAYOUT_TOP|LAYOUT_RIGHT,
		 0,0,0,0,0,0,0,0);
  delbutton =
    new FXButton(ctoolbar,_("Delete\nclient"),NULL,this,ID_DELCLIENT,
		 BUTTON_TOOLBAR|FRAME_RAISED|LAYOUT_TOP|LAYOUT_RIGHT,
		 0,0,0,0,0,0,0,0);

  // Iconlist
  FXSplitter *hsplitter =
    new FXSplitter(vframe,FRAME_RAISED|SPLITTER_VERTICAL|LAYOUT_FILL_X|
		   LAYOUT_FILL_Y|SPLITTER_REVERSED,0,0,0,0);

  clientslist = new FXIconList(hsplitter,this,ID_CLIENTSLIST,
			       LAYOUT_FILL_X|LAYOUT_FILL_Y|FRAME_SUNKEN|
			       ICONLIST_AUTOSIZE|ICONLIST_BIG_ICONS|
			       ICONLIST_SINGLESELECT|ICONLIST_COLUMNS);
  clientslist->setSortFunc(citemSortFunc);
  bpcicons[CCL_INACTIVE] = new FXGIFIcon(getApp(),pc00_gif);
  bpcicons[CCL_ACTIVE] = new FXGIFIcon(getApp(),pc01_gif);
  bpcicons[CCL_PAUSED] = new FXGIFIcon(getApp(),pc02_gif);
  bpcicons[3] = new FXGIFIcon(getApp(),pc03_gif);
  disconicon = new FXGIFIcon(getApp(),pc04s_gif);
  clientslist->appendHeader(_("Name"),NULL,100);
  notpaidframe = new NotpaidFrame(hsplitter);
  hsplitter->setSplit(1,100);
  /* Log */
  logframe = new LogFrame(litem2->getContent());
  /****************************************************************/
  FXLabel *labelhandle;
  FXVerticalFrame *ivframe;

  i_fonthandle = new FXFont(getApp(),"arial",13,FXFont::Bold);
  ivframe = new FXVerticalFrame(infoframe,0,0,0,0,0,0,0,0,0,0,0);
  labelhandle = new FXLabel(ivframe,_("Time:"));
//  labelhandle->setFont(i_fonthandle);
  i_time = new FX7Segment(ivframe,"00:00:00",
			  FRAME_SUNKEN|SEVENSEGMENT_SHADOW);
  i_time->setTextColor(FXRGB(230,230,230));
  i_time->setBackColor(FXRGB(0,0,0));
  ivframe = new FXVerticalFrame(infoframe,0,0,0,0,0,0,0,0,0,0,0);
  labelhandle = new FXLabel(ivframe,_("Total:"));
//  labelhandle->setFont(i_fonthandle);
  i_owes = new FX7Segment(ivframe,"0.00",
			  FRAME_SUNKEN|SEVENSEGMENT_SHADOW);
  i_owes->setTextColor(FXRGB(50,255,50));
  i_owes->setBackColor(FXRGB(0,0,0));
  ivframe = new FXVerticalFrame(infoframe,0,0,0,0,0,0,0,0,0,0,0);
  labelhandle = new FXLabel(ivframe,_("Terminal:"));
//  labelhandle->setFont(i_fonthandle);
  i_terminal = new FX7Segment(ivframe,"0.00",
			  FRAME_SUNKEN|SEVENSEGMENT_SHADOW);
  i_terminal->setTextColor(FXRGB(255,50,50));
  i_terminal->setBackColor(FXRGB(0,0,0));
  ivframe = new FXVerticalFrame(infoframe,0,0,0,0,0,0,0,0,0,0,0);
  labelhandle = new FXLabel(ivframe,_("Products:"));
//  labelhandle->setFont(i_fonthandle);
  i_products = new FX7Segment(ivframe,"0.00",
			      FRAME_SUNKEN|SEVENSEGMENT_SHADOW);
  i_products->setTextColor(FXRGB(255,50,50));
  i_products->setBackColor(FXRGB(0,0,0));
  labelhandle = NULL;
  // Client menu
  clmenu = new FXMenuPane(clientslist);
  clmenu_caption = new FXMenuCaption(clmenu,"");
  new FXMenuSeparator(clmenu);
  new FXMenuCommand(clmenu,_("Set member"),NULL,this,ID_SETMEMBER);
  new FXMenuCommand(clmenu,_("Turn off"),NULL,this,ID_POWEROFF);
  new FXMenuCommand(clmenu,_("Reboot"),NULL,this,ID_REBOOT);
  new FXMenuCommand(clmenu,_("Turn off screen"),NULL,this,ID_MONITOROFF);
  clmenu_allowuserlogin_check =
    new FXMenuCheck(clmenu,_("Allow users to start the session"),this,
		    ID_ALLOWUSERLOGIN);
  clmenu_allowmemberlogin_check =
    new FXMenuCheck(clmenu,_("Allow members to start the session"),this,
		    ID_ALLOWMEMBERLOGIN);

  /***********************************************************************/

  rshutter = new FXShutter(vsplitter,NULL,0,LAYOUT_FILL_X|LAYOUT_FILL_Y,
			   0,0,0,0,0,0,0,0,0,0);
  FXShutterItem *ritem1 =
    new FXShutterItem(rshutter,_("Products"),NULL,LAYOUT_FILL_X|LAYOUT_FILL_Y,
		      0,0,0,0,0,0,0,0);
  productsframe = new ProductsFrame(ritem1->getContent());
  FXShutterItem *ritem2 =
    new FXShutterItem(rshutter,_("Cashing"),NULL,LAYOUT_FILL_X|LAYOUT_FILL_Y,
		      0,0,0,0,0,0,0,0);
  cashingframe = new CashingFrame(ritem2->getContent());
  FXShutterItem *ritem3 =
    new FXShutterItem(rshutter,_("Tarif"),NULL,LAYOUT_FILL_X|LAYOUT_FILL_Y,
		      0,0,0,0,0,0,0,0);
  tarifframe = new TarifFrame(ritem3->getContent());
  FXShutterItem *ritem4 =
    new FXShutterItem(rshutter,_("Members"),NULL,LAYOUT_FILL_X|LAYOUT_FILL_Y,
		      0,0,0,0,0,0,0,0);
  membersframe = new MembersFrame(ritem4->getContent());

  vsplitter->setSplit(1,300);
  if (clientslist->getCurrentItem() != -1)
    onClientSelected(NULL,0,NULL);

  CCL_set_on_event_callback(onEventCallback,NULL);
  CCL_set_on_connect_callback(onConnectCallback,NULL);
  CCL_set_on_disconnect_callback(onDisconnectCallback,NULL);
  getApp()->addTimeout(this,ID_TIMERTICK,500);
  getApp()->addTimeout(this,ID_CHECKEVENTS,100);
}

CCLWin::~CCLWin()
{
  delete bigicon;
  delete miniicon;

  delete timeicon;
  delete stopicon;
  delete unstopicon;
  delete swapicon;
  delete playicon;
  delete pauseicon;
  delete disconicon;

  for (int i = 0; i < 4; i++)
    delete bpcicons[i];

  delete i_fonthandle;
}

void
CCLWin::create()
{
  FXMainWindow::create();

  bpcicons[CCL_INACTIVE]->create();
  bpcicons[CCL_ACTIVE]->create();
  bpcicons[CCL_PAUSED]->create();
  bpcicons[3]->create();
  disconicon->create();
  clientslist->sortItems();

  show();
}

void CCLWin::loadClients()
{
  int client;

  for (FXuint i = 0; -1 != (client = CCL_client_get_nth(i)); i++)
    if (!(CCL_client_flags_get(client) & CLIENT_DELETED))
      appendClient(client);
}

int CCLWin::appendClient(int client)
{
  FXString text = CCL_client_name_get(client);
  int idx = clientslist->findItem(text);

  if (-1 != idx)
    return idx;

  CCL_client_flags_toggle(client,CLIENT_DELETED,FALSE);
  int status = CCL_client_status_get(client);
  CCLIconItem *item = new CCLIconItem(text,bpcicons[status],NULL,
				      (void *)client,disconicon);

  item->setShowDisconIcon(TRUE);
  idx = clientslist->appendItem(item,TRUE);
  clientslist->sortItems();

  return idx;
}

void
CCLWin::deleteClient(int client)
{
  int idx = clientslist->findItem(CCL_client_name_get(client));

  if (-1 != idx)
    clientslist->removeItem(idx);
}

void
CCLWin::setClientDisconnected(int client,FXbool disconnected)
{
  int idx = clientslist->findItem(CCL_client_name_get(client));

  if (-1 != idx) {
    ((CCLIconItem*)clientslist->getItem(idx))->setShowDisconIcon(disconnected);
    clientslist->updateItem(idx);
  }

}

void
CCLWin::setClientMember(int client)
{
  int member = CCL_client_member_get(client);
  FXInputDialog *dlg = new FXInputDialog(this,_("Member"),
					 _("Member id (0 for none):"),
					 NULL,INPUTDIALOG_INTEGER);
  
  dlg->setLimits(0,9999);
  dlg->setText(FXStringVal(member));
  if (dlg->execute()) {
    member = FXIntVal(dlg->getText());
    if ((CCL_member_exists(member) || 0 == member)) {
      int enablepassbtn = FALSE;
      
      CCL_client_member_set(client,member);
      if (0 != member && CCL_data_key_exists(CCL_DATA_MEMBER,member,"password"))
	enablepassbtn = TRUE;
      
      CCL_client_send_cmd(client,CS_ENABLEPASSWORDEDIT,
			  &enablepassbtn,sizeof(enablepassbtn));
    }
    else
      FXMessageBox::error(this,MBOX_OK,_("Error"),_("Invalid member"));
  }
  delete dlg;
}

int CCLWin::getSelectedClient()
{
  int idx = clientslist->getCurrentItem();

  if (-1 == idx)
    return -1;
  else
    return (int)(clientslist->getItemData(idx));
}

void
CCLWin::showCashing()
{
  rshutter->setCurrent(1);
}

void
CCLWin::showProducts()
{
  rshutter->setCurrent(0);
}

FXbool
CCLWin::auth(int id,FXuchar *testpass)
{
  FXuchar *realpass;
  int size;
  int valid = FALSE;

  if (id > 0) {
    realpass = (FXuchar*)CCL_data_get_blob(CCL_DATA_MEMBER,id,"password",&size);
    valid = (CCL_member_exists(id) && realpass &&
	     !memcmp(testpass,realpass,size));
    
    if (realpass) CCL_free(realpass);
  }

  return valid;
}

void
onEventCallback(int client,FXuint cmd,void *data,FXuint size,void *userdata)
{
  time_t usedtime = CCL_client_time_used(client);
  FXuint owed = CCL_client_owed_terminal(client);
  FXuint products = CCL_client_owed_products(client);
  time_t timeout = CCL_client_timeout_get(client);
  // Values in network byte order
  FXuint nusedtime = CCL_htonl(usedtime);
  FXuint nowed = CCL_htonl(owed);
  FXuint nproducts = CCL_htonl(products);
  FXuint ntimeout = CCL_htonl(timeout);

  switch (cmd) {
    case CC_GETSTATUS:
      updateClientStatus(client);
      break;
    case CC_GETTIME:
      CCL_client_send_cmd(client,CS_SETTIME,&nusedtime,sizeof(nusedtime));
      break;
    case CC_GETTIMEOUT:
      CCL_client_send_cmd(client,CS_SETTIMEOUT,&ntimeout,sizeof(ntimeout));
      break;
    case CC_GETOWED:
      CCL_client_send_cmd(client,CS_SETOWED,&nowed,sizeof(nowed));
      CCL_client_send_cmd(client,CS_SETADDITIONAL,&nproducts,
			  sizeof(nproducts));
      break;
    case CC_USEREXIT:
      if (CCL_ACTIVE == CCL_client_status_get(client)) {
	CCL_client_stop(client);
	CCL_log_session(client,CCL_client_owed_terminal(client),NOTPAID);
	notpaidframe->readNotPaid();
      }
      CCL_client_flags_toggle(client,USERSTOP,TRUE);
      CCL_client_send_cmd(client,CS_STOP,NULL,0);
      CCL_client_send_cmd(client,CS_LOCKSCREEN,NULL,0);
      break;
    case CC_USERLOGIN:
      if (CCL_client_flags_get(client) & ALLOWUSERLOGIN) {
	if (CCL_client_flags_get(client) & USERSTOP) {
	  CCL_client_unstop(client);
	  notpaidframe->readNotPaid();
	} else
	  CCL_client_start(client);

	CCL_client_flags_toggle(client,USERSTOP,FALSE);
	updateClientStatus(client);
      }
      break;
    case CC_MEMBERLOGIN:      
    case CC_MEMBERLOGINWITHNAME:
      if (CCL_client_flags_get(client) & ALLOWMEMBERLOGIN) {
	int memberid;
	FXuchar *md5hash;
	
	if (CC_MEMBERLOGIN == cmd) {
	  memberid = CCL_ntohl(((FXuint*) data)[0]);
	  md5hash = (FXuchar*) ((FXuint*)data+1);
	} else {
	  char *c;

	  memberid =
	    CCL_data_find_by_key_sval(CCL_DATA_MEMBER,"login_name",(char*)data);
	  for (c = (char*)data; *c != '\0'; c++)
	    ;
	  md5hash = (FXuchar*)(c+1);
	}

	if (-1 != memberid &&  mainwin->auth(memberid,md5hash)) {
	  CCL_client_send_cmd(client,CS_UNLOCKSCREEN,NULL,0);
	  if ((CCL_client_flags_get(client) & USERSTOP) &&
	      CCL_client_member_get(client) == memberid) {
	    CCL_client_unstop(client);
	    notpaidframe->readNotPaid();
	  } else
	    CCL_client_start(client);
	  
	  CCL_client_member_set(client,memberid);
	  CCL_client_flags_toggle(client,USERSTOP,FALSE);
	  updateClientStatus(client);
	}
      }
      break;
    case CC_SETMEMBERPASSWORD:
      if (mainwin->auth(CCL_client_member_get(client),(FXuchar*)data)) {
	CCL_data_set_blob(CCL_DATA_MEMBER,CCL_client_member_get(client),
			  "password",((FXuchar*)data)+CCL_MD5_DIGEST_LENGTH,
			  CCL_MD5_DIGEST_LENGTH);
      } else {
	const char * message = _("Wrong old password.");
	
	CCL_client_send_cmd(client,CS_DISPLAYMESSAGE,message,strlen(message)+1);
      }
      break;
  }
}

void
updateClientStatus(int client)
{
  if (CCL_client_status_get(client) == CCL_ACTIVE) {
    time_t usedtime = CCL_client_time_used(client);
    FXuint owed = CCL_client_owed_terminal(client);
    FXuint products = CCL_client_owed_products(client);
    time_t timeout = CCL_client_timeout_get(client);
    int member = CCL_client_member_get(client);
    int enablepassbtn = CCL_htonl(0);
    // Values in network byte order
    FXuint nusedtime = CCL_htonl(usedtime);
    FXuint nowed = CCL_htonl(owed);
    FXuint nproducts = CCL_htonl(products);
    FXuint ntimeout = CCL_htonl(timeout);

    if (0 != member && CCL_data_key_exists(CCL_DATA_MEMBER,member,"password"))
      enablepassbtn = CCL_htonl(1);
    
    CCL_client_send_cmd(client,CS_START,NULL,0);
    CCL_client_send_cmd(client,CS_SETTIME,&nusedtime,sizeof(nusedtime));
    CCL_client_send_cmd(client,CS_SETTIMEOUT,&ntimeout,sizeof(ntimeout));
    CCL_client_send_cmd(client,CS_SETOWED,&nowed,sizeof(nowed));
    CCL_client_send_cmd(client,CS_SETADDITIONAL,&nproducts,
			sizeof(nproducts));
    CCL_client_send_cmd(client,CS_UNLOCKSCREEN,NULL,0);
    CCL_client_send_cmd(client,CS_ENABLEPASSWORDEDIT,
			&enablepassbtn,sizeof(enablepassbtn));
  } else {
    CCL_client_send_cmd(client,CS_STOP,NULL,0);
    CCL_client_send_cmd(client,CS_LOCKSCREEN,NULL,0);
  }

  int nallowuserlogin =
    CCL_htonl(CCL_client_flags_get(client) & ALLOWUSERLOGIN);
  int nallowmemberlogin =
    CCL_htonl(CCL_client_flags_get(client) & ALLOWMEMBERLOGIN);

  CCL_client_send_cmd(client,CS_ALLOWMEMBERLOGIN,&nallowmemberlogin,
		      sizeof(nallowmemberlogin));
  CCL_client_send_cmd(client,CS_ALLOWUSERLOGIN,&nallowuserlogin,
		      sizeof(nallowuserlogin));
}

void
onConnectCallback(int client,void *userdata)
{
  mainwin->appendClient(client);
  mainwin->setClientDisconnected(client,FALSE);
}

void
onDisconnectCallback(int client,void *userdata)
{
  mainwin->setClientDisconnected(client,TRUE);
}

long
CCLWin::onCheckEvents(FXObject*,FXSelector,void*)
{
  CCL_check_events();
  getApp()->addTimeout(this,ID_CHECKEVENTS,100);

  return 1;
}

long
CCLWin::onCommand(FXObject*,FXSelector sel,void*)
{
  int current = clientslist->getCurrentItem();

  if (-1 == current)
    return 1;

  int client = (int) (clientslist->getItemData(current));
  int status = CCL_client_status_get(client);
  time_t stime;
  time_t etime;
  time_t usedtime;
  FXuint nusedtime;

  if (client) {
    switch (FXSELID(sel)) {
      case ID_START:
	CCL_client_start(client);
	CCL_client_flags_toggle(client,USERSTOP,FALSE);
	CCL_client_send_cmd(client,CS_START,NULL,0);
	CCL_client_send_cmd(client,CS_UNLOCKSCREEN,NULL,0);
	break;
      case ID_PAUSE:
	CCL_client_pause(client);
	CCL_client_send_cmd(client,CS_PAUSE,NULL,0);
	CCL_client_send_cmd(client,CS_LOCKSCREEN,NULL,0);
	break;
      case ID_STOP:
	if (status == CCL_ACTIVE) {
	  CCL_client_stop(client);
	  CCL_log_session(client,CCL_client_owed_terminal(client),NOTPAID);
	  notpaidframe->readNotPaid();
	} else
	  CCL_client_flags_toggle(client,USERSTOP,FALSE);
	CCL_client_send_cmd(client,CS_STOP,NULL,0);
	CCL_client_send_cmd(client,CS_LOCKSCREEN,NULL,0);
	break;
      case ID_UNSTOP:
	stime = CCL_client_stime_get(client);
	etime = CCL_client_etime_get(client);
	usedtime = CCL_client_time_used(client);
	nusedtime = CCL_htonl(usedtime);

	if (CCL_INACTIVE == CCL_client_status_get(client)
	    && cashingframe->getSession() == CCL_log_session_find(client,
								  stime,etime))
	  cashingframe->setSession(0);

	CCL_client_flags_toggle(client,USERSTOP,FALSE);
	CCL_client_unstop(client);
	CCL_client_unpause(client);
	notpaidframe->readNotPaid();
	CCL_client_send_cmd(client,CS_RESUME,NULL,0);
	CCL_client_send_cmd(client,CS_SETTIME,&nusedtime,sizeof(nusedtime));
	CCL_client_send_cmd(client,CS_UNLOCKSCREEN,NULL,0);
	break;
      case ID_SETMEMBER:
	setClientMember(client);
	break;
      case ID_MONITOROFF:
	CCL_client_send_cmd(client,CS_MONITOROFF,NULL,0);
	break;
      case ID_REBOOT:
	CCL_client_send_cmd(client,CS_REBOOT,NULL,0);
	break;
      case ID_POWEROFF:
	CCL_client_send_cmd(client,CS_SHUTDOWN,NULL,0);
	break;
      case ID_QUITCLIENT:
	break;
    }
    updateClientIcon(client);
  }
  productsframe->updateClientProducts(client);
  return 1;
}

long
CCLWin::onAbout(FXObject*,FXSelector,void*)
{
  FXMessageBox
    about(this,_("About"),getApp()->getAppName() + " " + VERSION + "\n" +
	  "Copyright (C) 2004 Bruno Deferrari (tizoc@users.sourceforge.net)",
	  bigicon,MBOX_OK|DECOR_TITLE|DECOR_BORDER);

  about.execute();

  return 1;
}

long
CCLWin::onSwap(FXObject*,FXSelector,void*)
{
  int current = clientslist->getCurrentItem();

  if (-1 == current)
    return 1;

  if (-1 != toSwap) {
    playbutton->enable();
    pausebutton->enable();
    stopbutton->enable();
    unstopbutton->enable();
    timebutton->enable();
    newbutton->enable();
    delbutton->enable();
    toSwap = -1;
  } else {
    playbutton->disable();
    pausebutton->disable();
    stopbutton->disable();
    unstopbutton->disable();
    timebutton->disable();
    newbutton->disable();
    delbutton->disable();
    toSwap = (int) (clientslist->getItemData(current));
  }
  return 1;
}

long
CCLWin::onTime(FXObject*,FXSelector,void*)
{
  int current = clientslist->getCurrentItem();

  if (-1 == current)
    return 1;

  int client = (int) (clientslist->getItemData(current));
  int time = CCL_client_timeout_get(client) / 60;
  FXInputDialog *dlg = new FXInputDialog(this,_("Time"),_("Minutes:"),
					 NULL,INPUTDIALOG_INTEGER);

  dlg->setLimits(1,0);
  dlg->setText(FXStringVal(time));
  if (client && dlg->execute()) {
    time = FXIntVal(dlg->getText());
    int timeout = time * 60;
    FXuint ntimeout = CCL_htonl(timeout);
    
    CCL_client_timeout_set(client,timeout);
    CCL_client_send_cmd(client,CS_SETTIMEOUT,&ntimeout,sizeof(ntimeout));
    updateClientIcon(client);
  }
  delete dlg;

  return 1;
}

long
CCLWin::onAllowUserLogin(FXObject*,FXSelector,void*)
{
  int current = clientslist->getCurrentItem();

  if (-1 == current)
    return 1;
  int client = (int) (clientslist->getItemData(current));

  CCL_client_flags_toggle(client,ALLOWUSERLOGIN,
			  !(CCL_client_flags_get(client) & ALLOWUSERLOGIN));
  int allow = CCL_htonl(CCL_client_flags_get(client) & ALLOWUSERLOGIN);
  CCL_client_send_cmd(client,CS_ALLOWUSERLOGIN,&allow,sizeof(allow));

  return 1;
}

long
CCLWin::onAllowMemberLogin(FXObject*,FXSelector,void*)
{
  int current = clientslist->getCurrentItem();

  if (-1 == current)
    return 1;
  int client = (int) (clientslist->getItemData(current));

  CCL_client_flags_toggle(client,ALLOWMEMBERLOGIN,
			  !(CCL_client_flags_get(client) & ALLOWMEMBERLOGIN));
  int allow = CCL_htonl(CCL_client_flags_get(client) & ALLOWMEMBERLOGIN);
  CCL_client_send_cmd(client,CS_ALLOWMEMBERLOGIN,&allow,sizeof(allow));

  return 1;
}

long
CCLWin::onNewClient(FXObject*,FXSelector,void*)
{
  FXString result;

  if (FXInputDialog::getString(result,this,_("Add new client"),
			       _("Type the name:")) && result.length()) {
    char *name = fxstrdup(result.text());
    int id = CCL_client_new(name);

    appendClient(id);
    FXFREE(&name);
  }
  return 1;
}

long
CCLWin::onDelClient(FXObject*,FXSelector,void*)
{
  int current = clientslist->getCurrentItem();

  if (-1 == current)
    return 1;

  int client = (int) (clientslist->getItemData(current));

  clientslist->removeItem(current);
  if (client) {
    CCL_client_flags_toggle(client, CLIENT_DELETED, TRUE);
    deleteClient(client);
  }
  productsframe->updateClientProducts(client);
  return 1;
}

long
CCLWin::onClientSelected(FXObject*,FXSelector,void* ptr)
{
  int idx = (int) ptr;
  int client = (int) (clientslist->getItemData(idx));

  if (-1 != toSwap) {
    int oldidx = clientslist->findItem(CCL_client_name_get(toSwap));

    CCL_client_swap(toSwap,client);
    updateClientIcon(client);
    updateClientIcon(toSwap);
    updateClientStatus(client);
    updateClientStatus(toSwap);
    onSwap(NULL,0,NULL);	// Reenable buttons
  }

  idx = clientslist->getCurrentItem();
  client = (int) (clientslist->getItemData(idx));
  updateInfo(client);
  productsframe->updateClientProducts(client);
  showProducts();

  return 1;
}

long
CCLWin::onShowClientMenu(FXObject*,FXSelector,void* ptr)
{
  FXEvent *event = (FXEvent *) ptr;
  int idx = clientslist->getItemAt(event->click_x,event->click_y);

  if (-1 != idx) {
    int client = (int) clientslist->getItemData(idx);

    clmenu_caption->setText(CCL_client_name_get(client));
    if (CCL_client_flags_get(client) & ALLOWUSERLOGIN)
      clmenu_allowuserlogin_check->setCheck(TRUE);
    else
      clmenu_allowuserlogin_check->setCheck(FALSE);
    if (CCL_client_flags_get(client) & ALLOWMEMBERLOGIN)
      clmenu_allowmemberlogin_check->setCheck(TRUE);
    else
      clmenu_allowmemberlogin_check->setCheck(FALSE);
    clientslist->setCurrentItem(idx,TRUE);
    clientslist->selectItem(idx,TRUE);
    clmenu->popup(NULL,event->root_x,event->root_y);
  }

  return 0;
}

long
CCLWin::onTimerTick(FXObject*,FXSelector,void*)
{
  if (int num = clientslist->getNumItems()) {
    int idx = clientslist->getCurrentItem();
    int client = (int) (clientslist->getItemData(idx));

    updateInfo(client);
    for (idx = 0; idx < num; idx++) {
      client = (int) (clientslist->getItemData(idx));
      updateClientIcon(client);
    }
  }
  getApp()->addTimeout(this,ID_TIMERTICK,500);
  return 1;
}

void
CCLWin::updateInfo(int client)
{
  time_t time = 0;

  if (-2 != CCL_client_time_left(client))
    time = CCL_client_time_left(client);
  else
    time = CCL_client_time_used(client);
  int h = time / 3600,m = (time % 3600) / 60,s = (time % 3600) % 60;
  char buf[32];

  if (time > 0)
    snprintf(buf,32,"%.2d:%.2d:%.2d",h,m,s);
  else
    snprintf(buf,32,"--:--");

  i_time->setText(buf);

  int owedp = CCL_client_owed_products(client);

  snprintf(buf,32,"%.2f",owedp / 100.0);
  i_products->setText(buf);

  int owedt = CCL_client_owed_terminal(client);

  snprintf(buf,32,"%.2f",owedt / 100.0);
  i_terminal->setText(buf);

  int owed = owedt + owedp;

  snprintf(buf,32,"%.2f",owed / 100.0);
  i_owes->setText(buf);
}

void
CCLWin::updateClientIcon(int client)
{
  int idx = clientslist->findItem(CCL_client_name_get(client));

  if (-1 == idx) return;

  if (CCL_client_time_left(client) == 0
      && CCL_ACTIVE == CCL_client_status_get(client))
    clientslist->setItemBigIcon(idx,bpcicons[3]);
  else if (CCL_client_flags_get(client) & USERSTOP)
    clientslist->setItemBigIcon(idx,bpcicons[3]);
  else
    clientslist->setItemBigIcon(idx,bpcicons[CCL_client_status_get(client)]);
}

long
CCLWin::onProductAdd(FXObject*,FXSelector,void* ptr)
{
  FXFoldingItem *child = (FXFoldingItem *) ptr;
  FXFoldingItem *prnt = child->getParent();
  int idx = clientslist->getCurrentItem();

  if (!prnt || -1 == idx)
    return 0;

  int client = (int) (clientslist->getItemData(idx));
  int amount = 0;

  if (FXInputDialog::getInteger(amount,this,_("Add products"),
				_("Insert the amount:")) && amount >= 1) {
    int pid = (int) child->getData();

    CCL_client_product_add(client,pid,amount);
    productsframe->updateClientProducts(client);
  }

  return 1;
}

long
CCLWin::onProductRemove(FXObject*,FXSelector,void* ptr)
{
  FXFoldingItem *item = (FXFoldingItem *) ptr;
  int idx = clientslist->getCurrentItem();

  if (-1 == idx)
    return 0;
  int client = (int) (clientslist->getItemData(idx));
  int amount = 0;

  if (FXInputDialog::getInteger(amount,this,_("Substract products"),
				_("Insert the amount:")) && amount >= 1) {
    int pid = (int) item->getData();

    CCL_client_product_sub(client,pid,amount);
    productsframe->updateClientProducts(client);
  }

  return 1;
}
