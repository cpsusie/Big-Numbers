#pragma once

#include "MyUtil.h"

#define TR_ATEXT 01 // text is treated as char
#define TR_WTEXT 02 // text is treated as wchar_t
#define TR_ATTR  04

#ifdef UNICODE
#define TR_TEXT  TR_WTEXT
#else
#define TR_TEXT  TR_ATEXT
#endif

#define TR_ALL  TR_TEXT | TR_ATTR

typedef enum {
  NOFRAME      = 0
 ,SINGLE_FRAME = 1
 ,DOUBLE_FRAME = 2
// ,DOUBLE_HORZ  = 3
// ,DOUBLE_VERT  = 4
} FrameType;

class FrameChars {
private:
  static const FrameChars s_frames[];
public:
  _TUCHAR m_ul, m_ur, m_ll, m_lr, m_horz, m_vert;
  FrameChars(_TUCHAR ul, _TUCHAR ur, _TUCHAR ll, _TUCHAR lr, _TUCHAR horz, _TUCHAR vert);
  inline static const FrameChars &getFrameChars(FrameType type) {
    return s_frames[type];
  };
  String toString() const;
  static const _TUCHAR s_leftVertDoubleLineDoubleBorder, s_rightVertDoubleLineDoubleBorder;
  static const _TUCHAR s_leftVertSingleLineDoubleBorder, s_rightVertSingleLineDoubleBorder;
};

class TextRectBufferOp {
protected:
  int m_w, m_h;
public:
  virtual ~TextRectBufferOp() {};
  int getWidth()  const { return m_w; }
  int getHeight() const { return m_h; }
  virtual void getRect(      CHAR_INFO *dst,         int l, int t, int w, int h) const = 0; // dst is w*h long
  virtual void putRect(const CHAR_INFO *src, int op, int l, int t, int w, int h) = 0;       // src is w*h long
};

#ifdef UNICODE
#define CHARINFOTCHAR(charInfo) ((charInfo).Char.UnicodeChar)
#else
#define CHARINFOTCHAR(charInfo) ((charInfo).Char.AsciiChar)
#endif

class TextRect {
private:
  TextRectBufferOp *m_bufferOp;
  CHAR_INFO        *m_tempBuffer;
  int               m_bufferSize;
  CHAR_INFO        *allocateTempBuffer(int size);
public:
  TextRect(int w, int h);
  TextRect();
  ~TextRect();
  void copy(   int x, int y, TextRect *src, int l, int t, int w, int h, int op);
  void vset(   int l, int t, int w, int h, int op, va_list argptr);
  void set(    int l, int t, int w, int h, int op, ...);
  void rectangle(int left, int top, int right, int bottom, FrameType type, WORD color);
  int  atext(  int x, int y, WORD color, const char    *text);
  int  wtext(  int x, int y, WORD color, const wchar_t *text);
  inline int  text(   int x, int y, WORD color, const TCHAR   *text) {
#ifdef UNICODE
    return wtext(x,y,color,text);
#else
    return atext(x,y,color,text);
#endif
  }
  void vprintf(int x, int y, WORD color, _In_z_ _Printf_format_string_ TCHAR const * const format, va_list argptr);
  void printf( int x, int y, WORD color, _In_z_ _Printf_format_string_ TCHAR const * const format, ...);

  int getWidth()  const {
    return m_bufferOp->getWidth();
  }

  int getHeight() const {
    return m_bufferOp->getHeight();
  }
};
