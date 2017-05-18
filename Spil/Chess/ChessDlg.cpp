#include "stdafx.h"
#include <Process.h>
#include <Language.h>
#include <MFCUtil/Clipboard.h>
#include "MFCUtil/ConfirmDlg.h"
#include "Chess.h"
#include "ChessDlg.h"
#include "AboutDlg.h"
#include "HistoryDlg.h"
#include "ConnectDlg.h"
#include "OpeningDlgThread.h"
#include "TimeSettingsDlg.h"
#include "EngineOptionsDlg.h"
#include "ViewDlg.h"
#include "EnginesDlg.h"
#include "TablebaseDlg.h"
#include "EnterTextDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#define CTRL_INITDONE         0x01
#define CTRL_FIRSTPAINTDONE   0x02
#define CTRL_THINKENABLED     0x04
#define CTRL_VERBOSEATGAMEEND 0x08
#define CTRL_AUTOUPDATETITLE  0x10
#define CTRL_APPACTIVE        0x20

#define setControlFlag(  flag)   m_controlFlags |=  (flag)
#define clrControlFlag(  flag)   m_controlFlags &= ~(flag)
#define isControlFlagSet(flag) ((m_controlFlags & (flag)) == (flag))

#define isInitDone()     isControlFlagSet(CTRL_INITDONE    )
#define isThinkEnabled() isControlFlagSet(CTRL_THINKENABLED)
#define isAppActive()    isControlFlagSet(CTRL_APPACTIVE   )

int              CChessDlg::s_instanceCount = 0;
CTraceDlgThread *CChessDlg::s_traceThread   = NULL;

#define BEGINPAINT() m_graphics->beginPaint()
#define ENDPAINT()   m_graphics->endPaint()

CChessDlg::CChessDlg(const String &startupFileName, CWnd* pParent)
: CDialog(CChessDlg::IDD, pParent)
, m_initialMode(PLAYMODE)
, m_startPlyIndex(0)
, m_startupFileName(startupFileName)
{
  commonInit();
}

CChessDlg::CChessDlg(const String &name, const GameKey &startPosition, const GameHistory &history, int plyIndex, CWnd *pParent)
: CDialog(CChessDlg::IDD, pParent)
, m_initialMode(ANALYZEMODE)
, m_startPlyIndex(plyIndex)
{
  commonInit();
  try {
    m_savedGame.setGameAfterPly(startPosition, history, (int)history.size()-1);
    m_savedGame.setName(name);
    setGameName(name);
  } catch(Exception e) {
    errorMessage(e);
  }
}

void CChessDlg::setGameToPosition0() {
  m_game.setGameAfterPly(m_savedGame, m_startPlyIndex);
  notifyGameChanged(m_game);
}

bool CChessDlg::hasSamePartialHistory() const {
  return Game::hasSamePartialHistory(m_savedGame, getCurrentGame());
}

void CChessDlg::commonInit() {
  m_hIcon          = theApp.LoadIcon(IDR_MAINFRAME);
  m_graphics       = NULL;
  m_gameResult     = NORESULT;
  m_selectedPiece  = m_removedPiece = EMPTYPIECEKEY;
  forEachPlayer(p) m_chessPlayer[p]  = new ChessPlayer(p);
  m_controlFlags   = CTRL_VERBOSEATGAMEEND | CTRL_AUTOUPDATETITLE | CTRL_APPACTIVE;

  if(s_instanceCount++ == 0) {
    // redirect verbose to traceWindow
    s_traceThread = CTraceDlgThread::startThread();
  }

}

CChessDlg::~CChessDlg() {
  forEachPlayer(p) delete m_chessPlayer[p];

  if(--s_instanceCount == 0) {
    s_traceThread->kill();
    s_traceThread = NULL;
  }
  if(m_graphics) {
    delete m_graphics;
  }
}

void CChessDlg::DoDataExchange(CDataExchange* pDX) {
  CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CChessDlg, CDialog)
  ON_WM_QUERYDRAGICON()
  ON_WM_SYSCOMMAND()
  ON_WM_SIZING()
  ON_WM_PAINT()
  ON_WM_CLOSE()
  ON_WM_ACTIVATE()
  ON_WM_MOUSEMOVE()
  ON_WM_LBUTTONDOWN()
  ON_WM_LBUTTONUP()
  ON_WM_CONTEXTMENU()
  ON_WM_TIMER()
  ON_COMMAND(ID_FILE_NEWGAME_YOUPLAYWHITE       , OnFileNewGameYouPlayWhite        )
  ON_COMMAND(ID_FILE_NEWGAME_YOUPLAYBLACK       , OnFileNewGameYouPlayBlack        )
  ON_COMMAND(ID_FILE_LOADGAME                   , OnFileLoadGame                   )
  ON_COMMAND(ID_FILE_PLAY_REMOTE                , OnFilePlayRemote                 )
  ON_COMMAND(ID_FILE_DISCONNECT_REMOTEGAME      , OnFileDisconnectRemoteGame       )
  ON_COMMAND(ID_FILE_SAVE                       , OnFileSave                       )
  ON_COMMAND(ID_FILE_SAVEAS                     , OnFileSaveAs                     )
  ON_COMMAND(ID_FILE_SHOWHISTORY                , OnFileShowHistory                )
  ON_COMMAND(ID_FILE_OFFERDRAW                  , OnFileOfferDraw                  )
  ON_COMMAND(ID_FILE_RESIGN                     , OnFileResign                     )
  ON_COMMAND(ID_FILE_EXIT                       , OnFileExit                       )
  ON_COMMAND(ID_EDIT_UNDO                       , OnEditUndo                       )
  ON_COMMAND(ID_EDIT_UNDOPLY                    , OnEditUndoPly                    )
  ON_COMMAND(ID_EDIT_UNDOALL                    , OnEditUndoAll                    )
  ON_COMMAND(ID_EDIT_REDO                       , OnEditRedo                       )
  ON_COMMAND(ID_EDIT_COPY_FEN                   , OnEditCopyFEN                    )
  ON_COMMAND(ID_EDIT_PASTE_FEN                  , OnEditPasteFEN                   )
  ON_COMMAND(ID_EDIT_STARTSETUP                 , OnEditStartSetup                 )
  ON_COMMAND(ID_EDIT_TURNBOARD                  , OnEditTurnBoard                  )
  ON_COMMAND(ID_EDIT_CLEARBOARD                 , OnEditClearBoard                 )
  ON_COMMAND(ID_EDIT_SETUPSTARTPOSITION         , OnEditSetupStartPosition         )
  ON_COMMAND(ID_EDIT_SWAPCOLORS                 , OnEditSwapColors                 )
  ON_COMMAND(ID_EDIT_MIRRORCOLUMN               , OnEditMirrorColumn               )
  ON_COMMAND(ID_EDIT_MIRRORROW                  , OnEditMirrorRow                  )
  ON_COMMAND(ID_EDIT_MIRRORDIAG1                , OnEditMirrorDiag1                )
  ON_COMMAND(ID_EDIT_MIRRORDIAG2                , OnEditMirrorDiag2                )
  ON_COMMAND(ID_EDIT_ROTATERIGHT                , OnEditRotateRight                )
  ON_COMMAND(ID_EDIT_ROTATELEFT                 , OnEditRotateLeft                 )
  ON_COMMAND(ID_EDIT_ROTATE180                  , OnEditRotate180                  )
  ON_COMMAND(ID_EDIT_SWITCHPLAYERINTURN         , OnEditSwitchPlayerInTurn         )
  ON_COMMAND(ID_EDIT_ENDSETUP                   , OnEditEndSetup                   )
  ON_COMMAND(ID_ESCAPE                          , OnEscape                         )
  ON_COMMAND(ID_EDIT_VALIDATE_BOARD             , OnEditValidateBoard              )
  ON_COMMAND(ID_EDIT_ADD_WHITE_KING             , OnEditAddWhiteKing               )
  ON_COMMAND(ID_EDIT_ADD_WHITE_QUEEN            , OnEditAddWhiteQueen              )
  ON_COMMAND(ID_EDIT_ADD_WHITE_ROOK             , OnEditAddWhiteRook               )
  ON_COMMAND(ID_EDIT_ADD_WHITE_BISHOP           , OnEditAddWhiteBishop             )
  ON_COMMAND(ID_EDIT_ADD_WHITE_KNIGHT           , OnEditAddWhiteKnight             )
  ON_COMMAND(ID_EDIT_ADD_WHITE_PAWN             , OnEditAddWhitePawn               )
  ON_COMMAND(ID_EDIT_ADD_BLACK_KING             , OnEditAddBlackKing               )
  ON_COMMAND(ID_EDIT_ADD_BLACK_QUEEN            , OnEditAddBlackQueen              )
  ON_COMMAND(ID_EDIT_ADD_BLACK_ROOK             , OnEditAddBlackRook               )
  ON_COMMAND(ID_EDIT_ADD_BLACK_BISHOP           , OnEditAddBlackBishop             )
  ON_COMMAND(ID_EDIT_ADD_BLACK_KNIGHT           , OnEditAddBlackKnight             )
  ON_COMMAND(ID_EDIT_ADD_BLACK_PAWN             , OnEditAddBlackPawn               )
  ON_COMMAND(ID_EDIT_REMOVE                     , OnEditRemove                     )
  ON_COMMAND(ID_SETTINGS_START_THINKING         , OnSettingsStartThinking          )
  ON_COMMAND(ID_MOVENOW                         , OnMoveNow                        )
  ON_COMMAND(ID_AUTOPLAY_RESTART_AT_END         , OnAutoPlayRestartAtEnd           )
  ON_COMMAND(ID_AUTOPLAY_NO_RESTART             , OnAutoPlayNoRestart              )
  ON_COMMAND(ID_SETTINGS_TOGGLECLOCK            , OnSettingsToggleClock            )
  ON_COMMAND(ID_SETTINGS_RESETCLOCK             , OnSettingsResetClock             )
  ON_COMMAND(ID_AUTOPLAY_BOTH_LEVEL_1           , OnAutoPlayBothLevel1             )
  ON_COMMAND(ID_AUTOPLAY_BOTH_LEVEL_2           , OnAutoPlayBothLevel2             )
  ON_COMMAND(ID_AUTOPLAY_BOTH_LEVEL_3           , OnAutoPlayBothLevel3             )
  ON_COMMAND(ID_AUTOPLAY_BOTH_LEVEL_4           , OnAutoPlayBothLevel4             )
  ON_COMMAND(ID_AUTOPLAY_BOTH_LEVEL_5           , OnAutoPlayBothLevel5             )
  ON_COMMAND(ID_AUTOPLAY_BOTH_LEVEL_6           , OnAutoPlayBothLevel6             )
  ON_COMMAND(ID_AUTOPLAY_WHITE_LEVEL_1          , OnAutoPlayWhiteLevel1            )
  ON_COMMAND(ID_AUTOPLAY_WHITE_LEVEL_2          , OnAutoPlayWhiteLevel2            )
  ON_COMMAND(ID_AUTOPLAY_WHITE_LEVEL_3          , OnAutoPlayWhiteLevel3            )
  ON_COMMAND(ID_AUTOPLAY_WHITE_LEVEL_4          , OnAutoPlayWhiteLevel4            )
  ON_COMMAND(ID_AUTOPLAY_WHITE_LEVEL_5          , OnAutoPlayWhiteLevel5            )
  ON_COMMAND(ID_AUTOPLAY_WHITE_LEVEL_6          , OnAutoPlayWhiteLevel6            )
  ON_COMMAND(ID_AUTOPLAY_BLACK_LEVEL_1          , OnAutoPlayBlackLevel1            )
  ON_COMMAND(ID_AUTOPLAY_BLACK_LEVEL_2          , OnAutoPlayBlackLevel2            )
  ON_COMMAND(ID_AUTOPLAY_BLACK_LEVEL_3          , OnAutoPlayBlackLevel3            )
  ON_COMMAND(ID_AUTOPLAY_BLACK_LEVEL_4          , OnAutoPlayBlackLevel4            )
  ON_COMMAND(ID_AUTOPLAY_BLACK_LEVEL_5          , OnAutoPlayBlackLevel5            )
  ON_COMMAND(ID_AUTOPLAY_BLACK_LEVEL_6          , OnAutoPlayBlackLevel6            )
  ON_COMMAND(ID_VIEW                            , OnView                           )
  ON_COMMAND(ID_LEVEL_1                         , OnLevel1                         )
  ON_COMMAND(ID_LEVEL_2                         , OnLevel2                         )
  ON_COMMAND(ID_LEVEL_3                         , OnLevel3                         )
  ON_COMMAND(ID_LEVEL_4                         , OnLevel4                         )
  ON_COMMAND(ID_LEVEL_5                         , OnLevel5                         )
  ON_COMMAND(ID_LEVEL_6                         , OnLevel6                         )
  ON_COMMAND(ID_LEVEL_TIMED_GAME                , OnLevelTimedGame                 )
  ON_COMMAND(ID_LEVEL_SPEEDCHESS                , OnLevelSpeedChess                )
  ON_COMMAND(ID_LEVEL_EDIT_TIMESETTINGS         , OnLevelEditTimeSettings          )
  ON_COMMAND(ID_WHITE_ENGINE_SETTINGS           , OnWhiteEngineSettings            )
  ON_COMMAND(ID_WHITE_ENGINE_GETSTATE           , OnWhiteEngineGetState            )
  ON_COMMAND(ID_WHITE_EXTERNENGINE0             , OnWhiteEngine0                   )
  ON_COMMAND(ID_WHITE_EXTERNENGINE1             , OnWhiteEngine1                   )
  ON_COMMAND(ID_WHITE_EXTERNENGINE2             , OnWhiteEngine2                   )
  ON_COMMAND(ID_WHITE_EXTERNENGINE3             , OnWhiteEngine3                   )
  ON_COMMAND(ID_WHITE_EXTERNENGINE4             , OnWhiteEngine4                   )
  ON_COMMAND(ID_WHITE_EXTERNENGINE5             , OnWhiteEngine5                   )
  ON_COMMAND(ID_WHITE_EXTERNENGINE6             , OnWhiteEngine6                   )
  ON_COMMAND(ID_WHITE_EXTERNENGINE7             , OnWhiteEngine7                   )
  ON_COMMAND(ID_WHITE_EXTERNENGINE8             , OnWhiteEngine8                   )
  ON_COMMAND(ID_WHITE_EXTERNENGINE9             , OnWhiteEngine9                   )
  ON_COMMAND(ID_WHITE_EXTERNENGINE10            , OnWhiteEngine10                  )
  ON_COMMAND(ID_WHITE_EXTERNENGINE11            , OnWhiteEngine11                  )
  ON_COMMAND(ID_WHITE_EXTERNENGINE12            , OnWhiteEngine12                  )
  ON_COMMAND(ID_WHITE_EXTERNENGINE13            , OnWhiteEngine13                  )
  ON_COMMAND(ID_WHITE_EXTERNENGINE14            , OnWhiteEngine14                  )
  ON_COMMAND(ID_WHITE_EXTERNENGINE15            , OnWhiteEngine15                  )
  ON_COMMAND(ID_WHITE_EXTERNENGINE16            , OnWhiteEngine16                  )
  ON_COMMAND(ID_WHITE_EXTERNENGINE17            , OnWhiteEngine17                  )
  ON_COMMAND(ID_WHITE_EXTERNENGINE18            , OnWhiteEngine18                  )
  ON_COMMAND(ID_WHITE_EXTERNENGINE19            , OnWhiteEngine19                  )
  ON_COMMAND(ID_WHITE_EXTERNENGINE20            , OnWhiteEngine20                  )
  ON_COMMAND(ID_BLACK_ENGINE_SETTINGS           , OnBlackEngineSettings            )
  ON_COMMAND(ID_BLACK_ENGINE_GETSTATE           , OnBlackEngineGetState            )
  ON_COMMAND(ID_BLACK_EXTERNENGINE0             , OnBlackEngine0                   )
  ON_COMMAND(ID_BLACK_EXTERNENGINE1             , OnBlackEngine1                   )
  ON_COMMAND(ID_BLACK_EXTERNENGINE2             , OnBlackEngine2                   )
  ON_COMMAND(ID_BLACK_EXTERNENGINE3             , OnBlackEngine3                   )
  ON_COMMAND(ID_BLACK_EXTERNENGINE4             , OnBlackEngine4                   )
  ON_COMMAND(ID_BLACK_EXTERNENGINE5             , OnBlackEngine5                   )
  ON_COMMAND(ID_BLACK_EXTERNENGINE6             , OnBlackEngine6                   )
  ON_COMMAND(ID_BLACK_EXTERNENGINE7             , OnBlackEngine7                   )
  ON_COMMAND(ID_BLACK_EXTERNENGINE8             , OnBlackEngine8                   )
  ON_COMMAND(ID_BLACK_EXTERNENGINE9             , OnBlackEngine9                   )
  ON_COMMAND(ID_BLACK_EXTERNENGINE10            , OnBlackEngine10                  )
  ON_COMMAND(ID_BLACK_EXTERNENGINE11            , OnBlackEngine11                  )
  ON_COMMAND(ID_BLACK_EXTERNENGINE12            , OnBlackEngine12                  )
  ON_COMMAND(ID_BLACK_EXTERNENGINE13            , OnBlackEngine13                  )
  ON_COMMAND(ID_BLACK_EXTERNENGINE14            , OnBlackEngine14                  )
  ON_COMMAND(ID_BLACK_EXTERNENGINE15            , OnBlackEngine15                  )
  ON_COMMAND(ID_BLACK_EXTERNENGINE16            , OnBlackEngine16                  )
  ON_COMMAND(ID_BLACK_EXTERNENGINE17            , OnBlackEngine17                  )
  ON_COMMAND(ID_BLACK_EXTERNENGINE18            , OnBlackEngine18                  )
  ON_COMMAND(ID_BLACK_EXTERNENGINE19            , OnBlackEngine19                  )
  ON_COMMAND(ID_BLACK_EXTERNENGINE20            , OnBlackEngine20                  )
  ON_COMMAND(ID_EXTERNENGINE_SWAP               , OnExternEngineSwap               )
  ON_COMMAND(ID_EXTERNENGINE_REGISTER           , OnExternEngineRegister           )
  ON_COMMAND(ID_OPENINGLIBRARY_ENABLED          , OnOpeningLibraryEnabled          )
  ON_COMMAND(ID_ENDGAMETABLEBASE_ENABLED        , OnEndGameTablebaseEnabled        )
  ON_COMMAND(ID_TABLEBASE_SETTINGS              , OnTablebaseSettings              )
  ON_COMMAND(ID_SETTINGS_LANGUAGE0              , OnSettingsLanguage0              )
  ON_COMMAND(ID_SETTINGS_LANGUAGE1              , OnSettingsLanguage1              )
  ON_COMMAND(ID_SETTINGS_LANGUAGE2              , OnSettingsLanguage2              )
  ON_COMMAND(ID_SETTINGS_LANGUAGE3              , OnSettingsLanguage3              )
  ON_COMMAND(ID_SETTINGS_LANGUAGE4              , OnSettingsLanguage4              )
  ON_COMMAND(ID_SETTINGS_LANGUAGE5              , OnSettingsLanguage5              )
  ON_COMMAND(ID_SETTINGS_LANGUAGE6              , OnSettingsLanguage6              )
  ON_COMMAND(ID_SETTINGS_LANGUAGE7              , OnSettingsLanguage7              )
  ON_COMMAND(ID_SETTINGS_LANGUAGE8              , OnSettingsLanguage8              )
  ON_COMMAND(ID_SETTINGS_LANGUAGE9              , OnSettingsLanguage9              )
  ON_COMMAND(ID_VIEW_TRACEOPENING               , OnTraceOpening                   )
  ON_COMMAND(ID_VIEW_TRACEWINDOW                , OnTraceWindow                    )
  ON_COMMAND(ID_HELP_HINT                       , OnHelpHint                       )
  ON_COMMAND(ID_HELP_INFINITEHINT               , OnHelpInfiniteHint               )
  ON_COMMAND(ID_HELP_SHOWOPENINGLIBRARY         , OnShowOpeningLibrary             )
  ON_COMMAND(ID_HELP_ABOUTCHESS                 , OnHelpAboutChess                 )
  ON_COMMAND(ID_PROMOTE_TO_QUEEN                , OnPromoteToQueen                 )
  ON_COMMAND(ID_PROMOTE_TO_ROOK                 , OnPromoteToRook                  )
  ON_COMMAND(ID_PROMOTE_TO_BISHOP               , OnPromoteToBishop                )
  ON_COMMAND(ID_PROMOTE_TO_KNIGHT               , OnPromoteToKnight                )
  ON_COMMAND(ID_TEST_DEBUG                      , OnTestDebug                      )
  ON_COMMAND(ID_TEST_FINDKNIGHTROUTE            , OnTestFindknightroute            )
  ON_COMMAND(ID_TEST_SHOWFIELDATTACKS           , OnTestShowFieldAttacks           )
  ON_COMMAND(ID_TEST_SHOWMATERIAL               , OnTestShowMaterial               )
  ON_COMMAND(ID_TEST_SHOWBISHOPFLAGS            , OnTestShowBishopFlags            )
  ON_COMMAND(ID_TEST_SHOWPAWNCOUNT              , OnTestShowPawnCount              )
  ON_COMMAND(ID_TEST_SHOWLASTCAPTURE            , OnTestShowLastCapture            )
  ON_COMMAND(ID_TEST_SHOWPOSITIONREPEATS        , OnTestShowPositionRepeats        )
  ON_COMMAND(ID_TEST_SHOWLASTMOVEINFO           , OnTestShowLastMoveInfo           )
  ON_COMMAND(ID_TEST_SHOWCHECKINGSDAPOS         , OnTestShowCheckingSDAPos         )
  ON_COMMAND(ID_TEST_SHOWSETUPMODE              , OnTestShowSetupMode              )
  ON_COMMAND(ID_TEST_SHOWMESSAGE                , OnTestShowMessage                )
  ON_COMMAND(ID_TEST_MOVEBACKWARDS              , OnTestMoveBackwards              )
  ON_COMMAND(ID_TEST_FICTIVEPAWNCAPTURES        , OnTestGenerateFictivePawnCaptures)
  ON_COMMAND(ID_TEST_TRANSFORMPOSITION          , OnTestTransformEndGamePosition   )
  ON_COMMAND(ID_TEST_NORMALIZEPOSITION          , OnTestNormalizeEndGamePosition   )
  ON_COMMAND(ID_TEST_SAVETOEXTERNENGINE         , OnTestSaveToExternEngine         )
  ON_COMMAND(ID_TEST_SHOWENGINECONSOLE          , OnTestShowEngineConsole          )
  ON_COMMAND(ID_TEST_SHOWFEN                    , OnTestShowFEN                    )
  ON_MESSAGE(ID_MSG_CHESSPLAYERSTATE_CHANGED    , OnMsgChessPlayerStateChanged     )
  ON_MESSAGE(ID_MSG_CHESSPLAYERMSG_CHANGED      , OnMsgChessPlayerMsgChanged       )
  ON_MESSAGE(ID_MSG_ENGINE_CHANGED              , OnMsgEngineChanged               )
  ON_MESSAGE(ID_MSG_TRACEWINDOW_CHANGED         , OnMsgTraceWindowChanged          )
  ON_MESSAGE(ID_MSG_SHOW_SELECTED_MOVE          , OnMsgShowSelectedHistoryMove     )
  ON_MESSAGE(ID_MSG_REMOTESTATE_CHANGED         , OnMsgRemoteStateChanged          )
