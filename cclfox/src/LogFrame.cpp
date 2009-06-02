#include <ccls.h>
#include <fox-1.6/fx.h>
#include <fox-1.6/FXRex.h>
using namespace FX;
using namespace std;

#include "cclfox.h"
#include "LogFrame.h"
#include "CashingFrame.h"
#include "CCLWin.h"
#include "verifiers.h"

static void printTicket(const char *description, unsigned int amount);
static void openCashRegister();

FXDEFMAP(LogFrame) LogFrameMap[] =
{
  FXMAPFUNC(SEL_COMMAND,LogFrame::ID_REFRESH,LogFrame::onRefresh),
  FXMAPFUNC(SEL_COMMAND,LogFrame::ID_CLEAR,LogFrame::onClear),
  FXMAPFUNC(SEL_COMMAND,LogFrame::ID_CHECKVALID,LogFrame::onCheckValid),
  FXMAPFUNC(SEL_COMMAND,LogFrame::ID_RESET,LogFrame::onReset),
  FXMAPFUNC(SEL_COMMAND,LogFrame::ID_SESSIONS,LogFrame::onSwitchToSessions),
  FXMAPFUNC(SEL_COMMAND,LogFrame::ID_PRODUCTS,LogFrame::onSwitchToProducts),
  FXMAPFUNC(SEL_COMMAND,LogFrame::ID_EXPENSES,LogFrame::onSwitchToExpenses),
  FXMAPFUNC(SEL_COMMAND,LogFrame::ID_LOGEXPENSE,LogFrame::onLogExpense),
  FXMAPFUNC(SEL_COMMAND,LogFrame::ID_SAVEONFILE,LogFrame::onSaveOnFile),
  FXMAPFUNC(SEL_VERIFY,LogFrame::ID_CHECKVALID,LogFrame::onVerify),
  FXMAPFUNC(SEL_SELECTED,LogFrame::ID_SESSIONLIST,LogFrame::onSessionSelect),
  FXMAPFUNC(SEL_COMMAND,LogFrame::ID_STARTCASH,LogFrame::onStartingCashChange)
};

FXIMPLEMENT(LogFrame,FXVerticalFrame,LogFrameMap,ARRAYNUMBER(LogFrameMap))

extern CCLWin *mainwin;
extern CashingFrame *cashingframe;

