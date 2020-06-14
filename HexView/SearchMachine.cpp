#include "stdafx.h"
#include <BMAutomate.h>

SequentialByteContainer::SequentialByteContainer(ByteContainer &bc)
: m_bc(bc)
, m_size(bc.getSize())
{
  m_position = 0;
}

int SequentialByteContainer::fread(UINT count, BYTE *buffer) { // return number of bytes read
  ByteArray ba;
  m_bc.getBytes(m_position, count, ba);
  UINT got = (UINT)ba.size();
  if(got > 0) {
    if(got > count) {
      got = count;
    }
    m_position += got;
    memcpy(buffer, ba.getData(), got);
  }
  return got;
}

void SequentialByteContainer::fseek(INT64 pos) {
  if((pos < 0) || (pos > m_size)) {
    throwInvalidArgumentException(__TFUNCTION__, _T("pos=%I64d. size=%I64d"), pos, m_size);
  }
  m_position = pos;
}

SearchMachine::SearchMachine() {
  m_byteContainer = NULL;
  m_size          = 0;
  m_fileIndex     = 0;
  m_maxProgress   = 10;
}

void SearchMachine::prepareSearch(const SearchParameters &param, ByteContainer *byteContainer) {
  prepareSearch(param.m_forwardSearch, param.m_startPosition, param.m_findWhat, byteContainer);
}

void SearchMachine::prepareSearch(bool forwardSearch, INT64 startPosition, const String &findWhat, ByteContainer *byteContainer) {
  clearAllFlags();
  if(byteContainer != NULL) {
    m_byteContainer = byteContainer;
  }
  if(m_byteContainer == NULL) {
    throwException(_T("%s:ByteContainer not set"), __TFUNCTION__);
  }
  if(findWhat.length() > 0) {
    m_searchParam.m_findWhat = findWhat;
  }
  m_searchParam.m_forwardSearch = forwardSearch;
  if(startPosition >= 0) {
    m_searchParam.m_startPosition = startPosition;
  }
  m_size          = m_byteContainer->getSize();
  m_result.clear();

  m_maxProgress = forwardSearch ? (m_size - getStartPosition()) : getStartPosition();
}

UINT SearchMachine::safeRun() {
  if((m_result = doSearch()).isEmpty()) {
    const String msg = isInterrupted()
                     ? _T("Interrupted by user")
                     : format(_T("Bytesequence <%s> not found"), SearchPattern(getFindWhat()).toString().cstr());
    die(msg.cstr());
  }
  return 0;
}

double SearchMachine::getProgress() const {
  if(isForwardSearch()) {
    return (double)(m_fileIndex - getStartPosition());
  } else {
    return (double)(getStartPosition() - m_fileIndex);
  }
}

AddrRange SearchMachine::doSearch() {
  if(!isSet() || (getStartPosition() < 0) || (getStartPosition() >= m_size)) {
    return AddrRange();
  }

  SearchPattern pattern(getFindWhat());
  SequentialByteContainer sbc(*m_byteContainer);

  const size_t patternLength = pattern.size();
  BMAutomateBYTE bmSearch(pattern.getData(), patternLength, isForwardSearch());
  if(isForwardSearch()) {
    sbc.fseek(getStartPosition());
    size_t headSize = 0;
    BYTE   buffer[0x10000];
    for(m_fileIndex = getStartPosition(); !isInterrupted();) {
      const int bytesRead = sbc.fread(sizeof(buffer) - (int)headSize, buffer + headSize);
      if(bytesRead == 0) {
        break;
      }
      const intptr_t bufferIndex = bmSearch.search(buffer, bytesRead + headSize);
      if(bufferIndex >= 0) {
        const INT64 foundIndex = m_fileIndex + bufferIndex - headSize;
        return AddrRange(foundIndex, foundIndex + patternLength - 1);
      }
      m_fileIndex += bytesRead;
      const size_t newHeadSize = min(patternLength, (size_t)bytesRead);
      // move tail to the start of buffer, so they next buffer will be concatenated to them
      memmove(buffer, buffer+headSize+bytesRead-newHeadSize, newHeadSize);
      headSize = newHeadSize;
    }
  } else {
    intptr_t tailSize = 0;
    BYTE buffer[0x10000];
    bool BOF = false; // beginning_of_file
    for(INT64 addr = getStartPosition() + patternLength-1; addr >= (INT64)patternLength && !isInterrupted();) { // could be a match begining at m_startPosition-1
      const intptr_t bufferCapacity = ARRAYSIZE(buffer) - tailSize;
                     m_fileIndex    = addr - bufferCapacity;
      intptr_t       bytesNeeded    = bufferCapacity;
      if(m_fileIndex < 0) {
        bytesNeeded += (int)m_fileIndex;
        m_fileIndex = 0;
        BOF         = true;
      }
      if(tailSize > 0) {
        // move head to end of buffer, so they can be concatenated with the next databuffer
        memmove(buffer + bytesNeeded, buffer, tailSize);
      }
      sbc.fseek(m_fileIndex);
      const int bytesRead = sbc.fread((UINT)bytesNeeded, buffer);
      if((bytesRead < bytesNeeded) && (tailSize > 0)) {
        // There's a gap between the read bytes and the tail. move tail bytes to fill the gap
        memmove(buffer + bytesRead, buffer + bytesNeeded, tailSize);
      }
      const intptr_t bufferIndex = bmSearch.search(buffer, bytesRead + tailSize);
      if(bufferIndex >= 0) {
        const INT64 foundIndex = m_fileIndex + bufferIndex;
        return AddrRange(foundIndex, foundIndex + patternLength - 1);
      }
      if(BOF) {
        break;
      }
      addr -= bufferCapacity;
      tailSize = min((intptr_t)patternLength, bytesRead);
    }
  }
  return AddrRange();
}

int SearchMachine::getPatternLength() const {
  return (int)(SearchPattern(getFindWhat()).size());
}

SearchPattern::SearchPattern(const String &pattern) {
  convert(pattern);
}

SearchPattern &SearchPattern::operator=(const String &pattern) {
  return convert(pattern);
}

SearchPattern &SearchPattern::convert(const String &pattern) {
  const TCHAR *cp = pattern.cstr();
  clear();
  while(*cp) {
    add(escape(cp));
  }
  return *this;
}

String SearchPattern::toString() const {
  String result;
  String byteFormat = getSettings().getRadixFormat();
  if(byteFormat[byteFormat.length()-1] = ' ') {
    byteFormat.remove(byteFormat.length()-1);
  }
  byteFormat = _T("%s") + byteFormat;
  const TCHAR *form      = byteFormat.cstr();
  const TCHAR *delimiter = EMPTYSTRING;
  for(size_t i = 0; i < size(); i++, delimiter = _T(" ")) {
    result += format(form, delimiter, (*this)[i]);
  }
  return result;
}
