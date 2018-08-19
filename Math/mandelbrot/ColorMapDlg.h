#pragma once

#include "ColorMap.h"

class CColorMapDlg : public CDialog {
private:
  HACCEL       m_accelTable;
  ColorMapData m_colorMapData;

  void enableFields();
public:
  CColorMapDlg(const ColorMapData &colorMapData, CWnd *pParent = NULL);
  const ColorMapData &getColorMapData() const {
    return m_colorMapData;
  }

private:
	enum { IDD = IDD_COLORMAP_DIALOG };
  UINT  m_maxCount;
	UINT	m_seed;
	BOOL	m_randomSeed;

public:
    virtual BOOL PreTranslateMessage(MSG *pMsg);
protected:
    virtual void DoDataExchange(CDataExchange *pDX);

protected:
    virtual BOOL OnInitDialog();
    virtual void OnOK();
    afx_msg void OnGotoMaxCount();
    afx_msg void OnGotoSeed();
	  afx_msg void OnCheckRandomSeed();
    DECLARE_MESSAGE_MAP()
};
