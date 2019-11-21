#include "pch.h"
#include <MyUtil.h>
#include <Conio.h>
#include <Console.h>

#pragma warning(disable : 4073)
#pragma init_seg(lib)

HANDLE Console::s_hStdIn  = GetStdHandle( STD_INPUT_HANDLE  );
HANDLE Console::s_hStdOut = GetStdHandle( STD_OUTPUT_HANDLE );
HANDLE Console::s_hStdErr = GetStdHandle( STD_ERROR_HANDLE  );
Semaphore Console::s_gate;

static const TCHAR *thisFile = __TFILE__;

void Console::checkSysCall(DWORD c, int line) {
  if(!c) throwException(_T("Error in %s line %d:%s"), thisFile, line, getLastErrorText().cstr());
}

void Console::checkSysCall(BOOL  c, int line) {
  if(!c) throwException(_T("Error in %s line %d:%s"), thisFile, line, getLastErrorText().cstr());
}

#define CHECK(f) checkSysCall(f, __LINE__)

HANDLE Console::getHandle(int fileNo) { // static
  switch(fileNo) {
  case STD_INPUT_HANDLE : return s_hStdIn;
  case STD_OUTPUT_HANDLE: return s_hStdOut;
  case STD_ERROR_HANDLE : return s_hStdErr;
  default               : throwInvalidArgumentException(__TFUNCTION__
                                                       ,_T("fileNo (=%d) must be STD_INPUT_HANDLE (=%d), STD_OUTPUT_HANDLE (=%d) or STD_ERROR_HANDLE (=%d)")
                                                       ,fileNo
                                                       ,STD_INPUT_HANDLE
                                                       ,STD_OUTPUT_HANDLE
                                                       ,STD_ERROR_HANDLE);
                          return s_hStdOut;
  }
}

static int getWidth(const SMALL_RECT &r) {
  return r.Right - r.Left + 1;
}

static int getHeight(const SMALL_RECT &r) {
  return r.Bottom - r.Top  + 1;
}

static void setWidth(SMALL_RECT &r, int width) {
  r.Right = r.Left + width - 1;
}

static void setHeight(SMALL_RECT &r, int height) {
  r.Bottom = r.Top + height - 1;
}

void Console::setBufferSize(int width, int height, int fileNo) {
  COORD size;
  size.X = width;
  size.Y = height;
  setBufferSize(size, fileNo);
}

void Console::getBufferSize(int &width, int &height, int fileNo) {
  /* Get display screen's text row and column information. */
  COORD size = getBufferSize(fileNo);
  width  = size.X;
  height = size.Y;
}

void  Console::setBufferSize(const COORD &size, int fileNo) {
  COORD winSize = getWindowSize();
  if(size.X < winSize.X || size.Y < winSize.Y) {
    setWindowSize(min(size.X, winSize.X), min(size.Y, winSize.Y), fileNo);
  }

  if(SetConsoleScreenBufferSize(getHandle(fileNo),size) == 0) {
    throwException(_T("setBufferSize(%d,%d):%s"), size.X, size.Y, getLastErrorText().cstr());
  }
}

COORD Console::getBufferSize(int fileNo) {
  /* Get display screen's text row and column information. */
  CONSOLE_SCREEN_BUFFER_INFO csbi;
  CHECK(GetConsoleScreenBufferInfo(getHandle(fileNo), &csbi));
  return csbi.dwSize;
}

void Console::setWindowSize(int width, int height, int fileNo) {
  SMALL_RECT r = getWindowRect(fileNo);
  setWidth( r,width);
  setHeight(r,height);
  setWindowRect(r, fileNo);
}

void Console::getWindowSize(int &width, int &height, int fileNo) {
  SMALL_RECT r = getWindowRect(fileNo);
  width  = abs(getWidth(r));
  height = abs(getHeight(r));
}

void  Console::setWindowSize(const COORD &size, int fileNo) {
  setWindowSize(size.X, size.Y, fileNo);
}

COORD Console::getWindowSize(int fileNo) {
  int width, height;
  getWindowSize(width, height, fileNo);
  COORD result;
  result.X = width;
  result.Y = height;
  return result;
}

