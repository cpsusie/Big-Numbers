#include "pch.h"
#include <MyUtil.h>
#include <TextRect.h>
#include <Console.h>

class MemoryBufferOp : public TextRectBufferOp {
private:
  CHAR_INFO *m_p;
public:
  MemoryBufferOp(int w, int h);
  ~MemoryBufferOp();
  void getRect(      CHAR_INFO *dst,         int l, int t, int w, int h) const;
  void putRect(const CHAR_INFO *src, int op, int l, int t, int w, int h);
};

static const TCHAR *thisFile = __TFILE__;

class ConsoleBufferOp : public TextRectBufferOp {
private:
  HANDLE     m_console; // Handle for console window
  static void checkSysCall(DWORD c, int line) {
    if(!c) throwException(_T("Error in %s line %d:%s"), thisFile, line, getLastErrorText().cstr());
  }

  static void checkSysCall(BOOL  c, int line) {
    if(!c) throwException(_T("Error in %s line %d:%s"), thisFile, line, getLastErrorText().cstr());
  }
public:
  ConsoleBufferOp();
  void getRect(      CHAR_INFO *dst,         int l, int t, int w, int h) const;
  void putRect(const CHAR_INFO *src, int op, int l, int t, int w, int h);
};

#define CHECK(f) checkSysCall(f, __LINE__)

MemoryBufferOp::MemoryBufferOp(int w, int h) {
  m_w       = w;
  m_h       = h;
  int size  = w*h;
  m_p       = new CHAR_INFO[size];
  memset(m_p, 0, sizeof(CHAR_INFO)*size);
}

MemoryBufferOp::~MemoryBufferOp() {
  delete[] m_p;
}

void MemoryBufferOp::getRect(CHAR_INFO *dst, int l, int t, int w, int h) const {
  int right  = l + w;
  int bottom = t + h;
  for(int r = t; r < bottom; r++,dst += w) {
    CHAR_INFO *src = m_p + r*getWidth() + l;
    memcpy(dst, src, sizeof(CHAR_INFO)*w);
  }
}

void MemoryBufferOp::putRect(const CHAR_INFO *src, int op, int l, int t, int w, int h) {
  int right  = l + w;
  int bottom = t + h;
  for(int r = t; r < bottom; r++) {
    CHAR_INFO *dst = m_p + r*getWidth() + l;
    int c;
    switch(op) {
    case TR_ATEXT:
    case TR_WTEXT:
      for(c = l; c < right; c++) {
        (dst++)->Char = (src++)->Char;
      }
      break;
    case TR_ATTR:
      for(c = l; c < right; c++) {
        (dst++)->Attributes = (src++)->Attributes;
      }
      break;
    case TR_ATEXT | TR_ATTR:
    case TR_WTEXT | TR_ATTR:
      memcpy(dst, src, sizeof(CHAR_INFO)*w);
      src += w;
      break;
    }
  }
}

ConsoleBufferOp::ConsoleBufferOp() {
  m_console = Console::getHandle(STD_OUTPUT_HANDLE);
  CONSOLE_SCREEN_BUFFER_INFO info;
  CHECK(GetConsoleScreenBufferInfo(m_console, &info));
  m_w = info.dwSize.X;
  m_h = info.dwSize.Y;
}

void ConsoleBufferOp::getRect(CHAR_INFO *dst, int l, int t, int w, int h) const {

#define MAXCONSOPBUFSIZE  53000

  int maxLinesInBlock = MAXCONSOPBUFSIZE / sizeof(CHAR_INFO) / w;
  int linesInBlock;
  for(int topLine = 0; topLine < h; topLine += linesInBlock) {
    linesInBlock = h - topLine;
    if(linesInBlock > maxLinesInBlock) {
      linesInBlock = maxLinesInBlock;
    }
    COORD bufferSize, buffer;
    bufferSize.X = w;
    bufferSize.Y = h;
    buffer.X     = buffer.Y = 0;
    SMALL_RECT r;
    r.Left       = l;
    r.Top        = t + topLine;
    r.Right      = l + w;
    r.Bottom     = t + topLine + linesInBlock;
    CHECK(ReadConsoleOutput(m_console, dst + w*topLine, bufferSize, buffer, &r));
  }
}

