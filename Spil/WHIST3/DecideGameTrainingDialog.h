#pragma once

#include <Thread.h>
#include <Semaphore.h>
#include "GameTypes.h"

class DecideGameTrainingDialog;

class TrainerThread : public Thread {
private:
  DecideGameTrainingDialog    &m_dlg;
  Semaphore                    m_sync;
  volatile bool                m_doStop;
  DecideGameBpn                m_bpn;
  Array<DecideGameTrainingSet> m_trainingData;
  double                       m_errorSum;
  void loadBpn();
  void saveBpn();
  void learnTrainingSet(const DecideGameTrainingSet &data);
public:
  TrainerThread(DecideGameTrainingDialog *dlg);
  ~TrainerThread();
  UINT run();

  double getErrorSum() const;
  void stopTraining();
};

class DecideGameTrainingDialog : public CDialog {
private:
  DecideGameTrainingSet m_trainingSet;
  CSize                 m_cardFrameSize;
  HBRUSH                m_backgroundBrush;
  bool                  m_timerIsRunning;
  TrainerThread        *m_trainerThread;
  DecideGameBpn         m_bpn;
  GameType              getSelectedGameType() const;
  int                   getSelectedPointsPerTrick() const;
  CPoint getCardPosition(int index) const;
  void setDefaultSelection();
  void enableButtons();
  void generateRandomTestData();
  void startTimer();
  void stopTimer();
  void startTraining();
  void stopTraining();
public:
    DecideGameTrainingDialog(CWnd *pParent = NULL);

    enum { IDD = IDD_DECIDE_GAME_TRAINING_DIALOG };
    int     m_pointPerStik;
    int     m_gameType;

    virtual void DoDataExchange(CDataExchange *pDX);
    virtual BOOL OnInitDialog();
    afx_msg void OnPaint();
    afx_msg void OnButtonNextdata();
    afx_msg void OnButtonSave();
    afx_msg void OnButtontrain();
    afx_msg void OnButtondecide();
    afx_msg void OnTimer(UINT_PTR nIDEvent);
    DECLARE_MESSAGE_MAP()
};

