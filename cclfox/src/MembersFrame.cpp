#include <ccls.h>
#include <fox-1.6/fx.h>
#include <fox-1.6/FXRex.h>
using namespace FX;

#include "cclfox.h"
#include "MembersFrame.h"

FXDEFMAP(MembersFrame) MembersFrameMap[] =
{
  FXMAPFUNC(SEL_COMMAND,MembersFrame::ID_ADDMEMBER,MembersFrame::onAddMember),
  FXMAPFUNC(SEL_COMMAND,MembersFrame::ID_DELMEMBER,MembersFrame::onDelMember),
  FXMAPFUNC(SEL_COMMAND,MembersFrame::ID_APPLY,MembersFrame::onApplyChanges),
  FXMAPFUNC(SEL_SELECTED,MembersFrame::ID_MEMBERSLIST,
	    MembersFrame::onMemberSelect),
  FXMAPFUNC(SEL_COMMAND,MembersFrame::ID_SETTARIF,MembersFrame::onSetTarif),
  FXMAPFUNC(SEL_COMMAND,MembersFrame::ID_RESETPASS,MembersFrame::onResetPass),
  FXMAPFUNC(SEL_CHANGED,MembersFrame::ID_FILTER,MembersFrame::onFilter)
};

FXIMPLEMENT(MembersFrame,FXVerticalFrame,MembersFrameMap,
	    ARRAYNUMBER(MembersFrameMap))

#define MEMBER_DELETED	(1<<16)	// This member was deleted

extern FXSettings *passwords;
  
MembersFrame::MembersFrame(FXComposite * parent)
:FXVerticalFrame(parent,LAYOUT_FILL_X|LAYOUT_FILL_Y|FRAME_SUNKEN,
		0,0,0,0,0,0,0,0,0,0)
{
  FXVerticalFrame *membersframe =
    new FXVerticalFrame(this,FRAME_SUNKEN|LAYOUT_FILL_X|LAYOUT_FILL_Y,
			0,0,0,0,0,0,0,0);
  memberslist =
    new FXFoldingList(membersframe,this,ID_MEMBERSLIST,
		      FOLDINGLIST_SINGLESELECT|LAYOUT_FILL_X|LAYOUT_FILL_Y);
  memberslist->appendHeader(_("ID"),NULL,30);
  memberslist->appendHeader(_("Member name"),NULL,200);
  FXHorizontalFrame *hframe0 = new FXHorizontalFrame(this,LAYOUT_FILL_X);
  filtertf = new FXTextField(hframe0,40,this,ID_FILTER,TEXTFIELD_NORMAL);
  FXHorizontalFrame *hframe1 = new FXHorizontalFrame(this,LAYOUT_FILL_X);
  addmember = new FXButton(hframe1,_("New"),NULL,this,ID_ADDMEMBER,
			   FRAME_RAISED|FRAME_THICK);
  delmember = new FXButton(hframe1,_("Delete"),NULL,this,ID_DELMEMBER,
			   FRAME_RAISED|FRAME_THICK);
  applychanges = new FXButton(hframe1,_("Apply changes"),NULL,this,
			      ID_APPLY,FRAME_RAISED|FRAME_THICK);
  new FXHorizontalSeparator(this);
  new FXLabel(this,_("Edition:"),NULL,LAYOUT_CENTER_X);
  new FXHorizontalSeparator(this);
  FXHorizontalFrame *hframe2 = new FXHorizontalFrame(this,LAYOUT_FILL_X);
  new FXLabel(hframe2,_("Name:"));
  nametf = new FXTextField(hframe2,30,NULL,0,TEXTFIELD_NORMAL);
  FXHorizontalFrame *hframe3 = new FXHorizontalFrame(this,LAYOUT_FILL_X);
  new FXLabel(hframe3,_("E-Mail:"));
  emailtf = new FXTextField(hframe3,30,NULL,0,TEXTFIELD_NORMAL);
  FXHorizontalFrame *hframe4 = new FXHorizontalFrame(this,LAYOUT_FILL_X);
  new FXLabel(hframe4,_("Phone number:"));
  phonetf = new FXTextField(hframe4,9,NULL,0,TEXTFIELD_NORMAL);
  FXHorizontalFrame *hframe5 = new FXHorizontalFrame(this,LAYOUT_FILL_X);
  new FXLabel(hframe5,_("Login name:"));
  logintf = new FXTextField(hframe5,15,NULL,0,TEXTFIELD_NORMAL);
  FXHorizontalFrame *hframe6 = new FXHorizontalFrame(this,LAYOUT_FILL_X);
  settarif = new FXButton(hframe6,_("Set tarif"),NULL,this,ID_SETTARIF,
			  FRAME_RAISED|FRAME_THICK);
  resetpass = new FXButton(hframe6,_("Reset password"),NULL,this,ID_RESETPASS,
			   FRAME_RAISED|FRAME_THICK);
  editedmember = 0;
  tarifset = 0;
  clear();
}

