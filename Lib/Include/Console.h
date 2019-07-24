#pragma once

#include "Array.h"
#include "StringArray.h"
#include "TextRect.h"
#include "TextWin.h"
#include "QueueList.h"
#include "Semaphore.h"

#define FOREGROUNDCOLOR(c) ((c) & 0xf)
#define BACKGROUNDCOLOR(c) ((c) & 0xf0)

// see vc98\include\wincon.h for a complete list of console-attributes
#define BACKGROUND_WHITE  BACKGROUND_RED   | BACKGROUND_GREEN | BACKGROUND_BLUE
#define FOREGROUND_WHITE  FOREGROUND_RED   | FOREGROUND_GREEN | FOREGROUND_BLUE
#define FOREGROUND_YELLOW FOREGROUND_RED   | FOREGROUND_GREEN
#define BACKGROUND_YELLOW BACKGROUND_RED   | BACKGROUND_GREEN
#define FOREGROUND_CYAN   FOREGROUND_RED   | FOREGROUND_BLUE
#define BACKGROUND_CYAN   BACKGROUND_RED   | BACKGROUND_BLUE
#define FOREGROUND_TURKEY FOREGROUND_GREEN | FOREGROUND_BLUE
#define BACKGROUND_TURKEY BACKGROUND_GREEN | BACKGROUND_BLUE

#define NORMALVIDEO  FOREGROUND_WHITE
#define REVERSEVIDEO BACKGROUND_WHITE

class Console {
private:
  static HANDLE    s_hStdIn;             // Handle for stdin
  static HANDLE    s_hStdOut;            // Handle for stdout
  static HANDLE    s_hStdErr;            // Handle for stderr
  static Semaphore s_gate;
  static void checkSysCall(DWORD c, int line);
  static void checkSysCall(BOOL  c, int line);

public:
  static HANDLE getHandle(int fileNo);
  static void  setBufferSize(int  width, int  height, int fileNo = STD_OUTPUT_HANDLE);
  static void  getBufferSize(int &width, int &height, int fileNo = STD_OUTPUT_HANDLE);
  static void  setWindowSize(int  width, int  height, int fileNo = STD_OUTPUT_HANDLE);
  static void  getWindowSize(int &width, int &height, int fileNo = STD_OUTPUT_HANDLE);
  static void  setBufferSize(const COORD &size,       int fileNo = STD_OUTPUT_HANDLE);
  static COORD getBufferSize(                         int fileNo = STD_OUTPUT_HANDLE);
  static void  setWindowSize(const COORD &size,       int fileNo = STD_OUTPUT_HANDLE);
  static COORD getWindowSize(                         int fileNo = STD_OUTPUT_HANDLE);
  static void  setWindowRect(const SMALL_RECT &r,     int fileNo = STD_OUTPUT_HANDLE);
  static SMALL_RECT getWindowRect(                    int fileNo = STD_OUTPUT_HANDLE);
  static HWND getWindow();
  static void setWindowRect(         int left, int top, int right, int bottom,  int fileNo = STD_OUTPUT_HANDLE);
  static void setWindowAndBufferSize(int left, int top, int right, int bottom,  int fileNo = STD_OUTPUT_HANDLE);
  static void setTitle(const String &title);
  static String getTitle();
  static CONSOLE_FONT_INFOEX getFont();
  static COORD               getFontSize();
  static void setFontSize(const COORD &fontSize);
  static void clear(                                                WORD attr = NORMALVIDEO, int fileNo = STD_OUTPUT_HANDLE);
  static void clearLine(int line,                                   WORD attr = NORMALVIDEO, int fileNo = STD_OUTPUT_HANDLE);
  static void clearRect(int left,   int top, int right, int bottom, WORD attr = NORMALVIDEO, int fileNo = STD_OUTPUT_HANDLE);
  static void rectangle(int left,   int top, int right, int bottom, FrameType type = SINGLE_FRAME, WORD color = NORMALVIDEO);
  static void printf(   int x   ,   int y,             _In_z_ _Printf_format_string_ TCHAR const * const format, ...);
  static void printf(   int x   ,   int y, WORD color, _In_z_ _Printf_format_string_ TCHAR const * const format, ...);
  static void cprintf(  int x   ,   int y,             _In_z_ _Printf_format_string_ TCHAR const * const format, ...);            // caret not moved
  static void cprintf(  int x   ,   int y, WORD color, _In_z_ _Printf_format_string_ TCHAR const * const format, ...);            // caret not moved
  static void vprintf(  int x   ,   int y,             _In_z_ _Printf_format_string_ TCHAR const * const format, va_list argptr);
  static void vprintf(  int x   ,   int y, WORD color, _In_z_ _Printf_format_string_ TCHAR const * const format, va_list argptr);
  static void vcprintf( int x   ,   int y,             _In_z_ _Printf_format_string_ TCHAR const * const format, va_list argptr); // caret not moved
  static void vcprintf( int x   ,   int y, WORD color, _In_z_ _Printf_format_string_ TCHAR const * const format, va_list argptr); // caret not moved
  static void setColor( WORD color,                                             int fileNo = STD_OUTPUT_HANDLE);
  static void setColor( int x   ,   int y, WORD color,                          int fileNo = STD_OUTPUT_HANDLE);
  static void setColor( int left,   int top, int right, int bottom, WORD color, int fileNo = STD_OUTPUT_HANDLE);
  static WORD getColor( int x   ,   int y,                                      int fileNo = STD_OUTPUT_HANDLE);
  static void setCursorPos(int  x, int  y, int fileNo = STD_OUTPUT_HANDLE);
  static void setCursorPos(const COORD &p, int fileNo = STD_OUTPUT_HANDLE);
  static void getCursorPos(int &x, int &y, int fileNo = STD_OUTPUT_HANDLE);
  static COORD getCursorPos(               int fileNo = STD_OUTPUT_HANDLE);
  static void setCursorSize(int percent,   int fileNo = STD_OUTPUT_HANDLE);
  static int  getCursorSize(               int fileNo = STD_OUTPUT_HANDLE);
  static void getLargestConsoleWindowSize(int &maxWidth, int &maxHeight, int fileNo = STD_OUTPUT_HANDLE);

