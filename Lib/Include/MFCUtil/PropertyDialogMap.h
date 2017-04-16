#pragma once

#include <HashMap.h>
#include "PropertyDlgThread.h"

class PropertyDialogMap : private IntHashMap<CPropertyDlgThread*> {
private:
  mutable CPropertyDlgThread *m_visibleDlgThread;
public:
  PropertyDialogMap() : m_visibleDlgThread(NULL) {
  }
  ~PropertyDialogMap() {
    clear();
  }
  void addDialog(PropertyDialog *dlg);
  void removeDialog(int id);
  void clear();
  bool isDialogVisible() const;
  int  getVisibleDialogId() const;
  void hideDialog() const;
  void showDialog(int id, const void *data) const;
  bool hasPropertyContainer(const PropertyContainer *pc) const;
  const void *getProperty(int id) const;
};
