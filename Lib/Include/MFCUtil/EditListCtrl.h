#pragma once

#include <MyString.h>
#include <NumberInterval.h>
#include <TinyBitSet.h>

typedef enum {
  LFT_SHORT       = 1
 ,LFT_INT
 ,LFT_FLOAT
 ,LFT_DOUBLE
 ,LFT_STRING
 ,LFT_STRCOMBO
 ,LFT_BOOL
} ListFieldType;

#define LF_TYPEMASK                   0x00ff
#define LF_GETTYPE(                   flags) ((ListFieldType)((flags)&LF_TYPEMASK))

#define LF_NUM_HAS_LOWERLIMIT         0x00000100
#define LF_NUM_HAS_UPPERLIMIT         0x00000200
#define LF_NUM_LOWERLIMIT_EXCLUSIVE   0x00000400
#define LF_NUM_UPPERLIMIT_EXCLUSIVE   0x00000800
#define LF_NUM_SHOW_ZERO              0x00001000
#define LF_NUM_DECIMALS(dec)          (((UINT)(dec)&0x1f)<<24)

#define LF_ISINTEGERTYPE(type) (((type) == LFT_SHORT) || ((type) == LFT_INT))

#define LF_NUM_GETDECIMALCOUNT(flags) ((UINT)(((flags)>>24)&0x1f))
#define LF_NUM_HASINTERVAL(    flags) ((flags)&(LF_NUM_HAS_LOWERLIMIT|LF_NUM_HAS_UPPERLIMIT))

#define LF_STRCOMBO_DROPDOWN          0x00000100 /* can enter new text Value frmo model must be String */
#define LF_STRCOMBO_DROPLIST          0x00000200 /*                    Value from model must be int    */

class CTableModel {
private:
  CompactArray<CWnd*> m_editorArray;
protected:
  void saveEditor(UINT column, CWnd *editor);
  bool hasCachedEditor(UINT column) {
    return getCachedEditor(column) != NULL;
  }
  CWnd *getCachedEditor(UINT column);
  String legalIntervalToString() const;
  void outOfRangeException(double v) const;
public:
  virtual ~CTableModel();
  virtual UINT              getRowCount()                                                      = 0;
  virtual UINT              getColumnCount()                                                   = 0;
  virtual String            getColumnName(   UINT column)                                      = 0;
  virtual UINT              getColumnWidth(  UINT column)                                      = 0;
  virtual UINT              getFieldFlags(   UINT column)                                      = 0;
  virtual void             *getValueAt(      UINT row, UINT column)                            = 0;
  virtual bool              isEditableCell(  UINT row, UINT column)                            = 0;
  virtual DoubleInterval    getLegalInterval(UINT column) {
    return getMaxInterval(getListFieldType(column));
  }
  virtual const StringArray getStrComboStringArray(UINT column) {
    return StringArray();
  }

  virtual CWnd          *createEditor(CWnd *listCtrl, UINT column);
  CWnd                  *getEditor(   CWnd *listCtrl, UINT column);
  inline ListFieldType   getListFieldType(UINT column) {
    return LF_GETTYPE(getFieldFlags(column));
  }
  static bool           isNumericType(ListFieldType type);
  static String         getListFieldTypeName(ListFieldType type);
  static DoubleInterval getMaxInterval(      ListFieldType type);
};

#define EDITLIST_CONTROL_ID_COLUMN0 100
#define MAKE_EDITLIST_CONTROL_ID(column) (EDITLIST_CONTROL_ID_COLUMN0 + (column))

class ColumnOrderArray : public CompactIntArray {
public:
  ColumnOrderArray(CListCtrl *listCtrl);
  int findColumnIndex(UINT column) const;
};

class EditListCtrlFlags : public BitSet8 {
public:
  String toString() const;
};

typedef CPoint ListCell;

class CEditListCtrl : public CListCtrl {
private:
  friend LRESULT CALLBACK privateHeaderCtrlWinProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
  CTableModel      *m_model;
  ListCell          m_currentCell;
  CWnd             *m_currentEditor;
  CRect             m_lastCellRect;
  CPoint            m_lastMouseClick; // relative to clientRect
  WNDPROC           m_headerCtrlWinProc;
  EditListCtrlFlags m_controlFlags;
  int               m_itemHeight; // in pixels
  int               m_sortColumn;
  bool              m_ascending;
  CBitmap           m_ascBitmap, m_descBitmap;

