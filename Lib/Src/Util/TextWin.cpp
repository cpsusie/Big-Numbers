#include "pch.h"
#include <TextWin.h>

TextWin  *TextWin::s_windowList = NULL;
TextWin  *TextWin::s_background = NULL;
TextRect *TextWin::s_console    = NULL;
TextRect *TextWin::s_oldImage   = NULL;

#define FOREGROUND_WHITE FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_RED

void TextWin::initConsole() {
  if(s_background != NULL) {
    return;
  }
  s_console    = new TextRect();
  int w = s_console->getWidth();
  int h = s_console->getHeight();
  s_oldImage   = new TextRect(w,h);
  s_oldImage->copy(0,0,s_console,0,0,w,h,TR_ALL);
  s_background = new TextWin(0,0,w,h);
  s_background->set(0,0,w,h,TR_ALL,_T(' '), FOREGROUND_WHITE | BACKGROUND_GREEN | BACKGROUND_BLUE);
}

void TextWin::unInitConsole() {
  if(s_background == NULL) {
    return;
  }
  s_console->copy(0,0,s_oldImage,0,0,s_oldImage->getWidth(),s_oldImage->getHeight(),TR_ALL);
  delete s_background;
  s_background        = NULL;
  delete s_oldImage;
  s_oldImage          = NULL;
  delete s_console;
  s_console           = NULL;
}

class InitConsoleClass {
public:
  InitConsoleClass();
  ~InitConsoleClass();
};

InitConsoleClass::InitConsoleClass() {
  try {
    TextWin::initConsole();
  } catch(Exception e) {
    _tprintf(_T("Exception:%s\n"),e.what());
  }
}

InitConsoleClass::~InitConsoleClass() {
  try {
    TextWin::unInitConsole();
  } catch(Exception e) {
    _tprintf(_T("Exception:%s\n"),e.what());
  }
}

static InitConsoleClass dummy;

void TextWin::insertBefore(TextWin *tw) {
  m_next = tw;
  if(m_next) {
    m_prev = m_next->m_prev;
    m_next->m_prev = this;
  } else {
    m_prev = NULL;
  }
  if(m_prev) {
    m_prev->m_next = this;
  }

  if(tw == s_windowList) {
    s_windowList = this;
  }
}

void TextWin::insertAfter(TextWin *tw) {
  if(tw == NULL) {
    throwInvalidArgumentException(__TFUNCTION__, _T("tw = NULL"));
  }
  m_next = tw->m_next;
  m_prev = tw;
  if(m_next) {
    m_next->m_prev = this;
  }
  tw->m_next = this;
}

void TextWin::removeFromList() {
  if(m_next) {
    m_next->m_prev = m_prev;
  }
  if(m_prev) {
    m_prev->m_next = m_next;
  }

  if(s_windowList == this) {
    s_windowList = m_next;
  }
}

TextWin::TextWin(int l, int t, int w, int h, int flag, TextWin *ref) {
  m_l       = l;
  m_t       = t;
  m_trect   = new TextRect(w,h);
  m_visible = NULL;
  switch(flag) {
  case TW_BEFORE:
    insertBefore(ref);
    break;
  case TW_BEHIND:
    insertAfter(ref);
    break;
  case TW_FRONT :
    insertBefore(s_windowList);
    break;
  }
  winClipAll(this);
}

void TextWin::releaseClipRect() {
  ClipRect *next;
  for(ClipRect *v = m_visible; v; v = next) {
    next = v->m_next;
    delete v;
  }
  m_visible = NULL;
}

void TextWin::addClipRect(ClipRect *cr) { // cr may be a list.
  m_visible = ClipRect::concat(m_visible,cr);
}

TextWin::~TextWin() {
  delete m_trect;
  releaseClipRect();
  TextWin *next = m_next;
  removeFromList();
  winClipAll(next);
}

void TextWin::repaint(const ClipRect &r) {
  s_console->copy(r.Left,r.Top,m_trect,r.Left-m_l,r.Top-m_t,r.width(),r.height(),TR_ALL);
}

void TextWin::repaint(int l, int t, int w, int h) {
  ClipRect r1(m_l+l, m_t+t, m_l+l+w-1, m_t+t+h-1, NULL);
  for(ClipRect *r = m_visible; r; r = r->m_next) {
    if(r1.overlap(*r)) {
      repaint(ClipRect::intersect(r1, *r));
    }
  }
}

void TextWin::repaint() {
  for(ClipRect *r = m_visible; r; r = r->m_next) {
    repaint(*r);
  }
}

void TextWin::copy(int x, int y, TextRect *src, int l, int t, int w, int h, int op) {
  if(w > getWidth()  - x) {
    w = getWidth()  - x;
  }
  if(h > getHeight() - y) {
    h = getHeight() - y;
  }
  if(w <= 0 || h <= 0) {
    return;
  }
  m_trect->copy(x, y, src, l, t, w, h, op);
  repaint(x, y, w, h);
}

