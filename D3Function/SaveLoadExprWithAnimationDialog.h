#pragma once

#include <MFCUtil/ExprDialog.h>
#include <FileNameSplitter.h>

template<typename T> class SaveLoadExprWithAnimationDialog : public SaveLoadExprDialog<T> {
protected:
  BOOL    m_includeTime;
  double  m_timefrom;
  double  m_timeto;
  UINT    m_frameCount;
  BOOL    m_machineCode;
  BOOL    m_createListFile;

  SaveLoadExprWithAnimationDialog<T>(int resId, CWnd *pParent, const T &param, const String &paramTypeName, const String &extension)
    : SaveLoadExprDialog<T>(resId, pParent, param, paramTypeName, extension)
    , m_createListFile(FALSE)
  {
  }
  String getListFileName() const {
    if(!m_createListFile) return __super::getListFileName();
    return FileNameSplitter(getData().getName()).setExtension(_T("lst")).getFullPath();
  }

  inline void setTimeInterval(const DoubleInterval &interval) {
    m_timefrom = interval.getMin(); m_timeto = interval.getMax();
  }
  inline DoubleInterval getTimeInterval() const {
    return DoubleInterval(m_timefrom, m_timeto);
  }

#define MAXFRAMECOUNT 300

  bool validate() {
    if(m_includeTime) {
      if(!validateMinMax(IDC_EDIT_FRAMECOUNT, 1, MAXFRAMECOUNT)) {
        return false;
      }
      if(!validateInterval(IDC_EDIT_TIMEFROM, IDC_EDIT_TIMETO)) {
        return false;
      }
    }
    return true;
  }

  void paramToWin(const T &param) {
    const AnimationParameters &d = param.m_animation;
    m_includeTime   = d.m_includeTime ? TRUE : FALSE;
    m_frameCount    = d.m_frameCount;
    m_timefrom      = d.getTimeInterval().getMin();
    m_timeto        = d.getTimeInterval().getMax();
    m_machineCode   = param.m_machineCode ? TRUE : FALSE;
    enableTimeFields();
    enableCreateListFile();
    __super::paramToWin(param);
  }

  bool winToParam(T &param) {
    if(!__super::winToParam(param)) return false;
    AnimationParameters &d = param.m_animation;
    d.m_includeTime = m_includeTime ? true : false;
    d.m_frameCount  = m_frameCount;
    d.m_timeInterval.setFrom(m_timefrom);
    d.m_timeInterval.setTo(  m_timeto);
    param.m_machineCode = m_machineCode ? true : false;
    return true;
  }

  void DoDataExchange(CDataExchange *pDX) {
  __super::DoDataExchange(pDX);
    DDX_Check(pDX, IDC_CHECK_INCLUDETIME  , m_includeTime   );
    DDX_Text( pDX, IDC_EDIT_TIMEFROM      , m_timefrom      );
    DDX_Text( pDX, IDC_EDIT_TIMETO        , m_timeto        );
    DDX_Text( pDX, IDC_EDIT_FRAMECOUNT    , m_frameCount    );
    DDV_MinMaxUInt(pDX, m_frameCount      , 1, 300          );
    DDX_Check(pDX, IDC_CHECK_MACHINECODE  , m_machineCode   );
    DDX_Check(pDX, IDC_CHECKCREATELISTFILE, m_createListFile);
  }
  void OnGotoTimeInterval() {
    gotoEditBox(this, IDC_EDIT_TIMEFROM);
  }
  void OnGotoFrameCount() {
    gotoEditBox(this, IDC_EDIT_FRAMECOUNT);
  }
  void OnCheckMachineCode() {
    UpdateData(TRUE);
    enableCreateListFile();
  }

  void OnCheckIncludeTime() {
    UpdateData(TRUE);
    enableTimeFields();
  }

  void enableCreateListFile() {
    if(!m_machineCode) {
      m_createListFile = false;
      CheckDlgButton(IDC_CHECKCREATELISTFILE, BST_UNCHECKED);
    }
    GetDlgItem(IDC_CHECKCREATELISTFILE)->EnableWindow(m_machineCode);
  }
  virtual void enableTimeFields() {
    enableWindowList(*this, m_includeTime
                    ,IDC_STATIC_TIMEINTERVAL, IDC_EDIT_TIMEFROM
                    ,IDC_EDIT_TIMETO        , IDC_STATIC_FRAMECOUNT
                    ,IDC_EDIT_FRAMECOUNT
                    );
  }
};
