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
  bool m_notifyEnable;
  void alwaysNotifyPropertyChanged(int id, const void *oldValue, const void *newValue) const;
protected:
  // Check if notifications of propertychanges is enabled, and if so, notifies all listeners
  inline void notifyPropertyChanged(int id, const void *oldValue, const void *newValue) const {
    if(m_notifyEnable) {
      alwaysNotifyPropertyChanged(id, &oldValue, &newValue);
    }
  }

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
  inline bool setNotifyEnable(bool enable) {
    const bool old = m_notifyEnable; m_notifyEnable = enable; return old;
  }
  inline bool getNotifyEnable() const {
    return m_notifyEnable;
  }
public:
  inline PropertyContainer() : m_notifyEnable(true) {
  }
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
