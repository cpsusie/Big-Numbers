//-----------------------------------------------------------------------------
// Picture (Implementations) Version 1.00
//
// Routins 4 Showing Picture Files... (.BMP .DIB .EMF .GIF .ICO .JPG .WMF)
//
// Author: Dr. Yovav Gad, EMail: Sources@SuperMain.com ,Web: www.SuperMain.com
//=============================================================================
//
// Full Story:
// ~~~~~~~~~~~
// There R Many Libraries To Handle Image Files, Anyway Most Of Them Do Not
// Include Source Files Or Just Very Complicated To Implement / Understand,
//
// After Many Days Of Searching (And Not Finding) a Way To Load a JPG From a
// Resource And Show It On a *Dialog Based* Application, I Decided 2 Take Steps
//
// So I Created What I Call a Very *Simple* & Useful Class,
// It Can Easily Implemented By Adding It To a Project, And U Do Not Have To
// Be a Real JPEG Freak - And Invent All Header Reading From The Beginning
// (It Uses The IPicture Interface - Same Way As Internet Explorer Does)
//
// I Would Like To Thank Mr.Peter Hendrix For His Wonderful Work
// That I Found On: http://www.thecodeproject.com/bitmap/cpicture.asp
// Which I Was Inspired And Got The IPicture Interface Idea From...
//
// Guess U Will Find It Useful,
// Appreciate If U Can Mention My Name On Your Final Code,
// Please Feel Free To Send Me Any Improvements Or SaveAsJPG() Functions:
//
// Author: Dr. Yovav Gad, EMail: Sources@SuperMain.com ,Web: www.SuperMain.com
//
//--------------------------Example & Usage 4 Dummies--------------------------
//
//  CPicture m_Picture;  // Create a Picture Object (An Instance Of This Class)
//  #include "Picture.h" // Make Sure U Include This Where U Gonna Create The Object...
//
//  Load Picture Data Into The IPicture Interface (.BMP .DIB .EMF .GIF .ICO .JPG .WMF)
//  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//	m_Picture.Load("Test.JPG"); // Load From a File - Just Load It (Show Later)
//	m_Picture.Load(IDR_TEST, "JPG"); // Load From a Resource - Just Load It (Show Later)
//
//	m_Picture.UpdateSizeOnDC(&dc); // Get Picture Dimenions In Pixels
//	m_Picture.Show(&dc, CPoint(0,0), CPoint(m_Picture.m_width, m_Picture.m_height), 0,0);
//	m_Picture.Show(&dc, CRect(0,0,100,100)); // Change Original Dimenions
//	m_Picture.ShowBitmapResource(&dc, IDB_TEST, CPoint(0,0)); // Show Bitmap Resource
//
//  OR When Using a Pointer On a "Regular" MFC Application (CDC* pDC)
//  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//	m_Picture.UpdateSizeOnDC(pDC); // Get Picture Dimenions In Pixels
//	m_Picture.Show(pDC, CPoint(0,0), CPoint(m_Picture.m_width, m_Picture.m_height), 0,0);
//	m_Picture.Show(pDC, CRect(0,0,100,100)); // Change Original Dimenions
//	m_Picture.ShowBitmapResource(pDC, IDB_TEST, CPoint(0,0)); // Show Bitmap Resource
//

#include "pch.h"
#include <MyUtil.h>
#include <FileNameSplitter.h>
#include <FileContent.h>
#include <TinyBitSet.h>
#include <wtypes.h>
#include <crtdbg.h>
#include <comdef.h>
#include <atlconv.h>
#include <fcntl.h>
#include <MFCUtil/Picture.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#define HIMETRIC_INCH 2540

CPicture::CPicture() {
  initPictureData();
}

CPicture::CPicture(const CPicture &src) {
  initPictureData();
  if(src.isLoaded()) {
    copyPictureData(src);
  }
}

CPicture::CPicture(HBITMAP bm) {
  initPictureData();
  createPictureFromBitmap(cloneBitmap(bm));
}

CPicture::~CPicture() {
  freePictureData();
}

