#include "pch.h"
#include <WindowInfoMap.h>
#include <ProcessTools.h>

WindowInfo::WindowInfo(HWND hwnd, DWORD id, DWORD thread, const TCHAR *windowText, const TCHAR *className, DWORD style, DWORD exStyle) {
  m_hwnd       = hwnd;
  m_id         = id;
  m_thread     = thread;
  m_windowText = windowText;
  m_className  = className;
  m_style      = style;
  m_exStyle    = exStyle;
  ::GetWindowRect(m_hwnd,&m_rect);
}

String WindowInfo::toString() const {
  return format(_T("hwnd:%p, winId:%-8d, rect:(%4d,%4d,%4d,%4d), text:\"%s\", class:\"%-20s\", style:%08x, exStyle:%08x, %-11s, %-11s ")
               ,m_hwnd
               ,m_id
               ,m_rect.left,m_rect.top,m_rect.right,m_rect.bottom
               ,m_windowText.cstr()
               ,m_className.cstr()
               ,m_style
               ,m_exStyle
               ,(m_style & WS_CHILDWINDOW) ? _T("Childwindow") : _T("Mainwindow")
               ,(m_style & WS_VISIBLE    ) ? _T("Visible")     : _T("Not visible")
               );
}

String WindowInfo::toString(const Array<WindowInfoField> &fields) const {
  String result;
  TCHAR *delimiter = _T("");

  for(int i = 0; i < fields.size(); i++) {
    if(i > 0) {
      result += _T(",");
    }
    switch(fields[i]) {
    case SHOW_HWND:
      result += format(_T("HWND:%p"),m_hwnd);
      break;

    case SHOW_THREAD:
      result += format(_T("Thread:%p"),m_thread);
      break;

    case SHOW_RECT:
      result += format(_T("Rectangle:(%4d,%4d,%4d,%4d)"), m_rect.left,m_rect.top,m_rect.right,m_rect.bottom);
      break;

    case SHOW_WINDOWTEXT:
      result += format(_T("\"%s\""),m_windowText.cstr());
      break;

    case SHOW_CLASS:
      result += format(_T("\"%s\""),m_className.cstr());
      break;

    case SHOW_STYLE:
      result += format(_T("%08x"),m_style);
      break;

    case SHOW_EXSTYLE:
      result += format(_T("%08x"),m_exStyle);
      break;

    case SHOW_STYLETEXT:
      result += format(_T("{%s}"),getStyleString().cstr());
      break;

    case SHOW_EXSTYLETEXT:
      result += format(_T("{%s}"),getExStyleString().cstr());
      break;
    }
  }
  return result;
}

String WindowInfo::getStyleToString(DWORD style) { // static 
  String result;
  TCHAR *delimiter = _T("");
#define APPENDIFSTYLESET(s) if(style & WS_##s) { result += delimiter; result += _T(#s); delimiter = _T(" "); }
  APPENDIFSTYLESET(POPUP        );
  APPENDIFSTYLESET(CHILD        );
  APPENDIFSTYLESET(MINIMIZE     );
  APPENDIFSTYLESET(VISIBLE      );
  APPENDIFSTYLESET(DISABLED     );
  APPENDIFSTYLESET(CLIPSIBLINGS );
  APPENDIFSTYLESET(CLIPCHILDREN );
  APPENDIFSTYLESET(MAXIMIZE     );
  APPENDIFSTYLESET(CAPTION      );
  APPENDIFSTYLESET(BORDER       );
  APPENDIFSTYLESET(DLGFRAME     );
  APPENDIFSTYLESET(VSCROLL      );
  APPENDIFSTYLESET(HSCROLL      );
  APPENDIFSTYLESET(SYSMENU      );
  APPENDIFSTYLESET(THICKFRAME   );
  APPENDIFSTYLESET(GROUP        );
  APPENDIFSTYLESET(TABSTOP      );
  return result;
}

