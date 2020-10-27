#pragma once

#include "ColorMap.h"

class CShowColorMapDlg : public CDialog {
private:
    SimpleLayoutManager m_layoutManager;
    const ColorMap     &m_colorMap;
public:
    CShowColorMapDlg(const ColorMap &colorMap, CWnd *pParent = nullptr);

    enum { IDD = IDD_SHOWCOLORMAP_DIALOG };

    virtual void DoDataExchange(CDataExchange *pDX);
    virtual BOOL OnInitDialog();
    afx_msg void OnSize(UINT nType, int cx, int cy);
    afx_msg void OnPaint();
    DECLARE_MESSAGE_MAP()
};

