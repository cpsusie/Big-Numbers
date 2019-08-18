#pragma once

void overWriteCurrentChar(CDialog *dlg, TCHAR ascii);
void overWriteCurrentChar(CDialog *dlg, TCHAR ascii, UINT64 minValue, UINT64 maxValue);
bool isOverwriteCurrentHexChar(CDialog *dlg, MSG *pMsg, UINT64 minValue = 0, UINT64 maxValue = -1);
