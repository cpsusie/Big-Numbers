#pragma once

#include "SocketChannel.h"

class CConnectDlg : public CDialog, public OptionsAccessor {
private:
    HACCEL        m_accelTable;
    SOCKET        m_listener;
    Game         &m_game;
    SocketChannel m_channel;
    Player        m_remotePlayer;
    bool          m_timerIsRunning;
    void   enableComputerName();
    void   startTimer();
    void   stopTimer();
    void   makeHandshake();
public:
    CConnectDlg(Game &game, CWnd *pParent = nullptr);

  const SocketChannel &getSocketChannel() {
    return m_channel;
  }

  Player getRemotePlayer() const { // returns the ChessPlayer that should be connected.
    return m_remotePlayer;
  }
    enum { IDD = IDD_CONNECT_DIALOG };
    CString m_serverComputerName;
    int       m_connectedToServer;

public:
    virtual BOOL PreTranslateMessage(MSG *pMsg);
    virtual void DoDataExchange(CDataExchange *pDX);
    virtual void OnOK();
    virtual BOOL OnInitDialog();
    afx_msg void OnGotoComputerName();
    afx_msg void OnRadioCreateGame();
    afx_msg void OnRadioConnectToGame();
    afx_msg void OnTimer(UINT_PTR nIDEvent);
    virtual void OnCancel();
    DECLARE_MESSAGE_MAP()
};
