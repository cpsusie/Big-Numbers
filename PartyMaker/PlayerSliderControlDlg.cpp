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
, m_tr(createTransformation(controlType)) {
}

void CPlayerSliderControlDlg::DoDataExchange(CDataExchange *pDX) {
    CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CPlayerSliderControlDlg, CDialog)
    ON_WM_HSCROLL()
    ON_BN_CLICKED(IDC_DEFAULTSETTINGBUTTON, OnDefaultSettingButton)
END_MESSAGE_MAP()

BOOL CPlayerSliderControlDlg::OnInitDialog() {
  CDialog::OnInitDialog();

  initSliderRange();
  SetWindowText((m_controlType==CONTROL_BALANCE)?_T("Balance"):_T("Hastighed"));
  m_initialValue = getPlayerValue();
  setSliderPos(m_initialValue);

  return TRUE;
}

void CPlayerSliderControlDlg::OnCancel() {
  setPlayerValue(m_initialValue);
  CDialog::OnCancel();
}

void CPlayerSliderControlDlg::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar *pScrollBar) {
  setPlayerValue(getSliderPos());
  showPlayerValue();
  CDialog::OnHScroll(nSBCode, nPos, pScrollBar);
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

void CPlayerSliderControlDlg::initSliderRange() {
  const DoubleInterval &range = m_tr.getFromInterval();
  getSlider()->SetRange((int)range.getFrom(), (int)range.getTo(), TRUE);
}

double CPlayerSliderControlDlg::getSliderPos() {
  return m_tr.forwardTransform(getSlider()->GetPos());
}

void CPlayerSliderControlDlg::setSliderPos(double value) {
  getSlider()->SetPos((int)m_tr.backwardTransform(value));
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

  GetDlgItem(IDC_STATICTEXT)->SetWindowText(format(_T("%d%%  %s -> %s"),(int)(factor*100.0),formatSeconds(origlen).cstr(),formatSeconds(corriglen).cstr()).cstr());
}

void CPlayerSliderControlDlg::showBalance() {
  const double factor    = getPlayerValue();
  GetDlgItem(IDC_STATICTEXT)->SetWindowText(format(_T("%+d%%"), (int)factor).cstr());
}

CSliderCtrl *CPlayerSliderControlDlg::getSlider() {
  return (CSliderCtrl*)GetDlgItem(IDC_PLAYERCONTROLSLIDER);
}

LinearTransformation CPlayerSliderControlDlg::createTransformation(PlayerControlType type) { // static
  switch(type) {
  case CONTROL_BALANCE: return LinearTransformation(DoubleInterval(-100,100), DoubleInterval(-100,100));
  case CONTROL_SPEED  : return LinearTransformation(DoubleInterval(-200,200), DoubleInterval(0.5 ,2  ));
  default             : return LinearTransformation(DoubleInterval(-100,100), DoubleInterval(0 ,  1  ));
  }
}