LogFrame::LogFrame(FXComposite * parent)
:FXVerticalFrame(parent,LAYOUT_FILL_X|LAYOUT_FILL_Y|FRAME_SUNKEN,
		 0,0,0,0,0,0,0,0,0,0)
{
  FXVerticalFrame *loglistframe =
    new FXVerticalFrame(this,LAYOUT_FILL_X|LAYOUT_FILL_Y|FRAME_SUNKEN,
			0,0,0,0,0,0,0,0,0,0);

  listswitcher = new FXSwitcher(loglistframe,LAYOUT_FILL_X|LAYOUT_FILL_Y,
				0,0,0,0,0,0,0,0);
  // Sessions
  sessionslist = new FXFoldingList(listswitcher,this,ID_SESSIONLIST,
				   FOLDINGLIST_SINGLESELECT|LAYOUT_FILL_X|
				   LAYOUT_FILL_Y);
  sessionslist->appendHeader(_("Client"),NULL,120);
  sessionslist->appendHeader(_("Start"),NULL,120);
  sessionslist->appendHeader(_("End"),NULL,120);
  sessionslist->appendHeader(_("Time"),NULL,55);
  sessionslist->appendHeader(_("Price"),NULL,55);
  sessionslist->appendHeader(_("Discount"),NULL,55);
  // Products
  productslist = new FXFoldingList(listswitcher,NULL,0,
				   FOLDINGLIST_SINGLESELECT|LAYOUT_FILL_X|
				   LAYOUT_FILL_Y);
  productslist->appendHeader(_("Product"),NULL,140);
  productslist->appendHeader(_("Client"),NULL,115);
  productslist->appendHeader(_("Date"),NULL,120);
  productslist->appendHeader(_("Amount"),NULL,60);
  productslist->appendHeader(_("Price"),NULL,55);
  productslist->appendHeader(_("Discount"),NULL,55);
  // Expenses
  expenseslist = new FXFoldingList(listswitcher,NULL,0,
				   FOLDINGLIST_SINGLESELECT|LAYOUT_FILL_X|
				   LAYOUT_FILL_Y);
  expenseslist->appendHeader(_("Description"),NULL,260);
  expenseslist->appendHeader(_("Date"),NULL,120);
  expenseslist->appendHeader(_("Spent"),NULL,60);

  FXHorizontalFrame *hframe10 =
    new FXHorizontalFrame(this,LAYOUT_FILL_X,0,0,0,0,0,0,0,0,0,0);
  resetbtn = new FXButton(hframe10,_("Reset"),NULL,this,ID_RESET,
			  BUTTON_NORMAL|LAYOUT_CENTER_X|LAYOUT_FILL_X);
  clearbtn = new FXButton(hframe10,_("Clear"),NULL,this,ID_CLEAR,
			  BUTTON_NORMAL|LAYOUT_CENTER_X|LAYOUT_FILL_X);
  refreshbtn = new FXButton(hframe10,_("Refresh"),NULL,this,ID_REFRESH,
			    BUTTON_NORMAL|LAYOUT_CENTER_X|LAYOUT_FILL_X);
  logexpensebtn = new FXButton(hframe10,_("Log expense"),NULL,this,
			       ID_LOGEXPENSE,BUTTON_NORMAL|LAYOUT_CENTER_X|
			       LAYOUT_FILL_X);
  saveonfilebtn = new FXButton(hframe10,_("Save on file"),NULL,this,
			       ID_SAVEONFILE,BUTTON_NORMAL|LAYOUT_CENTER_X|
			       LAYOUT_FILL_X);

  FXHorizontalFrame *hframe1 = new FXHorizontalFrame(this,LAYOUT_FILL_X);
  FXVerticalFrame *vframe1 =
    new FXVerticalFrame(hframe1,LAYOUT_FILL_X|LAYOUT_FILL_Y|FRAME_GROOVE,
			0,0,0,0,0,0,0,0,0,0);
  FXVerticalFrame *vframe2 =
    new FXVerticalFrame(hframe1,LAYOUT_FILL_X|LAYOUT_FILL_Y|FRAME_GROOVE,
			0,0,0,0,0,0,0,0,0,0);
  FXHorizontalFrame *hframe2 = new FXHorizontalFrame(vframe1,LAYOUT_FILL_X);
  new FXLabel(hframe2,_("From:"));

  stimetf = new FXTextField(hframe2,5,this,ID_CHECKVALID,
			    TEXTFIELD_NORMAL|TEXTFIELD_LIMITED);
  new FXLabel(hframe2,_("of"));

  sdatetf = new FXTextField(hframe2,8,this,ID_CHECKVALID,
			    TEXTFIELD_NORMAL|TEXTFIELD_LIMITED);
  membertf = new FXTextField(hframe2,6,NULL,0,
			     LAYOUT_RIGHT|TEXTFIELD_NORMAL|TEXTFIELD_INTEGER);
  new FXLabel(hframe2,_("Member:"),NULL,LABEL_NORMAL|LAYOUT_RIGHT);
  FXHorizontalFrame *hframe3 = new FXHorizontalFrame(vframe1,LAYOUT_FILL_X);
  new FXLabel(hframe3,_("To:"));

  etimetf = new FXTextField(hframe3,5,this,ID_CHECKVALID,
			    TEXTFIELD_NORMAL|TEXTFIELD_LIMITED);
  new FXLabel(hframe3,_("of"));

  edatetf = new FXTextField(hframe3,8,this,ID_CHECKVALID,
			    TEXTFIELD_NORMAL|TEXTFIELD_LIMITED);
  FXHorizontalFrame *hframe4 = new FXHorizontalFrame(vframe1,LAYOUT_FILL_X);
  new FXLabel(hframe4,_("Days:"));

  daybtn[0] = new FXToggleButton(hframe4,_("Sun"),_("Sun"),NULL,NULL,this,
				 ID_CHECKVALID, TOGGLEBUTTON_NORMAL|
				 TOGGLEBUTTON_KEEPSTATE);
  daybtn[1] =
    new FXToggleButton(hframe4,_("Mon"),_("Mon"),NULL,NULL,this,ID_CHECKVALID,
		       TOGGLEBUTTON_NORMAL|TOGGLEBUTTON_KEEPSTATE);
  daybtn[2] =
    new FXToggleButton(hframe4,_("Tue"),_("Tue"),NULL,NULL,this,ID_CHECKVALID,
		       TOGGLEBUTTON_NORMAL|TOGGLEBUTTON_KEEPSTATE);
  daybtn[3] =
    new FXToggleButton(hframe4,_("Wed"),_("Wed"),NULL,NULL,this,ID_CHECKVALID,
		       TOGGLEBUTTON_NORMAL|TOGGLEBUTTON_KEEPSTATE);
  daybtn[4] =
    new FXToggleButton(hframe4,_("Thu"),_("Thu"),NULL,NULL,this,ID_CHECKVALID,
		       TOGGLEBUTTON_NORMAL|TOGGLEBUTTON_KEEPSTATE);
  daybtn[5] =
    new FXToggleButton(hframe4,_("Fri"),_("Fri"),NULL,NULL,this,ID_CHECKVALID,
		       TOGGLEBUTTON_NORMAL|TOGGLEBUTTON_KEEPSTATE);
  daybtn[6] =
    new FXToggleButton(hframe4,_("Sat"),_("Sat"),NULL,NULL,this,ID_CHECKVALID,
		       TOGGLEBUTTON_NORMAL|TOGGLEBUTTON_KEEPSTATE);
  FXHorizontalFrame *hframe5 = new FXHorizontalFrame(vframe1,LAYOUT_FILL_X);
  new FXLabel(hframe5,_("Between:"));

  strangetf = new FXTextField(hframe5,5,this,ID_CHECKVALID,
			      TEXTFIELD_NORMAL|TEXTFIELD_LIMITED);
  new FXLabel(hframe5,_("and"));

  etrangetf = new FXTextField(hframe5,5,this,ID_CHECKVALID,
			      TEXTFIELD_NORMAL|TEXTFIELD_LIMITED);
  canceledcheck = new FXCheckButton(hframe5,_("Canceled"),NULL,0,
				    CHECKBUTTON_NORMAL|LAYOUT_RIGHT);
  FXHorizontalFrame *hframe6 =
    new FXHorizontalFrame(vframe2,LAYOUT_FILL_X,0,0,0,0,2,2,2,2);
  new FXLabel(hframe6,_("Total:"));
  totallbl = new FXLabel(hframe6,"0.00");
  FXHorizontalFrame *hframe7 =
    new FXHorizontalFrame(vframe2,LAYOUT_FILL_X,0,0,0,0,2,2,2,2);
  new FXButton(hframe7,_("Sessions:"),NULL,this,ID_SESSIONS,FRAME_RAISED);
  stotallbl = new FXLabel(hframe7,"0.00");
  FXHorizontalFrame *hframe8 =
    new FXHorizontalFrame(vframe2,LAYOUT_FILL_X,0,0,0,0,2,2,2,2);
  new FXButton(hframe8,_("Products:"),NULL,this,ID_PRODUCTS,FRAME_RAISED);
  ptotallbl = new FXLabel(hframe8,"0.00");
  FXHorizontalFrame *hframe9 =
    new FXHorizontalFrame(vframe2,LAYOUT_FILL_X,0,0,0,0,2,2,2,2);
  new FXButton(hframe9,_("Expenses:"),NULL,this,ID_EXPENSES,FRAME_RAISED);
  etotallbl = new FXLabel(hframe9,"0.00");
  FXHorizontalFrame *hframe11 =
    new FXHorizontalFrame(vframe2,LAYOUT_FILL_X,0,0,0,0,2,2,2,2);
  new FXLabel(hframe11,_("Time:"));
  ttotallbl = new FXLabel(hframe11,"00:00");
  FXHorizontalFrame *hframe12 =
    new FXHorizontalFrame(vframe2,LAYOUT_FILL_X,0,0,0,0,2,2,2,2);
  new FXLabel(hframe12,_("Start:"));
  startcashsp = new FXRealSpinner(hframe12,6,this,ID_STARTCASH,
				  FRAME_SUNKEN|FRAME_THICK);
  startcashsp->setRange(0,9999999);
  startcashsp->setValue(CCL_data_get_int(CCL_DATA_NONE,0,
					 "report/starting_cash",0) / 100.0);

  reset();
}

