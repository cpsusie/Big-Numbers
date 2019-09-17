#pragma once

void overWriteCurrentChar(     CDialog *dlg, TCHAR ascii);
void overWriteCurrentDecChar(  CDialog *dlg, TCHAR ascii, UINT64 minValue, UINT64 maxValue);
void overWriteCurrentHexChar(  CDialog *dlg, TCHAR ascii, UINT64 minValue, UINT64 maxValue);
bool isOverwriteCurrentHexChar(CDialog *dlg, MSG *pMsg  , UINT64 minValue, UINT64 maxValue);
bool isOverwriteCurrentDecChar(CDialog *dlg, MSG *pMsg  , UINT64 minValue, UINT64 maxValue);
bool isOverwriteCurrentHexChar(CDialog *dlg, MSG *pMsg);
bool isOverwriteCurrentChar(   CDialog *dlg, MSG *pMsg, int maxLength = -1);