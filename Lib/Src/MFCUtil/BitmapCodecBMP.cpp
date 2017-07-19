#include "pch.h"
#include <plbmpenc.h>
#include <plbmpdec.h>
#include "BitmapCodec.h"

void writeAsBMP(HBITMAP bm, ByteOutputStream &out) {
  encodeBitmap(bm, out, PLBmpEncoder());
}