LogFrame::~LogFrame()
{
}

void
LogFrame::create()
{
  FXVerticalFrame::create();
}

void
LogFrame::reset()
{
  stimetf->setText("00:00");
  etimetf->setText("00:00");
  strangetf->setText("00:00");
  etrangetf->setText("23:59");
  sdatetf->setText("01/01/04");
  edatetf->setText("01/01/37");
  membertf->setText("");
  for (int i = 0; i < 7; i++)
    daybtn[i]->setState(TRUE);
  refreshbtn->enable();
}

void
LogFrame::clear()
{
  stotal = ptotal = etotal = 0.0;
  sessionslist->clearItems();
  productslist->clearItems();
  expenseslist->clearItems();
  totallbl->setText("0.00");
  ptotallbl->setText("0.00");
  stotallbl->setText("0.00");
  etotallbl->setText("0.00");
  ttotallbl->setText("00:00");
}

void
LogFrame::readLog()
{
  FXuint ttotal = 0;
  FXuint strangehr;
  FXuint strangemin;
  FXuint etrangehr;
  FXuint etrangemin;
  int member = 0;
  FXuint days;
  CCL_log_search_rules sr;
  CCL_log_session_entry *se = NULL;
  int num = 0;
  struct tm st;
  struct tm et;

  stotal = ptotal = etotal = 0.0;

  memset(&sr,0,sizeof(CCL_log_search_rules));
  memset(&st,0,sizeof(struct tm));
  memset(&et,0,sizeof(struct tm));

  sscanf(stimetf->getText().text(),"%d:%d",&st.tm_hour,&st.tm_min);
  sscanf(sdatetf->getText().text(),"%d/%d/%d",&st.tm_mday,&st.tm_mon,
	 &st.tm_year);
  sscanf(strangetf->getText().text(),"%d:%d",&strangehr,&strangemin);
  st.tm_year += 100;
  st.tm_mon -= 1;
  sscanf(etrangetf->getText().text(),"%d:%d",&etrangehr,&etrangemin);
  sscanf(etimetf->getText().text(),"%d:%d",&et.tm_hour,&et.tm_min);
  sscanf(edatetf->getText().text(),"%d/%d/%d",&et.tm_mday,&et.tm_mon,
	 &et.tm_year);
  et.tm_mon -= 1;
  et.tm_year += 100;
  sscanf(membertf->getText().text(),"%d",&member);
  if (member < 1)
    member = 0;

  for (int i = 0; i < 7; i++)
    if (daybtn[i]->getState())
      sr.days |= (1 << i);

  sr.etime_min = mktime(&st);
  sr.etime_max = mktime(&et);

  if (-1 == sr.etime_min) {
    FXMessageBox::error(this,MBOX_OK,_("Error"),_("Invalid start date"));
    return;
  } else if (-1 == sr.etime_max) {
    FXMessageBox::error(this,MBOX_OK,_("Error"),_("Invalid end date"));
    return;
  } else if (sr.etime_min > sr.etime_max) {
    FXMessageBox::error(this,MBOX_OK,_("Error"),
			_("Start date greater than end date"));
    return;
  }

  sr.daytime_min = strangehr * 60 * 60 + strangemin * 60;
  sr.daytime_max = etrangehr * 60 * 60 + etrangemin * 60 + 59;
  if (canceledcheck->getCheck())
    sr.flags = CANCELED;
  else
    sr.flags = PAID;
  sr.rulemask = CCL_SR_ETIMEMIN|CCL_SR_ETIMEMAX|CCL_SR_DAYTIME_RANGE
		|CCL_SR_DAYS|CCL_SR_FLAGS;
  sr.member = member;
  if (member > 0)
    sr.rulemask |= CCL_SR_MEMBER;
 
  num = CCL_log_sessions_get(&sr,&se);

  // Sessions
  for (int i = 0; i < num; i++) {
    char entry[512];
    char ststr[64];
    char etstr[64];
    unsigned int discount = 0;
    const char *cname = CCL_client_name_get(se[i].client);
    const char *mname = CCL_member_exists(se[i].member)
			  ? CCL_member_name_get(se[i].member) : "";

    if (!mname) mname = "";
    if (se[i].flags & WITH_DISCOUNT)
      discount = CCL_data_get_int(CCL_DATA_LOGSESSION, se[i].id,
				  "discount", 0);

    strftime(ststr,64,"%H:%M  %d/%m/%Y",localtime(&(se[i].stime)));
    strftime(etstr,64,"%H:%M  %d/%m/%Y",localtime(&(se[i].etime)));
    snprintf(entry,512,"%s:%s\t%s\t%s\t%.2d:%.2d\t%.2f\t%.2f",
	     cname,mname,ststr,etstr,
	     se[i].time / 3600,(se[i].time % 3600) / 60,se[i].price / 100.0,
	     discount / 100.0);
    sessionslist->prependItem(NULL,entry,NULL,NULL,
			       (void *)(se[i].id));
    stotal += se[i].price / 100.0;
    ttotal += se[i].time / 60;
  }

  // Products
  CCL_log_product_entry *pe = NULL;

  sr.time_min = sr.etime_min;
  sr.time_max = sr.etime_max;
  sr.rulemask = CCL_SR_TIMEMIN|CCL_SR_TIMEMAX|CCL_SR_DAYTIME_RANGE|CCL_SR_DAYS;
  if (member > 0) 
    sr.rulemask |= CCL_SR_MEMBER;

  num = CCL_log_products_get(&sr,&pe);

  for (int i = 0; i < num; i++) {
    char entry[512];
    char tstr[64];
    unsigned int discount = 0;
    char *pname = NULL;
    const char *cname = CCL_client_exists(pe[i].client)
			  ? CCL_client_name_get(pe[i].client) : "";
    const char *mname = CCL_member_exists(pe[i].member)
			  ? CCL_member_name_get(pe[i].member) : "";

    CCL_product_info_get(pe[i].product,NULL,&pname,NULL);
    if (!cname)  cname = "-";
    if (pe[i].flags & WITH_DISCOUNT)
      discount = CCL_data_get_int(CCL_DATA_LOGPRODUCT, pe[i].id,
				  "discount", 0);

    strftime(tstr,64,"%H:%M  %d/%m/%Y",localtime(&(pe[i].time)));
    snprintf(entry,512,"%s\t%s:%s\t%s\t%u\t%.2f\t%.2f",
	     pname,cname,mname,tstr,pe[i].amount,pe[i].price / 100.0,
	     discount / 100.0);
    productslist->prependItem(NULL,entry,NULL,NULL,NULL);

    ptotal += pe[i].price / 100.0;
    CCL_free(pname);
  }

  // Expenses
  CCL_log_expense_entry *ee = NULL;

  sr.time_min = sr.etime_min;
  sr.time_max = sr.etime_max;
  sr.rulemask = CCL_SR_TIMEMIN|CCL_SR_TIMEMAX|CCL_SR_DAYS|CCL_SR_DAYTIME_RANGE;
  if (member > 0) 
    sr.rulemask |= CCL_SR_MEMBER;

  num = CCL_log_expenses_get(&sr,&ee);

  for (int i = 0; i < num; i++) {
    char entry[512];
    char tstr[64];

    strftime(tstr,64,"%H:%M  %d/%m/%Y",localtime(&(ee[i].time)));
    snprintf(entry,512,"%s\t%s\t%.2f",
	     ee[i].description,tstr,ee[i].cash / 100.0);
    expenseslist->prependItem(NULL,entry,NULL,NULL,NULL);

    etotal += ee[i].cash / 100.0;
  }

  char buf[64];

  snprintf(buf,64,"%.2f",stotal);
  stotallbl->setText(buf);
  snprintf(buf,64,"%.2f",ptotal);
  ptotallbl->setText(buf);
  snprintf(buf,64,"%.2f",etotal);
  etotallbl->setText(buf);
  snprintf(buf,64,"%ud %.2u:%.2u",ttotal / 1440,(ttotal % 1440) / 60,
	   ttotal % 60);
  ttotallbl->setText(buf);
  snprintf(buf,64,"%.2f",startcashsp->getValue() + stotal + ptotal - etotal);
  totallbl->setText(buf);

  CCL_free(ee);
  CCL_free(pe);
  CCL_free(se);
}

