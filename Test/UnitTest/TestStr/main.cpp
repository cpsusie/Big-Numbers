#include "stdafx.h"
#include "TestStr.h"

//#define INTERACTIVE

#ifdef INTERACTIVE

#include <console.h>

#define IDTRIMLEFT  1000
#define IDTRIMRIGHT 1001
#define IDTRIM      1002
#define IDASSIGN1   1003
#define IDASSIGN2   1004
#define IDCLEAR     1005
#define IDQUIT      1006
#define IDREV       1007
#define IDSPACE     1008
#define IDLOWER     1009
#define IDUPPER     1010
#define IDREPLACE1  1011
#define IDREPLACE2  1012
#define IDREPLACE3  1013
#define IDASSIGN3   1014

MENU(mainmenu) = {
  MENUPOPUP("&File"),
    MENUITEM("&quit",IDQUIT),
  MENUEND,

  MENUPOPUP("&String"),
    MENUITEM("s1 = \"\""                             ,IDCLEAR),
    MENUITEM("Indtast s&1"                            ,'1'),
    MENUITEM("Indtast s&2"                            ,'2'),
    MENUITEM("Indtast n; s1 = n"                      ,'3'),
    MENUITEM("s2 += s1"                               ,'4'),
    MENUITEM("s1 == s2,s1 != s2,s1 < s2..."           ,'5'),
    MENUITEM("Indtast n: s2 = left(s1,n)"             ,'6'),
    MENUITEM("Indtast n: s2 = right(s1,n)"            ,'7'),
    MENUITEM("test &operator[]"                       ,'8'),
    MENUITEM("Indtast from,n: s2 = substr(s1,from,n)" ,'a'),
    MENUITEM("Indtast n: s2 = spacestring(n);"        ,IDSPACE),
    MENUITEM("s2 = &upper(s2)"                        ,IDUPPER),
    MENUITEM("s2 = &lower(s2)"                        ,IDLOWER),
    MENUITEM("s2 = re&v(s2)"                          ,IDREV  ),

    MENUPOPUP("s2 = &Trim..."),
      MENUITEM("s2 = trim&left(s2)" ,IDTRIMLEFT),
      MENUITEM("s2 = trim&right(s2)",IDTRIMRIGHT),
      MENUITEM("s2 = &trim(s2)"     ,IDTRIM),
    MENUEND,

    MENUPOPUP("s2 = &Replace..."),
      MENUITEM("s1.Replace(s1,ch1,ch2)  , &1" ,IDREPLACE1),
      MENUITEM("s1.Replace(s1,ch1,str)  , &2" ,IDREPLACE2),
      MENUITEM("s1.Replace(s1,str1,str2), &3" ,IDREPLACE3),
    MENUEND,

    MENUITEM("&insert char..."                        ,'i'),
    MENUITEM("insert String..."                       ,'I'),
    MENUITEM("&remove..."                             ,'R'),

    MENUPOPUP("s(1/2) &= s(2/1)"),
      MENUITEM("s2 <- s1"  ,IDASSIGN2),
      MENUITEM("s1 <- s2"  ,IDASSIGN1),
      MENUITEM("s1 <-> s2" ,IDASSIGN3),
    MENUEND,

    MENUEND,
  MENUEND
};



class Dialog1 : public Dialog {
public:
  Dialog1();
  String domodal(char *prompt, const String &s);
};

Dialog1::Dialog1() : Dialog("",30,10,40,7) {
  addStringField(1,2,3,25);
}

String Dialog1::domodal(char *prompt, const String &s) {
  setTitle(prompt);
  const String label = String(prompt) + ":";
  setLabel( 1,label);
  setVar(1,s);
  Dialog::domodal();
  String tmp;
  getVar(1,tmp);
  return tmp;
}

class Dialog2 : public Dialog {
public:
  Dialog2();
  int domodal(char *prompt, int n);
};

Dialog2::Dialog2() : Dialog("",30,10,40,7) {
  addIntField(1,2,3,8);
}

int Dialog2::domodal(char *prompt, int n) {
  setTitle(prompt);
  String label = String(prompt) + ":";
  setLabel(1,label.cstr());
  setVar(1,n);
  Dialog::domodal();
  getVar(1,n);
  return n;
}

