#pragma once

#include "SocketChannel.h"

class CConnectDlg : public CDialog, public OptionsAccessor {
private:
    HACCEL        m_accelTable;
    SOCKET        m_listener;
    SocketChannel m_channel;
    Player        m_myColor;
    bool          m_timerIsRunning;
    void   enableComputerName();
    void   startTimer();
    void   stopTimer();
    void   makeHandshake();
public:
	CConnectDlg(CWnd* pParent = NULL);

    const SocketChannel &getSocketChannel() {
      return m_channel;
    }

    Player getPlayer() const {
      return m_myColor;
    }

	enum { IDD = IDD_CONNECT_DIALOG };
	CString	m_serverComputerName;
	int		m_connectedToServer;


public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
protected:
	virtual void DoDataExchange(CDataExchange* pDX);

protected:

	virtual void OnOK();
	virtual BOOL OnInitDialog();
    afx_msg void OnGotoComputerName();
	afx_msg void OnRadioCreateGame();
	afx_msg void OnRadioConnectToGame();
	afx_msg void OnTimer(UINT nIDEvent);
	virtual void OnCancel();
	DECLARE_MESSAGE_MAP()
};

