%{

#include "ResourceSymbol.h"
#include "ResourceLex.h"
#include "ParserTree.h"

class ResourceParser : public LRparser {
private:
  static const ParserTables *ResourceTables;
public:
  ResourceParser(ParserTree &tree, ResourceLex *lex = NULL) : m_tree(tree), LRparser(*ResourceTables,lex) {}
  void  verror(const SourcePosition &pos, const TCHAR *format,va_list argptr);
  static int    ttoi(const TCHAR *s);
  static String stripQuotes(const String &s);
  static const ParserTables &getTables() {
    return *ResourceTables;
  }
private:
  ParserTree &m_tree;
  SyntaxNode *m_leftSide,**m_stacktop,*m_userstack[256];
  SyntaxNode *getStackTop(int fromtop)           { return m_stacktop[-fromtop];            }
  void userStackInit()                           { m_stacktop = m_userstack;               }
  void userStackShiftSymbol(unsigned int symbol) { m_stacktop++;                           } // push 1 element (garbage) on userstack
  void userStackPopSymbols( unsigned int count ) { m_stacktop      -= count;               } // pop count symbols from userstack
  void userStackShiftLeftSide()                  { *(++m_stacktop) = m_leftSide;           } // push($$) on userstack
  void defaultReduce(       unsigned int prod)   { m_leftSide      = *m_stacktop;          } // $$ = $1
  int  reduceAction(        unsigned int prod);
  SyntaxNode       *newNode(const SourcePosition &pos, int symbol, ... );
};

%}

%term NUMBER IDENTIFIER STRING
%left COMMA
%term GUIDELINES DESIGNINFO LEFTMARGIN RIGHTMARGIN TOPMARGIN BOTTOMMARGIN HORZGUIDE VERTGUIDE
%term TOOLBAR BUTTON
%term _BITMAP _ICON _FONT _MENU _MENUITEM STRINGTABLE ACCELERATORS VIRTKEY ASCII SHIFT ALT NOINVERT 
%term RCDATA DLGINIT TEXTINCLUDE LANGUAGE TYPELIB
%term MENUEX POPUP GRAYED CHECKED INACTIVE SEPARATOR 
%term VERSIONINFO FILEVERSION PRODUCTVERSION FILEFLAGSMASK FILEFLAGS FILEOS FILETYPE FILESUBTYPE BLOCK VALUE
%term DIALOG DIALOGEX 
%term AFX_DIALOG_LAYOUT

%term STYLE EXSTYLE CAPTION DISCARDABLE MOVEABLE _PURE IMPURE PRELOAD LOADONCALL 
%term PUSHBUTTON DEFPUSHBUTTON EDITTEXT SCROLLBAR COMBOBOX LISTBOX
%term GROUPBOX LTEXT RTEXT CTEXT CONTROL 

%term _WS_OVERLAPPED _WS_POPUP _WS_CHILD _WS_MINIMIZE _WS_VISIBLE _WS_DISABLED _WS_CLIPSIBLINGS _WS_CLIPCHILDREN 
%term _WS_MAXIMIZE _WS_CAPTION _WS_BORDER _WS_DLGFRAME _WS_VSCROLL _WS_HSCROLL _WS_SYSMENU _WS_THICKFRAME 
%term _WS_GROUP _WS_TABSTOP _WS_MINIMIZEBOX _WS_MAXIMIZEBOX 

%term _WS_EX_DLGMODALFRAME _WS_EX_NOPARENTNOTIFY _WS_EX_TOPMOST _WS_EX_ACCEPTFILES _WS_EX_TRANSPARENT 
%term _WS_EX_MDICHILD _WS_EX_TOOLWINDOW _WS_EX_WINDOWEDGE _WS_EX_CLIENTEDGE _WS_EX_CONTEXTHELP 
%term _WS_EX_RIGHT _WS_EX_LEFT _WS_EX_RTLREADING _WS_EX_LTRREADING _WS_EX_LEFTSCROLLBAR 
%term _WS_EX_RIGHTSCROLLBAR _WS_EX_CONTROLPARENT _WS_EX_STATICEDGE _WS_EX_APPWINDOW 

%term _SS_LEFT _SS_CENTER _SS_RIGHT _SS_ICON _SS_BLACKRECT _SS_GRAYRECT _SS_WHITERECT 
%term _SS_BLACKFRAME _SS_GRAYFRAME _SS_WHITEFRAME _SS_USERITEM _SS_SIMPLE _SS_LEFTNOWORDWRAP 
%term _SS_OWNERDRAW _SS_BITMAP _SS_ENHMETAFILE _SS_ETCHEDHORZ _SS_ETCHEDVERT _SS_ETCHEDFRAME 
%term _SS_TYPEMASK _SS_NOPREFIX _SS_NOTIFY _SS_CENTERIMAGE _SS_RIGHTJUST _SS_REALSIZEIMAGE 
%term _SS_SUNKEN _SS_ENDELLIPSIS _SS_PATHELLIPSIS _SS_WORDELLIPSIS _SS_ELLIPSISMASK 

