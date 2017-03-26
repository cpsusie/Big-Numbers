#pragma once

typedef struct {
  const TCHAR *m_name;
  COLORREF     m_color;
} ColorName;

const ColorName *findColorByName(const TCHAR *name);
String getLegalColorNames();
int getColorCount();
COLORREF getColor(int index);

void usage();

CFont *scaleFont(CFont *src, double scale);

COLORREF randomColor();

void errorMessage(TCHAR *format,...);