void ConsoleBufferOp::putRect(const CHAR_INFO *src, int op, int l, int t, int w, int h) {
  int right = l + w;

  switch(op) {
  case TR_ATEXT:
    { char *buffer = NULL;
      try {
        buffer = new char[w];
        for(int r = 0; r < h; r++) {
          const CHAR_INFO *csrc = src + r*w;
          char            *tmp  = buffer;
          for(int c = l; c < right; c++) {
            *(tmp++) = (csrc++)->Char.AsciiChar;
          }
          COORD pos;
          pos.X = l;
          pos.Y = t + r;
          DWORD nw;
          CHECK(WriteConsoleOutputCharacterA(m_console, buffer, w, pos, &nw));
        }
        delete[] buffer; buffer = NULL;
      } catch(...) {
        if(buffer) delete[] buffer;
        throw;
      }
    }
    break;

  case TR_WTEXT:
    { wchar_t *buffer = NULL;
      try {
        buffer = new wchar_t[w];
        for(int r = 0; r < h; r++) {
          const CHAR_INFO *csrc = src + r*w;
          wchar_t         *tmp  = buffer;
          for(int c = l; c < right; c++) {
            *(tmp++) = (csrc++)->Char.UnicodeChar;
          }
          COORD pos;
          pos.X = l;
          pos.Y = t + r;
          DWORD nw;
          CHECK(WriteConsoleOutputCharacterW(m_console, buffer, w, pos, &nw));
        }
        delete[] buffer; buffer = NULL;
      } catch(...) {
        if(buffer) delete[] buffer;
        throw;
      }
    }
    break;

  case TR_ATTR:
    { WORD *buffer = NULL;
      try {
        buffer = new WORD[w];
        for(int r = 0; r < h; r++) {
          const CHAR_INFO *csrc = src + r*w;
          WORD            *tmp  = buffer;
          for(int c = l; c < right; c++) {
            *(tmp++) = (csrc++)->Attributes;
          }
          COORD pos;
          pos.X = l;
          pos.Y = t + r;
          DWORD nw;
          CHECK(WriteConsoleOutputAttribute(m_console, buffer, w, pos, &nw));
        }
        delete[] buffer; buffer = NULL;
      } catch(...) {
        if(buffer) delete[] buffer;
        throw;
      }
    }
    break;

  case TR_ATEXT | TR_ATTR:
    { int maxLinesInBlock = MAXCONSOPBUFSIZE / sizeof(CHAR_INFO) / w;
      int linesInBlock;
      for(int topLine = 0; topLine < h; topLine += linesInBlock) {
        linesInBlock = h - topLine;
        if(linesInBlock > maxLinesInBlock) {
          linesInBlock = maxLinesInBlock;
        }
        COORD bufferSize, buffer;
        bufferSize.X = w;
        bufferSize.Y = h;
        buffer.X     = buffer.Y = 0;
        SMALL_RECT r;
        r.Left       = l;
        r.Top        = t + topLine;
        r.Right      = l + w;
        r.Bottom     = t + topLine + linesInBlock - 1;
        try {
          CHECK(WriteConsoleOutputA(m_console, src + w*topLine, bufferSize, buffer, &r));
        } catch(Exception e) {
          throwException(_T("%s. putRect(%d,%d,%d,%d), r:(%d,%d,%d,%d),bufferSize:(%d,%d),maxLinesInBlock:%d")
                        ,e.what(), l,t,w,h,r.Left,r.Top,r.Right,r.Bottom,bufferSize.X,bufferSize.Y,maxLinesInBlock);
        }
      }
    }
    break;
  case TR_WTEXT | TR_ATTR:
    { int maxLinesInBlock = MAXCONSOPBUFSIZE / sizeof(CHAR_INFO) / w;
      int linesInBlock;
      for(int topLine = 0; topLine < h; topLine += linesInBlock) {
        linesInBlock = h - topLine;
        if(linesInBlock > maxLinesInBlock) {
          linesInBlock = maxLinesInBlock;
        }
        COORD bufferSize, buffer;
        bufferSize.X = w;
        bufferSize.Y = h;
        buffer.X     = buffer.Y = 0;
        SMALL_RECT r;
        r.Left       = l;
        r.Top        = t + topLine;
        r.Right      = l + w;
        r.Bottom     = t + topLine + linesInBlock - 1;
        try {
          CHECK(WriteConsoleOutputW(m_console, src + w*topLine, bufferSize, buffer, &r));
        } catch(Exception e) {
          throwException(_T("%s. putRect(%d,%d,%d,%d), r:(%d,%d,%d,%d),bufferSize:(%d,%d),maxLinesInBlock:%d")
                        ,e.what(), l,t,w,h,r.Left,r.Top,r.Right,r.Bottom,bufferSize.X,bufferSize.Y,maxLinesInBlock);
        }
      }
    }
    break;
  default:
    if(op != 0) {
      throwInvalidArgumentException(__TFUNCTION__, _T("op=%#x"), op);
    }
    break;
  }
}