  static void showCursor(bool on,          int fileNo = STD_OUTPUT_HANDLE);
  static bool isCursorVisible(             int fileNo = STD_OUTPUT_HANDLE);
  static int  getKey();      // returns ascii
  static int  getKeyEvent(); // returns event
  static bool keyPressed();
};

#ifdef UNICODE
#define KEY_EVENT_RECORD_TCHAR(keyEvent) ((keyEvent).uChar.UnicodeChar)
#else
#define KEY_EVENT_RECORD_TCHAR(keyEvent) ((keyEvent).uChar.AsciiChar)
#endif

// return from getKey
#define KEY_ENTER          13
#define KEY_ESCAPE         27

// return from getKeyEvent
#define SCAN_ESCAPE         1
#define SCAN_BACKSPACE     14
#define SCAN_TAB           15
#define SCAN_ENTER         28
#define SCAN_CTRL          29
#define SCAN_LEFTSHIFT     42
#define SCAN_NUMDIV        53
#define SCAN_RIGHTSHIFT    54
#define SCAN_NUMMULT       55
#define SCAN_ALT           56
#define SCAN_SPACE         57
#define SCAN_CAPSLOCK      58
#define SCAN_NUMLOCK       69
#define SCAN_HOME          71
#define SCAN_UP            72
#define SCAN_PGUP          73
#define SCAN_NUMMINUS      74
#define SCAN_LEFT          75
#define SCAN_RIGHT         77
#define SCAN_NUMPLUS       78
#define SCAN_END           79
#define SCAN_DOWN          80
#define SCAN_PGDOWN        81
#define SCAN_INSERT        82
#define SCAN_DELETE        83
#define SCAN_F1            59
#define SCAN_F2            60
#define SCAN_F3            61
#define SCAN_F4            62
#define SCAN_F5            63
#define SCAN_F6            64
#define SCAN_F7            65
#define SCAN_F8            66
#define SCAN_F9            67
#define SCAN_F10           68
#define SCAN_F11           87
#define SCAN_F12           88

