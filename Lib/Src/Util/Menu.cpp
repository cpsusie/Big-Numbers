#include "pch.h"
#include <Console.h>

#define   SUBESCAPE -1
#define   SUBLEFT   -2
#define   SUBRIGHT  -3

StaticMenuItem::StaticMenuItem(MenuItemType type, const String &text, int command) {
  m_type    = type;
  m_text    = text;
  m_command = command;
}

MenuItem::MenuItem(MenuItemType type, const String &text, int command) {
  m_type     = type;
  m_highlite = -1;

  String tmp(text);
  TCHAR *dst = tmp.cstr();

  // Substitute "&&" with "&", '&' with '' and save last occurence of '&' in m_highlite
  for(const TCHAR *cp = text.cstr(); *cp;) {
    switch(*cp) {
    case _T('&'):
      if(cp[1] == _T('&')) {
        *(dst++) = *(cp++);
        cp++;
      } else {
        cp++;
        m_highlite = (int)(dst - tmp.cstr());
      }
      break;

    default:
      *(dst++) = *(cp++);
      break;
    }
  }
  *dst = 0;
  m_text = tmp.cstr();

  m_subMenu = NULL;
  m_checked = false;
  m_command = command;
}

int Menu::runSubMenu(int i) {
  Menu *submenu = m_items[i]->m_subMenu;
  int left, top;
  if(m_isSubMenu) {
    left = m_left + m_width + 1;
    top  = m_top  + i;
  } else {
    left = m_left + findItemPos(i);
    top  = m_top + 1;
  }
  drawItem(i,true);
  const int ret = submenu->domodal(left,top);
  drawItem(i,false);
  return ret;

}

int Menu::findFirstItem(int start, int dir) {
  if(dir > 0) {
    for(int i = start + 1; i != start; i++) {
      if(i >= (int)m_items.size()) {
        if(start == -1) {
          return -1;
        }
        i = -1;
      } else if(m_items[i]->m_type != MENUITEMSEPARATOR && m_items[i]->m_enabled) {
        return i;
      }
    }
    return start;
  } else {
    for(int i = start - 1; i != start; i--) {
      if(i < 0) {
        i = (int)m_items.size();
      } else if(m_items[i]->m_type != MENUITEMSEPARATOR && m_items[i]->m_enabled) {
        return i;
      }
    }
    return start;
  }
}

int Menu::domodal(int left, int top) {
  int  ret = SUBESCAPE;
  int  i;
  int  cx,cy;
  bool wasVisible       = isVisible();
  bool cursorWasVisible = Console::isCursorVisible();

  draw(left,top);
  Console::getCursorPos(cx,cy);
  Console::showCursor(false);

  int selected;
  if(m_startmenu >= 0) {
    selected = m_startmenu;
    m_startmenu = -1;
  } else {
    selected = findFirstItem(-1,1);
  }

  if(selected < 0) {
    goto cleanup;
  }

  for(;;) {
startloop:
    drawItem(selected,true);

    int event;
    for(;;) {
      event = getEvent();
      if(EVENTDOWN(event)) {
        break;
      }
    }
    drawItem(selected,false);
    int scan;

    switch(scan = EVENTSCAN(event)) {
    case SCAN_ESCAPE:
      ret = SUBESCAPE;
      goto cleanup;

    case SCAN_ENTER :
      startenter:
      if(m_items[selected]->m_type == MENUITEMPOPUP) {
        ret = runSubMenu(selected);
        switch(ret) {
        case SUBESCAPE:
          if(m_isSubMenu) {
            goto cleanup;
          }
          continue;

        case SUBLEFT  :
          if(m_isSubMenu) {
            continue;
          }
          selected = findFirstItem(selected,-1);
          goto startenter;

        case SUBRIGHT :
          if(m_isSubMenu) {
            goto cleanup;
          }
          selected = findFirstItem(selected,1);
          goto startenter;

        default       :
          goto cleanup;
        }
      }
      ret = m_items[selected]->m_command;
      goto cleanup;

    case SCAN_RIGHT :
      if(m_isSubMenu) {
        if(m_items[selected]->m_type == MENUITEMPOPUP) {
          ret = runSubMenu(selected);
          if(ret == SUBLEFT) {
            continue;
          }
          goto cleanup;
        } else {
          ret = SUBRIGHT;
          goto cleanup;
        }
      } else {
        selected = findFirstItem(selected,1);
      }
      break;

    case SCAN_LEFT  :
      if(m_isSubMenu) {
        ret = SUBLEFT;
        goto cleanup;
      } else {
        selected = findFirstItem(selected,-1);
      }
      break;

    case SCAN_UP    :
      if(m_isSubMenu) {
        selected = findFirstItem(selected,-1);
      }
      break;

    case SCAN_DOWN  :
      if(m_isSubMenu) {
        selected = findFirstItem(selected,1);
      } else {
        startdown:
        if(m_items[selected]->m_type == MENUITEMPOPUP) {
          ret = runSubMenu(selected);
          switch(ret) {
          case SUBESCAPE:
            if(m_isSubMenu) {
              goto cleanup;
            }
            continue;

          case SUBLEFT  :
            if(m_isSubMenu) {
              continue;
            }
            selected = findFirstItem(selected,-1);
            goto startdown;

          case SUBRIGHT :
            if(m_isSubMenu) {
              goto cleanup;
            }
            selected = findFirstItem(selected,1);
            goto startdown;

          default       :
            goto cleanup;
          }
        }
      }
      break;

    case SCAN_HOME  :
      selected = findFirstItem(-1,1);
      break;

    case SCAN_END   :
      selected = findFirstItem((int)m_items.size(),-1);
      break;

    default         :
      { int ch = EVENTASCII(event);
        if(EVENTSTATE(event) & ALT_PRESSED || m_isSubMenu) {
          for(i = 0; i < (int)m_items.size(); i++) {
            MenuItem *item = m_items[i];
            if(item->m_highlite >= 0 && tolower(ch) == tolower(item->m_text[item->m_highlite]) && item->m_enabled) {
              if(item->m_type == MENUITEMPOPUP) {
                selected = i;
                ret = runSubMenu(i);
                switch(ret) {
                case SUBLEFT  :
                  goto startloop;

                case SUBESCAPE:
                case SUBRIGHT :
                default       :
                  goto cleanup;
                }
              }           
              ret = item->m_command;
              goto cleanup;
            }
          }
        }
      }
    }
  }

cleanup:
  if(!wasVisible) {
    destroy();
  }
  if(cursorWasVisible) {
    Console::showCursor(true);
  }
  Console::setCursorPos(cx,cy);
  m_startmenu = -1;
  return ret;
}