void TextWin::set(int l, int t, int w, int h, int op, ...) {
  va_list argptr;
  va_start(argptr, op);
  m_trect->vset(l, t, w, h, op, argptr);
  va_end(argptr);
  repaint(l, t, w, h);
}

void TextWin::rectangle(int l, int t, int r, int b, FrameType type, WORD color) {
  m_trect->rectangle(l, t, r, b, type, color);
  repaint(l, t, r-l+1, b-t+1);
}

void TextWin::text(int x, int y, WORD color, const TCHAR *text) {
  int w = m_trect->text(x, y, color, text);
  repaint(x, y, w, 1);
}

void TextWin::vprintf(int x, int y, WORD color, _In_z_ _Printf_format_string_ TCHAR const * const format, va_list argptr) {
  text(x, y, color, vformat(format, argptr).cstr());
}

void TextWin::printf(int x, int y, WORD color, _In_z_ _Printf_format_string_ TCHAR const * const format,...) {
  va_list argptr;
  va_start(argptr, format);
  vprintf(x, y, color, format, argptr);
  va_end(argptr);
}

void TextWin::toFront() {
  removeFromList();
  insertBefore(s_windowList);
  winClipAll(this);
}

void TextWin::toBack() {
  TextWin *next = m_next;
  removeFromList();
  insertBefore(s_background);
  winClipAll(next);
}

void TextWin::getRect(SMALL_RECT &r) const {
  r.Left   = getLeft();
  r.Top    = getTop();
  r.Right  = r.Left + getWidth()  - 1;
  r.Bottom = r.Top  + getHeight() - 1;
}

void TextWin::getRect(ClipRect &r) const {
  getRect((SMALL_RECT&)r);
}

void TextWin::setRect(const SMALL_RECT &r) {
  SMALL_RECT oldr;
  getRect(oldr);
  bool update = false;
  if(oldr.Left != r.Left || oldr.Right != r.Right) {
    m_l = r.Left;
    m_t = r.Top;
    update = true;
  }
  int neww = r.Right - r.Left + 1;
  int newh = r.Bottom - r.Top + 1;
  if(neww != getWidth() || newh != getHeight()) {
    setSize(neww,newh);
  } else if(update) {
    winClipAll(this);
  }
}

void TextWin::setSize(int w, int h) {
  if(w < 1 || h < 1) {
    return;
  }
  TextRect *newRect = new TextRect(w, h);
  newRect->set(0,0,w,h,TR_ALL,' ', FOREGROUND_WHITE);
  newRect->copy( 0,0,m_trect,0,0,getWidth(),getHeight(),TR_ALL);
  delete m_trect;
  m_trect = newRect;
  winClipAll(this);
}

void TextWin::setPos(int l, int t) {
  m_l = l;
  m_t = t;
  winClipAll(this);
}

bool TextWin::overlap(TextWin *tw) const {
  SMALL_RECT r2;
  ClipRect r1;
  getRect(r1);
  tw->getRect(r2);
  return r1.overlap(r2);
}

bool TextWin::ptVisible(const COORD &c) const {
  for(const ClipRect *r = m_visible; r; r = r->m_next) {
    if(r->ptInRect(c)) {
      return true;
    }
  }
  return false;
}

void TextWin::winClip() {
  SMALL_RECT thisRect;
  getRect(thisRect);
  releaseClipRect();
  m_visible = new ClipRect(thisRect);
  for(TextWin *tw = s_windowList; tw && tw != this; tw = tw->m_next) {
    ClipRect twrect;
    tw->getRect(twrect);
    if(twrect.overlap(thisRect)) {
      ClipRect *newlist = NULL,*next;
      for(ClipRect *t = m_visible; t; t = next) {
        next = t->m_next;
        if(!t->overlap(twrect)) {
          newlist = ClipRect::concat(newlist,t);
          t->m_next = NULL;
        } else {
          newlist = ClipRect::concat(newlist,ClipRect::clip2(twrect,*t));
          delete t;
        }
      }
      m_visible = newlist;
    }
  }
}

void TextWin::winClipAll(TextWin *from) {
  for(TextWin *tw = from; tw; tw = tw->m_next) {
    tw->winClip();
    tw->repaint();
  }
}

/* ----------------------------- CLIPRECT --------------------------------------------- */

ClipRect::ClipRect(const SMALL_RECT &r, ClipRect *next) : SMALL_RECT(r) {
  m_next = next;
}

void ClipRect::init(int l, int t, int r, int b, ClipRect *next) {
  Left   = l;
  Top    = t;
  Right  = r;
  Bottom = b;
  m_next = next;
}

ClipRect *ClipRect::concat(ClipRect *list1, ClipRect *list2) {
  if(list2 == NULL) {
    return list1;
  }
  if(list1 == NULL) {
    return list2;
  }
  ClipRect *t;
  for(t = list1; t->m_next; t = t->m_next); // find end of list1

  t->m_next = list2;
  return list1;
}

static int clipcode(const SMALL_RECT &front, const SMALL_RECT &back) {
  int code1  =  front.Left   >  back.Left;
      code1 += (front.Right  >= back.Right ) ? 2 : 0;
  int code2  =  front.Top    >  back.Top;
      code2 += (front.Bottom >= back.Bottom) ? 2 : 0;
  return code2 * 4 + code1;
}

