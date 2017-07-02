#include "stdafx.h"

#define IDQUIT              1
#define IDTESTDIALOG        2
#define IDDRAWFRAMES        3
#define IDMESSAGEBOX        4
#define IDSHOWCOLORS        5
#define IDCHECKTESTITEM     6
#define IDENABLETESTITEM    7
#define IDSHOWMSGOK         8
#define IDSHOWMSGOKCANCEL   9
#define IDSHOWYESNO        10
#define IDSHOWYESNOCANCEL  11
#define IDOPEN             12
#define IDSAVE             13
#define IDSAVEAS           14
#define IDADDSTRING        15
#define IDINSERTSTRING     16
#define IDREMOVESTRING     17
#define IDINCRRANGEV       18
#define IDDECRRANGEV       19
#define IDINCRRANGEH       20
#define IDDECRRANGEH       21
#define IDHIDESHADOW       22
#define IDSHOWSHADOW       23
#define IDENABLEFIELD      24
#define IDDISABLEFIELD     25
#define IDINCPROGRESSBAR   26
#define IDDECPROGRESSBAR   27

void testdialog(int left, int top);

MENU(dialogmenu) = {
  MENUPOPUP("&File"                                    ),
    MENUITEM("&Open\tCtrl+O"                          ,IDOPEN),
    MENUITEM("&Save\tCtrl+S"                          ,IDSAVE),
    MENUITEM("Save &As\tCtrl+A"                       ,IDSAVEAS),
    MENUSEPARATOR,
    MENUITEM("&Quit"                                  ,IDQUIT),
  MENUEND,

  MENUPOPUP("&Edit"                                    ),
    MENUITEM("&test dialog2\tCtrl+T"                  ,IDTESTDIALOG     ),
    MENUITEM("&draw frames2\tCtrl+F"                  ,IDDRAWFRAMES     ),
    MENUSEPARATOR,
    MENUITEM("&Messagebox2\tCtrl+M"                   ,IDMESSAGEBOX     ),
    MENUITEM("&Show colors2\tCtrl+C"                  ,IDSHOWCOLORS     ),
    MENUITEM("&Checkitem test dialog2"                ,IDCHECKTESTITEM  ),
    MENUITEM("&Enable test dialog2"                   ,IDENABLETESTITEM ),
    MENUITEM("&Hideshadow"                            ,IDHIDESHADOW     ),
    MENUITEM("Sh&owshadow"                            ,IDSHOWSHADOW     ),
    MENUITEM("Enable field"                           ,IDENABLEFIELD    ),
    MENUITEM("Disable field"                          ,IDDISABLEFIELD   ),
  MENUEND,

  MENUPOPUP("&Messages"                                ),
    MENUITEM("Show Msg &Ok"                           ,IDSHOWMSGOK      ),
    MENUITEM("Show Msg Ok/c&ancel"                    ,IDSHOWMSGOKCANCEL),
    MENUITEM("Show &Yes/No"                           ,IDSHOWYESNO      ),
    MENUITEM("Show Yes/&No/Cancel"                    ,IDSHOWYESNOCANCEL),
  MENUEND,

  MENUPOPUP("&Tilføje/Slet"                            ),
    MENUITEM("Add String\tAlt+A"                      ,IDADDSTRING      ),
    MENUITEM("Insert String\tAlt+I"                   ,IDINSERTSTRING   ),
    MENUITEM("Remove String\tAlt+R"                   ,IDREMOVESTRING   ),
    MENUSEPARATOR,
    MENUITEM("Incr range (V)\tAlt++"                  ,IDINCRRANGEV     ),
    MENUITEM("Decr range (V)\tAlt+-"                  ,IDINCRRANGEV     ),
    MENUITEM("Incr range (H)\tCtrl++"                 ,IDINCRRANGEH     ),
    MENUITEM("Decr range (H)\tCtrl+-"                 ,IDINCRRANGEH     ),
    MENUITEM("Incr progress\tPg up"                   ,IDINCPROGRESSBAR ),
    MENUITEM("Decr progress\tPg down"                 ,IDDECPROGRESSBAR ),
  MENUEND,

  MENUPOPUP("&Help"                                    ),
    MENUITEM("&Test dialog1"                          ,IDTESTDIALOG),
    MENUITEM("&Draw frames1"                          ,IDDRAWFRAMES),
    MENUPOPUP("&Popup2"                                ),
      MENUITEM("&Test dialog2"                        ,IDTESTDIALOG),
      MENUITEM("&Draw frames2"                        ,IDDRAWFRAMES),
      MENUSEPARATOR,
        MENUPOPUP("&Popup2"                            ),
          MENUITEM("&Test dialog2"                    ,IDTESTDIALOG),
          MENUITEM("&Draw frames2"                    ,IDDRAWFRAMES),
          MENUSEPARATOR,
          MENUPOPUP("&popup2"                          ),
            MENUITEM("&test dialog2"                  ,IDTESTDIALOG),
            MENUITEM("&draw frames2"                  ,IDDRAWFRAMES),
            MENUSEPARATOR,
              MENUPOPUP("&popup2"                      ),
                MENUITEM("&test dialog2"              ,IDTESTDIALOG),
                MENUITEM("&draw frames2"              ,IDDRAWFRAMES),
                MENUSEPARATOR,
                MENUITEM("&messagebox2"               ,IDMESSAGEBOX),
                MENUITEM("&show colors2"              ,IDSHOWCOLORS),
              MENUEND,
            MENUITEM("&messagebox2"                   ,IDMESSAGEBOX),
            MENUITEM("&show colors2"                  ,IDSHOWCOLORS),
          MENUEND,
          MENUITEM("&messagebox2"                     ,IDMESSAGEBOX),
          MENUITEM("&show colors2"                    ,IDSHOWCOLORS),
        MENUEND,
      MENUITEM("&messagebox2"                         ,IDMESSAGEBOX),
      MENUITEM("&show colors2"                        ,IDSHOWCOLORS),
    MENUEND,
    MENUITEM("&messagebox1"                           ,IDMESSAGEBOX),
    MENUITEM("show &colors1"                          ,IDSHOWCOLORS),
  MENUEND,
MENUEND
};

