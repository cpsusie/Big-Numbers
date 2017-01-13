#include "stdafx.h"

#ifndef WM_GETOBJECT
#define WM_GETOBJECT                    0x003D
#endif

#if(WINVER < 0x0500)

#define WM_MENURBUTTONUP                0x0122
#define WM_MENUDRAG                     0x0123
#define WM_MENUGETOBJECT                0x0124
#define WM_UNINITMENUPOPUP              0x0125
#define WM_MENUCOMMAND                  0x0126
#define WM_IME_REQUEST                  0x0288

#define WM_NCMOUSEHOVER                 0x02A0
#define WM_NCMOUSELEAVE                 0x02A2
#endif

#define caseStr(m) case m: return _T(#m);

String getMessageName(int msg) {
  switch(msg) {
  caseStr(WM_CREATE                       );
  caseStr(WM_DESTROY                      );
  caseStr(WM_MOVE                         );
  caseStr(WM_SIZE                         );
  caseStr(WM_ACTIVATE                     );
  caseStr(WM_SETFOCUS                     );
  caseStr(WM_KILLFOCUS                    );
  caseStr(WM_ENABLE                       );
  caseStr(WM_SETREDRAW                    );
  caseStr(WM_SETTEXT                      );
  caseStr(WM_GETTEXT                      );
  caseStr(WM_GETTEXTLENGTH                );
  caseStr(WM_PAINT                        );
  caseStr(WM_CLOSE                        );
  caseStr(WM_QUERYENDSESSION              );
  caseStr(WM_QUIT                         );
  caseStr(WM_QUERYOPEN                    );
  caseStr(WM_ERASEBKGND                   );
  caseStr(WM_SYSCOLORCHANGE               );
  caseStr(WM_ENDSESSION                   );
  caseStr(WM_SHOWWINDOW                   );
  caseStr(WM_WININICHANGE                 );
  caseStr(WM_DEVMODECHANGE                );
  caseStr(WM_ACTIVATEAPP                  );
  caseStr(WM_FONTCHANGE                   );
  caseStr(WM_TIMECHANGE                   );
  caseStr(WM_CANCELMODE                   );
  caseStr(WM_SETCURSOR                    );
  caseStr(WM_MOUSEACTIVATE                );
  caseStr(WM_CHILDACTIVATE                );
  caseStr(WM_QUEUESYNC                    );
  caseStr(WM_GETMINMAXINFO                );
  caseStr(WM_PAINTICON                    );
  caseStr(WM_ICONERASEBKGND               );
  caseStr(WM_NEXTDLGCTL                   );
  caseStr(WM_SPOOLERSTATUS                );
  caseStr(WM_DRAWITEM                     );
  caseStr(WM_MEASUREITEM                  );
  caseStr(WM_DELETEITEM                   );
  caseStr(WM_VKEYTOITEM                   );
  caseStr(WM_CHARTOITEM                   );
  caseStr(WM_SETFONT                      );
  caseStr(WM_GETFONT                      );
  caseStr(WM_SETHOTKEY                    );
  caseStr(WM_GETHOTKEY                    );
  caseStr(WM_QUERYDRAGICON                );
  caseStr(WM_COMPAREITEM                  );
  caseStr(WM_GETOBJECT                    );
  caseStr(WM_COMPACTING                   );
  caseStr(WM_COMMNOTIFY                   );
  caseStr(WM_WINDOWPOSCHANGING            );
  caseStr(WM_WINDOWPOSCHANGED             );
  caseStr(WM_POWER                        );
  caseStr(WM_COPYDATA                     );
  caseStr(WM_CANCELJOURNAL                );
  caseStr(WM_NOTIFY                       );
  caseStr(WM_INPUTLANGCHANGEREQUEST       );
  caseStr(WM_INPUTLANGCHANGE              );
  caseStr(WM_TCARD                        );
  caseStr(WM_HELP                         );
  caseStr(WM_USERCHANGED                  );
  caseStr(WM_NOTIFYFORMAT                 );
  caseStr(WM_CONTEXTMENU                  );
  caseStr(WM_STYLECHANGING                );
  caseStr(WM_STYLECHANGED                 );
  caseStr(WM_DISPLAYCHANGE                );
  caseStr(WM_GETICON                      );
  caseStr(WM_SETICON                      );
  caseStr(WM_NCCREATE                     );
  caseStr(WM_NCDESTROY                    );
  caseStr(WM_NCCALCSIZE                   );
  caseStr(WM_NCHITTEST                    );
  caseStr(WM_NCPAINT                      );
  caseStr(WM_NCACTIVATE                   );
  caseStr(WM_GETDLGCODE                   );
  caseStr(WM_SYNCPAINT                    );
  caseStr(WM_NCMOUSEMOVE                  );
  caseStr(WM_NCLBUTTONDOWN                );
  caseStr(WM_NCLBUTTONUP                  );
  caseStr(WM_NCLBUTTONDBLCLK              );
  caseStr(WM_NCRBUTTONDOWN                );
  caseStr(WM_NCRBUTTONUP                  );
  caseStr(WM_NCRBUTTONDBLCLK              );
  caseStr(WM_NCMBUTTONDOWN                );
  caseStr(WM_NCMBUTTONUP                  );
  caseStr(WM_NCMBUTTONDBLCLK              );
  caseStr(WM_KEYDOWN                      );
  caseStr(WM_KEYUP                        );
  caseStr(WM_CHAR                         );
  caseStr(WM_DEADCHAR                     );
  caseStr(WM_SYSKEYDOWN                   );
  caseStr(WM_SYSKEYUP                     );
  caseStr(WM_SYSCHAR                      );
  caseStr(WM_SYSDEADCHAR                  );
  caseStr(WM_KEYLAST                      );
  caseStr(WM_IME_STARTCOMPOSITION         );
  caseStr(WM_IME_ENDCOMPOSITION           );
  caseStr(WM_IME_COMPOSITION              );
  caseStr(WM_INITDIALOG                   );
  caseStr(WM_COMMAND                      );
  caseStr(WM_SYSCOMMAND                   );
  caseStr(WM_TIMER                        );
  caseStr(WM_HSCROLL                      );
  caseStr(WM_VSCROLL                      );
  caseStr(WM_INITMENU                     );
  caseStr(WM_INITMENUPOPUP                );
  caseStr(WM_MENUSELECT                   );
  caseStr(WM_MENUCHAR                     );
  caseStr(WM_ENTERIDLE                    );
  caseStr(WM_MENURBUTTONUP                );
  caseStr(WM_MENUDRAG                     );
  caseStr(WM_MENUGETOBJECT                );
  caseStr(WM_UNINITMENUPOPUP              );
  caseStr(WM_MENUCOMMAND                  );
  caseStr(WM_CTLCOLORMSGBOX               );
  caseStr(WM_CTLCOLOREDIT                 );
  caseStr(WM_CTLCOLORLISTBOX              );
  caseStr(WM_CTLCOLORBTN                  );
  caseStr(WM_CTLCOLORDLG                  );
  caseStr(WM_CTLCOLORSCROLLBAR            );
  caseStr(WM_CTLCOLORSTATIC               );
  caseStr(WM_MOUSEMOVE                    );
  caseStr(WM_LBUTTONDOWN                  );
  caseStr(WM_LBUTTONUP                    );
  caseStr(WM_LBUTTONDBLCLK                );
  caseStr(WM_RBUTTONDOWN                  );
  caseStr(WM_RBUTTONUP                    );
  caseStr(WM_RBUTTONDBLCLK                );
  caseStr(WM_MBUTTONDOWN                  );
  caseStr(WM_MBUTTONUP                    );
  caseStr(WM_MBUTTONDBLCLK                );
  caseStr(WM_MOUSEWHEEL                   );
  caseStr(WM_PARENTNOTIFY                 );
  caseStr(WM_ENTERMENULOOP                );
  caseStr(WM_EXITMENULOOP                 );
  caseStr(WM_NEXTMENU                     );
  caseStr(WM_SIZING                       );
  caseStr(WM_CAPTURECHANGED               );
  caseStr(WM_MOVING                       );
  caseStr(WM_POWERBROADCAST               );
  caseStr(WM_DEVICECHANGE                 );
  caseStr(WM_MDICREATE                    );
  caseStr(WM_MDIDESTROY                   );
  caseStr(WM_MDIACTIVATE                  );
  caseStr(WM_MDIRESTORE                   );
  caseStr(WM_MDINEXT                      );
  caseStr(WM_MDIMAXIMIZE                  );
  caseStr(WM_MDITILE                      );
  caseStr(WM_MDICASCADE                   );
  caseStr(WM_MDIICONARRANGE               );
  caseStr(WM_MDIGETACTIVE                 );
  caseStr(WM_MDISETMENU                   );
  caseStr(WM_ENTERSIZEMOVE                );
  caseStr(WM_EXITSIZEMOVE                 );
  caseStr(WM_DROPFILES                    );
  caseStr(WM_MDIREFRESHMENU               );
  caseStr(WM_IME_SETCONTEXT               );
  caseStr(WM_IME_NOTIFY                   );
  caseStr(WM_IME_CONTROL                  );
  caseStr(WM_IME_COMPOSITIONFULL          );
  caseStr(WM_IME_SELECT                   );
  caseStr(WM_IME_CHAR                     );
  caseStr(WM_IME_REQUEST                  );
  caseStr(WM_IME_KEYDOWN                  );
  caseStr(WM_IME_KEYUP                    );
  caseStr(WM_MOUSEHOVER                   );
  caseStr(WM_MOUSELEAVE                   );
  caseStr(WM_NCMOUSEHOVER                 );
  caseStr(WM_NCMOUSELEAVE                 );
  caseStr(WM_CUT                          );
  caseStr(WM_COPY                         );
  caseStr(WM_PASTE                        );
  caseStr(WM_CLEAR                        );
  caseStr(WM_UNDO                         );
  caseStr(WM_RENDERFORMAT                 );
  caseStr(WM_RENDERALLFORMATS             );
  caseStr(WM_DESTROYCLIPBOARD             );
  caseStr(WM_DRAWCLIPBOARD                );
  caseStr(WM_PAINTCLIPBOARD               );
  caseStr(WM_VSCROLLCLIPBOARD             );
  caseStr(WM_SIZECLIPBOARD                );
  caseStr(WM_ASKCBFORMATNAME              );
  caseStr(WM_CHANGECBCHAIN                );
  caseStr(WM_HSCROLLCLIPBOARD             );
  caseStr(WM_QUERYNEWPALETTE              );
  caseStr(WM_PALETTEISCHANGING            );
  caseStr(WM_PALETTECHANGED               );
  caseStr(WM_HOTKEY                       );
  caseStr(WM_PRINT                        );
  caseStr(WM_PRINTCLIENT                  );
  caseStr(WM_HANDHELDFIRST                );
  caseStr(WM_HANDHELDLAST                 );
  caseStr(WM_AFXFIRST                     );
  caseStr(WM_AFXLAST                      );
  caseStr(WM_PENWINFIRST                  );
  caseStr(WM_PENWINLAST                   );
  default: return format(_T("Unknown messageId:%#04x (%d)"), msg, msg);
  }
}
