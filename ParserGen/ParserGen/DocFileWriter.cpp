#include "stdafx.h"
#include "GrammarCode.h"

void DocFileWriter::handleKeyword(TemplateWriter &writer, String &line) const {
  m_coder.generateDocFile();
}
