#ifndef MEMBERSFRAME_H
#define MEMBERSFRAME_H

class MembersFrame : public FXVerticalFrame
{
FXDECLARE(MembersFrame)
protected:
  FXTextField	 *filtertf;
  FXButton	 *settarif;
  FXButton	 *resetpass;
  FXButton	 *addmember;
  FXButton	 *delmember;
  FXButton	 *applychanges;
  FXFoldingList  *memberslist;
  FXTextField	 *nametf;
  FXTextField	 *emailtf;
  FXTextField	 *phonetf;
  FXTextField	 *logintf;
private:
  int		  editedmember;
  int		  tarifset;
protected:
  MembersFrame(){}
public:
  MembersFrame(FXComposite *parent);
  ~MembersFrame();
  void create();
public:
  void readAllMembers(const char * filter = NULL);
  void readMember(int id);
  void clear();
public:
  long onAddMember(FXObject*,FXSelector,void*);
  long onDelMember(FXObject*,FXSelector,void*);
  long onApplyChanges(FXObject*,FXSelector,void*);
  long onMemberSelect(FXObject*,FXSelector,void*);
  long onSetTarif(FXObject*,FXSelector,void*);
  long onFilter(FXObject*,FXSelector,void* ptr);
  long onResetPass(FXObject*,FXSelector,void*);
public:
  enum {
    ID_ADDMEMBER = FXVerticalFrame::ID_LAST,ID_DELMEMBER,ID_APPLY,
    ID_CHECKVALID,ID_MEMBERSLIST,ID_SETTARIF,ID_FILTER,ID_RESETPASS,
    ID_LAST
  };
};
#endif  