bool Menu::wantEvent(int event) {
  if(EVENTSCAN(event) == SCAN_ALT && EVENTUP(event)) {
    return true;
  }
  if(EVENTSTATE(event) & ALT_PRESSED) {
    int ch = EVENTASCII(event);
    for(int i = 0; i < (int)m_items.size(); i++) {
      MenuItem *item = m_items[i];
      if(item->m_highlite >= 0 && tolower(ch) == tolower(item->m_text[item->m_highlite]) && item->m_enabled) {
        return true;
      }
    }
  }
  return false;
}

bool Menu::handleEvent(int event) {
  if(m_hostWindow == NULL) {
    throwException("no hostwindow in menu");
  }
  if(EVENTSTATE(event) & ALT_PRESSED) {
    int ch = EVENTASCII(event);
    for(int i = 0; i < (int)m_items.size(); i++) {
      MenuItem *item = m_items[i];
      if(item->m_highlite >= 0 && tolower(ch) == tolower(item->m_text[item->m_highlite]) && item->m_enabled) {
        m_startmenu = i;
        postEvent(KEYEVENT(0,1,SCAN_ENTER,0));
        int ret = domodal(m_left,m_top);
        if(ret >= 0) {
          m_hostWindow->postEvent(COMMANDEVENT(ret));
          return true;
        }
      }
    }
    return false;
  }
  return false;
}

void Menu::findSize() {
  if(m_isSubMenu) {
    m_width = 0;
    for(int i = 0; i < (int)m_items.size(); i++) {
      MenuItem *item = m_items[i];
      if(item->m_type == MENUITEMTEXT || item->m_type == MENUITEMPOPUP) {
        int l = (int)item->m_text.length();
        if(l > m_width) {
          m_width = l;
        }
      }
    }

    m_width += 5;
    m_height = (int)m_items.size() + 1;
  } else {
    m_width = 0;
    for(int i = 0; i < (int)m_items.size(); i++) {
      m_width += m_items[i]->getItemWidth();
    }
    m_height = 1;
    if(m_hostWindow != NULL) {
      Dialog *dlg = (Dialog*)m_hostWindow;
      SMALL_RECT r;
      dlg->getRect(r);
      int w = r.Right - r.Left - 2;
      if(w > m_width) {
        m_width = w;
      }
    }
  }
}

int MenuItem::getItemWidth() {
  return (int)m_text.length() + 4;
}

int Menu::findItemPos(int i) {
  int x = 0;
  for(int k = 0; k < i; k++) {
    x += m_items[k]->getItemWidth();
  }
  return x;
}

