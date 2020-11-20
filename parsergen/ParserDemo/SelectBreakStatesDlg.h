#pragma once

#include <LRparser.h>

class CSelectBreakStatesDlg : public CDialog {
public:
  CSelectBreakStatesDlg(const AbstractParserTables &tables, BitSet &stateSet, CWnd *pParent = nullptr);

  enum { IDD = IDD_DIALOGSTATES };

  CString m_states;

private:
  const AbstractParserTables &m_tables;
  BitSet                     &m_stateSet;
protected:
  virtual void DoDataExchange(CDataExchange *pDX);
  virtual BOOL OnInitDialog();
  virtual void OnOK();
  DECLARE_MESSAGE_MAP()
};

