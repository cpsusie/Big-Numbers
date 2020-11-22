#include "stdafx.h"
#include "GrammarTables.h"
#include "TemplateWriter.h"
#include "GrammarCode.h"

void TablesWriter::handleKeyword(TemplateWriter &writer, String &line) const {
  const GrammarTables tables(m_coder);
  std::wostringstream tmp;
  tables.print(MarginFile(tmp));
  writer.printf(_T("%s"), tmp.str().c_str());
  m_coder.setByteCount(tables.getTotalSizeInBytes());
}
