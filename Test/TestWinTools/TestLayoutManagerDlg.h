#pragma once

class CTestLayoutManagerDlg : public CDialog {
private:
  int getWindowFlags() const;
  int getList1Flags()  const;
  int getlist2Flags()  const;
  int getButtonFlags() const;
  void setToDefault();

public:
    CTestLayoutManagerDlg(CWnd *pParent = nullptr);

    enum { IDD = IDD_TESTLAYOUTMANAGER_DIALOG };
    BOOL    m_windowRetainAspectRatio;
    BOOL    m_windowResizeFont   ;
    BOOL    m_windowFontRelativeSize;
    BOOL    m_list1ResizeFont    ;
    BOOL    m_list1FontRelativeSize;
    BOOL    m_list1RelativeLeft;
    BOOL    m_list1PctRelativeLeft;
    BOOL    m_list1PctRelativeXCenter;
    BOOL    m_list1RelativeTop;
    BOOL    m_list1PctRelativeTop;
    BOOL    m_list1PctRelativeYCenter;
    BOOL    m_list1RelativeRight;
    BOOL    m_list1PctRelativeRight;
    BOOL    m_list1RelativeBottom;
    BOOL    m_list1PctRelativeBottom;
    BOOL    m_list1FontRelativeXPosition;
    BOOL    m_list1FontRelativeYPosition;
    BOOL    m_list1ConstantWidth;
    BOOL    m_list1ConstantHeight;
    BOOL    m_list2ResizeFont;
    BOOL    m_list2FontRelativeSize;
    BOOL    m_list2RelativeLeft;
    BOOL    m_list2PctRelativeLeft;
    BOOL    m_list2PctRelativeXCenter;
    BOOL    m_list2RelativeTop;
    BOOL    m_list2PctRelativeTop;
    BOOL    m_list2PctRelativeYCenter;
    BOOL    m_list2RelativeRight ;
    BOOL    m_list2PctRelativeRight ;
    BOOL    m_list2RelativeBottom;
    BOOL    m_list2PctRelativeBottom;
    BOOL    m_list2FontRelativeXPosition;
    BOOL    m_list2FontRelativeYPosition;
    BOOL    m_list2ConstantWidth;
    BOOL    m_list2ConstantHeight;
    BOOL    m_list2InitListheaders;
    BOOL    m_list2ResizeListheaders;
    BOOL    m_buttonFontRelativeSize;
    BOOL    m_buttonFontRelativeXPosition;
    BOOL    m_buttonFontRelativeYPosition;
    BOOL    m_buttonRelativeLeft;
    BOOL    m_buttonPctRelativeLeft;
    BOOL    m_buttonPctRelativeXCenter;
    BOOL    m_buttonRelativeTop;
    BOOL    m_buttonPctRelativeTop;
    BOOL    m_buttonPctRelativeYCenter;
    BOOL    m_buttonRelativeRight;
    BOOL    m_buttonPctRelativeRight;
    BOOL    m_buttonRelativeBottom;
    BOOL    m_buttonPctRelativeBottom;
    BOOL    m_buttonConstantWidth;
    BOOL    m_buttonConstantHeight;
    BOOL    m_buttonResizeFont;
protected:
    virtual void DoDataExchange(CDataExchange *pDX);
    afx_msg void OnButtonOpenDialog();
    afx_msg void OnButtonSetToDefault();
    DECLARE_MESSAGE_MAP()
};

