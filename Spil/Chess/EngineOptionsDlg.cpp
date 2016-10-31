#include "stdafx.h"

#ifndef TABLEBASE_BUILDER

#include <MFCUtil/WinTools.h>
#include "ChessGraphics.h"
#include "EngineOptionsDlgThread.h"
#include "EngineOptionsDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

CEngineOptionsDlg::CEngineOptionsDlg(CEngineOptionsDlgThread &thread)
: m_player(thread.getPlayer())
, m_engineName(thread.getEngine().getName())
, m_optionArray(thread.getEngine().getOptionDescriptionArray())
, m_thread(&thread)
, m_engine(&thread.getEngine())
, CDialog(CEngineOptionsDlg::IDD, NULL)
{
  commonInit();
}

CEngineOptionsDlg::CEngineOptionsDlg(Player player)
: m_player(player)
, m_engineName(Options::getOptions().getPlayerOptions(player).m_engineName)
, m_optionArray(createEngineOptionDescriptionArray(player))
, m_thread(NULL)
, m_engine(NULL)
, CDialog(CEngineOptionsDlg::IDD, NULL)
{
  commonInit();
}

EngineOptionDescriptionArray CEngineOptionsDlg::createEngineOptionDescriptionArray(Player player) { // static
  try {
    ExternEngine engine(Options::getEnginePathByPlayer(player));
    engine.start();
    EngineOptionDescriptionArray result = engine.getOptionDescriptionArray();
    return result; //.removeOptionsByType(OptionTypeButton);
  } catch(Exception e) {
    AfxMessageBox(e.what(), MB_ICONWARNING);
    return EngineOptionDescriptionArray();
  }
}

void CEngineOptionsDlg::commonInit() {
  m_buttonCounter = m_editCounter = m_spinCounter = m_comboCounter = 0;

}

void CEngineOptionsDlg::DoDataExchange(CDataExchange* pDX) {
    CDialog::DoDataExchange(pDX);
}

#define FIRST_USER_CTRLID   WM_USER

#define FIRST_BUTTON_CTRLID  FIRST_USER_CTRLID
#define LAST_BUTTON_CTRLID  (FIRST_BUTTON_CTRLID + 99)
#define FIRST_EDIT_CTRLID   (LAST_BUTTON_CTRLID  + 1 )
#define LAST_EDIT_CTRLID    (FIRST_EDIT_CTRLID   + 99)
#define FIRST_COMBO_CTRLID  (LAST_EDIT_CTRLID    + 1 )
#define LAST_COMBO_CTRLID   (FIRST_COMBO_CTRLID  + 99)
#define FIRST_SPIN_CTRLID   (LAST_COMBO_CTRLID   + 1 )
#define LAST_SPIN_CTRLID    (FIRST_SPIN_CTRLID   + 99)

#define LAST_USER_CTRLID    LAST_SPIN_CTRLID

int CEngineOptionsDlg::nextCtrlId(EngineOptionType type) {
  switch(type) {
  case OptionTypeCheckbox:
  case OptionTypeButton  :
    return FIRST_BUTTON_CTRLID + m_buttonCounter++;

  case OptionTypeSpin    :
  case OptionTypeString  :
    return FIRST_EDIT_CTRLID + m_editCounter++;

  case OptionTypeCombo   :
    return FIRST_COMBO_CTRLID + m_comboCounter++;

  default:
    throwInvalidArgumentException(__TFUNCTION__, _T("type=%d"), type);
    return 0;
  }
}

int CEngineOptionsDlg::nextSpinCtrlId() {
  return FIRST_SPIN_CTRLID + m_spinCounter++;
}

