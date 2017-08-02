#pragma once
#include "colormap.h"
#include <Hashmap.h>
#include "SliderCtrlWithTransformation.h"
#include "ColorSpace.h"
#include "PropertyDialog.h"

template<class T> class CColormapDialog : public CPropertyDialog<T>  {
private:
  CompactArray<CSliderCtrlWithTransformation*> m_sliderArray;
public:
  CColormapDialog(int resId, int propertyId, CWnd *pParent) : CPropertyDialog<T>(resId, propertyId, pParent) {
  }
  ~CColormapDialog() {
    for (size_t i = 0; i < m_sliderArray.size(); i++) {
      CSliderCtrlWithTransformation *ctrl = m_sliderArray[i];
      SAFEDELETE(ctrl);
    }
    m_sliderArray.clear();
  }
protected:
  void initSlider(int ctrlId, double from, double to, UINT stepCount = 100, IntervalScale type=LINEAR) {
    CSliderCtrlWithTransformation *ctrl = new CSliderCtrlWithTransformation(); TRACE_NEW(ctrl);
    m_sliderArray.add(ctrl);
    try {
      ctrl->substituteControl(this, ctrlId, DoubleInterval(from,to), stepCount, type);
    } catch (Exception e) {
      Message(_T("%s"), e.what());
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
    ((CColormap*)GetDlgItem(ctrlId))->SetColor(D3DCOLORVALUE2COLORREF(c)); // TODO
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

