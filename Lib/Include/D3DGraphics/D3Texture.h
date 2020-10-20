#pragma once

#include <MFCUtil/DirectXDeviceFactory.h>

// Sent to listener for id=SP_TEXTUREIMAGE
class D3Texture {
private:
  // If < 0, texture is undefined and m_image == nullptr
  int               m_id;
  LPDIRECT3DTEXTURE m_image;
public:
  inline D3Texture() {
    setUndefined();
  }
  inline explicit D3Texture(UINT id) {
    setUndefined().m_id = id;
  }
  inline D3Texture &operator=(const LPDIRECT3DTEXTURE &image) {
    m_image = image;
    return *this;
  }
  ~D3Texture() {
    m_image = nullptr;
  }
  D3Texture &setUndefined() {
    m_image = nullptr;
    m_id    = -1;
    return *this;
  }
  inline bool isDefined() const {
    return m_id >= 0;
  }
  inline int getId() const {
    return m_id;
  }
  const LPDIRECT3DTEXTURE &getImage() const {
    return m_image;
  }
};

inline bool operator==(const D3Texture &t1, const D3Texture &t2) {
  return (t1.getId() == t2.getId()) && (t1.getImage() == t2.getImage());
}

inline bool operator!=(const D3Texture &t1, const D3Texture &t2) {
  return !(t1 == t2);
}

typedef CompactUIntHashMap<D3Texture,200> TextureMap;