END_MESSAGE_MAP()

HCURSOR CChessDlg::OnQueryDragIcon() {
  return (HCURSOR)m_hIcon;
}

void CChessDlg::OnSysCommand(UINT nID, LPARAM lParam) {
  if((nID & 0xFFF0) == IDM_ABOUTBOX) {
    CAboutDlg(this).DoModal();
  } else {
    CDialog::OnSysCommand(nID, lParam);
  }
}

void CChessDlg::OnClose()  { OnFileExit(); }
void CChessDlg::OnOK()     {}
void CChessDlg::OnCancel() {}

void CChessDlg::OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized) {
  CDialog::OnActivate(nState, pWndOther, bMinimized);
  switch(nState) {
  case WA_INACTIVE    :
    clrControlFlag(CTRL_APPACTIVE);
    m_graphics->unmarkMouse();
    break;
  case WA_ACTIVE      :
  case WA_CLICKACTIVE :
    setControlFlag(CTRL_APPACTIVE);
    break;
  }
}

BOOL CChessDlg::OnInitDialog() {
  CDialog::OnInitDialog();

  ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
  ASSERT(IDM_ABOUTBOX < 0xF000);

  CMenu* pSysMenu = GetSystemMenu(FALSE);
  if(pSysMenu != NULL) {
    CString strAboutMenu;
    strAboutMenu.LoadString(IDS_ABOUTBOX);
    if(!strAboutMenu.IsEmpty()) {
      pSysMenu->AppendMenu(MF_SEPARATOR);
      pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
    }
  }

  SetIcon(m_hIcon, TRUE );        // Set big icon
  SetIcon(m_hIcon, FALSE);        // Set small icon

  theApp.m_device.attach(m_hWnd);
  m_graphics = new ChessGraphics(this);

  m_accelTable = LoadAccelerators(theApp.m_hInstance, MAKEINTRESOURCE(IDR_MAINFRAME));

  setControlText(IDD, this);
  AttackSetInfo::initArray();
  buildAndMarkLanguageMenu();
  buildEngineMenues();

  attachPropertyContainers();

  pushDialogMode(m_initialMode);

  activateOptions();
  setTestItemStates();
  centerWindow(this);

  m_layoutManager.OnInitDialog(this, RETAIN_ASPECTRATIO);
  switch(getDialogMode()) {
  case PLAYMODE:
    if(m_startupFileName != EMPTYSTRING) {
      try {
        load(m_startupFileName);
      } catch(Exception e) {
        errorMessage(e);
      }
    } else {
      startNewGame();
    }
    break;
  case ANALYZEMODE:
    setMenuItemText(this, ID_ESCAPE, loadString(IDS_MOVETOSTART));
    removeSubMenuContainingId(this, ID_FILE_NEWGAME_YOUPLAYWHITE   );
    removeMenuItem(           this, ID_EDIT_PASTE_FEN              );
    removeMenuItem(           this, ID_EDIT_STARTSETUP             );
    removeMenuItem(           this, ID_EDIT_CLEARBOARD             );
    removeMenuItem(           this, ID_EDIT_SETUPSTARTPOSITION     );
    removeMenuItem(           this, ID_EDIT_ENDSETUP               );
    removeMenuItem(           this, ID_EDIT_VALIDATE_BOARD         );
    removeMenuItem(           this, ID_EDIT_SWITCHPLAYERINTURN     );
    removeSubMenuContainingId(this, ID_EDIT_SWAPCOLORS             );
    removeSubMenuContainingId(this, ID_LEVEL_EDIT_TIMESETTINGS     );
    removeMenuItem(           this, ID_HELP_ABOUTCHESS             );
    setGameToPosition0();
    break;
  default:
    errorMessage(_T("Invalid initial state for dialog (=%d)"), m_initialMode);
  }

//  traceThread->reposition();
#ifdef TABLEBASE_BUILDER
  enableMenuItem(this,ID_TEST_MOVEBACKWARDS      ,true);
  enableMenuItem(this,ID_TEST_FICTIVEPAWNCAPTURES,true);
#endif

  setControlFlag(CTRL_INITDONE | CTRL_THINKENABLED);

  return TRUE;  // return TRUE  unless you set the focus to a control
}

void CChessDlg::attachPropertyContainers() {
  s_traceThread->addPropertyChangeListener(  this);
  m_graphics->addPropertyChangeListener(     this);
  forEachPlayer(p) {
    ChessPlayer &cp = getChessPlayer(p);
    cp.addPropertyChangeListener(  this);
    s_traceThread->addPropertyChangeListener(&cp);
    cp.start();
  }
}

void CChessDlg::detachAllPropertyContainers() {
  m_graphics->removePropertyChangeListener(     this);
  s_traceThread->removePropertyChangeListener(  this);
  forEachPlayer(p) {
    ChessPlayer &cp = getChessPlayer(p);
    cp.removePropertyChangeListener(  this);
    s_traceThread->removePropertyChangeListener(&cp);
  }
}

#define ID_SETTINGS_LANGUAGE(i) (ID_SETTINGS_LANGUAGE0+i)
void CChessDlg::buildAndMarkLanguageMenu() {
  const Array<Language> &spla = Language::getSupportedLanguages();
  int index;
  HMENU languageMenu = findMenuContainingId(*GetMenu(), ID_LANGUAGE_DUMMY, index);
  removeAllMenuItems(languageMenu); // remove dummy item
  for(int i = 0; i < (int)spla.size(); i++) {
    insertMenuItem(languageMenu,i, spla[i].getLanguageName(),  ID_SETTINGS_LANGUAGE(i));
  }
  checkMenuItem(this, ID_SETTINGS_LANGUAGE(getOptions().getSelectedLanguageIndex()), true);
}

void CChessDlg::invalidModeError(const TCHAR *method) const {
  throwException(_T("%s:Invalid mode:%d"), method, getDialogMode());
}

void CChessDlg::errorMessage(const TCHAR *format, ...) const {
  va_list argptr;
  va_start(argptr,format);
  String msg = vformat(format, argptr);
  va_end(argptr);
  ((CDialog*)this)->MessageBox(msg.cstr(), loadString(IDS_ERRORLABEL).cstr(), MB_OK | MB_ICONERROR);
  ((CChessDlg*)this)->OnWhiteEngineGetState();
  ((CChessDlg*)this)->OnBlackEngineGetState();
  verbose(_T("Current game-history:\n%s\n"), getCurrentGame().getHistory().toString().cstr());
}

void CChessDlg::errorMessage(const Exception &e) const {
  errorMessage(_T("%s"), e.what());
}

void CChessDlg::OnSizing(UINT fwSide, LPRECT pRect) {
  m_layoutManager.OnSizing(fwSide, pRect);
  CDialog::OnSizing(fwSide, pRect);
  BEGINPAINT();
  ENDPAINT();
  getOptions().setBoardSize(m_graphics->getBoardSize(true));
}

// ------------------------- Painting ---------------------------

void CChessDlg::OnPaint() {
  if(IsIconic()) {
    CPaintDC dc(this); // device context for painting

    SendMessage(WM_ICONERASEBKGND, (WPARAM) dc.GetSafeHdc(), 0);

    // Center icon in client rectangle
    const int   cxIcon = GetSystemMetrics(SM_CXICON);
    const int   cyIcon = GetSystemMetrics(SM_CYICON);
    const CRect rect   = getClientRect(this);
    const int   x      = (rect.Width() - cxIcon + 1) / 2;
    const int   y      = (rect.Height() - cyIcon + 1) / 2;

    dc.DrawIcon(x, y, m_hIcon);
  } else {
    CDialog::OnPaint();

    BEGINPAINT();
    if(!isControlFlagSet(CTRL_FIRSTPAINTDONE)) {
      ::SetActiveWindow(m_hWnd);
      m_graphics->paintAll();
      setControlFlag(CTRL_FIRSTPAINTDONE);
    }
    ENDPAINT();
  }
}

void CChessDlg::invalidate() {
  if(isInitDone()) {
    Invalidate(FALSE);
  }
}

void CChessDlg::paintGamePosition() {
  BEGINPAINT();
  try {
    switch(getDialogMode()) {
    case PLAYMODE:
      if(getCurrentGame().getPlyCount() > 0 && getPlayerInTurn() == getHumanPlayer()) { // => last move was by the computer.
        m_graphics->markLastMoveAsComputerMove();
      } else {
        m_graphics->unmarkLastMove();
      }
      break;
    case AUTOPLAYMODE:
      m_graphics->markLastMoveAsComputerMove();
      break;
    }
    m_graphics->paintGamePositions();
    ENDPAINT();
  } catch(Exception e) {
    ENDPAINT();
    errorMessage(e);
  }
}

// ------------------------- Menu handling ---------------------------

void CChessDlg::ajourMenuItemsEnableStatus() {
  const DialogMode mode = getDialogMode();
  switch(mode) {
  case PLAYMODE :
    enableEditBoardMenuItems(!isRemoteGame(), false);
    break;

  case EDITMODE :
    enableEditBoardMenuItems(false,true);
    break;

  case DEBUGMODE:
    enableEditBoardMenuItems(true,false);
    break;

  case AUTOPLAYMODE:
    enableEditBoardMenuItems(false,false);
    break;

  case ANALYZEMODE:
    setMenuItemText(this, ID_EDIT_UNDO, loadString(IDS_MOVEBACKWARD));
    setMenuItemText(this, ID_EDIT_REDO, loadString(IDS_MOVEFORWARD ));
    break;

  case KNIGHTROUTEMODE:
    enableEditBoardMenuItems(false,false);
    break;

  default:
    invalidModeError(__TFUNCTION__);
    break;
  }
  enableSubMenuContainingId(this, ID_FILE_NEWGAME_YOUPLAYWHITE, (mode == PLAYMODE) || (mode == DEBUGMODE));
  enableRemoteGameItems();
  enableMenuItem(           this, ID_FILE_SHOWHISTORY         , (mode == PLAYMODE) || (mode == DEBUGMODE));
  enableMenuItem(           this, ID_FILE_RESIGN              , (mode == PLAYMODE) && (getPlyCount() > 0));
  updateEscapeMenuItem();
  enableUndoRedo();
  enableStartThinking();
  enableMenuItem(           this, ID_TABLEBASE_SETTINGS       , isMenuItemChecked(this, ID_ENDGAMETABLEBASE_ENABLED));
  enableSubMenuContainingId(this, ID_AUTOPLAY_RESTART_AT_END  , mode == PLAYMODE);
  updateLevelItemsText();
  enableLevelItems();
  enableHintItems();
  enableTestMenuItems();
}

void CChessDlg::enableRemoteGameItems() {
  const bool isRemote = isRemoteGame();
  enableMenuItem(           this, ID_FILE_PLAY_REMOTE          , !isRemote);
  enableMenuItem(           this, ID_FILE_DISCONNECT_REMOTEGAME,  isRemote);
}

