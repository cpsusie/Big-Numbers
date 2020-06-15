#pragma once

class CLinePrinterThread;

class CLinePrinterDlg : public CDialog {
private:
  HICON               m_hIcon;
  String              m_receivedText;
  CListBox           *m_lineList;
  CLinePrinterThread &m_thread;
  bool                m_visible;
  String getContent();
public:
  CLinePrinterDlg(CLinePrinterThread &thread);
  void setTitle(const String &title);
  void addLine( const String &line);
  enum { IDD = IDD_LINEPRINTER_DIALOG };

  virtual BOOL OnInitDialog();
  virtual void DoDataExchange(CDataExchange *pDX);
  afx_msg LRESULT OnSetTitle(   WPARAM wp, LPARAM lp);
  afx_msg LRESULT OnClearWindow(WPARAM wp, LPARAM lp);
  afx_msg LRESULT OnReceiveLine(WPARAM wp, LPARAM lp);
  afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
  afx_msg void OnClose();
  afx_msg void OnButtonCopy();
  afx_msg void OnButtonSave();
  afx_msg void OnButtonClose();
  DECLARE_MESSAGE_MAP()
};

