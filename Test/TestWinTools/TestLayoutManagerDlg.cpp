#include "stdafx.h"
#include "TestLayoutManagerDlg.h"
#include "RunLayoutManagerDlg.h"

#if defined(_DEBUG)
#define new DEBUG_NEW
#endif

CTestLayoutManagerDlg::CTestLayoutManagerDlg(CWnd *pParent /*=nullptr*/) : CDialog(CTestLayoutManagerDlg::IDD, pParent) {
  setToDefault();
}

void CTestLayoutManagerDlg::setToDefault() {
  m_windowRetainAspectRatio     = FALSE;
  m_windowResizeFont            = FALSE;
  m_windowFontRelativeSize      = FALSE;
  m_list1ResizeFont             = FALSE;
  m_list1FontRelativeSize       = FALSE;
  m_list1RelativeLeft           = FALSE;
  m_list1PctRelativeLeft        = FALSE;
  m_list1PctRelativeXCenter     = FALSE;
  m_list1RelativeTop            = FALSE;
  m_list1PctRelativeTop         = FALSE;
  m_list1PctRelativeYCenter     = FALSE;
  m_list1RelativeRight          = TRUE;
  m_list1PctRelativeRight       = FALSE;
  m_list1RelativeBottom         = TRUE;
  m_list1PctRelativeBottom      = FALSE;
  m_list1FontRelativeXPosition  = FALSE;
  m_list1FontRelativeYPosition  = FALSE;
  m_list1ConstantWidth          = FALSE;
  m_list1ConstantHeight         = FALSE;

  m_list2ResizeFont             = FALSE;
  m_list2FontRelativeSize       = FALSE;
  m_list2RelativeLeft           = FALSE;
  m_list2PctRelativeLeft        = FALSE;
  m_list2PctRelativeXCenter     = FALSE;
  m_list2RelativeTop            = TRUE;
  m_list2PctRelativeTop         = FALSE;
  m_list2PctRelativeYCenter     = FALSE;
  m_list2RelativeRight          = TRUE;
  m_list2PctRelativeRight       = FALSE;
  m_list2RelativeBottom         = TRUE;
  m_list2PctRelativeBottom      = FALSE;
  m_list2FontRelativeXPosition  = FALSE;
  m_list2FontRelativeYPosition  = FALSE;
  m_list2ConstantWidth          = FALSE;
  m_list2ConstantHeight         = FALSE;
  m_list2InitListheaders        = FALSE;
  m_list2ResizeListheaders      = FALSE;

  m_buttonResizeFont            = FALSE;
  m_buttonFontRelativeSize      = FALSE;
  m_buttonFontRelativeXPosition = FALSE;
  m_buttonFontRelativeYPosition = FALSE;
  m_buttonRelativeLeft          = TRUE;
  m_buttonPctRelativeLeft       = FALSE;
  m_buttonPctRelativeXCenter    = FALSE;
  m_buttonRelativeTop           = TRUE;
  m_buttonPctRelativeTop        = FALSE;
  m_buttonPctRelativeYCenter    = FALSE;
  m_buttonRelativeRight         = TRUE;
  m_buttonPctRelativeRight      = FALSE;
  m_buttonRelativeBottom        = TRUE;
  m_buttonPctRelativeBottom     = FALSE;
  m_buttonConstantWidth         = TRUE;
  m_buttonConstantHeight        = TRUE;
}
void CTestLayoutManagerDlg::DoDataExchange(CDataExchange *pDX) {
    __super::DoDataExchange(pDX);
    DDX_Check(pDX, IDC_CHECK_WINDOW_RETAIN_ASPECTRATIO        , m_windowRetainAspectRatio     );
    DDX_Check(pDX, IDC_CHECK_WINDOW_RESIZE_FONT               , m_windowResizeFont            );
    DDX_Check(pDX, IDC_CHECK_WINDOW_FONT_RELATIVE_SIZE        , m_windowFontRelativeSize      );
    DDX_Check(pDX, IDC_CHECK_LIST1_RESIZE_FONT                , m_list1ResizeFont             );
    DDX_Check(pDX, IDC_CHECK_LIST1_FONT_RELATIVE_SIZE         , m_list1FontRelativeSize       );
    DDX_Check(pDX, IDC_CHECK_LIST1_RELATIVE_LEFT              , m_list1RelativeLeft           );
    DDX_Check(pDX, IDC_CHECK_LIST1_PCT_RELATIVE_LEFT          , m_list1PctRelativeLeft        );
    DDX_Check(pDX, IDC_CHECK_LIST1_PCT_RELATIVE_X_CENTER      , m_list1PctRelativeXCenter     );
    DDX_Check(pDX, IDC_CHECK_LIST1_RELATIVE_TOP               , m_list1RelativeTop            );
    DDX_Check(pDX, IDC_CHECK_LIST1_PCT_RELATIVE_TOP           , m_list1PctRelativeTop         );
    DDX_Check(pDX, IDC_CHECK_LIST1_PCT_RELATIVE_Y_CENTER      , m_list1PctRelativeYCenter     );
    DDX_Check(pDX, IDC_CHECK_LIST1_RELATIVE_RIGHT             , m_list1RelativeRight          );
    DDX_Check(pDX, IDC_CHECK_LIST1_PCT_RELATIVE_RIGHT         , m_list1PctRelativeRight       );
    DDX_Check(pDX, IDC_CHECK_LIST1_RELATIVE_BOTTOM            , m_list1RelativeBottom         );
    DDX_Check(pDX, IDC_CHECK_LIST1_PCT_RELATIVE_BOTTOM        , m_list1PctRelativeBottom      );
    DDX_Check(pDX, IDC_CHECK_LIST1_FONT_RELATIVE_X_POSITION   , m_list1FontRelativeXPosition  );
    DDX_Check(pDX, IDC_CHECK_LIST1_FONT_RELATIVE_Y_POSITION   , m_list1FontRelativeYPosition  );
    DDX_Check(pDX, IDC_CHECK_LIST1_CONSTANT_WIDTH             , m_list1ConstantWidth          );
    DDX_Check(pDX, IDC_CHECK_LIST1_CONSTANT_HEIGHT            , m_list1ConstantHeight         );
    DDX_Check(pDX, IDC_CHECK_LIST2_RESIZE_FONT                , m_list2ResizeFont             );
    DDX_Check(pDX, IDC_CHECK_LIST2_FONT_RELATIVE_SIZE         , m_list2FontRelativeSize       );
    DDX_Check(pDX, IDC_CHECK_LIST2_RELATIVE_LEFT              , m_list2RelativeLeft           );
    DDX_Check(pDX, IDC_CHECK_LIST2_PCT_RELATIVE_LEFT          , m_list2PctRelativeLeft        );
    DDX_Check(pDX, IDC_CHECK_LIST2_PCT_RELATIVE_X_CENTER      , m_list2PctRelativeXCenter     );
    DDX_Check(pDX, IDC_CHECK_LIST2_RELATIVE_TOP               , m_list2RelativeTop            );
    DDX_Check(pDX, IDC_CHECK_LIST2_PCT_RELATIVE_TOP           , m_list2PctRelativeTop         );
    DDX_Check(pDX, IDC_CHECK_LIST2_PCT_RELATIVE_Y_CENTER      , m_list2PctRelativeYCenter     );
    DDX_Check(pDX, IDC_CHECK_LIST2_RELATIVE_RIGHT             , m_list2RelativeRight          );
    DDX_Check(pDX, IDC_CHECK_LIST2_PCT_RELATIVE_RIGHT         , m_list2PctRelativeRight       );
    DDX_Check(pDX, IDC_CHECK_LIST2_RELATIVE_BOTTOM            , m_list2RelativeBottom         );
    DDX_Check(pDX, IDC_CHECK_LIST2_PCT_RELATIVE_BOTTOM        , m_list2PctRelativeBottom      );
    DDX_Check(pDX, IDC_CHECK_LIST2_FONT_RELATIVE_X_POSITION   , m_list2FontRelativeXPosition  );
    DDX_Check(pDX, IDC_CHECK_LIST2_FONT_RELATIVE_Y_POSITION   , m_list2FontRelativeYPosition  );
    DDX_Check(pDX, IDC_CHECK_LIST2_CONSTANT_WIDTH             , m_list2ConstantWidth          );
    DDX_Check(pDX, IDC_CHECK_LIST2_CONSTANT_HEIGHT            , m_list2ConstantHeight         );
    DDX_Check(pDX, IDC_CHECK_LIST2_INIT_LISTHEADERS           , m_list2InitListheaders        );
    DDX_Check(pDX, IDC_CHECK_LIST2_RESIZE_LISTHEADERS         , m_list2ResizeListheaders      );
    DDX_Check(pDX, IDC_CHECK_BUTTON_FONT_RELATIVE_SIZE        , m_buttonFontRelativeSize      );
    DDX_Check(pDX, IDC_CHECK_BUTTON_FONT_RELATIVE_X_POSITION  , m_buttonFontRelativeXPosition );
    DDX_Check(pDX, IDC_CHECK_BUTTON_FONT_RELATIVE_Y_POSITION  , m_buttonFontRelativeYPosition );
    DDX_Check(pDX, IDC_CHECK_BUTTON_RELATIVE_LEFT             , m_buttonRelativeLeft          );
    DDX_Check(pDX, IDC_CHECK_BUTTON_PCT_RELATIVE_LEFT         , m_buttonPctRelativeLeft       );
    DDX_Check(pDX, IDC_CHECK_BUTTON_PCT_RELATIVE_X_CENTER     , m_buttonPctRelativeXCenter    );
    DDX_Check(pDX, IDC_CHECK_BUTTON_RELATIVE_TOP              , m_buttonRelativeTop           );
    DDX_Check(pDX, IDC_CHECK_BUTTON_PCT_RELATIVE_TOP          , m_buttonPctRelativeTop        );
    DDX_Check(pDX, IDC_CHECK_BUTTON_PCT_RELATIVE_Y_CENTER     , m_buttonPctRelativeYCenter    );
    DDX_Check(pDX, IDC_CHECK_BUTTON_RELATIVE_RIGHT            , m_buttonRelativeRight         );
    DDX_Check(pDX, IDC_CHECK_BUTTON_PCT_RELATIVE_RIGHT        , m_buttonPctRelativeRight      );
    DDX_Check(pDX, IDC_CHECK_BUTTON_RELATIVE_BOTTOM           , m_buttonRelativeBottom        );
    DDX_Check(pDX, IDC_CHECK_BUTTON_PCT_RELATIVE_BOTTOM       , m_buttonPctRelativeBottom     );
    DDX_Check(pDX, IDC_CHECK_BUTTON_CONSTANT_WIDTH            , m_buttonConstantWidth         );
    DDX_Check(pDX, IDC_CHECK_BUTTON_CONSTANT_HEIGHT           , m_buttonConstantHeight        );
    DDX_Check(pDX, IDC_CHECK_BUTTON_RESIZE_FONT               , m_buttonResizeFont            );
}


