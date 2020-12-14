#include "stdafx.h"
#include "Grammar.h"
#include "GrammarCode.h"
#include "TemplateWriter.h"

void ActionsWriter::handleKeyword(TemplateWriter &writer, String &line) const {
  const Options &options = Options::getInstance();
  if(!options.m_generateActions) {
    return;
  }

  const Grammar &grammar         = m_coder.getGrammar();
  const int      productionCount = grammar.getProductionCount();
  int            actionCount     = 0;

  for(int p = 0; p < productionCount; p++) { // first count the number of real actions
    if(grammar.getProduction(p).m_actionBody.isDefined()) {
      actionCount++;
    }
  }

  if(actionCount == 0) {
    return; // dont generate any switch
  }

  String text;
  for(int p = 0; p < productionCount; p++) {
    const Production &prod = grammar.getProduction(p);
    if(!prod.m_actionBody.isDefined()) {
      continue;
    }
    writer.printf(_T("case %d: /* %s */\n"), p, prod.toString().cstr());
    writer.incrLeftMargin(2);
    writer.writeSourceText(prod.m_actionBody);
    if(options.m_generateBreaks) {
      writer.printf(_T("break;\n"));
    }
    writer.decrLeftMargin(2);
  }
}
