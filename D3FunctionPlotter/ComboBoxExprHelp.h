#pragma once

class CComboBoxExprHelp : public CComboBox {
private:
  int  m_columnWidth1, m_columnWidth2, m_itemHeight;
  int  m_itemCount;
  void findColumnWidth(CWnd *parent);
  int getDropDownWidth() const;
public:

  CComboBoxExprHelp();
  virtual ~CComboBoxExprHelp();

  void substituteControl(CWnd *parent, int ctrlId);


	public:

  void MeasureItem(LPMEASUREITEMSTRUCT lpMeasureItemStruct);
  void DrawItem(   LPDRAWITEMSTRUCT lpDrawItemStruct);

  String getSelectedString();
protected:

	afx_msg void OnDropdown();

    DECLARE_MESSAGE_MAP()

};

// void initExprHelpCombo(CComboBox *ctrl);

