#include "stdafx.h"
#include "ParametricSurfaceDlg.h"

CParametricSurfaceDlg::CParametricSurfaceDlg(const ParametricSurfaceParameters &param, CWnd *pParent /*=NULL*/)
: SaveLoadExprDialog<ParametricSurfaceParameters>(IDD, pParent, param,_T("Parametric Surface"), _T("par"))
, m_sStepCount(0)
, m_tStepCount(0)
{
}

CParametricSurfaceDlg::~CParametricSurfaceDlg() {
}

void CParametricSurfaceDlg::DoDataExchange(CDataExchange *pDX) {
  __super::DoDataExchange(pDX);
  DDX_Text(pDX, IDC_EDITCOMMON        , m_commonText );
  DDX_Text(pDX, IDC_EDIT_EXPRX        , m_exprX      );
  DDX_Text(pDX, IDC_EDIT_EXPRY        , m_exprY      );
  DDX_Text(pDX, IDC_EDIT_EXPRZ        , m_exprZ      );
  DDX_Text(pDX, IDC_EDIT_TFROM        , m_tfrom      );
  DDX_Text(pDX, IDC_EDIT_TTO          , m_tto        );
  DDX_Text(pDX, IDC_EDIT_SFROM        , m_sfrom      );
  DDX_Text(pDX, IDC_EDIT_STO          , m_sto        );
  DDX_Check(pDX, IDC_CHECK_INCLUDETIME, m_includeTime);
  DDX_Check(pDX, IDC_CHECK_DOUBLESIDED, m_doubleSided);
  DDX_Text(pDX, IDC_EDIT_TIMEFROM     , m_timefrom   );
  DDX_Text(pDX, IDC_EDIT_TIMETO       , m_timeto     );
  DDX_Text(pDX, IDC_EDIT_TSTEPCOUNT   , m_tStepCount );
  DDV_MinMaxUInt(pDX, m_tStepCount    , 1, 200       );
  DDX_Text(pDX, IDC_EDIT_SSTEPCOUNT   , m_sStepCount );
  DDV_MinMaxUInt(pDX, m_sStepCount    , 1, 200       );
  DDX_Text(pDX, IDC_EDIT_FRAMECOUNT   , m_frameCount );
  DDV_MinMaxUInt(pDX, m_frameCount    , 1, 300       );
  DDX_Check(pDX, IDC_CHECK_MACHINECODE, m_machineCode);
}

BEGIN_MESSAGE_MAP(CParametricSurfaceDlg, CDialog)
    ON_WM_SIZE()
    ON_COMMAND(ID_FILE_OPEN                  , OnFileOpen                       )
    ON_COMMAND(ID_FILE_SAVE                  , OnFileSave                       )
    ON_COMMAND(ID_FILE_SAVE_AS               , OnFileSaveAs                     )
    ON_COMMAND(ID_EDIT_FINDMATCHINGPARENTESIS, OnEditFindMatchingParentesis     )
    ON_COMMAND(ID_GOTO_COMMON                , OnGotoCommon                     )
    ON_COMMAND(ID_GOTO_EXPRX                 , OnGotoExprX                      )
    ON_COMMAND(ID_GOTO_EXPRY                 , OnGotoExprY                      )
    ON_COMMAND(ID_GOTO_EXPRZ                 , OnGotoExprZ                      )
    ON_COMMAND(ID_GOTO_TINTERVAL             , OnGotoTInterval                  )
    ON_COMMAND(ID_GOTO_SINTERVAL             , OnGotoSInterval                  )
    ON_COMMAND(ID_GOTO_TSTEPCOUNT            , OnGotoTStepCount                 )
    ON_COMMAND(ID_GOTO_SSTEPCOUNT            , OnGotoSStepCount                 )
    ON_COMMAND(ID_GOTO_TIMEINTERVAL          , OnGotoTimeInterval               )
    ON_COMMAND(ID_GOTO_FRAMECOUNT            , OnGotoFrameCount                 )
    ON_BN_CLICKED(IDC_BUTTON_HELPX           , OnButtonHelpX                    )
    ON_BN_CLICKED(IDC_BUTTON_HELPY           , OnButtonHelpY                    )
    ON_BN_CLICKED(IDC_BUTTON_HELPZ           , OnButtonHelpZ                    )
    ON_BN_CLICKED(IDC_CHECK_INCLUDETIME      , OnCheckIncludeTime               )
END_MESSAGE_MAP()

