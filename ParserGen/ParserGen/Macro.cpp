#include "stdafx.h"
#include "GrammarCode.h"
#include "Macro.h"

void Macro::print(MarginFile &f) const {
  f.printf(_T("#define %s %-10s /* %-*s*/\n")
          ,getName().cstr()
          ,getValue().cstr()
          ,commentWidth
          ,getComment().cstr()
          );
}

String Macro::getComment() const {
  if((getStateSetSize() == 1) && m_stateSet.contains(m_index)) {
    return m_comment;
  }
  return format(_T("%s %s")
               ,m_comment.cstr()
               ,getStateSetComment(getStateSet()).cstr()
               );
}