%term _ES_LEFT _ES_CENTER _ES_RIGHT _ES_MULTILINE _ES_UPPERCASE _ES_LOWERCASE 
%term _ES_PASSWORD _ES_AUTOVSCROLL _ES_AUTOHSCROLL _ES_NOHIDESEL _ES_OEMCONVERT _ES_READONLY 
%term _ES_WANTRETURN _ES_NUMBER 

%term _BS_PUSHBUTTON _BS_DEFPUSHBUTTON _BS_CHECKBOX _BS_AUTOCHECKBOX _BS_RADIOBUTTON _BS_3STATE 
%term _BS_AUTO3STATE _BS_GROUPBOX _BS_USERBUTTON _BS_AUTORADIOBUTTON _BS_OWNERDRAW _BS_LEFTTEXT 
%term _BS_TEXT _BS_ICON _BS_BITMAP _BS_LEFT _BS_RIGHT _BS_CENTER _BS_TOP _BS_BOTTOM _BS_VCENTER 
%term _BS_PUSHLIKE _BS_MULTILINE _BS_NOTIFY _BS_FLAT _BS_RIGHTBUTTON

%term _SBS_HORZ _SBS_VERT _SBS_TOPALIGN _SBS_LEFTALIGN _SBS_BOTTOMALIGN _SBS_RIGHTALIGN
%term _SBS_SIZEBOXTOPLEFTALIGN _SBS_SIZEBOXBOTTOMRIGHTALIGN _SBS_SIZEBOX _SBS_SIZEGRIP
                            
%term _CBS_SIMPLE _CBS_DROPDOWN _CBS_DROPDOWNLIST _CBS_OWNERDRAWFIXED _CBS_OWNERDRAWVARIABLE 
%term _CBS_AUTOHSCROLL _CBS_OEMCONVERT _CBS_SORT _CBS_HASSTRINGS _CBS_NOINTEGRALHEIGHT 
%term _CBS_DISABLENOSCROLL _CBS_UPPERCASE _CBS_LOWERCASE 

%term _LBS_NOTIFY _LBS_SORT _LBS_NOREDRAW _LBS_MULTIPLESEL _LBS_OWNERDRAWFIXED _LBS_OWNERDRAWVARIABLE 
%term _LBS_HASSTRINGS _LBS_USETABSTOPS _LBS_NOINTEGRALHEIGHT _LBS_MULTICOLUMN _LBS_WANTKEYBOARDINPUT 
%term _LBS_EXTENDEDSEL _LBS_DISABLENOSCROLL _LBS_NODATA _LBS_NOSEL 

%term _TBS_AUTOTICKS _TBS_VERT _TBS_HORZ _TBS_TOP _TBS_BOTTOM _TBS_LEFT _TBS_RIGHT _TBS_BOTH 
%term _TBS_NOTICKS _TBS_ENABLESELRANGE _TBS_FIXEDLENGTH _TBS_NOTHUMB _TBS_TOOLTIPS 

%term _TVS_HASBUTTONS _TVS_HASLINES _TVS_LINESATROOT _TVS_EDITLABELS _TVS_DISABLEDRAGDROP
%term _TVS_SHOWSELALWAYS _TVS_RTLREADING _TVS_NOTOOLTIPS _TVS_CHECKBOXES _TVS_TRACKSELECT
%term _TVS_SINGLEEXPAND _TVS_INFOTIP _TVS_FULLROWSELECT _TVS_NOSCROLL _TVS_NONEVENHEIGHT

%term _LVS_ICON _LVS_REPORT _LVS_SMALLICON _LVS_LIST _LVS_TYPEMASK _LVS_SINGLESEL _LVS_SHOWSELALWAYS 
%term _LVS_SORTASCENDING _LVS_SORTDESCENDING _LVS_SHAREIMAGELISTS _LVS_NOLABELWRAP _LVS_AUTOARRANGE 
%term _LVS_EDITLABELS _LVS_OWNERDATA _LVS_NOSCROLL _LVS_TYPESTYLEMASK _LVS_ALIGNTOP _LVS_ALIGNLEFT 
%term _LVS_ALIGNMASK _LVS_OWNERDRAWFIXED _LVS_NOCOLUMNHEADER _LVS_NOSORTHEADER 

%term _UDS_WRAP _UDS_SETBUDDYINT _UDS_ALIGNRIGHT _UDS_ALIGNLEFT _UDS_AUTOBUDDY
%term _UDS_ARROWKEYS _UDS_HORZ _UDS_NOTHOUSANDS _UDS_HOTTRACK

