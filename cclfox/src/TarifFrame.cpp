#include <ccls.h>
#include <fox-1.6/fx.h>
#include <fox-1.6/FXRex.h>
using namespace FX;

#include "cclfox.h"
#include "TarifFrame.h"
#include "verifiers.h"

FXDEFMAP(TarifFrame) TarifFrameMap[] =
{
  FXMAPFUNC(SEL_COMMAND,TarifFrame::ID_ADDPART,TarifFrame::onAddPart),
  FXMAPFUNC(SEL_COMMAND,TarifFrame::ID_DELPART,TarifFrame::onDelPart),
  FXMAPFUNC(SEL_COMMAND,TarifFrame::ID_APPLY,TarifFrame::onApplyChanges),
  FXMAPFUNC(SEL_COMMAND,TarifFrame::ID_ADDPRICE,TarifFrame::onAddPrice),
  FXMAPFUNC(SEL_COMMAND,TarifFrame::ID_DELPRICE,TarifFrame::onDelPrice),
  FXMAPFUNC(SEL_COMMAND,TarifFrame::ID_CHECKVALID,TarifFrame::checkValid),
  FXMAPFUNC(SEL_SELECTED,TarifFrame::ID_PARTLIST,TarifFrame::onPartSelect),
  FXMAPFUNC(SEL_COMMAND,TarifFrame::ID_PERMIN,TarifFrame::onPerminSet),
  FXMAPFUNC(SEL_VERIFY,TarifFrame::ID_CHECKVALID,TarifFrame::onVerify),
  FXMAPFUNC(SEL_COMMAND,TarifFrame::ID_SETTARIF,TarifFrame::onSetTarif),
  FXMAPFUNC(SEL_COMMAND,TarifFrame::ID_NEWTARIF,TarifFrame::onNewTarif),
  FXMAPFUNC(SEL_COMMAND,TarifFrame::ID_TARIFNAME,TarifFrame::onTarifName)
};

FXIMPLEMENT(TarifFrame,FXVerticalFrame,TarifFrameMap,
	    ARRAYNUMBER(TarifFrameMap))

