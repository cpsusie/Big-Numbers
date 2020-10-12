%{

#include "PreprocessorSymbol.h"
#include "PreprocessorLex.h"
#include "ParserTree.h"

class PreprocessorParser : public LRparser {
public:
  PreprocessorParser(ParserTree &tree, PreprocessorLex *lex = NULL) : m_tree(tree), LRparser(*PreprocessorTables,lex) {
  }
  void  verror(const SourcePosition &pos, const char *format,va_list argptr);
private:
  ParserTree &m_tree;
  SyntaxNode *m_leftSide,**m_stacktop,*m_userstack[256];
  SyntaxNode *getStackTop(int fromtop)            { return m_stacktop[-fromtop];           }
  void userStackInit()                   override { m_stacktop = m_userstack;              }
  void userStackShiftSymbol(UINT symbol) override { m_stacktop++;                          } // push 1 element (garbage) on userstack
  void userStackPopSymbols( UINT count ) override { m_stacktop      -= count;              } // pop count symbols from userstack
  void userStackShiftLeftSide()          override { *(++m_stacktop) = m_leftSide;          } // push($$) on userstack
  void defaultReduce(       UINT prod  ) override { m_leftSide      = *m_stacktop;         } // $$ = $1
  int  reduceAction(        UINT prod  ) override;
  SyntaxNode       *newNode(const SourcePosition &pos, int symbol, ... );
};
%}

%term NUMBER NAME STRING
%left IF IFDEF DEFINE UNDEF
%term DIALOG DIALOGEX _BITMAP STRINGTABLE
%term GUIDELINES DESIGNINFO LEFTMARGIN RIGHTMARGIN TOPMARGIN BOTTOMMARGIN
%term TOOLBAR BUTTON
%term ACCELERATORS VIRTKEY ALT SHIFT NOINVERT
%term MENU POPUP MENUITEM GRAYED CHECKED SEPARATOR 

%term PUSHBUTTON DEFPUSHBUTTON EDITTEXT SCROLLBAR COMBOBOX LISTBOX
%term GROUPBOX ICON LTEXT RTEXT CONTROL 

%term FONT STYLE EXSTYLE CAPTION DISCARDABLE

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

%term _TVS_HASBUTTONS _TVS_HASLINES _TVS_LINESATROOT _TVS_SHOWSELALWAYS

%term _LVS_ICON _LVS_REPORT _LVS_SMALLICON _LVS_LIST _LVS_TYPEMASK _LVS_SINGLESEL _LVS_SHOWSELALWAYS 
%term _LVS_SORTASCENDING _LVS_SORTDESCENDING _LVS_SHAREIMAGELISTS _LVS_NOLABELWRAP _LVS_AUTOARRANGE 
%term _LVS_EDITLABELS _LVS_OWNERDATA _LVS_NOSCROLL _LVS_TYPESTYLEMASK _LVS_ALIGNTOP _LVS_ALIGNLEFT 
%term _LVS_ALIGNMASK _LVS_OWNERDRAWFIXED _LVS_NOCOLUMNHEADER _LVS_NOSORTHEADER 

%term _UDS_WRAP _UDS_SETBUDDYINT _UDS_ALIGNRIGHT _UDS_ALIGNLEFT _UDS_AUTOBUDDY
%term _UDS_ARROWKEYS _UDS_HORZ _UDS_NOTHOUSANDS _UDS_HOTTRACK

%term _DS_ABSALIGN _DS_SYSMODAL _DS_LOCALEDIT _DS_SETFONT _DS_MODALFRAME _DS_NOIDLEMSG _DS_SETFOREGROUND 
%term _DS_3DLOOK _DS_FIXEDSYS _DS_NOFAILCREATE _DS_CONTROL _DS_CENTER _DS_CENTERMOUSE _DS_CONTEXTHELP 

%term BEGIN END
%left OR
%term NOT

%{

#include "stdafx.h"
#include "PreprocessorParser.h"

%}
%%

start                       : resourceFile
                            ;

resourceFile                : resourceDefinitionList
                            ;

resourceDefinitionList      : resourceDefinitionList resourceDefinition
                            | resourceDefinition
                        ;

resourceDefinition          : bitmapDefinition
                            | iconDefinition
                            | customTypeDefinition
                            | dialogDefinition
                            | extendedDialogDefinition
                            | guideLinesDefinition
                            | toolbarDefnition
                            | menuDefinition
                            | acceleratorsDefinition
                            | stringTableDefnition
                            ;

bitmapDefinition            : name _BITMAP DISCARDABLE string
                            ;

iconDefinition              : name ICON DISCARDABLE string
                            ;

customTypeDefinition        : name name DISCARDABLE string
                            ;