#define EVENTTYPE_KEY      1
#define EVENTTYPE_COMMAND  2

#define EVENTTYPE(   event)                   ((event) >> 25)
#define EVENTSTATE(  event)                  (((event) >> 16) & 0xff)
#define EVENTDOWN(   event)                  ((EVENTTYPE(event)==EVENTTYPE_KEY) && (((event) >> 24) & 0x1))
#define EVENTUP(     event)                  ((EVENTTYPE(event)==EVENTTYPE_KEY) && !EVENTDOWN(event))
#define EVENTSCAN(   event)                  (((event) >>  8) & 0xff)
#define EVENTASCII(  event)                   ((event)        & 0xff)
#define EVENTCOMMAND(event)                   ((event) & 0xffffff)

#define EVENT2(type, arg1)                    (((type)          << 25) | ((arg1) & 0xffffff))
#define EVENT3(type, arg1, arg2)              (((type)          << 25) \
                                            | (((arg1)  & 0xff) << 16) \
                                            |  ((arg2)  & 0xffff))
#define EVENT5(type, state, down, scan, ascii) (((type)         << 25) \
                                            | (((down)  & 0x1)  << 24) \
                                            | (((state) & 0xff) << 16) \
                                            | (((scan)  & 0xff) <<  8) \
                                            | (((ascii) & 0xff) & 0xff))


#define KEYEVENT(state, down, scan, ascii) EVENT5(EVENTTYPE_KEY, state, down, scan, ascii)
#define COMMANDEVENT(command)              EVENT2(EVENTTYPE_COMMAND, command)

#define CTRL_PRESSED (LEFT_CTRL_PRESSED | RIGHT_CTRL_PRESSED)
#define ALT_PRESSED  (LEFT_ALT_PRESSED  | RIGHT_ALT_PRESSED )

#define MSG_QUIT      COMMANDEVENT(0x100000)
#define MSG_HSCROLL   COMMANDEVENT(0x100001)
#define MSG_VSCROLL   COMMANDEVENT(0x100002)
#define MSG_SELCHANGE COMMANDEVENT(0x100003)
#define MSG_CHANGE    COMMANDEVENT(0x100004)
//#define MSG_COMMAND   0x100005

class AcceleratorItem {
public:
  int m_keyevent;
  int m_command;
  AcceleratorItem(int key, int command);
  bool catchevent(int event);
};

#define ACCELITEM(state, scan, ascii, command) AcceleratorItem(KEYEVENT(state, 1, scan, ascii), COMMANDEVENT(command))
#define ACCELEND                            AcceleratorItem(0, 0)

class Accelerator : public Array<AcceleratorItem> {
public:
  Accelerator() {}
  Accelerator(AcceleratorItem *items);
};

class ConsoleWindow {
private:
  TextWin              *m_tw;
  ConsoleWindow        *m_parent;
  Array<ConsoleWindow*> m_children;
  bool                  m_shadow;
  QueueList<int>        m_eventQueue;
  Accelerator           m_accel;
  void drawShadow();
  void hideShadow();
public:
  ConsoleWindow();
  ~ConsoleWindow();
  void create(bool shadowed, int left, int top, int right, int bottom, ConsoleWindow *parent = NULL);
  void destroy();

  bool isVisible() const {
    return m_tw != NULL;
  }

  void setShadow(bool on);

  bool isShadowed() const {
    return m_shadow;
  }

  void setAccelerator(Accelerator &accel) {
    m_accel = accel;
  }

  bool translateEvent(int event);
  int  getEvent();
  void postEvent(int event);

  TextWin *getWin() {
    return m_tw;
  }

  virtual void draw();
};

typedef enum {
  MENUITEMTEXT
 ,MENUITEMSEPARATOR
 ,MENUITEMEND
 ,MENUITEMPOPUP
} MenuItemType;

class Menu;

