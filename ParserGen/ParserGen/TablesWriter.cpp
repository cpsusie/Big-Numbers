#include "stdafx.h"
#include <MarginFile.h>
#include "GrammarTables.h"
#include "GrammarCode.h"
#include "TemplateWriter.h"

void TablesWriter::handleKeyword(TemplateWriter &writer, String &line) const {
  const GrammarTables tables(m_coder);
  std::wostringstream tmp;
  tables.print(MarginFile(tmp));
  writer.printf(_T("%s"), tmp.str().c_str());
  m_coder.setByteCount(tables.getTotalSizeInBytes());
}