bool
LogFrame::saveLog(const char *filename)
{
  FXString output = "";
  FXFoldingList *lists[] = { sessionslist, productslist, expenseslist, NULL };
  char buf[64];

  snprintf(buf,64,"%.2f",startcashsp->getValue());

  output += stimetf->getText() + " " + sdatetf->getText() + "\n"
	  + etimetf->getText() + " " + edatetf->getText() + "\n"
	  + totallbl->getText() + "\n"
	  + stotallbl->getText() + "\n"
	  + ptotallbl->getText() + "\n"
	  + etotallbl->getText() + "\n"
	  + ttotallbl->getText() + "\n" +
	  + buf + "\n\n";
  
  for (FXFoldingList **list = lists; NULL != *list; list++) {
    for (FXFoldingItem *i = (*list)->getFirstItem(); NULL != i;
	 i = i->getNext()) {
      output += i->getText();
      output += "\n";
    }
    output += "\n";
  }
  
  FILE *out = fopen(filename,"w");

  if (out) {
    fwrite(output.text(),sizeof(char),output.length(),out);
    fclose(out);

    return TRUE;
  } else
    return FALSE;
}

long
LogFrame::onRefresh(FXObject*,FXSelector,void*)
{
  clear();
  readLog();

  return 1;
}

long
LogFrame::onClear(FXObject*,FXSelector,void*)
{
  clear();

  return 1;
}

