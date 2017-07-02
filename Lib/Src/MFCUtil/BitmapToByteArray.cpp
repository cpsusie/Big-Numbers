#include "pch.h"
#include <ByteMemoryStream.h>
#include <MFCUtil/FileSink.h>

#include <PragmaLib.h>
#include <pljpegenc.h>
//#include <plbmpenc.h>
#include <plwinbmp.h>


#pragma comment(lib, LIB_VERSION "libjpeg.lib")
#pragma comment(lib,LIB_VERSION "common.lib")
#pragma comment(lib,LIB_VERSION "zlib.lib")

ByteArray &bitmapToByteArray(ByteArray &dst, HBITMAP bm) {
  dst.clear();
  PLWinBmp winBmp;
  winBmp.CreateFromHBitmap(bm);

  PLJPEGEncoder().SaveBmp(&winBmp, &ByteStreamSink(ByteMemoryOutputStream(dst)));
  return dst;
}
/*
HBITMAP bitmapFromByteArray(ByteArray &src) {
  PLWinBmp winBmp;
  decoder.MakeBmpFromMemory(src..getData(),src.size(),&winBmp);
  const int width  = winBmp.GetWidth();
  const int height = winBmp.GetHeight();
  CBitmap bitmap;
  return CreateBitmap(width,height,1,winBmp.GetBitsPerPixel(),winBmp.GetBits());
}
*/
