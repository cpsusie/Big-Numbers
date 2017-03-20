#include "stdafx.h"

#define TURKEY NORMALVIDEO|BACKGROUND_GREEN|BACKGROUND_BLUE|BACKGROUND_INTENSITY

void drawBorder(TextWin *w, FrameType type, WORD color = NORMALVIDEO|BACKGROUND_GREEN|BACKGROUND_BLUE) {
  w->rectangle(0,0,w->getWidth()-1,w->getHeight()-1,type,color);
}

void paintWindow(TextWin *w, FrameType type=SINGLE_FRAME) {
  int color = NORMALVIDEO|BACKGROUND_RED|BACKGROUND_BLUE;
  w->set(0,0,w->getWidth(),w->getHeight(),TR_ALL,' ',color);
  for(int i = 0; i < w->getHeight(); i++)
    w->printf(0,i,color,_T(" %d %s"),i,_T("Hello World"));
  
  drawBorder(w,type);
}

TextWin *newWindow(UINT id) {
  TextWin *w = new TextWin(0,0,20,20);
  paintWindow(w);
  return w;
}


int _tmain(int argc, TCHAR **argv) {
  try {
    for (int i = 0; i < 3; i++) {
      const FrameChars &f = FrameChars::getFrameChars((FrameType)i);
      _tprintf(_T("%d:<%s>\n"), i, f.toString().cstr());
    }
    fflush(stdout);

  TextRect cons;
  CompactArray<TextWin*> windows;
  windows.add(newWindow((UINT)windows.size()));
  int currentWin = 0;
#define CURRENT windows[currentWin]
  SMALL_RECT rect;
  drawBorder(CURRENT,DOUBLE_FRAME);
  for(;;) {
    int event = Console::getKeyEvent();
    if(!EVENTDOWN(event)) continue;
    switch(EVENTSCAN(event)) {
    case SCAN_ESCAPE:
      goto exitloop;

    case SCAN_TAB   :
      drawBorder(CURRENT,SINGLE_FRAME);
      if(EVENTSTATE(event) & SHIFT_PRESSED) {
        currentWin = (currentWin == 0 ) ? ((int)windows.size() - 1) : currentWin - 1;
      } else {
        currentWin = (currentWin + 1) % windows.size();
      }
      drawBorder(CURRENT,DOUBLE_FRAME);
      break;
    case SCAN_LEFT  :
      if(EVENTSTATE(event) & SHIFT_PRESSED) {
        CURRENT->setSize(CURRENT->getWidth()-1,CURRENT->getHeight());
        paintWindow(CURRENT,DOUBLE_FRAME);
      } else {
        CURRENT->getRect(rect);
        CURRENT->setPos(rect.Left-1,rect.Top);
      }
      break;
    case SCAN_RIGHT :
      if(EVENTSTATE(event) & SHIFT_PRESSED) {
        CURRENT->setSize(CURRENT->getWidth()+1,CURRENT->getHeight());
        paintWindow(CURRENT,DOUBLE_FRAME);
      } else {
        CURRENT->getRect(rect);
        CURRENT->setPos(rect.Left+1,rect.Top);
      }
      break;
    case SCAN_DOWN  :
      if(EVENTSTATE(event) & SHIFT_PRESSED) {
        CURRENT->setSize(CURRENT->getWidth(),CURRENT->getHeight()+1);
        paintWindow(CURRENT,DOUBLE_FRAME);
      } else {
        CURRENT->getRect(rect);
        CURRENT->setPos(rect.Left,rect.Top+1);
      }
      break;
    case SCAN_UP    :
      if(EVENTSTATE(event) & SHIFT_PRESSED) {
        CURRENT->setSize(CURRENT->getWidth(),CURRENT->getHeight()-1);
        paintWindow(CURRENT,DOUBLE_FRAME);
      } else {
        CURRENT->getRect(rect);
        CURRENT->setPos(rect.Left,rect.Top-1);
      }
      break;
    case SCAN_SPACE :
      if(EVENTSTATE(event) & SHIFT_PRESSED) {
        CURRENT->toBack();
      } else {
        CURRENT->toFront();
      }
      break;
    case SCAN_INSERT:
      windows.add(newWindow((UINT)windows.size()));
      break;
    case SCAN_DELETE:
      if(windows.size() == 1) break;
      delete CURRENT;
      windows.remove(currentWin);
      if(currentWin >= (int)windows.size()) {
        currentWin--;
      }
      drawBorder(CURRENT,DOUBLE_FRAME);
    }
  }
exitloop:
  for(size_t i = 0; i < windows.size(); i++)
    delete windows[i];
  } catch(Exception e) {
    Console::setCursorPos(1,1);
    _tprintf(_T("Exception:%s\n"), e.what());
    Console::getKey();
  }
  return 0;
}
