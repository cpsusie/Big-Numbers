#include "pch.h"

String::String(char ch) {
  TCHAR tmp[] = { (TCHAR)ch, 0 };
  m_buf = newCharBuffer(tmp, m_len, m_capacity);
}

String::String(SHORT n) {
  TCHAR tmp[20];
  _stprintf(tmp, _T("%hd"), n);
  m_buf = newCharBuffer(tmp, m_len, m_capacity);
}

String::String(USHORT n) {
  TCHAR tmp[20];
  _stprintf(tmp, _T("%hu"), n);
  m_buf = newCharBuffer(tmp, m_len, m_capacity);
}

String::String(INT n) {
  TCHAR tmp[20];
  _stprintf(tmp, _T("%d"), n);
  m_buf = newCharBuffer(tmp, m_len, m_capacity);
}

String::String(UINT n) {
  TCHAR tmp[20];
  _stprintf(tmp, _T("%u"), n);
  m_buf = newCharBuffer(tmp, m_len, m_capacity);
}

String::String(LONG n) {
  TCHAR tmp[20];
  _stprintf(tmp, _T("%ld"), n);
  m_buf = newCharBuffer(tmp, m_len, m_capacity);
}

String::String(ULONG n) {
  TCHAR tmp[20];
  _stprintf(tmp, _T("%lu"), n);
  m_buf = newCharBuffer(tmp, m_len, m_capacity);
}

String::String(INT64 n) {
  TCHAR tmp[100];
  _stprintf(tmp, _T("%I64d"), n);
  m_buf = newCharBuffer(tmp, m_len, m_capacity);
}

String::String(UINT64 n) {
  TCHAR tmp[100];
  _stprintf(tmp, _T("%I64u"), n);
  m_buf = newCharBuffer(tmp, m_len, m_capacity);
}

String::String(float flt) {
  TCHAR tmp[100];
  _stprintf(tmp, _T("%g"), flt);
  m_buf = newCharBuffer(tmp, m_len, m_capacity);
}

String::String(double dbl) {
  TCHAR tmp[100];
  _stprintf(tmp, _T("%lg"), dbl);
  m_buf = newCharBuffer(tmp, m_len, m_capacity);
}
