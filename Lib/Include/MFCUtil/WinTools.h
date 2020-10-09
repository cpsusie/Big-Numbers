#pragma once

#if !defined(VC_EXTRALEAN)
#define VC_EXTRALEAN        // Exclude rarely-used stuff from Windows headers
#endif

#define _CRT_SECURE_NO_WARNINGS 1
#define _ATL_CSTRING_EXPLICIT_CONSTRUCTORS      // some CString constructors will be explicit
#define _AFX_ALL_WARNINGS                       // turns off MFC's hiding of some common and often safely ignored warning messages

#define _AFX_NO_OLE_SUPPORT
#define _AFX_NO_DB_SUPPORT
#define _AFX_NO_DAO_SUPPORT

#define OEMRESOURCE

#include <afxwin.h>         // MFC core and standard components
#include <afxext.h>         // MFC extensions
#include <afxdisp.h>        // MFC Automation classes
#include <afxdtctl.h>       // MFC support for Internet Explorer 4 Common Controls
#include <afxcontrolbars.h> // MFC support for ribbons and control bars

#if !defined(_AFX_NO_DB_SUPPORT)
#include <afxdb.h>          // MFC ODBC database classes
#endif // _AFX_NO_DB_SUPPORT

#if !defined(_AFX_NO_DAO_SUPPORT)
#include <afxdao.h>         // MFC DAO database classes
#endif // _AFX_NO_DAO_SUPPORT

#if !defined(_AFX_NO_OLE_SUPPORT)
#include <afxdtctl.h>       // MFC support for Internet Explorer 4 Common Controls
#endif
#if !defined(_AFX_NO_AFXCMN_SUPPORT)
#include <afxcmn.h>         // MFC support for Windows Common Controls
#endif // _AFX_NO_AFXCMN_SUPPORT

#include <BasicIncludes.h>
#include <ByteArray.h>
#include <StringArray.h>
#include "LayoutManager.h"

// ex. if s == "0/2", this will enable/disable the third menuitem in first pulldown-menu of the windows menu
int    getMenuItemType(                HMENU menu,  UINT          pos);
HMENU  findMenuByString(               HMENU menu,  const String &s , int *index = NULL);  // see comment at enableMenuItem
HMENU  findMenuContainingId(           HMENU menu,  UINT          id, int *index = NULL);
void   enableMenuItem(                 HMENU menu,  UINT          id, bool enabled);
void   checkMenuItem(                  HMENU menu,  UINT          id, bool checked);
bool   enableSubMenuContainingId(      HMENU menu,  UINT          id, bool enabled); // do
bool   isMenuItemChecked(              HMENU menu,  UINT          id );
void   insertMenuItem(                 HMENU menu,  UINT          pos, const String &itemText, UINT         commandId);
void   appendMenuItem(                 HMENU menu,                     const String &itemText, UINT         commandId);
void   insertMenuSeparator(            HMENU menu,  UINT          pos);
void   appendMenuSeparator(            HMENU menu                    );
HMENU  insertSubMenu(                  HMENU menu,  UINT          pos, const String &text);
HMENU  appendSubMenu(                  HMENU menu,                     const String &text);
void   removeMenuItem(                 HMENU menu,  UINT          id );
bool   removeSubMenuContainingId(      HMENU menu,  UINT          id );
bool   menuItemExists(                 HMENU menu,  UINT          id );

String getMenuItemText(                HMENU menu,  int           pos);
// returns old itemtext
String setMenuItemText(                HMENU menu,  UINT          id , const String &itemText);
HMENU  getSubMenu(                     HMENU menu,  const String &s);               // see comment at enableMenuItem
void   removeAllMenuItems(             HMENU menu);                                 // recursive destroy all submenus

