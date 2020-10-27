#include "stdafx.h"
#include <MyUtil.h>
#include "GetResourcesDlg.h"

#if defined(_DEBUG)
#define new DEBUG_NEW
#endif

class CAboutDlg : public CDialog {
public:
    CAboutDlg();
    enum { IDD = IDD_ABOUTBOX };

protected:
    virtual void DoDataExchange(CDataExchange *pDX);
    DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD) {
}

void CAboutDlg::DoDataExchange(CDataExchange *pDX) {
    __super::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
END_MESSAGE_MAP()

CGetresourcesDlg::CGetresourcesDlg(CWnd *pParent) : CDialog(CGetresourcesDlg::IDD, pParent) {
  m_hIcon = theApp.LoadIcon(IDR_MAINFRAME);
}

void CGetresourcesDlg::DoDataExchange(CDataExchange *pDX) {
  __super::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CGetresourcesDlg, CDialog)
    ON_WM_SYSCOMMAND()
    ON_WM_PAINT()
    ON_WM_QUERYDRAGICON()
    ON_COMMAND(ID_FILE_EXIT, OnFileExit)
    ON_COMMAND(ID_HELP_ABOUTGETRESOURCES, OnHelpAboutGetResources)
    ON_COMMAND(ID_FILE_OPENMODULE, OnFileOpenmodule)
END_MESSAGE_MAP()

BOOL CGetresourcesDlg::OnInitDialog() {
  __super::OnInitDialog();

  ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
  ASSERT(IDM_ABOUTBOX < 0xF000);

  CMenu *pSysMenu = GetSystemMenu(FALSE);
  if(pSysMenu != nullptr) {
    CString strAboutMenu;
    strAboutMenu.LoadString(IDS_ABOUTBOX);
    if (!strAboutMenu.IsEmpty()) {
      pSysMenu->AppendMenu(MF_SEPARATOR);
      pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
    }
  }

  SetIcon(m_hIcon, TRUE);         // Set big icon
  SetIcon(m_hIcon, FALSE);        // Set small icon

  m_accelTable = LoadAccelerators(theApp.m_hInstance,MAKEINTRESOURCE(IDR_ACCELERATOR1));

  return TRUE;  // return TRUE  unless you set the focus to a control
}

void CGetresourcesDlg::OnSysCommand(UINT nID, LPARAM lParam) {
  if((nID & 0xFFF0) == IDM_ABOUTBOX) {
    CAboutDlg().DoModal();
  } else {
    __super::OnSysCommand(nID, lParam);
  }
}

void CGetresourcesDlg::OnPaint() {
  if(IsIconic()) {
    CPaintDC dc(this); // device context for painting

    SendMessage(WM_ICONERASEBKGND, (WPARAM)dc.GetSafeHdc(), 0);

    int cxIcon = GetSystemMetrics(SM_CXICON);
    int cyIcon = GetSystemMetrics(SM_CYICON);
    CRect rect;
    GetClientRect(&rect);
    int x = (rect.Width() - cxIcon + 1) / 2;
    int y = (rect.Height() - cyIcon + 1) / 2;

    dc.DrawIcon(x, y, m_hIcon);
  } else {
    __super::OnPaint();
  }
}

HCURSOR CGetresourcesDlg::OnQueryDragIcon() {
  return (HCURSOR)m_hIcon;
}

BOOL CGetresourcesDlg::PreTranslateMessage(MSG *pMsg) {
  if(TranslateAccelerator(m_hWnd,m_accelTable,pMsg)) {
    return TRUE;
  }
  return __super::PreTranslateMessage(pMsg);
}

void CGetresourcesDlg::OnFileExit() {
  exit(0);
}

void CGetresourcesDlg::OnHelpAboutGetResources() {
  CAboutDlg().DoModal();
}

//#define IS_INTRESOURCE(_r) ((((unsigned long)(_r)) >> 16) == 0)

String encodeResourceString(LPCTSTR resourceType) {
  return IS_INTRESOURCE(resourceType) ? format(_T("(%d)"),resourceType) : resourceType;
}

const TCHAR *decodeResourceString(const String &str) {
  if(str[0] == '(') {
    int n = _ttoi(str.cstr()+1);
    return MAKEINTRESOURCE(n);
  } else {
    return str.cstr();
  }
}

static BOOL CALLBACK receiveResourceType(HMODULE module, LPTSTR resourceType, LONG_PTR param) {
  StringArray *stringArray = (StringArray*)param;
  stringArray->add(encodeResourceString(resourceType));
  return TRUE;
}

StringArray getResourceTypes(HMODULE module) {
  StringArray result;
  if(EnumResourceTypes(module, receiveResourceType, (LONG_PTR)&result) == 0) {
    throwException(_T("%s failed. %s"), __TFUNCTION__, getLastErrorText().cstr());
  }
  return result;
}

class ResourceName {
private:
  HMODULE m_module;
  String  m_resourceType;
  String  m_resourceName;
  friend class ResourceImage;
public:
  ResourceName(HMODULE module, LPCTSTR resourceType, LPTSTR resourceName);
  bool   isBitmap()     const;
  bool   isIcon()       const;
  bool   isCursor()     const;
  bool   isString()     const;
  bool   isImage()      const;
  int    getImageType() const;
  String getString()    const;
};

ResourceName::ResourceName(HMODULE module, LPCTSTR resourceType, LPTSTR resourceName) {
  m_module       = module;
  m_resourceType = encodeResourceString(resourceType);
  m_resourceName = encodeResourceString(resourceName);
}

bool ResourceName::isBitmap() const {
  return decodeResourceString(m_resourceType) == RT_BITMAP;
}

bool ResourceName::isIcon()   const {
  return decodeResourceString(m_resourceType) == RT_ICON;
}

bool ResourceName::isCursor() const {
  return decodeResourceString(m_resourceType) == RT_CURSOR;
}

int ResourceName::getImageType() const {
  if(isBitmap()) {
    return IMAGE_BITMAP;
  } else if(isIcon()) {
    return IMAGE_ICON;
  } else if(isCursor()) {
    return IMAGE_CURSOR;
  } else {
    throwException(_T("Resource <%s,%s> is not an image"), m_resourceType.cstr(),m_resourceName.cstr());
    return -1;
  }
}


bool ResourceName::isString() const {
  return decodeResourceString(m_resourceType) == RT_STRING;
}

bool ResourceName::isImage() const {
  return isBitmap() || isCursor() || isIcon();
}

#pragma warning(disable:4311 4302)

String ResourceName::getString() const {
  UINT id = (UINT)decodeResourceString(m_resourceName);
  TCHAR buffer[16000];
  if(LoadString(m_module,id, buffer, ARRAYSIZE(buffer)) == 0) {
    return format(_T("String(%s) not found. LoadString failed. %s"), m_resourceName.cstr(), getLastErrorText().cstr());
  }
  return buffer;
}

typedef Array<ResourceName> ResourceNameArray;

static BOOL CALLBACK receieveResourceName(HMODULE module, LPCTSTR resourceType, LPTSTR resourceName, LONG_PTR param) {
  ResourceNameArray *nameArray = (ResourceNameArray*)param;
  nameArray->add(ResourceName(module, resourceType, resourceName));
  return TRUE;
}

ResourceNameArray getResourceNames(HMODULE module) {
  StringArray types = getResourceTypes(module);
  ResourceNameArray result;
  for(size_t i = 0; i < types.size(); i++) {
    const String &rt = types[i];
    if(EnumResourceNames(module,decodeResourceString(types[i]),receieveResourceName, (LONG_PTR)&result) == 0) {
      throwException(_T("%s(type=%s) failed. %s"), __TFUNCTION__, rt.cstr(),getLastErrorText().cstr());
    }
  }
  return result;
}

class ResourceImage {
public:
  HANDLE m_image;
  int    m_type;

  ResourceImage(const ResourceName &rn);
 ~ResourceImage();
  CSize getSize() const;
  void draw(CDC &dc, const CPoint &p);
};

ResourceImage::ResourceImage(const ResourceName &rn) {
  m_type = rn.getImageType();
  m_image = ::LoadImage(rn.m_module
                       ,decodeResourceString(rn.m_resourceName)
                       ,m_type
                       ,0,0
                       ,LR_VGACOLOR);

  if(m_image == nullptr) {
    throwException(_T("ResourceImage cannot be constructed. LoadImage failed. %s"), getLastErrorText().cstr());
  }
}

ResourceImage::~ResourceImage() {
  if(m_image == nullptr) {
    return;
  }
  switch(m_type) {
  case IMAGE_BITMAP: DeleteObject(          m_image); break;
  case IMAGE_ICON  : DestroyIcon(  (HICON)  m_image); break;
  case IMAGE_CURSOR: DestroyCursor((HCURSOR)m_image); break;
  }
}

CSize ResourceImage::getSize() const {
  DEFINEMETHODNAME;
  switch(m_type) {
  case IMAGE_BITMAP:
    { BITMAP info;
      if(GetObject(m_image,sizeof(info),&info) == 0) {
        throwException(_T("%s:GetObject failed. %s"), method, getLastErrorText().cstr());
      }
      return CSize(info.bmWidth, info.bmHeight);
    }
    break;
  case IMAGE_ICON  :
  case IMAGE_CURSOR:
    { ICONINFO iconInfo;
      if(GetIconInfo((HICON)m_image,&iconInfo) == 0) {
        throwException(_T("%s:GetIconInfo failed. %s"), method, getLastErrorText().cstr());
      }
      BITMAP bmInfo;
      if(GetObject(iconInfo.hbmColor,sizeof(bmInfo),&bmInfo) == 0) {
        throwException(_T("%s:GetObject failed. %s"), method, getLastErrorText().cstr());
      }

      return CSize(bmInfo.bmWidth, bmInfo.bmHeight);
    }
    break;
  default:
    return CSize(0,0);
  }
}

void ResourceImage::draw(CDC &dc, const CPoint &p) {
  switch(m_type) {
  case IMAGE_BITMAP:
    { CDC dc1;
      dc1.CreateCompatibleDC(&dc);
      SelectObject(dc1.m_hDC,m_image);
      CSize size = getSize();
      dc.BitBlt(p.x,p.y,size.cx,size.cy, &dc1,0,0,SRCCOPY);
    }
    break;
  case IMAGE_ICON  :
  case IMAGE_CURSOR:
    DrawIcon(dc.m_hDC,p.x,p.y,(HICON)m_image);
    break;
  }
}

void CGetresourcesDlg::OnFileOpenmodule() {
  static const TCHAR *FileDialogExtensions = _T("Dll-Files (*.dll)\0*.dll;\0"
                                                "All files (*.*)\0*.*\0\0");
  CFileDialog dlg(TRUE);
  dlg.m_ofn.lpstrFilter = FileDialogExtensions;
  dlg.m_ofn.lpstrTitle  = _T("Open file");

  if(dlg.DoModal() != IDOK || _tcslen(dlg.m_ofn.lpstrFile) == 0) {
    return;
  }

  HMODULE module = LoadLibrary(dlg.m_ofn.lpstrFile);
  if(module == nullptr) {
    showWarning(getLastErrorText());
    return;
  }

  try {
    CWnd *frame = GetDlgItem(IDC_IMAGEFRAME);
    CClientDC dc(frame);
    CRect cr;
    frame->GetClientRect(&cr);
    CSize clientSize = cr.Size();
    dc.FillSolidRect(0,0,clientSize.cx,clientSize.cy,RGB(255,255,255));

    CPoint pos(0,0);
    int maxHeight = 0;
    ResourceNameArray ressourceArray = getResourceNames(module);
    for(size_t i = 0; i < ressourceArray.size(); i++) {
      const ResourceName &rn = ressourceArray[i];
      if(rn.isBitmap()) {
        continue;
        ResourceImage image(rn);
        CSize imageSize = image.getSize();
        if(pos.x + imageSize.cx > clientSize.cx) {
          pos.x = 0;
          pos.y += maxHeight + 5;
          maxHeight = 0;
        }
        image.draw(dc,pos);
        maxHeight = max(imageSize.cy,maxHeight);
        pos.x += imageSize.cx + 5;
      } else if(rn.isString()) {
        String s = rn.getString();
        if(pos.x > 0) {
          pos.x = 0;
          pos.y += maxHeight;
        }
        textOut(dc, pos, s);
        const CSize textSize = getTextExtent(dc, s);
        maxHeight = textSize.cy;
        pos.y += maxHeight;
      }
    }
  } catch(Exception e) {
    showException(e);
  }
  FreeLibrary(module);
}
