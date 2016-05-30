#pragma once

// ColormapCtrl.h : Declaration of the CColormapCtrl ActiveX Control class.

#include "ColorMapGraphics.h"

typedef enum {
    UPDATE_RGB = 1
  , UPDATE_LSH = 2
  , UPDATE_ALL = 3
} FieldUpdateSet;


// CColormapCtrl : See ColormapCtrl.cpp for implementation.

class CColormapCtrl : public COleControl
{
	DECLARE_DYNCREATE(CColormapCtrl)

private:
  ColorMapGraphics m_colorMap;
  CEdit            m_redEdit;
  CEdit            m_greenEdit;
  CEdit            m_blueEdit;
  CEdit            m_hueEdit;
  CEdit            m_saturationEdit;
  CEdit            m_luminationEdit;
  CFont            m_font;
  int              m_currentField;
  COLORREF         m_oldColor;
  bool             m_updateActive;
  bool             m_firstDrawDone;
  void drawEditFields(CDC *pdc, BOOL enabled);
  void drawEditField(CDC *pdc, const CRect &r, BOOL enabled);
  void setFieldPositions(BOOL enabled);
  void setFieldPosition(CEdit &f, const CRect &r, BOOL enabled);
  void drawMap();
  void setModifiedAndInvalidate();
  void createEditField(CEdit &f, const CRect &r, int id);
  void setFieldValue(CEdit &f, int value);
  int  getFieldValue(CEdit &f);
  void adjustHue(int dv);
  void adjustSaturation(int dv);
  void adjustLumination(int dv);
  bool adjustField(CEdit &f, int dv);
  void gotoField(int field);
  void initFields(FieldUpdateSet set);
  void updateFields(FieldUpdateSet set);
  void updateFields();
  void updateColor(COLORREF color);
  void updateLSHColor(const LSHColor &lsh);
  void fireIfChanged(COLORREF color);
  CRect getMapRect();
  CRect getScalePosRect();

  afx_msg void OnChangeRed();
  afx_msg void OnChangeGreen();
  afx_msg void OnChangeBlue();
  afx_msg void OnChangeHue();
  afx_msg void OnChangeSaturation();
  afx_msg void OnChangeLumination();
  afx_msg void OnSetFocusRed();
  afx_msg void OnSetFocusGreen();
  afx_msg void OnSetFocusBlue();
  afx_msg void OnSetFocusHue();
  afx_msg void OnSetFocusSaturation();
  afx_msg void OnSetFocusLumination();

public:
  // Constructor
  CColormapCtrl();

// Overrides
public:
	virtual void OnDraw(CDC* pdc, const CRect& rcBounds, const CRect& rcInvalid);
	virtual void DoPropExchange(CPropExchange* pPX);
	virtual void OnResetState();
  virtual BOOL PreTranslateMessage(MSG* pMsg);

// Implementation
protected:
	~CColormapCtrl();

	DECLARE_OLECREATE_EX(CColormapCtrl)    // Class factory and guid
	DECLARE_OLETYPELIB(CColormapCtrl)      // GetTypeInfo
	DECLARE_PROPPAGEIDS(CColormapCtrl)     // Property page IDs
	DECLARE_OLECTLTYPE(CColormapCtrl)		// Type name and misc status

  afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
  afx_msg void OnDestroy();
  afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
  afx_msg void OnMouseMove(UINT nFlags, CPoint point);
  afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
  afx_msg void OnSetFocus(CWnd* pOldWnd);

  // Message maps
	DECLARE_MESSAGE_MAP()

  afx_msg long GetColor();
  afx_msg void SetColor(long nNewValue);
  afx_msg BOOL GetSunken();
  afx_msg void SetSunken(BOOL bNewValue);
  afx_msg BOOL GetHasBorder();
  afx_msg void SetHasBorder(BOOL bNewValue);
  afx_msg BSTR GetCaption();
  afx_msg void SetCaption(LPCTSTR lpszNewValue);
  afx_msg BOOL GetClientEdge();
  afx_msg void SetClientEdge(BOOL bNewValue);
  afx_msg BOOL GetStaticEdge();
  afx_msg void SetStaticEdge(BOOL bNewValue);
  afx_msg BOOL GetModalFrame();
  afx_msg void SetModalFrame(BOOL bNewValue);
  afx_msg long GetTextAlignment();
  afx_msg void SetTextAlignment(long nNewValue);
  
  // Dispatch maps
	DECLARE_DISPATCH_MAP()

	afx_msg void AboutBox();

  void FireColorChanged()
  {
    FireEvent(eventidColorChanged, EVENT_PARAM(VTS_NONE));
  }
  // Event maps
	DECLARE_EVENT_MAP()

// Dispatch and event IDs
public:
	enum {
    dispidColor = 1L,
    dispidSunken = 2L,
    dispidBorder = 3L,
    dispidCaption = 4L,
    dispidClientEdge = 5L,
    dispidStaticEdge = 6L,
    dispidModalFrame = 7L,
    dispidTextAlignment = 8L,
    eventidColorChanged = 1L,
  };
};

