#pragma once

#include "D3Math.h"

class D3PosDirUpScale {
private:
  bool        m_rightHanded;
  D3DXVECTOR3 m_pos, m_dir, m_up, m_scale;
  D3DXMATRIX  m_view;
  D3PosDirUpScale &updateView();
public:
  D3PosDirUpScale(bool rightHanded);
  inline bool getRightHanded() const {
    return m_rightHanded;
  }
  // return *this
  D3PosDirUpScale &setRightHanded(bool rightHanded);
  inline D3DXVECTOR3 getPos() const {
    return m_pos;
  }
  inline D3DXVECTOR3 getDir() const {
    return m_dir;
  }
  inline D3DXVECTOR3 getUp() const {
    return m_up;
  }
  inline D3DXVECTOR3 getRight() const {
    return cross(m_dir, m_up);
  }
  inline D3DXVECTOR3 getScale() const {
    return m_scale;
  }
  D3PosDirUpScale &setPos(const D3DXVECTOR3 &pos) {
    m_pos = pos;
    return updateView();
  }
  D3PosDirUpScale &setOrientation(const D3DXVECTOR3 &dir, const D3DXVECTOR3 &up);
  inline D3PosDirUpScale &setScale(const D3DXVECTOR3 &scale) {
    m_scale = scale;
    return *this;
  }
  inline D3PosDirUpScale &setScaleAll(float scale) {
    return setScale(D3DXVECTOR3(scale, scale, scale));
  }
  // set Position to (0,0,0)
  inline D3PosDirUpScale &resetPos() {
    return setPos(getDefaultPos());
  }
  // set scale in all dimensions to 1
  inline D3PosDirUpScale &resetScale() {
    return setScaleAll(1);
  }
  // Return vector (0,0,0)
  static D3DXVECTOR3 getDefaultPos()   { return D3DXORIGIN;            }
  // Return vector (0,0,0)
  static D3DXVECTOR3 getDefaultScale() { return D3DXVECTOR3(1, 1, 1);  }
  // Return vector (0,1,0)
  static D3DXVECTOR3 getDefaultDir()   { return D3DXVECTOR3(0, 0, -1); }
  // Return vector (0,0,1)
  static D3DXVECTOR3 getDefaultUp()    { return D3DXVECTOR3(0, 1, 0);  }
  // Set dir=(0,1,0), up=(0,0,1)
  inline D3PosDirUpScale &resetOrientation() { return setOrientation(getDefaultDir(), getDefaultUp()); }

  D3PosDirUpScale &setWorldMatrix(const D3DXMATRIX &world);
  inline D3DXMATRIX getWorldMatrix() const {
    D3DXMATRIX result;
    return *D3DXMatrixScaling(&result, m_scale.x, m_scale.y, m_scale.z) * invers(m_view);
  }
  inline const D3DXMATRIX &getViewMatrix() const {
    return m_view;
  }
  inline D3DXMATRIX        getRotationMatrix() const {
    return D3PosDirUpScale(*this).resetPos().resetScale().getWorldMatrix();
  }
  inline D3DXQUATERNION    getQuarternion() const {
    D3DXQUATERNION result;
    return *D3DXQuaternionRotationMatrix(&result, &getRotationMatrix());
  }
  inline D3DXMATRIX        getScaleMatrix()    const {
    D3DXMATRIX result;
    return *D3DXMatrixScaling(&result, m_scale.x, m_scale.y, m_scale.z);
  }
  String toString(int dec = 3) const;
  bool operator==(const D3PosDirUpScale &pdus) const;
  inline bool operator!=(const D3PosDirUpScale &pdus) const {
    return !(*this == pdus);
  }
};