CPicture &CPicture::operator=(const CPicture &src) {
  if(&src == this) {
    return *this;
  }
  freePictureData();
  copyPictureData(src);
  return *this;
}

void CPicture::initPictureData() {
  m_IPicture = NULL;
  m_size     = CSize(0,0);
  m_weight   = 0;
  m_hasAlpha = false;
}

void CPicture::copyPictureData(const CPicture &src) {
  if(src.isLoaded()) {
    m_size     = src.m_size;
    m_weight   = src.m_weight;
    m_IPicture = src.m_IPicture;
    m_hasAlpha = src.m_hasAlpha;
    SAFEADDREF(m_IPicture);
  }
}

void CPicture::freePictureData() {
  if(m_IPicture != NULL) {
    SAFERELEASE(m_IPicture);
    initPictureData();
  }
}

//-----------------------------------------------------------------------------
// does:   open a resource and load it into ipicture (interface)
// ~~~~    (.bmp .dib .emf .gif .ico .jpg .wmf)
//
// note:   when adding a bitmap resource it would automatically show on "bitmap"
// ~~~~    this not good because we need to load it from a custom resource "bmp"
//         to add a custom rresource: import resource -> open as -> custom
//         (both .bmp and .dib should be found under "bmp")
//
// input:  resId    - as a uint defined (example: idr_picture_resource)
// ~~~~~   typeName - type name (example: "jpg")
//
//-----------------------------------------------------------------------------
CPicture &CPicture::loadFromResource(int resId, const String &typeName) {
  ByteArray ba;
  ba.loadFromResource(resId, typeName.cstr());
  load(ba);
  return *this;
}

CPicture &CPicture::load(ByteInputStream &in) {
  FileContent content(in);
  load(content);
  return *this;
}

typedef enum {
  RM_PICTURE
 ,RM_ICON
 ,RM_CURSOR
 ,RM_PNG
 ,RM_TIFF
} ReadMethod;

typedef struct {
  const char       *m_extension;
  PictureFormatType m_type;
  ReadMethod        m_readMethod;
} ExtensionType;

//#define EXT(type, rm) #type, PFT_##type, RM_##rm

static const ExtensionType extensionTable[] = {
  ""    , PFT_UNKNOWN , RM_PICTURE
 ,"BMP" , PFT_BMP     , RM_PICTURE
 ,"DIB" , PFT_DIB     , RM_PICTURE
 ,"GIF" , PFT_GIF     , RM_PICTURE
 ,"JPG" , PFT_JPG     , RM_PICTURE
 ,"JPEG", PFT_JPG     , RM_PICTURE
 ,"WMF" , PFT_WMF     , RM_PICTURE
 ,"ICO" , PFT_ICO     , RM_ICON
 ,"CUR" , PFT_CUR     , RM_CURSOR
 ,"PNG" , PFT_PNG     , RM_PNG
 ,"TIF" , PFT_TIFF    , RM_TIFF
 ,"TIFF", PFT_TIFF    , RM_TIFF
};

static int getExtensionTypeIndexFromFileName(const String &fileName) {
  String ext = FileNameSplitter(fileName).getExtension();
  if((ext.length() > 0) && (ext[0] == '.')) {
    ext = substr(ext,1,ext.length());
  }
  ext = toUpperCase(ext);
  for(int i = 1; i < ARRAYSIZE(extensionTable); i++) {
    const ExtensionType &et = extensionTable[i];
    if(et.m_extension == ext) {
      return i;
    }
  }
  return 0;
}

PictureFormatType CPicture::getFormatTypeFromFileName(const String &fileName) { // static
  return extensionTable[getExtensionTypeIndexFromFileName(fileName)].m_type;
}

