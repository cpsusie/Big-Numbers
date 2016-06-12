#include "pch.h"
#include <Tokenizer.h>
#include <Console.h>

class MessageDialog : public Dialog {
public:
  MessageDialog(const String &title, int left, int top, int width, int height);
  bool eventHandler(int event);
};

MessageDialog::MessageDialog(const String &title, int left, int top, int width, int height)
: Dialog(title,left,top,width,height) {
}

bool MessageDialog::eventHandler(int event) {
  switch(EVENTTYPE(event)) {
  case EVENTTYPE_COMMAND:
    switch(EVENTCOMMAND(event)) {
    case MSB_OK    :
      quit(MSB_OK);
      return true;
    case MSB_YES   :
      quit(MSB_YES);
      return true;
    case MSB_NO    :
      quit(MSB_NO);
      return true;
    case MSB_CANCEL:
      quit(MSB_CANCEL);
      return true;
    }
    break;
  case EVENTTYPE_KEY:
    if(EVENTDOWN(event) && EVENTSCAN(event) == SCAN_ESCAPE) {
      quit(MSB_CANCEL);
      return true;
    }
  }
  return false;
}

static void StringToLines(TCHAR *str, StringArray &lines) {
  String tmp(str);
  TCHAR *s = tmp.cstr();
  for(TCHAR *r = s; *r; ) {
    switch(*r) {
    case _T('\n'):
      *r = 0;
      lines.add(s);
      r++;
      s = r;
      break;

    case _T('\r'):
      *r = 0;
      lines.add(s);
      r++;
      if(*r == _T('\n')) {
        s = r+1;
      } else {
        s = r;
      }
      break;

    default:
      r++;
    }
  }
  if(*s) {
    lines.add(s);
  }
}

int messageBox(const String &str, int msgtype) {
  int width = 0;

  StringArray lines;
  String copy(str);
  StringToLines(copy.cstr(),lines);
  for(int i = 0; i < (int)lines.size(); i++) {
    int length = (int)lines[i].length();
    if(length > width) {
      width = length;
    }
  }

  width += 6;
  int height = (int)lines.size() + 5;
  int scrwidth,scrheight;
  int minwidth;

  switch(msgtype) {
  case MSGBUTTON_OK         :
    minwidth = 9;
    break;
  case MSGBUTTON_OKCANCEL   :
    minwidth = 25;
    break;
  case MSGBUTTON_YESNO      :
    minwidth = 20;
    break;
  case MSGBUTTON_YESNOCANCEL:
    minwidth = 33;
    break;
  }
  if(width < minwidth) {
    width = minwidth;
  }

  Console::getBufferSize(scrwidth, scrheight);
  int left   = (scrwidth  - width ) / 2;
  int top    = (scrheight - height) / 2;

  WORD attr = BACKGROUND_WHITE;
  MessageDialog dlg("",left,top,width,height);
  dlg.setShadow(true);
  dlg.setBorderType(DOUBLE_FRAME);
  dlg.setColor(attr);
  dlg.setBorderColor(attr);

  for(int i = 0; i < (int)lines.size(); i++) {
    dlg.printf(3, i + 2, _T("%s"),lines[i].cstr());
  }

  switch(msgtype) {
  case MSGBUTTON_OK         :
    dlg.addButton(MSB_OK     , width/2-2   , height - 2,6,1,"[ Ok ]"    );
    break;
  case MSGBUTTON_OKCANCEL   :
    dlg.addButton(MSB_OK     , width/2 - 8 , height - 2,6,1,"[ Ok ]"    );
    dlg.addButton(MSB_CANCEL,  width/2     , height - 2,6,1,"[ Cancel ]");
    break;
  case MSGBUTTON_YESNO      :
    dlg.addButton(MSB_YES    , width/2 - 7 , height - 2,6,1,"[ Yes ]"   );
    dlg.addButton(MSB_NO     , width/2 + 2 , height - 2,6,1,"[ No ]"    );
    break;
  case MSGBUTTON_YESNOCANCEL:
    dlg.addButton(MSB_YES    , width/2 - 13, height - 2,6,1,"[ Yes ]"   );
    dlg.addButton(MSB_NO     , width/2 - 4 , height - 2,6,1,"[ No ]"    );
    dlg.addButton(MSB_CANCEL , width/2 + 4 , height - 2,6,1,"[ Cancel ]");
    break;
  }
  return dlg.domodal();
}