void   enableMenuItem(           const CWnd  *wnd , UINT          id, bool enabled);
void   enableMenuItem(           const CWnd  *wnd , const String &s , bool enabled);
void   checkMenuItem(            const CWnd  *wnd,  UINT          id, bool checked);
// return true if done, false if not
bool   enableSubMenuContainingId(const CWnd  *wnd , UINT          id, bool enabled);
bool   isMenuItemChecked(        const CWnd  *wnd,  UINT          id        );
// flags = 0,MF_SEPARATOR
void   insertMenuItem(           const CWnd  *wnd,  UINT          afterId, int flags, const String &itemText, UINT commandId);
void   removeMenuItem(           const CWnd  *wnd,  UINT          id        );
void   removeMenuItem(           const CWnd  *wnd,  const String &s);           // see comment at enableMenuItem
bool   removeSubMenuContainingId(const CWnd  *wnd,  UINT          id        );
bool   menuItemExists(           const CWnd  *wnd,  UINT          id        );
String getMenuItemText(          const CWnd  *wnd,  UINT          id        );
// Returns old itemtext
String setMenuItemText(          const CWnd  *wnd,  UINT          id, const String &itemText);
// Return true, if menuitem is checked, after toggle, else false
bool   toggleMenuItem(           const CWnd  *wnd,  UINT          id        );


// Throws exception on fail
WINDOWPLACEMENT getWindowPlacement(HWND hwnd);
// Throws exception on fail
void            setWindowPlacement(HWND hwnd, const WINDOWPLACEMENT &wpl);
// Throws exception on fail
CRect           getWindowRect(     HWND hwnd);
// Throws exception on fail
CPoint          getWindowPosition( HWND hwnd);
// Throws exception on fail
void            setWindowPosition( HWND hwnd, const CPoint &pos);
// Throws exception on fail
CSize           getWindowSize(     HWND hwnd);
// Throws exception on fail
void            setWindowSize(     HWND hwnd, const CSize &size);
// Throws exception on fail
CRect           getClientRect(     HWND hwnd);

CRect  getRelativeClientRect(const CWnd *wnd, int id);
CRect  getRelativeWindowRect(const CWnd *wnd, int id);
bool   setRelativeWindowRect(      CWnd *wnd, int id, const CRect &rect);
bool   setClientRectSize(          CWnd *wnd,         const CSize &size);
bool   setClientRectSize(          CWnd *wnd, int id, const CSize &size);
CRect  getClientRect(        const CWnd *wnd);
CRect  getClientRect(        const CWnd *wnd, int id);
CRect  getWindowRect(        const CWnd *wnd);

CRect  getWindowRect(        const CWnd *wnd, int id);
bool   setWindowRect(              CWnd *wnd,         const CRect &rect);
bool   setWindowRect(              CWnd *wnd, int id, const CRect &rect);
bool   centerWindow(               CWnd *wnd);
void   putWindowBesideWindow(      CWnd *wnd, CWnd *otherWindow);
CSize  getWindowSize(        const CWnd *wnd);
CSize  getWindowSize(        const CWnd *wnd, int id);
bool   setWindowSize(              CWnd *wnd,         const CSize &size);
bool   setWindowSize(              CWnd *wnd, int id, const CSize &size);
CPoint getWindowPosition(    const CWnd *wnd);
CPoint getWindowPosition(    const CWnd *wnd, int id);
bool   setWindowPosition(          CWnd *wnd,         const CPoint &point);
bool   setWindowPosition(          CWnd *wnd, int id, const CPoint &point);
String getWindowText(        const CWnd *wnd);
String getWindowText(        const CWnd *wnd, int id);
void   setWindowText(              CWnd *wnd,         const String &str);
void   setWindowText(              CWnd *wnd, int id, const String &str);
// ... is a list of resid (int). terminate list with 0
void   enableWindowList(           HWND hwnd, bool enable, ...);

int    getFocusCtrlId(       const CWnd *wnd);
// assume id is CEdit
void   gotoEditBox(                CWnd *wnd, int id);
// assume id is CEdit
void   gotoMatchingParanthes(      CWnd *wnd, int id);
void   gotoMatchingParanthes(      CEdit     *edit);
void   setCaretPos(                CEdit     *edit, int pos);
int    getCaretPos(                CEdit     *edit);
void   moveCaret(                  CEdit     *edit, int amount=1);
void   gotoMatchingParanthes(      CComboBox *cb);

CView *_createView(CFrameWnd *frameWnd, CWnd *parent, CRuntimeClass *viewClass, const CRect &rect);

#define createView(frameWnd, parent, ViewType, rect) (ViewType*)_createView(frameWnd, parent, RUNTIME_CLASS(ViewType), rect)

