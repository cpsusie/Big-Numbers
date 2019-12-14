#include "stdafx.h"
#include <time.h>
#include <InputValue.h>
#include <ostream>

// see vc98\include\wincon.h for a complete list of console-attributes
#define COLOR FOREGROUND_RED | FOREGROUND_BLUE | FOREGROUND_INTENSITY
#define FOREGROUND_BLACK 0
#define BACKGROUND_BLACK 0

#define REVERSE_VIDEO BACKGROUND_WHITE | FOREGROUND_BLACK
/*
#define FOREGROUND_GREEN     0x0002 // text color contains green.
#define FOREGROUND_RED       0x0004 // text color contains red.
#define FOREGROUND_INTENSITY 0x0008 // text color is intensified.
#define BACKGROUND_BLUE      0x0010 // background color contains blue.
#define BACKGROUND_GREEN     0x0020 // background color contains green.
#define BACKGROUND_RED       0x0040 // background color contains red.
#define BACKGROUND_INTENSITY
*/

#define IDQUIT           1
#define IDTESTDIALOG     2
#define IDMESSAGEBOX     4
#define IDCHECKTESTITEM  6
#define IDENABLETESTITEM 7
#define IDTESTGRID       8
#define IDREMOVETESTITEM 9
#define IDTESTITEM       10

StaticMenuItem mainmenu[] = {
  MENUPOPUP(_T("&File")                                ),
    MENUITEM(_T("&quit")                              ,IDQUIT          ),
  MENUEND,
  MENUPOPUP(_T("E&&d&i&&t")                            ),
    MENUITEM(_T("&&tes&t &&dialog2\tCtrl+T")          ,IDTESTDIALOG    ),
    MENUITEM(_T("&test grid\tCtrl+G")                 ,IDTESTGRID      ),
    MENUSEPARATOR,
    MENUITEM(_T("&messagebox2")                       ,IDMESSAGEBOX    ),
    MENUITEM(_T("&checkitem test dialog2")            ,IDCHECKTESTITEM ),
    MENUITEM(_T("&enable test dialog2")               ,IDENABLETESTITEM),
    MENUITEM(_T("&remove testitem")                   ,IDREMOVETESTITEM),
    MENUITEM(_T("&testitem")                          ,IDTESTITEM      ),
  MENUEND,

  MENUPOPUP(_T("&View")                                ),
    MENUITEM(_T("&test dialog2")                      ,IDTESTDIALOG    ),
    MENUSEPARATOR,
    MENUITEM(_T("&messagebox2")                       ,IDMESSAGEBOX    ),
  MENUEND,

  MENUPOPUP(_T("&Options")                             ),
    MENUITEM(_T("&test dialog2")                      ,IDTESTDIALOG    ),
    MENUSEPARATOR,
    MENUITEM(_T("&messagebox2")                       ,IDMESSAGEBOX    ),
  MENUEND,

  MENUPOPUP(_T("&Help")                                ),
    MENUITEM(_T("&test dialog1")                      ,IDTESTDIALOG    ),
    MENUPOPUP(_T("&popup2")                            ),
      MENUITEM(_T("&test dialog2")                    ,IDTESTDIALOG    ),
      MENUSEPARATOR,
      MENUITEM(_T("&messagebox2")                     ,IDMESSAGEBOX    ),
      MENUPOPUP(_T("&View")                            ),
        MENUITEM(_T("&test dialog2")                  ,IDTESTDIALOG    ),
        MENUSEPARATOR,
        MENUITEM(_T("&messagebox2")                   ,IDMESSAGEBOX    ),
        MENUPOPUP(_T("&View")                          ),
          MENUITEM(_T("&test dialog2")                ,IDTESTDIALOG    ),
          MENUSEPARATOR,
          MENUITEM(_T("&messagebox2")                 ,IDMESSAGEBOX    ),
        MENUEND,
      MENUEND,
    MENUEND,
    MENUITEM(_T("&messagebox1")                       ,IDMESSAGEBOX),
  MENUEND,
  MENUEND
};

