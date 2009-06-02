#ifndef LOGFRAME_H
#define LOGFRAME_H

class LogFrame : public FXVerticalFrame
{
FXDECLARE(LogFrame)
protected:
  FXSwitcher	  *listswitcher;
  FXFoldingList	  *sessionslist;
  FXFoldingList	  *productslist;
  FXFoldingList	  *expenseslist;
  FXTextField	  *stimetf;
  FXTextField	  *etimetf;
  FXTextField	  *sdatetf;
  FXTextField	  *edatetf;
  FXTextField	  *membertf;
  FXToggleButton  *daybtn[7];
  FXTextField	  *strangetf;
  FXTextField	  *etrangetf;
  FXCheckButton	  *canceledcheck;
  FXButton	  *clearbtn;
  FXButton	  *refreshbtn;
  FXButton	  *resetbtn;
  FXButton	  *logexpensebtn;
  FXButton	  *saveonfilebtn;
  FXLabel	  *totallbl;
  FXLabel	  *ptotallbl;
  FXLabel	  *stotallbl;
  FXLabel	  *etotallbl;
  FXLabel	  *ttotallbl;
  FXRealSpinner	  *startcashsp;
protected:
  double	   stotal;
  double	   ptotal;
  double	   etotal;
protected:
  LogFrame(){}
public:
  LogFrame(FXComposite *parent);
  ~LogFrame();
  void create();
public:
  void reset();
  void clear();
  void readLog();
  bool saveLog(const char *filename);
public:
  long onRefresh(FXObject*,FXSelector,void*);
  long onClear(FXObject*,FXSelector,void*);
  long onCheckValid(FXObject*,FXSelector,void*);
  long onReset(FXObject*,FXSelector,void*);
  long onVerify(FXObject* sender,FXSelector,void* ptr);
  long onSessionSelect(FXObject*,FXSelector,void*);
  long onSwitchToSessions(FXObject*,FXSelector,void*);
  long onSwitchToProducts(FXObject*,FXSelector,void*);
  long onSwitchToExpenses(FXObject*,FXSelector,void*);
  long onLogExpense(FXObject*,FXSelector,void*);
  long onSaveOnFile(FXObject*,FXSelector,void*);
  long onStartingCashChange(FXObject*,FXSelector,void* ptr);
public:
  enum {
    ID_REFRESH = FXVerticalFrame::ID_LAST,ID_CLEAR,ID_EXPENSES,
    ID_CHECKVALID,ID_RESET,ID_SESSIONLIST,ID_SESSIONS,ID_PRODUCTS,
    ID_LOGEXPENSE,ID_SAVEONFILE,ID_STARTCASH,
    ID_LAST
  };
};
#endif