void Console::setWindowRect(int left, int top, int right, int bottom, int fileNo) {
  SMALL_RECT r;
  r.Left   = left;
  r.Top    = top;
  r.Right  = right;
  r.Bottom = bottom;
  setWindowRect(r, fileNo);
}

void Console::setWindowRect(const SMALL_RECT &r, int fileNo) {
  int bufWidth, bufHeight;
  getBufferSize(bufWidth, bufHeight, fileNo);

  int newWinWidth  = getWidth(r);
  int newWinHeight = getHeight(r);

  if(newWinWidth > bufWidth || newWinHeight > bufHeight) {
    setBufferSize(max(newWinWidth, bufWidth), max(newWinHeight, bufHeight), fileNo);
  }

  if(SetConsoleWindowInfo(getHandle(fileNo),true,&r) == 0) {
    throwException(_T("setWindowRect(%d,%d,%d,%d):%s"),r.Left,r.Top,r.Right,r.Bottom,getLastErrorText().cstr());
  }
}

HWND Console::getWindow() {
  return GetConsoleWindow();
}

SMALL_RECT Console::getWindowRect(int fileNo) {
  CONSOLE_SCREEN_BUFFER_INFO csbi;
  CHECK(GetConsoleScreenBufferInfo(getHandle(fileNo), &csbi));
  return csbi.srWindow;
}

void Console::setWindowAndBufferSize(int left, int top, int right, int bottom, int fileNo) {
  int winWidth, winHeight, bufWidth, bufHeight;
  getWindowSize(winWidth, winHeight, fileNo);
  getBufferSize(bufWidth, bufHeight, fileNo);

  int newWidth  = right  - left;
  int newHeight = bottom - top;
  int newRight  = left + newWidth  - 1;
  int newBottom = top  + newHeight - 1;

  s_gate.wait();
  try {
    if(newWidth <= winWidth && newHeight <= winHeight) {
      setWindowRect(left, top, newRight, newBottom, fileNo);
      setBufferSize(newWidth,newHeight, fileNo);
    } else if(newWidth <= winWidth && newHeight > winHeight) {
      setBufferSize(bufWidth,newHeight, fileNo);
      setWindowRect(left, top, newRight, newBottom, fileNo);
      setBufferSize(newWidth,newHeight, fileNo);
    } else if(newWidth > winWidth && newHeight <= winHeight) {
      setBufferSize(newWidth,bufHeight, fileNo);
      setWindowRect(left, top, newRight, newBottom, fileNo);
      setBufferSize(newWidth,newHeight, fileNo);
    } else { // newWidth > winwindth && newHeight > winHeight
      setBufferSize(newWidth,newHeight, fileNo);
      setWindowRect(left, top, newRight, newBottom, fileNo);
    }
    s_gate.notify();
  } catch(...) {
    s_gate.notify();
    throw;
  }
}

void Console::setTitle(const String &title) {
  CHECK(SetConsoleTitle(title.cstr()));
}

String Console::getTitle() { // static
  TCHAR title[4000];
  CHECK(GetConsoleTitle(title, ARRAYSIZE(title)));
  title[ARRAYSIZE(title)-1] = '\0';
  return title;
}

CONSOLE_FONT_INFOEX Console::getFont() {
  CONSOLE_FONT_INFOEX info;
  info.cbSize = sizeof(info);
  CHECK(GetCurrentConsoleFontEx(s_hStdOut, FALSE, &info));
  return info;
}

COORD Console::getFontSize() {
  const CONSOLE_FONT_INFOEX info = getFont();
  const COORD               c    = GetConsoleFontSize(s_hStdOut, info.nFont);
  if ((c.X | c.Y) == 0) {
    checkSysCall(FALSE, __LINE__);
  }
  return c;
}

void Console::setFontSize(const COORD &fontSize) {
  CONSOLE_FONT_INFOEX info = getFont();
  info.dwFontSize = fontSize;
  CHECK(SetCurrentConsoleFontEx(s_hStdOut, FALSE, &info));
}

void Console::clear(WORD attr, int fileNo) {
  int w,h;
  getBufferSize(w,h, fileNo);
  clearRect(0, 0, w-1, h-1, attr, fileNo);
}

void Console::clearLine(int line, WORD attr, int fileNo) {
  int w, h;
  getBufferSize(w, h, fileNo);
  clearRect(0, line, w-1, line, attr, fileNo);
}

