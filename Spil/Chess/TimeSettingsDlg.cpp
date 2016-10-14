#include "stdafx.h"
#include <Math.h>
#include <MFCUtil/WinTools.h>
#include "TimeSettingsDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

CTimeSettingsDlg::CTimeSettingsDlg(bool clockRunning, CWnd* pParent) 
: m_clockRunning(clockRunning)
, CDialog(CTimeSettingsDlg::IDD, pParent) {

  const Options &options = getOptions();

  m_tm[0].init(options.getTimeParameters(     WHITEPLAYER), options.getTimeParameters(     BLACKPLAYER));
  m_tm[1].init(options.getSpeedTimeParameters(WHITEPLAYER), options.getSpeedTimeParameters(BLACKPLAYER));
  m_lt = options.getLevelTimeout();

  paramToWindow(m_tm[0].m_tm[WHITEPLAYER], false);
  paramToWindow(m_tm[1].m_tm[WHITEPLAYER], true );
  paramToWindow(m_lt          );

    m_resetClocks = FALSE;
}

void TimeParametersWithComboSelection::init(const TimeParameters &tmw, const TimeParameters &tmb) {
  m_tm[WHITEPLAYER] = tmw;
  m_tm[BLACKPLAYER] = tmb;
  initSelectedComboItem();
}

void TimeParametersWithComboSelection::initSelectedComboItem() {
  if(m_tm[0] == m_tm[1]) {
    m_selectedComboItem = SELECTED_BOTH;
  } else {
    m_selectedComboItem = SELECTED_WHITE;
  }
}

void CTimeSettingsDlg::DoDataExchange(CDataExchange* pDX) {
    CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TAB_CLOCKTYPE, m_clockType);
    DDX_Text(pDX, IDC_EDIT_TIMEOUTMINUTES1, m_min1);
    DDX_Text(pDX, IDC_EDIT_TIMEOUTMINUTES2, m_min2);
    DDX_Text(pDX, IDC_EDIT_TIMEOUTMINUTES3, m_min3);
    DDX_Text(pDX, IDC_EDIT_TIMEOUTMINUTES4, m_min4);
    DDX_Text(pDX, IDC_EDIT_TIMEOUTMINUTES5, m_min5);
    DDX_Text(pDX, IDC_EDIT_TIMEOUTMINUTES6, m_min6);
    DDX_Text(pDX, IDC_EDIT_TIMEOUTSECONDS1, m_sec1);
    DDV_MinMaxFloat(pDX, m_sec1, 0.f, 59.99f);
    DDX_Text(pDX, IDC_EDIT_TIMEOUTSECONDS2, m_sec2);
    DDV_MinMaxFloat(pDX, m_sec2, 0.f, 59.99f);
    DDX_Text(pDX, IDC_EDIT_TIMEOUTSECONDS3, m_sec3);
    DDV_MinMaxFloat(pDX, m_sec3, 0.f, 59.99f);
    DDX_Text(pDX, IDC_EDIT_TIMEOUTSECONDS4, m_sec4);
    DDV_MinMaxFloat(pDX, m_sec4, 0.f, 59.99f);
    DDX_Text(pDX, IDC_EDIT_TIMEOUTSECONDS5, m_sec5);
    DDV_MinMaxFloat(pDX, m_sec5, 0.f, 59.99f);
    DDX_Text(pDX, IDC_EDIT_TIMEOUTSECONDS6, m_sec6);
    DDV_MinMaxFloat(pDX, m_sec6, 0.f, 59.99f);
    DDX_Text(pDX, IDC_EDIT_MOVECOUNT, m_movesLeft);
    DDV_MinMaxUInt(pDX, m_movesLeft, 2, 200);
    DDX_Text(pDX, IDC_EDIT_HOURSLEFT, m_hoursLeft);
    DDV_MinMaxUInt(pDX, m_hoursLeft, 0, 1000);
    DDX_Text(pDX, IDC_EDIT_MINUTESLEFT, m_minutesLeft);
    DDV_MinMaxUInt(pDX, m_minutesLeft, 0, 59);
    DDX_Text(pDX, IDC_EDIT_SECONDSLEFT, m_secondsLeft);
    DDV_MinMaxUInt(pDX, m_secondsLeft, 0, 59);
    DDX_Text(pDX, IDC_EDIT_SECONDSINCR, m_secondsIncr);
    DDV_MinMaxUInt(pDX, m_secondsIncr, 0, 1000);
    DDX_Text(pDX, IDC_EDIT_SPEEDCHESSHOURSLEFT, m_speedChessHoursLeft);
    DDV_MinMaxUInt(pDX, m_speedChessHoursLeft, 0, 1000);
    DDX_Text(pDX, IDC_EDIT_SPEEDCHESSMINUTESLEFT, m_speedChessMinutesLeft);
    DDV_MinMaxUInt(pDX, m_speedChessMinutesLeft, 0, 59);
    DDX_Text(pDX, IDC_EDIT_SPEEDCHESSSECONDSLEFT, m_speedChessSecondsLeft);
    DDV_MinMaxUInt(pDX, m_speedChessSecondsLeft, 0, 59);
    DDX_Text(pDX, IDC_EDIT_SPEEDCHESSSECONDSINCR, m_speedChessSecondsIncr);
    DDV_MinMaxUInt(pDX, m_speedChessSecondsIncr, 0, 1000);
    DDX_Check(pDX, IDC_CHECK_RESETCLOCKS, m_resetClocks);
}

