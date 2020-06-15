#include "pch.h"
#include <MFCUtil/WinTools.h>
#include <MFCUtil/DialogWithDynamicLayout.h>

void DynLayoutData::loadFromResource(int resId) {
  try {
    if(m_resId && (resId != m_resId)) {
      throwInvalidArgumentException(__TFUNCTION__, _T("resId=%d, differs from previous resId=%d"), resId, m_resId);
    }
    clear(-1);
    ByteArray ba;
    ba.loadFromResource(resId, RT_DIALOG_LAYOUT);
    if(ba.size() > 2) {
      const UINT        shortCount = (UINT)ba.size() / 2 - 1;
      CompactShortArray sa(shortCount);
      for(const BYTE *b0 = ba.getData(), *bp = b0 + 2, *endp = b0 + ba.size(); bp < endp; bp += 2) {
        sa.add(*(short*)bp);
      }
      if(shortCount % 4 != 0) {
        throwException(_T("Invalid size(=%u) of shortarray for dialog(resId=%d)"), shortCount, resId);
      }
      const int itemCount = shortCount / 4;
      setCapacity(itemCount);
      ChildWindowArray childIdArray(m_dlg);
      for(size_t i = 0; i < sa.size();) {
        const UINT childIndex = (UINT)size();
        ItemLayout l;
        l.m_moveSettings.m_nXRatio = sa[i++]; l.m_moveSettings.m_nYRatio = sa[i++];
        l.m_sizeSettings.m_nXRatio = sa[i++]; l.m_sizeSettings.m_nYRatio = sa[i++];
        if(childIndex < childIdArray.size()) {
          l.m_ctrlId = childIdArray[childIndex];
          l.m_ctrl   = m_dlg.GetDlgItem(l.m_ctrlId);
        }
        add(l);
      }
    }
    m_resId = resId;
  } catch(Exception e) {
    showWarning(_T("%s"), e.what());
  }
}

void DynLayoutData::updateWindowPointers() {
  for(ItemLayout il : *this) {
    il.m_ctrl = GetDlgItem(il.m_ctrlId);
  }
}

ItemLayout *DynLayoutData::findItem(int ctrlId) {
  if(isEmpty()) return NULL;
  for(ItemLayout *pil = &first(), *endp = &last(); pil<=endp; pil++) {
    if(pil->m_ctrlId == ctrlId) {
      return pil;
    }
  }
  return NULL;
}

const ItemLayout *DynLayoutData::findItem(int ctrlId) const {
  if(isEmpty()) return NULL;
  for(const ItemLayout *pil = &first(), *endp = &last(); pil<=endp; pil++) {
    if(pil->m_ctrlId == ctrlId) {
      return pil;
    }
  }
  return NULL;
}

BOOL CDialogWithDynamicLayout::OnInitDialog() {
  const BOOL result = __super::OnInitDialog();
  if(IS_INTRESOURCE(m_lpszTemplateName)) {
    const USHORT dlgId = (USHORT)((intptr_t)m_lpszTemplateName & 0xffff);
    m_layoutData.loadFromResource(dlgId);
  }
  return result;
}

CMFCDynamicLayout &CDialogWithDynamicLayout::resetMFCLayoutManager() {
  EnableDynamicLayout(FALSE);
  EnableDynamicLayout(TRUE);
  CMFCDynamicLayout *mfcLM = GetDynamicLayout();
  mfcLM->Create(this);
  return *mfcLM;
}

CDialogWithDynamicLayout &CDialogWithDynamicLayout::reloadDynamicLayoutResource() {
  resetMFCLayoutManager();
  LoadDynamicLayoutResource(m_lpszTemplateName);
  m_layoutData.reloadFromResource();
  return *this;
}

CDialogWithDynamicLayout &CDialogWithDynamicLayout::putLayoutDataToMFCLayoutManager() {
  CMFCDynamicLayout &mfcLM = resetMFCLayoutManager();
  for(ItemLayout il : m_layoutData) {
    if(il.m_ctrl) {
      mfcLM.AddItem(*il.m_ctrl, il.getMoveSettings(), il.getSizeSettings());
    }
  }
  return *this;
}

CDialogWithDynamicLayout &CDialogWithDynamicLayout::setCtrlPos(int ctrlId, CPoint &newPos, const CMFCDynamicLayout::MoveSettings *newSettings) {
  setWindowPosition(this, ctrlId, newPos);
  if(newSettings) {
    ItemLayout *li = m_layoutData.findItem(ctrlId);
    if(li) {
      li->m_moveSettings = *newSettings;
    }
  }
  return putLayoutDataToMFCLayoutManager();
}

CDialogWithDynamicLayout &CDialogWithDynamicLayout::setCtrlSize(int ctrlId, CSize  &newSize, const CMFCDynamicLayout::SizeSettings *newSettings) {
  setWindowSize(this, ctrlId, newSize);
  if(newSettings) {
    ItemLayout *li = m_layoutData.findItem(ctrlId);
    if(li) {
      li->m_sizeSettings = *newSettings;
    }
  }
  return putLayoutDataToMFCLayoutManager();
}

CDialogWithDynamicLayout &CDialogWithDynamicLayout::setCtrlRect(int ctrlId, CRect  &newRect, const CMFCDynamicLayout::MoveSettings *newMSettings, const CMFCDynamicLayout::SizeSettings *newSSettings) {
  setWindowRect(this, ctrlId, newRect);
  if(newMSettings || newSSettings) {
    ItemLayout *li = m_layoutData.findItem(ctrlId);
    if(li != NULL) {
      if(newMSettings) {
        li->m_moveSettings = *newMSettings;
      }
      if(newSSettings) {
        li->m_sizeSettings = *newSSettings;
      }
    }
  }
  return putLayoutDataToMFCLayoutManager();
}
