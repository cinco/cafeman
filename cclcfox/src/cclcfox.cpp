#include <fox-1.6/fx.h>
using namespace FX;

#include "cclcfox.h"
#include "gui.h"
#include "locker.h"
#ifdef WIN32
#  include <windows.h>
#  include <winuser.h>
#endif

extern CCLCFox *cclcfox;
extern ClientWin *clientwin;
extern Locker *locker;
extern Grabber *grabber;

void
onEventCallback(FXuint cmd,void * data,FXuint size,void * userdata)
{
  cclcfox->execCommand(cmd,data,size);
}

void
onDisconnectCallback(void * userdata)
{
  cclcfox->shutdownNetworking();
}

FXDEFMAP(CCLCFox) CCLCFoxMap[] =
{
  FXMAPFUNC(SEL_TIMEOUT,CCLCFox::ID_TIMER,CCLCFox::onTimer),
  FXMAPFUNC(SEL_TIMEOUT,CCLCFox::ID_CHECKEVENTS,CCLCFox::onCheckEvents)
};

FXIMPLEMENT(CCLCFox,FXObject,CCLCFoxMap,ARRAYNUMBER(CCLCFoxMap))

CCLCFox *CCLCFox::cclc = NULL;

CCLCFox::CCLCFox()
{
  if (CCLCFox::cclc)
    fxerror("Ya existe una instancia de CCLCFox\n");
  active = FALSE;
  stime = 0;
  networking = FALSE;
  timeout = 0;
  clientwin->getApp()->addTimeout(this,CCLCFox::ID_TIMER,5000);
  CCLCFox::cclc = this;
}

CCLCFox::~CCLCFox()
{
  shutdownNetworking();
  CCLC_shutdown();
  CCLCFox::cclc = NULL;
}

void
CCLCFox::initCCLC()
{
  CCLC_init();
  CCLC_set_on_event_callback(onEventCallback,NULL);
  CCLC_set_on_disconnect_callback(onDisconnectCallback,NULL);
}

FXbool
CCLCFox::initCCLC(const char * cafile,const char * certfile,
		  const char * certpass)
{
  int error;
  
  initCCLC();

  if (!CCLC_SSL_init(cafile,certfile,certpass,&error)) {
    switch (error) {
    case CCLC_ERROR_BAD_PASSWORD:
      fxerror("[E]Bad certificate password\n");
      exit(1);
      break;
    default:
      fxmessage("[!]Couldn't init SSL\n");
      return FALSE;
      break;
    }
  }
  
  return TRUE;
}

FXbool
CCLCFox::initNetworking(const char * server,FXushort port, const char * myname)
{
  static const char *sname = server;
  static FXushort sport = port;
  static const char *cname = myname;
  int error;

  if (CCLC_networking_init(sname,sport,cname,&error)) {
    clientwin->getApp()->addTimeout(this,CCLCFox::ID_CHECKEVENTS,100);
    CCLC_send_cmd(CC_GETSTATUS,NULL,0);
    networking = TRUE;
  } else {
    switch (error) {
      case CCLC_ERROR_CONNECT_FAIL:
	fxmessage("[!]Could not connect to the server\n");
	networking = FALSE;
	break;
    }
  }

  return networking;
}

FXbool
CCLCFox::shutdownNetworking()
{
  CCLC_networking_shutdown();
  clientwin->setOwed("--.--");
  clientwin->setProducts("--.--");
  networking = FALSE;
  
  return TRUE;
}

FXbool
CCLCFox::isLocked()
{
  return locker->shown();
}

FXbool
CCLCFox::isInfoShown()
{
  return grabber->shown();
}

void
CCLCFox::showInfo()
{
  grabber->show();
  clientwin->show();
}

void
CCLCFox::hideInfo()
{
  clientwin->hide();
  grabber->hide();
}

void
CCLCFox::start()
{
  stime = time(NULL);
  clientwin->setTime("00:00");
  clientwin->setOwed("0.00");
  clientwin->setProducts("0.00");
  timeout = 0;
  active = TRUE;
}

void
CCLCFox::stop()
{
  active = FALSE;
}

void
CCLCFox::userExit()
{
  if (active)
    CCLC_send_cmd(CC_USEREXIT,NULL,0);
  else
    lockScreen();
}

void
CCLCFox::resume()
{
  active = TRUE;
}