%term _PBS_SMOOTH _PBS_VERTICAL

%term _MCS_DAYSTATE _MCS_MULTISELECT _MCS_WEEKNUMBERS _MCS_NOTODAYCIRCLE _MCS_NOTODAY

%term _DS_ABSALIGN _DS_SYSMODAL _DS_LOCALEDIT _DS_SETFONT _DS_MODALFRAME _DS_NOIDLEMSG _DS_SETFOREGROUND 
%term _DS_3DLOOK _DS_FIXEDSYS _DS_NOFAILCREATE _DS_CONTROL _DS_CENTER _DS_CENTERMOUSE _DS_CONTEXTHELP 

%term _MFT_STRING _MFT_BITMAP _MFT_MENUBARBREAK _MFT_MENUBREAK _MFT_OWNERDRAW _MFT_RADIOCHECK 
%term _MFT_SEPARATOR _MFT_RIGHTORDER _MFT_RIGHTJUSTIFY
%term _MFS_GRAYED _MFS_DISABLED _MFS_CHECKED _MFS_HILITE _MFS_ENABLED _MFS_UNCHECKED _MFS_UNHILITE
%term _MFS_DEFAULT _MFS_MASK _MFS_HOTTRACKDRAWN _MFS_CACHEDBMP _MFS_BOTTOMGAPDROP _MFS_TOPGAPDROP _MFS_GAPDROP 


%term BEGIN END
%left OR
%term NOT
%left DOT
%term LPAR RPAR

%{

#include "stdafx.h"
#include "ResourceParser.h"

#pragma warning(disable:4312)

%}
%%

start                       : resourceFile                                              { m_tree.setRoot($1); }
                            ;

resourceFile                : resourceDefinitionList
                            ;

resourceDefinitionList      : resourceDefinitionList resourceDefinition                 { $$ = ($1&&$2)
                                                                                               ? newNode(getPos(2), COMMA, $1, $2, NULL)
                                                                                               : $1 ? $1 : $2;
                                                                                        }
                            | resourceDefinition
                            ;

resourceDefinition          : bitmapDefinition                                          { $$ = NULL; }
                            | iconDefinition                                            { $$ = NULL; }
                            | customTypeDefinition                                      { $$ = NULL; }
                            | dialogDefinition
                            | extendedDialogDefinition
                            | guideLinesDefinition                                      { $$ = NULL; }
                            | toolbarDefnition                                          { $$ = NULL; }
                            | menuDefinition
                            | dialogInitDefinition                                      { $$ = NULL; }
                            | acceleratorsDefinition
                            | rcdataDefinition                                          { $$ = NULL; }
                            | stringTableDefinition
                            | textIncludeDirective                                      { $$ = NULL; }
                            | languageDirective
                            | versionDefinition                                         { $$ = NULL; }
                            | typelibDefinition                                         { $$ = NULL; }
							| afxDialogLayout                                           { $$ = NULL; }
                            ;


bitmapDefinition            : resourceId _BITMAP resourceAttributeList string
                            ;

iconDefinition              : resourceId _ICON resourceAttributeList string
                            ;

customTypeDefinition        : resourceId identifier resourceAttributeList string
                            ;

dialogDefinition            : resourceId DIALOG resourceAttributeList rectangleSpec
                              dialogSpecList
                              BEGIN opt_dialogControlSpecList END                       { $$ = newNode(getPos(1), dialogDefinition, $1, $7, $5, NULL); }
                            ;

extendedDialogDefinition    : resourceId DIALOGEX rectangleSpec
                              extendedDialogSpecList
                              BEGIN opt_dialogControlSpecList END                       { $$ = newNode(getPos(1), dialogDefinition, $1, $6, $4, NULL);  }
                            ;

dialogSpecList              : dialogSpecList dialogSpec                                 { $$ = ($1&&$2)
                                                                                               ? newNode(getPos(2), COMMA, $1, $2, NULL)
                                                                                               : $1 ? $1 : $2;
                                                                                        }
                            | dialogSpec
                            ;

dialogSpec                  : STYLE dialogStyleExpr                                     { $$ = NULL; }
                            | CAPTION string                                            { $$ = NULL; }
                            | _MENU identifier                                          { $$ = newNode(getPos(1), _MENU, $2, NULL);            }
                            | _FONT fontSpec                                            { $$ = NULL; }
                            ;


extendedDialogSpecList      : extendedDialogSpecList extendedDialogSpec                 { $$ = ($1&&$2)
                                                                                               ? newNode(getPos(2), COMMA, $1, $2, NULL)
                                                                                               : $1 ? $1 : $2;
                                                                                        }
                            | extendedDialogSpec
                            ;

