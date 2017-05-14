#pragma once

#include <Stack.h>
#include <MFCUTIL/LayoutManager.h>
#include "ChessGraphics.h"
#include "GameEditHistory.h"
#include "ChessPlayer.h"
#include "StopWatch.h"
#include "TraceDlgThread.h"
#include <PropertyContainer.h>
#include "PieceValueDlg.h"

typedef enum {
  PLAYMODE
 ,EDITMODE
 ,DEBUGMODE
 ,AUTOPLAYMODE
 ,ANALYZEMODE
 ,KNIGHTROUTEMODE
} DialogMode;

class DialogSettings {
public:
  DialogMode m_mode;
  bool       m_moveBackwards;
  DialogSettings(DialogMode mode, bool moveBackwards=false) {
    m_mode          = mode;
    m_moveBackwards = moveBackwards;
  }
};

typedef enum {
  DIALOGMODE
 ,GAMERESULT
} DialogProperty;

typedef enum {
  UNDO_MOVE
 ,UNDO_PLY
 ,UNDO_ALL
} UndoMode;

#define SHOWWATCH_TIMER  1

#define MAXTIMERID 3

class CChessDlg : public CDialog, public PropertyChangeListener, public PropertyContainer, OptionsAccessor {
public:
  CChessDlg(const String &startupFileName, CWnd *pParent = NULL);
  CChessDlg(const String &name, const GameKey &startPosition, const GameHistory &history, int plyIndex, CWnd *pParent = NULL);
  ~CChessDlg();
  void toggleEnableTestMenu();
  void handlePropertyChanged(const PropertyContainer *source, int id, const void *oldValue, const void *newValue);
  friend class PlotWinsPValuesThread;
private:
  static int              s_instanceCount;
  static CTraceDlgThread *s_traceThread;     // only one instance of this
  const DialogMode        m_initialMode;
  const int               m_startPlyIndex;
  HACCEL                  m_accelTable;
  HICON                   m_hIcon;
  LayoutManager           m_layoutManager;
  const String            m_startupFileName;
  Game                    m_game, m_savedGame;
  GameResult              m_gameResult;
  ChessGraphics          *m_graphics;
  BYTE                    m_controlFlags;
  Stack<DialogSettings>   m_modeStack;
  int                     m_selectedPosition;
  PieceKey                m_selectedPiece, m_removedPiece;
  MoveAnnotation          m_selectedAnnotation;
  GameEditHistory         m_editHistory;
  BitSet8                 m_timersRunning;
  ChessWatch              m_watch;
  String                  m_origEscapeMenuText;
  ChessPlayer            *m_chessPlayer[2];
  PlotWinsPValuesThread  *m_plotThread;

  void   commonInit();
  void   attachPropertyContainers();
  void   detachAllPropertyContainers();
  void   setPiece(PieceKey key);
  int    getBoardPosition(const CPoint &p, bool screenRelative=false) const;
  void   setPlayerInTurn(Player player);
  inline Player getPlayerInTurn() const {
    return getCurrentGame().getPlayerInTurn();
  }
  inline int getPlyCount() const {
    return getCurrentGame().getPlyCount();
  }
  inline ChessPlayer &getChessPlayer(Player player) {
    return *m_chessPlayer[player];
  }
  void   setGameAfterPly(int ply);
  void   ajourMenuItemsEnableStatus();
  void   enableRemoteGameItems();
  void   enableLevelItems();
  void   enableUndoRedo();
  void   enableEditBoardMenuItems(     bool startEdit, bool editEnabled);
  void   enableStartThinking();
  void   enableClockControlMenu();
  void   enableHintItems();
  void   enableTestMenu(               bool enabled);
  void   enableTestMenuItems();
  void   setTestItemStates();
  void   setEscapeMenuText(int textId, bool enabled);
  void   resetEscapeMenuText(          bool enabled);
  void   buildAndMarkLanguageMenu();
  void   buildEngineMenues();
  void   addEnginesToMenu(HMENU menu, int startId);