void CChessDlg::enableUndoRedo() {
  bool undo,redo;
  switch(getDialogMode()) {
  case PLAYMODE    : if(isRemoteGame()) { undo = redo = false; break; } // continue case
  case DEBUGMODE   : undo = getPlyCount() > m_startPlyIndex;    redo = false;                   break;
  case EDITMODE    : undo = m_editHistory.canUndo();            redo = m_editHistory.canRedo(); break;
  case AUTOPLAYMODE: undo = false;                              redo = false;                   break;
  case ANALYZEMODE : undo = getPlyCount() > m_startPlyIndex+1;  redo = (getPlyCount() < m_savedGame.getPlyCount()) && hasSamePartialHistory();
                     break;
  case KNIGHTROUTEMODE: undo = false;                              redo = false;                   break;
  default          : invalidModeError(__TFUNCTION__);
  }
  enableMenuItem(this, ID_EDIT_UNDO              , undo);
  enableMenuItem(this, ID_EDIT_UNDOALL           , undo);
  enableMenuItem(this, ID_EDIT_REDO              , redo);
}

void CChessDlg::enableEditBoardMenuItems(bool startEdit, bool editEnabled) {
  if(getDialogMode() == ANALYZEMODE) {
    return; // they are removed in initdialog
  }
  enableMenuItem(           this, ID_EDIT_STARTSETUP        , startEdit  );
  enableMenuItem(           this, ID_EDIT_PASTE_FEN         , editEnabled);
  enableMenuItem(           this, ID_EDIT_ENDSETUP          , editEnabled);
  enableMenuItem(           this, ID_EDIT_CLEARBOARD        , editEnabled);
  enableMenuItem(           this, ID_EDIT_SETUPSTARTPOSITION, editEnabled);
  enableSubMenuContainingId(this, ID_EDIT_SWAPCOLORS        , editEnabled);
  enableMenuItem(           this, ID_EDIT_SWITCHPLAYERINTURN, editEnabled);
}

void CChessDlg::enableStartThinking() {
  bool moveNowEnabled = false;
  bool thinkEnabled   = false;
  switch(getDialogMode()) {
  case PLAYMODE   :
    moveNowEnabled = isThinking() && !isRemote(getPlayerInTurn());
    thinkEnabled   = !moveNowEnabled && (getPlayerInTurn() == getComputerPlayer());
    break;
  case DEBUGMODE  :
    moveNowEnabled = isThinking();
    break;
  case ANALYZEMODE:
    moveNowEnabled = isThinking();
    thinkEnabled   = !moveNowEnabled;
    break;
  default:
    break;
  }
  enableMenuItem(this, ID_MOVENOW                , moveNowEnabled);
  enableMenuItem(this, ID_SETTINGS_START_THINKING, thinkEnabled  );
}

typedef struct {
  int m_level;
  int m_menuId;
} LevelMenuItem;

void CChessDlg::updateLevelItemsText() {
  const LevelMenuItem items[] = {
    1, ID_AUTOPLAY_BOTH_LEVEL_1
   ,2, ID_AUTOPLAY_BOTH_LEVEL_2
   ,3, ID_AUTOPLAY_BOTH_LEVEL_3
   ,4, ID_AUTOPLAY_BOTH_LEVEL_4
   ,5, ID_AUTOPLAY_BOTH_LEVEL_5
   ,6, ID_AUTOPLAY_BOTH_LEVEL_6
   ,1, ID_AUTOPLAY_WHITE_LEVEL_1
   ,2, ID_AUTOPLAY_WHITE_LEVEL_2
   ,3, ID_AUTOPLAY_WHITE_LEVEL_3
   ,4, ID_AUTOPLAY_WHITE_LEVEL_4
   ,5, ID_AUTOPLAY_WHITE_LEVEL_5
   ,6, ID_AUTOPLAY_WHITE_LEVEL_6
   ,1, ID_AUTOPLAY_BLACK_LEVEL_1
   ,2, ID_AUTOPLAY_BLACK_LEVEL_2
   ,3, ID_AUTOPLAY_BLACK_LEVEL_3
   ,4, ID_AUTOPLAY_BLACK_LEVEL_4
   ,5, ID_AUTOPLAY_BLACK_LEVEL_5
   ,6, ID_AUTOPLAY_BLACK_LEVEL_6
   ,1, ID_LEVEL_1
   ,2, ID_LEVEL_2
   ,3, ID_LEVEL_3
   ,4, ID_LEVEL_4
   ,5, ID_LEVEL_5
   ,6, ID_LEVEL_6
  };
  for(int i = 0; i < ARRAYSIZE(items); i++) {
    const LevelMenuItem &item = items[i];
    const double   timeout = getOptions().getLevelTimeout().getTimeout(item.m_level);
    String         s       = saveOrigMenuText(item.m_menuId);
    const intptr_t tabPos  = s.find('\t');
    const String   secStr  = format(_T("%s%lg sec."), (tabPos < 0) ? _T("\t") : _T(" "), timeout);
    if(tabPos >= 0) {
      s.insert(tabPos, secStr);
    } else {
      s += secStr;
    }
    setMenuItemText(this, item.m_menuId, s);
  }
}

void CChessDlg::enableLevelItems() {
  bool enabled = true;
  switch(m_initialMode) {
  case PLAYMODE:
    if(getDialogMode() == PLAYMODE) {
      enabled = !isThinking();
    }
    break;
  case ANALYZEMODE:
    enabled = false;
    break;
  }
  static const int levelMenuItems[] = {
    ID_LEVEL_TIMED_GAME
   ,ID_LEVEL_1
   ,ID_LEVEL_2
   ,ID_LEVEL_3
   ,ID_LEVEL_4
   ,ID_LEVEL_5
   ,ID_LEVEL_6
  };
  for(int i = 0; i < ARRAYSIZE(levelMenuItems); i++) {
    enableMenuItem(this, levelMenuItems[i], enabled);
  }
  enableSubMenuContainingId(this, ID_LEVEL_1, enabled);
}

void CChessDlg::enableHintItems() {
  bool hintEnabled, searchForeverEnabled;
  switch(getDialogMode()) {
  case PLAYMODE   :
  case ANALYZEMODE:
  case DEBUGMODE  :
    if(isThinking()) {
      hintEnabled = searchForeverEnabled = false;
    } else {
      hintEnabled = (getPlayerInTurn() == getHumanPlayer()) || (getDialogMode() == DEBUGMODE);
      searchForeverEnabled = true;
    }
    break;
  default:
    hintEnabled = searchForeverEnabled = false;
    break;
  }
  enableMenuItem(this, ID_HELP_HINT            , hintEnabled         );
  enableMenuItem(this, ID_HELP_INFINITEHINT    , searchForeverEnabled);
}

void CChessDlg::enableTestMenuItems() {
  if(!getOptions().hasTestMenu()) {
    return;
  }
  const DialogMode mode = getDialogMode();
  enableMenuItem(this, ID_TEST_DEBUG             , mode == PLAYMODE );

#ifndef TABLEBASE_BUILDER
  enableMenuItem(this, ID_TEST_MOVEBACKWARDS     , false);
#else
  enableMenuItem(this, ID_TEST_MOVEBACKWARDS     , mode == DEBUGMODE);
  setMoveBackwards(m_modeStack.top().m_moveBackwards);
#endif
}

void CChessDlg::updateEscapeMenuItem() {
  switch(getDialogMode()) {
  case PLAYMODE       :
     if(isThinking()) {
       setEscapeMenuText(IDS_ENDSEARCH, true);
     } else {
       resetMenuText(ID_ESCAPE);
       enableMenuItem(this, ID_ESCAPE, false);
     }
     break;
  case DEBUGMODE      :
     if(isThinking()) {
       setEscapeMenuText(IDS_ENDSEARCH, true);
     } else {
       setEscapeMenuText(IDS_ENDDEBUG, true);
     }
     break;
  case EDITMODE       :
    resetMenuText(ID_ESCAPE);
    enableMenuItem(this, ID_ESCAPE, true);
    break;
  case AUTOPLAYMODE   :
    setEscapeMenuText(IDS_ENDAUTOPLAY, true);
    break;
  case ANALYZEMODE    :
  case KNIGHTROUTEMODE:
    setEscapeMenuText(IDS_ENDDEBUG, true);
    break;
  }
}

void CChessDlg::setEscapeMenuText(int textId, bool enabled) {
  saveOrigMenuText(ID_ESCAPE);
  setMenuItemText(this, ID_ESCAPE, loadString(textId));
  enableMenuItem(this, ID_ESCAPE, enabled);
}

// -------------------------------------------------------------------

String CChessDlg::saveOrigMenuText(int id) {
  const String *s = m_origMenuText.get(id);
  if(s) return *s;
  const String tmp = getMenuItemText(this, id);
  m_origMenuText.put(id, tmp);
  return tmp;
}

void CChessDlg::saveAndSetMenuText(int id, const String &s) {
  saveOrigMenuText(id);
  setMenuItemText(this,id, s);
}

void CChessDlg::resetMenuText(int id) {
  setMenuItemText(this,id, saveOrigMenuText(id));
}

// ------------------------- Dialog mode ---------------------------

void CChessDlg::pushDialogMode(const DialogSettings &settings) {
  const bool hasMode       = m_modeStack.getHeight() > 0;
  const DialogMode oldMode = getDialogMode();
  m_modeStack.push(settings);
  setDialogMode(settings);
  if(hasMode) {
    const DialogMode newMode = m_modeStack.top().m_mode;
    notifyPropertyChanged(DIALOGMODE, &oldMode, &newMode);
//    verbose(_T("Property DIALOGMODE changed. old:%d, new:%d\n"), oldMode, newMode);
  }
}

void CChessDlg::popDialogMode() {
  const DialogMode oldMode = m_modeStack.pop().m_mode;
  setDialogMode(m_modeStack.top());
  const DialogMode newMode = m_modeStack.top().m_mode;
  if(newMode != oldMode) {
    notifyPropertyChanged(DIALOGMODE, &oldMode, &newMode);
//    verbose(_T("Property DIALOGMODE changed. old:%d, new:%d\n"), oldMode, newMode);
  }
}

void CChessDlg::setDialogMode(const DialogSettings &settings) {
  switch(settings.m_mode) {
  case PLAYMODE   :
    stopAllBackgroundActivity(true);
    BEGINPAINT();
    m_graphics->unmarkAll();
    m_graphics->setGame(m_game);
    setGameSettings();
    m_graphics->setModeText(EMPTYSTRING);
    setVisibleClocks();
    setWindowCursor(this, MAKEINTRESOURCE(OCR_NORMAL));
    ENDPAINT();

    break;

  case EDITMODE   :
    stopAllBackgroundActivity(true);
    BEGINPAINT();
    m_graphics->setGame(m_editHistory.beginEdit(m_game));
    setGameSettings();
    m_graphics->setModeText(loadString(IDS_EDITMODETEXT));
    setVisibleClocks();
    setWindowCursor(this, MAKEINTRESOURCE(OCR_HAND));
    ENDPAINT();
    break;

  case DEBUGMODE   :
    stopAllBackgroundActivity(true);
    BEGINPAINT();
    m_graphics->setGame(m_game);
    setGameSettings();
    m_graphics->setModeText(loadString(IDS_DEBUGMODETEXT));
    setVisibleClocks();
    setWindowCursor(this, MAKEINTRESOURCE(OCR_NORMAL));
    ENDPAINT();
    break;

  case AUTOPLAYMODE:
    stopAllBackgroundActivity(true);
    BEGINPAINT();
    m_graphics->setGame(m_game);
    setGameSettings();
    m_graphics->setModeText(loadString(IDS_AUTOPLAYMODETEXT));
    m_graphics->unmarkMouse();
    setVisibleClocks();
    setWindowCursor(this, MAKEINTRESOURCE(OCR_NORMAL));
    ENDPAINT();
    startThinking();
    break;

  case ANALYZEMODE:
    stopAllBackgroundActivity(true);
    BEGINPAINT();
    m_graphics->setGame(m_game);
    setGameSettings();
    m_graphics->setModeText(loadString(IDS_ANALYZEMODETEXT));
    setVisibleClocks();
    setWindowCursor(this, MAKEINTRESOURCE(OCR_NORMAL));
    ENDPAINT();
    break;

  case KNIGHTROUTEMODE:
    stopAllBackgroundActivity(true);
    BEGINPAINT();
    m_graphics->setGame(m_game);
    setGameSettings();
    m_graphics->setModeText(_T("Find Knight Route"));
    setWindowCursor(this, MAKEINTRESOURCE(OCR_NORMAL));
    m_selectedPosition = -1;
    setVisibleClocks();
    ENDPAINT();
    break;

  default:
    invalidModeError(__TFUNCTION__);
    return;
  }
}

void CChessDlg::showHintMessageBox(const PrintableMove move) {
  if(move.isMove()) {
    MessageBox(move.toString(getOptions().getMoveFormat()).cstr(), _T("Hint"), MB_OK | MB_ICONEXCLAMATION);
  } else {
    MessageBox(getGameResultToString().cstr(), loadString(IDS_NOHINT).cstr(), MB_OK | MB_ICONEXCLAMATION);
  }
}

void CChessDlg::handlePropertyChanged(const PropertyContainer *source, int id, const void *oldValue, const void *newValue) {
  if(source == m_graphics) {
    switch(id) {
    case FLUSHEDGAMEKEY :
    case FLUSHEDMODETEXT:
      ajourMenuItemsEnableStatus();
      updateTitle();
      break;
    }
  } else if(source == s_traceThread) {
    switch(id) {
    case TRACEWINDOW_ACTIVE:
      PostMessage(ID_MSG_TRACEWINDOW_CHANGED);
      break;
    }
  } else {
    forEachPlayer(p) {
      if(source == &getChessPlayer(p)) {
        switch(id) {
        case CPP_STATE      :
          { const ChessPlayerState newState = *(ChessPlayerState*)newValue;
            PostMessage(ID_MSG_CHESSPLAYERSTATE_CHANGED, p, newState);
          }
          break;
        case CPP_MOVEFINDER:
          PostMessage(ID_MSG_ENGINE_CHANGED, p, 0);
          break;
        case CPP_REMOTE     :
          { const bool oldRemote = *(bool*)oldValue;
            const bool newRemote = *(bool*)newValue;
            PostMessage(ID_MSG_REMOTESTATE_CHANGED, oldRemote, newRemote);
          }
          break;
        case CPP_MESSAGETEXT:
          SendMessage(ID_MSG_CHESSPLAYERMSG_CHANGED, (WPARAM)newValue, 0);
          break;
        }
        break;
      }
    }
  }
}

void CChessDlg::postCommand(int command) {
  PostMessage(WM_COMMAND, MAKELONG(command, 0), 0);
}

LRESULT CChessDlg::OnMsgEngineChanged(WPARAM wp, LPARAM lp) {
  if(getDialogMode() == AUTOPLAYMODE) {
    updateTitle();
  }
  return 0;
}

LRESULT CChessDlg::OnMsgChessPlayerStateChanged(WPARAM wp, LPARAM lp) {
#ifdef _DEBUGDLG
  verbose(_T("%s(%d,%d)\n"), __TFUNCTION__, wp, lp);
#endif
  const Player           player   = (Player)wp;
  const ChessPlayerState newState = (ChessPlayerState)lp;
  ChessPlayer           &cp       = getChessPlayer(player);
  try {
    switch(newState) {
    case CPS_BUSY:
      startComputerTimeTimer();
      break;
    case CPS_MOVEREADY:
      { stopComputerTimeTimer();
        const SearchMoveResult result = cp.getSearchResult();
        const PrintableMove    move(getCurrentGame(),result.m_move);
        switch(getDialogMode()) {
        case PLAYMODE:
          if(result.isHint()) {
            showHintMessageBox(move);
          } else {
            executeMove(move);
          }
          break;
        case DEBUGMODE:
          showHintMessageBox(move);
          break;

        case AUTOPLAYMODE:
          executeMove(move);
          break;

        case ANALYZEMODE:
          enableStartThinking();
          showHintMessageBox(move);
          break;
        case KNIGHTROUTEMODE:
          break;
        default:
          errorMessage(_T("%s:Unexpected dialogstate:%d"), __TFUNCTION__,getDialogMode());
          break;
        }
      }
      break;
    case CPS_IDLE:
      stopComputerTimeTimer(); // interrupted by user
      break;
    }
  } catch(Exception e) {
    errorMessage(e);
  }
  if(newState == CPS_BUSY) {
    setWindowCursor(this, MAKEINTRESOURCE(OCR_WAIT));
  } else {
    setWindowCursor(this, MAKEINTRESOURCE(OCR_NORMAL));
  }
  ajourMenuItemsEnableStatus();
  return 0;
}

LRESULT CChessDlg::OnMsgChessPlayerMsgChanged(WPARAM wp, LPARAM lp) {
  const String msg = *(String*)wp;
  if(msg.length() == 0) return 0;
  stopComputerTimeTimer();
  errorMessage(_T("%s"), msg.cstr());
  if(getDialogMode() == AUTOPLAYMODE) {
    popDialogMode();
  }
  return 0;
}

LRESULT CChessDlg::OnMsgRemoteStateChanged(WPARAM wp, LPARAM lp) {
#ifdef _DEBUGDLG
  verbose(_T("%s(%d,%d)\n"), __TFUNCTION__, wp, lp);
#endif
  bool newRemote = lp ? true : false;
  if(newRemote) {
    startThinking();
  } else {
    invalidate();
  }
  return 0;
}

