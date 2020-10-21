#include "pch.h"
#include <Tokenizer.h>

static String getMenuString(HMENU menu, UINT pos) {
  TCHAR ms[2048];
  GetMenuString(menu, pos, ms, ARRAYSIZE(ms)-1, MF_BYPOSITION);
  return ms;
}

static bool menuContainsId(HMENU menu, UINT id) { // no recurse
  const int count = GetMenuItemCount(menu);
  for(int i = 0; i < count; i++) {
    if(GetMenuItemID(menu, i) == id) {
      return true;
    }
  }
  return false;
}

static int findMenuItemIndexByString(HMENU menu, const String &s) {
  const int n = GetMenuItemCount(menu);
  for(int i = 0; i < n; i++) {
    String t = getMenuString(menu, i);
    if(t.replace(_T('&'),EMPTYSTRING).equalsIgnoreCase(s)) {
      return i;
    }
  }
  return -1;
}

static MENUITEMINFO getMenuItemInfo(HMENU menu, UINT pos, int mask) {
  MENUITEMINFO info;
  memset(&info, 0, sizeof(info));
  info.cbSize = sizeof(MENUITEMINFO);
  info.fMask = mask;
  GetMenuItemInfo(menu, pos, true, &info);
  return info;
}

static HMENU findMenuContainingId(const CWnd *wnd, UINT id, int *index=nullptr) {
  CMenu *m = wnd->GetMenu();
  if(m == nullptr) return nullptr;
  return findMenuContainingId(*m, id, index);
}

static HMENU findMenuByString(const CWnd *wnd, const String &s, int *index=nullptr) {
  CMenu *m = wnd->GetMenu();
  if(m == nullptr) return nullptr;
  return findMenuByString(*m, s, index);
}

static String setItemText(HMENU menu, UINT index, const String &itemText) {
  MENUITEMINFO itemInfo;
  itemInfo.cbSize = sizeof(MENUITEMINFO);
  itemInfo.fMask = MIIM_TYPE;
  itemInfo.fType = MFT_STRING;
  itemInfo.dwTypeData = nullptr;
  GetMenuItemInfo(menu, index, TRUE, &itemInfo);
  const int bufferSize = itemInfo.cch+1;
  TCHAR *buffer = new TCHAR[bufferSize]; TRACE_NEW(buffer);
  itemInfo.dwTypeData = buffer;
  itemInfo.cch++;
  GetMenuItemInfo(menu, index, TRUE, &itemInfo);
  String result = itemInfo.dwTypeData ? itemInfo.dwTypeData : EMPTYSTRING;
  String tmp = itemText;
  SAFEDELETEARRAY(buffer);
  itemInfo.dwTypeData = tmp.cstr();
  SetMenuItemInfo(menu, index, TRUE, &itemInfo);
  return result;
}

// --------------------- public ---------------------------

int getMenuItemType(HMENU menu, UINT pos) {
  MENUITEMINFO info = getMenuItemInfo(menu, pos, MIIM_TYPE);
  return info.fType;
}

HMENU findMenuByString(HMENU menu, const String &s, int *index) {
  if(menu == nullptr) return nullptr;
  for(Tokenizer tok(s,_T("/")); menu != nullptr && tok.hasNext();) {
    String str = tok.next();
    int tmpIndex = findMenuItemIndexByString(menu, str);
    if(tmpIndex < 0) {
      if((_stscanf(str.cstr(), _T("%d"), &tmpIndex) != 1) || (tmpIndex < 0)) {
        return nullptr;
      }
    }

    if(tok.hasNext()) {
      menu = GetSubMenu(menu, tmpIndex);
    } else {
      if(index) {
        *index = tmpIndex;
      }
      return menu;
    }
  }
  return nullptr;
}

HMENU findMenuContainingId(HMENU menu, UINT id, int *index) {
  if(menu == nullptr) return nullptr;
  const int count = GetMenuItemCount(menu);
  for(int i = 0; i < count; i++) {
    MENUITEMINFO itemInfo = getMenuItemInfo(menu, i, MIIM_SUBMENU);
    if(itemInfo.hSubMenu != nullptr) {
      HMENU sm = findMenuContainingId(itemInfo.hSubMenu /*GetSubMenu(menu, i)*/, id, index);
      if(sm) {
        return sm;
      }
    } else if(GetMenuItemID(menu, i) == id) {
      if(index) {
        *index = i;
      }
      return menu;
    }
  }
  return nullptr;
}

