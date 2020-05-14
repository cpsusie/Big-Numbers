#pragma once

#include <PropertyContainer.h>

inline void putWindowBesideMainWindow(CWnd *wnd) {
  putWindowBesideWindow(wnd, AfxGetApp()->m_pMainWnd);
}

#define PROPDLG_VISIBLE_OFFSET 1

class PropertyDialog : public CDialog, public PropertyContainer {
private:
  const int m_propertyId;
  bool      m_hasBeenVisible;
  bool      m_visible; // when changed, notification with id = m_propertyId+PROPDLG_VISIBLE_OFFSET (bool) is sent to listeners
  bool      m_showWinActive;
protected:
  PropertyDialog(int resId, int propertyId, CWnd *pParent) : CDialog(resId, pParent), m_propertyId(propertyId) {
    setNotifyEnable(false);
    m_hasBeenVisible = false;
    m_visible        = false;
    m_showWinActive  = false; // to prevent infinite recursion
  }
  void setVisible(bool visible);
  void OnShowWindow(BOOL bShow, UINT nStatus);
  void checkSize(const TCHAR *method, size_t size) const {
    if(size != getDataSize()) {
      throwInvalidArgumentException(method, _T("size=%zu, getDataSize(%s)=%zu"), size, getTypeName().cstr(), getDataSize());
    }
  }
public:
  virtual ~PropertyDialog() {
    PropertyContainer::clear();
  }
  inline int getPropertyId() const {
    return m_propertyId;
  }
  inline int getPropertyIdOffset(int propertyId) const {
    return propertyId - getPropertyId();
  }
  inline bool isVisible() const {
    return m_visible;
  }
  virtual bool setStartProperty(const void *v, size_t size) = 0;
  virtual const void *getCurrentProperty(size_t size) const = 0;
  virtual void reposition() {
    putWindowBesideMainWindow(this);
  }
  virtual void resetControls() {
  }
  virtual size_t getDataSize() const = 0;
  virtual String getTypeName() const = 0;
};

template<typename T> class CPropertyDialog : public PropertyDialog {
private:
  T m_startValue, m_currentValue;
protected:
  CPropertyDialog(int resId, int propertyId, CWnd *pParent) : PropertyDialog(resId, propertyId, pParent) {
  }

  const T &getStartValue() const {
    return m_startValue;
  }

  void setCurrentValue(const T &v) {
    setProperty(getPropertyId(), m_currentValue, v);
  }
public:
  // Return true if m_startValue is changed
  bool setStartValue(const T &v) {
    if(v == m_startValue) {
      return false;
    }
    m_startValue = v;
    return true;
  }
  const T &getCurrentValue() const {
    return m_currentValue;
  }
  const void *getCurrentProperty(size_t size) const {
    checkSize(__TFUNCTION__, size);
    return &m_currentValue;
  }
  bool setStartProperty(const void *v, size_t size) {
    checkSize(__TFUNCTION__, size);
    return setStartValue(*(T*)v);
  }
  size_t getDataSize() const {
    return sizeof(T);
  }
};
