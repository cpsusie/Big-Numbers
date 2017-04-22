#include "stdafx.h"
#include "GrabXmlDlg.h"
#include "afxdialogex.h"
#include <Game.h>
#include <LDA.h>

#define CHESSLIBPATH "c:/mytools2015/spil/chesslib/"
#define _CHESSCONF_  "TablebaseBuilder" _CONFIGURATION_

#pragma comment(lib, CHESSLIBPATH _PLATFORM_ _CHESSCONF_ "Chesslib.lib")

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

class CAboutDlg : public CDialogEx {
public:
  CAboutDlg();

#ifdef AFX_DESIGN_TIME
  enum { IDD = IDD_ABOUTBOX };
#endif

protected:
  virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
  DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(IDD_ABOUTBOX) {
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX) {
  CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()

CGrabXmlDlg::CGrabXmlDlg(CWnd* pParent /*=NULL*/) : CDialogEx(IDD_GRABXML_DIALOG, pParent), m_url(EMPTYSTRING) {
  m_hIcon = theApp.LoadIcon(IDR_MAINFRAME);
}

void CGrabXmlDlg::DoDataExchange(CDataExchange* pDX) {
  CDialogEx::DoDataExchange(pDX);
  DDX_Control(pDX, IDC_EXPLORER1, m_browser);
  DDX_CBString(pDX, IDC_COMBOURL, m_url);
}

BEGIN_MESSAGE_MAP(CGrabXmlDlg, CDialogEx)
  ON_WM_SYSCOMMAND()
  ON_WM_QUERYDRAGICON()
  ON_WM_PAINT()
  ON_WM_SIZE()
  ON_WM_CLOSE()
  ON_BN_CLICKED(IDOK                , &CGrabXmlDlg::OnBnClickedOk        )
  ON_BN_CLICKED(IDCANCEL            , &CGrabXmlDlg::OnBnClickedCancel    )
  ON_BN_CLICKED(IDSEARCH            , &CGrabXmlDlg::OnBnClickedSearch    )
  ON_COMMAND(ID_FILE_EXIT           , &CGrabXmlDlg::OnFileExit           )
  ON_COMMAND(ID_TOOLS_FINDKNIGHTTOUR, &CGrabXmlDlg::OnToolsFindKnightTour)
  ON_WM_RBUTTONDBLCLK()
  ON_WM_TIMER()
END_MESSAGE_MAP()

BOOL CGrabXmlDlg::OnInitDialog() {
  CDialogEx::OnInitDialog();

  ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
  ASSERT(IDM_ABOUTBOX < 0xF000);

  CMenu* pSysMenu = GetSystemMenu(FALSE);
  if (pSysMenu != NULL) {
    BOOL bNameValid;
    CString strAboutMenu;
    bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
    ASSERT(bNameValid);
    if (!strAboutMenu.IsEmpty()) {
      pSysMenu->AppendMenu(MF_SEPARATOR);
      pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
    }
  }

  SetIcon(m_hIcon, TRUE);			// Set big icon
  SetIcon(m_hIcon, FALSE);		// Set small icon
  
  m_cbUrl.substituteControl(this, IDC_COMBOURL, _T("UrlHistory"));

  m_layoutManager.OnInitDialog(this);
  m_layoutManager.addControl(IDC_EXPLORER1, RELATIVE_SIZE);
  m_layoutManager.addControl(IDC_COMBOURL , RELATIVE_WIDTH);
  m_layoutManager.addControl(IDSEARCH     , RELATIVE_X_POS);

  m_accelTable  = LoadAccelerators(AfxGetApp()->m_hInstance,MAKEINTRESOURCE(IDR_MAINFRAME));

  m_cbUrl.SetFocus();
  m_currentDoc = NULL;

  return FALSE;
}

void CGrabXmlDlg::OnSysCommand(UINT nID, LPARAM lParam) {
  if ((nID & 0xFFF0) == IDM_ABOUTBOX) {
    CAboutDlg dlgAbout;
    dlgAbout.DoModal();
  } else {
    CDialogEx::OnSysCommand(nID, lParam);
  }
}

HCURSOR CGrabXmlDlg::OnQueryDragIcon() {
  return static_cast<HCURSOR>(m_hIcon);
}

void CGrabXmlDlg::OnPaint() {
  if (IsIconic()) {
    CPaintDC dc(this); // device context for painting

    SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

    // Center icon in client rectangle
    int cxIcon = GetSystemMetrics(SM_CXICON);
    int cyIcon = GetSystemMetrics(SM_CYICON);
    CRect rect;
    GetClientRect(&rect);
    int x = (rect.Width() - cxIcon + 1) / 2;
    int y = (rect.Height() - cyIcon + 1) / 2;

    // Draw the icon
    dc.DrawIcon(x, y, m_hIcon);
  } else {
    CDialogEx::OnPaint();
  }
}

void CGrabXmlDlg::OnSize(UINT nType, int cx, int cy) {
  CDialogEx::OnSize(nType, cx, cy);
  m_layoutManager.OnSize(nType, cx, cy);
}

void CGrabXmlDlg::OnBnClickedOk() {
}

void CGrabXmlDlg::OnBnClickedCancel() {
}

void CGrabXmlDlg::OnClose() {
  EndDialog(IDOK);
}

void CGrabXmlDlg::OnFileExit() {
  OnClose();
}

void CGrabXmlDlg::OnBnClickedSearch() {
  UpdateData();
  if(m_url.GetLength()) {
    m_currentDoc = NULL;
    m_browser.Navigate(m_url, NULL, NULL, NULL, NULL);
    m_cbUrl.updateList();
    m_cbUrl.save();
  }
}

void CGrabXmlDlg::OnToolsFindKnightTour() {
  MessageBox(_T("Find knight route"), _T("Message"));
}

BOOL CGrabXmlDlg::PreTranslateMessage(MSG* pMsg) {
  if(TranslateAccelerator(m_hWnd, m_accelTable, pMsg)) {
    return true;
  }
  switch (pMsg->message) {
  case WM_RBUTTONDBLCLK:
    OnRButtonDblClk(0,pMsg->pt);
    return true;
  case WM_KEYDOWN:
//    m_browser.SendMessage(pMsg->message, pMsg->wParam, pMsg->lParam);
    SetTimer(1, 300, NULL);
    break;
  }
  return CDialogEx::PreTranslateMessage(pMsg);
}

BEGIN_EVENTSINK_MAP(CGrabXmlDlg, CDialogEx)
  ON_EVENT(CGrabXmlDlg, IDC_EXPLORER1, 259, CGrabXmlDlg::DocumentCompleteExplorer1, VTS_DISPATCH VTS_PVARIANT)
END_EVENTSINK_MAP()


void CGrabXmlDlg::DocumentCompleteExplorer1(LPDISPATCH pDisp, VARIANT* URL) {
  m_currentDoc = (IHTMLDocument2*)m_browser.get_Document();
}

void CGrabXmlDlg::OnRButtonDblClk(UINT nFlags, CPoint point) {
  findRouteAndEnterSolution();
  CDialogEx::OnRButtonDblClk(nFlags, point);
}

void CGrabXmlDlg::findRouteAndEnterSolution() {
  if(!m_currentDoc) return;
  String str = getDocumentText(m_currentDoc);
  static const String label = _T("Route:");
  intptr_t index = str.find(label);
  String srcdst = substr(str, index+label.length()+1, 10);
  Tokenizer tok(srcdst, _T(" "));
  String fromStr = tok.next();
  tok.next();
  String toStr = tok.next();
  toStr = left(toStr,2);
  int    from  = decodePosition(fromStr);
  int    to    = decodePosition(toStr  );
  String route = findShortestKnightRoute(from, to);
  enterText(_T("routetext"), route);
}

#include <XMLDoc.h>

#define V(f) CHECKRESULT(f)

IHTMLInputElement *CGrabXmlDlg::findInputElementByName(const String &name) {
  IHTMLElementCollection *array  = NULL;
  IHTMLInputElement      *result = NULL;

  V(m_currentDoc->get_all(&array));
  
  IDispatch         *pElemDisp = NULL;
  IHTMLInputElement *pElem     = NULL;
  _bstr_t            bname     = name.cstr();
  _variant_t         varID(bname);
  _variant_t         varIdx(0);
  HRESULT            hr = array->item(varID, varIdx, &pElemDisp);

  if (SUCCEEDED(hr)) {
    hr = pElemDisp->QueryInterface(IID_IHTMLInputElement, (void**)&pElem);
    if (SUCCEEDED(hr)) {
      result = pElem;
    } else {
      pElem->Release();
    }
    pElemDisp->Release();
  }
  array->Release();
  return result;
}

String CGrabXmlDlg::getDocumentText(IHTMLDocument2 *doc) {
  String result;
  try {
	  IHTMLElementCollection *pColl;
	  V(doc->get_all( &pColl ));
    LONG celem;
	  V(pColl->get_length( &celem ));

    BOOL quit = FALSE;

    for(int i = 0; i < celem; i++) {
      if(quit) break;

      VARIANT varIndex;
      varIndex.vt   = VT_UINT;
      varIndex.lVal = i;
      VARIANT var2;
      VariantInit( &var2 );
      IDispatch *pDisp; 

      V(pColl->item( varIndex, var2, &pDisp ));

      IHTMLElement *pElem;
      V(pDisp->QueryInterface( IID_IHTMLElement, (void**)&pElem ));

      BSTR bstr;
      V(pElem->get_tagName(&bstr));
      CString strTag = bstr;

      IHTMLImgElement *pImgElem;
      if(pDisp->QueryInterface( IID_IHTMLImgElement, (void**)&pImgElem ) == S_OK) {
        pImgElem->get_href(&bstr);
        strTag += " - ";
        strTag += bstr;
        pImgElem->Release();
      } else {
        IHTMLAnchorElement* pAnchElem;
        if(pDisp->QueryInterface( IID_IHTMLAnchorElement, (void**)&pAnchElem ) == S_OK) {
          pAnchElem->get_href(&bstr);
          strTag += _T(" - ");
          strTag += bstr;
          BSTR name;
          pAnchElem->get_name(&name);
          if(name) {
            strTag += _T(" Name:");
            strTag += name;
          }
          pAnchElem->Release();
        }
      }
      V(pElem->get_innerText(&bstr));
      pElem->Release();
      result += (LPCTSTR)strTag;
      result += _T(" InnerText:<");
      result += BSTRToString(bstr);
      result += _T(">\n");
    }
    pColl->Release();
  }
  catch (Exception e) {
    result += format(_T("\nException:%s\n"), e.what());
  }
  return result;
}

void CGrabXmlDlg::enterText(const String &id, const String &text) {
  IHTMLInputElement *ie = findInputElementByName(id);
  if (ie) {
    const _bstr_t btext = text.cstr();
    ie->put_value(btext);
    ie->select();
    ie->Release();
    m_browser.Invalidate(FALSE);
  }
}

void CGrabXmlDlg::OnTimer(UINT_PTR nIDEvent) {
  CDialogEx::OnTimer(nIDEvent);
  KillTimer(1);
  findRouteAndEnterSolution();
}
