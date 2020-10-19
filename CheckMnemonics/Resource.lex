%{
#pragma once

 /* This part goes to the ResourceLex.h file */

#include <LRParser.h>

class ResourceLex : public Scanner {
private:
  LRparser *m_parser; // if set, redirect errors to parsers verror-routine
public:
  ResourceLex(LexStream *inputStream = NULL) : Scanner(inputStream) {
    m_parser = NULL;
  }
  void setParser(LRparser *parser) {
    m_parser = parser;
  }
  int  getNextLexeme() override;
  void verror(const SourcePosition &pos, const TCHAR *format, va_list argptr) override;
  static void findBestHashMapSize();
};

%}

%{

 /* This part goes to the first part of the ResourceLex.cpp file */

#include "stdafx.h"
#include <HashMap.h>
#include <TreeMap.h>
#include "ResourceSymbol.h"
#include "ResourceLex.h"

static ResourceInputSymbol nameOrKeyWord(const TCHAR *lexeme);
static ResourceInputSymbol preprocssessorDirective(const TCHAR *lexeme);

%}

preplet [a-z]
let     [_a-zA-Z]             /* Letter                                 */
alnum   [_a-zA-Z0-9]          /* Alphanumeric character                 */
d       [0-9]                 /* Decimal digit                          */
h       [0-9a-fA-F]           /* Hexadecimal digit                      */
white   [\x00-\x09\x0b\s]     /* White space: all control chars but \n  */

%%

{d}+                    |
0[xX]{h}+[lL]?          return NUMBER;

(\"(\\.|[^\\\"]|\"\")*\") return STRING;

"("                     return LPAR;
")"                     return RPAR;
"|"                     return OR;

","                     return COMMA;
"."                     return DOT;

"//"                    { int i;
                          while(i = input()) {
                            if(i < 0) {
                              flushBuf();  /* Discard lexeme. */
                            } else if(i == '\n') {
                              break;
                            }
                          }
                        }

"/*"                    { int i;
                          SourcePosition startpos = getPos();
                          while(i = input()) {
                            if(i < 0) {
                              flushBuf();  /* Discard lexeme. */
                            } else if(i == '*' && look(1) == '/') {
                              input();
                              break;       /* Recognized comment.*/
                            }
                          }
                          if(i == 0) {
                            error( startpos,_T("End of file in comment\n") );
                          }
                        }

#{white}*{preplet}+     { int i;
                          while(i = input()) { // skip until end of line ie handle as a comment
                            if(i < 0) {
                              flushBuf();  /* Discard lexeme. */
                            } else if(i == '\\' && (look(1) == '\n' || look(1) == '\r')) {
                              flushBuf();
                              if(input() == '\r') { // eat '\r' and '\n' and skip next line too
                                input();
                              }
                            } else if(i == '\n') {
                              break;
                            }
                          }
                        }

{let}{alnum}*           return nameOrKeyWord(getText());
{white}+                ;
\n+                     ;
\r+                     ;

%%

  /* This part goes to the last part of ResourceLex.cpp */

typedef struct {
  TCHAR              *m_name;
  ResourceInputSymbol m_token;
} KeyWord;

#define KW(w)  _T(#w), w
#define UKW(w) _T(#w), _##w

