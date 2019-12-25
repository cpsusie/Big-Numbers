#pragma once

#include <LRparser.h>

class CSelectBreakStatesDlg : public CDialog {
public:
  CSelectBreakStatesDlg(const ParserTables &tables, BitSet &stateSet, CWnd *pParent = NULL);

  enum { IDD = IDD_DIALOGSTATES };

  CString m_states;

private:
  const ParserTables &m_tables;
  BitSet             &m_stateSet;
protected:
  virtual void DoDataExchange(CDataExchange *pDX);
  virtual BOOL OnInitDialog();
  virtual void OnOK();
  DECLARE_MESSAGE_MAP()
};

