#pragma once

#include "MyUtil.h"
#include "TreeMap.h"

typedef enum {
  SHOW_HWND        
 ,SHOW_THREAD      
 ,SHOW_RECT        
 ,SHOW_WINDOWTEXT  
 ,SHOW_CLASS       
 ,SHOW_STYLE       
 ,SHOW_EXSTYLE     
 ,SHOW_STYLETEXT   
 ,SHOW_EXSTYLETEXT 
} WindowInfoField;

class WindowInfo {
public:
  HWND       m_hwnd;
  DWORD      m_id;
  DWORD      m_thread;
  CRect      m_rect;
  String     m_windowText;
  String     m_className;
  DWORD      m_style;
  DWORD      m_exStyle;
  WindowInfo(HWND hwnd, DWORD id, DWORD thread, const TCHAR *windowText, const TCHAR *className, DWORD style, DWORD exStyle);
  String toString() const;
  String toString(const Array<WindowInfoField> &fields) const;

  String getStyleString() const {
    return getStyleToString(m_style);
  }

  String getExStyleString() const {
    return getExStyleToString(m_exStyle);
  }

  static String getStyleToString(DWORD style);
  static String getExStyleToString(DWORD exStyle);
};

class ProcessWindowInfo {
private:
  const String      m_exeFile;
  const String      m_commandLine;
  Array<WindowInfo> m_windowList;
  void add(const WindowInfo &wi);
  friend BOOL CALLBACK winEnumProc(HWND hwnd, LPARAM param);
public:
  ProcessWindowInfo(const String &exeFile, const String &commandLine, const WindowInfo &wi);

  const String &getExeFile() const {
    return m_exeFile;
  }

  const String &getCommandLine() const {
    return m_commandLine;
  }

  const Array<WindowInfo> &getWindowList() const {
    return m_windowList;
  }
};

typedef IntTreeMap<ProcessWindowInfo> ProcessWindowInfoMap; // key is processId

// Retrieves information of active windows running on the system
// styleFilter and notStyleFilter can be any combination of Window Styles as defined in WinUser.h. All beginning with WS_
// exStyleFilter and notExStyleFilter can be any combination of Extended Window Styles as defined in WinUser.h. All beginning with WS_EX_
// Any window with one or more (ex)style-bit common with (ex)styleFilter AND no (ex)style-bits common with not(Ex)styleFilter will be 
// added to the list.
ProcessWindowInfoMap getProcessWindowInfoMap(DWORD styleFilter = -1, DWORD notStyleFilter = 0, DWORD exStyleFilter = -1, DWORD notExStyleFilter = 0);