long
LogFrame::onCheckValid(FXObject*,FXSelector,void*)
{
  FXRex time("\\A[0-2]?[0-9]:[0-5][0-9]\\Z",REX_NORMAL);
  FXRex date("\\A([1-3][0-9]|0?[1-9])/[0-1]?[0-9]/([0-2][0-9]|3[0-7])\\Z",
	     REX_NORMAL);
  FXbool dayset = FALSE;

  for (int i = 0; i < 7; i++)
    if (daybtn[i]->getState())
      dayset = TRUE;

  if (time.match(stimetf->getText()) && date.match(sdatetf->getText())
      && time.match(etimetf->getText()) && date.match(edatetf->getText())
      && time.match(strangetf->getText()) && time.match(etrangetf->getText())
      && dayset)
    refreshbtn->enable();
  else
    refreshbtn->disable();

  return 1;
}

long
LogFrame::onReset(FXObject*,FXSelector,void*)
{
  reset();

  return 1;
}

long
LogFrame::onVerify(FXObject* sender,FXSelector,void* ptr)
{
  if (sender == stimetf || sender == etimetf || sender == strangetf
      || sender == etrangetf)
    return !isTime((char *)ptr);
  else if (sender == sdatetf || sender == edatetf)
    return !isDate((char *)ptr);

  return 0;
}

