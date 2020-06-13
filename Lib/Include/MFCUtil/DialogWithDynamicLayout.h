#pragma once

#include <afxlayout.h>
#include "WinTools.h"

class ItemLayout {
public:
  int    m_ctrlId;
  CWnd  *m_ctrl;
  CMFCDynamicLayout::MoveSettings m_moveSettings;
  CMFCDynamicLayout::SizeSettings m_sizeSettings;
  inline ItemLayout() : m_ctrlId(0), m_ctrl(NULL) {
  }
  inline const CMFCDynamicLayout::MoveSettings &getMoveSettings() const {
    return m_moveSettings;
  }
  inline const CMFCDynamicLayout::SizeSettings &getSizeSettings() const {
    return m_sizeSettings;
  }
};

class DynLayoutData : public CompactArray<ItemLayout> {
private:
  CDialog &m_dlg;
  int      m_resId;
  CWnd *GetDlgItem(int ctrlId) const {
    return m_dlg.GetDlgItem(ctrlId);
  }
public:
  inline DynLayoutData(CDialog *dlg, int resId=0) : m_dlg(*dlg), m_resId(resId) {
    if(resId) {
      loadFromResource(resId);
    }
  }
  void loadFromResource(int resId);
  void reloadFromResource() {
    if(m_resId) {
      loadFromResource(m_resId);
    }
  }
  void updateWindowPointers();
  ItemLayout *findItem(int ctrlId);
};

class CDialogWithDynamicLayout : public CDialog {
private:
  DynLayoutData m_layoutData;
protected:
  CDialogWithDynamicLayout(int resId, CWnd *pParent) : CDialog(resId, pParent), m_layoutData(this) {
  }
  BOOL OnInitDialog();
  CMFCDynamicLayout &resetMFCLayoutManager();
  // Reload MFCLayoutManager and m_layoutData
  CDialogWithDynamicLayout &reloadDynamicLayoutResource();
  // Delete and recreate MFCLayoutManager and add all items in m_layoutData
  CDialogWithDynamicLayout &putLayoutDataToMFCLayoutManager();
  CDialogWithDynamicLayout &setCtrlPos( int ctrlId, CPoint &newPos , const CMFCDynamicLayout::MoveSettings *newSettings  = NULL);
  CDialogWithDynamicLayout &setCtrlSize(int ctrlId, CSize  &newSize, const CMFCDynamicLayout::SizeSettings *newSettings  = NULL);
  CDialogWithDynamicLayout &setCtrlRect(int ctrlId, CRect  &newRect, const CMFCDynamicLayout::MoveSettings *newMSettings = NULL, const CMFCDynamicLayout::SizeSettings *newSSettings = NULL);
};
