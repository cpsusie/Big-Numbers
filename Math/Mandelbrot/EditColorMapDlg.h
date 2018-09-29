#pragma once

#include "colormapctrl.h"
#include "ColorMap.h"

typedef enum {
  IDLE
 ,MOVING_RECT
} EditState;

class CEditColorMapDlgThread;

class CEditColorMapDlg : public CDialog {
private:
    CEditColorMapDlgThread &m_thread;
    CompactArray<CRect>     m_interpolationRect;
    EditState               m_state;
    int                     m_selectedRectIndex;
    CPoint                  m_pointInRect; // relative to rect.center
    float                   m_pixelWidth;
    CColormapctrl           m_colormap;

    const ColorMapData &getColorMapData() const;
    void setColorMapData(const ColorMapData &cd);
    CStatic *getColorMapWindow() const {
      return (CStatic*)GetDlgItem(IDC_STATIC_COLORMAPWINDOW);
    }
    CRect    getColorMapWinRect() const {
      return getClientRect(this, IDC_STATIC_COLORMAPWINDOW);
    }
    CSize    getColorMapWinSize() const {
      return getColorMapWinRect().Size();
    }
    UINT     getMapSize() const {
      return getColorMapWinSize().cx;
    }
    // Return index of interpolationRect containing p, -1 of none found
    int findInterpolationRect(const CPoint &p);
    void setSelectedInterpolationPoint(int index);
    void setState(EditState state);
    inline EditState getState() const {
      return m_state;
    }
    void reposition();
public:
    CEditColorMapDlg(CEditColorMapDlgThread *thread);

    enum { IDD = IDD_EDITCOLORMAP_DIALOG };

    virtual void    DoDataExchange(CDataExchange *pDX);
    virtual BOOL    OnInitDialog();
    afx_msg void    OnPaint();
    afx_msg void    OnContextMenu(  CWnd *pWnd , CPoint point);
    afx_msg void    OnLButtonDown(  UINT nFlags, CPoint point);
    afx_msg void    OnLButtonUp(    UINT nFlags, CPoint point);
    afx_msg void    OnRButtonDown(  UINT nFlags, CPoint point);
    afx_msg void    OnRButtonUp(    UINT nFlags, CPoint point);
    afx_msg void    OnMouseMove(    UINT nFlags, CPoint point);
    afx_msg void    OnDeleteInterpolationColor();
    afx_msg void    OnColorchangedColormapctrl();
    afx_msg void    OnBnClickedSetToDefault();
    DECLARE_MESSAGE_MAP()
    DECLARE_EVENTSINK_MAP()
};