// open a file and load it into ipicture (interface)
// (.bmp .dib .emf .gif .ico .jpg, .png, .tiff .wmf)
CPicture &CPicture::load(const String &fileName) {
  FileContent content;
  bool fileContentRead = false;
  BitSet16 readMethodsTried;
  int index = getExtensionTypeIndexFromFileName(fileName);
  String firstErrorMessage;
  for(int count = 0; count < ARRAYSIZE(extensionTable); count++, index = (index+1) % ARRAYSIZE(extensionTable)) {
    const ExtensionType &et = extensionTable[index];
    const ReadMethod     rm = et.m_readMethod;
    if(readMethodsTried.contains(rm)) {
      continue;
    }
    readMethodsTried.add(rm);
    try {
      switch(rm) {
      case RM_ICON   :
        loadAsIcon(fileName);
        break;
      case RM_CURSOR :
        loadAsCursor(fileName);
        break;
      case RM_PICTURE:
      case RM_PNG    :
      case RM_TIFF   :
        if(!fileContentRead) {
          fileContentRead = true;
          content.load(fileName);
        }
        loadAsFormat(content, et.m_type);
        break;
      }
      if(isLoaded()) {
        return *this;
      }
    } catch(Exception e) {
      if(firstErrorMessage.length() == 0) {
        firstErrorMessage = e.what();
      }
    }
  }
  throwException(firstErrorMessage);
  return *this;
}

// Cannot handle icon or cursor
void CPicture::load(const ByteArray &bytes, int firstIndex) {
  BitSet16 readMethodsTried;
  String firstErrorMessage;
  int index = (firstIndex >= 0) ? firstIndex : 0;
  for(int count = 0; count < ARRAYSIZE(extensionTable); count++, index = (index+1) % ARRAYSIZE(extensionTable)) {
    const ExtensionType &et = extensionTable[index];
    const ReadMethod     rm = et.m_readMethod;
    if(readMethodsTried.contains(rm)) {
      continue;
    }
    readMethodsTried.add(rm);
    try {
      switch(rm) {
      case RM_ICON   :
      case RM_CURSOR :
        break;
      case RM_PICTURE:
      case RM_PNG    :
      case RM_TIFF   :
        loadAsFormat(bytes, et.m_type);
        break;
      }
      if(isLoaded()) {
        return;
      }
    } catch(Exception e) {
      if(firstErrorMessage.length() == 0) {
        firstErrorMessage = e.what();
      }
    }
  }
  throwException(firstErrorMessage);
}

void CPicture::loadAsFormat(const ByteArray &bytes, PictureFormatType ft) {
  switch(ft) {
  case PFT_PNG    :
    { HBITMAP bm = decodeAsPNG(bytes, m_hasAlpha);
      if(bm) {
        try {
          createPictureFromBitmap(bm);
          m_weight = (int)bytes.size();
        } catch(...) {
          DeleteObject(bm);
          throw;
        }
      }
    }
    break;
  case PFT_TIFF   :
    { HBITMAP bm = decodeAsTIFF(bytes, m_hasAlpha);
      if(bm) {
        try {
          createPictureFromBitmap(bm);
          m_weight = (LONG)bytes.size();
        } catch(...) {
          DeleteObject(bm);
          throw;
        }
      }
    }
    break;
  case PFT_ICO    :
  case PFT_CUR    :
    throwException(_T("Icons and Cursors cannot be read from a bytearray"));
  default         :
    loadPicture(bytes);
    break;
  }
}

void CPicture::loadPicture(const ByteArray &bytes) {
  if(bytes.size() == 0) {
    throwException(_T("No data"));
  } else  {
    freePictureData();
    loadPictureData(bytes.getData(), (int)bytes.size());
    m_hasAlpha = false;
  }
}

void CPicture::loadAsIcon(const String &fileName) {
  HICON icon = NULL;
  try {
    icon = loadIcon(fileName);
    createPictureFromIcon(icon);
    m_weight   = STAT(fileName).st_size;
    m_hasAlpha = false;
  } catch(...) {
    if(icon) {
      DestroyIcon(icon);
    }
    throw;
  }
}

void CPicture::loadAsCursor(const String &fileName) {
  HICON icon = NULL;
  try {
    icon = loadCursor(fileName);
    createPictureFromIcon(icon);
    m_weight   = STAT(fileName).st_size;
    m_hasAlpha = false;
  } catch(...) {
    if(icon) {
      DestroyIcon(icon);
    }
    throw;
  }
}

