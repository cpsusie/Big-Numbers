#include "stdafx.h"
#include <Date.h>
#include <Console.h>
#include <InputValue.h>

class TestHandler : public TimeoutHandler {
private:
  Timestamp m_lastPrint;
public:
  void handleTimeout(Timer &t);
};

void TestHandler::handleTimeout(Timer &t) {
  const COORD cp = Console::getCursorPos();
  Timestamp now;
  Console::cprintf(1, 10, _T("Timer(%d,%s):time:%s, last:%s, diff:%lf, timeout:%s")
                  ,t.getId(), t.getName().cstr()
                  ,now.toString(hhmmssSSS).cstr(), m_lastPrint.toString(hhmmssSSS).cstr()
                  ,Timestamp::diff(m_lastPrint,now, TMILLISECOND)
                  ,format1000(t.getTimeout()).cstr());
  Console::setCursorPos(cp);
  m_lastPrint = now;
}

int main() {
  Timer timer(1, "TestTimer");
  TestHandler handler;
  UINT msec = 1000;
  bool repeatTimeout = true;
  for (;;) {
    Console::setCursorPos(0, 0);
    char ch = (char)inputValue<TCHAR>(_T("Enter command:((S)tart/S(t)op/(C)hange time/change (R)epeat/(Q)uit:"));
    ch = isupper(ch) ? tolower(ch) : ch;
    switch(ch) {
    case 's':
      timer.startTimer(msec, handler, repeatTimeout);
      break;
    case 't':
      timer.stopTimer();
      break;
    case 'c':
      { int newTimeout = inputValue<UINT>(_T("Enter timeout in msec (old timeout:%s):"), format1000(msec).cstr());
        msec = newTimeout;
        timer.setTimeout(msec, repeatTimeout);
      }
      break;
    case 'r':
      { char ch = (char)inputValue<TCHAR>(_T("Repeat timeout (Y/N) (old repeat:%c):"), repeatTimeout?'Y':'N');
        ch = isupper(ch) ? tolower(ch) : ch;
        switch (ch) {
        case 'y': repeatTimeout = true; break;
        case 'n': repeatTimeout = false; break;
        }
        timer.setTimeout(msec, repeatTimeout);
      }
      break;
    case 'q':
      goto End;
    default:;
    }
  }
End:
  return 0;
}