static const KeyWord keywordTable[] = {
  KW(DIALOG)
 ,KW(DIALOGEX)
 ,UKW(BITMAP)
 ,KW(STRINGTABLE)
 ,KW(GUIDELINES)
 ,KW(DESIGNINFO)
 ,KW(LEFTMARGIN)
 ,KW(RIGHTMARGIN)
 ,KW(TOPMARGIN)
 ,KW(BOTTOMMARGIN)
 ,KW(HORZGUIDE)
 ,KW(VERTGUIDE)
 ,KW(TOOLBAR)
 ,KW(BUTTON)
 ,KW(ACCELERATORS)
 ,KW(VIRTKEY)
 ,KW(ALT)
 ,KW(SHIFT)
 ,KW(NOINVERT)
 ,KW(ASCII)
 ,KW(RCDATA)
 ,KW(DLGINIT)
 ,UKW(MENU)
 ,KW(MENUEX)
 ,KW(POPUP)
 ,UKW(MENUITEM)
 ,KW(GRAYED)
 ,KW(CHECKED)
 ,KW(INACTIVE)
 ,KW(SEPARATOR)
 ,KW(TEXTINCLUDE)
 ,KW(LANGUAGE)
 ,KW(VERSIONINFO)
 ,KW(FILEVERSION)
 ,KW(PRODUCTVERSION)
 ,KW(FILEFLAGSMASK)
 ,KW(FILEFLAGS)
 ,KW(FILEOS)
 ,KW(FILETYPE)
 ,KW(FILESUBTYPE)
 ,KW(BLOCK)
 ,KW(VALUE)
 ,KW(TYPELIB)
 ,KW(PUSHBUTTON)
 ,KW(DEFPUSHBUTTON)
 ,KW(EDITTEXT)
 ,KW(SCROLLBAR)
 ,KW(COMBOBOX)
 ,KW(LISTBOX)
 ,KW(GROUPBOX)
 ,UKW(ICON)
 ,KW(LTEXT)
 ,KW(RTEXT)
 ,KW(CTEXT)
 ,KW(CONTROL)
 ,UKW(FONT)
 ,KW(STYLE)
 ,KW(EXSTYLE)
 ,KW(CAPTION)
 ,KW(DISCARDABLE)
 ,KW(MOVEABLE)
 ,UKW(PURE)
 ,KW(IMPURE)
 ,KW(PRELOAD)
 ,KW(LOADONCALL)
 ,KW(AFX_DIALOG_LAYOUT)
 ,UKW(WS_OVERLAPPED)
 ,UKW(WS_POPUP)
 ,UKW(WS_CHILD)
 ,UKW(WS_MINIMIZE)
 ,UKW(WS_VISIBLE)
 ,UKW(WS_DISABLED)
 ,UKW(WS_CLIPSIBLINGS)
 ,UKW(WS_CLIPCHILDREN)
 ,UKW(WS_MAXIMIZE)
 ,UKW(WS_CAPTION)
 ,UKW(WS_BORDER)
 ,UKW(WS_DLGFRAME)
 ,UKW(WS_VSCROLL)
 ,UKW(WS_HSCROLL)
 ,UKW(WS_SYSMENU)
 ,UKW(WS_THICKFRAME)
 ,UKW(WS_GROUP)
 ,UKW(WS_TABSTOP)
 ,UKW(WS_MINIMIZEBOX)
 ,UKW(WS_MAXIMIZEBOX)
 ,UKW(WS_EX_DLGMODALFRAME)
 ,UKW(WS_EX_NOPARENTNOTIFY)
 ,UKW(WS_EX_TOPMOST)
 ,UKW(WS_EX_ACCEPTFILES)
 ,UKW(WS_EX_TRANSPARENT)
 ,UKW(WS_EX_MDICHILD)
 ,UKW(WS_EX_TOOLWINDOW)
 ,UKW(WS_EX_WINDOWEDGE)
 ,UKW(WS_EX_CLIENTEDGE)
 ,UKW(WS_EX_CONTEXTHELP)
 ,UKW(WS_EX_RIGHT)
 ,UKW(WS_EX_LEFT)
 ,UKW(WS_EX_RTLREADING)
 ,UKW(WS_EX_LTRREADING)
 ,UKW(WS_EX_LEFTSCROLLBAR)
 ,UKW(WS_EX_RIGHTSCROLLBAR)
 ,UKW(WS_EX_CONTROLPARENT)
 ,UKW(WS_EX_STATICEDGE)
 ,UKW(WS_EX_APPWINDOW)
 ,UKW(SS_LEFT)
 ,UKW(SS_CENTER)
 ,UKW(SS_RIGHT)
 ,UKW(SS_ICON)
 ,UKW(SS_BLACKRECT)
 ,UKW(SS_GRAYRECT)
 ,UKW(SS_WHITERECT)
 ,UKW(SS_BLACKFRAME)
 ,UKW(SS_GRAYFRAME)
 ,UKW(SS_WHITEFRAME)
 ,UKW(SS_USERITEM)
 ,UKW(SS_SIMPLE)
 ,UKW(SS_LEFTNOWORDWRAP)
 ,UKW(SS_OWNERDRAW)
 ,UKW(SS_BITMAP)
 ,UKW(SS_ENHMETAFILE)
 ,UKW(SS_ETCHEDHORZ)
 ,UKW(SS_ETCHEDVERT)
 ,UKW(SS_ETCHEDFRAME)
 ,UKW(SS_TYPEMASK)
 ,UKW(SS_NOPREFIX)
 ,UKW(SS_NOTIFY)
 ,UKW(SS_CENTERIMAGE)
 ,UKW(SS_RIGHTJUST)
 ,UKW(SS_REALSIZEIMAGE)
 ,UKW(SS_SUNKEN)
 ,UKW(SS_ENDELLIPSIS)
 ,UKW(SS_PATHELLIPSIS)
 ,UKW(SS_WORDELLIPSIS)
 ,UKW(SS_ELLIPSISMASK)
 ,UKW(ES_LEFT)
 ,UKW(ES_CENTER)
 ,UKW(ES_RIGHT)
 ,UKW(ES_MULTILINE)
 ,UKW(ES_UPPERCASE)
 ,UKW(ES_LOWERCASE)
 ,UKW(ES_PASSWORD)
 ,UKW(ES_AUTOVSCROLL)
 ,UKW(ES_AUTOHSCROLL)
 ,UKW(ES_NOHIDESEL)
 ,UKW(ES_OEMCONVERT)
 ,UKW(ES_READONLY)
 ,UKW(ES_WANTRETURN)
 ,UKW(ES_NUMBER)
 ,UKW(BS_PUSHBUTTON)
 ,UKW(BS_DEFPUSHBUTTON)
 ,UKW(BS_CHECKBOX)
 ,UKW(BS_AUTOCHECKBOX)
 ,UKW(BS_RADIOBUTTON)
 ,UKW(BS_3STATE)
 ,UKW(BS_AUTO3STATE)
 ,UKW(BS_GROUPBOX)
 ,UKW(BS_USERBUTTON)
 ,UKW(BS_AUTORADIOBUTTON)
 ,UKW(BS_OWNERDRAW)
 ,UKW(BS_LEFTTEXT)
 ,UKW(BS_TEXT)
 ,UKW(BS_ICON)
 ,UKW(BS_BITMAP)
 ,UKW(BS_LEFT)
 ,UKW(BS_RIGHT)
 ,UKW(BS_CENTER)
 ,UKW(BS_TOP)
 ,UKW(BS_BOTTOM)
 ,UKW(BS_VCENTER)
 ,UKW(BS_PUSHLIKE)
 ,UKW(BS_MULTILINE)
 ,UKW(BS_NOTIFY)
 ,UKW(BS_FLAT)
 ,UKW(BS_RIGHTBUTTON)
 ,UKW(SBS_HORZ)
 ,UKW(SBS_VERT)
 ,UKW(SBS_TOPALIGN)
 ,UKW(SBS_LEFTALIGN)
 ,UKW(SBS_BOTTOMALIGN)
 ,UKW(SBS_RIGHTALIGN)
 ,UKW(SBS_SIZEBOXTOPLEFTALIGN)
 ,UKW(SBS_SIZEBOXBOTTOMRIGHTALIGN)
 ,UKW(SBS_SIZEBOX)
 ,UKW(SBS_SIZEGRIP)
 ,UKW(CBS_SIMPLE)
 ,UKW(CBS_DROPDOWN)
 ,UKW(CBS_DROPDOWNLIST)
 ,UKW(CBS_OWNERDRAWFIXED)
 ,UKW(CBS_OWNERDRAWVARIABLE)
 ,UKW(CBS_AUTOHSCROLL)
 ,UKW(CBS_OEMCONVERT)
 ,UKW(CBS_SORT)
 ,UKW(CBS_HASSTRINGS)
 ,UKW(CBS_NOINTEGRALHEIGHT)
 ,UKW(CBS_DISABLENOSCROLL)
 ,UKW(CBS_UPPERCASE)
 ,UKW(CBS_LOWERCASE)
 ,UKW(LBS_NOTIFY)
 ,UKW(LBS_SORT)
 ,UKW(LBS_NOREDRAW)
 ,UKW(LBS_MULTIPLESEL)
 ,UKW(LBS_OWNERDRAWFIXED)
 ,UKW(LBS_OWNERDRAWVARIABLE)
 ,UKW(LBS_HASSTRINGS)
 ,UKW(LBS_USETABSTOPS)
 ,UKW(LBS_NOINTEGRALHEIGHT)
 ,UKW(LBS_MULTICOLUMN)
 ,UKW(LBS_WANTKEYBOARDINPUT)
 ,UKW(LBS_EXTENDEDSEL)
 ,UKW(LBS_DISABLENOSCROLL)
 ,UKW(LBS_NODATA)
 ,UKW(LBS_NOSEL)
 ,UKW(TBS_AUTOTICKS)
 ,UKW(TBS_VERT)
 ,UKW(TBS_HORZ)
 ,UKW(TBS_TOP)
 ,UKW(TBS_BOTTOM)
 ,UKW(TBS_LEFT)
 ,UKW(TBS_RIGHT)
 ,UKW(TBS_BOTH)
 ,UKW(TBS_NOTICKS)
 ,UKW(TBS_ENABLESELRANGE)
 ,UKW(TBS_FIXEDLENGTH)
 ,UKW(TBS_NOTHUMB)
 ,UKW(TBS_TOOLTIPS)
 ,UKW(TVS_HASBUTTONS)
 ,UKW(TVS_HASLINES)
 ,UKW(TVS_LINESATROOT)
 ,UKW(TVS_EDITLABELS)
 ,UKW(TVS_DISABLEDRAGDROP)
 ,UKW(TVS_SHOWSELALWAYS)
 ,UKW(TVS_RTLREADING)
 ,UKW(TVS_NOTOOLTIPS)
 ,UKW(TVS_CHECKBOXES)
 ,UKW(TVS_TRACKSELECT)
 ,UKW(TVS_SINGLEEXPAND)
 ,UKW(TVS_INFOTIP)
 ,UKW(TVS_FULLROWSELECT)
 ,UKW(TVS_NOSCROLL)
 ,UKW(TVS_NONEVENHEIGHT)
 ,UKW(LVS_ICON)
 ,UKW(LVS_REPORT)
 ,UKW(LVS_SMALLICON)
 ,UKW(LVS_LIST)
 ,UKW(LVS_TYPEMASK)
 ,UKW(LVS_SINGLESEL)
 ,UKW(LVS_SHOWSELALWAYS)
 ,UKW(LVS_SORTASCENDING)
 ,UKW(LVS_SORTDESCENDING)
 ,UKW(LVS_SHAREIMAGELISTS)
 ,UKW(LVS_NOLABELWRAP)
 ,UKW(LVS_AUTOARRANGE)
 ,UKW(LVS_EDITLABELS)
 ,UKW(LVS_OWNERDATA)
 ,UKW(LVS_NOSCROLL)
 ,UKW(LVS_TYPESTYLEMASK)
 ,UKW(LVS_ALIGNTOP)
 ,UKW(LVS_ALIGNLEFT)
 ,UKW(LVS_ALIGNMASK)
 ,UKW(LVS_OWNERDRAWFIXED)
 ,UKW(LVS_NOCOLUMNHEADER)
 ,UKW(LVS_NOSORTHEADER)
 ,UKW(UDS_WRAP)
 ,UKW(UDS_SETBUDDYINT)
 ,UKW(UDS_ALIGNRIGHT)
 ,UKW(UDS_ALIGNLEFT)
 ,UKW(UDS_AUTOBUDDY)
 ,UKW(UDS_ARROWKEYS)
 ,UKW(UDS_HORZ)
 ,UKW(UDS_NOTHOUSANDS)
 ,UKW(UDS_HOTTRACK)
 ,UKW(PBS_SMOOTH)
 ,UKW(PBS_VERTICAL)
 ,UKW(MCS_DAYSTATE)
 ,UKW(MCS_MULTISELECT)
 ,UKW(MCS_WEEKNUMBERS)
 ,UKW(MCS_NOTODAYCIRCLE)
 ,UKW(MCS_NOTODAY)
 ,UKW(DS_ABSALIGN)
 ,UKW(DS_SYSMODAL)
 ,UKW(DS_LOCALEDIT)
 ,UKW(DS_SETFONT)
 ,UKW(DS_MODALFRAME)
 ,UKW(DS_NOIDLEMSG)
 ,UKW(DS_SETFOREGROUND)
 ,UKW(DS_3DLOOK)
 ,UKW(DS_FIXEDSYS)
 ,UKW(DS_NOFAILCREATE)
 ,UKW(DS_CONTROL)
 ,UKW(DS_CENTER)
 ,UKW(DS_CENTERMOUSE)
 ,UKW(DS_CONTEXTHELP)
 ,UKW(MFT_STRING)
 ,UKW(MFT_BITMAP)
 ,UKW(MFT_MENUBARBREAK)
 ,UKW(MFT_MENUBREAK)
 ,UKW(MFT_OWNERDRAW)
 ,UKW(MFT_RADIOCHECK)
 ,UKW(MFT_SEPARATOR)
 ,UKW(MFT_RIGHTORDER)
 ,UKW(MFT_RIGHTJUSTIFY)
 ,UKW(MFS_GRAYED)
 ,UKW(MFS_DISABLED)
 ,UKW(MFS_CHECKED)
 ,UKW(MFS_HILITE)
 ,UKW(MFS_ENABLED)
 ,UKW(MFS_UNCHECKED)
 ,UKW(MFS_UNHILITE)
 ,UKW(MFS_DEFAULT)
 ,UKW(MFS_MASK)
 ,UKW(MFS_HOTTRACKDRAWN)
 ,UKW(MFS_CACHEDBMP)
 ,UKW(MFS_BOTTOMGAPDROP)
 ,UKW(MFS_TOPGAPDROP)
 ,UKW(MFS_GAPDROP)
 ,KW(BEGIN)
 ,KW(END)
 ,KW(OR)
 ,KW(NOT)
};