String WindowInfo::getExStyleToString(DWORD exStyle) { // static
  String result;
  TCHAR *delimiter = _T("");
#define APPENDIFEXSTYLESET(s) if(exStyle & WS_EX_##s) { result += delimiter; result += _T(#s); delimiter = _T(" "); }
  APPENDIFEXSTYLESET(DLGMODALFRAME   );
  APPENDIFEXSTYLESET(NOPARENTNOTIFY  );
  APPENDIFEXSTYLESET(TOPMOST         );
  APPENDIFEXSTYLESET(ACCEPTFILES     );
  APPENDIFEXSTYLESET(TRANSPARENT     );
  APPENDIFEXSTYLESET(MDICHILD        );
  APPENDIFEXSTYLESET(TOOLWINDOW      );
  APPENDIFEXSTYLESET(WINDOWEDGE      );
  APPENDIFEXSTYLESET(CLIENTEDGE      );
  APPENDIFEXSTYLESET(CONTEXTHELP     );
  APPENDIFEXSTYLESET(RIGHT           );
  APPENDIFEXSTYLESET(LEFT            );
  APPENDIFEXSTYLESET(RTLREADING      );
  APPENDIFEXSTYLESET(LTRREADING      );
  APPENDIFEXSTYLESET(LEFTSCROLLBAR   );
  APPENDIFEXSTYLESET(RIGHTSCROLLBAR  );
  APPENDIFEXSTYLESET(CONTROLPARENT   );
  APPENDIFEXSTYLESET(STATICEDGE      );
  APPENDIFEXSTYLESET(APPWINDOW       );
  return result;
}

ProcessWindowInfo::ProcessWindowInfo(const String &exeFile, const String &commandLine, const WindowInfo &wi)
: m_exeFile(exeFile)
, m_commandLine(commandLine) {
  m_windowList.add(wi);
}

void ProcessWindowInfo::add(const WindowInfo &wi) {
  m_windowList.add(wi);
}

class WindowEnumeratorParameter {
public:
  ProcessWindowInfoMap &m_map;
  const DWORD          m_styleFilter  , m_notStyleFilter;
  const DWORD          m_exStyleFilter, m_notExStyleFilter;
  String styleString , notStyleString, exStyleString, notExStyleString;

  WindowEnumeratorParameter(ProcessWindowInfoMap &map, DWORD styleFilter, DWORD notStyleFilter, DWORD exStyleFilter, DWORD notExStyleFilter)
  : m_map(map)
  , m_styleFilter(     styleFilter     )
  , m_notStyleFilter(  notStyleFilter  )
  , m_exStyleFilter(   exStyleFilter   )
  , m_notExStyleFilter(notExStyleFilter)
  {
/*
    styleString      = getStyleToString(m_styleFilter);
    notStyleString   = getStyleToString(m_notStyleFilter);
    exStyleString    = getExStyleToString(m_exStyleFilter);
    notExStyleString = getExStyleToString(m_notExStyleFilter);
*/
  }
};

static BOOL CALLBACK winEnumProc(HWND hwnd, LPARAM param) {
  WindowEnumeratorParameter *wp = (WindowEnumeratorParameter*)param;
  ProcessWindowInfoMap  &pwiMap = wp->m_map;

  DWORD style   = GetWindowLong(hwnd, GWL_STYLE);
  DWORD exStyle = GetWindowLong(hwnd, GWL_EXSTYLE);

//  String styleString   = getStyleToString(style);
//  String exStyleString = getExStyleToString(exStyle);

  if((style   & wp->m_styleFilter  ) != 0 && (style   & wp->m_notStyleFilter  ) == 0
  && (exStyle & wp->m_exStyleFilter) != 0 && (exStyle & wp->m_notExStyleFilter) == 0) {
    TCHAR className[256];
    TCHAR windowText[256];
    DWORD pid;
    DWORD thread = GetWindowThreadProcessId(hwnd,&pid);

    ProcessInfo pInfo(pid);

    DWORD winId = GetWindowLong(hwnd, GWL_ID);

    GetClassName(hwnd,className,ARRAYSIZE(className));
    GetWindowText(hwnd,windowText,ARRAYSIZE(windowText));

    WindowInfo wi(hwnd, winId, thread, windowText, className, style, exStyle);
    ProcessWindowInfo *pwi = pwiMap.get(pid);
    if(pwi != NULL) {
      pwi->add(wi);
    } else {
      pwiMap.put(pid,ProcessWindowInfo(pInfo.getExePath(), pInfo.getCommandLine(),wi));
    }
  }

  return TRUE;
}

ProcessWindowInfoMap  getProcessWindowInfoMap(DWORD styleFilter, DWORD notStyleFilter, DWORD exStyleFilter, DWORD notExStyleFilter) {
  enableTokenPrivilege(SE_DEBUG_NAME,true);
  ProcessWindowInfoMap result;
  EnumWindows(winEnumProc,(long)&WindowEnumeratorParameter(result,styleFilter, notStyleFilter, exStyleFilter, notExStyleFilter));
  return result;
}

