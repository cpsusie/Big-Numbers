#include "pch.h"
#include <pljpegenc.h>
#include <pljpegdec.h>
#include "BitmapCodec.h"

#pragma comment(lib, LIB_VERSION "libjpeg.lib")

void writeAsJPG(HBITMAP bm, ByteOutputStream &out) {
  encodeBitmap(bm, out, PLJPEGEncoder());
}
