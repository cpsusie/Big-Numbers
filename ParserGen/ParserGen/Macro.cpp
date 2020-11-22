#include "stdafx.h"
#include "GrammarCode.h"
#include "Macro.h"

void Macro::print(MarginFile &f, UINT commentWidth1) const {
  f.printf(_T("#define %s %-10s /* %-*s*/\n")
          ,getName().cstr()
          ,getValue().cstr()
          ,commentWidth
          ,getComment(true, commentWidth1).cstr()
          );
}

String Macro::getComment(bool includeUsedBy, UINT commentWidth1) const {
  if(!includeUsedBy || ((getUsedByCount() == 1) && m_usedBySet.contains(m_index))) {
    return commentWidth1 ? format(_T("%-*s"), commentWidth1, m_comment.cstr()) : m_comment;
  }
  return format(_T("%-*s %s")
               ,commentWidth1
               ,m_comment.cstr()
               ,getUsedBySet().toString().cstr()
               );
}
