#pragma once

#ifndef TABLEBASE_BUILDER

#include "Externengine.h"

class CEngineOptionsDlg;
class CEngineOptionsDlgThread;

class EngineOptionDialogColumnInfo {
public:
  int m_maxCheckBoxTextWidth;
  int m_maxSpinLabelTextWidth;
  int m_maxLabel2TextWidth;
  int m_maxSpinBoxWidth;
  int m_maxNonSpinLabelTextWidth;
  EngineOptionDialogColumnInfo(int checkBoxTextWidth
                              ,int spinLabelTextWidth
                              ,int label2TextWidth
                              ,int spinBoxWidth
                              ,int nonSpinLabelTextWidth)
    : m_maxCheckBoxTextWidth(checkBoxTextWidth)
    , m_maxSpinLabelTextWidth(spinLabelTextWidth)
    , m_maxLabel2TextWidth(label2TextWidth)
    , m_maxSpinBoxWidth(spinBoxWidth)
    , m_maxNonSpinLabelTextWidth(nonSpinLabelTextWidth)
    {
    }
};

class DialogColumnInfoArray : public Array<EngineOptionDialogColumnInfo> {
public:
  DialogColumnInfoArray(int maxControlsPerColumn, int ctrlHeight, const CSize &space)
  : m_maxControlsPerColumn(maxControlsPerColumn)
  , m_ctrlHeight(ctrlHeight)
  , m_space(space)
  {
  }
  int   m_maxControlsPerColumn;
  int   m_ctrlHeight;
  CSize m_space;
};

class EngineOptionControl {
private:
  void invalidTypeException() const;
  void invalidTypeException(int type, const String &name) const;
public:
  static CStatic *newStaticLabel(CWnd *parent, const String &s);
  const EngineOptionDescription &m_option;
  CStatic                       *m_label, *m_label2;
  CWnd                          *m_ctrl;
  CSpinButtonCtrl               *m_spinCtrl;
  int                            m_ctrlId, m_spinCtrlId;
  char                           m_column;
  EngineOptionControl(CEngineOptionsDlg *parent, const EngineOptionDescription &option);
  void cleanup();
  EngineOptionType getType() const {
    return m_option.getType();
  }
  void   setFont(CFont *font);
  void   setValue(bool value);
  void   setValue(int  value);
  void   setValue(const String &value);
  bool   getBoolValue()   const;
  int    getIntValue(bool validate = false) const; // throw Exception if validate and content not numeric
  String getStringValue() const;
  int    findEditBoxWidth(CDC &dc) const;
  void   validate() const; // throw Exception on invalid content
  int    getMin() const {
    return m_option.getMin();
  }
  int    getMax() const {
    return m_option.getMax();
  }
  int    getColumn() const {
    return m_column;
  }
  int setPosition(const CPoint &p, const DialogColumnInfoArray &a); // return rightside of rightmost control
  CRect getWindowRect() const;
  void setEnable(bool enabled);
};

class EngineOptionControlArray : public Array<EngineOptionControl> {
private:
  int getNonPushbuttonCount() const;
  int findMaxCheckBoxTextWidth(CDC &dc, int from, int to) const;
  int findMaxLabelTextWidth(   CDC &dc, int from, int to, bool intFields) const;
  int findMaxLabel2TextWidth(  CDC &dc, int from, int to) const;
  int findMaxSpinBoxWidth(     CDC &dc, int from, int to) const;
  void setColumn(char column, int from, int to);
public:
  ~EngineOptionControlArray();
  EngineOptionControl *findControlById(  int          ctrlId);
  EngineOptionControl *findControlByName(const String &name );
  void putValues(const EngineOptionValueArray &valueArray);
  EngineOptionValueArray getValues(const String &engineName) const;
  DialogColumnInfoArray findColumnInfo(CWnd *wnd, int controlHeight, const CSize &space);
  CPoint createGrid(const DialogColumnInfoArray &columnInfo, const CPoint &ul);
  CompactIntArray selectCtrlIdByType(EngineOptionType type) const;
};

class CEngineOptionsDlg : public CDialog {
private:
  const Player                       m_player;
  const String                       m_engineName;
  const EngineOptionDescriptionArray m_optionArray;
  CEngineOptionsDlgThread           *m_thread;
  ExternEngine                      *m_engine;
  EngineOptionControlArray           m_controls;
  EngineOptionValueArray             m_origValueArray;
  int                                m_buttonCounter, m_editCounter, m_spinCounter, m_comboCounter;
  String                             m_currentToolTip;
  void     commonInit();
  static EngineOptionDescriptionArray createEngineOptionDescriptionArray(Player player);
  CSize    findBestClientRectSize();
  void     movePusbuttonsToCorner(const CompactIntArray &buttonIdArray);
  void     addControl(const EngineOptionDescription &option);
  void     putValuesToWindow(const EngineOptionValueArray &valueArray);
  void     putDefaultToWindow();
  EngineOptionValueArray getValuesFromWindow() const;
  void     OnUserButton(UINT id);
  void     OnDeltaPosSpin( UINT id, NMHDR *pNMHDR, LRESULT *pResult);
  BOOL     OnToolTipNotify(UINT id, NMHDR *pNMHDR, LRESULT *pResult);
  bool     validate();
  bool     isChanged() const;
public:
    CEngineOptionsDlg(CEngineOptionsDlgThread &thread);
    CEngineOptionsDlg(Player player);
    void enableUserButtons(bool enabled);
    int nextCtrlId(EngineOptionType type);
    int nextSpinCtrlId();
    enum { IDD = IDD_ENGINEOPTIONS_DIALOG };

public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
protected:
    virtual void DoDataExchange(CDataExchange* pDX);

protected:
    virtual BOOL OnInitDialog();
	  afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	  afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	  virtual void OnOK();
	  virtual void OnCancel();
	  afx_msg void OnClose();
    afx_msg void OnButtonSendToEngine();
	  afx_msg void OnButtonSetDefault();
    DECLARE_MESSAGE_MAP()
};

#endif