extendedDialogSpec          : STYLE dialogStyleExpr                                     { $$ = NULL; }
                            | EXSTYLE extendedDialogStyleExpr                           { $$ = NULL; }
                            | CAPTION string                                            { $$ = NULL; }
                            | _MENU identifier                                          { $$ = newNode(getPos(1), _MENU, $2, NULL);            }
                            | _FONT extendedFontSpec                                    { $$ = NULL; }
                            ;

extendedDialogStyleExpr     : extendedDialogStyleExpr OR extendedDialogStyle
                            | extendedDialogStyle
                            ;

extendedDialogStyle         : ws_ex_style
                            ;

fontSpec                    : number COMMA string
                            ;

extendedFontSpec            : number COMMA string COMMA number COMMA number COMMA number
                            | fontSpec
                            ;

dialogStyleExpr             : dialogStyleExpr OR dialogStyle
                            | dialogStyle
                            ;

dialogStyle                 : ds_style
                            | ws_style
                            ;

opt_dialogControlSpecList   : /* empty */                                               { $$ = newNode(getPos(0), dialogControlSpecList, NULL    ); }
                            | dialogControlSpecList                                     { $$ = newNode(getPos(1), dialogControlSpecList, $1, NULL); }
                            ;

dialogControlSpecList       : dialogControlSpec
                            | dialogControlSpecList dialogControlSpec                   { $$ = ($1&&$2)
                                                                                             ? newNode(getPos(2), COMMA, $1, $2, NULL)
                                                                                             : $1 ? $1 : $2;
                                                                                        }
                            ;


dialogControlSpec           : pushButtonSpec
                            | lrcTextSpec
                            | editSpec                                                  { $$ = NULL; }
                            | comboBoxSpec                                              { $$ = NULL; }
                            | listBoxSpec                                               { $$ = NULL; }
                            | iconSpec                                                  { $$ = NULL; }
                            | scrollBarSpec                                             { $$ = NULL; }
                            | controlSpec
                            ; 

pushButtonSpec              : buttonType string  COMMA identifier controlAttributeList  { $$ = newNode(getPos(1), $1->getSymbol(), $4, $2, NULL); }
                            ;

buttonType                  : PUSHBUTTON                                                { $$ = newNode(getPos(1), PUSHBUTTON   , NULL); }
                            | DEFPUSHBUTTON                                             { $$ = newNode(getPos(1), DEFPUSHBUTTON, NULL); }
                            | GROUPBOX                                                  { $$ = newNode(getPos(1), GROUPBOX     , NULL); }
                            ;

lrcTextSpec                 : lrcText string COMMA identifier controlAttributeList      { $$ = newNode(getPos(1), LTEXT, $4, $2, NULL); }
                            ;

lrcText                     : LTEXT 
                            | RTEXT 
                            | CTEXT
                            ;

editSpec                    : EDITTEXT identifier controlAttributeList
                            ;

comboBoxSpec                : COMBOBOX identifier controlAttributeList
                            ;

listBoxSpec                 : LISTBOX identifier controlAttributeList
                            ;

iconSpec                    : _ICON identifierOrString COMMA identifier controlAttributeList
                            ;

scrollBarSpec               : SCROLLBAR identifier controlAttributeList
                            ;

controlSpec                 : CONTROL controlId COMMA identifier COMMA string controlAttributeList { $$ = newNode(getPos(1), CONTROL, $4, $2, $6, NULL); }
                            ;

controlId                   : string
                            | identifier
                            | number
                            ;

controlAttributeList        : controlAttributeList controlAttribute
                            | controlAttribute
                            ;


controlAttribute            : COMMA ctrlAttribute
                            ;

ctrlAttribute               : number
                            | identifier
                            | controlStyleExpr
                            | extendedWindowStyleExpr
                            ;

controlStyleExpr            : controlStyleExpr OR ctrl_style
                            | ctrl_style
                            ;

extendedWindowStyleExpr     : extendedWindowStyleExpr OR ws_ex_style
                            | ws_ex_style
                            ;

ctrl_style					: add_ctrl_style
							| NOT add_ctrl_style
							;

add_ctrl_style              : ws_style
                            | ss_style
                            | es_style
                            | bs_style
                            | cbs_style
                            | sbs_style
                            | lbs_style
                            | tbs_style
                            | tvs_style
                            | lvs_style
                            | uds_style
                            | pbs_style
                            | mcs_style
                            ;

resourceAttributeList       : /* empty */
                            | resourceAttributeList resourceAttribute
                            ;

resourceAttribute           : DISCARDABLE 
                            | MOVEABLE
                            | _PURE
                            | IMPURE
                            | PRELOAD
                            | LOADONCALL
                            ;

