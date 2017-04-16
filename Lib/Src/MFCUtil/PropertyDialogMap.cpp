#include "pch.h"
#include <MFCUtil/PropertyDialogMap.h>

void PropertyDialogMap::addDialog(PropertyDialog *dlg) {
  const int id = dlg->getPropertyId();
  if(get(id)) {
    throwInvalidArgumentException(__TFUNCTION__, _T("PropertyDialog with id=%d already added"), id);
  }
  put(id, CPropertyDlgThread::startThread(dlg));
}

void PropertyDialogMap::removeDialog(int id) {
  CPropertyDlgThread **thr = get(id);
  if(thr) {
    if (*thr == m_visibleDlgThread) {
      m_visibleDlgThread = NULL;
    }
    (*thr)->kill();
    remove(id);
  }
}

void PropertyDialogMap::clear() {
  m_visibleDlgThread = NULL;
  for(Iterator<Entry<int, CPropertyDlgThread*> > it = entrySet().getIterator(); it.hasNext(); ) {
    Entry<int, CPropertyDlgThread*> &e = it.next();
    e.getValue()->kill();
  }
  HashMap<int, CPropertyDlgThread*>::clear();
}

bool PropertyDialogMap::isDialogVisible() const {
  return m_visibleDlgThread != NULL;
}

int PropertyDialogMap::getVisibleDialogId() const {
  return m_visibleDlgThread ? m_visibleDlgThread->getPropertyId() : -1;
}

void PropertyDialogMap::hideDialog() const {
  if(m_visibleDlgThread) {
    m_visibleDlgThread->setDialogVisible(false);
    m_visibleDlgThread = NULL;
  }
}

void PropertyDialogMap::showDialog(int id, const void *data) const {
  if((m_visibleDlgThread == NULL) || (m_visibleDlgThread->getPropertyId() != id)) {
    CPropertyDlgThread * const *thr = get(id);
    if(thr == NULL) return;
    hideDialog();
    m_visibleDlgThread = *thr;
    m_visibleDlgThread->setDialogVisible(true);
  }
  if(data != NULL) {
    m_visibleDlgThread->setCurrentDialogProperty(data);
  }
}

bool PropertyDialogMap::hasPropertyContainer(const PropertyContainer *pc) const {
  for(Iterator<CPropertyDlgThread*> it = ((PropertyDialogMap*)this)->values().getIterator(); it.hasNext();) {
    if (it.next()->getPropertyContainer() == pc) {
      return true;
    }
  }
  return false;
}

const void *PropertyDialogMap::getProperty(int id) const {
  CPropertyDlgThread * const *thr = get(id);
  if(thr == NULL) return NULL;
  return (*thr)->getCurrentDialogProperty();
}
