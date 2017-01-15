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

void SequentialByteContainer::fseek(__int64 pos) {
  if(pos < 0 || pos > m_size) {
    throwInvalidArgumentException(__TFUNCTION__, _T("pos=%I64d. size=%I64d"), pos, m_size);
  }
  m_position = pos;
}

SearchMachine::SearchMachine() {
  m_forwardSearch = true;
  m_startPosition = 0;
  m_byteContainer = NULL;
  m_finished      = true;
  m_size          = 0;
  m_fileIndex     = 0;
  m_maxProgress   = 10;
}

void SearchMachine::prepareSearch(bool forwardSearch, __int64 startPosition, const String &findWhat, ByteContainer *byteContainer) {
  clearAllFlags();
  if(byteContainer != NULL) {
    m_byteContainer = byteContainer;
  }
  if(m_byteContainer == NULL) {
    throwException(_T("%s:ByteContainer not set"), __TFUNCTION__);
  }
  if(findWhat.length() > 0) {
    m_findWhat = findWhat;
  }
  m_forwardSearch = forwardSearch;
  if(startPosition >= 0) {
    m_startPosition = startPosition;
  }
  m_size          = m_byteContainer->getSize();
  m_finished      = false;
  m_result.clear();
  m_resultMessage = _T("");

  m_maxProgress = forwardSearch ? (m_size - m_startPosition) : m_startPosition;
}

UINT SearchMachine::run() {
  try {
    if((m_result = doSearch()).isEmpty()) {
      m_resultMessage = isInterrupted()
                      ? _T("Interrupted by user") 
                      : format(_T("Bytesequence <%s> not found"), 
                                SearchPattern(m_findWhat).toString().cstr());
    }
  } catch(Exception e) {
    m_resultMessage = e.what();
  } catch(...) {
    m_resultMessage = _T("Unknown Exception");
  }
  m_finished = true;
  return 0;
}

double SearchMachine::getProgress() const {
  if(m_forwardSearch) {
    return (double)(m_fileIndex - m_startPosition);
  } else {
    return (double)(m_startPosition - m_fileIndex);
  }
}

AddrRange SearchMachine::doSearch() {
  if(!isSet() || (m_startPosition < 0) || (m_startPosition >= m_size)) {
    return AddrRange();
  }

  SearchPattern pattern(m_findWhat);
  SequentialByteContainer sbc(*m_byteContainer);

  const size_t patternLength = pattern.size();
  BMAutomateBYTE bmSearch(pattern.getData(), patternLength, m_forwardSearch);
  if(m_forwardSearch) {
    sbc.fseek(m_startPosition);
    size_t headSize = 0;
    BYTE   buffer[0x10000];
    for(m_fileIndex = m_startPosition; !isInterrupted();) {
      const int bytesRead = sbc.fread(sizeof(buffer) - (int)headSize, buffer + headSize);
      if(bytesRead == 0) {
        break;
      }
      const intptr_t bufferIndex = bmSearch.search(buffer, bytesRead + headSize);
      if(bufferIndex >= 0) {
        const __int64 foundIndex = m_fileIndex + bufferIndex - headSize;
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
    for(__int64 addr = m_startPosition + patternLength-1; addr >= (__int64)patternLength && !isInterrupted();) { // could be a match begining at m_startPosition-1
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
        const __int64 foundIndex = m_fileIndex + bufferIndex;
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
  return (int)SearchPattern(m_findWhat).size();
}

SearchPattern::SearchPattern(const String &pattern) {
  convert(pattern);
}

SearchPattern &SearchPattern::operator=(const String &pattern) {
  convert(pattern);
  return *this;
}

void SearchPattern::convert(const String &pattern) {
  const TCHAR *cp = pattern.cstr();
  clear();
  while(*cp) {
    append(escape(cp));
  }
}

String SearchPattern::toString() const {
  String result;
  String byteFormat = getSettings().getRadixFormat();
  if(byteFormat[byteFormat.length()-1] = ' ') {
    byteFormat.remove(byteFormat.length()-1);
  }
  byteFormat = _T("%s") + byteFormat;
  const TCHAR *form      = byteFormat.cstr();
  const TCHAR *delimiter = _T("");
  for(size_t i = 0; i < size(); i++, delimiter = _T(" ")) {
    result += format(form, delimiter, (*this)[i]);
  }
  return result;
}
