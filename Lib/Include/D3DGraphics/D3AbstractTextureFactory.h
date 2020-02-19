#pragma once

#include <MyUtil.h>
#include <MFCUtil/D3DeviceFactory.h>

class AbstractTextureFactory {
public:
  static LPDIRECT3DTEXTURE loadTextureFromFile(          LPDIRECT3DDEVICE device, const String &fileName);
  static LPDIRECT3DTEXTURE loadTextureFromResource(      LPDIRECT3DDEVICE device, int resId, const String &typeName);
  static LPDIRECT3DTEXTURE loadTextureFromByteArray(     LPDIRECT3DDEVICE device, ByteArray &ba);
  static LPDIRECT3DTEXTURE getTextureFromBitmap(         LPDIRECT3DDEVICE device, HBITMAP bm);
  static LPDIRECT3DTEXTURE loadTextureFromBitmapResource(LPDIRECT3DDEVICE device, int id);

  virtual LPDIRECT3DTEXTURE loadTextureFromFile(          const String &fileName)            = 0;
  virtual LPDIRECT3DTEXTURE loadTextureFromResource(      int resId, const String &typeName) = 0;
  virtual LPDIRECT3DTEXTURE loadTextureFromByteArray(     ByteArray &ba)                     = 0;
  virtual LPDIRECT3DTEXTURE getTextureFromBitmap(         HBITMAP bm)                        = 0;
  virtual LPDIRECT3DTEXTURE loadTextureFromBitmapResource(int id)                            = 0;
};
