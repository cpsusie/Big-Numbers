#include "stdafx.h"

class MyGrid : public Dialog {
private:
public:
  MyGrid(const String &title, int x, int y, int w, int h);
  bool eventHandler(int event);
};

MyGrid::MyGrid(const String &title, int x, int y, int w, int h) : Dialog(title,x,y,w,h) {
};


bool MyGrid::eventHandler(int event) {
  int id = -1;
  switch(event) {
    case MSG_SELCHANGE :
    case MSG_CHANGE    :
    ;
  }
  return Dialog::eventHandler(event);
}

void testgrid() {
  int id = 0;

  MyGrid dlg( _T("Fisk"), 1, 1, 70, 20);

  for(int r = 2; r < 10; r++)
    for(int c = 2; c < 15; c++)
      dlg.addIntField(id++,c*4,r,3);

  dlg.domodal();
}
