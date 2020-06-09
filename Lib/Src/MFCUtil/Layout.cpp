#include "pch.h"

/*
Layout &Layout::enableDynamicLayout(bool enable) {
  if(enable && m_dlg->IsDynamicLayoutEnabled()) {
    return *this;
  }
  m_dlg->EnableDynamicLayout(enable ? TRUE : FALSE);
  if(enable) {
    m_dlg->loLoadDynamicLayoutResource(m_dlg->m_lpszTemplateName);
  }
  return *this;
}

bool Layout::hasDynamicLayout() const {
  return IsDynamicLayoutEnabled() && ((CExprDialog*)this)->GetDynamicLayout()->HasItem(*GetDlgItem(ctrlId));
}
*/
/*
void Layout::saveLayout(HWND hwnd, int ctrlId) {
  m_tabOrder.saveTabOrder(hwnd);
  m_ctrlId    = ctrlId;
  m_useLayout = hasDynamicLayout();
}

void Layout::restoreLayout() {
  m_tabOrder.restoreTabOrder();
  if(m_useLayout) {
    enableDynamicLayout(false).enableDynamicLayout(true);
  }
}
*/