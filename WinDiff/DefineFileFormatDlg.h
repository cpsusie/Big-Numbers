#pragma once

#include "resource.h"
#include <MFCUtil/OBMButton.h>
#include "FileFormat.h"

class CDefineFileFormatDlg: public CDialog {
  OBMButton            m_delimiterCharButton;
public:
    CDefineFileFormatDlg(FileFormat &param, const String &sampleline, CWnd *pParent = NULL);
    FileFormat &m_param;

    enum { IDD = IDD_DIALOGCOLUMNS };
    UINT    m_columnFrom;
    UINT    m_columnTo;
    CString m_fieldDelimiter;
    CString m_textQualifier;
    CString m_sampleLine;
    BOOL    m_multipleDelimiters;

    HACCEL     m_accelTable;
    String     m_name;
    FileFormat m_lastCheckpoint;
    int        m_currentControl;
    bool validate();
    bool validateAndAdd();
    void setDelimited(bool value);
    int  flags() const;
    void enableDropdowns(bool enable);
    void setTitle();
    void paramToWindow(  const FileFormat &param);
    void paramFromWindow(      FileFormat &param);
    bool save();
    bool checkSave();
    int  getFrom();
    int  getTo();
    void setFrom(int value);
    void setTo(int value);
    int  findSampleStartSel(int fromfield);
    int  findSampleEndSel(int tofield);
    int  findFrom();
    int  findTo();
    void ajourSample();
    void ajourSample(int fromfield, int tofield);
    void ajourFromTo();
    void ajourDeleteButton();
    void ajourAddButton();
    bool delimiterMode();
    _TUCHAR getTextQualifier();
    String getDelimiters();
    void addDelimiter(const TCHAR *s);

public:
    virtual BOOL PreTranslateMessage(MSG *pMsg);
    virtual void DoDataExchange(CDataExchange *pDX);
    virtual BOOL OnInitDialog();
    virtual void OnOK();
    virtual void OnCancel();
    afx_msg void OnFormatNew();
    afx_msg void OnFileOpen();
    afx_msg void OnFileSave();
    afx_msg void OnFileSaveAs();
    afx_msg void OnDelimEscSymbolTab();
    afx_msg void OnDelimEscSymbolSpace();
    afx_msg void OnDelimEscSymbolCr();
    afx_msg void OnDelimEscSymbolFormFeed();
    afx_msg void OnDelimEscSymbolBackSpace();
    afx_msg void OnDelimEscSymbolNewLine();
    afx_msg void OnDelimEscSymbolEsc();
    afx_msg void OnDelimEscSymbolUnicode();
    afx_msg void OnDelimEscSymbolBackslash();
    afx_msg void OnDelimEscSymbolCtrl();
    afx_msg void OnButtonAdd();
    afx_msg void OnButtonDelete();
    afx_msg void OnRadioDelimited();
    afx_msg void OnCheckMultipleDelimiters();
    afx_msg void OnButtonDelimiterMenu();
    afx_msg void OnSetFocusEditSample();
    afx_msg void OnKillFocusEditSample();
    afx_msg void OnSetFocusListColumns();
    afx_msg void OnKillFocusListColumns();
    afx_msg void OnSelChangeListColumns();
    afx_msg void OnChangeEditTextQualifier();
    afx_msg void OnChangeEditFieldDelimiter();
    afx_msg void OnChangeEditFrom();
    afx_msg void OnChangeEditTo();
    DECLARE_MESSAGE_MAP()
};
