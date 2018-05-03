#include "pch.h"
#include "ListFile.h"

// --------------------------------- ListFile functions ----------------------------

void ListFile::adjustPositions(UINT pos, UINT bytesAdded) {
  const size_t n = m_lineArray.size();
  for(size_t i = 0; i < n; i++) {
    ListLine *l = m_lineArray[i];
    if(l->m_pos >= pos) l->m_pos += bytesAdded;
  }
}

void ListFile::clear() {
  const size_t n = m_lineArray.size();
  for(size_t i = 0; i < n; i++) {
    SAFEDELETE(m_lineArray[i]);
  }
  m_lineArray.clear();
}

static int listFileLineCmp(ListLine * const &l1, ListLine * const &l2) {
  return (int)l1->m_pos - (int)l2->m_pos;
}

void ListFile::flush() {
  if(!isOpen()) return;
  m_lineArray.sort(listFileLineCmp);
  const size_t n = m_lineArray.size();
  for(size_t i = 0; i < n; i++) {
    const ListLine *l = m_lineArray[i];
    _ftprintf(m_f, _T("%s\n"), l->toString().cstr());
  }
}

void ListFile::vprintf(const TCHAR *format, va_list argptr) const {
  _vftprintf(m_f, format, argptr);
}

const TCHAR *ListFile::findArgComment(const InstructionOperand &arg) const {
  if(arg.isMemoryRef()) {
    const MemoryRef &mr = arg.getMemoryReference();
    if(mr.hasBase() && (mr.getBase() == &m_tableRefRegister)) {
      const UINT index = m_addressTable.esiOffsetToIndex(mr.getOffset());
      if(index < m_nameCommentArray.size()) {
        return m_nameCommentArray[index].cstr();
      }
    }
  } else if(arg.isImmediateValue() && (arg.getSize() == REGSIZE_DWORD)) {
    const size_t index = m_addressTable.realRefToIndex((Real*)(size_t)arg.getImmUint32());
    if(index < m_nameCommentArray.size()) {
      return m_nameCommentArray[index].cstr();
    }
  }
  return NULL;
}

ListLine *ListFile::findLineByPos(UINT pos) {
  const size_t n = m_lineArray.size();
  for(size_t i = 0; i < n; i++) {
    ListLine *line = m_lineArray[i];
    if(line->m_pos == pos) {
      return line;
    }
  }
  return NULL;
}
