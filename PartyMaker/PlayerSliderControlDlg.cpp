#include "stdafx.h"
#include "PartyMaker.h"
#include "PartyMakerDlg.h"
#include "PlayerSliderControlDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

CPlayerSliderControlDlg::CPlayerSliderControlDlg(CWMPPlayer4 &player, PlayerControlType controlType, CWnd *pParent)
: CDialog(CPlayerSliderControlDlg::IDD, pParent)
, m_player(player)
, m_controlType(controlType)
{
}

void CPlayerSliderControlDlg::DoDataExchange(CDataExchange *pDX) {
    __super::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CPlayerSliderControlDlg, CDialog)
    ON_WM_HSCROLL()
    ON_BN_CLICKED(IDC_DEFAULTSETTINGBUTTON, OnDefaultSettingButton)
END_MESSAGE_MAP()

BOOL CPlayerSliderControlDlg::OnInitDialog() {
  __super::OnInitDialog();

  switch(m_controlType) {
  case CONTROL_BALANCE:
    m_sliderCtrl.substituteControl(this, IDC_PLAYERCONTROLSLIDER, DoubleInterval(-100,100));
    setWindowText(this, _T("Balance"));
    break;
  case CONTROL_SPEED  :
    m_sliderCtrl.substituteControl(this, IDC_PLAYERCONTROLSLIDER, DoubleInterval(0.5 ,2  ));
    setWindowText(this, _T("Hastighed"));
    break;
  default             :
    throwException(_T("%s:Invalid controllType:%d"), m_controlType);
  }
  m_initialValue = getPlayerValue();
  setSliderPos(m_initialValue);
  return TRUE;
}

void CPlayerSliderControlDlg::OnCancel() {
  setPlayerValue(m_initialValue);
  __super::OnCancel();
}

void CPlayerSliderControlDlg::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar *pScrollBar) {
  setPlayerValue(m_sliderCtrl.getPos());
  showPlayerValue();
  __super::OnHScroll(nSBCode, nPos, pScrollBar);
}

void CPlayerSliderControlDlg::OnDefaultSettingButton() {
  switch(m_controlType) {
  case CONTROL_BALANCE:
    setPlayerValue(0);
    break;
  case CONTROL_SPEED  :
    setPlayerValue(1);
    break;
  }
}

double CPlayerSliderControlDlg::getPlayerValue() {
  switch(m_controlType) {
  case CONTROL_BALANCE: return m_player.GetSettings().GetBalance();
  case CONTROL_SPEED  : return m_player.GetSettings().GetRate();
  default             : return 0;
  }
}

void CPlayerSliderControlDlg::setPlayerValue(double value) {
  switch(m_controlType) {
  case CONTROL_BALANCE:
    m_player.GetSettings().SetBalance((int)value);
    break;
  case CONTROL_SPEED  :
    m_player.GetSettings().SetRate(value);
    break;
  default             :;
  }
  setSliderPos(value);
}

double CPlayerSliderControlDlg::getSliderPos() {
  return m_sliderCtrl.getPos();
}

void CPlayerSliderControlDlg::setSliderPos(double value) {
  m_sliderCtrl.setPos(value);
  showPlayerValue();
}

void CPlayerSliderControlDlg::showPlayerValue() {
  if(m_controlType==CONTROL_BALANCE) {
    showBalance();
  } else {
    showSpeed();
  }
}

static String formatSeconds(double seconds) {
  int isec = (int)seconds;
  if(isec >= 3600) {
    return format(_T("%d:%02d:%02d"),isec / 3600, (isec % 3600) / 60,isec % 60);
  } else {
    return format(_T("%2d:%02d"),isec / 60,isec % 60);
  }
}

void CPlayerSliderControlDlg::showSpeed() {
  const double factor    = getPlayerValue();
  const double origlen   = m_player.GetCurrentMedia().GetDuration();
  const double corriglen = origlen / factor;
  setWindowText(this, IDC_STATICTEXT, format(_T("%.1lf%%  %s -> %s"),factor*100.0, formatSeconds(origlen).cstr(),formatSeconds(corriglen).cstr()).cstr());
}

void CPlayerSliderControlDlg::showBalance() {
  const double factor    = getPlayerValue();
  setWindowText(this, IDC_STATICTEXT, format(_T("%+d%%"), (int)factor).cstr());
}
