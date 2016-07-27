#pragma once

#include <scanner.h>

class CTextBox : public CEdit {
private:
  CString       &m_str;
  bool           m_marked;
  SourcePosition m_pos;
  CBrush         m_redBrush;

  void draw(CDC &dc);
public:
  CTextBox(CString &str);
  void markPos(SourcePosition *pos);
  SourcePosition getMarkedPos() const {
    return m_pos;
  }
  
  bool isMarked() const {
    return m_marked;
  }
  
  BOOL CreateEx(DWORD dwStyle, DWORD dwExStyle, const RECT& rect, CWnd* pParentWnd, UINT nID);
  void OnPaint();
  void DoDataExchange(CDataExchange* pDX);
};
