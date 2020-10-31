#pragma once

#include <SourcePosition.h>

class SourceText {
public:
  SourcePositionWithName m_pos;
  String                 m_sourceText;
  // -1 indicates that text is empty
  SourceText() : m_pos(EMPTYSTRING, -1, 0) {
  }
  bool isDefined() const {
    return m_pos.getLineNumber() != -1;
  }
};
