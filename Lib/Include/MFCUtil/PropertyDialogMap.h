#pragma once

#include <HashMap.h>
#include <FastSemaphore.h>
#include <PropertyContainer.h>

class CPropertyDialogThread;
class PropertyDialog;

typedef enum {
  PDM_VISIBLE_PROPERTYID  // int    - notification id, when active dialog changes, or hide/show
 ,PDM_CURRENT_DLGVALUE    // void * - notification id, when value in active dialog changes
 ,PDM_DIALOG_COUNT        // UINT   . notification id, when number of dialogs in map changes
} PropertyDialogMapProperty;

class PropertyDialogMap : private IntHashMap<CPropertyDialogThread*>, public PropertyContainer, private PropertyChangeListener {
private:
  mutable FastSemaphore  m_gate, m_lock1;
  CPropertyDialogThread *m_visibleDialogThread;
  int                    m_visiblePropertyId;

  void setVisibleDialogThread(CPropertyDialogThread *newValue);
  // no lock-protection
  bool isDialogVisible1() const;
  // no lock-protection
  // Return propertyId - getVisibledialogId() if has visible dialog, or -1 if no visible dialog
  int  getPropertyOffset(int propertyId) const {
    const int vdid = getVisiblePropertyId();
    return (vdid >= 0) ? (propertyId - vdid) : -1;
  }

  // Return true, if id or data differs from current id/dlg-data, else return false
  bool showDialog(int id, const void *data, size_t size);
  // no lock-protection
  PropertyDialogMap &hideVisibleDialog();
  const void *getProperty(int id, size_t size) const;
  void handlePropertyChanged(const PropertyContainer *source, int id, const void *oldValue, const void *newValue);
public:
  inline PropertyDialogMap()
    : m_visibleDialogThread(NULL)
    , m_visiblePropertyId(  -1  )
  {
  }
  ~PropertyDialogMap() {
    removeAllDialogs();
    PropertyContainer::clear();
  }
  // notification Id=PDM_DIALOG_COUNT
  PropertyDialogMap &addDialog(PropertyDialog *dlg);
  // notification Id=PDM_DIALOG_COUNT
  PropertyDialogMap &removeDialog(int id);
  // notification Id=PDM_DIALOG_COUNT (once)
  PropertyDialogMap &removeAllDialogs();
  // lock-protected
  // Return true if any property-dialog is visible.
  bool isDialogVisible() const;
  // Return property-id of visible dialog, if any, or -1 if no visible dialog
  inline int getVisiblePropertyId() const {
    return m_visiblePropertyId;
  }
  String getVisibleTypeName() const;
  PropertyDialogMap &hideDialog();
  // Return true, if id or data differs from current id/dlg-data, else return false
  template<typename T> bool showDialog(int id, const T &data) {
    return showDialog(id, &data, sizeof(T));
  }
  template<typename T> T &getProperty(int id, T &v) const {
    v = *(T*)getProperty(id, sizeof(T));
    return v;
  }
  template<typename T> bool getVisibleProperty(T &v) const {
    const int vid = getVisiblePropertyId();
    if(!vid) return false;
    v = *(T*)getProperty(vid, sizeof(T));
    return true;
  }
};