BEGIN_MESSAGE_MAP(CTestLayoutManagerDlg, CDialog)
    ON_BN_CLICKED(IDC_BUTTON_OPEN_DIALOG, OnButtonOpenDialog)
    ON_BN_CLICKED(IDC_BUTTON_SETTODEFAULT, OnButtonSetToDefault)
END_MESSAGE_MAP()

int CTestLayoutManagerDlg::getWindowFlags() const {
  return
    (m_windowRetainAspectRatio    ?RETAIN_ASPECTRATIO       :0)
   |(m_windowResizeFont           ?RESIZE_FONT              :0)
   |(m_windowFontRelativeSize     ?FONT_RELATIVE_SIZE       :0)
   ;
}

int CTestLayoutManagerDlg::getList1Flags()  const {
  return
    (m_list1ResizeFont            ? RESIZE_FONT              : 0)
   |(m_list1FontRelativeSize      ? FONT_RELATIVE_SIZE       : 0)
   |(m_list1RelativeLeft          ? RELATIVE_LEFT            : 0)
   |(m_list1PctRelativeLeft       ? PCT_RELATIVE_LEFT        : 0)
   |(m_list1PctRelativeXCenter    ? PCT_RELATIVE_X_CENTER    : 0)
   |(m_list1RelativeTop           ? RELATIVE_TOP             : 0)
   |(m_list1PctRelativeTop        ? PCT_RELATIVE_TOP         : 0)
   |(m_list1PctRelativeYCenter    ? PCT_RELATIVE_Y_CENTER    : 0)
   |(m_list1RelativeRight         ? RELATIVE_RIGHT           : 0)
   |(m_list1PctRelativeRight      ? PCT_RELATIVE_RIGHT       : 0)
   |(m_list1RelativeBottom        ? RELATIVE_BOTTOM          : 0)
   |(m_list1PctRelativeBottom     ? PCT_RELATIVE_BOTTOM      : 0)
   |(m_list1FontRelativeXPosition ? FONT_RELATIVE_LEFT       : 0)
   |(m_list1FontRelativeYPosition ? FONT_RELATIVE_TOP        : 0)
   |(m_list1ConstantWidth         ? CONSTANT_WIDTH           : 0)
   |(m_list1ConstantHeight        ? CONSTANT_HEIGHT          : 0)
   ;
}