LRESULT CChessDlg::OnMsgTraceWindowChanged(WPARAM wp, LPARAM lp) {
#ifdef _DEBUGDLG
  verbose(_T("%s(%d,%d)\n"), __TFUNCTION__, wp, lp);
#endif
  const bool active = isTraceWindowVisible();
  checkMenuItem(this, ID_VIEW_TRACEWINDOW, active);
  getOptions().setTraceWindowVisible(active);
  BringWindowToTop();
  return 0;
}

BOOL CChessDlg::PreTranslateMessage(MSG* pMsg) {
  if(TranslateAccelerator(m_hWnd, m_accelTable, pMsg)) {
    return true;
  }
/*
  switch(pMsg->message) {
  case WM_COMMAND:
    { long lp = pMsg->lParam;
      UINT wp = pMsg->wParam;
    }
    break;
  }
*/
  return CDialog::PreTranslateMessage(pMsg);
}

Game &CChessDlg::getCurrentGame1() {
  switch(getDialogMode()) {
  case PLAYMODE       : return m_game;
  case EDITMODE       : return m_editHistory.saveState();
  case DEBUGMODE      : return m_game;
  case AUTOPLAYMODE   : return m_game;
  case ANALYZEMODE    : return m_game;
  case KNIGHTROUTEMODE: return m_game;
  default             : invalidModeError(__TFUNCTION__);
                        return m_game;
  }
}

int CChessDlg::getBoardPosition(const CPoint &p, bool screenRelative) const {
  if(!screenRelative) {
    return m_graphics->getBoardPosition(p);
  } else {
    CPoint p1 = p;
    ScreenToClient(&p1);
    return m_graphics->getBoardPosition(p1);
  }
}

void CChessDlg::setGameName(const String &name) {
  getCurrentGame().setName(name);
  updateTitle();
}

void CChessDlg::updateTitle() {
  if(!isControlFlagSet(CTRL_AUTOUPDATETITLE | CTRL_INITDONE)) {
    return;
  }
  const DialogMode mode = getDialogMode();
  String title;

  switch(mode) {
  case PLAYMODE   :
  case EDITMODE   :
  case DEBUGMODE  :
    title = format(_T("%s - %s"), loadString(IDS_GAMENAME).cstr(), getCurrentGame().getDisplayName().cstr());
    break;
  case AUTOPLAYMODE:
    title = format(_T("%s - %s: %s  -  %s")
                  ,loadString(IDS_GAMENAME).cstr()
                  ,getCurrentGame().getDisplayName().cstr()
                  ,getChessPlayer(WHITEPLAYER).getName().cstr()
                  ,getChessPlayer(BLACKPLAYER).getName().cstr()
                  );
    break;
  case ANALYZEMODE:
    title = format(_T("%s - %s %s %d"), loadString(IDS_ANALYZEMODETEXT).cstr(), getCurrentGame().getDisplayName().cstr(), loadString(IDS_AFTERMOVE).cstr(), PLIESTOMOVES(m_startPlyIndex));
    break;
  case KNIGHTROUTEMODE:
    break;
  default         :
    invalidModeError(__TFUNCTION__);
  }

/*
  if(isMenuItemChecked(this,ID_VIEW_TRACEOPENING)) {
    const StringArray activeOpenings = ChessPlayer::getOpeningLibrary().getActiveOpenings(getCurrentGame());
    if(activeOpenings.size() > 0) {
      title += format(_T(" %s:%s"), loadString(IDS_OPENINGS).cstr(), activeOpenings.toString().cstr());
    }
  }
*/
  setWindowText(this, title);
}

void CChessDlg::startNewGame()              {  newGame(getOptions().getComputerPlayer()); }
void CChessDlg::OnFileNewGameYouPlayWhite() {  newGame(BLACKPLAYER);                      }
void CChessDlg::OnFileNewGameYouPlayBlack() {  newGame(WHITEPLAYER);                      }

void CChessDlg::newGame(Player computerPlayer) {
  stopAllBackgroundActivity(true);
  BEGINPAINT();
  getCurrentGame().newGame();
  setGameSettings();
  notifyGameChanged(getCurrentGame());
  setComputerPlayer(computerPlayer);
  m_graphics->unmarkAll();
  m_graphics->paintAll();
  OnSettingsResetClock();
  ENDPAINT();
}

String CChessDlg::getFileExtensions() const {
  return format(_T("%s%c*.chs%c%s%c*.txt%c%s%c*.*%c%c")
               ,loadString(IDS_CHESSFILEEXTENSION).cstr(),0, 0
               ,loadString(IDS_TEXTFILEEXTENSION).cstr() ,0, 0
               ,loadString(IDS_ALLFILESEXTENSION).cstr() ,0, 0
               ,0);
}

void CChessDlg::OnFileLoadGame() {
  const String title      = loadString(IDS_LOADGAMETITLE);
  const String extensions = getFileExtensions();
  String       initialDir = getOptions().getGameInitialDir();

  CFileDialog dlg(TRUE);
  dlg.m_ofn.lpstrTitle      = title.cstr();
  dlg.m_ofn.lpstrFilter     = extensions.cstr();
  if(initialDir.length() > 0) {
    dlg.m_ofn.lpstrInitialDir = initialDir.cstr();
  }
  if(dlg.DoModal() != IDOK) {
    return;
  }
  const bool   thinkWasEnabled = isThinkEnabled();
  const String fileName        = dlg.m_ofn.lpstrFile;
  try {
    clrControlFlag(CTRL_THINKENABLED);
    load(fileName);
    saveInitialDir(fileName);
    if(thinkWasEnabled) setControlFlag(CTRL_THINKENABLED);
  } catch(Exception e) {
    if(thinkWasEnabled) setControlFlag(CTRL_THINKENABLED);
    errorMessage(e);
  }
}

void CChessDlg::OnFileSave() {
  if(getCurrentGame().hasDefaultName()) {
    OnFileSaveAs();
  } else {
    try {
      Game &game = validateBeforeSave();
      save(game, game.getFileName());
    } catch(Exception e) {
      errorMessage(e);
    }
  }
}

void CChessDlg::OnFileSaveAs() {
  try {
    const String extensions = getFileExtensions();
    Game        &game       = validateBeforeSave();
    String       fileName   = game.getFileName();
    const String initialDir = getOptions().getGameInitialDir();

    CFileDialog  dlg(FALSE, _T("chs"), fileName.cstr());
    dlg.m_ofn.lpstrFilter     = extensions.cstr();
    if(initialDir.length() > 0) {
      dlg.m_ofn.lpstrInitialDir = initialDir.cstr();
    }
    if(dlg.DoModal() == IDOK) {
      fileName = dlg.m_ofn.lpstrFile;
    } else {
      return;
    }
    save(game, fileName);
    saveInitialDir(fileName);
  } catch(Exception e) {
    errorMessage(e);
  }
}

void CChessDlg::saveInitialDir(const String &fileName) {
  FileNameSplitter nameInfo(fileName);
  getOptions().setGameInitialDir(nameInfo.getDrive() + nameInfo.getDir());
}

Game &CChessDlg::validateBeforeSave() {
  return getCurrentGame().validateBoard(getOptions().getValidateAfterEdit());
}

void CChessDlg::save(Game &game, const String &name) {
  FileNameSplitter info(name);
  if(info.getExtension().length() == 0) {
    info.setExtension(_T("chs"));
  }
  const String fileName = info.getFullPath();
  FILE *f = MKFOPEN(fileName,_T("w"));
  try {
    game.save(f);
    getOptions().save(f);
    fclose(f);
    game.setName(fileName);
    invalidate();
  } catch(...) {
    fclose(f);
    unlink(fileName);
    throw;
  }
}

static FILE *openChessFile(const String &name) {
  FILE *f = fopen(name,_T("r"));
  if(f != NULL) {
    return f;
  } else {
    FileNameSplitter info(name);
    if(info.getExtension().length() == 0) {
      info.setExtension(_T("chs"));
    }
    const String newName = info.getFullPath();
    return FOPEN(newName, _T("r"));
  }
}

void CChessDlg::load(const String &fileName) {
  FILE *f = openChessFile(fileName);
  Game tmp;
  Options options = getOptions();
  try {
    tmp.load(f);
    options.load(f);
    fclose(f);
  } catch(...) {
    fclose(f);
    throw;
  }
  Game &cg = getCurrentGame();
  const bool isEditMode = cg.isSetupMode();
  stopAllBackgroundActivity(true);
  FileNameSplitter nameInfo(fileName);
  BEGINPAINT();
  setComputerPlayer(options.getComputerPlayer());
  cg = tmp;
  setGameSettings();
  if(isEditMode) {
    cg.beginSetup();
  }
  setGameName(fileName);
  notifyGameChanged(cg);
  paintGamePosition();
  ENDPAINT();
}

void CChessDlg::OnFilePlayRemote() {
  CConnectDlg dlg(getCurrentGame());
  if(dlg.DoModal() == IDOK) {
    stopAllBackgroundActivity(true);
    const SocketChannel &ch = dlg.getSocketChannel();
    if(getOptions().isConnectedToServer()) {
      BEGINPAINT();
      setComputerPlayer(dlg.getRemotePlayer());
      paintGamePosition();
      ENDPAINT();
      notifyGameChanged(getCurrentGame());
    }
    getChessPlayer(getComputerPlayer()).connect(ch);
  }
}

void CChessDlg::OnFileDisconnectRemoteGame() {
  if(!isRemoteGame()) {
    return;
  }
  stopAllBackgroundActivity(true);
  getChessPlayer(getComputerPlayer()).disconnect();
  invalidate();
}

void CChessDlg::OnFileShowHistory() {
  try {
    Game &game = getCurrentGame();
    m_savedGame = game;
    CHistoryDlg dlg(game, this);
    const bool update = (dlg.DoModal() == IDOK) && dlg.isChanged();
    game = m_savedGame;
    if(update) {
      for(int ply = 0; ply < game.getPlyCount(); ply++) {
        game.getMove(ply).setAnnotation(dlg.getAnnotation(ply));
      }
    }
    invalidate();
  } catch(Exception e) {
    errorMessage(e);
  }
}

void CChessDlg::OnFileOfferDraw() {
}

void CChessDlg::OnFileResign() {
  if(MessageBox(loadString(IDS_MSG_CONFIRM_RESIGN).cstr(), loadString(IDS_RESIGN).cstr(), MB_YESNO | MB_ICONQUESTION) == IDYES) {
    stopAllBackgroundActivity(true);
    return;
  }
}

void CChessDlg::OnFileExit() {
  stopAllBackgroundActivity(true);
  detachAllPropertyContainers();
  EndDialog(IDOK);
}

LRESULT CChessDlg::OnMsgShowSelectedHistoryMove(WPARAM wp, LPARAM lp) {
  try {
    getCurrentGame().setGameAfterPly(m_savedGame, (int)lp);
    m_graphics->paintGamePositions();
  } catch(Exception e) {
    errorMessage(e);
  }
  return 0;
}

void CChessDlg::activateOptions() {
  const Options &options = getOptions();
  setView();
  setComputerPlayer(              options.getComputerPlayer()                 );
  setValidateAfterEdit(           options.getValidateAfterEdit()              );
  setLevel(                       options.getNormalPlayLevel()                );
  setWhiteAutoPlayLevel(          options.getAutoPlayLevel(WHITEPLAYER)       );
  setBlackAutoPlayLevel(          options.getAutoPlayLevel(BLACKPLAYER)       );
  enableTestMenu(                 options.hasTestMenu()                       );
  setGameSettings();
  setClientRectSize(this,         options.getBoardSize()                      );
  setTraceWindowVisible(          options.getTraceWindowVisible()             );
}

void CChessDlg::setComputerPlayer(Player computerPlayer) {
  stopAllBackgroundActivity(true);
  m_graphics->setComputerPlayer(computerPlayer);
  getOptions().setComputerPlayer(computerPlayer);
  enableStartThinking();
}

void CChessDlg::setGameResult(GameResult gameResult) {
  setProperty(GAMERESULT, m_gameResult, gameResult);
//    verbose(_T("Property GAMERESULT changed. old:%s, new:%s\n"), ::getGameResultToString(oldResult).cstr(), ::getGameResultToString(gameResult).cstr());
}

void CChessDlg::notifyMove(const PrintableMove &move) {
  notifyMove(WHITEPLAYER, move);
  notifyMove(BLACKPLAYER, move);
}

void CChessDlg::notifyMove(Player player, const PrintableMove &move) {
  getChessPlayer(player).notifyMove(move);
}

void CChessDlg::notifyGameChanged(const Game &game) {
  notifyGameChanged(WHITEPLAYER, game);
  notifyGameChanged(BLACKPLAYER, game);
  setGameResult(game.findGameResult());
}

void CChessDlg::notifyGameChanged(Player player, const Game &game) {
  getChessPlayer(player).notifyGameChanged(game);
}

// ------------------------------ Mousehandlers -----------------------------------------------

void CChessDlg::OnLButtonDown(UINT nFlags, CPoint point) {
  if(!isAppActive()) return;
  try {
    switch(getDialogMode()) {
    case PLAYMODE       : OnLButtonDownPlayMode( nFlags, point); break;
    case EDITMODE       : OnLButtonDownEditMode( nFlags, point); break;
    case DEBUGMODE      :
    case ANALYZEMODE    : OnLButtonDownDebugMode(nFlags, point); break;
    case KNIGHTROUTEMODE: OnLButtonDownKRMode(   nFlags, point); break;
    }
  } catch(Exception e) {
    errorMessage(e);
  }
  CDialog::OnLButtonDown(nFlags, point);
}

void CChessDlg::OnLButtonUp(UINT nFlags, CPoint point) {
  if(!isAppActive()) return;
  try {
    switch(getDialogMode()) {
    case PLAYMODE       : OnLButtonUpPlayMode(   nFlags, point); break;
    case EDITMODE       : OnLButtonUpEditMode(   nFlags, point); break;
    case DEBUGMODE      :
    case ANALYZEMODE    : OnLButtonUpDebugMode(  nFlags, point); break;
    case KNIGHTROUTEMODE: OnLButtonUpKRMode(     nFlags, point); break;
    }
  } catch(Exception e) {
    errorMessage(e);
  }
  CDialog::OnLButtonUp(nFlags, point);
}

void CChessDlg::OnMouseMove(UINT nFlags, CPoint point) {
  if(!isAppActive()) return;
  try {
    switch(getDialogMode()) {
    case PLAYMODE       : OnMouseMovePlayMode(   nFlags, point); break;
    case EDITMODE       : OnMouseMoveEditMode(   nFlags, point); break;
    case DEBUGMODE      :
    case ANALYZEMODE    : OnMouseMoveDebugMode(  nFlags, point); break;
    case KNIGHTROUTEMODE: OnMouseMoveKRMode(     nFlags, point); break;
    }
  } catch(Exception e) {
    m_graphics->reopen();
    invalidate();
  }
  CDialog::OnMouseMove(nFlags, point);
}

// ------------------------------ Mousehandlers Play Mode -----------------------------------------------

void CChessDlg::OnLButtonDownPlayMode(UINT nFlags, CPoint point) {
  Game &game = getCurrentGame();
  if(game.getPlayerInTurn() != getHumanPlayer()) {
    return;
  }

  m_selectedPosition = getBoardPosition(point);
  if(!isValidPosition(m_selectedPosition)) {
    return;
  }

  const Piece *selectedPiece = m_graphics->getSelectedPiece();
  if(selectedPiece == NULL) {
    m_graphics->markSelectedPiece(m_selectedPosition);
  } else if(game.getLegalDestinationFields(selectedPiece).contains(m_selectedPosition)) {
    const int from = selectedPiece->getPosition();
    if(game.mustSelectPromotion(from, m_selectedPosition)) {
      selectAndExecutePromotion(point);
    } else {
      executeMove(game.generateMove(from, m_selectedPosition, NoPiece));
    }
  } else if(m_selectedPosition == selectedPiece->getPosition()) {
    m_graphics->unmarkAll();
    m_graphics->markMouse(m_selectedPosition);
  } else {
    m_graphics->markSelectedPiece(m_selectedPosition);
  }
}

typedef struct {
  PieceType m_type;
  int       m_commandId;
} PromoteMenuItem;

void CChessDlg::selectAndExecutePromotion(const CPoint &point, MoveAnnotation annotation) {
  static const PromoteMenuItem itemArray[4] = {
    Queen , ID_PROMOTE_TO_QUEEN
   ,Rook  , ID_PROMOTE_TO_ROOK
   ,Bishop, ID_PROMOTE_TO_BISHOP
   ,Knight, ID_PROMOTE_TO_KNIGHT
  };

  m_selectedAnnotation = annotation;

  CMenu subMenu;
  subMenu.CreateMenu();
  CBitmap bitmaps[4];
  for(int i = 0; i < 4; i++) {
    const PromoteMenuItem &item = itemArray[i];
    subMenu.AppendMenu(0, item.m_commandId, &m_graphics->getResources().getSmallPieceBitmap(bitmaps[i], MAKE_PIECEKEY(getPlayerInTurn(), item.m_type)));
  }
  CMenu menu;
  menu.CreateMenu();
  menu.AppendMenu(MF_POPUP, (UINT_PTR)((HMENU)subMenu), _T("Promote"));
  CPoint scrPoint = point;
  ClientToScreen(&scrPoint);
  menu.GetSubMenu(0)->TrackPopupMenu(TPM_LEFTALIGN|TPM_RIGHTBUTTON, scrPoint.x,scrPoint.y, this);
}

