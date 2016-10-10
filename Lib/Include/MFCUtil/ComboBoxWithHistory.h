#pragma once

#include <Registry.h>
#include "WinTools.h"

class ComboBoxHistory : public StringArray {
private:
  RegistryKey getKey();
  String      m_registryName;
  UINT        m_maxHistoryLength;
public:
  ComboBoxHistory() {
    m_maxHistoryLength = 25;
  }
  ~ComboBoxHistory();
  void setRegistryName(const String &name);
  void load();
  void save();
  bool add(const String &s);
  void removeString(const String &str);
  void setMaxHistoryLength(UINT maxLength);
  inline UINT getMaxHistoryLength() const {
    return m_maxHistoryLength;
  }
};

class CComboBoxWithHistory : public CComboBox {
private:
  ComboBoxHistory m_history;
  void fillDropdownList();
public:
  CComboBoxWithHistory() {
  }
  void substituteControl(CWnd *parent, int id, const String &registryName);
  void load();
  void save();
  void updateList();
  inline void setMaxHistoryLength(UINT maxLength) {
    m_history.setMaxHistoryLength(maxLength);
  }
  inline UINT getMaxHistoryLength() const {
    return m_history.getMaxHistoryLength();
  }
public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);

public:
	virtual ~CComboBoxWithHistory();

protected:
	DECLARE_MESSAGE_MAP()
};
