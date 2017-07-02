#pragma once

class CViewDlg : public CDialog, OptionsAccessor {
public:
    CViewDlg(CWnd *pParent = NULL);

    enum { IDD = IDD_VIEW_DIALOG };
    BOOL    m_animateCheckmate;
    BOOL    m_animateMoves;
    BOOL    m_askForNewGame;
    BOOL    m_showPlayerInTurn;
    BOOL    m_showComputerTime;
    BOOL    m_showFieldNames;
    BOOL    m_showLegalMoves;

protected:
    virtual void DoDataExchange(CDataExchange *pDX);
    virtual BOOL OnInitDialog();
    virtual void OnOK();
    DECLARE_MESSAGE_MAP()
};

