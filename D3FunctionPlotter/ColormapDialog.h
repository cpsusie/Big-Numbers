#pragma once
#include "colormap.h"
#include <Hashmap.h>
#include <Math/Transformation.h>
#include "PropertyDialog.h"

#define D3DCOLORVALUE2COLORREF(c) RGB((DWORD)((c.r)*255.f),(DWORD)((c.g)*255.f),(DWORD)((c.b)*255.f))
D3DCOLORVALUE COLORREF2COLORVALUE(COLORREF c);

template<class T> class CColormapDialog : public CPropertyDialog<T>  {
private:
  IntHashMap<IntervalTransformation*> m_sliderTransformMap;

  IntervalTransformation *getTransformation(int ctrlId) {
    IntervalTransformation **tpp = m_sliderTransformMap.get(ctrlId);
    return tpp ? *tpp : NULL;
  }
  const IntervalTransformation *getTransformation(int ctrlId) const {
    IntervalTransformation * const *tpp = m_sliderTransformMap.get(ctrlId);
    return tpp ? *tpp : NULL;
  }
  void removeTransformation(int ctrlId) {
    IntervalTransformation *trans = getTransformation(ctrlId);
    if(trans != NULL) {
      delete trans;
      m_sliderTransformMap.remove(ctrlId);
    }
  }

  void addTransformation(int ctrlId, double from, double to, unsigned int stepCount, bool logarithmic) {
    removeTransformation(ctrlId);
    if(logarithmic) {
      m_sliderTransformMap.put(ctrlId, new LogarithmicTransformation(DoubleInterval(from, to), DoubleInterval(1, stepCount)));
    } else {
      m_sliderTransformMap.put(ctrlId, new LinearTransformation(     DoubleInterval(from, to), DoubleInterval(1, stepCount)));
    }
  }

  void unknownSliderException(int ctrlId) const {
    throwException(_T("SliderCtrl with id %d not initialized"), ctrlId);
  }

public:
  CColormapDialog(int resId, int propertyId, CWnd *pParent) : CPropertyDialog<T>(resId, propertyId, pParent) {
  }

  ~CColormapDialog() {
    for(Iterator<Entry<int, IntervalTransformation*> > it = m_sliderTransformMap.entrySet().getIterator(); it.hasNext();) {
      Entry<int, IntervalTransformation*> &e = it.next();
      delete e.getValue();
    }
  }

protected:
  void initSlider(int ctrlId, double from, double to, unsigned int stepCount = 100, bool logarithmic = false) {
    addTransformation(ctrlId, from, to, stepCount, logarithmic);

    CSliderCtrl *slider = (CSliderCtrl*)GetDlgItem(ctrlId);
    slider->SetRange(1, stepCount);
    slider->SetPos(1);
  }

  float getSliderValue(int ctrlId) const {
    const IntervalTransformation *transform = getTransformation(ctrlId);
    if(transform == NULL) unknownSliderException(ctrlId);
    return (float)transform->backwardTransform(((CSliderCtrl*)GetDlgItem(ctrlId))->GetPos());
  }

  void setSliderValue(int ctrlId, double value) {
    const IntervalTransformation *transform = getTransformation(ctrlId);
    if(transform == NULL) unknownSliderException(ctrlId);
    ((CSliderCtrl*)GetDlgItem(ctrlId))->SetPos((int)transform->forwardTransform(value));
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

  void putWindowBesideParent() {
    if(GetParent()) {
      putWindowBesideWindow(this, GetParent());
    }
  }
};

