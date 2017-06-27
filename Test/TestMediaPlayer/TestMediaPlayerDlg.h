#pragma once

#include <MFCUtil/LayoutManager.h>
#include "mediaplayer.h"

class CTestMediaPlayerDlg : public CDialog {
private:
	HICON               m_hIcon;
  SimpleLayoutManager m_layoutManager;
public:
	CTestMediaPlayerDlg(CWnd *pParent = NULL);

	enum { IDD = IDD_TESTMEDIAPLAYER_DIALOG };

	virtual void DoDataExchange(CDataExchange* pDX);
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnButtonplay();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	DECLARE_MESSAGE_MAP()
  CMediaplayer m_player;
};

