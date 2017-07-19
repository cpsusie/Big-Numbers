#pragma once

// ColormapPropPage.h : Declaration of the CColormapPropPage property page class.


// CColormapPropPage : See ColormapPropPage.cpp for implementation.

class CColormapPropPage : public COlePropertyPage
{
    DECLARE_DYNCREATE(CColormapPropPage)
    DECLARE_OLECREATE_EX(CColormapPropPage)
  HACCEL m_accelTable;

// Constructor
public:
    CColormapPropPage();
  void adjustFieldStates();

// Dialog Data
    enum { IDD = IDD_PROPPAGE_COLORMAP };
  BOOL    m_sunken;
  BOOL    m_hasBorder;
  CString m_caption;
  BOOL    m_clientEdge;
  BOOL    m_staticEdge;
  BOOL    m_modalFrame;
  int     m_textAlignment;

  virtual BOOL PreTranslateMessage(MSG *pMsg);
  virtual void DoDataExchange(CDataExchange *pDX);    // DDX/DDV support
  virtual BOOL OnInitDialog();
  afx_msg void OnCheckSunken();
  afx_msg void OnCheckBorder();
  afx_msg void OnUpdateEditCaption();
  afx_msg void OnCheckClientEdge();
  afx_msg void OnCheckStaticEdge();
  afx_msg void OnCheckModalFrame();
  afx_msg void OnChangeEditCaption();
  afx_msg void OnEditChangeComboAlignment();
  afx_msg void OnGotoCaption();
  afx_msg void OnGotoAlignment();
  DECLARE_MESSAGE_MAP()
};