HICON CPicture::loadIcon(const String &fileName) { // static
  HICON icon = (HICON)LoadImage(0, fileName.cstr(), IMAGE_ICON, 0, 0, LR_LOADFROMFILE);
  if(icon == NULL) {
    throwLastErrorOnSysCallException(_T("LoadImage"));
  }
  return icon;
}

HICON CPicture::loadCursor(const String &fileName) { // static
  HICON icon = (HICON)LoadImage(0, fileName.cstr(), IMAGE_CURSOR, 0, 0, LR_LOADFROMFILE);
  if(icon == NULL) {
    throwLastErrorOnSysCallException(_T("LoadImage"));
  }
  return icon;
}

void CPicture::createPictureFromBitmap(HBITMAP bitmap) {
//  DebugBitmap::showBitmap(bitmap);
  freePictureData();
  PICTDESC desc;
  desc.cbSizeofstruct = sizeof(PICTDESC);
  desc.picType        = PICTYPE_BITMAP;
  desc.bmp.hbitmap    = bitmap;
  desc.bmp.hpal       = NULL;
  HRESULT hr;
  if((hr = OleCreatePictureIndirect(&desc, IID_IPicture, true, (LPVOID*)&m_IPicture)) != S_OK) {
    throwLastErrorOnSysCallException(_T("OleCreatePictureIndirect"));
  } else if(m_IPicture != NULL) {
    TRACE_CREATE(m_IPicture);
    setSize();
  }
}

void CPicture::setSize() {
  OLE_XSIZE_HIMETRIC width;
  OLE_YSIZE_HIMETRIC height;
  m_IPicture->get_Width( &width);
  m_IPicture->get_Height(&height);
  m_size.cx = MulDiv(width,  96, HIMETRIC_INCH);
  m_size.cy = MulDiv(height, 96, HIMETRIC_INCH);
}

CPicture::operator HBITMAP() const {
  if(!isLoaded()) {
    return NULL;
  }
  HDC     screenDC = NULL;
  HBITMAP bm       = NULL;
  HDC     dc       = NULL;
  try {
    screenDC = getScreenDC();
    bm = CreateCompatibleBitmap(screenDC, m_size.cx, m_size.cy);
    if(bm == NULL) {
      throwLastErrorOnSysCallException(_T("CreateCompatibleBitmap"));
    }
    dc = CreateCompatibleDC(screenDC);
    if(dc == NULL) {
      throwLastErrorOnSysCallException(_T("CreateCompatibleDC"));
    }
    HGDIOBJ oldGDI = SelectObject(dc, bm);
    show(dc);
    SelectObject(dc, oldGDI);
    DeleteDC(dc);
    DeleteDC(screenDC);
    return bm;
  } catch(...) {
    if(screenDC) DeleteDC(screenDC);
    if(dc)       DeleteDC(dc);
    if(bm)       DeleteObject(bm);
    throw;
  }
}

void CPicture::createPictureFromIcon(HICON icon) {
  freePictureData();
  PICTDESC desc;
  desc.cbSizeofstruct = sizeof(PICTDESC);
  desc.picType        = PICTYPE_ICON;
  desc.icon.hicon     = icon;
  HRESULT hr;
  if((hr = OleCreatePictureIndirect(&desc, IID_IPicture, true, (LPVOID*)&m_IPicture)) != S_OK) {
    throwLastErrorOnSysCallException(_T("OleCreatePictureIndirect"));
  } else if(m_IPicture != NULL) {
    TRACE_CREATE(m_IPicture);
    setSize();
  }
}

void CPicture::unload() {
  freePictureData();
}