void
CCLCFox::lockScreen()
{
  locker->lock();
  hideInfo();
}

void
CCLCFox::unlockScreen()
{
  locker->unlock();
  showInfo();
}

void
CCLCFox::shutdownSystem()
{
#ifndef WIN32
  system("/sbin/halt");
#else
  HANDLE hToken;
  TOKEN_PRIVILEGES tkp;

  // Get a token for this process.
  OpenProcessToken(GetCurrentProcess(),
		   TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY,&hToken);
  // Get the LUID for the shutdown privilege.
  LookupPrivilegeValue(NULL,SE_SHUTDOWN_NAME,&tkp.Privileges[0].Luid);
  tkp.PrivilegeCount = 1;	// one privilege to set
  tkp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;
  // Get the shutdown privilege for this process.
  AdjustTokenPrivileges(hToken,FALSE,&tkp,0,(PTOKEN_PRIVILEGES) NULL,0);

  OSVERSIONINFO osversion;

  osversion.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
  GetVersionEx(&osversion);
  if (osversion.dwPlatformId == VER_PLATFORM_WIN32_NT)	// NT/2000/XP
    ExitWindowsEx(EWX_POWEROFF | EWX_FORCE,0);
  else
    ExitWindowsEx(EWX_POWEROFF | EWX_FORCE,0);
#endif
}

void
CCLCFox::rebootSystem()
{
#ifndef WIN32
  system("/sbin/reboot");
#else
  HANDLE hToken;
  TOKEN_PRIVILEGES tkp;

  // Get a token for this process.
  OpenProcessToken(GetCurrentProcess(),
		   TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY,&hToken);
  // Get the LUID for the shutdown privilege.
  LookupPrivilegeValue(NULL,SE_SHUTDOWN_NAME,&tkp.Privileges[0].Luid);
  tkp.PrivilegeCount = 1;	// one privilege to set
  tkp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;
  // Get the shutdown privilege for this process.
  AdjustTokenPrivileges(hToken,FALSE,&tkp,0,(PTOKEN_PRIVILEGES) NULL,0);

  OSVERSIONINFO osversion;

  osversion.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
  GetVersionEx(&osversion);
  if (osversion.dwPlatformId == VER_PLATFORM_WIN32_NT)	// NT/2000/XP
    ExitWindowsEx(EWX_REBOOT | EWX_FORCE,0);
  else				// 9x/Me
    ExitWindowsEx(EWX_REBOOT | EWX_FORCE,0);
#endif
}

void
CCLCFox::turnOffMonitor()
{
#ifndef WIN32
  system("xset dpms force off");
#else
  HWND hWnd = (HWND) (clientwin->id());

  SendMessage(hWnd,WM_SYSCOMMAND,SC_MONITORPOWER,2);
#endif
}

void
CCLCFox::showMessage(char * message)
{
  FXMessageBox::information(grabber->getRoot(),MBOX_OK,_("Message"),message);
}

void
CCLCFox::setOwed(FXuint owed)
{
  char buf[64];

  snprintf(buf,64,"%.2f",owed / 100.0);
  clientwin->setOwed(buf);
}

void
CCLCFox::setProducts(FXuint owed)
{
  char buf[64];

  snprintf(buf,64,"%.2f",owed / 100.0);
  clientwin->setProducts(buf);
}

void
CCLCFox::userStart()
{
  CCLC_send_cmd(CC_USERLOGIN,NULL,0);
}

void
CCLCFox::unlockWithPass(int id,FXString password)
{
  char val[sizeof(id) + CCLC_MD5_DIGEST_LENGTH * sizeof(FXuchar)];
  FXuchar digest[CCLC_MD5_DIGEST_LENGTH];

  CCLC_MD5((FXuchar*)(password.text()),password.length(),digest);
  ((FXuint*)val)[0] = CCLC_htonl(id);
  memcpy(((FXuint*)val)+1,digest,CCLC_MD5_DIGEST_LENGTH); 
  
  CCLC_send_cmd(CC_MEMBERLOGIN,val,sizeof(val) * sizeof(char));
}