  void   activateOptions();
  void   setView();
  void   setComputerPlayer(    Player computerPlayer);
  void   setGameResult(        GameResult gameResult);
  void   setValidateAfterEdit( bool validate        );
  void   setMoveBackwards(     bool moveBackwards   );
  void   setGameSettings();
  void   setLevel(int level);
  void   setBothAutoPlayLevel( int level);
  void   setWhiteAutoPlayLevel(int level);
  void   setBlackAutoPlayLevel(int level);
  void   setPlayerAutoPlayLevel(Player player, int level);
  String getFiftyMovesString() const;
  TimeLimit getTimeLimit() const;
  void   pushDialogMode(const DialogSettings &settings);
  void   popDialogMode();
  void   setDialogMode(const DialogSettings &settings);
  void   postCommand(int command);
  void   setWhiteExternEngine(int index);
  void   setBlackExternEngine(int index);
  void   setExternEngine(    Player player, int startMenuId, int index);
  void   enginePrintState(   Player player);
  void   engineEditSettings( Player player);
  void   notifyMove(                        const PrintableMove &move);
  void   notifyMove(         Player player, const PrintableMove &move);
  void   notifyGameChanged(                 const Game          &game);
  void   notifyGameChanged(  Player player, const Game          &game);
  inline bool   isRemote(    Player player) const {
    return m_chessPlayer[player]->isRemote();
  }
  inline bool   isRemoteGame() const {
    return isRemote(WHITEPLAYER) || isRemote(BLACKPLAYER);
  }
  inline bool   isThinking(         Player player) const {
    return m_chessPlayer[player]->isBusy();
  }
  inline bool   isThinking() const {
    return isThinking(WHITEPLAYER) || isThinking(BLACKPLAYER);
  }
  inline bool   isThinkingForHint() const {
    return isThinking() && getPlayerInTurn() == getHumanPlayer();
  }
  inline DialogMode getDialogMode() const {
    return m_modeStack.isEmpty() ? m_initialMode : m_modeStack.top().m_mode;
  }
  inline GameResult getGameResult() const {
    return m_gameResult;
  }
  void   startNewGame();
  void   invalidate();
  void   paintGamePosition();
  void   executeMove(const PrintableMove &m);
  void   executePromotion(PieceType promoteTo);
  void   selectAndExecutePromotion(const CPoint &point, MoveAnnotation annotation = NOANNOTATION);
  void   executeBackMove(const Move &m);
  void   unExecuteLastBackMove();
  void   unExecuteLastPly();
  void   editUndo(UndoMode mode);
  Game  &validateBeforeSave();
  void   invalidModeError(const TCHAR *method) const;
  void   errorMessage(const TCHAR *format,...) const;
  void   errorMessage(const Exception &e) const;
  void   OnLButtonDownPlayMode( UINT nFlags, CPoint point);
  void   OnMouseMovePlayMode(   UINT nFlags, CPoint point);
  void   OnLButtonUpPlayMode(   UINT nFlags, CPoint point);
  void   OnLButtonDownEditMode( UINT nFlags, CPoint point);
  void   OnMouseMoveEditMode(   UINT nFlags, CPoint point);
  void   OnLButtonUpEditMode(   UINT nFlags, CPoint point);
  void   OnLButtonDownDebugMode(UINT nFlags, CPoint point);
  void   OnMouseMoveDebugMode(  UINT nFlags, CPoint point);
  void   OnLButtonUpDebugMode(  UINT nFlags, CPoint point);
  void   OnLButtonDownKRMode(   UINT nFlags, CPoint point);
  void   OnMouseMoveKRMode(     UINT nFlags, CPoint point);
  void   OnLButtonUpKRMode(     UINT nFlags, CPoint point);
  void   startThinking();
  void   stopThinking();
  void   startFindHint(int timeout);

  inline bool isTimedGame() const {
    return (getDialogMode() == PLAYMODE) && (getOptions().getNormalPlayLevel() == LEVEL_TIMEDGAME);
  }
  inline bool isSpeedChess() const {
    return (getDialogMode() == PLAYMODE) && (getOptions().getNormalPlayLevel() == LEVEL_SPEEDCHESS);
  }
  inline bool isGameWithTimeLimit() const {
    return isSpeedChess() || isTimedGame();
  }
  inline bool startClockNeeded() const {
    return isGameWithTimeLimit() && (m_watch.getState() == CW_STOPPED);
  }
  inline char getVisibleClocks() const {
    return m_graphics->getVisibleClocks();
  }
  inline bool isThinktimeClockActive() const {
    return !isGameWithTimeLimit() && (getVisibleClocks() & (1<<getPlayerInTurn()));
  }