TarifFrame::TarifFrame(FXComposite * parent)
:FXVerticalFrame(parent,LAYOUT_FILL_X|LAYOUT_FILL_Y|FRAME_SUNKEN,
		0,0,0,0,0,0,0,0,0,0)
{
  FXHorizontalFrame *hframe0 = new FXHorizontalFrame(this,LAYOUT_FILL_X);
  new FXLabel(hframe0,_("Fractioned after"));
  perminafter = new FXSpinner(hframe0,4,this,ID_PERMIN,
			      FRAME_SUNKEN|FRAME_THICK);
  perminafter->setRange(0,60*24*7);
  perminafter->setValue(CCL_data_get_int(CCL_DATA_NONE,0,
					 "tarif/perminafter",60));

  new FXHorizontalSeparator(this);
  new FXLabel(hframe0,_("minutes."));
  new FXHorizontalSeparator(this);
  FXHorizontalFrame *nameframe =
    new FXHorizontalFrame(this,LAYOUT_FILL_X);
  new FXLabel(nameframe,_("Tarif's name:"));
  tnametf = new FXTextField(nameframe,15,this,ID_TARIFNAME,TEXTFIELD_NORMAL);
  // TODO: setname

  FXVerticalFrame *tarifframe =
    new FXVerticalFrame(this,FRAME_SUNKEN|LAYOUT_FILL_X|LAYOUT_FILL_Y,
			0,0,0,0,0,0,0,0);
  tariflist =
    new FXFoldingList(tarifframe,this,ID_PARTLIST,
		      FOLDINGLIST_SINGLESELECT|LAYOUT_FILL_X|LAYOUT_FILL_Y);
  tariflist->appendHeader(_("Start"),NULL,80);
  tariflist->appendHeader(_("Days"),NULL,200);
  FXHorizontalFrame *hframe1 = new FXHorizontalFrame(this,LAYOUT_FILL_X);

  settarif = new FXButton(hframe1,_("Set Tarif"),NULL,this,ID_SETTARIF,
			  FRAME_RAISED|FRAME_THICK);
  addpart = new FXButton(hframe1,_("Add"),NULL,this,ID_ADDPART,
			 FRAME_RAISED|FRAME_THICK);
  delpart = new FXButton(hframe1,_("Delete"),NULL,this,ID_DELPART,
			 FRAME_RAISED|FRAME_THICK);
  applychanges = new FXButton(hframe1,_("Apply"),NULL,this,
			      ID_APPLY,FRAME_RAISED|FRAME_THICK);
  newtarif = new FXButton(hframe1,_("New"),NULL,this,
			  ID_NEWTARIF,FRAME_RAISED|FRAME_THICK);
  new FXHorizontalSeparator(this);
  new FXLabel(this,_("Edition:"),NULL,LAYOUT_CENTER_X);
  new FXHorizontalSeparator(this);
  FXHorizontalFrame *hframe2 = new FXHorizontalFrame(this,LAYOUT_FILL_X);
  new FXLabel(hframe2,_("Price per hour:"));

  hpricetf = new FXTextField(hframe2,10,this,ID_CHECKVALID,
			     TEXTFIELD_REAL|TEXTFIELD_NORMAL);
  FXHorizontalFrame *hframe3 = new FXHorizontalFrame(this,LAYOUT_FILL_X);
  new FXLabel(hframe3,_("Start time:"));

  stimetf = new FXTextField(hframe3,5,this,ID_CHECKVALID,
			    TEXTFIELD_NORMAL|TEXTFIELD_LIMITED);
  FXHorizontalFrame *hframe4 = new FXHorizontalFrame(this,LAYOUT_FILL_X);
  new FXLabel(hframe4,_("Days:"));

  daybtn[0] = new FXToggleButton(hframe4,_("Sun"),_("Sun"),NULL,NULL,
				 this,ID_CHECKVALID,TOGGLEBUTTON_NORMAL|
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
  FXVerticalFrame *priceframe =
    new FXVerticalFrame(this,FRAME_SUNKEN|LAYOUT_FILL_X|LAYOUT_FILL_Y,
			0,0,0,0,0,0,0,0);

  pricelist =
    new FXFoldingList(priceframe,NULL,0,
		      FOLDINGLIST_SINGLESELECT|LAYOUT_FILL_X|
		      LAYOUT_FILL_Y);
  pricelist->appendHeader(_("Minutes"),NULL,80);
  pricelist->appendHeader(_("Price"),NULL,120);
  FXHorizontalFrame *hframe5 = new FXHorizontalFrame(this,LAYOUT_FILL_X);

  addprice = new FXButton(hframe5,_("Add price"),NULL,this,ID_ADDPRICE,
			  FRAME_RAISED|FRAME_THICK);
  delprice = new FXButton(hframe5,_("Delete price"),NULL,this,ID_DELPRICE,
			  FRAME_RAISED|FRAME_THICK);
  editedpart = 0;
  clear();
}

TarifFrame::~TarifFrame()
{
}

void
TarifFrame::create()
{
  FXVerticalFrame::create();
}

void
TarifFrame::readTarif()
{
  int id;
  FXuint hr;
  FXuint min;
  FXuint days;
  int tarif = CCL_tarif_get();
  const char *name = CCL_data_get_string(CCL_DATA_TARIF,tarif,"name",NULL);
  const char *daynames[] = {
    N_("Sun"),N_("Mon"),N_("Tue"),N_("Wed"),N_("Thu"),N_("Fri"),N_("Sat")
  };

  tariflist->clearItems();
  
  tnametf->setText(name);

  for (FXuint i = 0; -1 != (id = CCL_tarifpart_get_nth(i)); i++) {
    char buf[128];
    FXString daysstr = "";

    CCL_tarifpart_info_get(id,&hr,&min,&days,NULL);
    for (int j = 0; j < 7; j++) {
      if (days & (1 << j)) {
	daysstr += _(daynames[j]);
	if (j < 6)
	  daysstr += " ";
      }
    }
    snprintf(buf,128,"%.2u:%.2u\t%s",hr,min,daysstr.text());
    tariflist->appendItem(NULL,buf,NULL,NULL,(void *)id);
  }

}

void
TarifFrame::readTarifPart(int id)
{
  FXuint hr;
  FXuint min;
  FXuint days;
  FXuint hprice;

  if (!CCL_tarifpart_info_get(id,&hr,&min,&days,&hprice))
    return;

  char buf[128];

  snprintf(buf,128,"%.2u:%.2u",hr,min);
  stimetf->setText(buf);
  snprintf(buf,128,"%.2f",hprice / 100.0);
  hpricetf->setText(buf);

  for (int i = 0; i < 7; i++) {
    if (days & (1 << i))
      daybtn[i]->setState(TRUE);
    else
      daybtn[i]->setState(FALSE);
  }

  FXuint mins;
  FXuint price;

  pricelist->clearItems();
  for (int i = 0; CCL_tarifpart_price_get_nth(id,i,&mins,&price); i++) {
    snprintf(buf,128,"%u\t%.2f",mins,price / 100.0);
    pricelist->appendItem(NULL,buf);
  }
}

void
TarifFrame::clear()
{
  pricelist->clearItems();
  for (int i = 0; i < 7; i++)
    daybtn[i]->setState(FALSE);
  hpricetf->setText("0.00");
  stimetf->setText("00:00");
}

long
TarifFrame::checkValid(FXObject*,FXSelector,void*)
{
  FXRex time("\\A[0-2]?[0-9]:[0-5][0-9]\\Z",REX_NORMAL);
  FXRex price("\\A\\d*\\.?\\d*\\Z",REX_NORMAL);
  FXbool dayset = FALSE;

  for (int i = 0; i < 7; i++)
    if (daybtn[i]->getState())
      dayset = TRUE;

  if (price.match(hpricetf->getText()) && time.match(stimetf->getText())
      && dayset) {
    addpart->enable();
    applychanges->enable();
  } else {
    addpart->disable();
    applychanges->disable();
  }

  return 1;
}

long
TarifFrame::onAddPart(FXObject*,FXSelector,void*)
{
  FXuint hr = 0;
  FXuint min = 0;
  FXuint days = 0;
  FXuint hprice = 0;
  double hprice_d = 0.0;

  sscanf(stimetf->getText().text(),"%u:%u",&hr,&min);
  sscanf(hpricetf->getText().text(),"%lf",&hprice_d);
  hprice = (FXuint) (hprice_d * 100);
  for (int i = 0; i < 7; i++)
    if (daybtn[i]->getState())
      days |= (1 << i);

  if (!days) {
    FXMessageBox::error(this,MBOX_OK,_("No days where selected"),
			_("There isn't any selected day."));
    return 1;
  }

  int id = CCL_tarifpart_new(hr,min,days,hprice);

  if (-1 != id) {
    FXuint mins = 0;
    FXuint price = 0;
    double price_d = 0.0;
    FXFoldingItem *item = pricelist->getFirstItem();

    while (item) {
      sscanf(item->getText().text(),"%u\t%lf",&mins,&price_d);
      price = (FXuint) (price_d * 100);
      CCL_tarifpart_price_add(id,mins,price);
      item = item->getNext();
    }
    CCL_tarif_rebuild();
    readTarif();
  } else {
    FXuint conflicts;

    CCL_tarifpart_conflicts(hr,min,days,&conflicts);
    for (int i = 0; i < 7; i++)
      if (conflicts & (1 << i))
	daybtn[i]->setState(FALSE);

    FXMessageBox::error(this,MBOX_OK,_("Conflict"),
			_("There is a conflict with another part of the tarif.\nI have deselected the conflicting days."));
  }

  return 1;
}

long
TarifFrame::onDelPart(FXObject*,FXSelector,void*)
{
  int id;
  FXFoldingItem *current = tariflist->getCurrentItem();

  if (current)
    id = (int) (tariflist->getItemData(current));
  else
    return 1;

  CCL_tarifpart_delete(id);
  CCL_tarif_rebuild();
  readTarif();

  return 1;
}

long
TarifFrame::onApplyChanges(FXObject*,FXSelector,void*)
{
  FXuint hr = 0;
  FXuint min = 0;
  FXuint days = 0;
  FXuint hprice = 0;
  double hprice_d = 0.0;

  sscanf(stimetf->getText().text(),"%u:%u",&hr,&min);
  sscanf(hpricetf->getText().text(),"%lf",&hprice_d);
  hprice = (FXuint) (hprice_d * 100);
  for (int i = 0; i < 7; i++)
    if (daybtn[i]->getState())
      days |= (1 << i);

  if (!days) {
    FXMessageBox::error(this,MBOX_OK,_("No days where selected"),
			_("There isn't any selected day."));
    return 1;
  }
  FXuint conflicts = 0;
  FXuint conflictid = CCL_tarifpart_conflicts(hr,min,days,&conflicts);

  if (!conflictid || editedpart == conflictid) {
    FXuint mins = 0;
    FXuint price = 0;
    double price_d = 0.0;
    FXFoldingItem *item = pricelist->getFirstItem();

    CCL_tarifpart_days_set(editedpart,days);
    CCL_tarifpart_stime_set(editedpart,hr,min);
    CCL_tarifpart_hourprice_set(editedpart,hprice);
    CCL_tarifpart_price_clear(editedpart);

    while (item) {
      sscanf(item->getText().text(),"%u\t%lf",&mins,&price_d);
      price = (FXuint) (price_d * 100);
      CCL_tarifpart_price_add(editedpart,mins,price);
      item = item->getNext();
    }
    CCL_tarif_rebuild();
    readTarif();
  } else {
    for (int i = 0; i < 7; i++)
      if (conflicts & (1 << i))
	daybtn[i]->setState(FALSE);

    FXMessageBox::error(this,MBOX_OK,_("Conflict"),
			_("There is a conflict with another part of the tarif.\nI have deselected the conflicting days."));
  }

  return 1;
}

long
TarifFrame::onAddPrice(FXObject*,FXSelector,void*)
{
  FXuint mins = 0;
  double price = 0.0;
  FXDataTarget minstgt(mins);
  FXDataTarget pricetgt(price);

  FXDialogBox dlg(this,_("Add price"));
  FXHorizontalFrame *hframe1 = new FXHorizontalFrame(&dlg,LAYOUT_FILL_X);
  new FXLabel(hframe1,_("Minutes:"));
  FXSpinner *minssp = new FXSpinner(hframe1,4,&minstgt,
				    FXDataTarget::ID_VALUE,
				    FRAME_SUNKEN|FRAME_THICK);
  minssp->setRange(0,10080);
  minssp->setValue(0);
  new FXHorizontalSeparator(&dlg);
  FXHorizontalFrame *hframe2 = new FXHorizontalFrame(&dlg,LAYOUT_FILL_X);
  new FXLabel(hframe2,_("Price:"));
  FXRealSpinner *pricesp = new FXRealSpinner(hframe2,8,&pricetgt,
					     FXDataTarget::ID_VALUE,
					     FRAME_SUNKEN|FRAME_THICK);
  pricesp->setRange(0,999999);
  new FXHorizontalSeparator(&dlg);

  FXHorizontalFrame *hframe = new FXHorizontalFrame(&dlg,LAYOUT_FILL_X);
  new FXButton(hframe,_("Accept"),NULL,&dlg,FXDialogBox::ID_ACCEPT,
	       FRAME_RAISED|FRAME_THICK);
  new FXButton(hframe,_("Cancel"),NULL,&dlg,FXDialogBox::ID_CANCEL,
	       FRAME_RAISED|FRAME_THICK);
  if (dlg.execute() && price >= 0.0) {
    char buf[128];

    snprintf(buf,128,"%u\t%.2f",mins,price);
    pricelist->appendItem(NULL,buf);
  }

  return 1;
}

long
TarifFrame::onDelPrice(FXObject*,FXSelector,void*)
{
  FXFoldingItem *current = pricelist->getCurrentItem();

  if (current) {
    pricelist->removeItem(current);
  }

  return 1;
}

long
TarifFrame::onPartSelect(FXObject*,FXSelector,void*)
{
  FXFoldingItem *current = tariflist->getCurrentItem();

  if (current) {
    editedpart = (int) (tariflist->getItemData(current));
    readTarifPart(editedpart);
  }

  return 1;
}

long
TarifFrame::onPerminSet(FXObject*,FXSelector,void*)
{
  CCL_perminafter_set(perminafter->getValue());
  CCL_data_set_int(CCL_DATA_NONE,0,"tarif/perminafter",CCL_perminafter_get());

  return 1;
}

long
TarifFrame::onVerify(FXObject* sender,FXSelector,void* ptr)
{
  if (sender == stimetf)
    return !isTime((char *)ptr);
  else if (sender == hpricetf)
    return !isPrice((char *)ptr);

  return 0;
}


long
TarifFrame::onSetTarif(FXObject*,FXSelector,void*)
{
  int tarif = CCL_tarif_get();
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

    if (sitem) {
      int starif = (int)(sitem->getData());
 
      CCL_tarif_set(starif);
      CCL_data_set_int(CCL_DATA_NONE,0,"tarif/default",starif);
      readTarif();
    }
  }

  return 1;
}

long
TarifFrame::onNewTarif(FXObject*,FXSelector,void*)
{
  int tarif = CCL_tarif_new(0,0,127,0);

  CCL_tarif_rebuild_all();
  CCL_tarif_set(tarif);
  readTarif();

  return 1;
}

long
TarifFrame::onTarifName(FXObject*,FXSelector,void*)
{
  FXString newname = tnametf->getText();
  int tarif = CCL_tarif_get();

  if (0 < newname.length())
    CCL_data_set_string(CCL_DATA_TARIF,tarif,"name",newname.text());
  else
    CCL_data_key_delete(CCL_DATA_TARIF,tarif,"name");
}