void enableMenuItem(HMENU menu, UINT id, bool enabled) {
  bool done = EnableMenuItem(menu, id, MF_BYCOMMAND | (enabled ? MF_ENABLED : MF_GRAYED)) != -1;
}

void checkMenuItem(HMENU menu, UINT  id, bool checked) {
  bool done = CheckMenuItem(menu, id, checked ? MF_CHECKED : MF_UNCHECKED) != 0xffffffff;
}

bool enableSubMenuContainingId(HMENU menu, UINT id, bool enabled) {
  if(menu == nullptr) return false;
  const int count = GetMenuItemCount(menu);
  for(int i = 0; i < count; i++) {
    const MENUITEMINFO itemInfo = getMenuItemInfo(menu, i, MIIM_SUBMENU);
    if(itemInfo.hSubMenu != nullptr) {
      if(menuContainsId(GetSubMenu(menu, i), id)) {
        EnableMenuItem(menu, i, MF_BYPOSITION | (enabled ? MF_ENABLED : MF_GRAYED));
        return true;
      }
      if(enableSubMenuContainingId(GetSubMenu(menu, i), id, enabled)) {
        return true;
      }
    }
  }
  return false;
}

bool isMenuItemChecked(HMENU menu, UINT id) {
  return (menu == nullptr) ? false : (GetMenuState(menu, id, MF_BYCOMMAND) & MF_CHECKED) ? true : false;
}

void insertMenuItem(HMENU menu, UINT pos, const String &itemText, UINT commandId) {
  String tmp = itemText;
  MENUITEMINFO info;
  info.cbSize     = sizeof(info);
  info.fMask      = MIIM_TYPE | MIIM_ID;
  info.fType      = MFT_STRING;
  info.dwTypeData = tmp.cstr();
  info.wID        = commandId;
  BOOL ok = InsertMenuItem(menu, pos, true, &info);
}

void appendMenuItem(HMENU menu, const String &itemText, UINT commandId) {
  insertMenuItem(menu, GetMenuItemCount(menu), itemText, commandId);
}

void insertMenuSeparator(HMENU menu, UINT pos) {
  MENUITEMINFO info;
  info.cbSize     = sizeof(info);
  info.fMask      = MIIM_TYPE ;
  info.fType      = MFT_SEPARATOR;
  InsertMenuItem(menu, pos, true, &info);
}

void appendMenuSeparator(HMENU menu) {
  insertMenuSeparator(menu, GetMenuItemCount(menu));
}

HMENU insertSubMenu(HMENU menu, UINT pos, const String &text) {
  String tmp = text;
  MENUITEMINFO info;
  info.cbSize     = sizeof(info);
  info.fMask      = MIIM_TYPE | MIIM_SUBMENU;
  info.fType      = MFT_STRING;
  info.dwTypeData = tmp.cstr();
  HMENU sm = CreatePopupMenu();
  info.hSubMenu   = sm;
  InsertMenuItem(menu, pos, true, &info);
  return sm;
}

HMENU appendSubMenu(HMENU menu, const String &text) {
  return insertSubMenu(menu, GetMenuItemCount(menu), text);
}

void removeMenuItem(HMENU menu, UINT id) {
  bool done = RemoveMenu(menu, id, MF_BYCOMMAND) != 0;
}

bool removeSubMenuContainingId(HMENU menu, UINT id) {
  if(menu == nullptr) return false;
  const int count = GetMenuItemCount(menu);
  for(int i = 0; i < count; i++) {
    const MENUITEMINFO itemInfo = getMenuItemInfo(menu, i, MIIM_SUBMENU);
    if(itemInfo.hSubMenu != nullptr) {
      if(menuContainsId(GetSubMenu(menu, i), id)) {
        RemoveMenu(menu, i, MF_BYPOSITION);
        return true;
      }
      if(removeSubMenuContainingId(GetSubMenu(menu, i), id)) {
        return true;
      }
    }
  }
  return false;
}

bool menuItemExists(HMENU menu,  UINT id) {
  return findMenuContainingId(menu, id) != nullptr;
}

String getMenuItemText(HMENU menu, int pos) {
  TCHAR str[4096];
  GetMenuString(menu, pos, str, ARRAYSIZE(str), MF_BYPOSITION);
  LASTVALUE(str) = 0;
  return str;
}

