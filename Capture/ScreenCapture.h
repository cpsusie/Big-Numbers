#pragma once

HBITMAP captureScreen(const CRect &rect);
HBITMAP captureScreen(int x, int y, const CSize &size);
HBITMAP captureWindow(HWND hwnd);