class StaticMenuItem {
private:
  MenuItemType  m_type;
  String        m_text;
  int           m_command;
public:
  StaticMenuItem(MenuItemType type, const String &text, int command);
  friend class Menu;
};

class MenuItem {
public:
  MenuItemType m_type;
  short        m_highlite;
  String       m_text;
  Menu        *m_subMenu;
  bool         m_checked;
  bool         m_enabled;
  int          m_command;
  MenuItem(MenuItemType type, const String &text, int command);
  int getItemWidth();
};

class Menu : public ConsoleWindow {
private:
  CompactArray<MenuItem*> m_items;
  int                     m_left, m_top, m_width, m_height, m_startmenu;
  bool                    m_isSubMenu;
  WORD                    m_borderColor, m_textColor, m_selTextColor, m_highLiteColor;
  ConsoleWindow          *m_hostWindow;
  bool                    m_shadowed;
  void drawItem(int i, bool selected);
  int  totalItems();
  void findSize();
  int  runSubMenu(       int i);
  int  findItemPos(      int i);
  int  findFirstItem(    int start, int dir);
  void traverse(WORD c1, WORD c2, WORD c3, WORD c4, bool sh);
  MenuItem *findMenuItem(int id);
  bool deleteItem(       int id);
public:
  void draw(             int left, int top);
  int  domodal(          int left, int top);
  void setBorderColor(  WORD color);
  void setTextColor(    WORD color);
  void setSelTextColor( WORD color);
  void setHighLiteColor(WORD color);
  void removeMenuItem(       int id);
  void checkMenuItem(        int id, bool checked);
  bool menuItemChecked(      int id);
  void enableMenuItem(       int id, bool enable);
  bool menuItemEnabled(      int id);
  bool handleEvent(      int event);
  bool wantEvent(        int event);
  void setHostWindow(ConsoleWindow *hostwindow);
  void setShadow(bool on);

  bool isShadow() const {
    return m_shadowed;
  }

  friend int showMenu(int left, int top, StaticMenuItem *items);

  Menu(StaticMenuItem *items);
  ~Menu();
};

#define MENU(m)             StaticMenuItem m[]
#define MENUPOPUP(text)     StaticMenuItem(MENUITEMPOPUP    , text  , 0   )
#define MENUSEPARATOR       StaticMenuItem(MENUITEMSEPARATOR, _T(""), 0   )
#define MENUITEM(text, cmd) StaticMenuItem(MENUITEMTEXT     , text  , cmd )
#define MENUEND             StaticMenuItem(MENUITEMEND      , _T(""), 0   )

int  showMenu(int left, int top, StaticMenuItem *items);

#define MSGBUTTON_OK          1
#define MSGBUTTON_OKCANCEL    2
#define MSGBUTTON_YESNO       3
#define MSGBUTTON_YESNOCANCEL 4

#define MSB_OK     1
#define MSB_YES    2
#define MSB_NO     3
#define MSB_CANCEL 4

int messageBox(const String &str, int msgtype = MSGBUTTON_OK);

class Dialog;

class DialogControl {
private:
  Dialog &m_dlg;
  int     m_id, m_left, m_top, m_width, m_height, m_tabIndex;
  bool    m_enabled, m_visible;
public:
  DialogControl(Dialog *dlg, int id, int left, int top, int width, int height);
  virtual ~DialogControl() {};

  Dialog &getDialog() {
    return m_dlg;
  }

  int getLeft() const {
    return m_left;
  }

  int getTop() const {
    return m_top;
  }

  int getWidth() const {
    return m_width;
  }

  int getHeight() const {
    return m_height;
  }

  int getid() const {
    return m_id;
  }

  int getTabIndex() const {
    return m_tabIndex;
  }

  void setEnable(bool v);

  bool isEnabled() const {
    return m_enabled;
  }

  void setVisible(bool v);

  bool isVisible()  const {
    return m_visible;
  }

  void setTabIndex(int tabindex);
  WORD getColor();

  virtual void draw() {
  }

  virtual void enterDialog() {
  }

