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
  bool                                  m_notifyEnable;
  void alwaysNotifyPropertyChanged(int id, const void *oldValue, const void *newValue) const;
protected:
  // Check if notifications of propertychanges is enabled, and if so, notifies all listeners
  void notifyPropertyChanged(int id, const void *oldValue, const void *newValue) const;

  // Set v=newValue, and if newValue != v and notifications of propertychanges is enabled, notifies all listeners
  template<typename T> void setProperty(int id, T &v, const T &newValue) {
    if(!m_notifyEnable) {
      v = newValue;
    } else if(newValue != v) {
      const T oldValue = v;
      v = newValue;
      alwaysNotifyPropertyChanged(id, &oldValue, &v);
    }
  }
  void setProperty(int id, String &v, const TCHAR *newValue) {
    setProperty<String>(id, v, String(newValue));
  }
  bool setNotifyEnable(bool enable);
  inline bool getNotifyEnable() const {
    return m_notifyEnable;
  }
public:
  inline PropertyContainer() : m_notifyEnable(true) {
  }
  virtual ~PropertyContainer() {
    clear();
  }
  inline bool hasListener(PropertyChangeListener * listener) const {
    return m_listeners.getFirstIndex(listener) >= 0;
  }
  inline void addPropertyChangeListener(PropertyChangeListener *listener) {
    if(!hasListener(listener)) {
      m_listeners.add(listener);
    }
  }
  void removePropertyChangeListener(PropertyChangeListener * const listener);
  inline void clear() {
    m_listeners.clear();
  }
};