BEGIN_MESSAGE_MAP(CEngineOptionsDlg, CDialog)
	ON_WM_CREATE()
	ON_WM_SHOWWINDOW()
	ON_WM_CLOSE()
	ON_BN_CLICKED(IDC_BUTTON_SENDTOENGINE                                   , OnButtonSendToEngine)
	ON_BN_CLICKED(IDC_BUTTON_SETDEFAULT                                     , OnButtonSetDefault  )
    ON_CONTROL_RANGE(  BN_CLICKED  , FIRST_BUTTON_CTRLID, LAST_BUTTON_CTRLID, OnUserButton        )
	ON_NOTIFY_RANGE(   UDN_DELTAPOS, FIRST_SPIN_CTRLID  , LAST_SPIN_CTRLID  , OnDeltaPosSpin      )
    ON_NOTIFY_EX_RANGE(TTN_NEEDTEXT, 0                  , 0xffff            , OnToolTipNotify     )
END_MESSAGE_MAP()

BOOL CEngineOptionsDlg::PreTranslateMessage(MSG *pMsg) {
  switch(pMsg->message) {
  case WM_KEYDOWN:
    { const int ctrlId = getFocusCtrlId(this);
      if(ctrlId < 0) break;
      const EngineOptionControl *ctrl = m_controls.findControlById(ctrlId);
      if((ctrl == NULL) || (ctrl->getType() != OptionTypeSpin)) break;
      const unsigned char asciiChar = toAscii((UINT)pMsg->wParam);
      if(!isprint(asciiChar)) break;
      if(isdigit(asciiChar) || ((asciiChar == '-') && (ctrl->getMin() < 0))) break;
      return TRUE; // ie do NOT process this message
    }
    break;
  }
  return CDialog::PreTranslateMessage(pMsg);
}

int CEngineOptionsDlg::OnCreate(LPCREATESTRUCT lpCreateStruct) {
  if (CDialog::OnCreate(lpCreateStruct) == -1) {
     return -1;
  }
  for(size_t i = 0; i < m_optionArray.size(); i++) {
    addControl(m_optionArray[i]);
  }
  addControl(EngineOptionDescription::debugOption);
  return 0;
}

#define LEFT_MARGIN                   10
#define TOP_MARGIN                    10
#define RIGHT_MARGIN                  10
#define BOTTOM_MARGIN                 10
#define XSPACE_BETWEEN_FIELDS         20
#define YSPACE_BETWEEN_FIELDS          6
#define SPACE_ABOVE_BUTTONS           20
#define SPACE_BETWEEN_BUTTONS         15

BOOL CEngineOptionsDlg::OnInitDialog() {
  CDialog::OnInitDialog();

  setControlText(IDD, this);

  SetWindowText(format(_T("Options for %s(%s)"), m_engineName.cstr(), getPlayerName(m_player).cstr()).cstr());

  const int ctrlHeight = getClientRect(GetDlgItem(IDOK)).Height();

  // first set size of erery engine defined control
  DialogColumnInfoArray columnInfo = m_controls.findColumnInfo(this, ctrlHeight, CSize(XSPACE_BETWEEN_FIELDS,YSPACE_BETWEEN_FIELDS));

  // then set position of controls that is NOT a pushbutton
  const CPoint maxPos = m_controls.createGrid(columnInfo, CPoint(LEFT_MARGIN, TOP_MARGIN));

  // then set position of all pushbuttons
  CompactIntArray buttonIdArray = m_controls.selectCtrlIdByType(OptionTypeButton);
  buttonIdArray.add(IDC_BUTTON_SETDEFAULT);
  if(m_engine == NULL) {
    GetDlgItem(IDC_BUTTON_SENDTOENGINE)->ShowWindow(SW_HIDE);
  } else {
    buttonIdArray.add(IDC_BUTTON_SENDTOENGINE);
  }
  buttonIdArray.add(IDOK    );
  buttonIdArray.add(IDCANCEL);
  CPoint buttonPos(LEFT_MARGIN, maxPos.y + SPACE_ABOVE_BUTTONS);

  for(size_t i = 0; i < buttonIdArray.size(); i++) {
    const int id = buttonIdArray[i];
    setWindowPosition(this, id, buttonPos);
    buttonPos.x += getWindowSize(this, id).cx + SPACE_BETWEEN_BUTTONS;
    if(buttonPos.x > maxPos.x + 70) {
      buttonPos.x = LEFT_MARGIN;
      buttonPos.y += SPACE_BETWEEN_BUTTONS;
    }
  }

  EnableToolTips();
  setClientRectSize(this, findBestClientRectSize());
  movePusbuttonsToCorner(buttonIdArray);
  centerWindow(this);

  enableUserButtons(m_engine != NULL);

  putDefaultToWindow();
  putValuesToWindow(Options::getEngineOptionValues(m_player, m_engineName));
  m_origValueArray = getValuesFromWindow();
  return TRUE;
}