BOOL CParametricSurfaceDlg::OnInitDialog() {
  __super::OnInitDialog();

  createExprHelpButton(IDC_BUTTON_HELPX, IDC_EDIT_EXPRX);
  createExprHelpButton(IDC_BUTTON_HELPY, IDC_EDIT_EXPRY);
  createExprHelpButton(IDC_BUTTON_HELPZ, IDC_EDIT_EXPRZ);
  setCommonExprFieldId(IDC_EDITCOMMON);
  m_layoutManager.OnInitDialog(this);
  m_layoutManager.addControl(IDC_STATICCOMMON        , PCT_RELATIVE_Y_CENTER);
  m_layoutManager.addControl(IDC_EDITCOMMON          , PCT_RELATIVE_BOTTOM | RELATIVE_WIDTH);
  m_layoutManager.addControl(IDC_STATIC_FUNCTIONX    , PCT_RELATIVE_Y_CENTER);
  m_layoutManager.addControl(IDC_BUTTON_HELPX        , PCT_RELATIVE_Y_CENTER);
  m_layoutManager.addControl(IDC_EDIT_EXPRX          , PCT_RELATIVE_TOP    | PCT_RELATIVE_BOTTOM | RELATIVE_WIDTH);
  m_layoutManager.addControl(IDC_STATIC_FUNCTIONY    , PCT_RELATIVE_Y_CENTER);
  m_layoutManager.addControl(IDC_BUTTON_HELPY        , PCT_RELATIVE_Y_CENTER);
  m_layoutManager.addControl(IDC_EDIT_EXPRY          , PCT_RELATIVE_TOP    | PCT_RELATIVE_BOTTOM | RELATIVE_WIDTH);
  m_layoutManager.addControl(IDC_STATIC_FUNCTIONZ    , PCT_RELATIVE_Y_CENTER);
  m_layoutManager.addControl(IDC_BUTTON_HELPZ        , PCT_RELATIVE_Y_CENTER);
  m_layoutManager.addControl(IDC_EDIT_EXPRZ          , PCT_RELATIVE_TOP    | RELATIVE_BOTTOM     | RELATIVE_WIDTH);
  m_layoutManager.addControl(IDC_STATIC_TINTERVAL    , RELATIVE_Y_POS       );
  m_layoutManager.addControl(IDC_EDIT_TFROM          , RELATIVE_Y_POS       );
  m_layoutManager.addControl(IDC_STATIC_DASH1        , RELATIVE_Y_POS       );
  m_layoutManager.addControl(IDC_EDIT_TTO            , RELATIVE_Y_POS       );
  m_layoutManager.addControl(IDC_STATIC_SINTERVAL    , RELATIVE_Y_POS       );
  m_layoutManager.addControl(IDC_EDIT_SFROM          , RELATIVE_Y_POS       );
  m_layoutManager.addControl(IDC_STATIC_DASH2        , RELATIVE_Y_POS       );
  m_layoutManager.addControl(IDC_EDIT_STO            , RELATIVE_Y_POS       );
  m_layoutManager.addControl(IDC_STATIC_TSTEPCOUNT   , RELATIVE_Y_POS       );
  m_layoutManager.addControl(IDC_EDIT_TSTEPCOUNT     , RELATIVE_Y_POS       );
  m_layoutManager.addControl(IDC_STATIC_SSTEPCOUNT   , RELATIVE_Y_POS       );
  m_layoutManager.addControl(IDC_EDIT_SSTEPCOUNT     , RELATIVE_Y_POS       );
  m_layoutManager.addControl(IDC_CHECK_MACHINECODE   , RELATIVE_Y_POS       );
  m_layoutManager.addControl(IDC_CHECK_INCLUDETIME   , RELATIVE_Y_POS       );
  m_layoutManager.addControl(IDC_CHECK_DOUBLESIDED   , RELATIVE_Y_POS       );
  m_layoutManager.addControl(IDC_STATIC_TIMEINTERVAL , RELATIVE_Y_POS       );
  m_layoutManager.addControl(IDC_EDIT_TIMEFROM       , RELATIVE_Y_POS       );
  m_layoutManager.addControl(IDC_STATIC_DASH3        , RELATIVE_Y_POS       );
  m_layoutManager.addControl(IDC_EDIT_TIMETO         , RELATIVE_Y_POS       );
  m_layoutManager.addControl(IDC_STATIC_FRAMECOUNT   , RELATIVE_Y_POS       );
  m_layoutManager.addControl(IDC_EDIT_FRAMECOUNT     , RELATIVE_Y_POS       );
  m_layoutManager.addControl(IDOK                    , RELATIVE_POSITION    );
  m_layoutManager.addControl(IDCANCEL                , RELATIVE_POSITION    );

  gotoEditBox(this, IDC_EDIT_EXPRX);
  return FALSE;  // return TRUE  unless you set the focus to a control
}

void CParametricSurfaceDlg::OnFileOpen() {
  __super::OnFileOpen();
  enableTimeFields();
}

#define MAXPOINTCOUNT 200
#define MAXFRAMECOUNT 300

