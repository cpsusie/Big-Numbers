#pragma once

#include <BitSet.h>
#include <TinyBitSet.h>

typedef struct {
  COLORREF      m_textEqual, m_bckEqual;
  COLORREF      m_textDiff , m_bckDiff;
} DiffMarkColorSet;

class CListBoxDiffMarks : public CListBox {
private:
  StringArray      m_lastContent;
  Array<BitSet>    m_diffSetArray;
  bool             m_hasFocus;
  bool             m_diffMarksEnabled;
  BitSet8          m_colorsInitialized;
  CSize            m_charSize;
  DiffMarkColorSet m_colorSet[2]; // 0 for not selected, 1 for selected
  void fillSolidRect(CDC &dc, size_t i1, size_t i2, LPDRAWITEMSTRUCT lpDrawItemStruct, COLORREF color);
  DECLARE_DYNAMIC(CListBoxDiffMarks)

public:
  CListBoxDiffMarks();
  virtual ~CListBoxDiffMarks();
  void substituteControl(CWnd *parent, int id);
  void clear();
  void setLines(const StringArray &lines);
  void enableDiffMarks(bool enable);
  void setEqualSelected(   COLORREF txtColor, COLORREF bckColor);
  void setEqualNotSelected(COLORREF txtColor, COLORREF bckColor);
  void setDiffSelected(    COLORREF txtColor, COLORREF bckColor);
  void setDiffNotSelected( COLORREF txtColor, COLORREF bckColor);
protected:
  afx_msg void DrawItem(   LPDRAWITEMSTRUCT    lpDrawItemStruct   );
  afx_msg void MeasureItem(LPMEASUREITEMSTRUCT lpMeasureItemStruct);
  afx_msg int  CompareItem(LPCOMPAREITEMSTRUCT lpCompareItemStruct);
  afx_msg void DeleteItem( LPDELETEITEMSTRUCT  lpDeleteItemStruct );
  afx_msg int  VKeyToItem(UINT nKey, UINT nIndex);
  afx_msg int  CharToItem(UINT nKey, UINT nIndex);
  afx_msg void OnSetfocus();
  afx_msg void OnKillfocus();
protected:
  DECLARE_MESSAGE_MAP()
};