//-----------------------------------------------------------------------------
// does:   read the picture data from a source (file / resource)
// ~~~~    and load it into the current ipicture object in use
//
// input:  buffer of data source (file / resource) and its size
// ~~~~~
//
// output: feed the ipicture object with the picture data
// ~~~~~~  (use draw functions to show it on a device context)
//         true if succeeded...
//-----------------------------------------------------------------------------
void CPicture::loadPictureData(const BYTE *pBuffer, int size) {
  HGLOBAL hGlobal = GlobalAlloc(GMEM_MOVEABLE, size);

  if(hGlobal == NULL) {
    throwException(_T("GlobalAlloc failed"));
  }
  __assume(hGlobal);
  void *pData = GlobalLock(hGlobal);
  if(pData == NULL) {
    FreeResource(hGlobal);
    throwLastErrorOnSysCallException(_T("GlobalLock"));
  }
  __assume(pData);
  memcpy(pData, pBuffer, size);
  GlobalUnlock(hGlobal);

  IStream *pStream = NULL;

  String errormsg;
  HRESULT hr;
  bool ok = true;
  if((hr = CreateStreamOnHGlobal(hGlobal, true, &pStream)) != S_OK) {
    errormsg = getErrorText(hr);
    ok = false;
  } else {
    TRACE_CREATE(pStream);
    if((hr = OleLoadPicture(pStream, size, false, IID_IPicture, (LPVOID*)&m_IPicture)) != S_OK) {
      switch(hr) {
      case E_OUTOFMEMORY: errormsg = _T("Ran out of memory"          ); break;
      case E_UNEXPECTED : errormsg = _T("An internal error occurred" ); break;
      case E_POINTER    : errormsg = _T("Invalid pointer"            ); break;
      case E_NOINTERFACE: errormsg = _T("No such interface supported"); break;
      default           : errormsg = getErrorText(hr);                  break;
      }
      ok = false;
    } else if(m_IPicture != NULL) {
      m_weight = size;
      TRACE_CREATE(m_IPicture);
      setSize();
    }

    SAFERELEASE(pStream)
  }

  FreeResource(hGlobal); // 16Bit Windows Needs This (32Bit - Automatic Release)

  if(!ok) {
    throwException(_T("loadPictureData failed:%s"), errormsg.cstr());
  }
}

void CPicture::saveAsBitmap(const String &name) {
  if(!isLoaded()) {
    throwException(_T("No picture to save"));
  }
  USES_CONVERSION;
  String tmpName = name;
  TCHAR *fname = tmpName.cstr();
  IDispatch *ptr;
  m_IPicture->QueryInterface(IID_IDispatch,(void**)&ptr);
  OleSavePictureFile(ptr, T2OLE(fname));
}


void CPicture::show(HDC hdc) const {
  show(hdc, getRectangle());
}

void CPicture::show(HDC hdc, const CPoint &p) {
  show(hdc, CRect(p,getSize()));
}

void CPicture::show(HDC hdc, const CRect &dstRect) const {
  show(hdc, dstRect, getRectangle());
}

void CPicture::show(HDC dc, const CRect &dstRect, const CRect &srcRect) const {
  if(!isLoaded()) {
    throwException(_T("No picture"));
  }
  if(!hasAlpha()) {
    render(dc, dstRect, srcRect);
  } else {
    const int blendCaps = GetDeviceCaps(dc, SHADEBLENDCAPS);
    switch(blendCaps) {
    case SB_NONE       :
      { const CSize &srcsz = srcRect.Size();
        const CSize  dstsz = dstRect.Size();
        HBITMAP picBM   = CreateBitmap(srcsz.cx, srcsz.cy,1,32,NULL);
        HBITMAP tmpBM   = CreateBitmap(dstsz.cx, dstsz.cy,1,32,NULL);
        HDC     picDC   = CreateCompatibleDC(NULL);
        HDC     tmpDC   = CreateCompatibleDC(NULL);
        HGDIOBJ picOld  = SelectObject(picDC,picBM);
        HGDIOBJ tmpOld  = SelectObject(tmpDC,tmpBM); // to hold original dst-rect
        try {
          BitBlt(tmpDC, 0,0,dstsz.cx,dstsz.cy,dc, dstRect.left,dstRect.top,SRCCOPY);
          render(picDC, CRect(0,0,srcsz.cx,srcsz.cy), srcRect);
          preMultiplyAlpha(picBM);
          alphaBlend(tmpDC, 0,0,dstsz.cx,dstsz.cy,picDC, 0,0,srcsz.cx,srcsz.cy,255);
          BitBlt(dc, dstRect.left,dstRect.top, dstsz.cx,dstsz.cy,tmpDC, 0,0,SRCCOPY);
          SelectObject(picDC,picOld); DeleteDC(picDC); DeleteObject(picBM);
          SelectObject(tmpDC,tmpOld); DeleteDC(tmpDC); DeleteObject(tmpBM);
        } catch(...) {
          SelectObject(picDC,picOld); DeleteDC(picDC); DeleteObject(picBM);
          SelectObject(tmpDC,tmpOld); DeleteDC(tmpDC); DeleteObject(tmpBM);
          throw;
        }
      }
      break;
    case SB_CONST_ALPHA:
    case SB_PIXEL_ALPHA:
      render(dc, dstRect, srcRect);
      break;
    }
  }
}