  virtual void gotFocus() {
  }

  virtual bool key(int event) {
    return false;
  }

  virtual void killFocus() {
  }

  virtual void exitDialog() {
  };
};

class DialogField : public DialogControl {
private:
  String  m_str;
  short   m_selStart, m_selEnd;
  int     m_cursorPos;
  String  m_label;
  void    copyToClipboard();
  void    copyFromClipboard();
protected:
  void    setCursorPos();
  void    postChangeEvent();
  void    setBuffer(const TCHAR *str);
public:
  DialogField(Dialog *dlg, int id, int left, int top, int width, int height, const String &label);
  void    draw();
  virtual void dataToField() = 0;
  virtual void fieldToData() = 0;

  const String &getBuffer() const {
    return m_str;
  }

  int getCursorPos() const {
    return m_cursorPos;
  }

  void  setCursorPos(int x);
  void  setLabel(const String &label);

  const String &getLabel() const {
    return m_label;
  }

  void charRight();

  void charLeft() {
    setCursorPos(getCursorPos()-1);
  }

  void home() {
    setCursorPos(0);
  }

  void end() {
    setCursorPos((int)m_str.length());
  }

  void setSel(int  start, int  end);
  void getSel(int &start, int &end);
  String getSelText();

  int getFieldPos() const {
    return getLeft() + (int)m_label.length();
  }

  bool isSelected()  const {
    return (0 <= m_selStart) && (m_selStart <= m_selEnd);
  }

  void unSelect() {
    setSel(0, -1);
  }

  void deleteChar();
  void deleteChar(int pos);
  void deleteSelected();
  void substSelected(TCHAR ch);
  bool insertChar(   TCHAR ch);
  bool navigate(     int event);
};

class StringField : public DialogField {
private:
  String m_var;
public:
  StringField(Dialog *dlg, int id, int left, int top, int width, int height, const String &label);
  String &getString() {
    return m_var;
  }
  void setString(const String &str);
  void enterDialog();
  void gotFocus();
  bool key(int event);
  void killFocus();
  void exitDialog();
  void dataToField();
  void fieldToData();
};

class IntField : public DialogField {
private:
  int m_var;
public:
  IntField(Dialog *dlg, int id, int left, int top, int width, int height, const String &label);
  int &getInt() {
    return m_var;
  }
  void enterDialog();
  void gotFocus();
  bool key(int event);
  void killFocus();
  void exitDialog();
  void dataToField();
  void fieldToData();
};

class DoubleField : public DialogField {
private:
  double m_var;
public:
  DoubleField(Dialog *dlg, int id, int left, int top, int width, int height, const String &label);
  double &getDouble() {
    return m_var;
  }
  void enterDialog();
  void gotFocus();
  bool key(int event);
  void killFocus();
  void exitDialog();
  void dataToField();
  void fieldToData();
};

class ShortField : public DialogField {
private:
  short m_var;
public:
  ShortField(Dialog *dlg, int id, int left, int top, int width, int height, const String &label);
  short &getShort() {
    return m_var;
  }
  void enterDialog();
  void gotFocus();
  bool key(int event);
  void killFocus();
  void exitDialog();
  void dataToField();
  void fieldToData();
};

class BoolField : public DialogField {
private:
  bool m_var;
public:
  BoolField(Dialog *dlg, int id, int left, int top, int width, int height, const String &label);
  bool &getBool() {
    return m_var;
  }
  void enterDialog();
  void gotFocus();
  bool key(int event);
  void killFocus();
  void exitDialog();
  void dataToField();
  void fieldToData();
};

class ComboBox : public DialogField {
private:
  String        m_var;
  StringArray   m_list;
  int           m_currentSel;
  int           m_scrollOffset;
  int           m_dropDownWidth;
  int           m_dropDownHeight;
  TextWin      *m_tw;

  bool isDropDownVisible() const {
    return m_tw != NULL;
  }

  void beginDropDown();
  void drawItem(int i, bool selected);
  void drawItem(int i);
  void drawScrollBar();
  void drawBorder();
  void drawDropDown();
  void hideDropDown();
  int  getRealDropDownHeight();
  void setScrollOffset(int i);

