#include "stdafx.h"
#include "GridParameters.h"

static const TCHAR *symbolHeader = _T("Symbol, Farve , Antal\n");

String CellSpec::toString() const {
  return format(_T("%-6s, %06x, %s\n"), getSymbolStr().cstr(), m_color&0xffffff, format1000(m_count).cstr());
}

const TCHAR *CellSpec::s_digitChar = _T("0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz#¤$%&/\\=?*-:<>§!|^\"@");
const int    CellSpec::s_radix     = (int)_tcslen(CellSpec::s_digitChar);

const String &CellSpec::getSymbolStr() const {
  if(m_symStr.length()) return m_symStr;
  int n = m_symbol;
  String tmp;
  do {
    tmp += s_digitChar[n % s_radix];
    n /= s_radix;
  } while(n);
  return m_symStr = rev(tmp);
}

static int getDecimalDigitCount(size_t n) {
  int count = 0;
  do {
    count++;
    n /= 10;
  } while(n);
  return count;
}

String PearlDiagram::toString() const {
  const size_t symCount = m_cellSpecArray.size();
  StringArray symStr(symCount);
  size_t total  = 0;
  int    maxLen = 0;
  String aStr = symbolHeader;
  for(size_t i = 0; i < symCount; i++) {
    const CellSpec &cs = m_cellSpecArray[i];
    total += cs.m_count;
    const String tmp = cs.getSymbolStr();
    symStr.add(tmp);
    aStr += cs.toString();
    if(tmp.length() > maxLen) maxLen = (int)tmp.length();
  }
  const MatrixDimension dim = m_cellMatrix.getDimension();
  const int cellLen = maxLen+1;
  for(size_t i = 0; i < symCount; i++) {
    String &s = symStr[i];
    if(s.length() < cellLen) {
      s = spaceString(cellLen-s.length()) + s;
    }
  }
  String mstr = _T("\n");

  const int vRulerW1 = getDecimalDigitCount(dim.rowCount/10);
  String hRulerStr = spaceString(vRulerW1+1), delim;
  if(dim.columnCount >= 10) {
    for(UINT x = 0; x < dim.columnCount; x += 10) {
      if(delim.length()) {
        hRulerStr += delim;
      } else {
        delim = spaceString(cellLen*9);
      }
      hRulerStr += format(_T("%*d"), cellLen, x/10);
    }
    hRulerStr += format(_T("\n%*s"),vRulerW1+1,_T(""));
  }
  for(UINT x = 0; x < dim.columnCount; x++) {
    hRulerStr += format(_T("%*d"), cellLen, x%10);
  }
  hRulerStr += _T("\n");

  for(UINT r = 0; r < dim.rowCount; r++) {
    const String vRulerStr = (r%10==0) 
                           ? format(_T("%*d0") , vRulerW1,r/10) 
                           : format(_T("%*s%d"), vRulerW1,_T(" "),r%10);
    mstr += vRulerStr;
    for(UINT c = 0; c < dim.columnCount; c++) {
      mstr += symStr[m_cellMatrix(r,c)];
    }
    mstr += _T(" ");
    mstr += vRulerStr;
    mstr += _T("\n");
  }
  String infoStr = format(_T("Dimension         :%d x %d\nAntal farver      :%d\nTotal antal celler:%s\n\n")
                         ,dim.columnCount, dim.rowCount
                         ,symCount
                         ,format1000(total).cstr());
  return infoStr + aStr + hRulerStr + mstr + hRulerStr;
}