  void     insertRow(UINT row);
  void     removeRow(UINT row);
  void     setCurrentCell(int row, int column);
  void     beginEdit();
  bool     endEdit();
  bool     commitEdit();
  void     cancelEdit();
  void     hideEditCtrl();
  void     setItem(LVITEM &lv, bool newItem);
  void     modelValueToEditor(     const ListCell &cell);
  void     editorValueToModel(     const ListCell &cell);
  void     modelValueToList(       const ListCell &cell, bool newItem);
  double   getNumericValueAt(      const ListCell &cell);
  String   getModelComboStringAt(  const ListCell &cell);
  void     modelValueToComboEditor(const ListCell &cell);
  void     comboEditorValueToModel(const ListCell &cell);
  void     clearItemRect(HDC hdc, const CRect &r);
  int      findRowFromPoint( const CPoint &p) const;
  int      findColumnFromPoint(int itemIndex, const CPoint &p);
  ListCell findCellFromPoint(const CPoint &p);
  int      getItemHeight() const; // in pixels
  void     createBitmaps();
  void     paintHeaderSortMark();

  inline CWnd *getEditor(    const ListCell &cell) {
    return m_model->getEditor(this, cell.x);
  }

  BOOL PreTranslateMessage1(MSG *pMsg);

  inline bool hasCurrentEditor() const {
    return m_currentEditor != NULL;
  }
  inline bool isEditorVisible() const {
    return hasCurrentEditor() && (m_currentEditor->IsWindowVisible());
  }
  inline bool isCurrentCellEditable() const {
    return hasCurrentCell() ? isCellEditable(m_currentCell) : false;
  }
  inline bool isCellEditable(const ListCell &cell) const {
    return m_model->isEditableCell(cell.y, cell.x);
  }
  inline ListFieldType getListFieldType(const ListCell &cell) const {
    return m_model->getListFieldType(cell.x);
  }

  CRect  getEditorRect(      const ListCell &cell) const;
  CRect  getTextRect(        const ListCell &cell) const;
  CSize  getTextSize(        const ListCell &cell) const ;
  CRect  getCellRect(        const ListCell &cell) const;
  CRect  getComboBoxWinRect( const ListCell &cell) const;
  CRect  getComboBoxTextRect(const ListCell &cell) const;
  CRect  getCheckBoxWinRect( const ListCell &cell) const;
  CPoint getCheckBoxPosition(const ListCell &cell) const;

  bool   isChildWindow(CWnd *wnd);
  void   trace(const TCHAR *format,...);
  String infoToString() const;
  String controlFlagsToString() const;
public:
    CEditListCtrl();
    void substituteControl(CWnd *wnd, int id, CTableModel &model);
    void insertNewItem(int  index = -1); // if index == -1 then append item at the end
    void removeItem(   UINT index);

    bool lastMessageWasMouseClick() const;
    const CPoint &getLastMouseClickPoint() const {
      return m_lastMouseClick;
    }
    void setSortColumn(int column, bool asc);
    inline int  getSortColumn() const {
      return m_sortColumn;
    }
    inline bool isSortAscending() const {
      return m_ascending;
    }
    inline int getRowCount() {
      return GetItemCount();
    }
    inline int getColumnCount() {
      return GetHeaderCtrl()->GetItemCount();
    }
    inline bool hasCurrentCell() const {
      return (m_currentCell.x >= 0) && (m_currentCell.y >= 0);
    }
    inline int getCurrentRow() const {
      return m_currentCell.y;
    }
    inline int getCurrentCol() const {
      return m_currentCell.x;
    }
    inline const ListCell &getCurrentCell() const {
      return m_currentCell;
    }
    inline CSize getCurrentTextSize()   const {
      return hasCurrentCell() ? getTextSize(getCurrentCell()  ) : CSize(0,0);
    }
    inline CRect getCurrentCellRect()   const {
      return hasCurrentCell() ? getCellRect(getCurrentCell()  ) : CRect(0,0,0,0);
    }
    inline CRect getCurrentTextRect()   const {
      return hasCurrentCell() ? getTextRect(getCurrentCell()  ) : CRect(0,0,0,0);
    }
    inline CRect getCurrentEditorRect() const {
      return hasCurrentCell() ? getEditorRect(getCurrentCell()) : CRect(0,0,0,0);
    }

public:
    virtual BOOL PreTranslateMessage(MSG *pMsg);
protected:
  virtual void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);

public:
  virtual ~CEditListCtrl();

protected:
  afx_msg void OnPaint();
  afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
  afx_msg void OnSetFocus( CWnd *pOldWnd);
  afx_msg void OnKillFocus(CWnd *pNewWnd);
  afx_msg void OnSize(UINT nType, int cx, int cy);
  afx_msg void OnItemchanged(NMHDR *pNMHDR, LRESULT *pResult);
  afx_msg void OnColumnclick(NMHDR *pNMHDR, LRESULT *pResult);
  afx_msg void OnEditSetFocus( UINT id);
  afx_msg void OnEditKillFocus(UINT id);

  afx_msg void MeasureItem ( LPMEASUREITEMSTRUCT lpMeasureItemStruct );

  DECLARE_MESSAGE_MAP()
};
