#include "stdafx.h"

String IndexDefinition::toString() const {
  String result = String(_T("IndexDefinition:\n"))
                + format(_T("  Indexname  :<%s>\n"),m_indexName.cstr())
                + format(_T("  Tablename  :<%s>\n"),m_tableName.cstr())
                + format(_T("  Filename   :<%s>\n"),m_fileName.cstr() )
                + format(_T("  Indextype  :<%c>\n"),m_indexType       )
                + format(_T("  Columncount:%d\n")  ,getColumnCount()  );
  result += _T("  Columns  :(");
  for(UINT i = 0; i < getColumnCount();i++) {
    const IndexColumn &col = getColumn(i);
    result += format(_T("%d %c%s"),col.m_col, col.m_asc ? 'A':'D', i < getColumnCount() - 1 ? _T(","):_T(")\n"));
  }
  return result;
}

void IndexDefinition::dump(FILE *f) const {
  _ftprintf(f,_T("%s"),toString().cstr());
}
