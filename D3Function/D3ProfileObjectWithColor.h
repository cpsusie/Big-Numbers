#pragma once

#include <D3DGraphics/D3SceneObjectWithMesh.h>

class D3SceneObjectVisualNormals;
class CProfileDlg;

class D3ProfileObjectWithColor : public D3SceneObjectWithMesh {
private:
  CProfileDlg                *m_dlg;
  int                         m_materialId;
  D3SceneObjectVisualNormals *m_normalsObj;
public:
  D3ProfileObjectWithColor(CProfileDlg *dlg);
  ~D3ProfileObjectWithColor();
  int getMaterialId() const {
    return m_materialId;
  }
  bool hasNormals() const {
    return false;
  }
  void modifyContextMenu(HMENU menu);
  void draw();
};
