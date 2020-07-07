#pragma once

#include <MFCUtil/WinTools.h>

CBitmap          *getRotatedBitmapResource(int id, int degree);
HBITMAP           bitmapRotate(HBITMAP  b0, double degree);
CBitmap          *bitmapRotate(CBitmap *b0, double degree);
HBITMAP           bitmapRotate(LPDIRECT3DDEVICE device, HBITMAP  b0, double degree);
CBitmap          *bitmapRotate(LPDIRECT3DDEVICE device, CBitmap *b0, double degree);