void Console::clearRect(int left, int top, int right, int bottom, WORD attr, int fileNo) {
  const int width = right - left + 1;

  WORD attrs[1000];
  for(int i = 0; i < width; i++) {
    attrs[i] = attr;
  }

  s_gate.wait();
  HANDLE h = getHandle(fileNo);
  COORD coord;
  coord.X = left;
  for(int i = top; i <= bottom; i++) {
    DWORD res;
    coord.Y = i;
    CHECK(FillConsoleOutputCharacter( h,  ' ' , width, coord, &res));
    CHECK(WriteConsoleOutputAttribute(h, attrs, width, coord, &res));
  }
  s_gate.notify();
}

void Console::vprintf(int x, int y, _In_z_ _Printf_format_string_ TCHAR const * const format, va_list argptr) {
  const String tmp = vformat(format, argptr);

  COORD coord;
  coord.X = x;
  coord.Y = y;

  s_gate.wait();

  DWORD res;
  CHECK(WriteConsoleOutputCharacter(s_hStdOut, tmp.cstr(), (int)tmp.length(), coord, &res));

  s_gate.notify();
}

void Console::vprintf(int x, int y, WORD color, _In_z_ _Printf_format_string_ TCHAR const * const format, va_list argptr) {
  const String tmp = vformat(format, argptr);

  const int length = (int)tmp.length();
  WORD *attr = new WORD[length];

  for(int i = 0; i < length; i++) {
    attr[i] = color;
  }

  COORD coord;
  coord.X = x;
  coord.Y = y;

  s_gate.wait();

  DWORD res;
  CHECK(WriteConsoleOutputCharacter(s_hStdOut, tmp.cstr(), (int)tmp.length(), coord, &res));
  CHECK(WriteConsoleOutputAttribute(s_hStdOut, attr, length, coord, &res));

  s_gate.notify();

  delete[] attr;
}

void Console::vcprintf(int x, int y, _In_z_ _Printf_format_string_ TCHAR const * const format, va_list argptr) {
  const COORD oldPos = getCursorPos();
  vprintf(x, y, format, argptr);
  setCursorPos(oldPos);
}

void Console::vcprintf(int x, int y, WORD color, _In_z_ _Printf_format_string_ TCHAR const * const format, va_list argptr) {
  const COORD oldPos = getCursorPos();
  vprintf(x, y, color, format, argptr);
  setCursorPos(oldPos);
}

void Console::printf(int x, int y, _In_z_ _Printf_format_string_ TCHAR const * const format, ...) {
  va_list argptr;
  va_start(argptr, format);
  vprintf(x, y, format, argptr);
  va_end(argptr);
}

void Console::printf(int x, int y, WORD color, _In_z_ _Printf_format_string_ TCHAR const * const format, ...) {
  va_list argptr;
  va_start(argptr,format);
  vprintf(x, y, color, format, argptr);
  va_end(argptr);
}

void Console::cprintf(int x, int y, _In_z_ _Printf_format_string_ TCHAR const * const format,...) {
  va_list argptr;
  va_start(argptr, format);
  vcprintf(x, y, format, argptr);
  va_end(argptr);
}

void Console::cprintf(int x, int y, WORD color, _In_z_ _Printf_format_string_ TCHAR const * const format,...) {
  va_list argptr;
  va_start(argptr, format);
  vcprintf(x, y, color, format, argptr);
  va_end(argptr);
}

void Console::setColor(WORD color, int fileNo) {
  CHECK(SetConsoleTextAttribute(getHandle(fileNo), color));
}

void Console::setColor(int x, int y, WORD color, int fileNo) {
  COORD coord;
  coord.X = x;
  coord.Y = y;
  DWORD res;
  CHECK(WriteConsoleOutputAttribute(getHandle(fileNo), &color, 1, coord, &res));
}

WORD Console::getColor(int x, int y, int fileNo) {
  WORD attr;
  COORD coord;
  coord.X = x;
  coord.Y = y;
  DWORD res;
  CHECK(ReadConsoleOutputAttribute(getHandle(fileNo), &attr, 1, coord, &res));
  return attr;
}

