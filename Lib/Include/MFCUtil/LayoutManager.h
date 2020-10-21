#pragma once

#include <CompactArray.h>
#include "WinTools.h"

class ClientRectWidthColumnWidthSum {
protected:
  int m_clientRectWidth;
  int m_columnWidthSum;

  ClientRectWidthColumnWidthSum(int clientRectWidth, int columnWidthSum)
    : m_clientRectWidth(clientRectWidth)
    , m_columnWidthSum(columnWidthSum)
  {
  }

public:
  inline int getClientWidth() const {
    return m_clientRectWidth;
  }
  inline int getColumnWidthSum() const {
    return m_columnWidthSum;
  }
  bool operator==(const ClientRectWidthColumnWidthSum &ccs) const {
    return m_clientRectWidth == ccs.m_clientRectWidth
        && m_columnWidthSum  == ccs.m_columnWidthSum;
  }
  bool operator!=(const ClientRectWidthColumnWidthSum &ccs) const {
    return !(*this == ccs);
  }
};

class ColumnWeightArray : public ClientRectWidthColumnWidthSum, public CompactFloatArray {
public:
  ColumnWeightArray() : ClientRectWidthColumnWidthSum(0,0) {
  }
  ColumnWeightArray(const ClientRectWidthColumnWidthSum &ccs) : ClientRectWidthColumnWidthSum(ccs) {
  }
};

class ListCtrlHeaderInfo : public ClientRectWidthColumnWidthSum, public CompactIntArray {
public:
  ListCtrlHeaderInfo(const CListCtrl *ctrl);
  ColumnWeightArray getColumnWeightArray() const;
  void adjustColumnWidthArray(const ColumnWeightArray &wa);

  inline bool operator==(const ListCtrlHeaderInfo &info) const {
    return (((ClientRectWidthColumnWidthSum&)*this) == info) && (((CompactIntArray&)*this) == info);
  }
  inline bool operator!=(const ListCtrlHeaderInfo &info) const {
    return !(*this == info);
  }
};

class SimpleLayoutManager;

class LayoutAttribute {
private:
  SimpleLayoutManager &m_manager;
  const int            m_ctrlId;
  const int            m_flags;
  CRect                m_startRect; // orignal windowRect relative to parent
  CRect                m_lastRect;  // current windowRect relative to parent
  double               m_currentFontScale;
  CFont               *m_startFont;
  CFont               *m_font;

  void setFont(CFont *font);
protected:
  inline SimpleLayoutManager &getManager() {
    return m_manager;
  }
  CWnd       *getControl();
  const CWnd *getControl() const;
  CRect       getControlRect() const;
public:

  LayoutAttribute(SimpleLayoutManager *manager, int ctrlId, int flags);
  virtual ~LayoutAttribute();

  inline int getCtrlId() const {
    return m_ctrlId;
  }
  inline int getFlags() const {
    return m_flags;
  }
  void   setFontScale(double scale);
  inline double getCurrentFontScale() const {
    return m_currentFontScale;
  }
  inline CFont *getFont() {
    return m_font;
  }
  bool         isControlWindowChanged() const;
  inline const CRect &getStartRect() const {
    return m_startRect;
  }
  virtual void resetStartRect();
  virtual void setControlRect(const CRect &r);
};

class ListCtrlLayoutAttribute : public LayoutAttribute {
private:
  ListCtrlHeaderInfo  m_ctrlHeaderInfo;
  ColumnWeightArray   m_columnWeightArray;

  inline CListCtrl *getListControl() {
    return (CListCtrl*)getControl();
  }
  inline const CListCtrl *getListControl() const {
    return (CListCtrl*)getControl();
  }

  inline ListCtrlHeaderInfo getCurrentCtrlHeaderInfo() const {
    return ListCtrlHeaderInfo(getListControl());
  }
public:
  ListCtrlLayoutAttribute(SimpleLayoutManager *manager, int ctrlId, int flags);
  void resetStartRect();
  void setControlRect(const CRect &r);
};

#define RELATIVE_LEFT            0x00000001
#define RELATIVE_TOP             0x00000002
#define RELATIVE_RIGHT           0x00000004
#define RELATIVE_BOTTOM          0x00000008
#define PCT_RELATIVE_LEFT        0x00000010
#define PCT_RELATIVE_TOP         0x00000020
#define PCT_RELATIVE_RIGHT       0x00000040
#define PCT_RELATIVE_BOTTOM      0x00000080
#define PCT_RELATIVE_X_CENTER    0x00000100
#define PCT_RELATIVE_Y_CENTER    0x00000200
#define CONSTANT_WIDTH           0x00000400
#define CONSTANT_HEIGHT          0x00000800

