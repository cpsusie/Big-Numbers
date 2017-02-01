#pragma once

#include <Math/Transformation.h>

typedef enum {
  CONTROL_BALANCE
 ,CONTROL_SPEED
} PlayerControlType;

class CPlayerSliderControlDlg : public CDialog {
private:
  CWMPPlayer4         &m_player;
  PlayerControlType    m_controlType;
  LinearTransformation m_tr;
  double               m_initialValue;

  static LinearTransformation createTransformation(PlayerControlType type);
  CSliderCtrl *getSlider();
  double getPlayerValue();
  void   setPlayerValue(double value);
  double getSliderPos();
  void   setSliderPos(double value);
  void   initSliderRange();
  void   showPlayerValue();
  void   showSpeed();
  void   showBalance();
public:
  CPlayerSliderControlDlg(CWMPPlayer4 &player, PlayerControlType controlType, CWnd *pParent = NULL);

  enum { IDD = IDD_SLIDERCONTROLDIALOG };

protected:
  virtual void DoDataExchange(CDataExchange *pDX);

protected:
  afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar *pScrollBar);
  virtual BOOL OnInitDialog();
    virtual void OnCancel();
    afx_msg void OnDefaultSettingButton();
  DECLARE_MESSAGE_MAP()
};

