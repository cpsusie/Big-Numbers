#include "stdafx.h"
#include "GrammarTables.h"
#include "GrammarCode.h"

void TablesWriter::handleKeyword(TemplateWriter &writer, String &line) const {
  GrammarTables tables(m_coder.getGrammar(), m_coder.getTablesClassName(), m_coder.getParserClassName());
  const String  tmpFileName = TemplateWriter::createTempFileName(_T("txt"));
  MarginFile    f(tmpFileName);
  tables.print(f, m_coder.getGrammar().getLanguage(), m_coder.getFlags().m_useTableCompression);
  f.close();
  const String text = readTextFile(tmpFileName);
  writer.printf(_T("%s"), text.cstr());
  unlink(tmpFileName);
  m_coder.setByteCount(tables.getTotalSizeInBytes());
}
