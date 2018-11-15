#pragma once

#ifndef VC_EXTRALEAN
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

#ifndef _AFX_NO_DB_SUPPORT
#include <afxdb.h>          // MFC ODBC database classes
#endif // _AFX_NO_DB_SUPPORT

#ifndef _AFX_NO_DAO_SUPPORT
#include <afxdao.h>         // MFC DAO database classes
#endif // _AFX_NO_DAO_SUPPORT

#ifndef _AFX_NO_OLE_SUPPORT
#include <afxdtctl.h>       // MFC support for Internet Explorer 4 Common Controls
#endif
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>         // MFC support for Windows Common Controls
#endif // _AFX_NO_AFXCMN_SUPPORT

#include <MyUtil.h>
#include <ByteArray.h>
#include "LayoutManager.h"

// ex. if s == "0/2", this will enable/disable the third menuitem in first pulldown-menu of the windows menu
int    getMenuItemType(                HMENU menu,  UINT          pos);
HMENU  findMenuByString(               HMENU menu,  const String &s , int &index);  // see comment at enableMenuItem
HMENU  findMenuContainingId(           HMENU menu,  UINT          id, int &index);
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
// returns old itemtext
String setMenuItemText(          const CWnd  *wnd,  UINT          id, const String &itemText);
bool   toggleMenuItem(           const CWnd  *wnd,  UINT          id        );


CRect  getRelativeClientRect(const CWnd *wnd, int id);
CRect  getRelativeWindowRect(const CWnd *wnd, int id);
bool   setRelativeWindowRect(      CWnd *wnd, int id, const CRect &rect);
bool   setClientRectSize(          CWnd *wnd,         const CSize &size);
bool   setClientRectSize(          CWnd *wnd, int id, const CSize &size);
CRect  getClientRect(        const CWnd *wnd);
CRect  getClientRect(              HWND  wnd);
CRect  getClientRect(        const CWnd *wnd, int id);
CRect  getWindowRect(        const CWnd *wnd);
CRect  getWindowRect(              HWND  wnd);

CRect  getWindowRect(        const CWnd *wnd, int id);
bool   setWindowRect(              CWnd *wnd,         const CRect &rect);
bool   setWindowRect(              CWnd *wnd, int id, const CRect &rect);
bool   centerWindow(               CWnd *wnd);
void   putWindowBesideWindow(      CWnd *wnd, CWnd *otherWindow);
CSize  getWindowSize(        const CWnd *wnd);
CSize  getWindowSize(              HWND  wnd);
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

int    getFocusCtrlId(       const CWnd *wnd);
// assume id is CEdit
void   gotoEditBox(                CWnd *wnd, int id);
// assume id is CEdit
void   gotoMatchingParanthes(      CWnd *wnd, int id);
void   gotoMatchingParanthes(      CEdit     *edit);
void   gotoMatchingParanthes(      CComboBox *cb);

CompactIntArray getTabOrder(CWnd *wnd);
void            setTabOrder(CWnd *wnd, const CompactIntArray &tabOrderArray);

void   setWindowCursor(     HWND  wnd, const TCHAR *name);
void   setWindowCursor(     HWND  wnd, int resId);
void   setWindowCursor(     HWND  wnd, HCURSOR cursor);
inline void setWindowCursor(CWnd *wnd, const TCHAR *name) {
  ::setWindowCursor(wnd->m_hWnd, name);
}
inline void setWindowCursor(CWnd *wnd, int resId) {
  ::setWindowCursor(wnd->m_hWnd, resId);
}
inline void setWindowCursor(CWnd *wnd, HCURSOR cursor) {
  ::setWindowCursor(wnd->m_hWnd, cursor);
}
void   setSystemCursor(            int   id , const TCHAR *name);
void   setSystemCursor(            int   id , int resId);
void   setSystemCursor(            int   id , HCURSOR cursor);

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

void showException(const Exception &e, int flags = MB_ICONERROR);

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

void   notifyIconAdd(             HWND hwnd, UINT uID, HICON hicon, const String &toolTip = _T(""), int callbackMessage = WM_LBUTTONDOWN);
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

inline int getArea(const CSize &sz) {
  return sz.cx * sz.cy;
}

inline int getArea(const CRect &r) {
  return getArea(r.Size());
}

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
