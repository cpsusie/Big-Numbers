#include "stdafx.h"
#include <MarginFile.h>
#include "GrammarCode.h"
#include "Macro.h"

Macro::Macro(const BitSetParameters &usedByParam, UINT usedByV0, const String &value, const String &comment)
  : m_usedBySet(usedByParam)
  , m_usedByCount(0        )
  , m_index(  -1           )
  , m_value(  value        )
  , m_comment(comment      )
{
  addUsedByValue(usedByV0);
}

void Macro::print(MarginFile &f, UINT commentWidth1, const AbstractSymbolNameContainer *nameContainer) const {
  f.printf(_T("#define %s %-10s /* %-*s*/\n")
          ,getName().cstr()
          ,getValue().cstr()
          ,commentWidth
          ,getComment(true, commentWidth1, nameContainer).cstr()
          );
}

String Macro::getComment(bool includeUsedBy, UINT commentWidth1, const AbstractSymbolNameContainer *nameContainer) const {
  if(!includeUsedBy || ((getUsedByCount() == 1) && m_usedBySet.contains(m_index))) {
    return commentWidth1 ? format(_T("%-*s"), commentWidth1, m_comment.cstr()) : m_comment;
  }
  return format(_T("%-*s %s")
               ,commentWidth1
               ,m_comment.cstr()
               ,getUsedBySet().toString(nameContainer).cstr()
               );
}