// assume id is CEdit
template<typename T> bool getEditValue(CWnd *wnd, int id, T &v, bool verbose = true) {
  const String str = getWindowText(wnd, id);
  std::wstringstream stream(str.cstr());
  stream >> v;
  if(stream) {
    return true;
  } else if(verbose) {
    gotoEditBox(wnd, id);
    showWarning(_T("Invalid input"));
  }
  return false;
}
template<typename T> void setEditValue(CWnd *wnd, int id, const T &v) {
  std::wstringstream stream;
  stream << v;
  setWindowText(wnd, id, stream.str().c_str());
}

void getTabOrder(HWND  dlg,       CompactUintArray &tabOrder);
void setTabOrder(HWND  dlg, const CompactUintArray &tabOrder);

class ChildWindowArray : public CompactUintArray {
private:
  friend BOOL CALLBACK childWindowArrayEnumerationProc(_In_ HWND wnd, _In_ LPARAM lParam);
  HWND m_parent;
  bool m_recursive;
public:
  ChildWindowArray() : m_parent(NULL), m_recursive(false) {
  }
  // Create a list of ctrlId's of the child-windows (recursive or non recursive) of the specified parent-window
  ChildWindowArray(        HWND  parent, bool recursive=false) {
    enumerateChildren(parent, recursive);
  }
  ChildWindowArray(        CWnd *parent, bool recursive=false) {
    enumerateChildren(*parent, recursive);
  }
  void enumerateChildren(  HWND  parent, bool recursive=false);

  // Return index in array of child or -1 if not found
  int        getChildIndex(HWND child ) const;
  // Return index in array of ctrlId or -1 if not found
  inline int getChildIndex(UINT ctrlId) const {
    return (int)getFirstIndex(ctrlId);
  }
  inline HWND getParent() const {
    return m_parent;
  }
};

class TabOrder : public ChildWindowArray {
public:
  inline TabOrder() {
  }
  inline TabOrder(CWnd *dlg) {
    saveTabOrder(*dlg);
  }
  inline TabOrder(HWND dlg) {
    saveTabOrder(dlg);
  }
  void saveTabOrder(HWND wnd) {
    enumerateChildren(wnd, false);
  }
  void restoreTabOrder() const {
    setTabOrder(getParent(), *this);
  }
};

/*
class Layout {
private:
  TabOrder m_tabOrder;
  CWnd    *m_dlg;
  int      m_ctrlId;
  bool     m_useLayout;

  Layout &enableDynamicLayout(bool enable);
  bool    hasDynamicLayout() const;
public:
  inline Layout(CWnd *dlg, int ctrlId) {
    saveLayout(dlg, ctrlId);
  }
  void saveLayout(CWnd *dlg, int ctrlId);
  void restoreLayout();
};
*/

void        setWindowCursor(     HWND  wnd, const TCHAR *name  );
void        setWindowCursor(     HWND  wnd, int          resId );
void        setWindowCursor(     HWND  wnd, HCURSOR      cursor);
inline void setWindowCursor(CWnd *wnd, const TCHAR      *name  ) {
  ::setWindowCursor(wnd->m_hWnd, name);
}
inline void setWindowCursor(CWnd *wnd, int               resId ) {
  ::setWindowCursor(wnd->m_hWnd, resId);
}
inline void setWindowCursor(CWnd *wnd, HCURSOR           cursor) {
  ::setWindowCursor(wnd->m_hWnd, cursor);
}
void        setSystemCursor(      int  id , const TCHAR *name  );
void        setSystemCursor(      int  id , int          resId );
void        setSystemCursor(      int  id , HCURSOR      cursor);

HCURSOR createCursor(HINSTANCE hInstance, int bitmapResId, int maskResId, int xHotSpot, int yHotSpot);
HCURSOR createCursor(const ByteArray &bytes);
HCURSOR loadCursor(  FILE *f);
HCURSOR loadCursor(  const String &fileName);
HICON   createIcon(  HINSTANCE hInstance, int bitmapResId, int maskResId);
HICON   createIcon(  const ByteArray &bytes);
HICON   loadIcon(    FILE *f);
HICON   loadIcon(    const String &fileName);