dialogDefinition            : name DIALOG DISCARDABLE rectangleSpec
                              dialogSpecList
                              BEGIN dialogControlSpecList END
                            ;

dialogSpecList              : dialogSpecList dialogSpec
                            | dialogSpec
                            ;

dialogSpec                  : STYLE dialogStyleExpr
                            | CAPTION string
                            | MENU name
                            | FONT fontSpec
                            ;

extendedDialogDefinition    : name DIALOGEX rectangleSpec
                              extendedDialogSpecList
                              BEGIN dialogControlSpecList END
                            ;

extendedDialogSpecList      : extendedDialogSpecList extendedDialogSpec
                            | extendedDialogSpec
                            ;

extendedDialogSpec          : STYLE dialogStyleExpr
                            | EXSTYLE extendedDialogStyleExpr
                            | CAPTION string
                            | MENU name
                            | FONT extendedFontSpec
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

dialogControlSpecList       : dialogControlSpecList dialogControlSpec
                            | dialogControlSpec
                            ;


dialogControlSpec           : pushButtonSpec
                            | lrTextSpec
                            | editSpec
                            | comboBoxSpec
                            | listBoxSpec
                            | iconSpec
                            | scrollBarSpec
                            | controlSpec
                            ; 

pushButtonSpec              : buttonType string  COMMA name COMMA rectangleSpec 
                            | buttonType string  COMMA name COMMA rectangleSpec COMMA buttonStyleExpr
                            | buttonType string  COMMA name COMMA rectangleSpec COMMA extendedWindowStyleExpr
                            | buttonType string  COMMA name COMMA rectangleSpec COMMA buttonStyleExpr COMMA extendedWindowStyleExpr
                            ;

buttonStyleExpr             : buttonStyleExpr OR buttonStyle
                            | buttonStyle
                            ;

buttonStyle                 : ws_style
                            | bs_style
                            ;

extendedWindowStyleExpr     : extendedWindowStyleExpr OR ws_ex_style
                            | ws_ex_style
                            ;

buttonType                  : PUSHBUTTON 
                            | DEFPUSHBUTTON 
                            | GROUPBOX
                            ;

lrTextSpec                  : lrText string COMMA name COMMA rectangleSpec
                            | lrText string COMMA name COMMA rectangleSpec COMMA staticStyleExpr
                            | lrText string COMMA name COMMA rectangleSpec COMMA extendedWindowStyleExpr
                            | lrText string COMMA name COMMA rectangleSpec COMMA staticStyleExpr COMMA extendedWindowStyleExpr
                            ;

lrText                      : LTEXT 
                            | RTEXT 
                            ;

staticStyleExpr             : staticStyleExpr OR staticStyle
                            | staticStyle
                            ;

staticStyle                 : ws_style
                            | ss_style
                            ;

editSpec                    : EDITTEXT name COMMA rectangleSpec 
                            | EDITTEXT name COMMA rectangleSpec COMMA editStyleExpr
                            | EDITTEXT name COMMA rectangleSpec COMMA extendedWindowStyleExpr
                            | EDITTEXT name COMMA rectangleSpec COMMA editStyleExpr COMMA extendedWindowStyleExpr
                            ;

editStyleExpr               : editStyleExpr OR editStyle
                            | editStyle
                            ;

editStyle                   : ws_style
                            | es_style
                            ;

comboBoxSpec                : COMBOBOX name COMMA rectangleSpec
                            | COMBOBOX name COMMA rectangleSpec COMMA comboBoxStyleExpr
                            | COMBOBOX name COMMA rectangleSpec COMMA extendedWindowStyleExpr
                            | COMBOBOX name COMMA rectangleSpec COMMA comboBoxStyleExpr COMMA extendedWindowStyleExpr
                            ;

comboBoxStyleExpr           : comboBoxStyleExpr OR comboBoxStyle
                            | comboBoxStyle
                            ;

comboBoxStyle               : ws_style
                            | cbs_style
                            ;

listBoxSpec                 : LISTBOX name COMMA rectangleSpec
                            | LISTBOX name COMMA rectangleSpec COMMA listBoxStyleExpr 
                            | LISTBOX name COMMA rectangleSpec COMMA extendedWindowStyleExpr
                            | LISTBOX name COMMA rectangleSpec COMMA listBoxStyleExpr COMMA extendedWindowStyleExpr
                            ;

listBoxStyleExpr            : listBoxStyleExpr OR listBoxStyle
                            | listBoxStyle
                            ;

listBoxStyle                : ws_style
                            | lbs_style
                            ;

iconSpec                    : ICON name COMMA name COMMA rectangleSpec
                            ;