// computes the visible parts of back when covered by front. This is a list of at most 4 rectangles
ClipRect *ClipRect::clip2(const SMALL_RECT &front, const SMALL_RECT &back) {
  switch(clipcode(front,back)) {
  case 0:
    return new ClipRect(front.Right+1 ,back.Top        ,back.Right   ,front.Bottom,
           new ClipRect(back.Left     ,front.Bottom+1  ,back.Right   ,back.Bottom    ,NULL));
  case 1:
    return new ClipRect(back.Left     ,back.Top        ,front.Left-1 ,front.Bottom+1 ,
           new ClipRect(front.Right+1 ,back.Top        ,back.Right   ,front.Bottom+1 ,
           new ClipRect(back.Left     ,front.Bottom+1  ,back.Right   ,back.Bottom    ,NULL)));
  case 2:
    return new ClipRect(back.Left     ,front.Bottom+1  ,back.Right   ,back.Bottom    ,NULL);
  case 3:
    return new ClipRect(back.Left     ,back.Top        ,front.Left-1 ,front.Bottom+1 ,
           new ClipRect(back.Left     ,front.Bottom+1  ,back.Right   ,back.Bottom    ,NULL));
  case 4:
    return new ClipRect(back.Left     ,back.Top        ,back.Right   ,front.Top-1    ,
           new ClipRect(front.Right+1 ,front.Top-1     ,back.Right   ,front.Bottom+1 ,
           new ClipRect(back.Left     ,front.Bottom+1  ,back.Right   ,back.Bottom    ,NULL)));
  case 5:
    return new ClipRect(back.Left     ,back.Top        ,back.Right   ,front.Top-1    ,
           new ClipRect(back.Left     ,front.Top-1     ,front.Left-1 ,front.Bottom+1 ,
           new ClipRect(front.Right+1 ,front.Top-1     ,back.Right   ,front.Bottom+1 ,
           new ClipRect(back.Left     ,front.Bottom+1  ,back.Right   ,back.Bottom    ,NULL))));
  case 6:
    return new ClipRect(back.Left     ,back.Top        ,back.Right   ,front.Top-1    ,
           new ClipRect(back.Left     ,front.Bottom+1  ,back.Right   ,back.Bottom    ,NULL));
  case 7:
    return new ClipRect(back.Left     ,back.Top        ,back.Right   ,front.Top-1    ,
           new ClipRect(back.Left     ,front.Top-1     ,front.Left-1 ,front.Bottom+1 ,
           new ClipRect(back.Left     ,front.Bottom+1  ,back.Right   ,back.Bottom    ,NULL)));
  case 8:
    return new ClipRect(front.Right+1 ,back.Top        ,back.Right   ,back.Bottom    ,NULL);
  case 9:
    return new ClipRect(back.Left     ,back.Top        ,front.Left-1 ,back.Bottom    ,
           new ClipRect(front.Right+1 ,back.Top        ,back.Right   ,back.Bottom    ,NULL));
  case 10:
    return NULL;
  case 11:
    return new ClipRect(back.Left     ,back.Top        ,front.Left-1 ,back.Bottom    ,NULL);
  case 12:
    return new ClipRect(back.Left     ,back.Top        ,back.Right   ,front.Top-1    ,
           new ClipRect(front.Right+1 ,front.Top-1     ,back.Right   ,back.Bottom    ,NULL));
  case 13:
    return new ClipRect(back.Left     ,back.Top        ,back.Right   ,front.Top-1    ,
           new ClipRect(back.Left     ,front.Top-1     ,front.Left-1 ,back.Bottom    ,
           new ClipRect(front.Right+1 ,front.Top-1     ,back.Right   ,back.Bottom    ,NULL)));
  case 14:
    return new ClipRect(back.Left     ,back.Top        ,back.Right   ,front.Top-1    ,NULL);
  case 15:
    return new ClipRect(back.Left     ,back.Top        ,back.Right   ,front.Top-1    ,
           new ClipRect(back.Left     ,front.Top-1     ,front.Left-1 ,back.Bottom    ,NULL));
  default:
    throwException(_T("clip2:Invalid code:%d"), clipcode(front,back));
    return NULL;
  }
}

ClipRect ClipRect::intersect(const SMALL_RECT &r1, const SMALL_RECT &r2) {
  return ClipRect(max(r1.Left  ,r2.Left  ),
                  max(r1.Top   ,r2.Top   ),
                  min(r1.Right ,r2.Right ),
                  min(r1.Bottom,r2.Bottom),
                  NULL
                 );
}

bool ClipRect::overlap(const SMALL_RECT &r) const {
  return (Left <= r.Right  && r.Left <= Right
       && Top  <= r.Bottom && r.Top  <= Bottom);
}

bool ClipRect::ptInRect(const COORD &c) const {
  return c.X >= Left && c.X <= Right && c.Y >= Top && c.Y <= Bottom;
}