extern void testdialog(int left, int top);
extern void testgrid();


static void testSetScreenBufferSize() {
  for(;;) {
    int bufWidth,bufHeight, winWidth,winHeight;
    Console::getBufferSize(bufWidth, bufHeight);
    Console::getWindowSize(winWidth, winHeight);
    _tprintf(_T("Current bufferSize:(%3d,%3d)\n"), bufWidth, bufHeight);
    _tprintf(_T("Current windowSize:(%3d,%3d)\n"), winWidth, winHeight);

    int w,h;
    _tprintf(_T("Set (W)indowSize/(B)ufsize/Window(P)osition:"));
    switch(Console::getKey()) {
    case 'w':
    case 'W':
      w = inputValue<int>(_T("\nEnter new window width :"));
      h = inputValue<int>(_T("Enter new window height:"));

      try {
        Console::setWindowSize(w,h);
      } catch(Exception e) {
        _tprintf(_T("Exception:%s\n"),e.what());
      }
      break;
    case 'b':
    case 'B':
      w = inputValue<int>(_T("\nEnter new buffer width :"));
      h = inputValue<int>(_T("Enter new buffer height:"));

      try {
        Console::setBufferSize(w,h);
      } catch(Exception e) {
        _tprintf(_T("Exception:%s\n"),e.what());
      }
      break;

    }
  }
}

int _tmain(int argc, TCHAR **argv) {

//  testSetScreenBufferSize();

  try {
    int scrWidth, scrHeight;
    Console::getBufferSize(scrWidth, scrHeight);

    Console::setWindowAndBufferSize(0,0,120,60);
//    setScreenSize(100,40);
//    setWindowSize(0,0,99,39);


    Console::getBufferSize(scrWidth, scrHeight);

    Console::clearRect(1,1,2,2,REVERSEVIDEO);

    Console::clear(REVERSEVIDEO);
    for(int i = 0; i < scrHeight; i++) {
      for(int j = 0; j < scrWidth/2; j++) {
        int attr = ((i%16) << 4) + (j%16);
        Console::printf(2*j,i,attr,_T("%02x"),attr);
      }
    }

//  setwindowinfo(0,0,89,39);
//  setScreenSize(90, 40);

    Console::setTitle(_T("fisk"));

    Menu m(mainmenu);
  //  consoleprintf(1,1,_T("screensize:%d,%d"),scrWidth,scrHeight);
  //  getLey();
  //  m.enableitem(IDQUIT,false);

    for(;;) {
      switch(m.domodal(0,0)) {
      case IDTESTDIALOG:
        testdialog(10,1);
        break;
      case IDTESTGRID  :
        testgrid();
        break;
      case IDMESSAGEBOX:
        { time_t tt;
          time(&tt);
          TCHAR *strtime = _tctime(&tt);
          messageBox(format(_T("screensize:(%d,%d)\n\n\ntime:%s\n\n12345676890123456789012345678901234567890"),scrWidth,scrHeight,strtime));
        }
        break;
      case IDCHECKTESTITEM:
        if(m.menuItemChecked(IDTESTDIALOG))
          m.checkMenuItem(IDTESTDIALOG,false);
        else
          m.checkMenuItem(IDTESTDIALOG,true);
        break;
      case IDENABLETESTITEM:
        if(m.menuItemEnabled(IDTESTDIALOG))
          m.enableMenuItem(IDTESTDIALOG,false);
        else
          m.enableMenuItem(IDTESTDIALOG,true);
        break;
      case IDREMOVETESTITEM:
        m.removeMenuItem(IDTESTITEM);
        break;
      case IDQUIT:
        return 0;
      }
    }
  } catch(Exception e) {
    _tprintf(_T("%s\n"),e.what());
    Console::getKey();
  }
  return 0;
}
