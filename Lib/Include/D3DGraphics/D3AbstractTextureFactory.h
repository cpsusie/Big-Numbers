#pragma once

#include <MyUtil.h>
#include <MFCUtil/DirectXDeviceFactory.h>

class AbstractTextureFactory {
public:
  static LPDIRECT3DTEXTURE loadTextureFromFile(          LPDIRECT3DDEVICE device, const String &fileName);
  static LPDIRECT3DTEXTURE loadTextureFromResource(      LPDIRECT3DDEVICE device, int resId, const String &typeName);
  static LPDIRECT3DTEXTURE loadTextureFromByteArray(     LPDIRECT3DDEVICE device, ByteArray &ba);
  static LPDIRECT3DTEXTURE getTextureFromBitmap(         LPDIRECT3DDEVICE device, HBITMAP bm);
  static LPDIRECT3DTEXTURE loadTextureFromBitmapResource(LPDIRECT3DDEVICE device, int id);

  // call SAFERELEASE(texture); on the returned value when finished use
  inline LPDIRECT3DTEXTURE loadTextureFromFile(const String &fileName) const {
    return loadTextureFromFile(getDirectDevice(), fileName);
  }
  // call SAFERELEASE(texture); on the returned value when finished use
  inline LPDIRECT3DTEXTURE loadTextureFromResource(int resId, const String &typeName) const {
    return loadTextureFromResource(getDirectDevice(), resId, typeName);
  }
  // call SAFERELEASE(texture); on the returned value when finished use
  inline LPDIRECT3DTEXTURE loadTextureFromByteArray(ByteArray &ba) const {
    return loadTextureFromByteArray(getDirectDevice(), ba);
  }
  // call SAFERELEASE(texture); on the returned value when finished use
  inline LPDIRECT3DTEXTURE getTextureFromBitmap(HBITMAP bm) const {
    return getTextureFromBitmap(getDirectDevice(), bm);
  }
  // call SAFERELEASE(texture); on the returned value when finished use
  inline LPDIRECT3DTEXTURE loadTextureFromBitmapResource(int id) const {
    return loadTextureFromBitmapResource(getDirectDevice(), id);
  }
  bool validateTextureFile(const String &textureFileName) const;

  virtual LPDIRECT3DDEVICE getDirectDevice() const = 0;
};

String selectAndValidateTextureFile(AbstractTextureFactory &atf);