BITMAP           getBitmapInfo(              HBITMAP   bm);
BITMAPINFOHEADER getBitmapInfoHeader(        HBITMAP   bm);
CSize            getBitmapSize(              HBITMAP   bm);
CBitmap         *createFromHandle(           HBITMAP   bm);

inline int getArea(const CSize &sz) {
  return sz.cx * sz.cy;
}

inline int getArea(const CRect &r) {
  return getArea(r.Size());
}

class BackgroundSaver {
private:
  CRect   m_rect;
  CBitmap m_bm;
  inline void initRect() {
    m_rect.left = m_rect.top = m_rect.right = m_rect.bottom = 0;
  }
public:
  inline BackgroundSaver() {
    initRect();
  }
  virtual ~BackgroundSaver() {
  }
  void saveBackground(   HDC hdc, const CRect &r);
  void restoreBackground(HDC hdc);
  inline const CRect &getSavedRect() const {
    return m_rect;
  }
  inline bool hasSavedRect() const {
    return getArea(m_rect) != 0;
  }
};

ICONINFO  getIconInfo(                       HICON     icon);
CSize     getIconSize(                       HICON     icon);
void      closeIconInfo(                     ICONINFO &info);
ByteArray &bitmapToByteArray(ByteArray &dst, HBITMAP   bm  );

ByteArray &getBitmapBits(HBITMAP bm,       ByteArray &bytes,       BITMAPINFO &bmInfo);
void       setBitmapBits(HBITMAP bm, const ByteArray &bytes, const BITMAPINFO &bmInfo);

void writeAsBMP( HBITMAP bm, ByteOutputStream &out);
void writeAsJPG( HBITMAP bm, ByteOutputStream &out);
void writeAsPNG( HBITMAP bm, ByteOutputStream &out);
void writeAsTIFF(HBITMAP bm, ByteOutputStream &out);

void alphaBlend(HDC dst, int x, int y, int w, int h,  HDC src, int sx, int sy, int sw, int sh, int srcConstAlpha);
void preMultiplyAlpha(HBITMAP bm);

HBITMAP cloneBitmap(HBITMAP bm);
HBITMAP decodeAsPNG( const ByteArray &bytes, bool &hasAlpha);
HBITMAP decodeAsTIFF(const ByteArray &bytes, bool &hasAlpha);

void textOutTransparentBackground(HDC hdc, const CPoint &p, const String &s, CFont &font, COLORREF color);
void textOut(                     HDC hdc, const CPoint &p, const String &s);
void textOut(                     HDC hdc, int x, int y, const String &s);

// void Message(_In_z_ _Printf_format_string_ TCHAR const * const format, ... );

int vshowMessageBox(int flags, _In_z_ _Printf_format_string_ TCHAR const * const format, va_list argptr);

inline int showMessageBox(int flags, _In_z_ _Printf_format_string_ TCHAR const * const format, ...) {
  va_list argptr;
  va_start(argptr, format);
  const int result = vshowMessageBox(flags, format, argptr);
  va_end(argptr);
  return result;
}

inline void showInformation(_In_z_ _Printf_format_string_ TCHAR const * const format, ...) {
  va_list argptr;
  va_start(argptr, format);
  vshowMessageBox(MB_ICONINFORMATION, format, argptr);
  va_end(argptr);
}

inline void showInformation(const String &msg) {
  showInformation(_T("%s"), msg.cstr());
}

inline void showWarning(_In_z_ _Printf_format_string_ TCHAR const * const format, ...) {
  va_list argptr;
  va_start(argptr, format);
  vshowMessageBox(MB_ICONWARNING, format, argptr);
  va_end(argptr);
}

inline void showWarning(const String &msg) {
  showWarning(_T("%s"), msg.cstr());
}

inline void showError(_In_z_ _Printf_format_string_ TCHAR const * const format, ...) {
  va_list argptr;
  va_start(argptr, format);
  vshowMessageBox(MB_ICONERROR, format, argptr);
  va_end(argptr);
}

inline void showError(const String &msg) {
  showError(_T("%s"), msg.cstr());
}

String getMessageName(int msg);
void   showException(const Exception &e, int flags = MB_ICONERROR);

