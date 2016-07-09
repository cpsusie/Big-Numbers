#include "stdafx.h"

DECLARE_THISFILE;

LPDIRECT3DTEXTURE9 loadTextureFromFile(LPDIRECT3DDEVICE9 device, const String &fileName) {
  LPDIRECT3DTEXTURE9 result;
  V(D3DXCreateTextureFromFileEx( device
                                ,fileName.cstr()
                                ,D3DX_DEFAULT, D3DX_DEFAULT, 1
                                ,D3DUSAGE_DYNAMIC
                                ,D3DFMT_A8R8G8B8
                                ,D3DPOOL_DEFAULT
                                ,D3DX_FILTER_NONE
                                ,D3DX_FILTER_NONE
                                ,0xffffffff
                                ,NULL
                                ,NULL
                                ,&result));

  return result;
}

LPDIRECT3DTEXTURE9 loadTextureFromByteArray(LPDIRECT3DDEVICE9 device, ByteArray &ba) {
  LPDIRECT3DTEXTURE9 result;
  V(D3DXCreateTextureFromFileInMemoryEx(device
                                       ,ba.getData(), ba.size()
                                       ,D3DX_DEFAULT, D3DX_DEFAULT, 1
                                       ,D3DUSAGE_DYNAMIC
                                       ,D3DFMT_A8R8G8B8
                                       ,D3DPOOL_DEFAULT
                                       ,D3DX_FILTER_NONE
                                       ,D3DX_FILTER_NONE
                                       ,0xffffffff
                                       ,NULL
                                       ,NULL
                                       ,&result));
  return result;
}

LPDIRECT3DTEXTURE9 loadTextureFromResource(LPDIRECT3DDEVICE9 device, int resId, const String &typeName) {
  ByteArray tmp;
  tmp.loadFromResource(resId, typeName.cstr());
  return loadTextureFromByteArray(device, tmp);
}

LPDIRECT3DTEXTURE9 getTextureFromBitmap(LPDIRECT3DDEVICE9 device, HBITMAP bm) {
  ByteArray tmp;
  return loadTextureFromByteArray(device, bitmapToByteArray(tmp, bm));
}

