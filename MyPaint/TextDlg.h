#pragma once

#include "DrawTool.h"

class CTextDlg : public CDialog {
public:
  CTextDlg(FontParameters &fontParameters, String &text, CWnd *pParent = NULL);   // standard constructor

  FontParameters m_fontParameters;
  enum { IDD = IDD_TEXTDIALOG };
  CString   m_text;
  float m_degree;

protected:
  virtual void DoDataExchange(CDataExchange *pDX);
  virtual BOOL OnInitDialog();
  afx_msg void OnButtonfont();
  virtual void OnOK();
  DECLARE_MESSAGE_MAP()
};