bool CParametricSurfaceDlg::validate() {
  if(!validateAllExpr()) {
    return false;
  }
  if(!validateInterval(IDC_EDIT_TFROM, IDC_EDIT_TTO)) {
    return false;
  }
  if(!validateMinMax(IDC_EDIT_TSTEPCOUNT, 1, MAXPOINTCOUNT)) {
    return false;
  }
  if(!validateInterval(IDC_EDIT_SFROM, IDC_EDIT_STO)) {
    return false;
  }
  if(!validateMinMax(IDC_EDIT_SSTEPCOUNT, 1, MAXPOINTCOUNT)) {
    return false;
  }

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

void CParametricSurfaceDlg::OnCheckIncludeTime() {
  UpdateData(TRUE);
  enableTimeFields();
}

void CParametricSurfaceDlg::enableTimeFields() {
  BOOL enable = IsDlgButtonChecked(IDC_CHECK_INCLUDETIME);
  GetDlgItem(IDC_STATIC_TIMEINTERVAL)->EnableWindow(enable);
  GetDlgItem(IDC_EDIT_TIMEFROM      )->EnableWindow(enable);
  GetDlgItem(IDC_EDIT_TIMETO        )->EnableWindow(enable);
  GetDlgItem(IDC_EDIT_FRAMECOUNT    )->EnableWindow(enable);
  setWindowText(this, IDC_STATIC_FUNCTIONX, enable ? _T("&X(time,t,s) =") : _T("&X(t,s) ="));
  setWindowText(this, IDC_STATIC_FUNCTIONY, enable ? _T("&Y(time,t,s) =") : _T("&Y(t,s) ="));
  setWindowText(this, IDC_STATIC_FUNCTIONZ, enable ? _T("&Z(time,t,s) =") : _T("&Z(t,s) ="));
}

void CParametricSurfaceDlg::OnEditFindMatchingParentesis() {
  gotoMatchingParentesis();
}

void CParametricSurfaceDlg::OnGotoCommon() {
  gotoExpr(IDC_EDITCOMMON);
}
void CParametricSurfaceDlg::OnGotoExprX() {
  gotoExprX();
}
void CParametricSurfaceDlg::OnGotoExprY() {
  gotoExprY();
}
void CParametricSurfaceDlg::OnGotoExprZ() {
  gotoExprZ();
}
void CParametricSurfaceDlg::OnGotoTInterval() {
  gotoEditBox(this, IDC_EDIT_TFROM);
}
void CParametricSurfaceDlg::OnGotoSInterval() {
  gotoEditBox(this, IDC_EDIT_SFROM);
}
void CParametricSurfaceDlg::OnGotoTStepCount() {
  gotoEditBox(this, IDC_EDIT_TSTEPCOUNT);
}
void CParametricSurfaceDlg::OnGotoSStepCount() {
  gotoEditBox(this, IDC_EDIT_SSTEPCOUNT);
}
void CParametricSurfaceDlg::OnGotoTimeInterval() {
  gotoEditBox(this, IDC_EDIT_TIMEFROM);
}
void CParametricSurfaceDlg::OnGotoFrameCount() {
  gotoEditBox(this, IDC_EDIT_FRAMECOUNT);
}

void CParametricSurfaceDlg::OnButtonHelpX() {
  handleExprHelpButtonClick(IDC_BUTTON_HELPX);
}

void CParametricSurfaceDlg::OnButtonHelpY() {
  handleExprHelpButtonClick(IDC_BUTTON_HELPY);
}

void CParametricSurfaceDlg::OnButtonHelpZ() {
  handleExprHelpButtonClick(IDC_BUTTON_HELPZ);
}

void CParametricSurfaceDlg::paramToWin(const ParametricSurfaceParameters &param) {
  m_commonText    = param.m_commonText.cstr();
  m_exprX         = param.m_exprX.cstr();
  m_exprY         = param.m_exprY.cstr();
  m_exprZ         = param.m_exprZ.cstr();
  m_tfrom         = param.getTInterval().getMin();
  m_tto           = param.getTInterval().getMax();
  m_sfrom         = param.getSInterval().getMin();
  m_sto           = param.getSInterval().getMax();
  m_timefrom      = param.getTimeInterval().getMin();
  m_timeto        = param.getTimeInterval().getMax();
  m_tStepCount    = param.m_tStepCount;
  m_sStepCount    = param.m_sStepCount;
  m_frameCount    = param.m_frameCount;
  m_machineCode   = param.m_machineCode ? TRUE : FALSE;
  m_includeTime   = param.m_includeTime ? TRUE : FALSE;
  m_doubleSided   = param.m_doubleSided ? TRUE : FALSE;

  UpdateData(false);
  enableTimeFields();
  __super::paramToWin(param);
}

void CParametricSurfaceDlg::winToParam(ParametricSurfaceParameters &param) const {
  param.m_commonText  = m_commonText;
  param.m_exprX       = m_exprX;
  param.m_exprY       = m_exprY;
  param.m_exprZ       = m_exprZ;
  param.m_tInterval.setFrom(   m_tfrom);
  param.m_tInterval.setTo(     m_tto);
  param.m_sInterval.setFrom(   m_sfrom);
  param.m_sInterval.setTo(     m_sto);
  param.m_timeInterval.setFrom(m_timefrom);
  param.m_timeInterval.setTo(  m_timeto);
  param.m_tStepCount  = m_tStepCount;
  param.m_sStepCount  = m_sStepCount;
  param.m_frameCount  = m_frameCount;
  param.m_machineCode = m_machineCode ? true : false;
  param.m_includeTime = m_includeTime ? true : false;
  param.m_doubleSided = m_doubleSided ? true : false;
  __super::winToParam(param);
}