void CChessDlg::OnPromoteToQueen()  { executePromotion(Queen);  }
void CChessDlg::OnPromoteToRook()   { executePromotion(Rook);   }
void CChessDlg::OnPromoteToBishop() { executePromotion(Bishop); }
void CChessDlg::OnPromoteToKnight() { executePromotion(Knight); }

void CChessDlg::executePromotion(PieceType promoteTo) {
  Game &game = getCurrentGame();
  const Piece *selectedPiece = m_graphics->getSelectedPiece();
  const PrintableMove move   = game.generateMove(selectedPiece->getPosition(), m_selectedPosition, promoteTo, m_selectedAnnotation);
  executeMove(move);
}

void CChessDlg::OnLButtonUpPlayMode(UINT nFlags, CPoint point) {
  // do nothing
}

void CChessDlg::OnMouseMovePlayMode(UINT nFlags, CPoint point) {
  if(getPlayerInTurn() != getHumanPlayer()) {
    return;
  }
  m_graphics->markMouse(getBoardPosition(point));
}

// --------------------------------------------------------------------------------------

void CChessDlg::OnTimer(UINT_PTR nIDEvent) {
  CDialog::OnTimer(nIDEvent);
  switch(nIDEvent) {
  case SHOWWATCH_TIMER:
    updateClock();
    break;
  default:
    break;
  }
}

String CChessDlg::getGameResultToString() {
  const Game &game = getCurrentGame();
  const GameResult result = game.findGameResult();
  switch(result) {
  case NORESULT                          :
    if(game.getPositionType() == DRAW_POSITION) {
      return format(_T("%s\n%s"), loadString(IDS_MSG_INSUFFICIENT_MATERIAL).cstr(), loadString(IDS_DRAW).cstr());
    } else {
      return EMPTYSTRING;
    }
  case WHITE_CHECKMATE           : return loadString(IDS_MSG_BLACK_WIN);
  case BLACK_CHECKMATE           : return loadString(IDS_MSG_WHITE_WIN);
  case STALEMATE                 : return loadString(IDS_STALEMATE);
#ifndef TABLEBASE_BUILDER
  case POSITION_REPEATED_3_TIMES : return loadString(IDS_MSG_POSITION_REPEATED_3_TIMES);
  case NO_CAPTURE_OR_PAWNMOVE    : return getFiftyMovesString();
  case DRAW                      : return format(_T("%s\n%s"), loadString(IDS_MSG_INSUFFICIENT_MATERIAL).cstr(), loadString(IDS_DRAW).cstr());
#endif
  default: return format(_T("Unknown gameResult:%d"), result);
  }
}

String CChessDlg::getFiftyMovesString() const {
  return format(loadString(IDS_MSG_d_MOVES_RULE).cstr(), getOptions().getMaxMovesWithoutCaptureOrPawnMove());
}

void CChessDlg::executeMove(const PrintableMove &m) {
  Game &game = getCurrentGame();
  stopAllBackgroundActivity(false);
  m_graphics->unmarkAll();
  const Player playerWhoMoved = game.getPlayerInTurn();

  if(m.isMove()) {
    notifyMove(m);
    if(getOptions().getAnimateMoves()) {
      const bool clockRunning = m_watch.isRunning();
      if(clockRunning) pauseClock();
      m_graphics->animateMove(m);
      if(clockRunning) resumeClock();
    }

    if(startClockNeeded()) {
      OnSettingsToggleClock();
    }

    game.executeMove(m);
    paintGamePosition();
    setGameResult(game.findGameResult());
    if(getGameResult() != NORESULT) {
      stopClock();
    }
  }
  switch(getDialogMode()) {
  case PLAYMODE:
    { switch(getGameResult()) {
      case NORESULT                           :
        if(game.getPositionType() == DRAW_POSITION) {
          setGameResult(DRAW);
          stopClock();
          handleEndGameForPlayMode(loadString(IDS_MSG_INSUFFICIENT_MATERIAL), loadString(IDS_DRAW));
        } else {
          startThinking();
        }
        break;

      case WHITE_CHECKMATE                    :
      case BLACK_CHECKMATE                    :
        if(getOptions().getAnimateCheckmate()) {
          m_graphics->animateCheckMate();
        }
        handleEndGameForPlayMode(loadString(GAMERESULTTOWINNER(getGameResult()) == getHumanPlayer() ? IDS_MSG_HUMAN_WIN : IDS_MSG_COMPUTER_WIN)
                                ,loadString(IDS_CHECKMATE)
                                ,(getGameResult()==WHITE_CHECKMATE) ? IDS_MSG_BLACK_WIN: IDS_MSG_WHITE_WIN);
        break;

      case STALEMATE                          :
        handleEndGameForPlayMode(format(loadString(IDS_MSG_s_STALEMATE).cstr(), getPlayerName(game.getPlayerInTurn()))
                                ,loadString(IDS_STALEMATE));
        break;

  #ifndef TABLEBASE_BUILDER
      case POSITION_REPEATED_3_TIMES          :
        handleEndGameForPlayMode(loadString(IDS_MSG_POSITION_REPEATED_3_TIMES), loadString(IDS_DRAW));
        break;

      case NO_CAPTURE_OR_PAWNMOVE:
        handleEndGameForPlayMode(getFiftyMovesString(), loadString(IDS_DRAW));
        break;
  #endif
      }
    }
    break;

  case EDITMODE :
    break;

  case DEBUGMODE:
    break;

  case AUTOPLAYMODE:
    switch(getGameResult()) {
    case NORESULT                          :
      if(game.getPositionType() == DRAW_POSITION) {
        setGameResult(DRAW);
        stopClock();
        handleEndGameForAutoPlayMode(loadString(IDS_MSG_INSUFFICIENT_MATERIAL), loadString(IDS_DRAW));
      } else {
        postCommand(ID_SETTINGS_START_THINKING); //startThinking();
      }
      break;

    case WHITE_CHECKMATE                   :
    case BLACK_CHECKMATE                   :
      if(getOptions().getAnimateCheckmate()) {
        m_graphics->animateCheckMate();
      }
      handleEndGameForAutoPlayMode(loadString((game.getGameResult()==WHITE_CHECKMATE)? IDS_MSG_BLACK_WIN : IDS_MSG_WHITE_WIN)
                                  ,loadString(IDS_CHECKMATE));
      break;

    case STALEMATE                         :
      handleEndGameForAutoPlayMode(format(loadString(IDS_MSG_s_STALEMATE).cstr(), getPlayerName(game.getPlayerInTurn())), loadString(IDS_STALEMATE));
      break;

#ifndef TABLEBASE_BUILDER
    case POSITION_REPEATED_3_TIMES          :
      handleEndGameForAutoPlayMode(loadString(IDS_MSG_POSITION_REPEATED_3_TIMES), loadString(IDS_DRAW));
      break;

    case NO_CAPTURE_OR_PAWNMOVE:
      handleEndGameForAutoPlayMode(getFiftyMovesString(), loadString(IDS_DRAW));
      break;
#endif
    }
    break;
  case ANALYZEMODE:
    break;
  default:
    invalidModeError(__TFUNCTION__);
    break;
  }
}

#ifdef TABLEBASE_BUILDER
void CChessDlg::executeBackMove(const Move &m) { // always debugMode
  Game &game = getCurrentGame();
  m_graphics->unmarkAll();
  if(m.isMove()) {
    if(getOptions().getAnimateMoves()) {
      m_graphics->animateMove(m);
    }
    game.doBackMove(m);
    paintGamePosition();
  }
}

void CChessDlg::unExecuteLastBackMove() {
  getCurrentGame().undoBackMove();
  paintGamePosition();
}

#endif

void CChessDlg::handleEndGameForPlayMode(const String &message, const String &caption, int verboseMessageId) {
  if(isControlFlagSet(CTRL_VERBOSEATGAMEEND)) {
    if(!getOptions().getAskForNewGame()) {
      verbose(_T("%s\n"), verboseMessageId ? loadString(verboseMessageId).cstr() : message.cstr());
      return;
    }
    Sleep(1300);
    String askPlayAgain = loadString(IDS_MSG_ASK_PLAYAGAIN);

    bool showAgain;
    if(confirmDialogBox(format(_T("%s\r\n\r\n%s"), message.cstr(), askPlayAgain.cstr()).cstr(), caption.cstr(), showAgain, MB_YESNO | MB_ICONQUESTION) == IDYES) {
      startNewGame();
    }
    if(!showAgain) {
      getOptions().setAskForNewGame(false);
    }
  }
}

void CChessDlg::handleEndGameForAutoPlayMode(const String &message, const String &caption) {
  if(isControlFlagSet(CTRL_VERBOSEATGAMEEND)) {
    if(caption.length() == 0) {
      verbose(_T("%s\n"), message.cstr());
      Sleep(4000);
    } else {
      showMessage(this, 4000, caption, _T("%s"), message.cstr());
    }
  }

  if(isMenuItemChecked(this, ID_AUTOPLAY_RESTART_AT_END)) {
    startNewGame();
    postCommand(ID_SETTINGS_START_THINKING);
  } else {
    OnEscape();
  }
}

void CChessDlg::stopAllBackgroundActivity(bool stopAll) {
  stopThinking();
  if(stopAll) {
    stopClock();
  }
}

TimeLimit CChessDlg::getTimeLimit() const {
  const Game           &game    = getCurrentGame();
  const Player          player  = game.getPlayerInTurn();
  const Options        &options = getOptions();
  int                   level;

  if(getDialogMode() == PLAYMODE) {
    level = options.getNormalPlayLevel();
  } else {
    level = options.getAutoPlayLevel(player);
  }
  switch(level) {
  case LEVEL_TIMEDGAME:
  case LEVEL_SPEEDCHESS:
    { const TimeParameters &tm = options.getTimeParameters(player);
      const int movesLeft      = tm.getMovesLeft() - game.getPlyCount()/2;
      const int m              = movesLeft <= 0 ? 40 : movesLeft;
      return TimeLimit((int)(1000 * (0.9 * m_watch.getSecondsRemaining(player)/m+(m-1)*tm.getSecondsIncr())), true);
    }
//  case LEVEL_SPEEDCHESS:
//    {  const TimeParameters &tm = options.getSpeedTimeParameters(player);
//       m_watch.getRemainingTime(player)
//    }
  default:
    if(1 <= level && level <= LEVELCOUNT) {
      return TimeLimit((int)(1000 * options.getLevelTimeout().getTimeout(level)), false);
    } else {
      errorMessage(_T("Invalid level:%d. Must be [0..%d]"), level, LEVELCOUNT);
      return 100;
    }
  }
}

void CChessDlg::startThinking() {
  if(!isControlFlagSet(CTRL_INITDONE | CTRL_THINKENABLED)) {
    return;
  }
  Game             &game   =  getCurrentGame();
  const Player      player =  game.getPlayerInTurn();
  ChessPlayer      &cp     =  getChessPlayer(player);
  switch(getDialogMode()) {
  case PLAYMODE:
    if(!isThinking() && (player == getComputerPlayer())) {
      cp.startSearch(game, getTimeLimit(), false);
    }
    break;

  case EDITMODE :
  case DEBUGMODE:
    break;

  case AUTOPLAYMODE:
    if(!isThinking()) {
      cp.startSearch(game, getTimeLimit(), false);
    }
    break;
  }
}

void CChessDlg::stopThinking() {
  if(!isInitDone()) {
    return;
  }
  forEachPlayer(p) {
    getChessPlayer(p).stopSearch();
  }
}

void CChessDlg::startFindHint(int timeout) { // timeout in milliseconds
  if(!isControlFlagSet(CTRL_INITDONE | CTRL_THINKENABLED)) {
    return;
  }
  ChessPlayer &cp = getChessPlayer(getPlayerInTurn());

  switch(getDialogMode()) {
  case PLAYMODE    :
  case DEBUGMODE   :
  case ANALYZEMODE :
    if(!isThinking()) {
      cp.startSearch(getCurrentGame(), timeout, true);
    }
    break;

  case EDITMODE    :
  case AUTOPLAYMODE:
    break;
  }
}

void CChessDlg::startTimer(UINT timerId, UINT milliSeconds) {
  if(isTimerRunning(timerId)) {
    return;
  }
  if(SetTimer(timerId, milliSeconds, NULL)) {
    m_timersRunning.add(timerId);
    ajourMenuItemsEnableStatus();
  } else {
    errorMessage(_T("Cannot start timer %d"), timerId);
  }
}

void CChessDlg::stopTimer(UINT timerId) {
  if(isTimerRunning(timerId)) {
    KillTimer(timerId);
    m_timersRunning.remove(timerId);
    ajourMenuItemsEnableStatus();
  }
}

bool CChessDlg::isTimerRunning(UINT timerId) const {
  if((timerId == 0) || (timerId > MAXTIMERID)) {
    errorMessage(_T("%s:Invalid argument. timerId=%d. Valid interval=[%d..%d]")
                ,__TFUNCTION__, timerId, 1, MAXTIMERID);
    return false;
  }
  return m_timersRunning.contains(timerId);
}

// --------------------------------------------- Edit functions -------------------------------------------

void CChessDlg::OnEditStartSetup() {
  try {
    pushDialogMode(EDITMODE);
  } catch(Exception e) {
    errorMessage(e);
  }
}

void CChessDlg::OnEditTurnBoard() {
  BEGINPAINT();
  setComputerPlayer(GETENEMY(getComputerPlayer()));
  setVisibleClocks();
  ENDPAINT();
}

void CChessDlg::OnEditEndSetup() {
  BEGINPAINT();
  try {
    m_graphics->unmarkAll();
    if(m_editHistory.isModified()) {
      m_game = m_editHistory.getGame().validateBoard(getOptions().getValidateAfterEdit());
      m_game.endSetup();
      notifyGameChanged(m_game);
    }
    m_editHistory.endEdit();
    popDialogMode();
  } catch(Exception e) {
    errorMessage(e);
  }
  ENDPAINT();
}

void CChessDlg::OnEditRemove() {
  try {
    Game &game = m_editHistory.saveState();
    game.removePieceAtPosition(m_selectedPosition);
    game.initState();
    paintGamePosition();
  } catch(Exception e) {
    errorMessage(e);
  }
}

void CChessDlg::OnEditClearBoard() {
  try {
    m_editHistory.saveState().clearBoard();
    paintGamePosition();
  } catch(Exception e) {
    errorMessage(e);
  }
}

void CChessDlg::OnEditSetupStartPosition() {
  try {
    m_editHistory.saveState().newGame(m_editHistory.getGame().getFileName());
    paintGamePosition();
  } catch(Exception e) {
    errorMessage(e);
  }
}

void CChessDlg::OnEditSwitchPlayerInTurn() {
  setPlayerInTurn(GETENEMY(m_editHistory.getGame().getPlayerInTurn()));
}

void CChessDlg::setPlayerInTurn(Player player) {
  try {
    m_editHistory.saveState().setPlayerInTurn(player);
    paintGamePosition();
  } catch(Exception e) {
    errorMessage(e);
  }
}

void CChessDlg::OnEditSwapColors() {
  applySymmetricTransformation(TRANSFORM_SWAPPLAYERS, false);
  setComputerPlayer(GETENEMY(getComputerPlayer()));
  invalidate();
}

void CChessDlg::OnEditMirrorColumn() { applySymmetricTransformation(TRANSFORM_MIRRORCOL  ); }
void CChessDlg::OnEditMirrorRow()    { applySymmetricTransformation(TRANSFORM_MIRRORROW  ); }
void CChessDlg::OnEditMirrorDiag1()  { applySymmetricTransformation(TRANSFORM_MIRRORDIAG1); }
void CChessDlg::OnEditMirrorDiag2()  { applySymmetricTransformation(TRANSFORM_MIRRORDIAG2); }
void CChessDlg::OnEditRotateRight()  { applySymmetricTransformation(TRANSFORM_ROTATERIGHT); }
void CChessDlg::OnEditRotateLeft()   { applySymmetricTransformation(TRANSFORM_ROTATELEFT ); }
void CChessDlg::OnEditRotate180()    { applySymmetricTransformation(TRANSFORM_ROTATE180  ); }

void CChessDlg::applySymmetricTransformation(SymmetricTransformation st, bool paint) {
  try {
    GameKey key = m_editHistory.saveState().getKey();
    Game    g   = key.transform(st);
    g.beginSetup();
    m_editHistory.getGame() = g;
    if(paint) {
      paintGamePosition();
    }
  } catch(Exception e) {
    errorMessage(e);
  }
}

// ------------------------------------- Edit mousehandlers -----------------------------------------

void CChessDlg::OnLButtonDownEditMode(UINT nFlags, CPoint point) {
  const int pos = getBoardPosition(point);
  if(isValidPosition(pos)) {
    m_selectedPosition = pos;
    m_selectedPiece    = m_editHistory.saveState().getPieceKeyAtPosition(pos);
    m_removedPiece     = EMPTYPIECEKEY;
  } else {
    const OffboardPiece *obp = m_graphics->getOffboardPieceByPosition(point);
    if(obp != NULL) {
      m_graphics->beginDragPiece(point, obp);
      m_editHistory.saveState();
    }
  }
  enableUndoRedo();
}

