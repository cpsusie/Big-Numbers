#include "stdafx.h"
#include "TestConfirmDlg.h"

#if defined(_DEBUG)
#define new DEBUG_NEW
#endif

CTestConfirmDlg::CTestConfirmDlg(CWnd *pParent /*=nullptr*/) : CDialog(CTestConfirmDlg::IDD, pParent) {
    m_confirmMessage    = _T("Confirm");
    m_confirmCaption    = _T("ConfirmBox");
}

void CTestConfirmDlg::DoDataExchange(CDataExchange *pDX) {
    __super::DoDataExchange(pDX);
    DDX_Text(pDX, IDC_EDIT_CONFIRMMESSAGE      , m_confirmMessage);
    DDX_Text(pDX, IDC_EDIT_CONFIRMCAPTION      , m_confirmCaption);
}


BEGIN_MESSAGE_MAP(CTestConfirmDlg, CDialog)
    ON_BN_CLICKED(IDC_BUTTON_TESTCONFIRM           , OnButtonTestConfirm           )
    ON_BN_CLICKED(IDC_BUTTON_SHOWSTANDARDMESSAGEBOX, OnButtonShowStandardMessageBox)
END_MESSAGE_MAP()

BOOL CTestConfirmDlg::OnInitDialog() {
    __super::OnInitDialog();
    initComboItems();
    getConfirmButtonCombo()->SetCurSel(0);
    getConfirmIconCombo()->SetCurSel(0);
    return TRUE;
}

typedef struct {
  int          m_id;
  const TCHAR *m_name;
} ComboItem;

#define CBITEM(id) id, _T(#id)

typedef ComboItem EnumItem;

#define ENUMITEM(id) id, _T(#id)

static const ComboItem buttonItems[] = {
  CBITEM(MB_OK)
 ,CBITEM(MB_OKCANCEL)
 ,CBITEM(MB_ABORTRETRYIGNORE)
 ,CBITEM(MB_YESNOCANCEL)
 ,CBITEM(MB_YESNO)
 ,CBITEM(MB_RETRYCANCEL)
 ,CBITEM(MB_CANCELTRYCONTINUE)
};

static int findComboItemId(CComboBox *cb, const ComboItem *items, int n) {
  const String s = getWindowText(cb);
  for(int i = 0; i < n; i++) {
    if(items[i].m_name == s) {
      return items[i].m_id;
    }
  }
  return 0;
}

static const TCHAR *findEnumName(int id, const EnumItem *items, int n) {
  for(int i = 0; i < n; i++) {
    if(items[i].m_id == id) {
      return items[i].m_name;
    }
  }
  return _T("?");
};

static const ComboItem iconItems[] = {
  0, _T("No icon")
 ,CBITEM(MB_ICONINFORMATION)
 ,CBITEM(MB_ICONQUESTION   )
 ,CBITEM(MB_ICONWARNING    )
 ,CBITEM(MB_ICONERROR      )
};

static const EnumItem replyItems[] = {
  ENUMITEM(IDOK      )
 ,ENUMITEM(IDCANCEL  )
 ,ENUMITEM(IDABORT   )
 ,ENUMITEM(IDRETRY   )
 ,ENUMITEM(IDIGNORE  )
 ,ENUMITEM(IDYES     )
 ,ENUMITEM(IDNO      )
 ,ENUMITEM(IDCLOSE   )
 ,ENUMITEM(IDHELP    )
 ,ENUMITEM(IDTRYAGAIN)
 ,ENUMITEM(IDCONTINUE)
};

void CTestConfirmDlg::initComboItems() {
  CComboBox *cb = getConfirmButtonCombo();
  for (int i = 0; i < ARRAYSIZE(buttonItems); i++) {
    cb->AddString(buttonItems[i].m_name);
  }
  cb = getConfirmIconCombo();
  for (int i = 0; i < ARRAYSIZE(iconItems); i++) {
    cb->AddString(iconItems[i].m_name);
  }
}

int CTestConfirmDlg::getSelectedConfirmButtons() {
  return findComboItemId(getConfirmButtonCombo(), buttonItems, ARRAYSIZE(buttonItems));
}

int CTestConfirmDlg::getSelectedConfirmIcon() {
  return findComboItemId(getConfirmIconCombo(), iconItems, ARRAYSIZE(iconItems));
}

void CTestConfirmDlg::OnButtonTestConfirm() {
  UpdateData();

  bool showAgain;
  int ret = confirmDialogBox( (LPCTSTR)m_confirmMessage, (LPCTSTR)m_confirmCaption, showAgain, getSelectedConfirmButtons() | getSelectedConfirmIcon());

  MessageBox(format(_T("confirmDialogBox return %s. showAgain:%s")
                   ,findEnumName(ret, replyItems, ARRAYSIZE(replyItems))
                   ,boolToStr(showAgain)
                   ).cstr()
            ,_T("Message"));
}


void CTestConfirmDlg::OnButtonShowStandardMessageBox() {
  UpdateData();

  int ret = MessageBox((LPCTSTR)m_confirmMessage, (LPCTSTR)m_confirmCaption, getSelectedConfirmButtons() | getSelectedConfirmIcon());

  MessageBox(format(_T("MessageBox return %s.")
                   ,findEnumName(ret, replyItems, ARRAYSIZE(replyItems))
                   ).cstr()
            ,_T("Result"));
}