  int  getNoOfVisibleItems() {
    return getRealDropDownHeight() - 2;
  }

  int  getMaxScrollOffset();
  bool isItemVisible(int i);
public:
  ComboBox(Dialog *dlg, int id, int left, int top, int width, int height, const String &label);
  ~ComboBox();
  String &getString() { return m_var; }
  void addString(const String &str);
  void setString(const String &str);
  void setSel(int i);
  void setDropDownHeight(int newvalue);
  int  getDropDownHeight() const;
  void draw();
  void enterDialog();
  void gotFocus();
  bool key(int event);
  void killFocus();
  void exitDialog();
  void dataToField();
  void fieldToData();
};

class DialogButton : public DialogControl {
private:
  String m_label;
  bool   m_pressed;
public:
  DialogButton(Dialog *dlg, int id, int left, int top, int width, int height, const String &label);
  void draw();
  void enterDialog();
  void gotFocus();
  bool key(int event);
  void killFocus();
  void exitDialog();
  const String &label() const {
    return m_label;
  }
};

#define SCROLLBAR_VERT 1
#define SCROLLBAR_HORZ 2

class ScrollBar : public DialogControl {
private:
  int m_min, m_max, m_pos;
  int m_type;
  int m_pageSize;
  WORD m_color, m_barColor;
public:
  ScrollBar(Dialog *dlg, int type, int id, int left, int top, int width, int height);
  void setRange(int  Min, int  Max, int  pagesize = 1);
  void getRange(int &Min, int &Max, int &pagesize);
  void setPos(  int pos );
  int  getPos() { return m_pos; }
  void setColor(WORD color , WORD barcolor);
  void getColor(WORD &color, WORD &barcolor);
  void draw();
  void enterDialog();
  void gotFocus();
  bool key(int event);
  void killFocus();
  void exitDialog();
  static void drawvScrollBar(TextWin *tw, int left, int top, int height, int pageSize, int itemCount, int maxPos, int pos, WORD color, WORD barColor, FrameType borderType = NOFRAME);
  static void drawhScrollBar(TextWin *tw, int left, int top, int width , int pageSize, int itemCount, int maxPos, int pos, WORD color, WORD barColor, FrameType borderType = NOFRAME);
};

class ListBox : public DialogControl {
private:
  StringArray   m_strings;
  int           m_currentSel;
  int           m_scrollOffset;
  void drawItem(int i, bool selected);
  void drawItem(int i);
  void drawBorder();
  bool isItemVisible(int i) const;
  void setScrollOffset(int i);
  int getMaxScrollOffset();

  int getVisibleItemCount() const {
    return getHeight() - 2;
  }

  void drawScrollBar();
public:
  ListBox(Dialog *dlg, int id, int left, int top, int width, int height, const String &label);
  ~ListBox();

  String &getString(int index) {
    return m_strings[index];
  }

  int getCurSel() const {
    return m_currentSel;
  }

  int getCount() const {
    return (int)m_strings.size();
  }

  void setSel(int i);
  void addString(          const String &str);
  void insertString(int i, const String &str);
  void removeString(int i);
  void draw();
  void enterDialog();
  void gotFocus();
  bool key(int event);
  void killFocus();
  void exitDialog();
};

#define PROGRESSBAR_VERT 1
#define PROGRESSBAR_HORZ 2

class ProgressBar : public DialogControl {
private:
  int  m_type;
  int  m_max;
  int  m_pos;
  WORD m_color, m_barColor;
public:
  ProgressBar(Dialog *dlg, int type, int id, int left, int top, int width, int height);
  bool setMax(int value);
  void setPos(int value);

  int getMax() const {
    return m_max;
  }

  int getPos() const {
    return m_pos;
  }

