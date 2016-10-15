#include "stdafx.h"
#include "PlotWinsPValuesThread.h"
#include "ChessDlg.h"

#ifdef __NEVER__

PlotWinsPValuesThread::PlotWinsPValuesThread(CChessDlg *dlg, const PValuePlotParameters &param) : m_dlg(*dlg), m_param(param) {
  m_dlg.addPropertyChangeListener(this);
}

PlotWinsPValuesThread::~PlotWinsPValuesThread() {
  m_dlg.removePropertyChangeListener(this);
}

UINT PlotWinsPValuesThread::run() {
  const String fileName = format(_T("c:\\temp\\ChessWinPlot_%s_%d-%d"), getPieceTypeNameEnglish(m_param.m_pieceType), m_param.m_from, m_param.m_to);
  FILE *f = FOPEN(fileName, _T("w"));
  try {
    for(int v = m_param.m_from; v <= m_param.m_to; v += m_param.m_step) {
      m_dlg.m_moveFinder[WHITEPLAYER].getExternEngine().setPieceValue(m_param.m_pieceType, v);
      double pctWhiteWin, pctWhiteLoose;
      playTournament(WHITEPLAYER, v, pctWhiteWin, pctWhiteLoose);
      swapEngines();
      m_dlg.m_moveFinder[BLACKPLAYER].getExternEngine().setPieceValue(m_param.m_pieceType, v);
      double pctBlackWin, pctBlackLoose;
      playTournament(BLACKPLAYER, v, pctBlackWin, pctBlackLoose);
      swapEngines();

      _ftprintf(f, _T("%d %.2lf %.2lf\n"), v, (pctWhiteWin + pctBlackWin)/2, (pctWhiteLoose + pctBlackLoose)/2);
      fflush(f);
    }
  } catch(Exception e) {
    AfxMessageBox(e.what(), MB_ICONWARNING);
  }
  fclose(f);
  return 0;
}

#define GAME_OVER              0x01
#define TOURNAMENT_INTERRUPTED 0x02

void PlotWinsPValuesThread::playTournament(Player playerToCount, int value, double &pctWin, double &pctLoose) {
  int gameCount = 0, whiteWin = 0, blackWin = 0;
  enableOpenLib(false);
  for(int level = 1; level <= LEVELCOUNT; level++) {
    setLevel(level);
    startGame();
    m_dlg.SetWindowText(format(_T("Varierer %s %s [%d-%d] (=%d). Niveau:%d. Spil:%d, Stilling(%d/%d/%d)")
                              ,getPlayerName(playerToCount).cstr()
                              ,getPieceTypeName(m_param.m_pieceType).cstr()
                              ,m_param.m_from,m_param.m_to
                              ,value
                              ,level
                              ,gameCount
                              ,whiteWin
                              ,blackWin
                              ,gameCount - (whiteWin+blackWin)
                              ).cstr()
                       );
    gameCount++;
    const int cmd = m_msgQueue.get();
    switch(cmd) {
    case TOURNAMENT_INTERRUPTED:
      throwException(_T("Make plot interrupted"));
      break;
    case GAME_OVER:
      switch(m_dlg.getGameResult()) {
      case WHITE_CHECKMATE           :
        blackWin++;
        break;
      case BLACK_CHECKMATE           :
        whiteWin++;
        break;
      case STALEMATE                 :
#ifndef TABLEBASE_BUILDER
      case POSITION_REPEATED_3_TIMES :
      case NO_CAPTURE_OR_PAWNMOVE    :
      case DRAW                               :
#endif
        break;
      default:
        throwException(_T("playTournament:Unknown gameResult:%d"), m_dlg.getGameResult());
      }
    }
  }
  pctWin = pctLoose = 0;
  if(playerToCount == WHITEPLAYER) {
    pctWin   = whiteWin*100.0/gameCount;
    pctLoose = blackWin*100.0/gameCount;
  } else {
    pctWin   = blackWin*100.0/gameCount;
    pctLoose = whiteWin*100.0/gameCount;
  }
}

void PlotWinsPValuesThread::startGame() {
  sendCommand(ID_FILE_NEWGAME_YOUPLAYWHITE);
  sendCommand(ID_AUTOPLAY_NO_RESTART      );
}

void PlotWinsPValuesThread::enableOpenLib(bool enabled) {
  if(m_dlg.getOptions().isOpeningLibraryEnabled() != enabled) {
    sendCommand(ID_OPENINGLIBRARY_ENABLED);
  }
}

void PlotWinsPValuesThread::swapEngines() {
  sendCommand(ID_EXTERNENGINE_SWAP);
}

void PlotWinsPValuesThread::setLevel(int level) {
  int levelCommands[] = {
    0
   ,ID_AUTOPLAY_BOTH_LEVEL_1
   ,ID_AUTOPLAY_BOTH_LEVEL_2
   ,ID_AUTOPLAY_BOTH_LEVEL_3
   ,ID_AUTOPLAY_BOTH_LEVEL_4
   ,ID_AUTOPLAY_BOTH_LEVEL_5
   ,ID_AUTOPLAY_BOTH_LEVEL_6
  };

  sendCommand(levelCommands[level]);
}

void PlotWinsPValuesThread::sendCommand(int cmd) {
  m_dlg.SendMessage(WM_COMMAND, cmd);
}

void PlotWinsPValuesThread::handlePropertyChanged(const PropertyContainer *source, int id, const void *oldValue, const void *newValue) {
  if(source == &m_dlg) {
    switch(id) {
    case DIALOGMODE:
      { const DialogMode oldMode = *(const DialogMode*)oldValue;
        const DialogMode newMode = *(const DialogMode*)newValue;
        switch(newMode) {
        case PLAYMODE    :
          if(m_dlg.getGameResult() == NORESULT) { // interrupted
            m_msgQueue.put(TOURNAMENT_INTERRUPTED);
          } else if(oldMode == AUTOPLAYMODE) {
            m_msgQueue.put(GAME_OVER);
          }
          break;

        case AUTOPLAYMODE:
          break;
        }
      }
      break;
    case GAMERESULT:
      break;
    default        :
      AfxMessageBox(format(_T("Unknown propertyId:%d"), id).cstr(), MB_ICONSTOP);
      m_msgQueue.put(TOURNAMENT_INTERRUPTED);
      break;
    }
  }
}


#endif