void   addData(                     CListCtrl &ctrl, int row, int col, const String &str, bool newItem = false);
String getItemString(         const CListCtrl &ctrl, int row, int col);
void   setSelectedIndex(            CListCtrl &ctrl, int index);
void   filloutListHeaderWidth(      CListCtrl &ctrl);

CRect  getScreenRect(); // the visible rectangle NOT overlapping the taskbar
CRect  getTaskBarRect();
HDC    getScreenDC();   // after use of HDC, call DeleteDC.
int    getScreenPlanes();
int    getScreenBitsPixel();

CSize  getScreenSize(bool includeTaskBar = true);
CSize  getScreenSizeInMillimeters();
CSize  getDCSizeInPixels(         HDC hdc);
CSize  getDCSizeInMillimeters(    HDC hdc);
CSize  pixelsToMillimeters(       HDC hdc, const CSize &size);    // convert size in pixels -> size in millimeters

CSize  getTextExtent(             HDC hdc, const String &s);      // return size of bounding box for s with hdc's selected font
CSize  getTextExtent1(            HDC hdc, const StringArray &a); // return size of union of all bounding boxes for lines written under each other with hdc's selected font
CSize  getMaxTextExtent(          HDC hdc, const StringArray &a); // return size of biggest bounding box of lines in a with hdc's selected font

void   setControlText(   int id, CWnd *wnd);

int    confirmDialogBox(const String &message, const String &caption, bool &showAgain, UINT nType = MB_OK);

bool   moveFileToTrashCan(        HWND hwnd, const String      &fileName , bool confirm = true);
bool   moveFilesToTrashCan(       HWND hwnd, const StringArray &fileNames, bool confirm = true);

void   notifyIconAdd(             HWND hwnd, UINT uID, HICON hicon, const String &toolTip = EMPTYSTRING, int callbackMessage = WM_LBUTTONDOWN);
void   notifyIconDelete(          HWND hwnd, UINT uID);
void   notifyIconSetToolTip(      HWND hwnd, UINT uID, const String &toolTip);

String toString(const CPoint &p);
String toString(const CSize  &s);

  inline CSize operator*(const CSize &s1, const CSize &s2) {
  return CSize(s1.cx*s2.cx,s1.cy*s2.cy);
}

inline CSize operator/(const CSize &s1, const CSize &s2) {
  return CSize(s1.cx/s2.cx,s1.cy/s2.cy);
}

#pragma warning(push)

#pragma warning(disable : 4244)

#define ORIGIN CPoint(0,0)

typedef CompactArray<CPoint> PointArray;

inline CSize operator*(const CSize &sz, double factor) {
  return CSize(sz.cx*factor, sz.cy*factor);
}

inline CSize operator/(const CSize &sz, double factor) {
  return CSize(sz.cx/factor, sz.cy/factor);
}

#pragma warning(pop)

CRect makePositiveRect(const CRect &r);

bool isBorderHit(UINT hitTestCode); // hitTestCode comes from OnNcLButtonDown(UINT nHitTest, CPoint point)

#define OCR_HAND        32649
#define OCR_QUESTION    OCR_APPSTARTING+1
#define OCR_SCROLLNS    OCR_APPSTARTING+2
#define OCR_SCROLLWE    OCR_APPSTARTING+3
#define OCR_SCROLLALL   OCR_APPSTARTING+4
#define OCR_SCROLLUP    OCR_APPSTARTING+5
#define OCR_SCROLLDOWN  OCR_APPSTARTING+6
#define OCR_SCROLLLEFT  OCR_APPSTARTING+7
#define OCR_SCROLLRIGHT OCR_APPSTARTING+8
#define OCR_SCROLLNW    OCR_APPSTARTING+9
#define OCR_SCROLLNE    OCR_APPSTARTING+10
#define OCR_SCROLLSW    OCR_APPSTARTING+11
#define OCR_SCROLLSE    OCR_APPSTARTING+12
#define OCR_COMPACTDISC OCR_APPSTARTING+13

// extra tools to set in HeaderCtrl (HDITEM.fmt)
#define HDF_SORTUP              0x0400
#define HDF_SORTDOWN            0x0200

class DebugBitmap {
public:
  static void showBitmap(HBITMAP bitmap); // paint bitmap in upper left corner of the screen.
};

#include "CommontCtrlManifest.h"

#include "PragmaLib.h"