TextRect::TextRect(int w, int h) {
  if((w < 0) || (h < 0)) {
    throwInvalidArgumentException(__TFUNCTION__, _T("(w,h)=(%d,%d)"), w, h);
  }
  m_bufferOp   = new MemoryBufferOp(w,h);
  m_tempBuffer = NULL;
  m_bufferSize = 0;
}

TextRect::TextRect() {
  m_bufferOp   = new ConsoleBufferOp();
  m_tempBuffer = NULL;
  m_bufferSize = 0;
}

CHAR_INFO *TextRect::allocateTempBuffer(int size) {
  if(size > m_bufferSize) {
    if(m_bufferSize > 0) {
      delete[] m_tempBuffer;
    }
    m_bufferSize = size;
    m_tempBuffer = new CHAR_INFO[m_bufferSize];
  }
  return m_tempBuffer;
}

TextRect::~TextRect() {
  delete m_bufferOp;
  delete[] m_tempBuffer;
}

static int Max(int x, int y) {
  return (x > y) ? x : y;
}

static int Min(int x, int y) {
  return (x < y) ? x : y;
}

void TextRect::copy(int x, int y, TextRect *src, int l, int t, int w, int h, int op) {
  if(x < 0) {
    l -= x;
    w += x;
    x = 0;
  }
  if(y < 0) {
    t -= y;
    h += y;
    y = 0;
  }
  if(l < 0) {
    w += l;
    l = 0;
  }
  if(t < 0) {
    h += t;
    t = 0;
  }
  w = Min(w,src->getWidth());
  h = Min(h,src->getHeight());
  w = Min(w,getWidth()  - x);
  h = Min(h,getHeight() - y);

  if(h <= 0 || w <= 0) {
    return;
  }

  CHAR_INFO *buffer = allocateTempBuffer(w*h);
  src->m_bufferOp->getRect(buffer,l,t,w,h);
  m_bufferOp->putRect(buffer,op,x,y,w,h);
}