int CTestLayoutManagerDlg::getlist2Flags()  const {
  return
    (m_list2ResizeFont            ? RESIZE_FONT              : 0)
   |(m_list2FontRelativeSize      ? FONT_RELATIVE_SIZE       : 0)
   |(m_list2RelativeLeft          ? RELATIVE_LEFT            : 0)
   |(m_list2PctRelativeLeft       ? PCT_RELATIVE_LEFT        : 0)
   |(m_list2PctRelativeXCenter    ? PCT_RELATIVE_X_CENTER    : 0)
   |(m_list2RelativeTop           ? RELATIVE_TOP             : 0)
   |(m_list2PctRelativeTop        ? PCT_RELATIVE_TOP         : 0)
   |(m_list2PctRelativeYCenter    ? PCT_RELATIVE_Y_CENTER    : 0)
   |(m_list2RelativeRight         ? RELATIVE_RIGHT           : 0)
   |(m_list2PctRelativeRight      ? PCT_RELATIVE_RIGHT       : 0)
   |(m_list2RelativeBottom        ? RELATIVE_BOTTOM          : 0)
   |(m_list2PctRelativeBottom     ? PCT_RELATIVE_BOTTOM      : 0)
   |(m_list2FontRelativeXPosition ? FONT_RELATIVE_LEFT       : 0)
   |(m_list2FontRelativeYPosition ? FONT_RELATIVE_TOP        : 0)
   |(m_list2ConstantWidth         ? CONSTANT_WIDTH           : 0)
   |(m_list2ConstantHeight        ? CONSTANT_HEIGHT          : 0)
   |(m_list2InitListheaders       ? INIT_LISTHEADERS         : 0)
   |(m_list2ResizeListheaders     ? RESIZE_LISTHEADERS       : 0)
   ;
}

