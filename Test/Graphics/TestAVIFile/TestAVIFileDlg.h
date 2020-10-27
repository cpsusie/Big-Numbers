#pragma once

#include "MovieThread.h"
#include <QueueList.h>

class LineElement {
public:
  CPoint m_p1, m_p2;
  LineElement(const CPoint &p1, const CPoint &p2) : m_p1(p1), m_p2(p2) {
  }
};

class CTestAVIFileDlg : public CDialog, public FrameGenerator {
private:
    MovieThread           *m_movieThread;
    UINT                   m_frameCount;
    UINT                   m_frameIndex;
    unsigned int           m_timersRunning;
    CSize                  m_frameSize;
    PixRect               *m_frame;
    CPoint                 m_p1, m_p2;
    CSize                  m_step1, m_step2;
    QueueList<LineElement> m_queue;
    D3DCOLOR               m_color;
    int                    m_colorStep[3];

    void startMovieThread(UINT frameCount);
    void stopMovieThread();
    void startTimer(unsigned int id, int msec);
    void stopTimer( unsigned int id);
    void drawFrame();
    void flushFrame();
    void nextPoint(CPoint &p, CSize &step);
    void nextColor();
    void nextColorComponent(int &cc, int &step);
    void showState();
public:
    CTestAVIFileDlg(CWnd *pParent = nullptr);

    CSize    getFrameSize();
    PixRect *nextFrame();

    enum { IDD = IDD_TESTAVIFILE_DIALOG };

protected:
    virtual void DoDataExchange(CDataExchange *pDX);

    HICON m_hIcon;

    virtual BOOL OnInitDialog();
    afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
    afx_msg void OnPaint();
    afx_msg HCURSOR OnQueryDragIcon();
    virtual void OnOK();
    virtual void OnCancel();
    afx_msg void OnFileExit();
    afx_msg void OnClose();
    afx_msg void OnEditMakeAVIFile();
    afx_msg void OnTimer(UINT_PTR nIDEvent);
    DECLARE_MESSAGE_MAP()
};