long
LogFrame::onSessionSelect(FXObject*,FXSelector,void*)
{
  FXFoldingItem *current = sessionslist->getCurrentItem();
  int session = (int) current->getData();

  cashingframe->setSession(session,FALSE);
  mainwin->showCashing();

  return 1;
}

long
LogFrame::onSwitchToSessions(FXObject*,FXSelector,void*)
{
  listswitcher->setCurrent(0);

  return 1;
}

long
LogFrame::onSwitchToProducts(FXObject*,FXSelector,void*)
{
  listswitcher->setCurrent(1);

  return 1;
}

long
LogFrame::onSwitchToExpenses(FXObject*,FXSelector,void*)
{
  listswitcher->setCurrent(2);

  return 1;
}

long
LogFrame::onLogExpense(FXObject*,FXSelector,void*)
{
  FXString description = "";
  double cash = 0.0;

  if (FXInputDialog::getString(description,this,_("Description"),
			       _("Description:"))
      && description.length()
      && FXInputDialog::getReal(cash,this,_("Amount"),_("Amount:"))
      && cash > 0.0) {
    CCL_log_expense(description.text(),(FXuint) (cash * 100),0);
    printTicket(description.text(),(FXuint) (cash * 100));
    openCashRegister();
  }

  return 1;
}