void CPicture::render(HDC dst, const CRect &dstRect, const CRect &srcRect) const {
  const int left   = MulDiv(srcRect.left    , HIMETRIC_INCH, 96);
  const int bottom = MulDiv(srcRect.bottom  , HIMETRIC_INCH, 96);
  const int width  = MulDiv(srcRect.Width() , HIMETRIC_INCH, 96);
  const int height = MulDiv(srcRect.Height(), HIMETRIC_INCH, 96);

  const HRESULT hr = m_IPicture->Render(dst
                                       ,dstRect.left
                                       ,dstRect.top
                                       ,dstRect.Width()
                                       ,dstRect.Height()
                                       ,left
                                       ,bottom
                                       ,width
                                       ,-height
                                       ,&dstRect);

  if(FAILED(hr)) {
    throwException(_T("%s:%s"), __TFUNCTION__, getErrorText(hr).cstr());
  }
}

void CPicture::showBitmapResource(HDC hdc, int resId, const CPoint &p) { // static
  if(hdc == NULL) {
    throwInvalidArgumentException(__TFUNCTION__, _T("hdc = NULL"));
  }

  CBitmap bm;
  if(!bm.LoadBitmap(resId)) {
	  throwException(_T("Cannot find bitmap resource:%d"), resId);
  }
  const BITMAP bmInfo = getBitmapInfo(bm);
  HDC srcDC = NULL;
  try {
    srcDC = CreateCompatibleDC(hdc);
    if(srcDC == NULL) {
      throwLastErrorOnSysCallException(_T("CreateCompatibleDC"));
    }
    HGDIOBJ oldGDI = SelectObject(srcDC, bm);
    if(!BitBlt(hdc, p.x, p.y, bmInfo.bmWidth, bmInfo.bmHeight, srcDC, 0, 0, SRCCOPY)) {
      throwLastErrorOnSysCallException(_T("BitBlt"));
    }
    SelectObject(srcDC, oldGDI);
    DeleteDC(srcDC);
    srcDC = NULL;
  } catch(...) {
    if(srcDC) {
      DeleteDC(srcDC);
    }
    throw;
  }
}

void CPicture::updateSizeOnDC(CDC *pDC) {
  if(pDC == NULL) {
    throwInvalidArgumentException(__TFUNCTION__, _T("pDC == NULL"));
  }

  if(isLoaded()) {
    throwException(_T("No picture"));
  }

  OLE_XSIZE_HIMETRIC width;
  OLE_YSIZE_HIMETRIC height;
  m_IPicture->get_Width( &width);
  m_IPicture->get_Height(&height);

  // get current dpi - Dots Per Inch
  int currentDPI_X, currentDPI_Y;

  // use a "standard" print (when printing)
  if(pDC->IsPrinting()) {
    currentDPI_X = 96;
    currentDPI_Y = 96;
  } else {
    currentDPI_X = pDC->GetDeviceCaps(LOGPIXELSX);
    currentDPI_Y = pDC->GetDeviceCaps(LOGPIXELSY);
  }

  m_size.cx = MulDiv(width,  currentDPI_X, HIMETRIC_INCH);
  m_size.cy = MulDiv(height, currentDPI_Y, HIMETRIC_INCH);
}