String setMenuItemText(HMENU menu, UINT id , const String &itemText) {
  int index;
  menu = findMenuContainingId(menu, id, &index);
  if(menu == nullptr) {
    return EMPTYSTRING;
  } else {
    return setItemText(menu, index, itemText);
  }
}

HMENU getSubMenu(HMENU menu, const String &s) {
  int index;
  menu = findMenuByString(menu, s, &index);
  if(menu) {
    return GetSubMenu(menu, index);
  } else {
    return nullptr;
  }
}

void removeAllMenuItems(HMENU menu) {
  int n = GetMenuItemCount(menu);
  while(n--)  {
    const MENUITEMINFO info = getMenuItemInfo(menu, n, MIIM_SUBMENU);
    if(info.hSubMenu != nullptr) {
      HMENU sm = GetSubMenu(menu, n);
      removeAllMenuItems(sm);
      RemoveMenu(menu, n, MF_BYPOSITION);
      DestroyMenu(sm);
    } else {
      RemoveMenu(menu, n, MF_BYPOSITION);
    }
  }
}

// -------------------------- CWnd  versions --------------------

void enableMenuItem(const CWnd *wnd, UINT id, bool enabled) {
  CMenu *m = wnd->GetMenu();
  if(m == nullptr) return;
  enableMenuItem(*m, id, enabled);
}

void enableMenuItem(const CWnd *wnd, const String &s, bool enabled) {
  int index;
  HMENU menu = findMenuByString(wnd, s, &index);
  if(menu != nullptr) {
    bool done = EnableMenuItem(menu, index, MF_BYPOSITION | (enabled ? MF_ENABLED : MF_GRAYED)) != -1;
  }
}

void checkMenuItem(const CWnd *wnd, UINT id, bool checked) {
  CMenu *m = wnd->GetMenu();
  if(m == nullptr) return;
  checkMenuItem(*m, id, checked);
}

bool enableSubMenuContainingId(const CWnd *wnd, UINT id, bool enabled) {
  CMenu *m = wnd->GetMenu();
  if(m == nullptr) return false;
  return enableSubMenuContainingId(*m, id, enabled);
}

bool isMenuItemChecked(const CWnd *wnd, UINT id) {
  CMenu *m = wnd->GetMenu();
  if(m == nullptr) return false;
  return isMenuItemChecked(*m, id);
}

// flags = 0,MF_SEPARATOR
void insertMenuItem(const CWnd *wnd, UINT afterId, int flags, const String &itemText, UINT commandId) {
  HMENU menu = findMenuContainingId(wnd, afterId);
  if(menu) {
    bool done = InsertMenu(menu, afterId, MF_BYCOMMAND | (flags & MF_SEPARATOR), commandId, itemText.cstr()) != 0;
  }
}

void removeMenuItem(const CWnd *wnd, UINT id) {
  HMENU menu = findMenuContainingId(wnd, id);
  if(menu != nullptr) {
    removeMenuItem(menu, id);
  }
}

void removeMenuItem(const CWnd *wnd, const String &s) {
  int index;
  bool done = false;
  HMENU menu = findMenuByString(wnd, s, &index);
  if(menu != nullptr) {
    done = RemoveMenu(menu, index, MF_BYPOSITION) != 0;
  }
}

bool removeSubMenuContainingId(const CWnd *wnd,  UINT id) {
  CMenu *m = wnd->GetMenu();
  if(m == nullptr) return false;
  return removeSubMenuContainingId(*m, id);
}

bool menuItemExists(const CWnd *wnd, UINT id) {
  return findMenuContainingId(wnd, id) != nullptr;
}

String getMenuItemText(const CWnd *wnd, UINT id) {
  int index;
  HMENU menu = findMenuContainingId(wnd, id, &index);
  if(menu == nullptr) return EMPTYSTRING;
  return getMenuItemText(menu, index);
}

String setMenuItemText(const CWnd *wnd, UINT id, const String &itemText) {
  int index;
  HMENU menu = findMenuContainingId(wnd, id, &index);
  if(menu == nullptr) return EMPTYSTRING;
  return setItemText(menu, index, itemText);
}

bool toggleMenuItem(const CWnd *wnd, UINT id) {
  const bool checked = !isMenuItemChecked(wnd, id);
  checkMenuItem(wnd, id, checked);
  return checked;
}