BEGIN_MESSAGE_MAP(CTimeSettingsDlg, CDialog)
    ON_COMMAND(ID_GOTO_MINUTES1                 , OnGotoMinutes1            )
    ON_COMMAND(ID_GOTO_MINUTES2                 , OnGotoMinutes2            )
    ON_COMMAND(ID_GOTO_MINUTES3                 , OnGotoMinutes3            )
    ON_COMMAND(ID_GOTO_MINUTES4                 , OnGotoMinutes4            )
    ON_COMMAND(ID_GOTO_MINUTES5                 , OnGotoMinutes5            )
    ON_COMMAND(ID_GOTO_MINUTES6                 , OnGotoMinutes6            )
    ON_COMMAND(ID_GOTO_PLAYER                   , OnGotoPlayer              )
    ON_COMMAND(ID_GOTO_MOVECOUNT                , OnGotoMoveCount           )
    ON_COMMAND(ID_GOTO_TIME                     , OnGotoTime                )
    ON_COMMAND(ID_GOTO_TIMEINCREMENT            , OnGotoTimeIncrement       )
    ON_CBN_SELCHANGE(IDC_COMBO_PLAYER           , OnSelChangeComboPlayer    )
    ON_COMMAND(ID_SELECT_TIMEDGAMETAB           , OnSelectTimedGame         )
    ON_COMMAND(ID_SELECT_LEVELTSTAB             , OnSelectLevels            )
    ON_COMMAND(ID_SELECT_SPEEDCHESSTAB          , OnSelectSpeedChess        )
	ON_NOTIFY(TCN_SELCHANGE, IDC_TAB_CLOCKTYPE  , OnSelchangeTabClocktype   )
	ON_CBN_SETFOCUS( IDC_COMBO_PLAYER           , OnSetfocusComboplayer     )
    ON_CBN_SELCHANGE(IDC_COMBO_SPEEDPLAYER      , OnSelChangeComboPlayer    )
	ON_CBN_SETFOCUS( IDC_COMBO_SPEEDPLAYER      , OnSetfocusSpeedcomboplayer)
    ON_MESSAGE(ID_MSG_SHOW_DROPDOWN             , OnMsgShowDropdown         )
END_MESSAGE_MAP()