MembersFrame::~MembersFrame()
{
}

void
MembersFrame::create()
{
  FXVerticalFrame::create();
}

void
MembersFrame::readAllMembers(const char * filter)
{
  int id;
  const char *name;
  char buf[128];
  FXString regexp = ".*";
  
  if (filter)
    regexp = regexp + filter + ".*";
  FXRex rex(regexp,REX_ICASE);

  memberslist->clearItems();
  for (FXuint i = 0; -1 != (id = CCL_member_get_nth(i)); i++) {
    if (!(CCL_member_flags_get(id) & MEMBER_DELETED)) {
      name = CCL_member_name_get(id);
      if (rex.match(name)) {
	snprintf(buf,128,"%d\t%s",id,name);
	memberslist->appendItem(NULL,buf,NULL,NULL,(void *)id);
      }
    }
  }
}

void
MembersFrame::readMember(int id)
{
  if (!CCL_member_exists(id))
    return;

  editedmember = id;
  nametf->setText(CCL_member_name_get(id));
  emailtf->setText(CCL_member_email_get(id));
  phonetf->setText(CCL_member_other_get(id));

  char *login_name = CCL_data_get_string(CCL_DATA_MEMBER,id,"login_name",NULL);
  logintf->setText(login_name);
  CCL_free(login_name);
  
  tarifset = CCL_member_tarif_get(id);
}

void
MembersFrame::clear()
{
  nametf->setText("");
  emailtf->setText("");
  phonetf->setText("");
}

long
MembersFrame::onAddMember(FXObject*,FXSelector,void*)
{
  FXString name;
  
  if (FXInputDialog::getString(name,this,_("Add new member"),
			   _("Type the name:")) && name.length()) {
    int id = CCL_member_new(name.text());

    if (-1 != id) {
      CCL_member_flags_toggle(id,MEMBER_DELETED,FALSE);
      readAllMembers();
      readMember(id);
    } else
      FXMessageBox::error(this,MBOX_OK,_("Error"),
			  _("The new member could not be created"));
  }

  return 1;
}

long
MembersFrame::onDelMember(FXObject*,FXSelector,void*)
{
  int id;
  FXFoldingItem *current = memberslist->getCurrentItem();

  if (current)
    id = (int) (memberslist->getItemData(current));
  else
    return 1;

  clear();
  CCL_member_flags_toggle(id,MEMBER_DELETED,TRUE);
  memberslist->removeItem(current);

  return 1;
}

