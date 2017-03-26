#include "pch.h"
#include <D3DGraphics/D3Scene.h>

DECLARE_THISFILE;

LPDIRECT3DTEXTURE loadTextureFromFile(LPDIRECT3DDEVICE device, const String &fileName) {
  LPDIRECT3DTEXTURE result;
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

LPDIRECT3DTEXTURE loadTextureFromByteArray(LPDIRECT3DDEVICE device, ByteArray &ba) {
  LPDIRECT3DTEXTURE result;
  V(D3DXCreateTextureFromFileInMemoryEx(device
                                       ,ba.getData(), (UINT)ba.size()
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

LPDIRECT3DTEXTURE loadTextureFromResource(LPDIRECT3DDEVICE device, int resId, const String &typeName) {
  ByteArray tmp;
  tmp.loadFromResource(resId, typeName.cstr());
  return loadTextureFromByteArray(device, tmp);
}

LPDIRECT3DTEXTURE getTextureFromBitmap(LPDIRECT3DDEVICE device, HBITMAP bm) {
  ByteArray tmp;
  return loadTextureFromByteArray(device, bitmapToByteArray(tmp, bm));
}