ds_style                    : _DS_ABSALIGN 
                            | _DS_SYSMODAL 
                            | _DS_LOCALEDIT 
                            | _DS_SETFONT 
                            | _DS_MODALFRAME 
                            | _DS_NOIDLEMSG 
                            | _DS_SETFOREGROUND 
                            | _DS_3DLOOK 
                            | _DS_FIXEDSYS 
                            | _DS_NOFAILCREATE 
                            | _DS_CONTROL 
                            | _DS_CENTER 
                            | _DS_CENTERMOUSE 
                            | _DS_CONTEXTHELP
                            ; 

ws_style                    : _WS_OVERLAPPED 
                            | _WS_POPUP
                            | _WS_CHILD
                            | _WS_MINIMIZE
                            | _WS_VISIBLE
                            | _WS_DISABLED
                            | _WS_CLIPSIBLINGS
                            | _WS_CLIPCHILDREN 
                            | _WS_MAXIMIZE
                            | _WS_CAPTION
                            | _WS_BORDER
                            | _WS_DLGFRAME
                            | _WS_VSCROLL
                            | _WS_HSCROLL
                            | _WS_SYSMENU
                            | _WS_THICKFRAME 
                            | _WS_GROUP
                            | _WS_TABSTOP
                            | _WS_MINIMIZEBOX
                            | _WS_MAXIMIZEBOX
                            ;

ws_ex_style                 : _WS_EX_DLGMODALFRAME
                            | _WS_EX_NOPARENTNOTIFY
                            | _WS_EX_TOPMOST
                            | _WS_EX_ACCEPTFILES
                            | _WS_EX_TRANSPARENT
                            | _WS_EX_MDICHILD
                            | _WS_EX_TOOLWINDOW
                            | _WS_EX_WINDOWEDGE
                            | _WS_EX_CLIENTEDGE
                            | _WS_EX_CONTEXTHELP
                            | _WS_EX_RIGHT
                            | _WS_EX_LEFT
                            | _WS_EX_RTLREADING
                            | _WS_EX_LTRREADING
                            | _WS_EX_LEFTSCROLLBAR
                            | _WS_EX_RIGHTSCROLLBAR
                            | _WS_EX_CONTROLPARENT
                            | _WS_EX_STATICEDGE
                            | _WS_EX_APPWINDOW
                            ;

ss_style                    : _SS_LEFT
                            | _SS_CENTER
                            | _SS_RIGHT
                            | _SS_ICON
                            | _SS_BLACKRECT
                            | _SS_GRAYRECT
                            | _SS_WHITERECT
                            | _SS_BLACKFRAME
                            | _SS_GRAYFRAME
                            | _SS_WHITEFRAME
                            | _SS_USERITEM
                            | _SS_SIMPLE
                            | _SS_LEFTNOWORDWRAP
                            | _SS_OWNERDRAW
                            | _SS_BITMAP
                            | _SS_ENHMETAFILE
                            | _SS_ETCHEDHORZ
                            | _SS_ETCHEDVERT
                            | _SS_ETCHEDFRAME
                            | _SS_TYPEMASK
                            | _SS_NOPREFIX
                            | _SS_NOTIFY
                            | _SS_CENTERIMAGE
                            | _SS_RIGHTJUST
                            | _SS_REALSIZEIMAGE
                            | _SS_SUNKEN
                            | _SS_ENDELLIPSIS
                            | _SS_PATHELLIPSIS
                            | _SS_WORDELLIPSIS
                            | _SS_ELLIPSISMASK
                            ;

es_style                    : _ES_LEFT 
                            | _ES_CENTER
                            | _ES_RIGHT
                            | _ES_MULTILINE
                            | _ES_UPPERCASE
                            | _ES_LOWERCASE
                            | _ES_PASSWORD
                            | _ES_AUTOVSCROLL
                            | _ES_AUTOHSCROLL
                            | _ES_NOHIDESEL
                            | _ES_OEMCONVERT
                            | _ES_READONLY 
                            | _ES_WANTRETURN
                            | _ES_NUMBER
                            ;

cbs_style                   : _CBS_SIMPLE
                            | _CBS_DROPDOWN
                            | _CBS_DROPDOWNLIST
                            | _CBS_OWNERDRAWFIXED
                            | _CBS_OWNERDRAWVARIABLE
                            | _CBS_AUTOHSCROLL
                            | _CBS_OEMCONVERT
                            | _CBS_SORT
                            | _CBS_HASSTRINGS
                            | _CBS_NOINTEGRALHEIGHT
                            | _CBS_DISABLENOSCROLL
                            | _CBS_UPPERCASE
                            | _CBS_LOWERCASE 
                            ;