void
CCLCFox::unlockWithPass(FXString login,FXString password)
{
  const char *login_name = login.text();
  char val[strlen(login_name) * sizeof(char) + 1
	   + CCLC_MD5_DIGEST_LENGTH * sizeof(FXuchar)];
  FXuchar digest[CCLC_MD5_DIGEST_LENGTH];

  CCLC_MD5((FXuchar*)(password.text()),password.length(),digest);
  memcpy(val,login_name,strlen(login_name) + 1);
  memcpy(val + strlen(login_name) + 1,digest,CCLC_MD5_DIGEST_LENGTH); 
  
  CCLC_send_cmd(CC_MEMBERLOGINWITHNAME,val,sizeof(val) * sizeof(char));
}

void
CCLCFox::exitProgram()
{
#ifdef WIN32
  OSVERSIONINFO osversion;

  osversion.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
  GetVersionEx(&osversion);
  if (osversion.dwPlatformId == VER_PLATFORM_WIN32_NT) {	// NT/2000/XP
    HWND hwnd = FindWindow("Shell_traywnd",NULL);

    EnableWindow(hwnd,TRUE);
    HKEY hk;
    const char *key =
      "Software\\Microsoft\\Windows\\CurrentVersion\\Policies\\System";
    if (RegOpenKey(HKEY_CURRENT_USER,key,&hk) != ERROR_SUCCESS)
      RegCreateKey(HKEY_CURRENT_USER,key,&hk);
    RegDeleteValue(hk,"DisableTaskMgr");
  }
#endif
  exit(0);
}

void
CCLCFox::execCommand(FXuint cmd,const void *data,FXuint datasize)
{
  FXuint hdata = 0;

  switch (cmd) {
    case CS_STOP:
      stop();
      break;
    case CS_START:
      start();
      break;
    case CS_RESUME:
      resume();
      break;
    case CS_MONITOROFF:
      turnOffMonitor();
      break;
    case CS_SHUTDOWN:
      shutdownSystem();
      break;
    case CS_REBOOT:
      rebootSystem();
      break;
    case CS_SETOWED:
      hdata = CCLC_ntohl(*(FXuint *) data);
      setOwed(hdata);
      break;
    case CS_SETADDITIONAL:
      hdata = CCLC_ntohl(*(FXuint *) data);
      setProducts(hdata);
      break;
    case CS_SETTIME:
      stime = time(NULL) - CCLC_ntohl(*((int *) data));
      break;
    case CS_SETTIMEOUT:
      timeout = CCLC_ntohl(*((int *) data));
      break;
    case CS_DISPLAYMESSAGE:
      showMessage((char *) data);
      break;
    case CS_QUITCLIENT:
      exitProgram();
      break;
    case CS_UNLOCKSCREEN:
      unlockScreen();
      break;
    case CS_LOCKSCREEN:
      lockScreen();
      break;
    case CS_ENABLEPASSWORDEDIT:
      clientwin->setPasswordEnabled(CCLC_ntohl((*((int *) data))));
      break;
    case CS_ALLOWMEMBERLOGIN:
      locker->allowMemberLogin(bool(CCLC_ntohl((*((int *) data)))));
      break;
    case CS_ALLOWUSERLOGIN:
      locker->allowUserLogin(bool(CCLC_ntohl((*((int *) data)))));
      break;
  }
}

long
CCLCFox::onTimer(FXObject*,FXSelector,void*)
{
  if (!networking)
    initNetworking();
  if (active) {
    char buf[8];
    int usedtime = time(NULL) - stime;
    int hours = usedtime / 3600;
    int mins = (usedtime % 3600) / 60;

    snprintf(buf,8,"%.2d:%.2d",hours,mins);
    clientwin->setTime(buf);
    CCLC_send_cmd(CC_GETOWED,NULL,0);
    if (0 < timeout && usedtime > timeout)
      userExit();
  }
  clientwin->getApp()->addTimeout(this,CCLCFox::ID_TIMER,5000);

  return 1;
}

long
CCLCFox::onCheckEvents(FXObject*,FXSelector,void*)
{
  CCLC_check_events();
  clientwin->getApp()->addTimeout(this,CCLCFox::ID_CHECKEVENTS,100);

  return 1;
}

void
CCLCFox::setPassword(FXuchar digest[2*CCLC_MD5_DIGEST_LENGTH])
{
  CCLC_send_cmd(CC_SETMEMBERPASSWORD,digest,2*CCLC_MD5_DIGEST_LENGTH);
}