BOOL CTimeSettingsDlg::OnInitDialog() {
  CDialog::OnInitDialog();
  setControlText(IDD, this);

  CTabCtrl &tab = m_clockType;
  tab.InsertItem(0, loadString(IDS_LEVELS    ).cstr()); m_currentControl[TAB_LEVELS    ] = IDC_EDIT_TIMEOUTMINUTES1;
  tab.InsertItem(1, loadString(IDS_TIMEDGAME ).cstr()); m_currentControl[TAB_TIMEDGAME ] = IDC_EDIT_HOURSLEFT;
  tab.InsertItem(2, loadString(IDS_SPEEDCHESS).cstr()); m_currentControl[TAB_SPEEDCHESS] = IDC_EDIT_SPEEDCHESSHOURSLEFT;

  initPlayerComboItems();

  m_accelTable = LoadAccelerators(theApp.m_hInstance, MAKEINTRESOURCE(IDR_TIMESETTINGS_ACCELERATOR));

  getPlayerCombo(TAB_TIMEDGAME )->SetCurSel(m_tm[0].m_selectedComboItem);
  getPlayerCombo(TAB_SPEEDCHESS)->SetCurSel(m_tm[1].m_selectedComboItem);
  switch(getOptions().getNormalPlayLevel()) {
  case LEVEL_TIMEDGAME:
    CheckDlgButton(IDC_CHECK_RESETCLOCKS, m_clockRunning ? BST_UNCHECKED : BST_CHECKED);
    showTimedGameTab();
    gotoEditBox(this, IDC_EDIT_HOURSLEFT);
    break;
  case LEVEL_SPEEDCHESS:
    CheckDlgButton(IDC_CHECK_RESETCLOCKS, m_clockRunning ? BST_UNCHECKED : BST_CHECKED);
    showSpeedChessTab();
    gotoEditBox(this, IDC_EDIT_SPEEDCHESSHOURSLEFT);
    break;
  default:
    GetDlgItem(IDC_CHECK_RESETCLOCKS)->ShowWindow(SW_HIDE);
    showLevelsTab();
    gotoMinutes(getOptions().getNormalPlayLevel()-1);
    break;
  }
  return FALSE;
}

void CTimeSettingsDlg::initPlayerComboItems() {
  initPlayerComboItems(getPlayerCombo(TAB_TIMEDGAME ));
  initPlayerComboItems(getPlayerCombo(TAB_SPEEDCHESS));
}

void CTimeSettingsDlg::initPlayerComboItems(CComboBox *cb) {
  const String bothLabel  = loadString(IDS_BOTH_LABEL);
  const String whiteLabel = getPlayerName(WHITEPLAYER);
  const String blackLabel = getPlayerName(BLACKPLAYER);
  cb->AddString(bothLabel.cstr());
  cb->AddString(whiteLabel.cstr());
  cb->AddString(blackLabel.cstr());
}

ClockTab CTimeSettingsDlg::getCurrentTab() {
  return (ClockTab)m_clockType.GetCurSel();
}

void CTimeSettingsDlg::OnSelchangeTabClocktype(NMHDR* pNMHDR, LRESULT* pResult) {
  switch(getCurrentTab()) {
  case TAB_LEVELS    : showLevelsTab();     break;
  case TAB_TIMEDGAME : showTimedGameTab();  break;
  case TAB_SPEEDCHESS: showSpeedChessTab(); break;
  }
  *pResult = 0;
}

void CTimeSettingsDlg::OnSelectTimedGame() {
  showTimedGameTab();
}

void CTimeSettingsDlg::OnSelectLevels() {
  showLevelsTab();
}

void CTimeSettingsDlg::OnSelectSpeedChess() {
  showSpeedChessTab();
}

void CTimeSettingsDlg::showLevelsTab() {
  enableSpeedChess(false);
  enableTimedGame( false);
  enableLevels(    true );
  m_clockType.SetCurSel(0);
}

void CTimeSettingsDlg::showTimedGameTab() {
  enableLevels(    false);
  enableSpeedChess(false);
  enableTimedGame( true );
  m_clockType.SetCurSel(1);
}

void CTimeSettingsDlg::showSpeedChessTab() {
  enableLevels(    false);
  enableTimedGame( false);
  enableSpeedChess(true );
  m_clockType.SetCurSel(2);
}

void CTimeSettingsDlg::enableLevels(bool enable) {
  static int ctrlId[] = {
    IDC_STATIC_LEVEL
   ,IDC_STATIC_TIMEPERMOVE
   ,IDC_STATIC_MINUTES
   ,IDC_STATIC_SECONDS
   ,IDC_STATIC_LEVEL1LABEL
   ,IDC_STATIC_LEVEL2LABEL
   ,IDC_STATIC_LEVEL3LABEL
   ,IDC_STATIC_LEVEL4LABEL
   ,IDC_STATIC_LEVEL5LABEL
   ,IDC_STATIC_LEVEL6LABEL
   ,IDC_STATIC_LEVEL1COLON
   ,IDC_STATIC_LEVEL2COLON
   ,IDC_STATIC_LEVEL3COLON
   ,IDC_STATIC_LEVEL4COLON
   ,IDC_STATIC_LEVEL5COLON
   ,IDC_STATIC_LEVEL6COLON
   ,IDC_EDIT_TIMEOUTMINUTES1
   ,IDC_EDIT_TIMEOUTSECONDS1
   ,IDC_EDIT_TIMEOUTMINUTES2
   ,IDC_EDIT_TIMEOUTSECONDS2
   ,IDC_EDIT_TIMEOUTMINUTES3
   ,IDC_EDIT_TIMEOUTSECONDS3
   ,IDC_EDIT_TIMEOUTMINUTES4
   ,IDC_EDIT_TIMEOUTSECONDS4
   ,IDC_EDIT_TIMEOUTMINUTES5
   ,IDC_EDIT_TIMEOUTSECONDS5
   ,IDC_EDIT_TIMEOUTMINUTES6
   ,IDC_EDIT_TIMEOUTSECONDS6
  };
  setVisible(enable, ctrlId, ARRAYSIZE(ctrlId));
}