lbs_style                   : _LBS_NOTIFY
                            | _LBS_SORT
                            | _LBS_NOREDRAW
                            | _LBS_MULTIPLESEL
                            | _LBS_OWNERDRAWFIXED
                            | _LBS_OWNERDRAWVARIABLE
                            | _LBS_HASSTRINGS
                            | _LBS_USETABSTOPS
                            | _LBS_NOINTEGRALHEIGHT
                            | _LBS_MULTICOLUMN
                            | _LBS_WANTKEYBOARDINPUT
                            | _LBS_EXTENDEDSEL
                            | _LBS_DISABLENOSCROLL
                            | _LBS_NODATA 
                            | _LBS_NOSEL
                            ;

bs_style                    : _BS_PUSHBUTTON
                            | _BS_DEFPUSHBUTTON
                            | _BS_CHECKBOX
                            | _BS_AUTOCHECKBOX
                            | _BS_RADIOBUTTON
                            | _BS_3STATE
                            | _BS_AUTO3STATE
                            | _BS_GROUPBOX
                            | _BS_USERBUTTON
                            | _BS_AUTORADIOBUTTON
                            | _BS_OWNERDRAW
                            | _BS_LEFTTEXT
                            | _BS_TEXT
                            | _BS_ICON
                            | _BS_BITMAP
                            | _BS_LEFT
                            | _BS_RIGHT
                            | _BS_CENTER
                            | _BS_TOP
                            | _BS_BOTTOM
                            | _BS_VCENTER
                            | _BS_PUSHLIKE
                            | _BS_MULTILINE
                            | _BS_NOTIFY
                            | _BS_FLAT
                            | _BS_RIGHTBUTTON
                            ;

sbs_style                   : _SBS_HORZ 
                            | _SBS_VERT 
                            | _SBS_TOPALIGN 
                            | _SBS_LEFTALIGN 
                            | _SBS_BOTTOMALIGN 
                            | _SBS_RIGHTALIGN 
                            | _SBS_SIZEBOXTOPLEFTALIGN 
                            | _SBS_SIZEBOXBOTTOMRIGHTALIGN 
                            | _SBS_SIZEBOX 
                            | _SBS_SIZEGRIP
                            ;

tbs_style                   : _TBS_AUTOTICKS
                            | _TBS_VERT
                            | _TBS_HORZ
                            | _TBS_TOP
                            | _TBS_BOTTOM
                            | _TBS_LEFT
                            | _TBS_RIGHT
                            | _TBS_BOTH
                            | _TBS_NOTICKS
                            | _TBS_ENABLESELRANGE
                            | _TBS_FIXEDLENGTH
                            | _TBS_NOTHUMB
                            | _TBS_TOOLTIPS 
                            ;

tvs_style                   : _TVS_HASBUTTONS 
                            | _TVS_HASLINES
                            | _TVS_LINESATROOT
                            | _TVS_EDITLABELS
                            | _TVS_DISABLEDRAGDROP
                            | _TVS_SHOWSELALWAYS
                            | _TVS_RTLREADING
                            | _TVS_NOTOOLTIPS
                            | _TVS_CHECKBOXES
                            | _TVS_TRACKSELECT
                            | _TVS_SINGLEEXPAND
                            | _TVS_INFOTIP
                            | _TVS_FULLROWSELECT
                            | _TVS_NOSCROLL
                            | _TVS_NONEVENHEIGHT
                            ;

lvs_style                   : _LVS_ICON 
                            | _LVS_REPORT
                            | _LVS_SMALLICON
                            | _LVS_LIST
                            | _LVS_TYPEMASK
                            | _LVS_SINGLESEL
                            | _LVS_SHOWSELALWAYS
                            | _LVS_SORTASCENDING
                            | _LVS_SORTDESCENDING
                            | _LVS_SHAREIMAGELISTS
                            | _LVS_NOLABELWRAP
                            | _LVS_AUTOARRANGE
                            | _LVS_EDITLABELS
                            | _LVS_OWNERDATA
                            | _LVS_NOSCROLL
                            | _LVS_TYPESTYLEMASK
                            | _LVS_ALIGNTOP
                            | _LVS_ALIGNLEFT
                            | _LVS_ALIGNMASK
                            | _LVS_OWNERDRAWFIXED
                            | _LVS_NOCOLUMNHEADER
                            | _LVS_NOSORTHEADER 
                            ; 

uds_style                   : _UDS_WRAP
                            | _UDS_SETBUDDYINT
                            | _UDS_ALIGNRIGHT
                            | _UDS_ALIGNLEFT
                            | _UDS_AUTOBUDDY
                            | _UDS_ARROWKEYS
                            | _UDS_HORZ
                            | _UDS_NOTHOUSANDS
                            | _UDS_HOTTRACK
                            ;

pbs_style                   : _PBS_SMOOTH
                            | _PBS_VERTICAL
                            ;

mcs_style                   : _MCS_DAYSTATE
                            | _MCS_MULTISELECT
                            | _MCS_WEEKNUMBERS
                            | _MCS_NOTODAYCIRCLE
                            | _MCS_NOTODAY 
                            ;

