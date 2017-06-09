#pragma once

#include "turtle.h"

class CFraktalsDlg : public CDialog {
private:
    HICON m_hIcon;
public:
    CFraktalsDlg(CWnd* pParent = NULL);
    void setcheckstate(int id, bool on);
    bool ischecked(    int id);
    void togglestate(  int id);
    void setcurrentfraktype(int id);
    void sirp1(                     double l, int dir) ;
    void sirp2(          int level, double l, int dir);
    void trekant(        int level, double l, int dir);
    void rose   (        int level, double l, int dir);
    void krussedulle(    int level, double l, int dir);
    void trae(           int level, double l);
    void fnug(           int level, double l, int dir);
    void stjerne(        int level, double l);
    void drawtrekant(    int level);
    void drawsierpinski( int level);
    void drawrose(       int level);
    void drawkrussedulle(int level);
    void drawtrae(       int level);
    void drawfnug(       int level);
    void drawstjerne(    int level);

    void drawfraktal();

    Turtle m_turtle;
    int m_currentlevel;
    int m_currentfraktal;
    enum { IDD = IDD_FRAKTALS_DIALOG };

protected:
    virtual void DoDataExchange(CDataExchange* pDX);
    virtual BOOL OnInitDialog();
    afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
    afx_msg void OnPaint();
    afx_msg HCURSOR OnQueryDragIcon();
    afx_msg void OnFileExit();
    afx_msg void OnFileSierpinski();
    afx_msg void OnFileTrekant();
    afx_msg void OnFileRose();
    afx_msg void OnFileTr();
    virtual void OnCancel();
    virtual void OnOK();
    afx_msg void OnFileCountDown();
    afx_msg void OnFileCountUp();
    afx_msg void OnFileKrussedulle();
    afx_msg void OnFileBeholdforrige();
    afx_msg void OnHelpAbout();
    afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
    afx_msg void OnSize(UINT nType, int cx, int cy);
    afx_msg void OnFileFnug();
    afx_msg void OnFileStjerne();
    DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
