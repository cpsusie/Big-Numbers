#pragma once

#include "EditFieldArray.h"

class CSudokuDlg : public CDialog {
private:
    HICON          m_hIcon;
    HACCEL         m_accelTable;
    EditFieldArray m_editFieldArray;
    GameLevel      m_level;
    time_t         m_startTime;
    time_t         m_startPause;
    bool           m_timerIsRunning;
public:
    CSudokuDlg(CWnd *pParent = NULL);

    enum { IDD = IDD_SUDOKU_DIALOG };

    void startTimer();
    void stopTimer();
    void pauseTimer(bool paused);
    void showMessage(const TCHAR *format,...);
    void showTime(   UINT64 sec      );
    void setName(    const CString &name   );
    CString getName();
    bool loadGame(FILE *f);
    void saveGame(FILE *f);
    void save(const TCHAR *fname);
    void messageFreeFieldCount();
    void newGame();
    bool isShowTimeChecked() { return isMenuItemChecked(this,ID_GAME_SHOWTIME); }
    bool isGamePaused()      { return isMenuItemChecked(this,ID_GAME_PAUSE   ); }

public:
    virtual BOOL PreTranslateMessage(MSG *pMsg);
protected:
    virtual void DoDataExchange(CDataExchange *pDX);

    MarkableEditField *findField(const CPoint &p);
protected:
    void putMatrix(const FieldMatrix &m);
    FieldMatrix getMatrix();

    afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
    afx_msg HCURSOR OnQueryDragIcon();
    afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
    virtual BOOL OnInitDialog();
    afx_msg void OnPaint();
    afx_msg void OnSize(UINT nType, int cx, int cy);
    afx_msg void OnClose();
    afx_msg void OnTimer(UINT_PTR nIDEvent);
    virtual void OnCancel();
    afx_msg void OnFileOpen();
    afx_msg void OnFileNewGame();
    afx_msg void OnFileSelectGame();
    afx_msg void OnFileEmptyGame();
    afx_msg void OnFileSave();
    afx_msg void OnFileSaveas();
    afx_msg void OnFileExit();
    afx_msg void OnGameClearUnmarked();
    afx_msg void OnGameUnmarkAllFields();
    afx_msg void OnGameErrors();
    afx_msg void OnGameLockFields();
    afx_msg void OnGameOpenAllFields();
    afx_msg void OnGameShowTime();
    afx_msg void OnGamePause();
    afx_msg void OnGameResetTimer();
    afx_msg void OnGameLevel();
    afx_msg void OnHelpAboutSudoku();
    afx_msg void OnButtonCount();
    afx_msg void OnSolve();

    DECLARE_MESSAGE_MAP()
};