void CTimeSettingsDlg::enableTimedGame(bool enable) {
  static const int ctrlId[] = {
     IDC_STATIC_PLAYER
    ,IDC_COMBO_PLAYER
    ,IDC_STATIC_MOVECOUNT
    ,IDC_EDIT_MOVECOUNT
    ,IDC_STATIC_TIME
    ,IDC_STATIC_TIMEINCR
    ,IDC_STATIC_SECONDS_LC
    ,IDC_STATIC_COLON1
    ,IDC_STATIC_COLON2
    ,IDC_EDIT_HOURSLEFT
    ,IDC_EDIT_MINUTESLEFT
    ,IDC_EDIT_SECONDSLEFT
    ,IDC_EDIT_SECONDSINCR
    ,IDC_CHECK_RESETCLOCKS
  };
  setVisible(enable, ctrlId, ARRAYSIZE(ctrlId));
}

void CTimeSettingsDlg::enableSpeedChess(bool enable) {
  static const int ctrlId[] = {
     IDC_STATIC_PLAYER
    ,IDC_COMBO_SPEEDPLAYER  
    ,IDC_STATIC_TIME
    ,IDC_STATIC_TIMEINCR
    ,IDC_STATIC_SECONDS_LC
    ,IDC_STATIC_COLON1
    ,IDC_STATIC_COLON2
    ,IDC_EDIT_SPEEDCHESSHOURSLEFT
    ,IDC_EDIT_SPEEDCHESSMINUTESLEFT
    ,IDC_EDIT_SPEEDCHESSSECONDSLEFT
    ,IDC_EDIT_SPEEDCHESSSECONDSINCR
    ,IDC_CHECK_RESETCLOCKS
  };
  setVisible(enable, ctrlId, ARRAYSIZE(ctrlId));
}

void CTimeSettingsDlg::setVisible(bool visible, const int *ctrlArray, int size) {
  for(int i = 0; i < size; i++) {
    const int id = ctrlArray[i];
    GetDlgItem(id)->ShowWindow(visible ? SW_SHOW : SW_HIDE);
  }
}

void CTimeSettingsDlg::OnGotoMinutes1() { gotoEditBox(this, IDC_EDIT_TIMEOUTMINUTES1); }
void CTimeSettingsDlg::OnGotoMinutes2() { gotoEditBox(this, IDC_EDIT_TIMEOUTMINUTES2); }
void CTimeSettingsDlg::OnGotoMinutes3() { gotoEditBox(this, IDC_EDIT_TIMEOUTMINUTES3); }
void CTimeSettingsDlg::OnGotoMinutes4() { gotoEditBox(this, IDC_EDIT_TIMEOUTMINUTES4); }
void CTimeSettingsDlg::OnGotoMinutes5() { gotoEditBox(this, IDC_EDIT_TIMEOUTMINUTES5); }
void CTimeSettingsDlg::OnGotoMinutes6() { gotoEditBox(this, IDC_EDIT_TIMEOUTMINUTES6); }

void CTimeSettingsDlg::OnOK() {
  if(!UpdateData()) {
    return;
  }

  if(!windowToParam()) {
    return;
  }

  getOptions().setTimeParameters(     WHITEPLAYER, m_tm[0].m_tm[WHITEPLAYER]);
  getOptions().setTimeParameters(     BLACKPLAYER, m_tm[0].m_tm[BLACKPLAYER]);
  getOptions().setSpeedTimeParameters(WHITEPLAYER, m_tm[1].m_tm[WHITEPLAYER]);
  getOptions().setSpeedTimeParameters(BLACKPLAYER, m_tm[1].m_tm[BLACKPLAYER]);
  getOptions().setLevelTimeout(m_lt);

  CDialog::OnOK();
}