typedef StrHashMap<ResourceInputSymbol> HashMapType;

class MapInfo {
public:
  int             m_capacity;
  int             m_size;
  int             m_maxChainLength;
  CompactIntArray m_chainCount;

  MapInfo(size_t capacity, size_t size, size_t maxChainLength, const CompactIntArray &chainCount) 
  : m_capacity((int)capacity)
  , m_size((int)size)
  , m_maxChainLength((int)maxChainLength)
  , m_chainCount(chainCount) {
  }
  String toString() const;
};

String MapInfo::toString() const {
  String result = format(_T("Capacity:%4d, Size:%4d, MaxChainLength:%2d, chaincount:"), m_capacity, m_size, m_maxChainLength);
  for(size_t i = 0; i < m_chainCount.size(); i++) {
    result += format(_T(" (%d,%3d)"), i, m_chainCount[i]);
  }
  return result;
}

class ResourceKeyWordMap : public HashMapType {
public:
  ResourceKeyWordMap(size_t capacity, const KeyWord *initTable, size_t size) : HashMapType(capacity) {
    for(size_t i = 0; i < size; i++) {
      put(initTable[i].m_name, initTable[i].m_token);
    }
  }
  MapInfo getMapInfo() const {
    return MapInfo(getCapacity(), size(), getMaxChainLength(), getLength());
  }

};