  void setColor(WORD color , WORD barcolor);
  void getColor(WORD &color, WORD &barcolor);
  static void drawvProgressBar(TextWin *tw, int left, int top, int height, int maxPos, int pos, WORD color, WORD barColor);
  static void drawhProgressBar(TextWin *tw, int left, int top, int width , int maxPos, int pos, WORD color, WORD barColor);
  void draw();
};

class DialogString {
public:
  int    m_left, m_top;
  String m_str;
  DialogString(int left, int top, const String &str);
};

class Dialog : public ConsoleWindow {
private:
  Array<DialogControl*> m_controls;
  Array<DialogString>   m_strings;
  String                m_title;
  int                   m_left, m_top, m_width, m_height;
  int                   m_currentField;
  WORD                  m_color;
  WORD                  m_borderColor;
  WORD                  m_fieldColor;
  WORD                  m_selFieldColor;
  WORD                  m_selTextColor;
  FrameType             m_borderType;
  int                   m_return;
  Menu                 *m_menu;
  bool                  m_shadowed;

  void                  checkIdUnique(int id);
  int                   findControlIndex(int id);
  void                  drawBorder();
  void                  drawStrings();
  void                  drawControls();
  void                  drawMenu();
  int                   findNextTabIndex();
public:
  Dialog(const String &title, int left, int top, int width, int height);
  ~Dialog();
  virtual void onOk();
  virtual void onCancel();
  virtual bool eventHandler(int event);
  int domodal();

  void addIntField(    int id, int left, int top, int length    , const String &label = _T(""));
  void addShortField(  int id, int left, int top, int length    , const String &label = _T(""));
  void addDoubleField( int id, int left, int top, int length    , const String &label = _T(""));
  void addStringField( int id, int left, int top, int length    , const String &label = _T(""));
  void addBoolField(   int id, int left, int top, int length = 1, const String &label = _T(""));
  void addComboBox(    int id, int left, int top, int length    , const String &label = _T(""));
  void addButton(      int id, int left, int top, int width, int height, const String &label);
  void addListBox(     int id, int left, int top, int width, int height, const String &label = _T(""));
  void addControl(DialogControl *control);
  void setMenu(Menu *m);
  Menu *getMenu();
  void firstField();
  void lastField();
  void nextField();
  void prevField();
  bool fieldUp();
  bool fieldDown();
  bool fieldRight();
  bool fieldLeft();
  bool interField(int event);
  bool gotoField( int index);

  DialogControl *getCurrentControl() {
    return (m_currentField < 0) ? NULL : m_controls[m_currentField];
  }
  DialogControl *getDlgItem(int id);
  void draw();
  void quit(int ret);
  void setColor(WORD color);

  WORD getColor() const {
    return m_color;
  }
  void setBorderColor(WORD color);
  void setBorderType(FrameType bordertype);

  void setNormalFieldColor(WORD color);

  WORD getNormalFieldColor() const {
    return m_fieldColor;
  }

  void setSelectedFieldColor(WORD color);

  WORD getSelectedFieldColor() const {
    return m_selFieldColor;
  }

  void setSelectedTextColor(WORD color);

  WORD getSelectedTextColor() const {
    return m_selTextColor;
  }

  void setVar(  int id, int     n);
  void setVar(  int id, short   n);
  void setVar(  int id, double  n);
  void setVar(  int id, const String &s);
  void setVar(  int id, bool    b);
  void getVar(  int id, int    &n);
  void getVar(  int id, short  &n);
  void getVar(  int id, double &n);
  void getVar(  int id, String &s);
  void getVar(  int id, bool   &b);
  void printf(  int left, int top, _In_z_ _Printf_format_string_ TCHAR const * const format, ...);
  void setTitle(const String &title);

  const String &getTitle() const {
    return m_title;
  }

  void setShadow(bool on);

  bool isShadowed() const {
    return m_shadowed;
  }

  void setLabel(int id, const String &label);
  void getRect(SMALL_RECT &rect);

  int getTop() const {
    return m_top;
  }

  int getLeft() const {
    return m_left;
  }

  int getBottom() const {
    return m_top  + m_height;
  }

  int getRight() const {
    return m_left + m_width;
  }

  friend class DialogControl;
};
