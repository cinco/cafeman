#include <signal.h>
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
#include "CCLWin.h"

CCLWin *mainwin;
ProductsFrame *productsframe;
CashingFrame *cashingframe;
TarifFrame *tarifframe;
NotpaidFrame *notpaidframe;
LogFrame *logframe;
MembersFrame *membersframe;

static void sigpipe_handler(int n) {}

static void
show_help(const char *appname)
{
  fxmessage(_(
"Usage: %s [options]\n"
"Options:\n"
"\t-port <port>: Listen on the specified port (default: 2999)\n"
"\t-nossl: do not use SSL to connect (default: use SSL)\n"
"\t-certpass <password>: password used for the cerfificate (default: none)\n"
"\t\t\t       Ignored if not using SSL (-nossl option enabled).\n"),appname);
}

static FXbool
parse_args(int argc,char *argv[])
{
  FXbool withssl = TRUE;
  char * certpass = NULL;
  int port = 2999;

  int i = 1;

  while (i < argc) {
    if (!strcmp(argv[i],"-port") && ++i < argc)
      port = atoi(argv[i]);
    else if (!strcmp(argv[i],"-nossl"))
      withssl = FALSE;
    else if (!strcmp(argv[i],"-certpass") && ++i < argc)
      certpass = argv[i];
    else {
      fxmessage(_("[E]Invalid option %s\n"),argv[i]);
      return FALSE;
    }
    ++i;
  }

  if (withssl) {
    if (!FXStat::exists("cert.pem"))
      fxerror(_("[E]File \"cert.pem\" not found!!\n"));
    if (!FXStat::exists("CA.pem"))
      fxerror(_("[E]File \"CA.pem\" not found!!\n"));
   
    int error;
    if (!CCL_SSL_init("CA.pem","cert.pem",certpass,&error)) {
      switch (error) {
      case CCL_ERROR_BAD_PASSWORD:
	fxerror(_("[E]Bad certificate password given\n"));
	return 1;
      }
    }
  }

  int error;
  if (!CCL_networking_init(port,&error))
    fxmessage(_("[!]Networking not initialized\n"));
 
  return TRUE;
}

int
main(int argc,char *argv[])
{
#ifdef SIGPIPE
  signal(SIGPIPE, sigpipe_handler);
#endif
#ifndef WIN32
  if (!FXStat::exists(FXSystem::getHomeDirectory() + "/.cclfox/"))
    FXDir::create(FXSystem::getHomeDirectory() + "/.cclfox/", 0755);
  FXSystem::setCurrentDirectory(FXSystem::getHomeDirectory() + "/.cclfox/");
#endif
  // Gettext
  //
#ifdef HAVE_GETTEXT
  setlocale(LC_MESSAGES,"");
  textdomain("cclfox");
# ifdef WIN32
  bindtextdomain("cclfox","./locale");
# endif
#endif
  // Init ccl 
  CCL_init("cclfox.db");
  if (-1 == CCL_tarif_get_nth(0))
    CCL_tarif_new(0,0,127,0);
  CCL_tarif_rebuild_all();

  if (!parse_args(argc,argv)) {
    show_help(argv[0]);
    return 1;
  }

  // Start the GUI
  FXApp app("CCLFox","CafeConLeche");

  app.init(argc,argv);
  mainwin = new CCLWin(&app);
  app.create();
  CCL_perminafter_set(CCL_data_get_int(CCL_DATA_NONE,0,"tarif/perminafter",60));
  CCL_tarif_set(CCL_data_get_int(CCL_DATA_NONE,0,"tarif/default",1));
  
  mainwin->loadClients();
  productsframe->loadProducts();
  tarifframe->readTarif();
  notpaidframe->readNotPaid();
  membersframe->readAllMembers();

  int exitcode = app.run();

  mainwin = NULL;
  CCL_networking_shutdown();
  CCL_shutdown();

  return exitcode;
}
