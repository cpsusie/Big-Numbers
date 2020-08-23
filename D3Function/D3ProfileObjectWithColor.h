#pragma once

#include <D3DGraphics/D3SceneObjectWithMesh.h>
#include <D3DGraphics/D3SceneObjectPoint.h>

class D3SceneEditor;
class D3SceneObjectVisualNormals;
class CProfileDlg;

class ProfileObjectProperties {
  friend class D3ProfileObjectWithColor;
private:
  D3DXMATRIX         m_world;
  D3SceneObjectPoint m_centerOfRotation;
  D3DFILLMODE        m_fillMode;
  D3DSHADEMODE       m_shadeMode;
  D3MATERIAL         m_material;
public:
  void resetProperties();
  ProfileObjectProperties() {
    resetProperties();
  }
};

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
  D3SceneEditor &getD3SceneEditor() const;
  void getAllProperties(      ProfileObjectProperties &prop) const;
  void setAllProperties(const ProfileObjectProperties &prop);

  void modifyContextMenu(HMENU menu) override;
  void draw() override;
};