scrollBarSpec               : SCROLLBAR name COMMA rectangleSpec
                            | SCROLLBAR name COMMA rectangleSpec COMMA scrollBarStyleExpr
                            | SCROLLBAR name COMMA rectangleSpec COMMA extendedWindowStyleExpr
                            | SCROLLBAR name COMMA rectangleSpec COMMA scrollBarStyleExpr COMMA extendedWindowStyleExpr
                            ;

scrollBarStyleExpr          : scrollBarStyleExpr OR scrollBarStyle
                            | scrollBarStyle
                            ;

scrollBarStyle              : ws_style
                            | sbs_style
                            ;

controlSpec                 : CONTROL string COMMA name COMMA string COMMA rectangleSpec
                            | CONTROL string COMMA name COMMA string COMMA controlStyleExpr COMMA rectangleSpec
                            | CONTROL string COMMA name COMMA string COMMA extendedWindowStyleExpr COMMA rectangleSpec
                            | CONTROL string COMMA name COMMA string COMMA controlStyleExpr COMMA extendedWindowStyleExpr COMMA rectangleSpec
                            ;

controlStyleExpr            : controlStyleExpr OR ctrl_style
                            | ctrl_style
                            ;

ctrl_style                  : ws_style
                            | ss_style
                            | bs_style
                            | tvs_style
                            | lvs_style
                            | uds_style
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

ws_style                    : add_ws_style
                            | NOT add_ws_style
                            ;

add_ws_style                : _WS_OVERLAPPED 
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

tvs_style                   : _TVS_HASBUTTONS
                            | _TVS_HASLINES
                            | _TVS_LINESATROOT
                            | _TVS_SHOWSELALWAYS
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

guideLinesDefinition        : GUIDELINES DESIGNINFO DISCARDABLE BEGIN designInfoList END
                            ;

designInfoList              : designInfoList designInfo
                            | designInfo
                            ;

designInfo                  : BEGIN designSpecList END
                            ;

designSpecList              : designSpecList designSpec
                            | designSpec
                            ;

designSpec                  : marginSpec COMMA number
                            ;

marginSpec                  : LEFTMARGIN
                            | RIGHTMARGIN
                            | TOPMARGIN
                            | BOTTOMMARGIN
                            ;

toolbarDefnition            : name TOOLBAR DISCARDABLE sizeSpec BEGIN buttonSpecList END
                            ;

buttonSpecList              : buttonSpecList buttonSpec
                            | buttonSpec
                            ;

buttonSpec                  : BUTTON name
                            ;

menuDefinition              : name MENU DISCARDABLE menuBody
                            ;

menuBody                    : BEGIN menuSpecList END
                            ;

menuSpecList                : menuSpecList menuSpec
                            | menuSpec
                            ;

menuSpec                    : popupMenu
                            | menuItem
                            ;

popupMenu                   : POPUP string menuBody
                            ;

menuItem                    : MENUITEM string COMMA name menuItemModifierList
                            | MENUITEM SEPARATOR
                            ;

menuItemModifierList        : /* empty */
                            | menuItemModifierList COMMA menuItemModifier
                            ;

menuItemModifier            : GRAYED
                            | CHECKED
                            ;

acceleratorsDefinition      : name ACCELERATORS DISCARDABLE BEGIN acceleratorList END
                            ;

acceleratorList             : acceleratorList accelerator
                            | accelerator
                            ;

accelerator                 : string COMMA name acceleratorSpecList
                            | name   COMMA name acceleratorSpecList
                            ;

acceleratorSpecList         : /* empty */
                            | acceleratorSpecList acceleratorSpec
                            ;

acceleratorSpec             : COMMA acceleratorModifier
                            ;

acceleratorModifier         : VIRTKEY
                            | ALT
                            | SHIFT
                            | NOINVERT
                            | CONTROL
                            ;

stringTableDefnition        : STRINGTABLE DISCARDABLE BEGIN stringSpecList END
                            ;


stringSpecList              : stringSpecList stringSpec
                            | stringSpec
                            ;

stringSpec                  : name string
                            ;
    
rectangleSpec               : number COMMA number COMMA number COMMA number
                            ;

sizeSpec                    : number COMMA number 
                            ;

name                        : NAME
                            ;

number                      : NUMBER
                            ;

string                      : STRING
                            ;


%%

SyntaxNode *PreprocessorParser::newNode(const SourcePosition &pos, int symbol, ...) {
  va_list argptr;
  va_start(argptr,symbol);
  SyntaxNode *p = m_tree.vFetchNode(pos,symbol,argptr);
  va_end(argptr);
  return p;
}

void PreprocessorParser::verror(const SourcePosition &pos, const char *format, va_list argptr) {
  m_tree.vAddError(&pos,format,argptr);
}