  void   setVisibleClocks();
  void   startClock();
  void   stopClock();
  void   startComputerTimeTimer();
  void   stopComputerTimeTimer();
  void   pauseClock();
  void   resumeClock();
  void   updateClock();
  void   setClockMenuText();
  void   startTimer(UINT timerId, UINT milliSeconds);
  void   stopTimer( UINT timerId);
  bool   isTimerRunning(UINT timerId) const;
  void   showHintMessageBox(const PrintableMove move);
  void   checkAutoplayMenuItems(bool restartChecked, bool noRestartChecked);
  void   handleEndGameForPlayMode(    const String &message, const String &caption, int verboseMessageId=0);
  void   handleEndGameForAutoPlayMode(const String &message, const String &caption);
  String getGameResultToString();

  bool   isTraceWindowVisible() const;
  void   setTraceWindowVisible(bool visible);
  void   setLanguage(int index);
  void   makePlot(const PValuePlotParameters &param);

  inline Player getComputerPlayer() const {
    return getOptions().getComputerPlayer();
  }

  inline Player getHumanPlayer() const {
    return GETENEMY(getComputerPlayer());
  }

  inline Game &getCurrentGame() {
    return getCurrentGame1();
  }
  inline const Game &getCurrentGame() const {
    return ((CChessDlg*)this)->getCurrentGame1();
  }
  Game &getCurrentGame1();
  void save(Game &game, const String &name);
  void load(const String &fileName);
  void newGame(Player computerPlayer);
  void updateTitle();
  void setGameName(const String &name);
  void saveInitialDir(const String &fileName);
  String getFileExtensions() const;
  void stopAllBackgroundActivity(bool stopAll);
  void applySymmetricTransformation(SymmetricTransformation st, bool paint=true);
  void setGameToPosition0();
  bool hasSamePartialHistory() const;