void CChessDlg::OnLButtonUpEditMode(UINT nFlags, CPoint point) {
  Game &game = getCurrentGame();
  if(m_removedPiece != EMPTYPIECEKEY) {
    const Player rp = GET_PLAYER_FROMKEY(m_removedPiece);
    game.resetCapturedPieceTypes(rp);
    m_graphics->repaintOffboardPieces(rp);
  } else if(m_graphics->isDragging()) {
    game.resetCapturedPieceTypes(GET_PLAYER_FROMKEY(m_graphics->getDraggedPiece()));
    m_graphics->endDragPiece();
  }
  game.initState();
  m_selectedPiece = m_removedPiece = EMPTYPIECEKEY;
  enableUndoRedo();
}

void CChessDlg::OnMouseMoveEditMode(UINT nFlags, CPoint point) {
  const int newPos = getBoardPosition(point);
  if((nFlags & MK_LBUTTON) == 0) {
    m_selectedPiece = m_removedPiece = EMPTYPIECEKEY;
    if(newPos != m_graphics->getMouseField()) {
      m_graphics->markMouse(newPos);
    }
  } else {
    Game &game = getCurrentGame();
    if(m_graphics->isDragging()) {
      if(!isValidPosition(newPos)) {   // drag piece outside board
        m_graphics->dragPiece(point);
      } else {                         // drag piece from outside board to a valid board position
        try {
          const PieceKey dpKey = m_graphics->getDraggedPiece();
          m_removedPiece       = game.setPieceAtPosition(dpKey, newPos); // can throw
          game.initState();

          m_selectedPiece      = dpKey;
          m_selectedPosition   = newPos;

          BEGINPAINT();
          m_graphics->endDragPiece();
          m_graphics->markMouse(newPos);
          ENDPAINT();
        } catch(Exception e) {
//          m_graphics->dragPiece(point);
        }
      }
    } else { // !dragging
      if((m_selectedPiece != EMPTYPIECEKEY) && isValidPosition(m_selectedPosition) && (newPos != m_selectedPosition)) {
        if(!isValidPosition(newPos)) { // drag piece from a valid board position to a position outside the board
          const PieceKey key = game.removePieceAtPosition(m_selectedPosition, false);
          if(key != EMPTYPIECEKEY) {
            game.setPieceAtPosition(m_removedPiece, m_selectedPosition);
            game.initState();
            BEGINPAINT();
            m_graphics->unmarkMouse();
            m_graphics->paintGamePositions();
            m_graphics->beginDragPiece(point, key);
            m_selectedPiece = m_removedPiece = EMPTYPIECEKEY;
            ENDPAINT();
          }
        } else {                       // drag piece from one field to another
          const PieceKey oldKey = game.removePieceAtPosition(m_selectedPosition);
          try {
            game.setPieceAtPosition(m_removedPiece, m_selectedPosition);
            const PieceKey newRemovedPiece = game.setPieceAtPosition(m_selectedPiece, newPos); // can throw
            game.initState();

            BEGINPAINT();
            m_graphics->unmarkMouse();
            m_graphics->paintGamePositions();
            m_removedPiece     = newRemovedPiece;
            m_selectedPosition = newPos;
            m_graphics->markMouse(newPos);
            ENDPAINT();
          } catch(Exception e) {
            game.setPieceAtPosition(oldKey, m_selectedPosition); // set it back again
            game.initState();
          }
        }
      }
    }
  }
}

typedef struct {
  PieceKey  m_key;
  int       m_commandId;
} ContextMenuItem;

void CChessDlg::OnContextMenu(CWnd* pWnd, CPoint point) {
  if(getDialogMode() != EDITMODE) {
    return;
  }

  static const ContextMenuItem itemArray[2][6] = {
    { WHITEKING   , ID_EDIT_ADD_WHITE_KING
     ,WHITEQUEEN  , ID_EDIT_ADD_WHITE_QUEEN
     ,WHITEROOK   , ID_EDIT_ADD_WHITE_ROOK
     ,WHITEBISHOP , ID_EDIT_ADD_WHITE_BISHOP
     ,WHITEKNIGHT , ID_EDIT_ADD_WHITE_KNIGHT
     ,WHITEPAWN   , ID_EDIT_ADD_WHITE_PAWN
    },
    { BLACKKING   , ID_EDIT_ADD_BLACK_KING
     ,BLACKQUEEN  , ID_EDIT_ADD_BLACK_QUEEN
     ,BLACKROOK   , ID_EDIT_ADD_BLACK_ROOK
     ,BLACKBISHOP , ID_EDIT_ADD_BLACK_BISHOP
     ,BLACKKNIGHT , ID_EDIT_ADD_BLACK_KNIGHT
     ,BLACKPAWN   , ID_EDIT_ADD_BLACK_PAWN
    }
  };

  static const int colorMarkResourceId[] = {
    IDB_BITMAPWHITEMARK
   ,IDB_BITMAPBLACKMARK
  };

  m_selectedPosition = getBoardPosition(point, true);
  if(!isValidPosition(m_selectedPosition)) {
    return;
  }

  const Game &game = getCurrentGame();
  CMenu pieceMenu[2];
  CBitmap bitmaps[2][6];
  bool subMenuCreated = false;
  forEachPlayer(player) {
    CMenu &pm = pieceMenu[player];
    for(int i = 0; i < ARRAYSIZE(itemArray[player]); i++) {
      const ContextMenuItem &item = itemArray[player][i];
      if(game.isAddPieceLegal(item.m_key, m_selectedPosition)) {
        if(pm.m_hMenu == NULL) {
          pm.CreateMenu();
          subMenuCreated = true;
        }
        pm.AppendMenu(0, item.m_commandId, &m_graphics->getResources().getSmallPieceBitmap(bitmaps[player][i], item.m_key));
      }
    }
  }
  if(!game.isPositionEmpty(m_selectedPosition)) {
    subMenuCreated = true;
  }
  if(!subMenuCreated) {
    return;
  }

  CMenu menu;
  menu.CreateMenu();
  CBitmap colorBitmap[2];
  CBitmap deleteBitmap;
  forEachPlayer(p) {
    CMenu &pm = pieceMenu[p];
    if(pm.m_hMenu) {
      CBitmap &cbm = colorBitmap[p];
      cbm.LoadBitmap(colorMarkResourceId[p]);
      menu.AppendMenu(MF_POPUP, (UINT_PTR)((HMENU)pm), &cbm);
    }
  }
  if(!game.isPositionEmpty(m_selectedPosition)) {
    deleteBitmap.LoadBitmap(IDB_BITMAPDELETEMARK);
    menu.AppendMenu(MF_STRING, ID_EDIT_REMOVE, &deleteBitmap);
  }
  CMenu topMenu;
  topMenu.CreateMenu();
  topMenu.AppendMenu(MF_POPUP, (UINT_PTR)((HMENU)menu), _T("Edit"));
  topMenu.GetSubMenu(0)->TrackPopupMenu(TPM_LEFTALIGN|TPM_RIGHTBUTTON, point.x,point.y, this);
}

void CChessDlg::OnEditAddWhiteKing()   { setPiece(WHITEKING   ); }
void CChessDlg::OnEditAddWhiteQueen()  { setPiece(WHITEQUEEN  ); }
void CChessDlg::OnEditAddWhiteRook()   { setPiece(WHITEROOK   ); }
void CChessDlg::OnEditAddWhiteBishop() { setPiece(WHITEBISHOP ); }
void CChessDlg::OnEditAddWhiteKnight() { setPiece(WHITEKNIGHT ); }
void CChessDlg::OnEditAddWhitePawn()   { setPiece(WHITEPAWN   ); }
void CChessDlg::OnEditAddBlackKing()   { setPiece(BLACKKING   ); }
void CChessDlg::OnEditAddBlackQueen()  { setPiece(BLACKQUEEN  ); }
void CChessDlg::OnEditAddBlackRook()   { setPiece(BLACKROOK   ); }
void CChessDlg::OnEditAddBlackBishop() { setPiece(BLACKBISHOP ); }
void CChessDlg::OnEditAddBlackKnight() { setPiece(BLACKKNIGHT ); }
void CChessDlg::OnEditAddBlackPawn()   { setPiece(BLACKPAWN   ); }

void CChessDlg::setPiece(PieceKey key) {
  try {
    Game &game = m_editHistory.saveState();
    const PieceKey removedPiece = game.setPieceAtPosition(key, m_selectedPosition);
    if(removedPiece != EMPTYPIECEKEY) {
      game.resetCapturedPieceTypes(GET_PLAYER_FROMKEY(removedPiece));
    }
    game.initState();
    paintGamePosition();
  } catch(Exception e) {
    errorMessage(e);
  }
}

// ----------------------------------------------------------------------------------------

void CChessDlg::OnEditUndo()    { editUndo(UNDO_MOVE); }
void CChessDlg::OnEditUndoPly() { editUndo(UNDO_PLY);  }
void CChessDlg::OnEditUndoAll() { editUndo(UNDO_ALL);  }

void CChessDlg::editUndo(UndoMode mode) {
  try {
    switch(getDialogMode()) {
    case PLAYMODE   :
      { const int plyCount = getPlyCount();
        if(plyCount > 0) {
          if(!getChessPlayer(getComputerPlayer()).acceptUndoMove()) {
            MessageBox(loadString(IDS_MSG_UNDOREFUSED).cstr(), NULL, MB_OK | MB_ICONEXCLAMATION);
            return;
          }
          stopAllBackgroundActivity(true);
          BEGINPAINT();
          m_graphics->unmarkAll();
          switch(mode) {
          case UNDO_MOVE:
            if(getPlayerInTurn() == getHumanPlayer()) {
              unExecuteLastPly();
              if(plyCount > 1) {
                unExecuteLastPly();
              }
            } else { // playerInTurn == computer. Undo only one
              unExecuteLastPly();
            }
            break;
          case UNDO_PLY :
            if(plyCount > 1) {
              unExecuteLastPly();
            }
            break;
          case UNDO_ALL :
            while(getPlyCount() > 0) {
              unExecuteLastPly();
            }
            break;
          }
          paintGamePosition();
          ENDPAINT();
          notifyGameChanged(getCurrentGame());
        }
      }
      break;
    case EDITMODE :
      if(m_editHistory.canUndo()) {
        m_editHistory.undo(mode == UNDO_ALL);
        paintGamePosition();
      }
      break;
    case DEBUGMODE  :
    case ANALYZEMODE:
      if(getPlyCount() > m_startPlyIndex) {
        BEGINPAINT();
        m_graphics->unmarkAll();
        for(int count = (mode == UNDO_ALL) ? (getPlyCount() - m_startPlyIndex) : 1; count--;) {
          unExecuteLastPly();
        }
        paintGamePosition();
        ENDPAINT();
        notifyGameChanged(getCurrentGame());
      }
      break;
    }
  } catch(Exception e) {
    errorMessage(e);
  }
}

void CChessDlg::unExecuteLastPly() {
#ifdef TABLEBASE_BUILDER
  if(isMenuItemChecked(this,ID_TEST_MOVEBACKWARDS)) {
    unExecuteLastBackMove();
    return;
  }
#endif
  getCurrentGame().unExecuteLastMove();
}

void CChessDlg::OnEditRedo() {
  try {
    switch(getDialogMode()) {
    case PLAYMODE:
      break;
    case EDITMODE:
      m_editHistory.redo();
      paintGamePosition();
      break;
    case DEBUGMODE   :
      break;
    case AUTOPLAYMODE:
      break;
    case ANALYZEMODE:
      errorMessage(_T("%s not implemented in analyzeMode"), __TFUNCTION__);
      // OnEditRedoexecuteMove(m_savedGame.getMove(getCurrentGame().getPlyCount()));
      break;
    default:
      invalidModeError(__TFUNCTION__);
      break;
    }
  } catch(Exception e) {
    errorMessage(e);
  }
}

void CChessDlg::OnEditCopyFEN() {
#ifndef TABLEBASE_BUILDER
  putClipboard(m_hWnd, getCurrentGame().toFENString());
#else
  errorMessage(_T("Copy FEN not available in BUILDER_MODE"));
#endif
}

void CChessDlg::OnEditPasteFEN() {
#ifndef TABLEBASE_BUILDER
  try {
    const String text = getClipboardText();
    Game tmp;
    tmp.fromFENString(text);
    Game &game = m_editHistory.saveState();
    game = tmp;
    game.beginSetup();
    paintGamePosition();
  } catch(Exception e) {
    errorMessage(e);
  }
#else
  errorMessage(_T("Paste FEN not available in BUILDER_MODE"));
#endif

}

void CChessDlg::OnEscape() {
  try {
    switch(getDialogMode()) {
    case PLAYMODE:
      if(isThinking()) {
        OnMoveNow();
      }
      break;

    case EDITMODE:
      if(!m_editHistory.isModified()) {
        m_editHistory.endEdit();
        popDialogMode();
      } else if(confirmCancel(this)) {
        m_editHistory.endEdit();
        popDialogMode();
      }
      break;

    case DEBUGMODE:
      if(isThinking()) {
        OnMoveNow();
      } else {
        popDialogMode();
      }
      break;

    case AUTOPLAYMODE:
      popDialogMode();
      checkAutoplayMenuItems(false, false);
      break;
    case ANALYZEMODE:
      if(isThinking()) {
        OnMoveNow();
      } else {
        setGameToPosition0();
        paintGamePosition();
      }
      break;
    case KNIGHTROUTEMODE: 
      popDialogMode();
      break;

    default:
      invalidModeError(__TFUNCTION__);
      break;
    }
  } catch(Exception e) {
    errorMessage(e);
  }
}

void CChessDlg::OnEditValidateBoard() {
  setValidateAfterEdit(toggleMenuItem(this, ID_EDIT_VALIDATE_BOARD));
}

void CChessDlg::setValidateAfterEdit(bool validate) {
  checkMenuItem(this, ID_EDIT_VALIDATE_BOARD,validate);
  getOptions().setValidateAfterEdit(validate);
}

// ----------------------------------- Settings -------------------------------------------

void CChessDlg::OnSettingsStartThinking() {
  startClock();
  startThinking();
}

void CChessDlg::OnMoveNow() {
  forEachPlayer(p) {
    if(getChessPlayer(p).isBusy()) {
      getChessPlayer(p).moveNow();
      break;
    }
  }
}

void CChessDlg::OnAutoPlayRestartAtEnd() {
  checkAutoplayMenuItems(true, false);
  pushDialogMode(AUTOPLAYMODE);
}

void CChessDlg::OnAutoPlayNoRestart() {
  checkAutoplayMenuItems(false, true);
  pushDialogMode(AUTOPLAYMODE);
}

void CChessDlg::checkAutoplayMenuItems(bool restartChecked, bool noRestartChecked) {
  checkMenuItem(this, ID_AUTOPLAY_RESTART_AT_END, restartChecked  );
  checkMenuItem(this, ID_AUTOPLAY_NO_RESTART    , noRestartChecked);
}

void CChessDlg::OnAutoPlayBothLevel1()  { setBothAutoPlayLevel( 1); }
void CChessDlg::OnAutoPlayBothLevel2()  { setBothAutoPlayLevel( 2); }
void CChessDlg::OnAutoPlayBothLevel3()  { setBothAutoPlayLevel( 3); }
void CChessDlg::OnAutoPlayBothLevel4()  { setBothAutoPlayLevel( 4); }
void CChessDlg::OnAutoPlayBothLevel5()  { setBothAutoPlayLevel( 5); }
void CChessDlg::OnAutoPlayBothLevel6()  { setBothAutoPlayLevel( 6); }
void CChessDlg::OnAutoPlayWhiteLevel1() { setWhiteAutoPlayLevel(1); }
void CChessDlg::OnAutoPlayWhiteLevel2() { setWhiteAutoPlayLevel(2); }
void CChessDlg::OnAutoPlayWhiteLevel3() { setWhiteAutoPlayLevel(3); }
void CChessDlg::OnAutoPlayWhiteLevel4() { setWhiteAutoPlayLevel(4); }
void CChessDlg::OnAutoPlayWhiteLevel5() { setWhiteAutoPlayLevel(5); }
void CChessDlg::OnAutoPlayWhiteLevel6() { setWhiteAutoPlayLevel(6); }
void CChessDlg::OnAutoPlayBlackLevel1() { setBlackAutoPlayLevel(1); }
void CChessDlg::OnAutoPlayBlackLevel2() { setBlackAutoPlayLevel(2); }
void CChessDlg::OnAutoPlayBlackLevel3() { setBlackAutoPlayLevel(3); }
void CChessDlg::OnAutoPlayBlackLevel4() { setBlackAutoPlayLevel(4); }
void CChessDlg::OnAutoPlayBlackLevel5() { setBlackAutoPlayLevel(5); }
void CChessDlg::OnAutoPlayBlackLevel6() { setBlackAutoPlayLevel(6); }

void CChessDlg::setBothAutoPlayLevel( int level) {
  setWhiteAutoPlayLevel(level);
  setBlackAutoPlayLevel(level);
}

void CChessDlg::setWhiteAutoPlayLevel(int level) { setPlayerAutoPlayLevel(WHITEPLAYER,level); }
void CChessDlg::setBlackAutoPlayLevel(int level) { setPlayerAutoPlayLevel(BLACKPLAYER,level); }

