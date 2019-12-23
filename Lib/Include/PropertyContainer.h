#pragma once

#include <CompactArray.h>

class PropertyContainer;

class PropertyChangeListener {
public:
  virtual void handlePropertyChanged(const PropertyContainer *source, int id, const void *oldValue, const void *newValue) = 0;
};

class PropertyContainer {
private:
  CompactArray<PropertyChangeListener*> m_listeners;
protected:
  void notifyPropertyChanged(int id, const void *oldValue, const void *newValue) const;
  template<typename T> void setProperty(int id, T &v, const T &newValue) {
    if(newValue != v) {
      const T oldValue = v;
      v = newValue;
      notifyPropertyChanged(id, &oldValue, &v);
    }
  }
  void setProperty(int id, String &v, const TCHAR *newValue) {
    setProperty<String>(id, v, String(newValue));
  }
public:
  virtual ~PropertyContainer() {
    clear();
  }
  inline void addPropertyChangeListener(PropertyChangeListener *listener) {
    m_listeners.add(listener);
  }
  void removePropertyChangeListener(PropertyChangeListener *listener);
  inline void clear() {
    m_listeners.clear();
  }
};