#define DEFINE_KEYWORDTABLE(name, capacity, initTable) \
  static ResourceKeyWordMap name(capacity, initTable, ARRAYSIZE(initTable));

DEFINE_KEYWORDTABLE(keywords    , 2156, keywordTable    )

static ResourceInputSymbol nameOrKeyWord(const TCHAR *lexeme) {
  ResourceInputSymbol *p = keywords.get(lexeme);
  return p ? *p : IDENTIFIER;
}

void ResourceLex::findBestHashMapSize() {
  IntTreeMap<MapInfo> cl;
  int bestCapacity;
  int currentMax = -1;
  for(int capacity = 500; capacity < 3000; capacity++) {
    ResourceKeyWordMap ht(capacity, keywordTable, ARRAYSIZE(keywordTable));
    MapInfo v = ht.getMapInfo();
    cl.put((int)ht.getCapacity(), v);
    if((currentMax < 0) || v.m_maxChainLength < currentMax) {
      bestCapacity = capacity;
      currentMax = v.m_maxChainLength;
    }
    if(currentMax == 1) {
      break;
    }
  }

  if(currentMax > 1) {
    for(Iterator<Entry<int, MapInfo> > it = cl.entrySet().getIterator(); it.hasNext();) {
      Entry<int, MapInfo > &e = it.next();
      const int capacity = e.getKey();
      const MapInfo &v = e.getValue();
      if(v.m_maxChainLength == currentMax && v.m_chainCount[2] <= 12) {
        _tprintf(_T("%s\n"), v.toString().cstr());
      }
    }
  }
  const MapInfo *bestInfo = cl.get(bestCapacity);
  _tprintf(_T("bestInfo:%s\n"), bestInfo->toString().cstr());
  MapInfo info = keywords.getMapInfo();
  _tprintf(_T("current map:%s\n"), info.toString().cstr());
}

void ResourceLex::verror(const SourcePosition &pos, const TCHAR *format, va_list argptr) {
  if(m_parser) {
    m_parser->verror(pos,format,argptr);
  } else {
    Scanner::verror(pos,format,argptr);
  }
}

