#include "stdafx.h"
#include <MyUtil.h>
#include <MFCUtil/WinTools.h>

#pragma warning(disable : 4244)

ListCtrlLayoutAttribute::ListCtrlLayoutAttribute(SimpleLayoutManager *manager, int ctrlId, int flags)
: LayoutAttribute(manager, ctrlId, flags)
, m_ctrlHeaderInfo((CListCtrl*)manager->getChild(ctrlId))
{
  m_columnWeightArray = m_ctrlHeaderInfo.getColumnWeightArray();
}

void ListCtrlLayoutAttribute::resetStartRect() {
  if(getFlags() & RESIZE_LISTHEADERS) {
    setControlRect(getControlRect());
  }
  LayoutAttribute::resetStartRect();
}

void ListCtrlLayoutAttribute::setControlRect(const CRect &r) {
  const CompactIntArray    oldWidthArray     = m_ctrlHeaderInfo;
  const ListCtrlHeaderInfo currentHeaderInfo = getCurrentCtrlHeaderInfo();
  const CompactIntArray    currentWidthArray = currentHeaderInfo;
  const bool               winChanged        = isControlWindowChanged();

  if(currentWidthArray != oldWidthArray) { // user has changed the columns with since last resize
    m_columnWeightArray = currentHeaderInfo.getColumnWeightArray();
  } else if(winChanged) {
    m_ctrlHeaderInfo = currentHeaderInfo;
    m_ctrlHeaderInfo.adjustColumnWidthArray(m_columnWeightArray);
    m_columnWeightArray = m_ctrlHeaderInfo.getColumnWeightArray();
  }

  CListCtrl *ctrl = getListControl();
  if(!winChanged) {
    LayoutAttribute::setControlRect(r);
  }

  const int                oldClientWidth = m_columnWeightArray.getClientWidth();
  const ListCtrlHeaderInfo newHeaderInfo  = getCurrentCtrlHeaderInfo();
  const int                newClientWidth = newHeaderInfo.getClientWidth();
 
  if((winChanged || (newClientWidth != oldClientWidth)) && (oldClientWidth && newClientWidth)) {
    const double newSum = (double)m_columnWeightArray.getColumnWidthSum() * newClientWidth / oldClientWidth;
    const int    n      = m_columnWeightArray.size();
    for(int i = 0; i < n; i++) {
      const int cw = newSum * m_columnWeightArray[i];
      ctrl->SetColumnWidth(i, cw);
    }

    m_ctrlHeaderInfo = ListCtrlHeaderInfo(ctrl);
  }
}

ListCtrlHeaderInfo::ListCtrlHeaderInfo(const CListCtrl *ctrl) : ClientRectWidthColumnWidthSum(0,0) {
  m_columnWidthSum = 0;
  const int n = ((CListCtrl*)ctrl)->GetHeaderCtrl()->GetItemCount();
  for(int i = 0; i < n; i++) {
    const int cw = ctrl->GetColumnWidth(i);
    add(cw);
    m_columnWidthSum += cw;
  }
  m_clientRectWidth = getClientRect(ctrl).Width();
}

void ListCtrlHeaderInfo::adjustColumnWidthArray(const ColumnWeightArray &wa) {
  clear();
  const double newSum = wa.getColumnWidthSum() * m_clientRectWidth / wa.getClientWidth();
  m_columnWidthSum = 0;
  for(size_t i = 0; i < wa.size(); i++) {
    const int cw = newSum * wa[i];
    add(cw);
    m_columnWidthSum += cw;
  }
}

ColumnWeightArray ListCtrlHeaderInfo::getColumnWeightArray() const {
  ColumnWeightArray result(*this);
  if(m_columnWidthSum == 0) {
    for(size_t i = 0; i < size(); i++) {
      result.add(0);
    }
  } else {
    for(size_t i = 0; i < size(); i++) {
      result.add((float)((*this)[i]) / m_columnWidthSum);
    }
  }
  return result;
}