guideLinesDefinition        : GUIDELINES DESIGNINFO resourceAttributeList BEGIN designInfoList END
                            ;

designInfoList              : designInfoList designInfo
                            | designInfo
                            ;

designInfo                  : identifierOrString COMMA DIALOG BEGIN designSpecList END
                            ;

identifierOrString          : identifier
                            | string
                            ;

designSpecList              : designSpecList designSpec
                            | /* empty */
                            ;

designSpec                  : marginSpec COMMA number
                            ;

marginSpec                  : /* empty */
                            | LEFTMARGIN
                            | RIGHTMARGIN
                            | TOPMARGIN
                            | BOTTOMMARGIN
                            | HORZGUIDE
                            | VERTGUIDE
                            ;

toolbarDefnition            : resourceId TOOLBAR resourceAttributeList sizeSpec BEGIN buttonSpecList END
                            ;

buttonSpecList              : buttonSpecList buttonSpec
                            | buttonSpec
                            ;

buttonSpec                  : BUTTON identifier
                            | SEPARATOR
                            ;

menuDefinition              : resourceId menuType resourceAttributeList menuBody            { $$ = newNode(getPos(1), menuDefinition, $1, $4, NULL); }
                            ;

menuType                    : _MENU 
                            | MENUEX
                            ;

menuBody                    : BEGIN menuSpecList END                                        { $$ = $2; }
                            ;

menuSpecList                : menuSpecList menuSpec                                         { $$ = ($1&&$2)
                                                                                                 ? newNode(getPos(2), COMMA, $1, $2, NULL)
                                                                                                 : $1 ? $1 : $2;
                                                                                            }
                            | menuSpec
                            ;

menuSpec                    : popupMenu
                            | menuItem
                            ;

popupMenu                   : POPUP string menuItemModifierList menuBody                    { $$ = newNode(getPos(1), POPUP, $2, $4, NULL); }
                            ;

menuItem                    : _MENUITEM string COMMA identifierOrNumber menuItemModifierList { $$ = newNode(getPos(1), _MENUITEM, $2, $4, NULL); }
                            | _MENUITEM SEPARATOR                                            { $$ = NULL; }
                            ;

menuItemModifierList        : /* empty */
                            | menuItemModifierList COMMA menuItemModifier
                            ;

menuItemModifier            : number 
                            | GRAYED
                            | CHECKED
                            | INACTIVE
                            | menuItemFlagExpr
                            ;

menuItemFlagExpr            : menuItemFlagExpr OR menuItemFlag
                            | menuItemFlag
                            ;

menuItemFlag                : _MFT_STRING 
                            | _MFT_BITMAP
                            | _MFT_MENUBARBREAK
                            | _MFT_MENUBREAK
                            | _MFT_OWNERDRAW
                            | _MFT_RADIOCHECK 
                            | _MFT_SEPARATOR
                            | _MFT_RIGHTORDER
                            | _MFT_RIGHTJUSTIFY
                            | _MFS_GRAYED
                            | _MFS_DISABLED
                            | _MFS_CHECKED
                            | _MFS_HILITE
                            | _MFS_ENABLED
                            | _MFS_UNCHECKED
                            | _MFS_UNHILITE
                            | _MFS_DEFAULT
                            | _MFS_MASK
                            | _MFS_HOTTRACKDRAWN
                            | _MFS_CACHEDBMP
                            | _MFS_BOTTOMGAPDROP
                            | _MFS_TOPGAPDROP
                            | _MFS_GAPDROP 
                            ;

dialogInitDefinition        : resourceId DLGINIT BEGIN constantElementList END
                            ;

constantElementList         : constantElementList constantElement
                            | constantElement
                            ;

constantElement             : constant
                            | COMMA constant
                            ;

acceleratorsDefinition      : resourceId ACCELERATORS resourceAttributeList BEGIN acceleratorList END { $$ = newNode(getPos(1), acceleratorsDefinition, $1, $5, NULL ); }
                            ;

acceleratorList             : acceleratorList accelerator                       { $$ = newNode(getPos(1), COMMA, $1, $2, NULL); }
                            | accelerator
                            ;

accelerator                 : constant COMMA identifier opt_acceleratorSpecList { $$ = newNode(getPos(1), accelerator, $3, $1, $4, NULL); }
                            ;

opt_acceleratorSpecList     : /* empty */
                            | COMMA acceleratorSpecList                         { $$ = $2; }
                            ;

acceleratorSpecList         : acceleratorModifier
                            | acceleratorSpecList COMMA acceleratorModifier     { $$ = newNode(getPos(2), COMMA, $1, $3, NULL); }
                            ;

