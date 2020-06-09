#pragma once

#include <MFCUtil/ExprDialog.h>
#include <FileNameSplitter.h>
#include <D3DGraphics/D3AbstractTextureFactory.h>

template<typename T> class SaveLoadExprWithCommonParametersDialog : public SaveLoadExprDialog<T> {
protected:
  BOOL    m_includeTime;
  double  m_timefrom;
  double  m_timeto;
  UINT    m_frameCount;
  BOOL    m_machineCode;
  BOOL    m_createListFile;
  BOOL    m_doubleSided;
  BOOL    m_calculateNormals;
  BOOL    m_hasTexture;
  CString m_textureFileName;
  AbstractTextureFactory &m_atf;

  SaveLoadExprWithCommonParametersDialog<T>(int resId, CWnd *pParent, const T &param, AbstractTextureFactory &atf, const String &paramTypeName, const String &extension)
    : SaveLoadExprDialog<T>(resId, pParent, param, paramTypeName, extension)
    , m_atf(                atf  )
    , m_createListFile(     FALSE)
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

  bool validateTexture(const String &textureFileName) const {
    if(m_atf.validateTextureFile(textureFileName)) {
      gotoEditBox((CWnd*)this, IDC_EDIT_TEXTUREFILENAME);
      showWarning(_T("Cannot load texture from file %s"), textureFileName.cstr());
      return false;
    }
    return true;
  }

  bool validate() {
    if(m_includeTime) {
      if(!validateMinMax(IDC_EDIT_FRAMECOUNT, 1, MAXFRAMECOUNT)) {
        return false;
      }
      if(!validateInterval(IDC_EDIT_TIMEFROM, IDC_EDIT_TIMETO)) {
        return false;
      }
    }
    if(m_hasTexture) {
      if(m_textureFileName.GetLength() == 0) {
        gotoEditBox(this, IDC_EDIT_TEXTUREFILENAME);
        showWarning(_T("Must specify texture image file"));
        return false;
      }
      if(!validateTexture((LPCTSTR)m_textureFileName)) {
        return false;
      }
    }
    return true;
  }

  void paramToWin(const T &param) {
    const AnimationParameters &d = param.m_animation;
    m_includeTime      = d.m_includeTime     ? TRUE : FALSE;
    m_frameCount       = d.m_frameCount;
    m_timefrom         = d.getTimeInterval().getMin();
    m_timeto           = d.getTimeInterval().getMax();
    m_machineCode      = param.m_machineCode ? TRUE : FALSE;
    m_doubleSided      = param.m_doubleSided ? TRUE : FALSE;
    m_calculateNormals = param.m_hasNormals  ? TRUE : FALSE;
    m_hasTexture       = param.m_hasTexture  ? TRUE : FALSE;
    m_textureFileName  = param.m_textureFileName.cstr();
    enableTimeFields();
    enableCreateListFile();
    enableTextureFields();
    __super::paramToWin(param);
  }

  bool winToParam(T &param) {
    if(!__super::winToParam(param)) return false;
    AnimationParameters &d  = param.m_animation;
    d.m_includeTime         = m_includeTime      ? true : false;
    d.m_frameCount          = m_frameCount;
    d.m_timeInterval        = DoubleInterval(m_timefrom, m_timeto);
    param.m_machineCode     = m_machineCode      ? true : false;
    param.m_doubleSided     = m_doubleSided      ? true : false;
    param.m_hasNormals      = m_calculateNormals ? true : false;
    param.m_hasTexture      = m_hasTexture       ? true : false;
    param.m_textureFileName = (LPCTSTR)m_textureFileName;
    return true;
  }

  void DoDataExchange(CDataExchange *pDX) {
  __super::DoDataExchange(pDX);
    DDX_Check(pDX, IDC_CHECK_INCLUDETIME     , m_includeTime     );
    DDX_Text( pDX, IDC_EDIT_TIMEFROM         , m_timefrom        );
    DDX_Text( pDX, IDC_EDIT_TIMETO           , m_timeto          );
    DDX_Text( pDX, IDC_EDIT_FRAMECOUNT       , m_frameCount      );
    DDV_MinMaxUInt(pDX, m_frameCount         , 1, 300            );
    DDX_Check(pDX, IDC_CHECK_MACHINECODE     , m_machineCode     );
    DDX_Check(pDX, IDC_CHECK_CREATELISTFILE  , m_createListFile  );
    DDX_Check(pDX, IDC_CHECK_DOUBLESIDED     , m_doubleSided     );
    DDX_Check(pDX, IDC_CHECK_HASTEXTURE      , m_hasTexture      );
    DDX_Check(pDX, IDC_CHECK_CALCULATENORMALS, m_calculateNormals);
    DDX_Text( pDX, IDC_EDIT_TEXTUREFILENAME  , m_textureFileName );
  }
  void OnBnClickedCheckMachineCode() {
    UpdateData(TRUE);
    enableCreateListFile();
  }

  void OnBnClickedCheckIncludeTime() {
    UpdateData(TRUE);
    enableTimeFields();
  }

  void enableCreateListFile() {
    if(!m_machineCode) {
      m_createListFile = false;
      CheckDlgButton(IDC_CHECK_CREATELISTFILE, BST_UNCHECKED);
    }
    GetDlgItem(IDC_CHECK_CREATELISTFILE)->EnableWindow(m_machineCode);
  }
  virtual void enableTimeFields() {
    enableWindowList(*this, m_includeTime
                    ,IDC_STATIC_TIMEINTERVAL, IDC_EDIT_TIMEFROM
                    ,IDC_EDIT_TIMETO        , IDC_STATIC_FRAMECOUNT
                    ,IDC_EDIT_FRAMECOUNT
                    ,0
                    );
  }
  virtual void enableTextureFields() {
    enableWindowList(*this, m_hasTexture
                    ,IDC_EDIT_TEXTUREFILENAME
                    ,IDC_BUTTON_BROWSETEXTURE
                    ,0);
  }

  afx_msg void OnBnClickedCheckHasTexture() {
    UpdateData();
    enableTextureFields();
  }

  afx_msg void OnBnClickedButtonBrowseTexture() {
    const String fileName = selectAndValidateTextureFile(m_atf);
    if(fileName.length() > 0) {
      setWindowText(this, IDC_EDIT_TEXTUREFILENAME, fileName);
    }
  }
};