void TextRect::vset(int l, int t, int w, int h, int op, va_list argptr) {
  if(l < 0) {
    w += l;
    l = 0;
  }
  if(t < 0) {
    h += t;
    t = 0;
  }
  if(l > getWidth()) {
    return;
  }
  if(t > getHeight()) {
    return;
  }
  w = Min(w,getWidth()  - l);
  h = Min(h,getHeight() - t);
  if(h <= 0 || w <= 0) {
    return;
  }

  CHAR_INFO arg;
  if(op & TR_ATEXT) {
    assert((op & TR_WTEXT) == 0);
    arg.Char.AsciiChar   = va_arg(argptr, int);
  } else if(op & TR_WTEXT) {
    arg.Char.UnicodeChar = va_arg(argptr, int);
  }
  if(op & TR_ATTR) {
    arg.Attributes       = va_arg(argptr,WORD);
  }

  CHAR_INFO *buffer = allocateTempBuffer(w*h);
  CHAR_INFO *tmp    = buffer;
  for(int r = 0; r < h; r++) {
    for(int c = 0; c < w; c++) {
      *(tmp++) = arg;
    }
  }

  m_bufferOp->putRect(buffer,op,l,t,w,h);
}

void TextRect::set(int l, int t, int w, int h, int op, ...) {
  va_list argptr;
  va_start(argptr,op);
  vset(l,t,w,h,op,argptr);
  va_end(argptr);
}

int TextRect::atext(int x, int y, WORD color, const char *text) {
  if(y < 0 || y >= getHeight()) {
    return 0;
  }
  int length = (int)strlen(text);
  if(x < 0) {
    length  += x;
    text -= x;
    x = 0;
  }
  length = Min(length,getWidth() - x);
  if(length <= 0) {
    return 0;
  }
  CHAR_INFO *buffer = allocateTempBuffer(length);
  CHAR_INFO *tmp    = buffer;
  for(int i = 0; i < length; i++,tmp++) {
    tmp->Attributes     = color;
    tmp->Char.AsciiChar = *(text++);
  }
  m_bufferOp->putRect(buffer,TR_ATEXT|TR_ATTR,x,y,length,1);
  return length;
}

int TextRect::wtext(int x, int y, WORD color, const wchar_t *text) {
  if(y < 0 || y >= getHeight()) {
    return 0;
  }
  int length = (int)wcslen(text);
  if(x < 0) {
    length  += x;
    text -= x;
    x = 0;
  }
  length = Min(length,getWidth() - x);
  if(length <= 0) {
    return 0;
  }
  CHAR_INFO *buffer = allocateTempBuffer(length);
  CHAR_INFO *tmp    = buffer;
  for(int i = 0; i < length; i++,tmp++) {
    tmp->Attributes      = color;
    tmp->Char.UnicodeChar= *(text++);
  }
  m_bufferOp->putRect(buffer,TR_WTEXT|TR_ATTR,x,y,length,1);
  return length;
}

void TextRect::vprintf(int x, int y, WORD color, _In_z_ _Printf_format_string_ TCHAR const * const format, va_list argptr) {
  text(x,y,color,vformat(format,argptr).cstr());
}

void TextRect::printf(int x, int y, WORD color, _In_z_ _Printf_format_string_ TCHAR const * const format,...) {
  va_list argptr;
  va_start(argptr,format);
  vprintf(x,y,color,format,argptr);
  va_end(argptr);
}

void TextRect::rectangle(int left, int top, int right, int bottom, FrameType type, WORD color) {
  const FrameChars &f   = FrameChars::getFrameChars(type);
  TCHAR             lineStr[4096], charStr[2];
  const int         len = min(ARRAYSIZE(lineStr)-2,right-left-1);
  TMEMSET(lineStr, f.m_horz, len);
  lineStr[len] = 0;
  charStr[1]   = 0;

#define SETCORNER(x,y,ch) { charStr[0] = ch; text(x,y,color,charStr); }

  SETCORNER(left, top , f.m_ul);
  text(left+1, top   , color, lineStr);
  SETCORNER(right,top,f.m_ur);

  charStr[0] = f.m_vert;
  for(int y = top+1; y < bottom; y++) {
    text(left , y, color, charStr);
    text(right, y, color, charStr);
  }
  SETCORNER(left  , bottom, f.m_ll);
  text(left+1, bottom, color, lineStr);
  SETCORNER(right , bottom, f.m_lr);
}
