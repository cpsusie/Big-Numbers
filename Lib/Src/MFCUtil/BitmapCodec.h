#pragma once

#include <ByteArray.h>
#include <plbmpenc.h>
#include <plbmpdec.h>

void    encodeBitmap(HBITMAP bm, ByteOutputStream &out, PLPicEncoder &encoder);
HBITMAP decodeToBitmap(const ByteArray &bytes, PLPicDecoder &decoder, bool &hasAlpha);
