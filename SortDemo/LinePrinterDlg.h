#pragma once

#include <MFCUtil/LayoutManager.h>

class CLinePrinterThread;

class CLinePrinterDlg : public CDialog {
private:
  HICON               m_hIcon;
  SimpleLayoutManager m_layoutManager;
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

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);

protected:
	afx_msg LRESULT OnSetTitle(   WPARAM wp, LPARAM lp);
	afx_msg LRESULT OnClearWindow(WPARAM wp, LPARAM lp);
	afx_msg LRESULT OnReceiveLine(WPARAM wp, LPARAM lp);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	virtual BOOL OnInitDialog();
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	afx_msg void OnClose();
	afx_msg void OnButtonCopy();
	afx_msg void OnButtonSave();
	afx_msg void OnButtonClose();
	DECLARE_MESSAGE_MAP()
};