void CTimeSettingsDlg::gotoSeconds(int index) {
  const int editBoxId[] = {
    IDC_EDIT_TIMEOUTSECONDS1
   ,IDC_EDIT_TIMEOUTSECONDS2
   ,IDC_EDIT_TIMEOUTSECONDS3
   ,IDC_EDIT_TIMEOUTSECONDS4
   ,IDC_EDIT_TIMEOUTSECONDS5
   ,IDC_EDIT_TIMEOUTSECONDS6
  };
  if(0 <= index || index < ARRAYSIZE(editBoxId)) {
    gotoEditBox(this, editBoxId[index]);
  }
}

void CTimeSettingsDlg::gotoMinutes(int index) {
  const int editBoxId[] = {
   IDC_EDIT_TIMEOUTMINUTES1
  ,IDC_EDIT_TIMEOUTMINUTES2
  ,IDC_EDIT_TIMEOUTMINUTES3
  ,IDC_EDIT_TIMEOUTMINUTES4
  ,IDC_EDIT_TIMEOUTMINUTES5
  ,IDC_EDIT_TIMEOUTMINUTES6
  };
  if(0 <= index || index < ARRAYSIZE(editBoxId)) {
    gotoEditBox(this, editBoxId[index]);
  }
}

void CTimeSettingsDlg::OnGotoPlayer() {
  if(getCurrentTab() != TAB_LEVELS) {
    CComboBox *cb = getPlayerCombo(getCurrentTab());
    cb->SetFocus();
  }
}

CComboBox *CTimeSettingsDlg::getPlayerCombo(ClockTab tab) {
  switch(tab) {
  case TAB_TIMEDGAME : return (CComboBox*)GetDlgItem(IDC_COMBO_PLAYER);
  case TAB_SPEEDCHESS: return (CComboBox*)GetDlgItem(IDC_COMBO_SPEEDPLAYER);
  default            : throwInvalidArgumentException(__TFUNCTION__, _T("tab=%d"), tab);
  }
  return NULL;
}

void CTimeSettingsDlg::OnGotoMoveCount() {
  gotoEditBox(this, IDC_EDIT_MOVECOUNT);
}

void CTimeSettingsDlg::OnGotoTime() {
  gotoEditBox(this, (getCurrentTab() == TAB_TIMEDGAME) ? IDC_EDIT_HOURSLEFT : IDC_EDIT_SPEEDCHESSHOURSLEFT);
}

void CTimeSettingsDlg::OnGotoTimeIncrement() {
  gotoEditBox(this, (getCurrentTab() == TAB_TIMEDGAME) ? IDC_EDIT_SECONDSINCR : IDC_EDIT_SPEEDCHESSSECONDSINCR);
}

bool CTimeSettingsDlg::windowToParam() {
  bool ok = windowToParam(m_tm[0], false);
  if(ok) {
    ok = windowToParam(m_tm[1], true);
  }
  if(ok) {
    ok = windowToParam(m_lt);
  }
  return ok;
}

/*
void CTimeSettingsDlg::paramToWindow(const TimeParametersWithComboSelection &tm, bool speedChess) {

}
*/

bool CTimeSettingsDlg::windowToParam(TimeParametersWithComboSelection &tm, bool speedChess) {
  bool ok;
  switch(tm.m_selectedComboItem) {
  case SELECTED_BOTH :
    ok = windowToParam(tm.m_tm[WHITEPLAYER], speedChess); 
    if(ok) {
      windowToParam(tm.m_tm[BLACKPLAYER], speedChess); 
    }
    break;
  case SELECTED_WHITE:
    ok = windowToParam(tm.m_tm[WHITEPLAYER], speedChess); 
    break;
  case SELECTED_BLACK:
    ok = windowToParam(tm.m_tm[BLACKPLAYER], speedChess); 
    break;
  }
  return ok;
}

