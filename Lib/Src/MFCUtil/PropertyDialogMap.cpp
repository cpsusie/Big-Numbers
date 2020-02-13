#include "pch.h"
#include <MFCUtil/PropertyDialogMap.h>

void PropertyDialogMap::addDialog(PropertyDialog *dlg) {
  m_gate.wait();
  const int id = dlg->getPropertyId();
  if(get(id)) {
    m_gate.notify();
    throwInvalidArgumentException(__TFUNCTION__, _T("PropertyDialog with id=%d already added"), id);
  }
  put(id, CPropertyDlgThread::startThread(dlg));
  m_containerSet.add((*get(id))->getPropertyContainer());
  m_gate.notify();
}

void PropertyDialogMap::removeDialog(int id) {
  m_gate.wait();
  CPropertyDlgThread **thr = get(id);
  if(thr) {
    if(*thr == m_visibleDlgThread) {
      m_visibleDlgThread = NULL;
    }
    m_containerSet.remove((*thr)->getPropertyContainer());
    (*thr)->kill();
    remove(id);
  }
  m_gate.notify();
}

void PropertyDialogMap::clear() {
  m_gate.wait();
  m_visibleDlgThread = NULL;
  for(Iterator<Entry<int, CPropertyDlgThread*> > it = entrySet().getIterator(); it.hasNext(); ) {
    Entry<int, CPropertyDlgThread*> &e = it.next();
    e.getValue()->kill();
  }
  m_containerSet.clear();
  __super::clear();
  m_gate.notify();
}

bool PropertyDialogMap::isDialogVisible() const {
  m_gate.wait();
  const bool result = isDialogVisible1();
  m_gate.notify();
  return result;
}

int PropertyDialogMap::getVisibleDialogId() const {
  m_gate.wait();
  const int result = getCurrentVisibleDialogId();
  m_gate.notify();
  return result;
}

void PropertyDialogMap::hideDialog() const {
  m_gate.wait();
  hideCurrentVisibleDialog();
  m_gate.notify();
}

void PropertyDialogMap::showDialog(int id, const void *data, size_t size) const {
  m_gate.wait();
  if(getCurrentVisibleDialogId() != id) {
    CPropertyDlgThread * const *thr = get(id);
    if(thr) {
      hideCurrentVisibleDialog();
      m_visibleDlgThread = *thr;
      m_visibleDlgThread->setDialogVisible(true);
    }
  }
  if(data && m_visibleDlgThread) {
    m_visibleDlgThread->setCurrentDialogProperty(data, size);
  }
  m_gate.notify();
}

void PropertyDialogMap::hideCurrentVisibleDialog() const {
  if(m_visibleDlgThread) {
    m_visibleDlgThread->setDialogVisible(false);
    m_visibleDlgThread = NULL;
  }
}

bool PropertyDialogMap::hasPropertyContainer(const PropertyContainer *pc) const {
  return m_containerSet.contains(pc);
}

const void *PropertyDialogMap::getProperty(int id, size_t size) const {
  const void *result = NULL;
  m_gate.wait();
  CPropertyDlgThread * const *thr = get(id);
  if(thr) {
    result = (*thr)->getCurrentDialogProperty(size);
  }
  m_gate.notify();
  return result;
}
