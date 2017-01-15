#pragma once

#include <MFCUtil/MMCapture.h>
#include <Date.h>


class CTestWebCamDlg : public CDialog, public CaptureReceiver {
private:
	HICON      m_hIcon;
  bool       m_timerIsRunning;
  bool       m_edgeDetectionOn;
  PixRect   *m_lastImage;
  MMCapture *m_capture;
  Semaphore  m_gate;
public:
	CTestWebCamDlg(CWnd *pParent = NULL);	// standard constructor
  HWND getWindow() { 
    return m_hWnd;
  }
  LRESULT captureVideoStreamCallback(MMCapture &capture, PixRect *image);
  LRESULT captureStatusCallback(     MMCapture &capture, int id, const TCHAR *description);
  LRESULT captureControlCallback(    MMCapture &capture, int state);
  LRESULT captureErrorCallback(      MMCapture &capture, int id, const TCHAR *message);
  void initLog();
  void vlog(const TCHAR *format, va_list argptr);
  void startTimer();
  void stopTimer();

	enum { IDD = IDD_TESTWEBCAM_DIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);

protected:
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnButtonStartCapture();
	afx_msg void OnButtonStopCapture();
	afx_msg void OnButtonStartTimer();
	afx_msg void OnButtonStopTimer();
	afx_msg void OnClose();
	afx_msg void OnMove(int x, int y);
	DECLARE_MESSAGE_MAP()
public:
  afx_msg void OnBnClickedOk();
  afx_msg void OnBnClickedCancel();
  afx_msg void OnFileExit();
  afx_msg void OnFiltersEdge();
};
