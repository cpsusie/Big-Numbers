#pragma once

#include <MFCUtil/LayoutManager.h>

class CShowColorMapDlg : public CDialog {
private:
    SimpleLayoutManager m_layoutManager;
    const UINT          m_maxIteration; // m_colorMap has length m_maxIteration+1
    const D3DCOLOR     *m_colorMap;     // last element is always BLACK

public:
    CShowColorMapDlg(UINT maxIteration, const D3DCOLOR *colorMap, CWnd* pParent = NULL);

    enum { IDD = IDD_SHOWCOLORMAP_DIALOG };

protected:
    virtual void DoDataExchange(CDataExchange* pDX);

protected:

	virtual BOOL OnInitDialog();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnPaint();
    DECLARE_MESSAGE_MAP()
};