#define RESIZE_LISTHEADERS       0x00001000
#define INIT_LISTHEADERS         0x00002000
#define RESIZE_FONT              0x00004000
#define FONT_RELATIVE_LEFT       0x00008000
#define FONT_RELATIVE_TOP        0x00010000
#define FONT_RELATIVE_SIZE       0x00020000
//#define RESIZE_MENUFONT        0x00040000

#define RETAIN_ASPECTRATIO       0x00080000

#define CONSTANT_SIZE          (CONSTANT_WIDTH           | CONSTANT_HEIGHT         )
#define RELATIVE_X_POS         (RELATIVE_LEFT            | CONSTANT_WIDTH          )
#define RELATIVE_Y_POS         (RELATIVE_TOP             | CONSTANT_HEIGHT         )
#define PCT_RELATIVE_X_POS     (PCT_RELATIVE_LEFT        | CONSTANT_WIDTH          )
#define PCT_RELATIVE_Y_POS     (PCT_RELATIVE_TOP         | CONSTANT_HEIGHT         )
#define RELATIVE_WIDTH          RELATIVE_RIGHT
#define RELATIVE_HEIGHT         RELATIVE_BOTTOM
#define RELATIVE_POSITION      (RELATIVE_X_POS           | RELATIVE_Y_POS          )
#define PCT_RELATIVE_POSITION  (PCT_RELATIVE_X_POS       | PCT_RELATIVE_Y_POS      )
#define FONT_RELATIVE_POSITION (FONT_RELATIVE_LEFT       | FONT_RELATIVE_TOP       )
#define RELATIVE_SIZE          (RELATIVE_WIDTH           | RELATIVE_HEIGHT         )

class LayoutManager {
private:
  CWnd  *m_wnd;
  int    m_flags;
  CSize  m_winStartSize;                                          // original size of clientRect
  CSize  m_borderSize;                                            // thickness of windows border
  CFont *m_startFont;
  CFont *m_font;
  double m_currentFontScale;
  void setFont(CFont *font);

  LayoutManager(const LayoutManager &src);                        // Not defined. Class not cloneable
  LayoutManager &operator=(const LayoutManager &src);             // Not defined. Class not cloneable
protected:
  void setFontScale(double scale, bool resizeWindow, bool redraw);
  void resetWinStartSize();
  const CSize &getWinStartSize() const {
    return m_winStartSize;
  }
  CSize findScaledClientSize(const CSize &size, bool isCorner) const;  // size = clientRect.size
  const CFont *getFont() const {
    return m_font;
  }
  int getFlags() const {
    return m_flags;
  }
  void checkIsInitialized(const TCHAR *method) const;
public:
  LayoutManager();
  virtual ~LayoutManager();
  bool isInitialized() const {
    return m_wnd != nullptr;
  }
  void OnInitDialog(CWnd *wnd, int flags = 0);                    // flags any combination of
                                                                  // RESIZE_FONT, RESIZE_MENUFONT + (FONT_RELATIVE_SIZE or RETAIN_ASPECTRATIO)
  virtual void OnSize(UINT nType, int cx, int cy) {
  };
  virtual void OnSizing(UINT fwSide, LPRECT pRect);               // should be called before __super::OnSizing

  static CFont *createScaledFont(const CFont &src, double scale); // returned font must be deleted after use

  CWnd         *getWindow();
  const CWnd   *getWindow() const;
  CWnd         *getChild(int ctrlId);
  const CWnd   *getChild(int ctrlId) const;
  inline double getCurrentFontScale() const {
    return m_currentFontScale;
  }
};

class SimpleLayoutManager : public LayoutManager {
private:
  CompactArray<LayoutAttribute*> m_attributes;
  bool                           m_arrayModified;

  SimpleLayoutManager(const SimpleLayoutManager &src);            // Not defined. Class not cloneable
  SimpleLayoutManager &operator=(const SimpleLayoutManager &src); // Not defined. Class not cloneable

  void updateFontScale(LayoutAttribute &attr, double scale, bool redraw, const CSize &currentSize);
  void updateChildRect(LayoutAttribute &attr, const CSize &currentSize);
  bool isAnyChildrenChanged() const;
public:
  SimpleLayoutManager() : m_arrayModified(false) {
  }
  ~SimpleLayoutManager();
  void addControl(   int ctrlId, int flags);
  void removeControl(int ctrlId);
  void removeAll();
  void OnSize(UINT nType, int cx, int cy);
  void scaleFont(double scale, bool redraw);
};
