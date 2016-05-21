#pragma once

#include "MyUtil.h"

#define TR_TEXT 01
#define TR_ATTR 02
#define TR_ALL  TR_TEXT | TR_ATTR

typedef enum {
  NOFRAME      = 0
 ,SINGLE_FRAME = 1
 ,DOUBLE_FRAME = 2
 ,DOUBLE_HORZ  = 3
 ,DOUBLE_VERT  = 4
} FrameType;

class FrameChars {
public:
  int m_ul, m_ur, m_ll, m_lr, m_horz, m_vert;
  FrameChars(int ul, int ur, int ll, int lr, int horz, int vert);
  static const FrameChars &getFrameChars(FrameType type);
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

class TextRect {
private:
  TextRectBufferOp *m_bufferOp;
  CHAR_INFO *m_tempBuffer;
  int        m_bufferSize;
  CHAR_INFO *allocateTempBuffer(int size);
public:
  TextRect(int w, int h);
  TextRect();
  ~TextRect();
  void copy(   int x, int y, TextRect *src, int l, int t, int w, int h, int op);
  void vset(   int l, int t, int w, int h, int op, va_list argptr);
  void set(    int l, int t, int w, int h, int op, ...);
  void rectangle(int left, int top, int right, int bottom, FrameType type, WORD color);
  int  text(   int x, int y, WORD color, const TCHAR *text);
  void vprintf(int x, int y, WORD color, const TCHAR *format, va_list argptr);
  void printf( int x, int y, WORD color, const TCHAR *format, ...);
  
  int getWidth()  const {
    return m_bufferOp->getWidth();
  }
  
  int getHeight() const {
    return m_bufferOp->getHeight();
  }
};
