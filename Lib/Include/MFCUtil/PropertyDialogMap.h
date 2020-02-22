#pragma once

#include <HashMap.h>
#include <FastSemaphore.h>
#include "PropertyDlgThread.h"

// TODO:must be PropertyContainer receieving changes from contained dialogs and resend them to listener of this.
// it must also control visibility, and that at most one dialog is visible at any time
// should notify listener when a dialog is closed
class PropertyDialogMap : private IntHashMap<CPropertyDlgThread*> {
private:
  PointerHashSet<const PropertyContainer*> m_containerSet;
  mutable CPropertyDlgThread              *m_visibleDlgThread;
  mutable FastSemaphore                    m_gate;
  // no lock-protection
  void hideCurrentVisibleDialog() const;
  // no lock-protection
  inline bool isDialogVisible1() const {
    return m_visibleDlgThread && m_visibleDlgThread->isDialogVisible();
  }
  // no lock-protection
  // Return property-id of visible dialog, if any, or -1 if not visible dialog
  inline int  getCurrentVisibleDialogId() const {
    return isDialogVisible1() ? m_visibleDlgThread->getPropertyId() : -1;
  }
  void showDialog(int id, const void *data, size_t size) const;
  const void *getProperty(int id, size_t size) const;

public:
  PropertyDialogMap() : m_visibleDlgThread(NULL) {
  }
  ~PropertyDialogMap() {
    clear();
  }
  void addDialog(PropertyDialog *dlg);
  void removeDialog(int id);
  void clear();
  // lock-protected
  // Return true if any property-dialog is visible.
  bool isDialogVisible() const;
  // lock-protected
  // Return property-id of visible dialog, if any, or -1 if not visible dialog
  int  getVisibleDialogId() const;
  void hideDialog() const;
  bool hasPropertyContainer(const PropertyContainer *pc) const;
  template<typename T> void showDialog(int id, const T &data) const {
    showDialog(id, &data, sizeof(T));
  }
  template<typename T> T &getProperty(int id, T &v) const {
    v = *(T*)getProperty(id, sizeof(T));
    return v;
  }
};
