#pragma once
#include "colormap.h"
#include <CompactHashmap.h>
#include "SliderCtrlWithTransformation.h"
#include "ColorSpace.h"
#include "PropertyDialog.h"

template<typename T> class CColormapDialog : public CPropertyDialog<T>  {
private:
  CompactIntHashMap<CSliderCtrlWithTransformation*> m_sliderMap;
public:
  CColormapDialog(int resId, int propertyId, CWnd *pParent) : CPropertyDialog<T>(resId, propertyId, pParent) {
  }
  ~CColormapDialog() {
    for(Iterator<Entry<CompactIntKeyType, CSliderCtrlWithTransformation*> > it = m_sliderMap.getIterator(); it.hasNext();) {
      CSliderCtrlWithTransformation *ctrl = it.next().getValue();
      SAFEDELETE(ctrl);
    }
    m_sliderMap.clear();
  }
protected:
  BOOL PreTranslateMessage(MSG *pMsg) {
    if(pMsg->message == WM_MOUSEWHEEL) {
      short zDelta = (short)(pMsg->wParam >> 16);
      zDelta = -zDelta;
      pMsg->wParam &= ~WPARAM(0xffff0000);
      pMsg->wParam |= zDelta << 16;
    }
    return __super::PreTranslateMessage(pMsg);
  }

  void initSlider(int ctrlId, double from, double to, UINT stepCount = 100, IntervalScale type=LINEAR) {
    CSliderCtrlWithTransformation *ctrl = NULL;
    try {
      ctrl = new CSliderCtrlWithTransformation(); TRACE_NEW(ctrl);
      if(!m_sliderMap.put(ctrlId,ctrl)) {
        throwInvalidArgumentException(__TFUNCTION__, _T("ctrlID=%d already exist"), ctrlId);
      }
      ctrl->substituteControl(this, ctrlId, DoubleInterval(from,to), stepCount, type);
    } catch(Exception e) {
      SAFEDELETE(ctrl);
      showException(e);
    }
  }

  float getSliderValue(int ctrlId) const {
    return (float)((CSliderCtrlWithTransformation*)GetDlgItem(ctrlId))->getPos();
  }

  void setSliderValue(int ctrlId, double value) {
    ((CSliderCtrlWithTransformation*)GetDlgItem(ctrlId))->setPos(value);
  }

  D3DCOLORVALUE getD3DCOLORVALUE(int ctrlId) {
    return COLORREF2COLORVALUE(((CColormap*)GetDlgItem(ctrlId))->GetColor());
  }
  void setD3DCOLORVALUE(int ctrlId, D3DCOLORVALUE c) {
    ((CColormap*)GetDlgItem(ctrlId))->SetColor(D3DCOLORVALUE2COLORREF(c));
  }

  D3DCOLOR getD3DCOLOR(int ctrlId) {
    const COLORREF cr = ((CColormap*)GetDlgItem(ctrlId))->GetColor();
    return COLORREF2D3DCOLOR(cr);
  }
  void setD3DCOLOR(int ctrlId, COLORREF cr) {
    ((CColormap*)GetDlgItem(ctrlId))->SetColor(D3DCOLOR2COLORREF(cr));
  }

  inline void putWindowBesideParent() {
    if(GetParent()) {
      putWindowBesideWindow(this, GetParent());
    }
  }
};