void Menu::drawItem(int i, bool selected) {
  WORD attr, hlattr;
  if(selected) {
    attr = m_selTextColor;
  } else {
    attr = m_textColor;
  }
  MenuItem *item = m_items[i];
  if(item->m_enabled) {
    attr  |= FOREGROUND_INTENSITY;
    hlattr = BACKGROUNDCOLOR(attr) | FOREGROUNDCOLOR(m_highLiteColor) | FOREGROUND_INTENSITY;
  } else {
    attr  &= ~FOREGROUND_INTENSITY;
    hlattr = (BACKGROUNDCOLOR(attr) | FOREGROUNDCOLOR(m_highLiteColor)) & ~FOREGROUND_INTENSITY;
  }

  if(m_isSubMenu) {
    int x = 1;
    int y = i + 1;
    if(item->m_type == MENUITEMSEPARATOR) {
      String sp(spaceString(m_width-1,(char)196));
      getWin()->printf(x, y, attr, _T("%s"), sp.cstr());
//      getWin()->printf(x-1,y, m_borderColor, "%c",199);
//      getWin()->printf(x+m_width-1,y,m_borderColor, "%c",182);
    } else {
      getWin()->printf(x, y, attr, _T("%c %-*.*s"),
                       item->m_checked?251:' ', m_width - 3, m_width - 3, item->m_text.cstr());
      if(item->m_highlite >= 0) {
        getWin()->printf(x + item->m_highlite+2, y, hlattr,_T("%c"),item->m_text[item->m_highlite]);
      }
      if(item->m_subMenu) {
        getWin()->printf(m_width - 1, y, attr, _T("%c"),16);
      }
    }
  } else {
    int x = findItemPos(i);
    getWin()->printf(x, 0, attr, _T("  %s  "),item->m_text.cstr());
    if(item->m_highlite >= 0) {
      getWin()->printf(x + item->m_highlite+2, 0, hlattr,_T("%c"), item->m_text[item->m_highlite]);
    }
  }
}

void Menu::draw(int left, int top) {
  m_left = left;
  m_top  = top;
  findSize();
  if(m_isSubMenu) {
    int scrwidth,scrheight;
    Console::getBufferSize(scrwidth,scrheight);
    if(m_left + m_width >= scrwidth) {
      m_left = scrwidth - m_width   - 2;
    }
    if(m_top + m_height >= scrheight) {
      m_top  = scrheight - m_height - 2;
    }

    if(!isVisible()) {
      create(m_shadowed, m_left, m_top, m_left + m_width, m_top + m_height);
    }

    getWin()->rectangle(0, 0, m_width, m_height, DOUBLE_FRAME, m_borderColor);

    for(int i = 0; i < (int)m_items.size(); i++) {
      MenuItem *item = m_items[i];
      switch(item->m_type) {
      case MENUITEMTEXT     :
      case MENUITEMPOPUP    :
      case MENUITEMSEPARATOR:
        drawItem(i, false);
        break;
      }
    }
  } else {
    if(!isVisible()) {
      create(false, m_left, m_top, m_left + m_width, m_top + m_height-1);
    }
    getWin()->set(0,0, m_width, m_height-1, TR_ALL,' ',m_textColor);
    for(int i = 0; i < (int)m_items.size(); i++) {
      drawItem(i, false);
    }
  }
  ConsoleWindow::draw();
}

bool Menu::deleteItem(int id) {
  for(int i = 0; i < (int)m_items.size(); i++) {
    MenuItem *item = m_items[i];
    if(item->m_command == id) {
      if(item->m_subMenu) {
        delete item->m_subMenu;
      }
      m_items.removeIndex(i);
      delete item;
      return true;
    }
  }
  for(int i = 0; i < (int)m_items.size(); i++) {
    MenuItem *item = m_items[i];
    if(item->m_subMenu) {
      if(item->m_subMenu->deleteItem(id)) {
        return true;
      }
    }
  }
  return false;
}

void Menu::removeMenuItem(int id) {
  deleteItem(id);
}

MenuItem *Menu::findMenuItem(int id) {
  for(int i = 0; i < (int)m_items.size(); i++) {
    MenuItem *item = m_items[i];
    if(item->m_type == MENUITEMPOPUP) {
      MenuItem *si = item->m_subMenu->findMenuItem(id);
      if(si != NULL) {
        return si;
      }
    } else if(item->m_type == MENUITEMTEXT && item->m_command == id) {
      return item;
    }
  }
  return NULL;
}

void Menu::checkMenuItem(int id, bool checked) {
  MenuItem *m = findMenuItem(id);
  if(m) {
    m->m_checked = checked;
  }
}

bool Menu::menuItemChecked(int id) {
  MenuItem *m = findMenuItem(id);
  return m ? m->m_checked : false;
}

void Menu::enableMenuItem( int id, bool enable) {
  MenuItem *m = findMenuItem(id);
  if(m) {
    m->m_enabled = enable;
  }
}

