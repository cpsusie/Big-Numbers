#pragma once

#include "MyString.h"
#include "Array.h"
#include "WinTools.h"

void clipboardDropFiles(HWND hwnd, StringArray  &fnames);
void clipboardDropFile( HWND hwnd, const String &fname );
void clipboardDropFile( HWND hwnd, const char   *fname );

String getClipboardText();
void putClipboard(HWND hwnd, const String &s);

HBITMAP getClipboardBitmap();
void putClipboard(HWND hwnd, HBITMAP bitmap);
