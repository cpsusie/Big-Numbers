#pragma once

class ColorMapData {
public:
  UINT m_maxIteration;
  UINT m_seed;
  BOOL m_randomSeed;

  ColorMapData() : m_maxIteration(2000), m_randomSeed(TRUE), m_seed(127) {
  }
  bool operator==(const ColorMapData &d) const {
    return (m_maxIteration == d.m_maxIteration)
        && (m_randomSeed   == d.m_randomSeed  )
        && (m_randomSeed || (m_seed == d.m_seed));
  }
  bool operator!=(const ColorMapData &d) const {
    return !(*this == d);
  }
};

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
  UINT  m_maxIteration;
	UINT	m_seed;
	BOOL	m_randomSeed;

public:
    virtual BOOL PreTranslateMessage(MSG *pMsg);
protected:
    virtual void DoDataExchange(CDataExchange *pDX);

protected:
    virtual BOOL OnInitDialog();
    virtual void OnOK();
    afx_msg void OnGotoMaxIteration();
    afx_msg void OnGotoSeed();
	  afx_msg void OnCheckRandomSeed();
    DECLARE_MESSAGE_MAP()
};