AcceleratorItem accel[] = {
  ACCELITEM(CTRL_PRESSED , 0             , 'S' , IDSAVE          ),
  ACCELITEM(CTRL_PRESSED , 0             , 'O' , IDOPEN          ),
  ACCELITEM(CTRL_PRESSED , 0             , 'T' , IDTESTDIALOG    ),
  ACCELITEM(CTRL_PRESSED , 0             , 'F' , IDDRAWFRAMES    ),
  ACCELITEM(CTRL_PRESSED , 0             , 'M' , IDMESSAGEBOX    ),
  ACCELITEM(CTRL_PRESSED , 0             , 'C' , IDSHOWCOLORS    ),
  ACCELITEM(CTRL_PRESSED , 0             , 'A' , IDSAVEAS        ),
  ACCELITEM(0            , SCAN_PGUP     ,  0  , IDINCPROGRESSBAR),
  ACCELITEM(0            , SCAN_PGDOWN   ,  0  , IDDECPROGRESSBAR),
  ACCELITEM(ALT_PRESSED  , 0             , 'A' , IDADDSTRING     ),
  ACCELITEM(ALT_PRESSED  , 0             , 'I' , IDINSERTSTRING  ),
  ACCELITEM(ALT_PRESSED  , 0             , 'R' , IDREMOVESTRING  ),
  ACCELITEM(ALT_PRESSED  , 0             , 'x' , IDDRAWFRAMES    ),
  ACCELITEM(ALT_PRESSED  , SCAN_NUMPLUS  ,  0  , IDINCRRANGEV    ),
  ACCELITEM(ALT_PRESSED  , SCAN_NUMMINUS ,  0  , IDDECRRANGEV    ),
  ACCELITEM(CTRL_PRESSED , SCAN_NUMPLUS  ,  0  , IDINCRRANGEH    ),
  ACCELITEM(CTRL_PRESSED , SCAN_NUMMINUS ,  0  , IDDECRRANGEH    ),
  ACCELEND
};

class MyDialog : public Dialog {
private:
  Menu m_menu;
public:
  MyDialog(const String &title, int x, int y, int w, int h);
  bool eventHandler(int event);
};

MyDialog::MyDialog(const String &title, int x, int y, int w, int h) : Dialog(title,x,y,w,h), m_menu(dialogmenu) {
  setMenu(&m_menu);
//  m_menu.setBorderColor(BACKGROUND_GREEN | FOREGROUND_RED | BACKGROUND_INTENSITY | FOREGROUND_INTENSITY );
  setAccelerator(Accelerator(accel));
  printf(0,2,_T("123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890"));
};


