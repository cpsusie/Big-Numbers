#pragma once

#include "TextRect.h"

class ClipRect : public SMALL_RECT {
private:
  void init(int l, int t, int r, int b, ClipRect *next = NULL);
  ClipRect  *m_next;
  static ClipRect *clip2(   const SMALL_RECT &front, const SMALL_RECT &back);
  static ClipRect intersect(const SMALL_RECT &r1   , const SMALL_RECT &r2  );
  static ClipRect *concat(ClipRect *list1, ClipRect *list2);
  friend class TextWin;
public:
  ClipRect(int l, int t, int r, int b, ClipRect *next) { init(l, t, r, b, next); }
  ClipRect(const SMALL_RECT &r, ClipRect *next = NULL);
  ClipRect()  { init(0, 0, 0, 0); }
  int  width()  const { return Right  - Left + 1; }
  int  height() const { return Bottom - Top  + 1; }
  bool ptInRect(const COORD &c) const;
  bool overlap(const SMALL_RECT &r) const;
};

#define TW_BEFORE 0x1
#define TW_BEHIND 0x2
#define TW_FRONT  0x3

class TextWin {
private:
  static TextWin  *s_background;
  static TextWin  *s_windowList;
  static TextRect *s_console;
  static TextRect *s_oldImage;

  int       m_l, m_t;
  TextRect *m_trect;
  ClipRect *m_visible;
  TextWin  *m_next, *m_prev;
  void removeFromList();
  void insertBefore(TextWin *tw);
  void insertAfter( TextWin *tw);
  static void winClipAll(TextWin *from);
  void winClip();
  bool overlap(TextWin *tw) const;
  void repaint(const ClipRect &r);
  void repaint(int l, int t, int w, int h);
  void repaint();
  void releaseClipRect();
  void addClipRect(ClipRect *cr);
  void getRect(ClipRect &r) const;
public:
  TextWin(int l, int t, int w, int h, int flag = TW_FRONT, TextWin *ref = NULL);
  ~TextWin();
  void copy(     int x, int y, TextRect *src, int l, int t, int w, int h, int op);
  void set(      int l, int t, int w, int h, int op, ...);
  void rectangle(int l, int t, int r, int b, FrameType type, WORD color);
  void text(   int x, int y, WORD color, const TCHAR *text);
  void vprintf(int x, int y, WORD color, const TCHAR *format, va_list argptr);
  void printf( int x, int y, WORD color, const TCHAR *format, ...);
  int  getLeft()   const { return m_l; }
  int  getTop()    const { return m_t; }
  int  getWidth()  const { return m_trect->getWidth();  }
  int  getHeight() const { return m_trect->getHeight(); }
  void getRect(SMALL_RECT &r) const;
  void setRect(const SMALL_RECT &r);
  bool ptVisible(const COORD &c) const;
  void toFront();
  void toBack();
  void setSize(int w, int h);
  void setPos( int l, int t);
  static void initConsole();
  static void unInitConsole();
};