CSize CEngineOptionsDlg::findBestClientRectSize() {
  int titleWidth = getTextExtent(CClientDC(this), getWindowText(this)).cx;
  CSize size(titleWidth + 60,0);
  for(CWnd *child = GetWindow(GW_CHILD); child; child = child->GetNextWindow()) {
    const CRect r = getWindowRect(child);
    size.cx = max(size.cx, r.right + RIGHT_MARGIN);
    size.cy = max(size.cy, r.bottom);
  }
  size.cy += BOTTOM_MARGIN;
  return size;
}

void CEngineOptionsDlg::movePusbuttonsToCorner(const CompactIntArray &buttonIdArray) {
  CPoint maxlr(0,0);
  for(size_t  i = 0; i < buttonIdArray.size(); i++) {
    CWnd *b = GetDlgItem(buttonIdArray[i]);
    if(!b) continue;
    const CRect r = getWindowRect(b);
    maxlr.x = max(maxlr.x, r.right);
    maxlr.y = max(maxlr.y, r.bottom);
  }
  const CSize clSize = getClientRect(this).Size();
  CPoint newlr(maxlr);
  newlr.x = max(newlr.x, clSize.cx - RIGHT_MARGIN );
  newlr.y = max(newlr.y, clSize.cy - BOTTOM_MARGIN);
  if(newlr != maxlr) {
    const int dx = newlr.x - maxlr.x;
    const int dy = newlr.y - maxlr.y;
    for(size_t i = 0; i < buttonIdArray.size(); i++) {
      CWnd *b = GetDlgItem(buttonIdArray[i]);
      if(!b) continue;
      const CRect r = getWindowRect(b);
      setWindowPosition(b, CPoint(r.left+dx, r.top+dy));
    }
  }
}


void CEngineOptionsDlg::addControl(const EngineOptionDescription &option) {
//  if((option.getType() == OptionTypeButton) && m_engine == NULL) { // do not add buttons if engine == NULL
//    return;
//  }
  m_controls.add(EngineOptionControl(this, option));
}

void CEngineOptionsDlg::putValuesToWindow(const EngineOptionValueArray &valueArray) {
  m_controls.putValues(valueArray);
}

void CEngineOptionsDlg::putDefaultToWindow() {
  putValuesToWindow(m_optionArray.createDefaultValueArray(m_engineName));
}

EngineOptionValueArray CEngineOptionsDlg::getValuesFromWindow() const {
  return m_controls.getValues(m_engineName);
}

void CEngineOptionsDlg::OnShowWindow(BOOL bShow, UINT nStatus) {
  CDialog::OnShowWindow(bShow, nStatus);
  if(m_thread && bShow) {
    m_thread->setDialogRunning(true);
  }
}

void CEngineOptionsDlg::OnOK() {
  if(!validate()) {
    return;
  }
  Options::saveEngineOptionValues(m_player, m_optionArray.pruneDefaults(getValuesFromWindow()));
  CDialog::OnOK();
}

void CEngineOptionsDlg::OnCancel() {
  if(isChanged()) {
    if(!confirmCancel(this)) {
      return;
    }
  }
  CDialog::OnCancel();
}

void CEngineOptionsDlg::OnClose() {
  if(m_thread) {
    m_thread->setDialogRunning(false);
  }
  CDialog::OnClose();
}

bool CEngineOptionsDlg::isChanged() const {
  const EngineOptionValueArray valueArray = getValuesFromWindow();
  return valueArray != m_origValueArray;
}

