#pragma once

#include <HashMap.h>
#include <MFCUtil/LayoutManager.h>
#include <MFCUtil/OBMButton.h>

#define MAXHELPBUTTONS 10

class CExprDialog : public CDialog {
private:
  int                    m_helpButtonCount;
  OBMButton              m_helpButton[MAXHELPBUTTONS];
  CFont                  m_exprFont;
  IntHashMap<int>        m_helpButtonMap;
  int                    m_selectedExprId;
  void createExprFont();
  void setExprFont(int id);
  void createMenuExprHelp(CMenu &menu);
  void showExprError(const String &msg, int id);
  void showExprHelpMenu(int id);
  void handleSelectedExprHelpId(int menuId, int ctrlId);
  String getExprSyntax(int index);
  void substituteSelectedText(int ctrlId, const String &s);

protected:
  SimpleLayoutManager m_layoutManager;

  CExprDialog(int resId, CWnd *pParent) : CDialog(resId, pParent) {
    m_helpButtonCount = 0;
  }
  void createExprHelpButton(int buttonId, int exprEditId);
  void handleExprHelpButtonClick(int buttonId);
  void gotoExpr(int id) {
    GetDlgItem(id)->SetFocus();
  }

  CEdit *getExprField( int id);
  String getExprString(int id);

  void gotoMatchingParentesis();

  bool validateAllExpr();
  bool validateExpr(int id);
  bool validateInterval(int fromId, int toId);
  virtual bool validate() = 0;
  BOOL PreTranslateMessage(MSG *pMsg);
};

template <class T> class SaveLoadExprDialog : public CExprDialog {
private:
  const String m_paramTypeName;
  const String m_extension;
  const int    m_resId;
  HACCEL       m_accelTable;
  T            m_param;
  CString      m_name;

  String getFileDialogFilter() const {
    return format(_T("%s-files (*.%s%c*.%s%cAll files (*.*)%c*.*%c%c")
                 ,m_paramTypeName.cstr()
                 ,m_extension.cstr(),0
                 ,m_extension.cstr(),0
                 ,0,0,0
                 );
  }

protected:
  virtual void paramToWin(const T &param) {
    setWindowText(this, format(_T("%s (%s)"), m_paramTypeName.cstr(), param.getDisplayName().cstr()));
    m_name = param.getName().cstr();
  }
  virtual void winToParam(T &param) const {
    param.setName((LPCTSTR)m_name);
  };

  SaveLoadExprDialog(int resId, CWnd *pParent, const T &param, const String &paramTypeName, const String &extension)
   : CExprDialog(    resId, pParent)
   , m_resId(        resId         )
   , m_param(        param         )
   , m_paramTypeName(paramTypeName )
   , m_extension(    extension     )
  {
  }

  virtual BOOL OnInitDialog() {
    __super::OnInitDialog();
    m_accelTable = LoadAccelerators(theApp.m_hInstance,MAKEINTRESOURCE(m_resId));
    paramToWin(m_param);
    return TRUE;
  }

  BOOL PreTranslateMessage(MSG *pMsg) {
    if(TranslateAccelerator(m_hWnd, m_accelTable, pMsg)) {
      return true;
    }
    return __super::PreTranslateMessage(pMsg);
  }

  void OnOK() {
    if(!UpdateData() || !validate()) return;
    winToParam(m_param);
    __super::OnOK();
  }

  void OnFileOpen() {
    CFileDialog dlg(TRUE);
    const String dlgTitle  = format(_T("Open %s"), m_paramTypeName.cstr());
    const String dlgFilter = getFileDialogFilter();
    dlg.m_ofn.lpstrFilter = dlgFilter.cstr();
    dlg.m_ofn.lpstrTitle  = dlgTitle.cstr();
    if((dlg.DoModal() != IDOK) || (_tcslen(dlg.m_ofn.lpstrFile) == 0)) {
      return;
    }
    try {
      T param;
      param.load(dlg.m_ofn.lpstrFile);
      paramToWin(param);
    } catch(Exception e) {
      showException(e);
    }
  }

  void OnFileSave() {
    if(!UpdateData() || !validate()) return;
    T param;
    winToParam(param);
    if(param.hasDefaultName()) {
      saveAs(param);
    } else {
      save(param.getName(), param);
    }
  }

  void OnFileSaveAs() {
    if(!UpdateData() || !validate()) return;
    T param;
    winToParam(param);
    saveAs(param);
  }

  void saveAs(T &param) {
    CString objname = param.getName().cstr();
    const String dlgTitle         = format(_T("Save %s"), m_paramTypeName.cstr());
    const String defaultExtension = format(_T("*.%s"), m_extension.cstr());
    const String dlgFilter        = getFileDialogFilter();
    CFileDialog dlg(FALSE, defaultExtension.cstr(), objname);
    dlg.m_ofn.lpstrFilter = dlgFilter.cstr();
    dlg.m_ofn.lpstrTitle  = dlgTitle.cstr();
    if((dlg.DoModal() != IDOK) ||(_tcslen(dlg.m_ofn.lpstrFile) == 0)) {
      return;
    }
    save(dlg.m_ofn.lpstrFile, param);
  }

  void save(const String &fileName, T &param) {
    try {
      param.save(fileName);
      paramToWin(param);
    } catch(Exception e) {
      showException(e);
    }
  }
public:
  const T &getData() const {
    return m_param;
  }
};