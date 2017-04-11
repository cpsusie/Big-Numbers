#pragma once

#include <PropertyContainer.h>

inline void putWindowBesideMainWindow(CWnd *wnd) {
  putWindowBesideWindow(wnd, AfxGetApp()->GetMainWnd());
}

class PropertyDialog : public CDialog, public PropertyContainer {
private:
  const int m_propertyId;
  bool      m_notifyEnabled;
  bool      m_visible;
  bool      m_showWinActive;
protected:
  PropertyDialog(int resId, int propertyId, CWnd *pParent) : CDialog(resId, pParent), m_propertyId(propertyId) {
    m_notifyEnabled = false;
    m_visible       = false;
    m_showWinActive = false; // to prevent infinte recursion
  }
  inline void setNotifyEnabled(bool enable) {
    m_notifyEnabled = enable;
  }
  inline bool isNotifyEnabled() const {
    return m_notifyEnabled;
  }
  inline void setVisible(bool visible) {
    m_visible = visible;
  }
  void OnShowWindow(BOOL bShow, UINT nStatus);
public:
  virtual ~PropertyDialog() {
  }
  inline int getPropertyId() const {
    return m_propertyId;
  }
  inline bool isVisible() const {
    return m_visible;
  }
  virtual void setStartProperty(const void *v) = 0;
  virtual void reposition() {
    putWindowBesideMainWindow(this);
  }
  virtual void resetControls() {
  };
};

template<class T> class CPropertyDialog : public PropertyDialog {
private:
  T m_startValue, m_currentValue, m_prevValue;
protected:
  CPropertyDialog(int resId, int propertyId, CWnd *pParent) : PropertyDialog(resId, propertyId, pParent) {
  }

  const T &getStartValue() const {
    return m_startValue;
  }

  void setCurrentValue(const T &v) {
    m_currentValue = v;
    if(isNotifyEnabled() && (m_currentValue != m_prevValue)) {
      notifyPropertyChanged(getPropertyId(), &m_prevValue, &m_currentValue);
    }
    m_prevValue = m_currentValue;
  }
  const T &getPrevValue() const {
    return m_prevValue;
  }
public:
  void setStartValue(const T &v) {
    m_startValue = v;
  }
  const T &getCurrentValue() const {
    return m_currentValue;
  }
  void setStartProperty(const void *v) {
    setStartValue(*(T*)v);
  }
};
