#pragma once
#include "explorer1.h"
#include <MsHTML.h>
#include <MFCUtil/ComboBoxWithHistory.h>

class CGrabXmlDlg : public CDialogEx {
private:
  HICON                m_hIcon;
  HACCEL               m_accelTable;
  SimpleLayoutManager  m_layoutManager;
  CExplorer1           m_browser;
  CComboBoxWithHistory m_cbUrl;
  CString              m_url;
  IHTMLDocument2      *m_currentDoc;
  String             getDocumentText(IHTMLDocument2 *doc);
  IHTMLInputElement *findInputElementByName(const String &name);
  void enterText(const String &id, const String &text);
  void findRouteAndEnterSolution();
public:
  CGrabXmlDlg(CWnd* pParent = NULL);	// standard constructor

#ifdef AFX_DESIGN_TIME
  enum { IDD = IDD_GRABXML_DIALOG };
#endif

protected:
  virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
  virtual BOOL OnInitDialog();
  virtual BOOL PreTranslateMessage(MSG* pMsg);
  afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
  afx_msg HCURSOR OnQueryDragIcon();
  afx_msg void OnPaint();
  afx_msg void OnSize(UINT nType, int cx, int cy);
  afx_msg void OnClose();
  afx_msg void OnBnClickedOk();
  afx_msg void OnBnClickedCancel();
  afx_msg void OnBnClickedSearch();
  afx_msg void OnFileExit();
  afx_msg void OnToolsFindKnightTour();
  afx_msg void OnRButtonDblClk(UINT nFlags, CPoint point);
  afx_msg void OnTimer(UINT_PTR nIDEvent);

  DECLARE_MESSAGE_MAP()
public:
  DECLARE_EVENTSINK_MAP()
  void DocumentCompleteExplorer1(LPDISPATCH pDisp, VARIANT* URL);
};
