#include "pch.h"
#include <MFCUtil/PropertyDialog.h>
#include <MFCUtil/PropertyDialogThread.h>
#include <MFCUtil/PropertyDialogMap.h>

PropertyDialogMap &PropertyDialogMap::addDialog(PropertyDialog *dlg) {
  m_gate.wait();
  const UINT oldSize = (UINT)size();
  UINT newSize;
  try {
    const int id = dlg->getPropertyId();
    if(get(id)) {
      throwInvalidArgumentException(__TFUNCTION__, _T("PropertyDialog with id=%d already added"), id);
    }
    dlg->addPropertyChangeListener(this);
    put(id, CPropertyDialogThread::startThread(dlg));
    newSize = (UINT)size();
    m_gate.notify();
  } catch(...) {
    m_gate.notify();
    throw;
  }
  notifyPropertyChanged(PDM_DIALOG_COUNT, &oldSize, &newSize);
  return *this;
}

PropertyDialogMap &PropertyDialogMap::removeDialog(int id) {
  m_gate.wait();
  UINT oldSize = (UINT)size();
  UINT newSize;
  try {
    CPropertyDialogThread **thr = get(id);
    if(thr) {
      (*thr)->kill();
      remove(id);
    }
    newSize = (UINT)size();
    m_gate.notify();
  } catch (...) {
    m_gate.notify();
    throw;
  }
  setProperty(PDM_DIALOG_COUNT, oldSize, newSize);
  return *this;
}

PropertyDialogMap &PropertyDialogMap::removeAllDialogs() {
  m_gate.wait();
  UINT oldSize = (UINT)size();
  UINT newSize;
  try {
    for (Iterator<Entry<int, CPropertyDialogThread*> > it = entrySet().getIterator(); it.hasNext(); ) {
      Entry<int, CPropertyDialogThread*> &e = it.next();
      e.getValue()->kill();
    }
    IntHashMap<CPropertyDialogThread*>::clear();
    newSize = (UINT)size();
    m_gate.notify();
  } catch (...) {
    m_gate.notify();
    throw;
  }
  setProperty(PDM_DIALOG_COUNT, oldSize, newSize);
  return *this;
}

bool PropertyDialogMap::isDialogVisible() const {
  m_gate.wait();
  const bool result = isDialogVisible1();
  m_gate.notify();
  return result;
}

PropertyDialogMap &PropertyDialogMap::hideVisibleDialog() {
  if(isDialogVisible1()) {
    m_visibleDialogThread->setDialogVisible(false);
  }
  return *this;
}

String PropertyDialogMap::getVisibleTypeName() const {
  String result;
  m_gate.wait();
  if(isDialogVisible1()) {
    result = m_visibleDialogThread->getPropertyTypeName();
  }
  m_gate.notify();
  return result;
}

PropertyDialogMap &PropertyDialogMap::hideDialog() {
  m_gate.wait();
  try {
    hideVisibleDialog();
    m_gate.notify();
  } catch (...) {
    m_gate.notify();
    throw;
  }
  return *this;
}

bool PropertyDialogMap::showDialog(int id, const void *data, size_t size) {
  m_gate.wait();
  bool changed = false;
  try {
    CPropertyDialogThread * const * thr = get(id);
    if((getVisiblePropertyId() != id) && thr) {
      hideVisibleDialog();
      (*thr)->setDialogVisible(true);
      changed = true;
    }
    if(thr && data) {
      changed |= (*thr)->setCurrentDialogProperty(data, size);
    }
    m_gate.notify();
  } catch (...) {
    m_gate.notify();
    throw;
  }
  return changed;
}

// no lock-protection
bool PropertyDialogMap::isDialogVisible1() const {
  return m_visibleDialogThread && m_visibleDialogThread->isDialogVisible();
}

const void *PropertyDialogMap::getProperty(int id, size_t size) const {
  m_gate.wait();
  try {
    const void *result = NULL;
    CPropertyDialogThread * const *thr = get(id);
    if(thr) {
      result = (*thr)->getCurrentDialogProperty(size);
    }
    m_gate.notify();
    return result;
  } catch(...) {
    m_gate.notify();
    throw;
  }
}

void PropertyDialogMap::handlePropertyChanged(const PropertyContainer *source, int id, const void *oldValue, const void *newValue) {
  PropertyDialog *dlg = (PropertyDialog*)source;
  switch(dlg->getPropertyIdOffset(id)) {
  case 0:
    notifyPropertyChanged(PDM_CURRENT_DLGVALUE, oldValue, newValue);
    break;
  case PROPDLG_VISIBLE_OFFSET:
    { const bool newVisible = *(bool*)newValue;
      if(!newVisible) {
        setVisibleDialogThread(NULL);
      } else {
        setVisibleDialogThread(*get(dlg->getPropertyId()));
      }
    }
    break;
  default:
    showError(_T("%s:Unknown propertyId:%d"), __TFUNCTION__, id);
    break;
  }
}

void PropertyDialogMap::setVisibleDialogThread(CPropertyDialogThread *newValue) {
  m_lock1.wait();
  m_visibleDialogThread = newValue;
  const int newVisibleId = newValue ? newValue->getPropertyId() : -1;
  setProperty(PDM_VISIBLE_PROPERTYID, m_visiblePropertyId, newVisibleId);
  m_lock1.notify();
}