void CChessDlg::setPlayerAutoPlayLevel(Player player, int level) {
  static const int bothLevelItems[LEVELCOUNT] = {  ID_AUTOPLAY_BOTH_LEVEL_1
                                                  ,ID_AUTOPLAY_BOTH_LEVEL_2
                                                  ,ID_AUTOPLAY_BOTH_LEVEL_3
                                                  ,ID_AUTOPLAY_BOTH_LEVEL_4
                                                  ,ID_AUTOPLAY_BOTH_LEVEL_5
                                                  ,ID_AUTOPLAY_BOTH_LEVEL_6
  };

  static const int whiteLevelItems[LEVELCOUNT] = { ID_AUTOPLAY_WHITE_LEVEL_1
                                                 , ID_AUTOPLAY_WHITE_LEVEL_2
                                                 , ID_AUTOPLAY_WHITE_LEVEL_3
                                                 , ID_AUTOPLAY_WHITE_LEVEL_4
                                                 , ID_AUTOPLAY_WHITE_LEVEL_5
                                                 , ID_AUTOPLAY_WHITE_LEVEL_6
  };

  static const int blackLevelItems[LEVELCOUNT] = { ID_AUTOPLAY_BLACK_LEVEL_1
                                                 , ID_AUTOPLAY_BLACK_LEVEL_2
                                                 , ID_AUTOPLAY_BLACK_LEVEL_3
                                                 , ID_AUTOPLAY_BLACK_LEVEL_4
                                                 , ID_AUTOPLAY_BLACK_LEVEL_5
                                                 , ID_AUTOPLAY_BLACK_LEVEL_6
  };

#define UNCHECKITEMS(a, n) { for(int i = 0; i < n; i++) checkMenuItem(this, a[i], false); }

  const int *levelItems = (player == WHITEPLAYER) ? whiteLevelItems : blackLevelItems;

  try {
    UNCHECKITEMS(levelItems, LEVELCOUNT);
    if(level < 1 || level > LEVELCOUNT) {
      level = 1;
    }

    const int checkItem = levelItems[level-1];
    checkMenuItem(this, checkItem, true);
    Options &options = getOptions();
    if(player == WHITEPLAYER) {
      options.setAutoPlayLevel(WHITEPLAYER, level);
    } else {
      options.setAutoPlayLevel(BLACKPLAYER, level);
    }

    UNCHECKITEMS(bothLevelItems, LEVELCOUNT);
    if(options.getAutoPlayLevel(WHITEPLAYER) == options.getAutoPlayLevel(BLACKPLAYER)) {
      checkMenuItem(this, bothLevelItems[level-1], true);
    }
  } catch(Exception e) {
    errorMessage(e);
  }
}

// ------------------------------------ Settings/View -------------------------------------------------

void CChessDlg::OnView() {
  CViewDlg dlg;
  if(dlg.DoModal() == IDOK) {
    setView();
  }
}

void CChessDlg::setView() {
  const Options &options = getOptions();
  BEGINPAINT();
  m_graphics->setShowFieldNames(  options.getShowFieldNames()  );
  m_graphics->setShowLegalMoves(  options.getShowLegalMoves()  );
  m_graphics->setShowPlayerInTurn(options.getShowPlayerInTurn());
  setVisibleClocks();
  ENDPAINT();
}

// ------------------------------ Settings/Level + Clock -----------------------------------------------

void CChessDlg::OnLevelTimedGame()  { setLevel(LEVEL_TIMEDGAME ); }
void CChessDlg::OnLevelSpeedChess() { setLevel(LEVEL_SPEEDCHESS); }
void CChessDlg::OnLevel1()          { setLevel(1);                }
void CChessDlg::OnLevel2()          { setLevel(2);                }
void CChessDlg::OnLevel3()          { setLevel(3);                }
void CChessDlg::OnLevel4()          { setLevel(4);                }
void CChessDlg::OnLevel5()          { setLevel(5);                }
void CChessDlg::OnLevel6()          { setLevel(6);                }

void CChessDlg::OnLevelEditTimeSettings() {
  CTimeSettingsDlg dlg(m_watch.isRunning());
  if(dlg.DoModal() == IDOK) {
    if(dlg.getResetClocks()) {
      OnSettingsResetClock();
    }
  }
}

void CChessDlg::OnSettingsToggleClock() {
  switch(m_watch.getState()) {
  case CW_STOPPED: startClock();  break;
  case CW_RUNNING: pauseClock();  break;
  case CW_PAUSED : resumeClock(); break;
  }
}

void CChessDlg::OnSettingsResetClock() {
  stopClock();
  m_watch.reset(false);
  updateClock();
}

void CChessDlg::setLevel(int level) {
  static const LevelMenuItem itemArray[] = {
    LEVEL_SPEEDCHESS , ID_LEVEL_SPEEDCHESS
   ,LEVEL_TIMEDGAME  , ID_LEVEL_TIMED_GAME
   ,1                , ID_LEVEL_1
   ,2                , ID_LEVEL_2
   ,3                , ID_LEVEL_3
   ,4                , ID_LEVEL_4
   ,5                , ID_LEVEL_5
   ,6                , ID_LEVEL_6
  };
  const LevelMenuItem *item = itemArray+2;
  try {
    for(int i = 0; i < ARRAYSIZE(itemArray); i++) {
      item = itemArray + i;
      if(item->m_level == level) {
        break;
      }
    }

    for(int i = 0; i < ARRAYSIZE(itemArray); i++) {
      checkMenuItem(this, itemArray[i].m_menuId, i == (item - itemArray));
    }
    getOptions().setNormalPlayLevel(item->m_level);
    setVisibleClocks();
  } catch(Exception e) {
    errorMessage(e);
  }
}

void CChessDlg::setVisibleClocks() {
  const char oldVisible = getVisibleClocks();
  char visible = 0;
  if(isGameWithTimeLimit()) {
    visible = BOTHCLOCKS_VISIBLE;
  } else if(getOptions().getShowComputerTime()) {
    switch(getDialogMode()) {
    case PLAYMODE    :
      visible = (getComputerPlayer() == WHITEPLAYER) ? WHITECLOCK_VISIBLE : BLACKCLOCK_VISIBLE;
      break;
    case AUTOPLAYMODE:
      visible = BOTHCLOCKS_VISIBLE;
      break;
    }
  }
  m_graphics->setVisibleClocks(visible);
  if(!visible) {
    stopClock();
  } else if(visible != oldVisible) {
    OnSettingsResetClock();
  }
}

void CChessDlg::startClock() {
  if(isGameWithTimeLimit()) {
    m_watch.start(false);
    startTimer(SHOWWATCH_TIMER, 1000);
    setClockMenuText();
  }
}

void CChessDlg::stopClock() {
  m_watch.stop();
  stopTimer(SHOWWATCH_TIMER);
  setClockMenuText();
}

void CChessDlg::pauseClock() {
  m_watch.pause();
  stopTimer(SHOWWATCH_TIMER);
  setClockMenuText();
}

void CChessDlg::resumeClock() {
  if(isGameWithTimeLimit()) {
    m_watch.resume();
    startTimer(SHOWWATCH_TIMER, 200);
    setClockMenuText();
  }
}

void CChessDlg::startComputerTimeTimer() {
  if(isThinktimeClockActive()) {
    m_watch.start(getDialogMode() == AUTOPLAYMODE);
    updateClock();
    startTimer(SHOWWATCH_TIMER, 200);
  }
}

void CChessDlg::stopComputerTimeTimer() {
  if(isThinktimeClockActive()) {
    stopTimer(SHOWWATCH_TIMER);
    m_watch.stop();
  }
}

void CChessDlg::setClockMenuText() {
  String menuText;
  switch(m_watch.getState()) {
  case CW_STOPPED:
  case CW_PAUSED : menuText = loadString(IDS_STARTCLOCK);break;
  case CW_RUNNING: menuText = loadString(IDS_STOPCLOCK );break;
  }
  setMenuItemText(this, ID_SETTINGS_TOGGLECLOCK, menuText);
  enableClockControlMenu();
}

void CChessDlg::enableClockControlMenu() {
  const bool enabled = isGameWithTimeLimit();
  const bool enableToggleClock = enabled && (m_watch.getState() != CW_STOPPED);
  enableMenuItem(           this, ID_SETTINGS_TOGGLECLOCK, enableToggleClock);
  enableMenuItem(           this, ID_SETTINGS_RESETCLOCK , enabled);
  enableSubMenuContainingId(this, ID_SETTINGS_RESETCLOCK , enabled);
}

void CChessDlg::updateClock() {
  const UINT t[2] = {
    m_watch.getSecondsRemaining(WHITEPLAYER)
   ,m_watch.getSecondsRemaining(BLACKPLAYER)
  };
  m_graphics->showClocks(t[WHITEPLAYER], t[BLACKPLAYER]);

  if(isGameWithTimeLimit() && (getPlayerInTurn() == getHumanPlayer()) && (t[getPlayerInTurn()] == 0)) {
    stopClock();
    handleEndGameForPlayMode(loadString(IDS_MSG_COMPUTER_WINONTIME), loadString(IDS_TIME));
  }
}


// ---------------------------------------- Settings/Engine -------------------------------------------------

void CChessDlg::OnWhiteEngineSettings() { engineEditSettings(WHITEPLAYER); }
void CChessDlg::OnBlackEngineSettings() { engineEditSettings(BLACKPLAYER); }
void CChessDlg::OnWhiteEngineGetState() { enginePrintState(  WHITEPLAYER); }
void CChessDlg::OnBlackEngineGetState() { enginePrintState(  BLACKPLAYER); }

void CChessDlg::engineEditSettings(Player player) {
#ifndef TABLEBASE_BUILDER
  CEngineOptionsDlg dlg(player);
  if(dlg.DoModal() == IDOK) {
    getChessPlayer(player).resetMoveFinder();
  }
#else
  MessageBox(_T("Cannot edit options for extern engines in BUILDER_MODE"), _T("Error"), MB_ICONEXCLAMATION);
#endif
}

void CChessDlg::enginePrintState(Player player) {
  if(isTraceWindowVisible()) {
    verbose(_T("%s\n"), getChessPlayer(player).toString(shiftKeyPressed()).cstr());
  }
}

void CChessDlg::buildEngineMenues() {
  int index;
  try {
    const Options &options = getOptions();
    addEnginesToMenu(findMenuContainingId(*GetMenu(), ID_WHITE_ENGINE_GETSTATE, index), ID_WHITE_EXTERNENGINE0);
    addEnginesToMenu(findMenuContainingId(*GetMenu(), ID_BLACK_ENGINE_GETSTATE, index), ID_BLACK_EXTERNENGINE0);
    setWhiteExternEngine(options.getCurrentEngineIndex(WHITEPLAYER));
    setBlackExternEngine(options.getCurrentEngineIndex(BLACKPLAYER));
  } catch(Exception e) {
    errorMessage(e);
  }
}

void CChessDlg::addEnginesToMenu(HMENU menu, int startId) {
  int separatorIndex = 0;
  for(int i = GetMenuItemCount(menu); i--;)  { // dont remove first 2 items
    if(getMenuItemType(menu, i) == MFT_SEPARATOR) {
      separatorIndex = i;
      break;
    }
    RemoveMenu(menu, i, MF_BYPOSITION);
  }

  const EngineRegister &engineList = Options::getEngineRegister();
  for(UINT i = 0; i < engineList.size(); i++) {
    const EngineDescription &desc = engineList[i];
    insertMenuItem(menu, i+separatorIndex+1, desc.getName(),  startId + i);
  }
}

void CChessDlg::OnExternEngineSwap() {
  Options &options = getOptions();
  const int whiteIndex = options.getCurrentEngineIndex(WHITEPLAYER);
  const int blackIndex = options.getCurrentEngineIndex(BLACKPLAYER);

  setWhiteExternEngine(blackIndex);
  setBlackExternEngine(whiteIndex);
}

void CChessDlg::OnWhiteEngine0()  { setWhiteExternEngine(  0); }
void CChessDlg::OnWhiteEngine1()  { setWhiteExternEngine(  1); }
void CChessDlg::OnWhiteEngine2()  { setWhiteExternEngine(  2); }
void CChessDlg::OnWhiteEngine3()  { setWhiteExternEngine(  3); }
void CChessDlg::OnWhiteEngine4()  { setWhiteExternEngine(  4); }
void CChessDlg::OnWhiteEngine5()  { setWhiteExternEngine(  5); }
void CChessDlg::OnWhiteEngine6()  { setWhiteExternEngine(  6); }
void CChessDlg::OnWhiteEngine7()  { setWhiteExternEngine(  7); }
void CChessDlg::OnWhiteEngine8()  { setWhiteExternEngine(  8); }
void CChessDlg::OnWhiteEngine9()  { setWhiteExternEngine(  9); }
void CChessDlg::OnWhiteEngine10() { setWhiteExternEngine( 10); }
void CChessDlg::OnWhiteEngine11() { setWhiteExternEngine( 11); }
void CChessDlg::OnWhiteEngine12() { setWhiteExternEngine( 12); }
void CChessDlg::OnWhiteEngine13() { setWhiteExternEngine( 13); }
void CChessDlg::OnWhiteEngine14() { setWhiteExternEngine( 14); }
void CChessDlg::OnWhiteEngine15() { setWhiteExternEngine( 15); }
void CChessDlg::OnWhiteEngine16() { setWhiteExternEngine( 16); }
void CChessDlg::OnWhiteEngine17() { setWhiteExternEngine( 17); }
void CChessDlg::OnWhiteEngine18() { setWhiteExternEngine( 18); }
void CChessDlg::OnWhiteEngine19() { setWhiteExternEngine( 19); }
void CChessDlg::OnWhiteEngine20() { setWhiteExternEngine( 20); }

void CChessDlg::OnBlackEngine0()  { setBlackExternEngine(  0); }
void CChessDlg::OnBlackEngine1()  { setBlackExternEngine(  1); }
void CChessDlg::OnBlackEngine2()  { setBlackExternEngine(  2); }
void CChessDlg::OnBlackEngine3()  { setBlackExternEngine(  3); }
void CChessDlg::OnBlackEngine4()  { setBlackExternEngine(  4); }
void CChessDlg::OnBlackEngine5()  { setBlackExternEngine(  5); }
void CChessDlg::OnBlackEngine6()  { setBlackExternEngine(  6); }
void CChessDlg::OnBlackEngine7()  { setBlackExternEngine(  7); }
void CChessDlg::OnBlackEngine8()  { setBlackExternEngine(  8); }
void CChessDlg::OnBlackEngine9()  { setBlackExternEngine(  9); }
void CChessDlg::OnBlackEngine10() { setBlackExternEngine( 10); }
void CChessDlg::OnBlackEngine11() { setBlackExternEngine( 11); }
void CChessDlg::OnBlackEngine12() { setBlackExternEngine( 12); }
void CChessDlg::OnBlackEngine13() { setBlackExternEngine( 13); }
void CChessDlg::OnBlackEngine14() { setBlackExternEngine( 14); }
void CChessDlg::OnBlackEngine15() { setBlackExternEngine( 15); }
void CChessDlg::OnBlackEngine16() { setBlackExternEngine( 16); }
void CChessDlg::OnBlackEngine17() { setBlackExternEngine( 17); }
void CChessDlg::OnBlackEngine18() { setBlackExternEngine( 18); }
void CChessDlg::OnBlackEngine19() { setBlackExternEngine( 19); }
void CChessDlg::OnBlackEngine20() { setBlackExternEngine( 20); }

void CChessDlg::setWhiteExternEngine(int index) { setExternEngine(WHITEPLAYER, ID_WHITE_EXTERNENGINE0, index); }
void CChessDlg::setBlackExternEngine(int index) { setExternEngine(BLACKPLAYER, ID_BLACK_EXTERNENGINE0, index); }

void CChessDlg::setExternEngine(Player player, int startMenuId, int index) {
  const EngineRegister &engines = Options::getEngineRegister();
  if(index > (int)engines.size()) {
    errorMessage(_T("%s:Invalid argument. index=%d size of engine register=%d")
                ,__TFUNCTION__
                , index, engines.size());
    return;
  }
  if(index >= 0) {
    getOptions().setEngineName(player, engines[index].getName());
  }
  getChessPlayer(player).resetMoveFinder();
  for(int i = 0; i <= 20; i++) {
    checkMenuItem(this, startMenuId+i, index==i);
  }
}

void CChessDlg::OnExternEngineRegister() {
  CEnginesDlg dlg;
  if(dlg.DoModal() == IDOK) {
    buildEngineMenues();
  }
}

void CChessDlg::OnTraceOpening() {
  toggleMenuItem(this, ID_VIEW_TRACEOPENING);
  updateTitle();
}

// ---------------------------------- Settings/Opeingslibrary ----------------------------------

void CChessDlg::OnOpeningLibraryEnabled() {
  getOptions().enableOpeningLibrary(toggleMenuItem(this, ID_OPENINGLIBRARY_ENABLED));
  setGameSettings();
}

// ---------------------------------- Settings/Endgame tablebase ----------------------------------

void CChessDlg::OnTablebaseSettings() {
  CTablebaseDlg dlg;
  if(dlg.DoModal() == IDOK) {
    setGameSettings();
  }
}

void CChessDlg::OnEndGameTablebaseEnabled() {
  getOptions().enableEndGameTablebase(toggleMenuItem(this, ID_ENDGAMETABLEBASE_ENABLED));
  setGameSettings();
}