long
LogFrame::onSaveOnFile(FXObject*,FXSelector,void*)
{
  FXString cwd = FXSystem::getCurrentDirectory() + "/";
  char *logpath = CCL_data_get_string(CCL_DATA_NONE,0,"logs/path",cwd.text());
  FXString input = logpath;

  CCL_free(logpath);

  if (FXInputDialog::getString(input,this,_("File name"),_("File name:"))
      && FXPath::name(input).length()) {
    FXString filename = input + ".log";
    FXString directory = FXPath::directory(input) + "/";

    if (!FXStat::exists(filename)
	|| FXMessageBox::question(this,MBOX_YES_NO,_("File exists"),
				  _("File '%s' exists, overwrite?"),
				  filename.text()) == MBOX_CLICKED_YES) {
      if (saveLog(filename.text())) {
	FXMessageBox::information(this,MBOX_OK,_("Log file saved"),
				  _("Log file saved"));
	CCL_data_set_string(CCL_DATA_NONE,0,"logs/path",directory.text());
      }
      else
	FXMessageBox::error(this,MBOX_OK,_("Log file not saved"),
			    _("Log file not saved"));

    }
  }

  return 1;
}

long
LogFrame::onStartingCashChange(FXObject*,FXSelector,void *ptr)
{
  double value = *(double*)ptr;
  char buf[64];

  CCL_data_set_int(CCL_DATA_NONE,0,"report/starting_cash",(int)(value*100));

  snprintf(buf,64,"%.2f",value + stotal + ptotal - etotal);
  totallbl->setText(buf);

  return 1;
}

static void
printTicket(const char *description, unsigned int amount)
{
  char buf[256];
  int ticketnum = 1 + CCL_data_get_int(CCL_DATA_NONE,0,
				       "ticket/number",0);
//  int last_day = CCL_data_get_int(CCL_DATA_NONE,0,
//				  "ticket/last_day",-1);
  time_t t = time(NULL);
  struct tm *tm = localtime(&t);

//  if (-1 == last_day || tm->tm_yday != last_day) {
//    ticketnum = 1;
//    CCL_data_set_int(CCL_DATA_NONE,0,"ticket/last_day",tm->tm_yday);
//  }

  FXString tickettext = "";

  snprintf(buf,sizeof(buf)/sizeof(char),"%05d",ticketnum);

  tickettext += _("Ticket number:");
  tickettext += " ";
  tickettext += buf;
  tickettext += "\n";

  strftime(buf,sizeof(buf)/sizeof(char),"%T %d/%m/%Y",tm);

  tickettext += _("Date:");
  tickettext += " ";
  tickettext += buf;
  tickettext += "\n\n";

  tickettext += _("Description:");
  tickettext += " ";
  tickettext += description;
  tickettext += "\n\n";

  snprintf(buf,64,"$ %.2f",amount/100.0);

  tickettext += _("Cost:");
  tickettext += " ";
  tickettext += buf;
  tickettext += "\n\n\n\n\n";

#ifdef WIN32
  FILE *p = fopen("PRN","w");
  fwrite(tickettext.text(),sizeof(char),tickettext.length(),p);
  fclose(p);
#else
  FXString command = "lpr ";
  FXString filename = FXPath::unique("__ticket.txt");
  FILE *p = fopen(filename.text(),"w");
  
  command += filename;
  fwrite(tickettext.text(),sizeof(char),tickettext.length(),p);
  fclose(p);
  system(command.text());
//    FXFile::remove(filename);
#endif
  
  CCL_data_set_int(CCL_DATA_NONE,0,"ticket/number",ticketnum);
}

static void
openCashRegister()
{
#ifdef WIN32
  FILE *p = fopen("PRN","w");
  char code = 0x07;

  fwrite(&code,sizeof(code),1,p);
  fclose(p);
#endif
}