int CTestLayoutManagerDlg::getButtonFlags() const {
  return
    (m_buttonResizeFont           ? RESIZE_FONT              : 0)
   |(m_buttonFontRelativeSize     ? FONT_RELATIVE_SIZE       : 0)
   |(m_buttonFontRelativeXPosition? FONT_RELATIVE_LEFT       : 0)
   |(m_buttonFontRelativeYPosition? FONT_RELATIVE_TOP        : 0)
   |(m_buttonRelativeLeft         ? RELATIVE_LEFT            : 0)
   |(m_buttonPctRelativeLeft      ? PCT_RELATIVE_LEFT        : 0)
   |(m_buttonPctRelativeXCenter   ? PCT_RELATIVE_X_CENTER    : 0)
   |(m_buttonRelativeTop          ? RELATIVE_TOP             : 0)
   |(m_buttonPctRelativeTop       ? PCT_RELATIVE_TOP         : 0)
   |(m_buttonPctRelativeYCenter   ? PCT_RELATIVE_Y_CENTER    : 0)
   |(m_buttonRelativeRight        ? RELATIVE_RIGHT           : 0)
   |(m_buttonPctRelativeRight     ? PCT_RELATIVE_RIGHT       : 0)
   |(m_buttonRelativeBottom       ? RELATIVE_BOTTOM          : 0)
   |(m_buttonPctRelativeBottom    ? PCT_RELATIVE_BOTTOM      : 0)
   |(m_buttonConstantWidth        ? CONSTANT_WIDTH           : 0)
   |(m_buttonConstantHeight       ? CONSTANT_HEIGHT          : 0)
   ;
}

void CTestLayoutManagerDlg::OnButtonOpenDialog() {
  UpdateData();
  int windowFlags = getWindowFlags();
  int list1Flags  = getList1Flags();
  int list2Flags  = getlist2Flags();
  int buttonFlags = getButtonFlags();

  CRunLayoutManagerDlg dlg(windowFlags, list1Flags, list2Flags, buttonFlags);
  dlg.DoModal();
}

void CTestLayoutManagerDlg::OnButtonSetToDefault() {
  setToDefault();
  UpdateData(FALSE);
}
