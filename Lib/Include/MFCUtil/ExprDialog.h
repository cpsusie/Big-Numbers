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
  // if >= 0, fieldId of CEdit that will be prefixed to all expressions in dialog
  int                    m_commonExprId;
  IntHashMap<int>        m_helpButtonMap;
  int                    m_selectedExprId;
  FILE                  *m_listFile;

  void createExprFont();
  void setExprFont(int id);
  void createMenuExprHelp(CMenu &menu);
  void showExprError(const String &msg, int id);
  void showExprHelpMenu(int id);
  void handleSelectedExprHelpId(int menuId, int ctrlId);
  String getExprSyntax(int index);
  void openListFile();
  void closeListFile();
  inline bool isListFileOpen() const {
    return m_listFile != nullptr;
  }
  void substituteSelectedText(int ctrlId, const String &s);

protected:

  CExprDialog(int resId, CWnd *pParent) : CDialog(resId, pParent) {
    m_helpButtonCount = 0;
    m_commonExprId    = -1;
    m_listFile        = nullptr;
  }
  virtual ~CExprDialog() {
    closeListFile();
  }
  void createExprHelpButton(int buttonId, int exprEditId);
  void handleExprHelpButtonClick(int buttonId);
  void gotoExpr(int id) {
    GetDlgItem(id)->SetFocus();
  }
  inline void   setCommonExprFieldId(int id) {
    m_commonExprId = id;
    if(id >= 0) setExprFont(id);
  }
  inline int    getCommonExprFieldId() const {
    return m_commonExprId;
  }
  inline bool   hasCommonExprField() const {
    return m_commonExprId >= 0;
  }
  inline String getCommonExprText() const {
    return hasCommonExprField() ? getWindowText(this, getCommonExprFieldId()) : EMPTYSTRING;
  }
  inline CEdit *getExprField(int id) {
    return (CEdit*)GetDlgItem(id);
  }
  inline String getExprText(int id) const {
    return getCommonExprText() + getWindowText(this, id);
  }
  inline void gotoMatchingParentesis() {
    ::gotoMatchingParanthes(this, getFocusCtrlId(this));
  }
  CExprDialog &enableDynamicLayoutHelper(bool enable);
  bool hasDynamicLayout(int ctrlId) const;
  bool validateAllExpr();
  bool validateExpr(int id);
  bool validateInterval(int fromId, int toId);
  bool validateMinMax(int id, double min, double max);
  virtual bool validate() = 0;
  bool generateListFile() const {
    return getListFileName().length() > 0;
  }
  // should return empty string, if listfile not needed. Returning a non-empty string, a file with the returned
  // name will be created on successfull compilation
  virtual String getListFileName() const {
    return EMPTYSTRING;
  }
  virtual void addToRecent(const String &fileName) {
  }

  BOOL PreTranslateMessage(MSG *pMsg);
  virtual void OnFileNew() {
  }
  virtual void OnFileOpen() {
  }
  virtual void OnFileSave() {
  }
  virtual void OnFileSaveAs() {
  }
};

template <typename T> class SaveLoadExprDialog : public CExprDialog {
private:
  const String m_paramTypeName;
  const String m_extension;
  const int    m_resId;
  HACCEL       m_accelTable;
  T            m_param;
  CString      m_name;

  String getFileDialogFilter() const {
    return format(_T("%s-files (*.%s)%c*.%s%cAll files (*.*)%c*.*%c%c")
                 ,m_paramTypeName.cstr()
                 ,m_extension.cstr(),0
                 ,m_extension.cstr(),0
                 ,0,0,0
                 );
  }

  inline bool updateAndValidate() {
    return UpdateData() && validate();
  }

  inline bool nameEntered(CFileDialog &dlg) {
    return (dlg.DoModal() == IDOK) && (_tcslen(dlg.m_ofn.lpstrFile) > 0);
  }


protected:
  virtual void paramToWin(const T &param) {
    setWindowText(this, format(_T("%s (%s)")
                      , firstLetterToUpperCase(m_paramTypeName).cstr()
                      , param.getDisplayName().cstr()));
    m_name = param.getName().cstr();
    UpdateData(FALSE);
  }
  virtual bool winToParam(T &param) {
    const String oldName = param.getName();
    param.setName((LPCTSTR)m_name);
    try {
      const bool ok = updateAndValidate();
      if(!ok) param.setName(oldName);
      return ok;
    } catch(...) {
      param.setName(oldName);
      throw;
    }
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
    try {
      if(!winToParam(m_param)) return;
      __super::OnOK();
    } catch(Exception e) {
      showException(e);
    }
  }

  void OnFileNew() {
    try {
      T param;
      paramToWin(param);
    } catch(Exception e) {
      showException(e);
    }
  }

  void OnFileOpen() {
    try {
      CFileDialog dlg(TRUE);
      const String dlgTitle  = format(_T("Open %s"), m_paramTypeName.cstr());
      const String dlgFilter = getFileDialogFilter();
      dlg.m_ofn.lpstrFilter = dlgFilter.cstr();
      dlg.m_ofn.lpstrTitle  = dlgTitle.cstr();
      if(!nameEntered(dlg)) {
        return;
      }
      T param;
      const String fileName = dlg.m_ofn.lpstrFile;
      param.load(fileName);
      paramToWin(param);
      addToRecent(fileName);
    } catch(Exception e) {
      showException(e);
    }
  }

  void OnFileSave() {
    try {
      T param;
      if(!winToParam(param)) return;
      if(param.hasDefaultName()) {
        saveAs(param);
      } else {
        save(param.getName(), param);
      }
    } catch(Exception e) {
      showException(e);
    }
  }

  void OnFileSaveAs() {
    try {
      T param;
      if(!winToParam(param)) return;
      saveAs(param);
    } catch(Exception e) {
      showException(e);
    }
  }

  void saveAs(T &param) {
    const String objname          = param.getName();
    const String dlgTitle         = format(_T("Save %s"), m_paramTypeName.cstr());
    const String defaultExtension = format(_T("*.%s"), m_extension.cstr());
    const String dlgFilter        = getFileDialogFilter();
    CFileDialog dlg(FALSE, defaultExtension.cstr(), objname.cstr());
    dlg.m_ofn.lpstrFilter = dlgFilter.cstr();
    dlg.m_ofn.lpstrTitle  = dlgTitle.cstr();
    if(!nameEntered(dlg)) {
      return;
    }
    save(dlg.m_ofn.lpstrFile, param);
  }

  void save(const String &fileName, T &param) {
    try {
      param.save(fileName);
      paramToWin(param);
      addToRecent(fileName);
    } catch(Exception e) {
      showException(e);
    }
  }
public:
  const T &getData() const {
    return m_param;
  }
};