void CChessDlg::setGameSettings() {
  const Options &options = getOptions();
#ifndef TABLEBASE_BUILDER
  getCurrentGame().setMaxPlyCountWithoutCaptureOrPawnMove(options.getMaxMovesWithoutCaptureOrPawnMove()*2);
#endif
  EndGameKeyDefinition::setMetric(options.getEndGameTablebaseMetric());
  EndGameKeyDefinition::setDbPath(options.getEndGameTablebasePath());
  checkMenuItem( this, ID_OPENINGLIBRARY_ENABLED  , options.isOpeningLibraryEnabled()  );
  checkMenuItem( this, ID_ENDGAMETABLEBASE_ENABLED, options.isEndGameTablebaseEnabled());
  enableMenuItem(this, ID_TABLEBASE_SETTINGS      , options.isEndGameTablebaseEnabled());
}

// ------------------------------ Settings/Log Window -----------------------------------------------

void CChessDlg::OnTraceWindow() {
  setTraceWindowVisible(toggleMenuItem(this, ID_VIEW_TRACEWINDOW));
}

bool CChessDlg::isTraceWindowVisible() const {
  return s_traceThread->isActive();
}

void CChessDlg::setTraceWindowVisible(bool visible) {
  if(visible != isTraceWindowVisible()) {
    s_traceThread->setActive(visible);
  }
}

// ------------------------------ Settings/Language -----------------------------------------------

void CChessDlg::OnSettingsLanguage0() { setLanguage(0); }
void CChessDlg::OnSettingsLanguage1() { setLanguage(1); }
void CChessDlg::OnSettingsLanguage2() { setLanguage(2); }
void CChessDlg::OnSettingsLanguage3() { setLanguage(3); }
void CChessDlg::OnSettingsLanguage4() { setLanguage(4); }
void CChessDlg::OnSettingsLanguage5() { setLanguage(5); }
void CChessDlg::OnSettingsLanguage6() { setLanguage(6); }
void CChessDlg::OnSettingsLanguage7() { setLanguage(7); }
void CChessDlg::OnSettingsLanguage8() { setLanguage(8); }
void CChessDlg::OnSettingsLanguage9() { setLanguage(9); }

void CChessDlg::setLanguage(int index) {
  const Array<Language> &spla = Language::getSupportedLanguages();
  if(index >= (int)spla.size()) {
    errorMessage(_T("setLanguage:Invalid index:%d"), index);
    return;
  }
  LANGID langID = spla[index].m_langID;

  if(PRIMARYLANGID(langID) != PRIMARYLANGID(getOptions().getLangID())) {
    if(MessageBox(loadString(IDS_RESTARTQUESTION).cstr(), loadString(IDS_GAMENAME).cstr(), MB_YESNO | MB_ICONQUESTION) == IDYES) {
      getOptions().setLangID(langID);

      const String programName = getModuleFileName();
      if(_tspawnlp(_P_NOWAITO, programName.cstr(), programName.cstr(), NULL) == -1) {
        errorMessage(_T("spawn %s failed. %s"), programName.cstr(), _sys_errlist[errno]);
      } else {
        setTraceWindowVisible(false);
        ShowWindow(SW_HIDE);
        Sleep(200);
        OnFileExit();
      }
    }
  }
}

// ----------------------------------- Help functions -------------------------------------------

void CChessDlg::OnHelpHint() {
  startFindHint(5000);
}

void CChessDlg::OnHelpInfiniteHint() {
  setTraceWindowVisible(true);
  startFindHint(INFINITE);
}

void CChessDlg::OnHelpAboutChess() {
  CAboutDlg dlg(this);
  dlg.DoModal();
}

void CChessDlg::OnShowOpeningLibrary() {
  startNewOpeningDialogThread();
}

// ------------------------------ Mousehandlers Debug Mode -----------------------------------------------

void CChessDlg::OnLButtonDownDebugMode(UINT nFlags, CPoint point) {
  m_selectedPosition = getBoardPosition(point);
  if(!isValidPosition(m_selectedPosition)) {
    return;
  }

  const Piece *selectedPiece = m_graphics->getSelectedPiece();
  Game &game = getCurrentGame();

#ifdef TABLEBASE_BUILDER
  if(isMenuItemChecked(this, ID_TEST_MOVEBACKWARDS)) {
    game.setEndGameKeyDefinition();
    if(selectedPiece == NULL) {
      m_graphics->markSelectedPiece(m_selectedPosition);
    } else if(game.getLegalSourceFields(selectedPiece).contains(m_selectedPosition)) {
      const int to = selectedPiece->getPosition();
      executeBackMove(game.generateBackMove(m_selectedPosition, to));
    } else if(m_selectedPosition == selectedPiece->getPosition()) {
      BEGINPAINT();
      m_graphics->unmarkAll();
      m_graphics->markMouse(m_selectedPosition);
      ENDPAINT();
    } else {
      m_graphics->markSelectedPiece(m_selectedPosition);
    }
    return;
  }
#endif

  MoveAnnotation annotation = NOANNOTATION;
  if(nFlags & MK_SHIFT) {
    annotation = GOOD_MOVE;
  } else if(nFlags & MK_CONTROL) {
    annotation = BAD_MOVE;
  }
  if(selectedPiece == NULL) {
    m_graphics->markSelectedPiece(m_selectedPosition);
  } else if(game.getLegalDestinationFields(selectedPiece).contains(m_selectedPosition)) {
    const int from = selectedPiece->getPosition();
    if(game.mustSelectPromotion(from, m_selectedPosition)) {
      selectAndExecutePromotion(point, annotation);
    } else {
      executeMove(game.generateMove(from, m_selectedPosition, NoPiece, annotation));
    }
  } else if(m_selectedPosition == selectedPiece->getPosition()) {
    BEGINPAINT();
    m_graphics->unmarkAll();
    m_graphics->markMouse(m_selectedPosition);
    ENDPAINT();
  } else {
    m_graphics->markSelectedPiece(m_selectedPosition);
  }
}

void CChessDlg::OnLButtonUpDebugMode(UINT nFlags, CPoint point) {
  // do nothing
}

void CChessDlg::OnMouseMoveDebugMode(UINT nFlags, CPoint point) {
  m_graphics->markMouse(getBoardPosition(point));
}

// ------------------------------ Mousehandlers KnightRoute Mode ------------------------------------------

void CChessDlg::OnLButtonDownKRMode(UINT nFlags, CPoint point) {
  const int pos = getBoardPosition(point);
  if(!isValidPosition(pos)) return;

  if(m_selectedPosition < 0) {
    m_selectedPosition = pos;
    m_graphics->markField(m_selectedPosition, YELLOWMARK);
  } else if (pos == m_selectedPosition) {
    m_graphics->unmarkField(m_selectedPosition);
    m_selectedPosition = -1;
  } else {
    m_graphics->markField(pos, YELLOWMARK);
    String str = findShortestKnightRoute(m_selectedPosition, pos);
    putClipboard(*this, str);
    MessageBox(str.cstr(), _T("Best Knight route"));
    BEGINPAINT();
    m_graphics->unmarkField(pos               );
    m_graphics->unmarkField(m_selectedPosition);
    m_selectedPosition = -1;
    ENDPAINT();
  }
}

void CChessDlg::OnLButtonUpKRMode(UINT nFlags, CPoint point) {
}
void CChessDlg::OnMouseMoveKRMode(UINT nFlags, CPoint point) {
//  m_graphics->markMouse(getBoardPosition(point));
}

// -------------------------------- Test functions -------------------------------------------------------

void CChessDlg::toggleEnableTestMenu() {
  enableTestMenu(!getOptions().hasTestMenu());
  invalidate();
}

void CChessDlg::enableTestMenu(bool enabled) {
  if(enabled != menuItemExists(this, ID_TEST_DEBUG)) {
    if(enabled) {
      HMENU testmenu = LoadMenu(theApp.m_hInstance,MAKEINTRESOURCE(IDR_TEST_MENU));
      if(testmenu == NULL) {
        errorMessage(_T("LoadMenu failed:%s"), getLastErrorText().cstr());
        return;
      }
      if(!GetMenu()->AppendMenu(MF_POPUP | MF_ENABLED, (UINT_PTR)GetSubMenu(testmenu,0), getMenuItemText(testmenu,0).cstr())) {
        errorMessage(_T("AppendMenu failed:%s"), getLastErrorText().cstr());
        return;
      }
      checkMenuItem(this, ID_TEST_SHOWENGINECONSOLE, getOptions().getShowEngineConsole());

    } else {
      removeSubMenuContainingId(this, ID_TEST_DEBUG);
    }
    if(isInitDone()) {
      DrawMenuBar();
    }
  }
  getOptions().enableTestMenu(enabled);
}

void CChessDlg::setTestItemStates() {
  if(getOptions().hasTestMenu()) {
    m_graphics->setShowFieldAttacks(    isMenuItemChecked(this, ID_TEST_SHOWFIELDATTACKS   ));
    m_graphics->setShowMaterial(        isMenuItemChecked(this, ID_TEST_SHOWMATERIAL       ));
    m_graphics->setShowCheckingSDAPos(  isMenuItemChecked(this, ID_TEST_SHOWCHECKINGSDAPOS ));
    m_graphics->setShowPawnCount(       isMenuItemChecked(this, ID_TEST_SHOWPAWNCOUNT      ));
    m_graphics->setShowLastCapture(     isMenuItemChecked(this, ID_TEST_SHOWLASTCAPTURE    ));
    m_graphics->setShowPositionRepeats( isMenuItemChecked(this, ID_TEST_SHOWPOSITIONREPEATS));
    m_graphics->setShowLastMoveInfo(    isMenuItemChecked(this, ID_TEST_SHOWLASTMOVEINFO   ));
    m_graphics->setShowBishopFlags(     isMenuItemChecked(this, ID_TEST_SHOWBISHOPFLAGS    ));
  } else {
    m_graphics->resetAllDebugFlags();
  }
}

void CChessDlg::OnTestDebug() {
  if(!getOptions().hasTestMenu()) return;
  pushDialogMode(DEBUGMODE);
}

void CChessDlg::OnTestFindknightroute() {
  if(!getOptions().hasTestMenu()) return;
  pushDialogMode(KNIGHTROUTEMODE);
  invalidate();
}

void CChessDlg::OnTestShowFieldAttacks() {
  if(!getOptions().hasTestMenu()) return;
  m_graphics->setShowFieldAttacks(   toggleMenuItem(this, ID_TEST_SHOWFIELDATTACKS    ));
  invalidate();
}

void CChessDlg::OnTestShowMaterial() {
  if(!getOptions().hasTestMenu()) return;
  m_graphics->setShowMaterial(      toggleMenuItem(this, ID_TEST_SHOWMATERIAL        ));
  invalidate();
}

void CChessDlg::OnTestShowCheckingSDAPos() {
  if(!getOptions().hasTestMenu()) return;
  m_graphics->setShowCheckingSDAPos( toggleMenuItem(this, ID_TEST_SHOWCHECKINGSDAPOS  ));
  invalidate();
}

void CChessDlg::OnTestShowBishopFlags() {
  if(!getOptions().hasTestMenu()) return;
  m_graphics->setShowBishopFlags(    toggleMenuItem(this, ID_TEST_SHOWBISHOPFLAGS     ));
  invalidate();
}

void CChessDlg::OnTestShowPawnCount() {
  if(!getOptions().hasTestMenu()) return;
  m_graphics->setShowPawnCount(      toggleMenuItem(this, ID_TEST_SHOWPAWNCOUNT       ));
  invalidate();
}

void CChessDlg::OnTestShowPositionRepeats() {
  if(!getOptions().hasTestMenu()) return;
  m_graphics->setShowPositionRepeats(toggleMenuItem(this, ID_TEST_SHOWPOSITIONREPEATS ));
  invalidate();
}

void CChessDlg::OnTestShowLastCapture() {
  if(!getOptions().hasTestMenu()) return;
  m_graphics->setShowLastCapture(    toggleMenuItem(this, ID_TEST_SHOWLASTCAPTURE     ));
  invalidate();
}

void CChessDlg::OnTestShowLastMoveInfo() {
  if(!getOptions().hasTestMenu()) return;
  m_graphics->setShowLastMoveInfo(   toggleMenuItem(this, ID_TEST_SHOWLASTMOVEINFO    ));
  invalidate();
}

void CChessDlg::OnTestShowSetupMode() {
  if(!getOptions().hasTestMenu()) return;
  m_graphics->setShowSetupMode(      toggleMenuItem(this, ID_TEST_SHOWSETUPMODE       ));
  invalidate();
}

void CChessDlg::OnTestShowFEN() {
  if(!getOptions().hasTestMenu()) return;
  m_graphics->setShowFEN(      toggleMenuItem(this, ID_TEST_SHOWFEN                   ));
  invalidate();
}

void CChessDlg::OnTestMoveBackwards() {
  if(!getOptions().hasTestMenu()) return;
  setMoveBackwards(                 toggleMenuItem(this, ID_TEST_MOVEBACKWARDS       ));
  invalidate();
}

void CChessDlg::OnTestGenerateFictivePawnCaptures() {
  if(!getOptions().hasTestMenu()) return;
#ifdef TABLEBASE_BUILDER
  getCurrentGame().setGenerateFictivePawnCaptures(toggleMenuItem(this, ID_TEST_FICTIVEPAWNCAPTURES));
#endif
}

void CChessDlg::OnTestTransformEndGamePosition() {
  if(!getOptions().hasTestMenu()) return;
  try {
    Game &game = getCurrentGame();
    if(game.getPositionType() == TABLEBASE_POSITION) {
      bool swapPlayers;
      EndGameTablebase *tb = EndGameTablebase::getInstanceBySignature(game.getPositionSignature(), swapPlayers);
      if(tb == NULL) {
        errorMessage(_T("No tablebase registered for positionsignature %s"), game.getPositionSignature().toString().cstr());
      } else {
        const EndGameKeyDefinition &keydef = tb->getKeyDefinition();
              EndGameKey            egk    = keydef.getEndGameKey(game.getKey().transform(keydef.getPlayTransformation(game)));
                                    egk    = keydef.getTransformedKey(egk, keydef.getSymTransformation(egk));
        const GameKey               gk     = egk.getGameKey(keydef);
        game = gk;
        invalidate();
      }
    }
  } catch(Exception e) {
    errorMessage(e);
  }
}

void CChessDlg::OnTestNormalizeEndGamePosition() {
  if(!getOptions().hasTestMenu()) return;
  try {
    Game &game = getCurrentGame();
    if(game.getPositionType() == TABLEBASE_POSITION) {
      bool swapPlayers;
      EndGameTablebase *tb = EndGameTablebase::getInstanceBySignature(game.getPositionSignature(), swapPlayers);
      if(tb == NULL) {
        errorMessage(_T("No tablebase registered for positionsignature %s"), game.getPositionSignature().toString().cstr());
      } else {
        const EndGameKeyDefinition &keydef = tb->getKeyDefinition();
              EndGameKey            egk    = keydef.getEndGameKey(game.getKey().transform(keydef.getPlayTransformation(game)));
                                    egk    = keydef.getTransformedKey(egk, keydef.getSymTransformation(egk));
                                    egk    = keydef.getNormalizedKey(egk);
        const GameKey               gk     = egk.getGameKey(keydef);
        game = gk;
        invalidate();
      }
    }
  } catch(Exception e) {
    errorMessage(e);
  }
}

void CChessDlg::setMoveBackwards(bool moveBackwards) {
#ifdef TABLEBASE_BUILDER
  if(!getOptions().hasTestMenu()) return;
  checkMenuItem(this,ID_TEST_MOVEBACKWARDS,moveBackwards);
  m_graphics->setShowBackMoves(moveBackwards);
  m_modeStack.top().m_moveBackwards = moveBackwards;
#endif
}

void CChessDlg::OnTestSaveToExternEngine() {
#ifdef __NEVER__
  const String extensions = format(_T("%s%c*.txt%c%s%c*.*%c%c")
                                   ,loadString(IDS_TEXTFILEEXTENSION).cstr() ,0, 0
                                   ,loadString(IDS_ALLFILESEXTENSION).cstr() ,0, 0
                                   ,0);
  const String initialDir = getOptions().getGameInitialDir();

  CFileDialog  dlg(FALSE, _T("txt"));
  dlg.m_ofn.lpstrFilter     = extensions.cstr();
  if(initialDir.length() > 0) {
    dlg.m_ofn.lpstrInitialDir = initialDir.cstr();
  }
  String fileName;
  if(dlg.DoModal() == IDOK) {
    fileName = dlg.m_ofn.lpstrFile;
    try {
      FILE *f = FOPEN(fileName, _T("w"));
      const Game &game = getCurrentGame();
      _ftprintf(f, ExternEngine::getPositionString(game).cstr());
      fclose(f);
    } catch(Exception e) {
      errorMessage(e);
    }
  }
#endif
  MessageBox(_T("Function not implemented"), _T("Error"));
}

void CChessDlg::OnTestShowMessage() {
  if(!getOptions().hasTestMenu()) return;
  static String s;
  CEnterTextDlg dlg(s);
  if(dlg.DoModal() == IDOK) {
    s = dlg.m_text;
    showMessage(this,3000,_T("Test"), _T("%s"), s.cstr());

    MessageBox(s.cstr(), _T("Test"));
  }
}

void CChessDlg::OnTestShowEngineConsole() {
  getOptions().setShowEngineConsole(toggleMenuItem(this, ID_TEST_SHOWENGINECONSOLE));
  forEachPlayer(p) {
    getChessPlayer(p).resetMoveFinder();
  }
}
