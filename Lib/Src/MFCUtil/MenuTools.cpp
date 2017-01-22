#include "pch.h"
#include <Tokenizer.h>

void enableMenuItem(const CWnd *wnd, UINT id, bool enabled) {
  CMenu *menu = wnd->GetMenu();
  bool done = false;
  if(menu != NULL) {
    done = menu->EnableMenuItem(id, MF_BYCOMMAND | (enabled ? MF_ENABLED : MF_GRAYED)) != -1;
  }
}

static String getMenuString(HMENU menu, UINT pos) {
  TCHAR ms[2048];
  GetMenuString(menu, pos, ms, sizeof(ms)-1, MF_BYPOSITION);
  return ms;
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

static int findMenuItemIndexByString(const CMenu *menu, const String &s) {
  return findMenuItemIndexByString(menu->m_hMenu, s);
}

static CMenu *findMenuByString(const CWnd *wnd, const String &s, int &index) {
  CMenu *menu = wnd->GetMenu();
  if(menu == NULL) {
    return NULL;
  }
  for(Tokenizer tok(s,"/"); menu != NULL && tok.hasNext();) {
    String str = tok.next();
    index = findMenuItemIndexByString(menu, str);
    if(index < 0) {
      if((_stscanf(str.cstr(), _T("%d"), &index) != 1) || (index < 0)) {
        return NULL;
      }
    }

    if(tok.hasNext()) {
      menu = menu->GetSubMenu(index);
    } else {
      return menu;
    }
  }
  return NULL;
}

HMENU findMenuByString(HMENU menu, const String &s, int &index) {
  if(menu == NULL) {
    return NULL;
  }
  for(Tokenizer tok(s,"/"); menu != NULL && tok.hasNext();) {
    String str = tok.next();
    index = findMenuItemIndexByString(menu, str);
    if(index < 0) {
      if((_stscanf(str.cstr(), _T("%d"), &index) != 1) || (index < 0)) {
        return NULL;
      }
    }

    if(tok.hasNext()) {
      menu = GetSubMenu(menu, index);
    } else {
      return menu;
    }
  }
  return NULL;
}

static MENUITEMINFO getMenuItemInfo(HMENU menu, UINT pos, int mask) {
  MENUITEMINFO info;
  memset(&info, 0, sizeof(info));
  info.cbSize = sizeof(MENUITEMINFO);
  info.fMask = mask;
  GetMenuItemInfo(menu, pos, true, &info);
  return info;
}

int getMenuItemType(HMENU menu, UINT pos) {
  MENUITEMINFO info = getMenuItemInfo(menu, pos, MIIM_TYPE);
  return info.fType;
}

static CMenu *findMenuContainingId(CMenu *menu, UINT id, int &index) {
  if(menu == NULL) {
    return NULL;
  } else {
    const int count = menu->GetMenuItemCount();
    for(int i = 0; i < count; i++) {
      const MENUITEMINFO itemInfo = getMenuItemInfo(menu->m_hMenu, i, MIIM_SUBMENU);
      if(itemInfo.hSubMenu != NULL) {
        CMenu *sm = findMenuContainingId(menu->GetSubMenu(i), id, index);
        if(sm) {
          return sm;
        }
      } else if(menu->GetMenuItemID(i) == id) {
        index = i;
        return menu;
      }
    }
  }
  return NULL;
}

HMENU findMenuContainingId(HMENU menu, UINT id, int &index) {
  if(menu == NULL) {
    return NULL;
  } else {
    const int count = GetMenuItemCount(menu);
    for(int i = 0; i < count; i++) {
      MENUITEMINFO itemInfo = getMenuItemInfo(menu, i, MIIM_SUBMENU);
      if(itemInfo.hSubMenu != NULL) {
        HMENU sm = findMenuContainingId(itemInfo.hSubMenu /*GetSubMenu(menu, i)*/, id, index);
        if(sm) {
          return sm;
        }
      } else if(GetMenuItemID(menu, i) == id) {
        index = i;
        return menu;
      }
    }
  }
  return NULL;
}

static CMenu *findMenuContainingId(const CWnd *wnd, UINT id, int &index) {
  return findMenuContainingId(wnd->GetMenu(), id, index);
}

void enableMenuItem(const CWnd *wnd, const String &s, bool enabled) {
  int index;
  bool done = false;
  CMenu *menu = findMenuByString(wnd, s, index);
  if(menu != NULL) {
    done = menu->EnableMenuItem(index, MF_BYPOSITION | (enabled ? MF_ENABLED : MF_GRAYED)) != -1;
  }
}

bool enableSubMenuContainingId(const CWnd *wnd, UINT id, bool enabled) {
  return enableSubMenuContainingId(wnd->GetMenu(), id, enabled);
}

static bool menuContainsId(const CMenu *menu, UINT id) { // no recurse
  const int count = menu->GetMenuItemCount();
  for(int i = 0; i < count; i++) {
    if(menu->GetMenuItemID(i) == id) {
      return true;
    }
  }
  return false;
}

bool enableSubMenuContainingId(CMenu *menu, UINT id, bool enabled) {
  if(menu) {
    const int count = menu->GetMenuItemCount();
    for(int i = 0; i < count; i++) {
      const MENUITEMINFO itemInfo = getMenuItemInfo(menu->m_hMenu, i, MIIM_SUBMENU);
      if(itemInfo.hSubMenu != NULL) {
        if(menuContainsId(menu->GetSubMenu(i), id)) {
          menu->EnableMenuItem(i, MF_BYPOSITION | (enabled ? MF_ENABLED : MF_GRAYED));
          return true;
        }
        if(enableSubMenuContainingId(menu->GetSubMenu(i), id, enabled)) {
          return true;
        }
      }
    }
  }
  return false;
}

bool removeSubMenuContainingId(const CWnd *wnd,  UINT id) {
  return removeSubMenuContainingId(wnd->GetMenu(), id);
}

bool removeSubMenuContainingId(CMenu *menu, UINT id) {
  if(menu) {
    const int count = menu->GetMenuItemCount();
    for(int i = 0; i < count; i++) {
      const MENUITEMINFO itemInfo = getMenuItemInfo(menu->m_hMenu, i, MIIM_SUBMENU);
      if(itemInfo.hSubMenu != NULL) {
        if(menuContainsId(menu->GetSubMenu(i), id)) {
          menu->RemoveMenu(i, MF_BYPOSITION);
          return true;
        }
        if(removeSubMenuContainingId(menu->GetSubMenu(i), id)) {
          return true;
        }
      }
    }
  }
  return false;
}

bool isMenuItemChecked(const CWnd *wnd, UINT id) {
  CMenu *menu = wnd->GetMenu();
  return (menu == NULL) ? false : (menu->GetMenuState(id, MF_BYCOMMAND) & MF_CHECKED) ? true : false;
}

void checkMenuItem(const CWnd *wnd, UINT id, bool checked) {
  CMenu *menu = wnd->GetMenu();
  if(menu != NULL) {
    checkMenuItem(*menu, id, checked);
  }
}

void checkMenuItem(HMENU menu, UINT  id, bool checked) {
  bool done = CheckMenuItem(menu, id, checked ? MF_CHECKED : MF_UNCHECKED) != 0xffffffff ;
}

bool toggleMenuItem(const CWnd *wnd, UINT id) {
  const bool checked = !isMenuItemChecked(wnd, id);
  checkMenuItem(wnd,id,checked);
  return checked;
}

void removeMenuItem(const CWnd *wnd, UINT id) {
  int index;
  bool done = false;
  CMenu *menu = findMenuContainingId(wnd, id, index);
  if(menu != NULL) {
    done = menu->RemoveMenu(id, MF_BYCOMMAND) != 0;
  }
}

void removeMenuItem(const CWnd *wnd, const String &s) {
  int index;
  bool done = false;
  CMenu *menu = findMenuByString(wnd, s, index);
  if(menu != NULL) {
    done = menu->RemoveMenu(index, MF_BYPOSITION) != 0;
  }
}

HMENU getSubMenu(HMENU menu, const String &s) {
  int index;
  menu = findMenuByString(menu, s, index);
  if(menu) {
    return GetSubMenu(menu, index);
  } else {
    return NULL;
  }
}

void removeAllMenuItems(HMENU menu) {
  int n = GetMenuItemCount(menu);
  while(n--)  {
    const MENUITEMINFO info = getMenuItemInfo(menu, n, MIIM_SUBMENU);
    if(info.hSubMenu != NULL) {
      HMENU sm = GetSubMenu(menu, n);
      removeAllMenuItems(sm);
      RemoveMenu(menu, n, MF_BYPOSITION);
      DestroyMenu(sm);
    } else {
      RemoveMenu(menu, n, MF_BYPOSITION);
    }
  }
}

void removeMenuItem(CMenu *menu, UINT id) {
  bool done = menu->RemoveMenu(id, MF_BYCOMMAND) != 0;
}

bool menuItemExists(const CWnd *wnd, UINT id) {
  int index;
  return findMenuContainingId(wnd, id, index) != NULL;
}

void insertMenuItem(const CWnd *wnd, UINT afterId, int flags, const String &itemText, UINT commandId) { // flags = 0,MF_SEPARATOR
  int index;
  bool done = false;
  CMenu *menu = findMenuContainingId(wnd, afterId, index);
  if(menu != NULL) {
    done = menu->InsertMenu(afterId, MF_BYCOMMAND | (flags & MF_SEPARATOR), commandId, itemText.cstr()) != 0;
  }
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

void insertMenuSeparator(HMENU menu, UINT pos) {
  MENUITEMINFO info;
  info.cbSize     = sizeof(info);
  info.fMask      = MIIM_TYPE ;
  info.fType      = MFT_SEPARATOR;
  InsertMenuItem(menu, pos, true, &info);
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

void removeMenuItem(HMENU menu, UINT pos) {
  RemoveMenu(menu, MF_BYPOSITION, pos);
}

String getMenuItemText(const CWnd *wnd, UINT id) {
  int index;
  CMenu *menu = findMenuContainingId(wnd, id, index);
  if(menu == NULL) {
    return EMPTYSTRING;
  } else {
    MENUITEMINFO itemInfo;
    itemInfo.cbSize = sizeof(MENUITEMINFO);
    itemInfo.fMask = MIIM_TYPE;
    itemInfo.fType = MFT_STRING;
    itemInfo.dwTypeData = NULL;
    menu->GetMenuItemInfo(index,&itemInfo, true);
    const int bufferSize = itemInfo.cch+1;
    TCHAR *buffer = new TCHAR[bufferSize];
    itemInfo.dwTypeData = buffer;
    itemInfo.cch++;
    menu->GetMenuItemInfo(index, &itemInfo, true);
    String result = itemInfo.dwTypeData ? itemInfo.dwTypeData : EMPTYSTRING;
    delete[] buffer;
    return result;
  }
}

String setMenuItemText(const CWnd *wnd, UINT id, const String &itemText) {
  int index;
  CMenu *menu = findMenuContainingId(wnd, id, index);
  if(menu == NULL) {
    return EMPTYSTRING;
  } else {
    MENUITEMINFO itemInfo;
    itemInfo.cbSize = sizeof(MENUITEMINFO);
    itemInfo.fMask = MIIM_TYPE;
    itemInfo.fType = MFT_STRING;
    itemInfo.dwTypeData = NULL;
    menu->GetMenuItemInfo(index, &itemInfo, true);
    const int bufferSize = itemInfo.cch+1;
    TCHAR *buffer = new TCHAR[bufferSize];
    itemInfo.dwTypeData = buffer;
    itemInfo.cch++;
    menu->GetMenuItemInfo(index,&itemInfo, true);
    String result = itemInfo.dwTypeData ? itemInfo.dwTypeData : EMPTYSTRING;
    String tmp = itemText;
    delete[] buffer;
    itemInfo.dwTypeData = tmp.cstr();
    SetMenuItemInfo(menu->m_hMenu, index, true, &itemInfo);
    return result;
  }
}

String getMenuItemText(HMENU menu, int pos) {
  TCHAR str[4096];
  GetMenuString(menu, pos, str, sizeof(str), MF_BYPOSITION);
  LASTVALUE(str) = 0;
  return str;
}