acceleratorModifier         : VIRTKEY                                           { $$ = newNode(getPos(1), VIRTKEY , NULL); }
                            | ASCII                                             { $$ = newNode(getPos(1), ASCII   , NULL); }
                            | SHIFT                                             { $$ = newNode(getPos(1), SHIFT   , NULL); }
                            | CONTROL                                           { $$ = newNode(getPos(1), CONTROL , NULL); }
                            | ALT                                               { $$ = newNode(getPos(1), ALT     , NULL); }
                            | NOINVERT                                          { $$ = newNode(getPos(1), NOINVERT, NULL); }
                            ;

rcdataDefinition            : resourceId RCDATA resourceAttributeList BEGIN numberList END
                            | resourceId RCDATA resourceAttributeList string
                            ;

numberList                  : numberList COMMA number
                            | number
                            ;

stringTableDefinition       : STRINGTABLE resourceAttributeList BEGIN stringSpecList END  { $$ = newNode(getPos(1), stringTableDefinition, $4, NULL); }
                            ;


stringSpecList              : stringSpecList stringSpec                                   { $$ = newNode(getPos(1), COMMA, $1, $2, NULL); }
                            | stringSpec
                            ;

stringSpec                  : identifier string                                           { $$ = newNode(getPos(1), stringSpec, $1, $2, NULL); }
                            ;

textIncludeDirective        : number TEXTINCLUDE resourceAttributeList BEGIN stringList END
                            ;

stringList                  : stringList string
                            | string
                            ;

languageDirective           : LANGUAGE constant COMMA constant                            { $$ = newNode(getPos(1), languageDirective, $2, $4, NULL); }
                            ;

versionDefinition           : resourceId VERSIONINFO resourceAttributeList versionSpecList BEGIN blockList END
                            ;

versionSpecList             : versionSpecList versionSpec
                            | versionSpec
                            ;

versionSpec                 : FILEVERSION     exprList
                            | PRODUCTVERSION  exprList
                            | FILEFLAGSMASK   expr
                            | FILEFLAGS       expr
                            | FILEOS          expr
                            | FILETYPE        expr
                            | FILESUBTYPE     expr
                            ;

blockList                   : blockList block
                            | block
                            ;

block                       : BLOCK string BEGIN blockBody END
                            ;

blockBody                   : blockList
                            | valueList
                            ;

valueList                   : valueList value
                            | value
                            ;

value                       : VALUE constantElementList
                            ;

typelibDefinition           : number TYPELIB fileName
                            ;

afxDialogLayout             : resourceId AFX_DIALOG_LAYOUT BEGIN layoutInfo END
							              ;

layoutInfo					        : numberList
							              ;

fileName                    : string
                            | name
                            ;

name                        : identifier                                        { $$ = newNode( getPos(1), STRING, $1->getName().cstr());                                 }
                            | name DOT identifier                               { $$ = newNode( getPos(1), STRING, String($1->getString() + _T(".") + $3->getName()).cstr()); }
                            ;

exprList                    : exprList COMMA expr
                            | expr
                            ;

expr                        : expr OR constant
                            | constant
                            ;

constant                    : number
                            | identifier
                            | string
                            | identifier LPAR exprList RPAR
                            ;

rectangleSpec               : number COMMA number COMMA number COMMA number
                            ;

sizeSpec                    : number COMMA number
                            ;

resourceId					        : identifierOrNumber
							              ;

identifierOrNumber          : identifier
                            | number
                            ;

number                      : NUMBER                                            { $$ = newNode( getPos(1), NUMBER, ttoi(getText()));       }
                            ;

identifier                  : IDENTIFIER                                        { $$ = newNode( getPos(1), IDENTIFIER, getText());         }
                            ;

string                      : STRING                                            { $$ = newNode( getPos(1), STRING, stripQuotes(getText()).cstr()); }
                            ;


%%

SyntaxNode *ResourceParser::newNode(const SourcePosition &pos, int symbol, ...) {
  va_list argptr;
  va_start(argptr,symbol);
  SyntaxNode *p = m_tree.vFetchNode(pos,symbol,argptr);
  va_end(argptr);
  return p;
}

void ResourceParser::verror(const SourcePosition &pos, const TCHAR *format, va_list argptr) {
  m_tree.vAddError(&pos,format,argptr);
}

int ResourceParser::ttoi(const TCHAR *s) { // static
  int result;
  if(_tcsncicmp(s,_T("0x"),2) == 0) {
    _stscanf(s+2,_T("%x"), &result);
  } else {
    _stscanf(s, _T("%d"), &result);
  }
  return result;
}

String ResourceParser::stripQuotes(const String &s) { // static
  const intptr_t len = s.length();
  if((len >= 2) && (s[0]=='"') && (s[len-1] == '"')) {
    return substr(s, 1, len-2);
  } else {
    return s;
  }
}