bool MyDialog::eventHandler(int event) {
  int id = -1;
  if(EVENTUP(event)) return false;
  switch(event) {
    case MSG_HSCROLL   :
    { ScrollBar *sc = (ScrollBar*)getDlgItem(9);
      int mi,ma,ps,pos;
      sc->getRange(mi,ma,ps);
      pos = sc->getPos();
      setVar(11,format(_T("(%d,%d,%d,%d)"),pos,mi,ma,ps));
    }
    break;

    case MSG_VSCROLL   :
    { ScrollBar *sc = (ScrollBar*)getDlgItem(8);
      int mi,ma,ps,pos;
      sc->getRange(mi,ma,ps);
      pos = sc->getPos();
      setVar(11,format(_T("(%d,%d,%d,%d)"),pos,mi,ma,ps));
    }
    break;
    case MSG_SELCHANGE :
    case MSG_CHANGE    :
      break;
    default:
      switch(EVENTTYPE(event)) {
      case EVENTTYPE_COMMAND :
        switch(EVENTCOMMAND(event)) {
        case IDQUIT:
          quit(IDCANCEL);
          break;
        case IDCHECKTESTITEM :
          getMenu()->checkMenuItem(IDTESTDIALOG,!getMenu()->menuItemChecked(IDTESTDIALOG));
          break;
        case IDENABLETESTITEM:
          getMenu()->enableMenuItem(IDTESTDIALOG,!getMenu()->menuItemEnabled(IDTESTDIALOG));
          break;
        case IDENABLEFIELD   :
          getDlgItem(1)->setEnable(true);
          break;
        case IDDISABLEFIELD  :
          getDlgItem(1)->setEnable(false);
          break;
        case IDTESTDIALOG    :
          { SMALL_RECT r;
            getRect(r);
            testdialog(r.Left + 4, r.Top+2);
            break;
          }
        case IDHIDESHADOW    :
          setShadow(false);
          break;

        case IDSHOWSHADOW    :
          setShadow(true);
          break;

        case IDDRAWFRAMES    :
          messageBox(_T("Draw frames"));
          break;

        case IDSHOWMSGOK        :
          id = messageBox(format(_T("message : %d"), EVENTCOMMAND(event)),MSGBUTTON_OK          );
          break;

        case IDSHOWMSGOKCANCEL  :
          id = messageBox(format(_T("message : %d"), EVENTCOMMAND(event)),MSGBUTTON_OKCANCEL    );
          break;

        case IDSHOWYESNO        :
          id = messageBox(format(_T("message : %d"), EVENTCOMMAND(event)),MSGBUTTON_YESNO       );
          break;

        case IDSHOWYESNOCANCEL  :
          id = messageBox(format(_T("message : %d"), EVENTCOMMAND(event)),MSGBUTTON_YESNOCANCEL );
          break;

        case IDSHOWCOLORS       :
          Console::setColor(1,1,4,4,BACKGROUND_RED|BACKGROUND_GREEN);
          Console::clearRect(2,2,2,2);
          break;
    //    case 6:
    //      messageBox(format(_T("message : %d"), EVENTCOMMAND(event)),MSGBUTTON_OK          );
    //      break;

        case IDADDSTRING:
          {
            ListBox *lb = (ListBox*)getDlgItem(7);
            lb->addString(format(_T("12345678901234567890"),lb->getCount()).cstr());
          }
          break;

        case IDINSERTSTRING:
          {
            ListBox *lb = (ListBox*)getDlgItem(7);
            lb->insertString(lb->getCurSel(),format(_T("strekekek%d"),lb->getCount()).cstr());
          }
          break;

        case IDREMOVESTRING:
          {
            ListBox *lb = (ListBox*)getDlgItem(7);
            lb->removeString(lb->getCurSel());
          }
          break;

        case IDINCRRANGEV:
          { ScrollBar *sc = (ScrollBar*)getDlgItem(8);
            int mi,ma,ps,pos;
            sc->getRange(mi,ma,ps);
            ma++;
            sc->setRange(mi,ma,ps);
            sc->getRange(mi,ma,ps);
            pos = sc->getPos();
            setVar(11,format(_T("(%d,%d,%d,%d)"),pos,mi,ma,ps));
          }
          break;
        case IDDECRRANGEV:
          { ScrollBar *sc = (ScrollBar*)getDlgItem(8);
            int mi,ma,ps,pos;
            sc->getRange(mi,ma,ps);
            ma--;
            sc->setRange(mi,ma,ps);
            sc->getRange(mi,ma,ps);
            pos = sc->getPos();
            setVar(11,format(_T("(%d,%d,%d,%d)"),pos,mi,ma,ps));
          }
          break;
        case IDINCRRANGEH:
          { ScrollBar *sc = (ScrollBar*)getDlgItem(9);
            int mi,ma,ps,pos;
            sc->getRange(mi,ma,ps);
            ma++;
            sc->setRange(mi,ma,ps);
            sc->getRange(mi,ma,ps);
            pos = sc->getPos();
            setVar(11,format(_T("(%d,%d,%d,%d)"),pos,mi,ma,ps));
          }
          break;
        case IDDECRRANGEH:
          { ScrollBar *sc = (ScrollBar*)getDlgItem(9);
            int mi,ma,ps,pos;
            sc->getRange(mi,ma,ps);
            ma--;
            sc->setRange(mi,ma,ps);
            sc->getRange(mi,ma,ps);
            pos = sc->getPos();
            setVar(11,format(_T("(%d,%d,%d,%d)"),pos,mi,ma,ps));
          }
          break;

        case IDINCPROGRESSBAR   :
          { ProgressBar *pg = (ProgressBar*)getDlgItem(12);
            pg->setPos(pg->getPos() + 1);
            setVar(11,format(_T("%d"),pg->getPos()));
          }
          break;
        case IDDECPROGRESSBAR   :
          { ProgressBar *pg = (ProgressBar*)getDlgItem(12);
            pg->setPos(pg->getPos() - 1);
            setVar(11,format(_T("%d"),pg->getPos()));
          }
          break;

        default:
          id = messageBox(format(_T("message : %d"), EVENTCOMMAND(event)),MSGBUTTON_OK          );
          break;
        }
        if(id >= 0)
          messageBox(format(_T("Messagebox return:%d"),id));
        break;

      case EVENTTYPE_KEY:
        switch(EVENTSCAN(event)) {
        case SCAN_ENTER :
          quit(IDOK);
          break;
        case SCAN_ESCAPE:
          quit(IDCANCEL);
          break;
        }
      }
  }
  return Dialog::eventHandler(event);
}

