#pragma once

#include "MakeGifDoc.h"

class CSettingsDlg : public CDialog {
private:
  CComboBox *getColorCountCombo();
  int getColorCount();
  void setColorCount(int colorCount);
  UINT m_colorCount;
public:
  CSettingsDlg(const ImageSettings &settings, CWnd *pParent = NULL);

  enum { IDD = IDD_SETTINGS_DIALOG };
  UINT    m_scaleFactor;

  ImageSettings getImageSettings() const;

protected:
  virtual void DoDataExchange(CDataExchange *pDX);
  virtual void OnOK();
  virtual BOOL OnInitDialog();
  DECLARE_MESSAGE_MAP()
};

