#include "pch.h"
#include <PropertyChangeListener.h>

void PropertyContainer::notifyPropertyChanged(int id, const void *oldValue, const void *newValue) const {
  for(size_t i = 0; i < m_listeners.size(); i++) {
    m_listeners[i]->handlePropertyChanged(this, id, oldValue, newValue);
  }
}

void PropertyContainer::addPropertyChangeListener(PropertyChangeListener *listener, bool first) {
  if(first) {
    m_listeners.add(0, listener);
  } else {
    m_listeners.add(listener);
  }
}

void PropertyContainer::removePropertyChangeListener(PropertyChangeListener *listener) {
  const intptr_t index = m_listeners.getFirstIndex(listener);
  if(index >= 0) {
    m_listeners.remove(index);
  }
}