void CTimeSettingsDlg::paramToWindow(const TimeParameters &tm, bool speedChess) {
  const int secondsLeft  = tm.getSecondsLeft();
  if(speedChess) {
    m_speedChessHoursLeft   = GET_HOURS(  secondsLeft);
    m_speedChessMinutesLeft = GET_MINUTES(secondsLeft);
    m_speedChessSecondsLeft = GET_SECONDS(secondsLeft);
    m_speedChessSecondsIncr = tm.getSecondsIncr();
  } else {
    m_movesLeft             = tm.getMovesLeft();
    m_hoursLeft             = GET_HOURS(  secondsLeft);
    m_minutesLeft           = GET_MINUTES(secondsLeft);
    m_secondsLeft           = GET_SECONDS(secondsLeft);
    m_secondsIncr           = tm.getSecondsIncr();
  }
}

bool CTimeSettingsDlg::windowToParam(TimeParameters &tm, bool speedChess) {
  if(speedChess) {
    const int seconds = HHMMSS_TO_SECONDS(m_speedChessHoursLeft, m_speedChessMinutesLeft, m_speedChessSecondsLeft);
    if(seconds <= 0) {
      showSpeedChessTab();
      OnGotoTime();
      MessageBox(loadString(IDS_MSG_0_SECONDS_NOT_ALLOWED).cstr(), loadString(IDS_ERRORLABEL).cstr(), MB_OK | MB_ICONWARNING);
      return false;
    }
    tm = TimeParameters(-1, seconds, m_speedChessSecondsIncr);
  } else {
    if(m_movesLeft <= 0) {
      showTimedGameTab();
      OnGotoMoveCount();
      MessageBox(loadString(IDS_MSG_0_MOVES_NOT_ALLOWED).cstr(), loadString(IDS_ERRORLABEL).cstr(), MB_OK | MB_ICONWARNING);
      return false;
    }
    const int seconds = HHMMSS_TO_SECONDS(m_hoursLeft, m_minutesLeft, m_secondsLeft);
    if(seconds <= 0) {
      showTimedGameTab();
      OnGotoTime();
      MessageBox(loadString(IDS_MSG_0_SECONDS_NOT_ALLOWED).cstr(), loadString(IDS_ERRORLABEL).cstr(), MB_OK | MB_ICONWARNING);
      return false;
    }
    tm = TimeParameters(m_movesLeft, seconds, m_secondsIncr);
  }
  return true;
}

void CTimeSettingsDlg::paramToWindow(const LevelTimeout &lt) {
  m_min1 = (int)lt.getTimeout(1)/60;
  m_min2 = (int)lt.getTimeout(2)/60;
  m_min3 = (int)lt.getTimeout(3)/60;
  m_min4 = (int)lt.getTimeout(4)/60;
  m_min5 = (int)lt.getTimeout(5)/60;
  m_min6 = (int)lt.getTimeout(6)/60;
  m_sec1 = (float)fmod(lt.getTimeout(1),60);
  m_sec2 = (float)fmod(lt.getTimeout(2),60);
  m_sec3 = (float)fmod(lt.getTimeout(3),60);
  m_sec4 = (float)fmod(lt.getTimeout(4),60);
  m_sec5 = (float)fmod(lt.getTimeout(5),60);
  m_sec6 = (float)fmod(lt.getTimeout(6),60);
}

bool CTimeSettingsDlg::windowToParam(LevelTimeout &lt) {
  double timeout[LEVELCOUNT];
  timeout[0] = MMSS_TO_SECONDS(m_min1, m_sec1);
  timeout[1] = MMSS_TO_SECONDS(m_min2, m_sec2);
  timeout[2] = MMSS_TO_SECONDS(m_min3, m_sec3);
  timeout[3] = MMSS_TO_SECONDS(m_min4, m_sec4);
  timeout[4] = MMSS_TO_SECONDS(m_min5, m_sec5);
  timeout[5] = MMSS_TO_SECONDS(m_min6, m_sec6);

/*
  for(int i = 0; i < LEVELCOUNT; i++) {
    if(timeout[i] <= 0) {
      showLevelsTab();
      gotoSeconds(i);
      MessageBox(loadString(IDS_MSG_0_SECONDS_NOT_ALLOWED).cstr(), loadString(IDS_ERRORLABEL).cstr(), MB_OK | MB_ICONWARNING);
      return false;
    }
  }
*/
  for(int i = 0; i < LEVELCOUNT; i++) {
    lt.setTimeout(i+1, timeout[i]);
  }
  return true;
}