long
MembersFrame::onApplyChanges(FXObject*,FXSelector,void*)
{
  FXFoldingItem *mitem = memberslist->findItemByData((void*)editedmember);
  FXString newname = nametf->getText();
  FXString newlogin = logintf->getText();

  if (mitem) {
    newname.trim();
    newlogin.trim();
    CCL_member_tarif_set(editedmember,tarifset);
    if (!newname.empty() && -1 == CCL_member_find(newname.text()))
      CCL_member_name_set(editedmember,nametf->getText().text());
    CCL_member_email_set(editedmember,emailtf->getText().text());
    CCL_member_other_set(editedmember,phonetf->getText().text());
    if (newlogin.empty() && CCL_data_key_exists(CCL_DATA_MEMBER,editedmember,
						"login_name"))
      CCL_data_key_delete(CCL_DATA_MEMBER,editedmember,"login_name");
    else if (-1 == CCL_data_find_by_key_sval(CCL_DATA_MEMBER,"login_name",
					     newlogin.text()))
      CCL_data_set_string(CCL_DATA_MEMBER,editedmember,"login_name",
			  newlogin.text());
    mitem->setText(FXStringVal(editedmember) + "\t" + CCL_member_name_get(editedmember));
    memberslist->updateItem(mitem);
  }
  
  return 1;
}

long
MembersFrame::onMemberSelect(FXObject*,FXSelector,void*)
{
  FXFoldingItem *current = memberslist->getCurrentItem();

  if (current) {
    editedmember = (int) (memberslist->getItemData(current));
    tarifset = CCL_member_tarif_get(editedmember);
    readMember(editedmember);
  }

  return 1;
}

long
MembersFrame::onSetTarif(FXObject*,FXSelector,void*)
{
  int tarif = tarifset;
  FXDialogBox dlg(this,_("Tarif"));
  FXVerticalFrame *vframe =
    new FXVerticalFrame(&dlg,LAYOUT_FILL_X|LAYOUT_FILL_Y);
  FXVerticalFrame *tlistframe =
    new FXVerticalFrame(vframe,FRAME_SUNKEN|LAYOUT_FILL_X|LAYOUT_FILL_Y,
			0,0,0,0,0,0,0,0);
  FXFoldingList *tlist =
    new FXFoldingList(tlistframe,NULL,0,
		      LAYOUT_FILL_X|LAYOUT_FILL_Y|FOLDINGLIST_SINGLESELECT);
  new FXButton(vframe,_("Accept"),NULL,&dlg,FXDialogBox::ID_ACCEPT,
	       FRAME_RAISED|FRAME_THICK);

  tlist->appendHeader(_("ID"),NULL,40);
  tlist->appendHeader(_("Name"),NULL,180);
  dlg.resize(250,200);

  tlist->appendItem(NULL,_("0\tNo special tarif"),NULL,NULL,(void*)0);
  
  for (int i = 0, t; -1 != (t = CCL_tarif_get_nth(i)); i++) {
    const char *name = NULL;
    char buf[256];

    name = CCL_data_get_string(CCL_DATA_TARIF,t,"name",NULL);
    snprintf(buf,256,"%d\t%s",t,name);
    CCL_free(name);
    tlist->appendItem(NULL,buf,NULL,NULL,(void*)t);
  }

  FXFoldingItem *ctitem = tlist->findItemByData((void*)tarif);
  if (ctitem) tlist->selectItem(ctitem);

  if (dlg.execute()) {
    FXFoldingItem *sitem = tlist->getCurrentItem();

    if (sitem)
      tarifset = (int)(sitem->getData());
  }

  return 1;
}

long
MembersFrame::onResetPass(FXObject*,FXSelector,void*)
{
  FXuchar digest[CCL_MD5_DIGEST_LENGTH];
  char password[256];

  snprintf(password,sizeof(password)/sizeof(char),"%d",editedmember);
  CCL_MD5((FXuchar*)password,strlen(password),digest);
  CCL_data_set_blob(CCL_DATA_MEMBER,editedmember,"password",digest,
		    CCL_MD5_DIGEST_LENGTH);

  return 1;
}

long
MembersFrame::onFilter(FXObject*,FXSelector,void* ptr)
{
  readAllMembers((const char *)ptr);

  return 1;
}
