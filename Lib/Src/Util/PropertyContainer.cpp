#include "pch.h"
#include <PropertyContainer.h>

void PropertyContainer::alwaysNotifyPropertyChanged(int id, const void *oldValue, const void *newValue) const {
  for(size_t i = 0; i < m_listeners.size(); i++) {
    m_listeners[i]->handlePropertyChanged(this, id, oldValue, newValue);
  }
}

void PropertyContainer::notifyPropertyChanged(int id, const void *oldValue, const void *newValue) const {
  if(m_notifyEnable) {
    alwaysNotifyPropertyChanged(id, oldValue, newValue);
  }
}

void PropertyContainer::removePropertyChangeListener(PropertyChangeListener * const listener) {
  const intptr_t index = m_listeners.getFirstIndex(listener);
  if(index >= 0) {
    m_listeners.remove(index);
  }
}

bool PropertyContainer::setNotifyEnable(bool enable) {
  const bool old = m_notifyEnable;
  m_notifyEnable = enable;
  return old;
}
