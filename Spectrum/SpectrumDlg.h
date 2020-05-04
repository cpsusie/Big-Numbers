#pragma once

#include <MFCUtil/CoordinateSystem/CoordinateSystem.h>
#include <MFCUtil/MMCapture.h>
#include <FastSemaphore.h>

class CSpectrumDlg : public CDialog, public CaptureReceiver {
private:
    HICON             m_hIcon;
    HACCEL            m_accelTable;
    CCoordinateSystem m_waveSystem, m_frequenceSystem;
    MMCapture        *m_capture;
    unsigned int      m_beepFrequence;
    bool              m_firstSample;
    FastSemaphore     m_sampleDone;
    bool              m_handlingEvent;
public:
    CSpectrumDlg(CWnd *pParent = NULL);
  HWND getWindow() {
    return m_hWnd;
  }
  HWND getVideoWindow() {
    return m_hWnd;
  }
  LRESULT captureWaveStreamCallback(MMCapture &capture, WAVEHDR *audioHeader);

  enum { IDD = IDD_SPECTRUM_DIALOG };

public:
    virtual BOOL PreTranslateMessage(MSG *pMsg);
    virtual void DoDataExchange(CDataExchange *pDX);
    afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
    afx_msg HCURSOR OnQueryDragIcon();
    virtual BOOL OnInitDialog();
    afx_msg void OnPaint();
    afx_msg void OnSize(UINT nType, int cx, int cy);
    afx_msg void OnClose();
    virtual void OnOK();
    virtual void OnCancel();
    afx_msg void OnFileExit();
    afx_msg void OnCaptureStart();
    afx_msg void OnCaptureStop();
    afx_msg void OnSoundBeep();
    afx_msg void OnSoundSilent();
    afx_msg void OnSoundFrequence();
    DECLARE_MESSAGE_MAP()
};