class Dialog3 : public Dialog {
public:
  int m_n1,m_n2;
  Dialog3();
  void domodal(char *prompt);
};

Dialog3::Dialog3() : Dialog("",30,10,30,7) {
  addIntField(1,2,3,8);
  addIntField(2,2,5,8);
}

void Dialog3::domodal(char *prompt) {
  setTitle(prompt);
  Dialog::domodal();
  getVar(1, m_n1);
  getVar(2, m_n2);
}

class Dialog4 : public Dialog {
public:
  int  m_n;
  char m_ch;
  Dialog4();
  void domodal(char *prompt);
};

Dialog4::Dialog4() : Dialog("",30,10,30,7) {
  addIntField(1,2,3,8);
  addStringField(2,2,5,1);
}

void Dialog4::domodal(char *prompt) {
  setTitle(prompt);
  Dialog::domodal();
  getVar(1,m_n);
  String tmp;
  getVar(2,tmp);
  m_ch = tmp[0];
}

class DialogCharChar : public Dialog {
public:
  char m_chFrom;
  char m_chTo;
  DialogCharChar();
  void domodal(char *prompt);
};

DialogCharChar::DialogCharChar() : Dialog("",30,10,30,7) {
  addStringField(1,2,3,1,"from:");
  addStringField(2,2,5,1,"to  :");
}

void DialogCharChar::domodal(char *prompt) {
  setTitle(prompt);
  Dialog::domodal();
  String fromStr,toStr;
  getVar(1,fromStr);
  getVar(2,toStr  );
  m_chFrom = fromStr[0];
  m_chTo   = toStr[0];
}

class DialogChaSstr : public Dialog {
public:
  char m_chFrom;
  String m_strTo;
  DialogChaSstr();
  void domodal(char *prompt);
};

DialogChaSstr::DialogChaSstr() : Dialog("",30,10,30,7) {
  addStringField(1,2,3,1 ,"from:");
  addStringField(2,2,5,10,"to  :");
}

void DialogChaSstr::domodal(char *prompt) {
  setTitle(prompt);
  Dialog::domodal();
  String fromStr;
  getVar(1,fromStr);
  getVar(2,m_strTo  );
  m_chFrom = fromStr[0];
}

class DialogStrStr : public Dialog {
public:
  String m_strFrom;
  String m_strTo;
  DialogStrStr();
  void domodal(char *prompt);
};

DialogStrStr::DialogStrStr() : Dialog("",30,10,30,7) {
  addStringField(1,2,3,10,"from:");
  addStringField(2,2,5,10,"to  :");
}

void DialogStrStr::domodal(char *prompt) {
  setTitle(prompt);
  Dialog::domodal();
  getVar(1, m_strFrom);
  getVar(2, m_strTo  );
}

class MainDialog : public Dialog {
private:
  Menu   m_menu;
  String m_s1,m_s2;
  char   m_ch;
  Dialog1 dlg1;
  Dialog2 dlg2;
  Dialog3 dlg3;
  Dialog4 dlg4;
  DialogCharChar DlgCharChar;
  DialogChaSstr  DlgCharStr;
  DialogStrStr   DlgStrStr;
//  s1 = "\r\nJKesper\r\n";
//  s1 = trim(s1);
  int m_n,m_from;
//  dlg5.domodal();

//  return 0;
public:
  bool eventHandler(int event);
  MainDialog();
};

MainDialog::MainDialog() : Dialog("",30,10,30,15), m_menu(mainmenu) {
  setMenu(&m_menu);
  
  addStringField(1,2,3 ,10,"s1:");
  addStringField(2,2,5 ,10,"s2:");
  addIntField(   3,2,7 ,10,"length(s1):");
  addIntField(   4,2,9 ,10,"length(s2):");
  addIntField(   5,2,11,10,"streicmp(s1,s2):%d");
}