void Console::setColor(int left, int top, int right, int bottom, WORD color, int fileNo) {
  COORD coord;
  WORD  attrs[1000];
  int width = right - left + 1;
  for(int i = 0; i < width; i++) {
    attrs[i] = color;
  }
  s_gate.wait();
  coord.X = left;
  for(int i = top; i <= bottom; i++) {
    coord.Y = i;
    DWORD res;
    CHECK(WriteConsoleOutputAttribute(getHandle(fileNo), attrs, width, coord, &res));
  }
  s_gate.notify();
}

void Console::setCursorPos(int x, int y, int fileNo) {
  COORD p;
  p.X = x;
  p.Y = y;
  setCursorPos(p, fileNo);
}

void Console::getCursorPos(int &x, int &y, int fileNo) {
  COORD p = getCursorPos(fileNo);
  x = p.X;
  y = p.Y;
}

void Console::setCursorPos(const COORD &p, int fileNo) {
  CHECK(SetConsoleCursorPosition(getHandle(fileNo), p));
}

COORD Console::getCursorPos(int fileNo) {
  CONSOLE_SCREEN_BUFFER_INFO info;
  CHECK(GetConsoleScreenBufferInfo(getHandle(fileNo), &info));
  return info.dwCursorPosition;
}

void Console::setCursorSize(int percent, int fileNo) {
  HANDLE h = getHandle(fileNo);
  CONSOLE_CURSOR_INFO info;
  CHECK(GetConsoleCursorInfo(h, &info));
  info.dwSize = percent;
  CHECK(SetConsoleCursorInfo(h, &info));
}

int Console::getCursorSize(int fileNo) {
  CONSOLE_CURSOR_INFO info;
  CHECK(GetConsoleCursorInfo(getHandle(fileNo), &info));
  return info.dwSize;
}

void Console::showCursor(bool on, int fileNo) {
  HANDLE h = getHandle(fileNo);
  CONSOLE_CURSOR_INFO info;
  CHECK(GetConsoleCursorInfo(h, &info));
  info.bVisible = on;
  CHECK(SetConsoleCursorInfo(h, &info));
}

bool Console::isCursorVisible(int fileNo) {
  CONSOLE_CURSOR_INFO info;
  CHECK(GetConsoleCursorInfo(getHandle(fileNo), &info));
  return info.bVisible ? true : false;
}

void Console::getLargestConsoleWindowSize(int &maxWidth, int &maxHeight, int fileNo) {
  COORD coord = GetLargestConsoleWindowSize(getHandle(fileNo));
  maxWidth  = coord.X;
  maxHeight = coord.Y;
}

int Console::getKey() {
  for(;;) {
    INPUT_RECORD r;
    DWORD res;
    BOOL ret = ReadConsoleInput(s_hStdIn, &r, 1, &res);
    Console::printf(1,1,_T("key:%3d scancode:%d  "),KEY_EVENT_RECORD_TCHAR(r.Event.KeyEvent),r.Event.KeyEvent.wVirtualScanCode);
    if(r.EventType == KEY_EVENT && r.Event.KeyEvent.bKeyDown && r.Event.KeyEvent.uChar.AsciiChar != 0) {
      return KEY_EVENT_RECORD_TCHAR(r.Event.KeyEvent);
    }
  }
}


int Console::getKeyEvent() {
  INPUT_RECORD r;
  DWORD        mode, saveMode;

  CHECK(GetConsoleMode(s_hStdIn, &mode));
  saveMode = mode;
  mode &= ~(ENABLE_PROCESSED_INPUT | ENABLE_ECHO_INPUT);
  CHECK(SetConsoleMode(s_hStdIn,mode));

  for(;;) {
    DWORD res;
    BOOL ret = ReadConsoleInput(s_hStdIn,&r,1,&res);
    if(r.EventType == KEY_EVENT)
      break;
  }

  CHECK(SetConsoleMode(s_hStdIn,saveMode));

  return KEYEVENT(r.Event.KeyEvent.dwControlKeyState,
                  r.Event.KeyEvent.bKeyDown,
                  r.Event.KeyEvent.wVirtualScanCode,
                  r.Event.KeyEvent.uChar.AsciiChar
                 );
}

bool Console::keyPressed() {
  return _kbhit() != 0;
/*
  INPUT_RECORD r;
  DWORD res;
  PeekConsoleInput(s_hStdIn,&r,1,&res);
  return res == 1;
*/
}
