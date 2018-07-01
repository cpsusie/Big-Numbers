#pragma once

#include "SubDivision.h"

class CIncrementalDelaunayTriangulationDlg : public CDialogEx {
private:
  SubDivision *m_edges;
  CPoint       m_p[3];
  int          m_count;
  void paintEdges(CDC &dc);
  void paintAll(  CDC &dc);
public:
	CIncrementalDelaunayTriangulationDlg(CWnd* pParent = nullptr);	// standard constructor

#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_INCREMENTALDELAUNAYTRIANGULATION_DIALOG };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support

protected:
	HICON m_hIcon;

	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
  afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
  afx_msg void OnFileExit();
  afx_msg void OnEditClear();
	DECLARE_MESSAGE_MAP()
public:
};