bool Menu::menuItemEnabled(  int id) {
  MenuItem *m = findMenuItem(id);
  return m ? m->m_enabled : false;
}

int Menu::totalItems() {
  int sum = 0;
  for(int i = 0; i < (int)m_items.size(); i++) {
    if(m_items[i]->m_type == MENUITEMPOPUP) {
      sum += m_items[i]->m_subMenu->totalItems();
    }
  }
  return sum + (int)m_items.size() + 1;
}

Menu::Menu(StaticMenuItem *items) {
  MenuItem *item;
  Menu     *submenu;

  m_isSubMenu     = false;
  m_startmenu     = -1;
  m_borderColor   = NORMALVIDEO | FOREGROUND_INTENSITY | BACKGROUND_BLUE | BACKGROUND_GREEN;
  m_textColor     = NORMALVIDEO                        | BACKGROUND_BLUE | BACKGROUND_GREEN;
  m_selTextColor  = NORMALVIDEO;
  m_highLiteColor = FOREGROUND_YELLOW;
  m_hostWindow    = NULL;
  m_shadowed      = false;

  for(int i = 0; items[i].m_type != MENUITEMEND; ) {
    switch(items[i].m_type) {
    case MENUITEMTEXT     :
      m_items.add(new MenuItem(MENUITEMTEXT, items[i].m_text, items[i].m_command));
      i++;
      break;
    case MENUITEMSEPARATOR:
      m_items.add(new MenuItem(MENUITEMSEPARATOR, _T(""), 0));
      i++;
      break;
    case MENUITEMPOPUP    :
      item    = new MenuItem(MENUITEMPOPUP, items[i].m_text, 0);
      submenu = new Menu(items + i + 1);
      submenu->m_isSubMenu = true;
      item->m_subMenu = submenu;
      m_items.add(item);
      i += 1 + submenu->totalItems();
      break;
    }
  }

  int tabulatorPosition = 0;
  for(int i = 0; i < (int)m_items.size(); i++) {
    MenuItem *item = m_items[i];
    TCHAR *tab = _tcschr(item->m_text.cstr(),_T('\t'));
    int p;
    if(tab != NULL) {
      p = (int)(tab - item->m_text.cstr());
    } else {
      p = (int)item->m_text.length();
    }
    if(p > tabulatorPosition) {
      tabulatorPosition = p;
    }
  }

  if(tabulatorPosition > 0) {
    tabulatorPosition += 2;
    for(int i = 0; i < (int)m_items.size(); i++) {
      MenuItem *item = m_items[i];
      TCHAR *tab = _tcschr(item->m_text.cstr(),_T('\t'));
      if(tab != NULL) {
        int p = (int)(tab - item->m_text.cstr());
        item->m_text = left(item->m_text,p) + spaceString(tabulatorPosition - p) + substr(item->m_text, p + 1, item->m_text.length());
      }
    }
  }
}

Menu::~Menu() {
  for(int i = 0; i < (int)m_items.size(); i++) {
    MenuItem *item = m_items[i];
    if(item->m_subMenu != NULL) {
      delete item->m_subMenu;
    }
    delete item;
  }
}

void Menu::setHostWindow(ConsoleWindow *hostwindow) {
  m_hostWindow = hostwindow;
}

void Menu::setBorderColor( WORD color) {
  traverse(color,m_textColor,m_selTextColor,m_highLiteColor,m_shadowed);
}

void Menu::setTextColor(   WORD color) {
  traverse(m_borderColor,color,m_selTextColor,m_highLiteColor,m_shadowed);
}

void Menu::setSelTextColor(WORD color) {
  traverse(m_borderColor,m_textColor,color,m_highLiteColor,m_shadowed);
}

void Menu::setHighLiteColor(WORD color) {
  traverse(m_borderColor,m_textColor,m_selTextColor,color,m_shadowed);
}

void Menu::setShadow(bool on) {
  traverse(m_borderColor,m_textColor,m_selTextColor,m_highLiteColor,on);
}

void Menu::traverse(WORD c1, WORD c2, WORD c3, WORD c4, bool sh) {
  m_borderColor   = c1;
  m_textColor     = c2;
  m_selTextColor  = c3;
  m_highLiteColor = c4;
  m_shadowed      = sh;

  for(int i = 0; i < (int)m_items.size(); i++) {
    MenuItem *item = m_items[i];
    if(item->m_subMenu != NULL) {
      item->m_subMenu->traverse(c1,c2,c3,c4,sh);
    }
  }
}

int showMenu(int left, int top, StaticMenuItem *items) {
  Menu m(items);
  return m.m_items[0]->m_subMenu->domodal(left, top);
}