  enum { IDD = IDD_MAINFRAME };

protected:
  virtual BOOL PreTranslateMessage(MSG *pMsg);
  virtual void DoDataExchange(CDataExchange *pDX);
  afx_msg HCURSOR OnQueryDragIcon();
  afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
  virtual BOOL OnInitDialog();
  afx_msg void OnSizing(UINT fwSide, LPRECT pRect);
  afx_msg void OnPaint();
  afx_msg void OnClose();
  afx_msg void OnOK();
  afx_msg void OnCancel();
  afx_msg void OnActivate(UINT nState, CWnd *pWndOther, BOOL bMinimized);
  afx_msg void OnMouseMove(  UINT nFlags, CPoint point);
  afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
  afx_msg void OnLButtonUp(  UINT nFlags, CPoint point);
  afx_msg void OnContextMenu(CWnd *pWnd , CPoint point);
  afx_msg void OnTimer(      UINT_PTR nIDEvent);
  afx_msg void OnFileNewGameYouPlayWhite();
  afx_msg void OnFileNewGameYouPlayBlack();
  afx_msg void OnFileLoadGame();
  afx_msg void OnFilePlayRemote();
  afx_msg void OnFileDisconnectRemoteGame();
  afx_msg void OnFileSave();
  afx_msg void OnFileSaveAs();
  afx_msg void OnFileShowHistory();
  afx_msg void OnFileOfferDraw();
  afx_msg void OnFileResign();
  afx_msg void OnFileExit();
  afx_msg void OnEditUndo();
  afx_msg void OnEditUndoPly();
  afx_msg void OnEditUndoAll();
  afx_msg void OnEditRedo();
  afx_msg void OnEditCopyFEN();
  afx_msg void OnEditPasteFEN();
  afx_msg void OnEditStartSetup();
  afx_msg void OnEditTurnBoard();
  afx_msg void OnEditClearBoard();
  afx_msg void OnEditSetupStartPosition();
  afx_msg void OnEditSwapColors();
  afx_msg void OnEditMirrorColumn();
  afx_msg void OnEditMirrorRow();
  afx_msg void OnEditMirrorDiag1();
  afx_msg void OnEditMirrorDiag2();
  afx_msg void OnEditRotateRight();
  afx_msg void OnEditRotateLeft();
  afx_msg void OnEditRotate180();
  afx_msg void OnEditSwitchPlayerInTurn();
  afx_msg void OnEditEndSetup();
  afx_msg void OnEscape();
  afx_msg void OnEditValidateBoard();
  afx_msg void OnEditAddWhiteKing();
  afx_msg void OnEditAddWhiteQueen();
  afx_msg void OnEditAddWhiteRook();
  afx_msg void OnEditAddWhiteBishop();
  afx_msg void OnEditAddWhiteKnight();
  afx_msg void OnEditAddWhitePawn();
  afx_msg void OnEditAddBlackKing();
  afx_msg void OnEditAddBlackQueen();
  afx_msg void OnEditAddBlackRook();
  afx_msg void OnEditAddBlackBishop();
  afx_msg void OnEditAddBlackKnight();
  afx_msg void OnEditAddBlackPawn();
  afx_msg void OnEditRemove();
  afx_msg void OnSettingsStartThinking();
  afx_msg void OnMoveNow();
  afx_msg void OnAutoPlayRestartAtEnd();
  afx_msg void OnAutoPlayNoRestart();
  afx_msg void OnSettingsToggleClock();
  afx_msg void OnSettingsResetClock();
  afx_msg void OnAutoPlayBothLevel1();
  afx_msg void OnAutoPlayBothLevel2();
  afx_msg void OnAutoPlayBothLevel3();
  afx_msg void OnAutoPlayBothLevel4();
  afx_msg void OnAutoPlayBothLevel5();
  afx_msg void OnAutoPlayBothLevel6();
  afx_msg void OnAutoPlayWhiteLevel1();
  afx_msg void OnAutoPlayWhiteLevel2();
  afx_msg void OnAutoPlayWhiteLevel3();
  afx_msg void OnAutoPlayWhiteLevel4();
  afx_msg void OnAutoPlayWhiteLevel5();
  afx_msg void OnAutoPlayWhiteLevel6();
  afx_msg void OnAutoPlayBlackLevel1();
  afx_msg void OnAutoPlayBlackLevel2();
  afx_msg void OnAutoPlayBlackLevel3();
  afx_msg void OnAutoPlayBlackLevel4();
  afx_msg void OnAutoPlayBlackLevel5();
  afx_msg void OnAutoPlayBlackLevel6();
  afx_msg void OnView();
  afx_msg void OnLevel1();
  afx_msg void OnLevel2();
  afx_msg void OnLevel3();
  afx_msg void OnLevel4();
  afx_msg void OnLevel5();
  afx_msg void OnLevel6();
  afx_msg void OnLevelTimedGame();
  afx_msg void OnLevelSpeedChess();
  afx_msg void OnLevelEditTimeSettings();
  afx_msg void OnWhiteEngineSettings();
  afx_msg void OnWhiteEngineGetState();
  afx_msg void OnWhiteEngine0();
  afx_msg void OnWhiteEngine1();
  afx_msg void OnWhiteEngine2();
  afx_msg void OnWhiteEngine3();
  afx_msg void OnWhiteEngine4();
  afx_msg void OnWhiteEngine5();
  afx_msg void OnWhiteEngine6();
  afx_msg void OnWhiteEngine7();
  afx_msg void OnWhiteEngine8();
  afx_msg void OnWhiteEngine9();
  afx_msg void OnWhiteEngine10();
  afx_msg void OnWhiteEngine11();
  afx_msg void OnWhiteEngine12();
  afx_msg void OnWhiteEngine13();
  afx_msg void OnWhiteEngine14();
  afx_msg void OnWhiteEngine15();
  afx_msg void OnWhiteEngine16();
  afx_msg void OnWhiteEngine17();
  afx_msg void OnWhiteEngine18();
  afx_msg void OnWhiteEngine19();
  afx_msg void OnWhiteEngine20();
  afx_msg void OnBlackEngineSettings();
  afx_msg void OnBlackEngineGetState();
  afx_msg void OnBlackEngine0();
  afx_msg void OnBlackEngine1();
  afx_msg void OnBlackEngine2();
  afx_msg void OnBlackEngine3();
  afx_msg void OnBlackEngine4();
  afx_msg void OnBlackEngine5();
  afx_msg void OnBlackEngine6();
  afx_msg void OnBlackEngine7();
  afx_msg void OnBlackEngine8();
  afx_msg void OnBlackEngine9();
  afx_msg void OnBlackEngine10();
  afx_msg void OnBlackEngine11();
  afx_msg void OnBlackEngine12();
  afx_msg void OnBlackEngine13();
  afx_msg void OnBlackEngine14();
  afx_msg void OnBlackEngine15();
  afx_msg void OnBlackEngine16();
  afx_msg void OnBlackEngine17();
  afx_msg void OnBlackEngine18();
  afx_msg void OnBlackEngine19();
  afx_msg void OnBlackEngine20();
  afx_msg void OnExternEngineSwap();
  afx_msg void OnExternEngineRegister();
  afx_msg void OnOpeningLibraryEnabled();
  afx_msg void OnEndGameTablebaseEnabled();
  afx_msg void OnTablebaseSettings();
  afx_msg void OnSettingsLanguage0();
  afx_msg void OnSettingsLanguage1();
  afx_msg void OnSettingsLanguage2();
  afx_msg void OnSettingsLanguage3();
  afx_msg void OnSettingsLanguage4();
  afx_msg void OnSettingsLanguage5();
  afx_msg void OnSettingsLanguage6();
  afx_msg void OnSettingsLanguage7();
  afx_msg void OnSettingsLanguage8();
  afx_msg void OnSettingsLanguage9();
  afx_msg void OnTraceOpening();
  afx_msg void OnTraceWindow();
  afx_msg void OnHelpHint();
  afx_msg void OnHelpInfiniteHint();
  afx_msg void OnShowOpeningLibrary();
  afx_msg void OnHelpAboutChess();
  afx_msg void OnPromoteToQueen();
  afx_msg void OnPromoteToRook();
  afx_msg void OnPromoteToBishop();
  afx_msg void OnPromoteToKnight();
  afx_msg void OnTestDebug();
  afx_msg void OnTestFindknightroute();
  afx_msg void OnTestShowFieldAttacks();
  afx_msg void OnTestShowMaterial();
  afx_msg void OnTestShowBishopFlags();
  afx_msg void OnTestShowPawnCount();
  afx_msg void OnTestShowLastCapture();
  afx_msg void OnTestShowPositionRepeats();
  afx_msg void OnTestShowLastMoveInfo();
  afx_msg void OnTestShowCheckingSDAPos();
  afx_msg void OnTestShowSetupMode();
  afx_msg void OnTestShowMessage();
  afx_msg void OnTestMoveBackwards();
  afx_msg void OnTestGenerateFictivePawnCaptures();
  afx_msg void OnTestTransformEndGamePosition();
  afx_msg void OnTestNormalizeEndGamePosition();
  afx_msg void OnTestSaveToExternEngine();
  afx_msg void OnTestPlotWinsPValues();
  afx_msg void OnTestShowEngineConsole();
  afx_msg void OnTestShowFEN();
  afx_msg LRESULT OnMsgChessPlayerStateChanged(     WPARAM wp, LPARAM lp);
  afx_msg LRESULT OnMsgChessPlayerMsgChanged(       WPARAM wp, LPARAM lp);
  afx_msg LRESULT OnMsgEngineChanged(               WPARAM wp, LPARAM lp);
  afx_msg LRESULT OnMsgTraceWindowChanged(          WPARAM wp, LPARAM lp);
  afx_msg LRESULT OnMsgShowSelectedHistoryMove(     WPARAM wp, LPARAM lp);
  afx_msg LRESULT OnMsgRemoteStateChanged(          WPARAM wp, LPARAM lp);
  DECLARE_MESSAGE_MAP()
public:
};

