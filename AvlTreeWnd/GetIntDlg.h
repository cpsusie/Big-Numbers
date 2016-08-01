#pragma once

#include <NumberInterval.h>

class CGetIntDlg : public CDialog {
private:
  const TCHAR *m_title, *m_prompt;
  IntInterval m_interval;
  bool m_intervalSpecified;
public:
  CGetIntDlg(const TCHAR *title, const TCHAR *prompt, const IntInterval *legalInterval = NULL, int defaultValue = 0, CWnd* pParent = NULL);

  int getValue() const {
    return m_int;
  }

  enum { IDD = IDD_GETINTDIALOG };
  int       m_int;

  protected:
  virtual void DoDataExchange(CDataExchange* pDX);

protected:
    virtual BOOL OnInitDialog();
    virtual void OnOK();
  DECLARE_MESSAGE_MAP()
};
