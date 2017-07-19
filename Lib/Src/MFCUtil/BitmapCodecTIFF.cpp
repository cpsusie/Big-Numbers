#include "pch.h"
#include <pltiffenc.h>
#include <pltiffdec.h>
#include "BitmapCodec.h"

#pragma comment(lib, LIB_VERSION "libtiff.lib")

void writeAsTIFF(HBITMAP bm, ByteOutputStream &out) {
  encodeBitmap(bm, out, PLTIFFEncoder());
}

HBITMAP decodeAsTIFF(const ByteArray &bytes, bool &hasAlpha) {
  return decodeToBitmap(bytes, PLTIFFDecoder(), hasAlpha);
}

