#pragma once

#include <PropertyContainer.h>
#include "ColorMap.h"

typedef enum {
  COLORMAPDATA
 ,DIALOGACTIVE
} ColorMapEditorProperties;

class CEditColorMapDlgThread : private CWinThread, public PropertyContainer {
private:
  friend class CEditColorMapDlg;

  ColorMapData m_cd;
  bool         m_dlgActive;
  void setColorMapData(const ColorMapData &cd);
  DECLARE_DYNCREATE(CEditColorMapDlgThread)
protected:
  CEditColorMapDlgThread();

public:
  void kill();
  const ColorMapData &getColorMapData() const {
    return m_cd;
  }

public:
  virtual BOOL InitInstance();
  static CEditColorMapDlgThread *startThread(PropertyChangeListener *listener, const ColorMapData &cd);
protected:
  virtual ~CEditColorMapDlgThread();
  DECLARE_MESSAGE_MAP()
};
