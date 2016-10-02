#pragma once

#include <Thread.h>
#include <SynchronizedQueue.h>
#include <PropertyChangeListener.h>

class CChessDlg;

class PValuePlotParameters {
public:
  UINT	    m_from;
  UINT	    m_to;
  UINT	    m_step;
  PieceType m_pieceType;
};

class PlotWinsPValuesThread : public Thread, public PropertyChangeListener {
private:
  CChessDlg                 &m_dlg;
  const PValuePlotParameters m_param;
  SynchronizedQueue<int>     m_msgQueue;
  void playTournament(Player playerToCount, int value, double &pctWin, double &pctLoose);
  void enableOpenLib(bool enabled);
  void swapEngines();
  void setLevel(int level);
  void startGame();
  void sendCommand(int cmd);
public:
  PlotWinsPValuesThread(CChessDlg *dlg, const PValuePlotParameters &param);
  ~PlotWinsPValuesThread();
  void handlePropertyChanged(const PropertyContainer *source, int id, const void *oldValue, const void *newValue);
  UINT run();
};