void testdialog(int left, int top) {
  int f1(0),f3(0);
  String f2;
  double f4;
  bool   f5 = true;

  MyDialog dlg( _T("Fisk"), left, top, 60, 20);
  dlg.addIntField(    1 ,3 ,3 ,5 ,_T("integer:")   );
  dlg.addStringField( 2 ,3 ,5 ,10,_T("String :")   );
  dlg.addIntField(    3 ,3 ,6 ,5 ,_T("integer:")   );
  dlg.addDoubleField( 4 ,3 ,7 ,10,_T("double :")   );
  dlg.addComboBox(    10,3 ,8 ,10,_T("combo  :")   );
  dlg.addBoolField(   5 ,3 ,9 ,1 ,_T("bool   :")   );
  dlg.addButton(      6 ,10,12,6 , 1, _T("[ OK ]") );
  dlg.addListBox(     7 ,25,4 ,20,10           );
  dlg.addStringField( 11,17,3 ,30,_T("range:")     );

  ScrollBar *scb = new ScrollBar(&dlg,SCROLLBAR_VERT,8,55,2,1,17);
  scb->setRange(1,21,20);
  WORD attr = dlg.getColor();
  scb->setColor(attr,attr);
  dlg.addControl(scb);
  scb = new ScrollBar(&dlg,SCROLLBAR_HORZ,9,5,18,40,1);
  scb->setRange(0,21,20);
  scb->setColor(attr,attr);
  dlg.addControl(scb);
  ListBox *lb = (ListBox*)dlg.getDlgItem(7);
  for(int i = 0; i < 10; i++)
    lb->addString(format(_T("strekekekeke%d"),i).cstr());
  ComboBox *cb = (ComboBox*)dlg.getDlgItem(10);
  for(int i = 0; i < 20; i++)
    cb->addString(format(_T("jespkeke%d"),i).cstr());

  dlg.printf(3,11,_T("fisk"));

  ProgressBar *pgb = new ProgressBar(&dlg,PROGRESSBAR_HORZ,12,5,17,40,1);
  dlg.addControl(pgb);
  dlg.setVar(1,f1);
  dlg.setVar(2,f2);
  dlg.setVar(3,f3);
  dlg.setVar(5,f5);
//    setCursorSize(50);
  switch(dlg.domodal()) {
  case IDOK:
    dlg.getVar(1,f1);
    dlg.getVar(2,f2);
    dlg.getVar(3,f3);
    dlg.getVar(4,f4);
    dlg.getVar(5,f5);
    messageBox( format(_T("ok f1:%d f2:<%s> f3:%d, f4:%lf f5:%s"), f1,f2,f3,f4,f5?_T("true"):_T("false")));
    break;
  case IDCANCEL:
    messageBox(_T("cancel"));
    break;
  }

}