void CTimeSettingsDlg::OnSelChangeComboPlayer() {
  if(!UpdateData()) {
    return;
  }
  const ClockTab tab = getCurrentTab();
  switch(tab) {
  case TAB_LEVELS     : return;
  case TAB_TIMEDGAME  : selChangePlayerCombo(tab, m_tm[0]); break;
  case TAB_SPEEDCHESS : selChangePlayerCombo(tab, m_tm[1]); break;
  }
}

void CTimeSettingsDlg::selChangePlayerCombo(ClockTab tab, TimeParametersWithComboSelection &tm) {
  const SelectedComboItem oldSelection = tm.m_selectedComboItem;
  CComboBox *cb = getPlayerCombo(tab);
  if(!windowToParam()) {
    tm.m_selectedComboItem = oldSelection;
    cb->SetCurSel(oldSelection);
    return;
  }
  tm.m_selectedComboItem = (SelectedComboItem)(getPlayerCombo(tab)->GetCurSel());

  switch(tm.m_selectedComboItem) {
  case SELECTED_BOTH :
    if(tm.m_tm[WHITEPLAYER] != tm.m_tm[BLACKPLAYER]) {
      if(MessageBox(loadString(IDS_TIMEPARAMETERSDIFFER).cstr(), loadString(IDS_WARNING).cstr(), MB_YESNO | MB_ICONQUESTION) == IDNO) {
        tm.m_selectedComboItem = oldSelection;
        cb->SetCurSel(oldSelection);
        return;
      }
    }
    tm.m_tm[BLACKPLAYER] = tm.m_tm[WHITEPLAYER];
    // continue case
  case SELECTED_WHITE:
    paramToWindow(tm.m_tm[WHITEPLAYER], tab==TAB_SPEEDCHESS);
    UpdateData(false);
    break;
  case SELECTED_BLACK:
    paramToWindow(tm.m_tm[BLACKPLAYER], tab==TAB_SPEEDCHESS);
    UpdateData(false);
    break;
  }
}

BOOL CTimeSettingsDlg::PreTranslateMessage(MSG *pMsg) {
  BOOL result = FALSE;

  try {
    const ClockTab oldTab  = getCurrentTab();
    const int      oldCtrl = getFocusCtrlId(this);

    if(TranslateAccelerator(m_hWnd, m_accelTable, pMsg)) {
      result = TRUE;
    }

    if(!result) {
      result = CDialog::PreTranslateMessage(pMsg);
    }

    const ClockTab newTab = getCurrentTab();
    if(newTab != oldTab) {
      if(oldTab >= 0) {
        setCurrentControl(oldTab, oldCtrl);
      }
      gotoControl(m_currentControl[newTab]);
    } else if(newTab >= 0) {
      setCurrentControl(newTab, getFocusCtrlId(this));
    }
  } catch(Exception e) {
    MessageBox(e.what(), _T("Error"));
  }
  return result;
}

void CTimeSettingsDlg::gotoControl(int id) {
  CWnd *ctrl = GetDlgItem(id);
  if(ctrl) ctrl->SetFocus();
}

void CTimeSettingsDlg::setCurrentControl(ClockTab tab, int id) {
  if(GetDlgItem(id)) {
    m_currentControl[tab] = id;
  }
}

void CTimeSettingsDlg::OnSetfocusComboplayer() {
  setCurrentControl(getCurrentTab(), IDC_COMBO_PLAYER);
//  PostMessage(ID_MSG_SHOW_DROPDOWN);
}

void CTimeSettingsDlg::OnSetfocusSpeedcomboplayer() {
  setCurrentControl(getCurrentTab(), IDC_COMBO_SPEEDPLAYER);
//  PostMessage(ID_MSG_SHOW_DROPDOWN);
}

long CTimeSettingsDlg::OnMsgShowDropdown(WPARAM wp, LPARAM lp) {
  int id;
  switch(getCurrentTab()) {
  case TAB_TIMEDGAME  : id = IDC_COMBO_PLAYER     ; break;
  case TAB_SPEEDCHESS : id = IDC_COMBO_SPEEDPLAYER; break;
  default             : return 0;
  }

  CComboBox *cb = (CComboBox*)GetDlgItem(id);
  cb->UpdateWindow();
  cb->ShowDropDown();
  return 0;
}