bool CEngineOptionsDlg::validate() {
  for(size_t i = 0; i < m_controls.size(); i++) {
    const EngineOptionControl &ctrl = m_controls[i];
    try {
      ctrl.validate();
    } catch(Exception e) {
      gotoEditBox(this, ctrl.m_ctrlId);
      MessageBox(e.what(), loadString(IDS_ERRORLABEL).cstr(), MB_OK | MB_ICONWARNING);
      return false;
    }
  }
  return true;
}

void CEngineOptionsDlg::enableUserButtons(bool enabled) {
  const CompactIntArray buttonIds = m_controls.selectCtrlIdByType(OptionTypeButton);
  for(size_t i = 0; i < buttonIds.size(); i++) {
    EngineOptionControl *ctrl = m_controls.findControlById(buttonIds[i]);
    if(ctrl) {
      ctrl->setEnable(enabled);
    }
  }
}

void CEngineOptionsDlg::OnButtonSendToEngine() {
  MessageBox(_T("send to engine"));
}

void CEngineOptionsDlg::OnButtonSetDefault() {
  putDefaultToWindow();
}

void CEngineOptionsDlg::OnUserButton(UINT id) { // do nothing if m_engine == NULL. userButtons should not be created in this cse
  if(m_engine == NULL) {
    return;
  }
  const EngineOptionControl *ctrl = m_controls.findControlById(id);
  if(ctrl) {
    switch(ctrl->getType()) {
    case OptionTypeCheckbox :
      m_engine->setParameterValue(ctrl->m_option, ctrl->getBoolValue());
      break;
    case OptionTypeButton   :
      m_engine->clickButton(ctrl->m_option);
      break;
    default:
      MessageBox( format(_T("Control %s not a button"), ctrl->m_option.getName()).cstr(),  _T("Error"), MB_ICONEXCLAMATION);
      break;
    }
  }
}

void CEngineOptionsDlg::OnDeltaPosSpin(UINT id, NMHDR *pNMHDR, LRESULT *pResult) {
  EngineOptionControl *ctrl = m_controls.findControlById(id);
  if(ctrl) {
    NM_UPDOWN *pNMUpDown = (NM_UPDOWN*)pNMHDR;
    UpdateData();
    const int oldValue = ctrl->getIntValue();
    const int newValue = minMax((int)oldValue - pNMUpDown->iDelta, ctrl->m_option.getMin(), ctrl->m_option.getMax());
    ctrl->setValue(newValue);
    *pResult = 0;
  }
}

BOOL CEngineOptionsDlg::OnToolTipNotify(UINT id, NMHDR *pNMHDR, LRESULT *pResult) {
  TOOLTIPTEXT *pTTT = (TOOLTIPTEXT*)pNMHDR; // Get the tooltip structure.
  UINT_PTR CtrlHandle = pNMHDR->idFrom; // Actually the idFrom holds Control's handle.

  // Check once again that the idFrom holds handle itself.
  if(pTTT->uFlags & TTF_IDISHWND) {
    UINT ctrlId = ::GetDlgCtrlID(HWND(CtrlHandle));
    const EngineOptionControl *ctrl = m_controls.findControlById(ctrlId);
    if((ctrl == NULL) || (ctrl->getType() == OptionTypeButton)) {
      return FALSE;
    }

#if (_MFC_VER < 0x0700)
    _AFX_THREAD_STATE* pThreadState = AfxGetThreadState();
#else
     AFX_MODULE_THREAD_STATE* pThreadState = AfxGetModuleThreadState();
#endif
    CToolTipCtrl* pToolTip = pThreadState->m_pToolTip;
    if(pToolTip) {
      pToolTip->SetMaxTipWidth(SHRT_MAX); // Do this to make \r\n work!
    }

    // Now you have the ID. depends on control, set your tooltip message.
    m_currentToolTip = format(_T("Default:%s"), ctrl->m_option.getDefaultAsString().cstr());
    pTTT->lpszText = m_currentToolTip.cstr();
    return TRUE;
  }
  return FALSE;
}

#endif // TABLEBASE_BUILDER
