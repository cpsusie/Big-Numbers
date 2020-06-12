#pragma once

#include <MFCUtil/ViewPort2D.h>

typedef double AfinType[7];

#define BREGNE

#if defined(BREGNE)

#define AFIN_COUNT 4
#define MINX -6
#define MAXX 6
#define MINY 0
#define MAXY 10

#else

#define AFIN_COUNT 3
#define MINX -0.6
#define MAXX 2
#define MINY 0
#define MAXY 1

#endif

class CFernDlg : public CDialog {
private:
  HICON                   m_hIcon;
  bool                    m_timerIsRunning;
  RectangleTransformation m_tr;
  AfinType                m_afin[AFIN_COUNT];
  int                     m_currentAfin;
  Point2D                 m_currentPoint;
  double                  m_akkumulatedProbabilities[AFIN_COUNT];
  void initAfin();
  void initProbability();
  Point2D newPoint(AfinType a, const Point2D &point);
  void setNewPoint();
  void startTimer();
  void stopTimer();
  void afinToFields();
  void fieldsToAfin();

public:
    CFernDlg(CWnd *pParent = NULL);

    enum { IDD = IDD_FERN_DIALOG };
    double  m_x00;
    double  m_x01;
    double  m_x02;
    double  m_x03;
    double  m_x04;
    double  m_x05;
    double  m_x06;
    double  m_x10;
    double  m_x11;
    double  m_x12;
    double  m_x13;
    double  m_x14;
    double  m_x15;
    double  m_x16;
    double  m_x20;
    double  m_x21;
    double  m_x22;
    double  m_x23;
    double  m_x24;
    double  m_x25;
    double  m_x26;
    double  m_x30;
    double  m_x31;
    double  m_x32;
    double  m_x33;
    double  m_x34;
    double  m_x35;
    double  m_x36;

protected:
    virtual void DoDataExchange(CDataExchange *pDX);
    virtual BOOL OnInitDialog();
    afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
    afx_msg void OnPaint();
    afx_msg HCURSOR OnQueryDragIcon();
    afx_msg void OnTimer(UINT_PTR nIDEvent);
    afx_msg void OnSize(UINT nType, int cx, int cy);
    virtual void OnOK();
    virtual void OnCancel();
    afx_msg void OnClose();
    afx_msg void OnHelpAbout();
    afx_msg void OnEditParameters();
    afx_msg void OnUpdateEdit();
    afx_msg void OnButtonReset();
    DECLARE_MESSAGE_MAP()
public:
  afx_msg void OnFileExit();
};
