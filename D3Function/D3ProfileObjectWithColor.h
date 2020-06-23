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
  ~D3ProfileObjectWithColor() override;
  int getMaterialId() const override {
    return m_materialId;
  }
  bool hasNormals() const override {
    return false;
  }
  void modifyContextMenu(HMENU menu) override;
  void draw() override;
};