bool MainDialog::eventHandler(int event) {
  bool ret = false;
  switch(EVENTCOMMAND(event)) {
  case IDCLEAR:
    m_s1 = "";
    ret = true;
    break;
  case '1':
    m_s1 = dlg1.domodal("Indtast s1",m_s1);
    ret = true;
    break;
  case '2':
    m_s2 = dlg1.domodal("Indtast s2",m_s2);
    ret = true;
    break;
  case '3':
    m_n  = dlg2.domodal("Indtast n", 0);
    m_s1 = String(m_n);
    ret = true;
    break;
  case '4':
    m_s2 += m_s1;
    ret = true;
    break;
  case '5':
#define printbool(s1,op,s2) cout << "[" << s1 << "]" << #op << "[" << s2 << "]" << ((s1 op s2) ? "true" : "false") << "\n";
    printbool(m_s1,==,m_s2);
    printbool(m_s1,!=,m_s2);
    printbool(m_s1,< ,m_s2);
    printbool(m_s1,> ,m_s2);
    printbool(m_s1,<=,m_s2);
    printbool(m_s1,>=,m_s2);
    cout.flush();
    pause();
    ret = true;
    break;
  case '6':
    m_n = dlg2.domodal("Indtast n",0);
    m_s2 = ::left(m_s1,m_n);
    ret = true;
    break;
  case '7':
    m_n = dlg2.domodal("Indtast n",0);
    m_s2 = ::right(m_s1,m_n);
    ret = true;
    break;
  case '8':
    try {
      for(m_n = 0; m_n <= m_s1.length(); m_n++)
        cout << "s1[" << m_n << "]:" << m_s1[m_n] << "\n";
    } catch(Exception e) {
      cout << e.what() << "\n";
    }
    ret = true;
    cout.flush();
    pause();
    break;
  case IDTRIMLEFT:
    m_s2 = trimLeft(m_s2);
    ret = true;
    break;
  case IDTRIMRIGHT:
    m_s2 = trimRight(m_s2);
    ret = true;
    break;
  case IDTRIM:
    m_s2 = trim(m_s2);
    ret = true;
    break;
  case 'a':
    dlg3.domodal("Indtast from og length");
    m_from = dlg3.m_n1;
    m_n    = dlg3.m_n2;
    m_s2 = substr(m_s1,m_from,m_n);
    ret = true;
    break;
  case 'i':
    dlg4.domodal("Indtast pos og ch");
    m_n    = dlg4.m_n;
    m_ch   = dlg4.m_ch;
    m_s2.insert(m_n,m_ch);
    ret = true;
    break;
  case 'I':
    m_n    = dlg2.domodal("Indtast pos",0);
    m_s2.insert(m_n,m_s1);
    ret = true;
    break;
  case 'R':
    dlg3.domodal("Indtast pos og length");
    m_from = dlg3.m_n1;
    m_n    = dlg3.m_n2;
    m_s2.remove(m_from,m_n);
    ret = true;
    break;
  case IDSPACE:
    m_n  = dlg2.domodal("Indtast n",0);
    m_s2 = spaceString(m_n);
    ret  = true;
    break;
  case IDREPLACE1:
    DlgCharChar.domodal("indtast from og to");
    m_s1.replace(DlgCharChar.m_chFrom,DlgCharChar.m_chTo);
    ret = true;
    break;

  case IDREPLACE2:
    DlgCharStr.domodal("indtast from og to");
    m_s1.replace(DlgCharStr.m_chFrom,DlgCharStr.m_strTo);
    ret = true;
    break;

  case IDREPLACE3:
    DlgStrStr.domodal("indtast from og to");
    m_s1.replace(DlgStrStr.m_strFrom,DlgStrStr.m_strTo);
    ret = true;
    break;

  case IDUPPER:
    streToUpperCase(m_s2.cstr());
    ret = true;
    break;
  case IDLOWER:
    streToLowerCase(m_s2.cstr());
    ret = true;
    break;
  case IDREV:
    m_s2   = rev(m_s2);
    ret = true;
    break;
  case IDASSIGN2:
    m_s2 = m_s1;
    ret = true;
    break;
  case IDASSIGN1:
    m_s1 = m_s2;
    ret = true;
    break;
  case IDASSIGN3:
    { String s3 = m_s1; m_s1 = m_s2; m_s2 = s3; }
    ret = true;
    break;

  case IDQUIT:
    onOk();
    ret = true;
  }
  if(ret) {
    setVar(1,m_s1);
    setVar(2,m_s2);
  }

  return ret;
}

#endif

int main(int argc, char **argv) {

  testStr();

#ifdef INTERACTIVE

  MainDialog m;
  m.domodal();

#endif

  return 0;

}
