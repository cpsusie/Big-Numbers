#pragma once

#include <MFCUtil/DirectXDeviceFactory.h>

#define VADJUST_X    0x01
#define VADJUST_Y    0x02
#define VADJUST_Z    0x04
#define VADJUST_ALL  (VADJUST_X | VADJUST_Y | VADJUST_Z)

class D3World {
private:
  D3DXVECTOR3    m_scale, m_pos;
  D3DXQUATERNION m_q;

  inline D3World &decomposeMatrix(D3DXVECTOR3 &scale, D3DXQUATERNION &q, D3DXVECTOR3 &pos, const D3DXMATRIX &mat) {
    V(D3DXMatrixDecompose(&scale, &q, &pos, &mat));
    return *this;
  }
  static inline D3DXMATRIX composeMatrix(const D3DXVECTOR3 &scale, const D3DXQUATERNION &q, const D3DXVECTOR3 &pos, D3DXMATRIX &mat) {
    return *D3DXMatrixTransformation(&mat, NULL, NULL, &scale, NULL, &q, &pos);
  }

public:
  D3World() {
    reset();
  }
  D3World(const D3DXMATRIX &world) {
    decomposeMatrix(m_scale, m_q, m_pos, world);
  }
  operator D3DXMATRIX() const {
    D3DXMATRIX world;
    return composeMatrix(m_scale, m_q, m_pos, world);
  }
  inline D3World &reset() {
    return resetPos().resetOrientation().resetScale();
  }
  inline D3World &resetPos() {
    return setPos(D3DXORIGIN);
  }
  inline D3World &resetOrientation() {
    D3DXQuaternionIdentity(&m_q);
    return *this;
  }
  inline D3World &resetScale() {
    return setScaleAll(1);
  }
  inline D3World &setPos(const D3DXVECTOR3 &pos) {
    m_pos = pos;
    return *this;
  }
  inline const D3DXVECTOR3 &getPos() const {
    return m_pos;
  }
  inline D3World &setScale(const D3DXVECTOR3 &scale) {
    m_scale = scale;
    return *this;
  }
  inline const D3DXVECTOR3 &getScale() const {
    return m_scale;
  }
  inline D3World &setScaleAll(float scale) {
    return setScale(D3DXVECTOR3(scale, scale, scale));
  }
  inline D3World &multiplyScale(float factor, BYTE component = VADJUST_ALL) {
    if(component & VADJUST_X) m_scale.x *= factor;
    if(component & VADJUST_Y) m_scale.y *= factor;
    if(component & VADJUST_Z) m_scale.z *= factor;
    return *this;
  }
  inline D3World &setOrientation(const D3DXQUATERNION &q) {
    m_q = q;
    return *this;
  }
  // centerOfRotation in world space
  D3World &setOrientation(const D3DXQUATERNION &q, const D3DXVECTOR3 &centerOfRotation);
  inline D3DXVECTOR3 getDir() const {
    return rotate(E[0], m_q);
  }
  inline D3DXVECTOR3 getUp() const {
    return rotate(E[2], m_q);
  }
  inline D3DXVECTOR3 getRight() const {
    return cross(getDir(), getUp());
  }
  inline D3DXVECTOR3 getLeft() const {
    return cross(getUp(), getDir());
  }
  inline const D3DXQUATERNION &getOrientation() const {
    return m_q;
  }
  inline D3World &setLookAt(const D3DXVECTOR3 &point) {
    return setLookAt(getPos(), point, getUp());
  }
  D3World &setLookAt(const D3DXVECTOR3 &pos, const D3DXVECTOR3 &lookAt, const D3DXVECTOR3 &up);

  D3DXMATRIX &createViewMatrix(D3DXMATRIX &dst, bool rightHanded) const;
  String toString(int dec = 3) const;
};

bool operator==(const D3World &w1, const D3World &w2);
inline bool operator!=(const D3World &w1, const D3World &w2) {
  return !(w1 == w2);
}
