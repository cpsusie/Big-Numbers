#include "pch.h"
#include <DebugLog.h>
#include <CompactArray.h>
#include <Iterator.h>
#include <StreamParameters.h>

using namespace std;

class AllFormatFlagsArray : public CompactArray<FormatFlags> {
private:
  static inline bool hasMany1Bits(FormatFlags flags, FormatFlags field) {
    const int m = flags & field;
    return (m & -m) != m; // i.e., n is a power of 2
  }
  static bool isMultiBitFieldAllowed(FormatFlags flags, UINT multibitFieldsFilter) {
    return ((multibitFieldsFilter & ALLOWMANY_ADJUSTFIELDBITS) || !hasMany1Bits(flags, ios::adjustfield))
        && ((multibitFieldsFilter & ALLOWMANY_BASEFIELDBITS  ) || !hasMany1Bits(flags, ios::basefield  ))
        && ((multibitFieldsFilter & ALLOWMANY_FLOATFIELDBITS ) || !hasMany1Bits(flags, ios::floatfield ));
  }
public:
  // Array will only contain flag-combinations with 1-bits where mask has a 1-bit
  AllFormatFlagsArray(FormatFlags lowMask, FormatFlags highMask, UINT multibitFieldsFilter);
  void dumpAllFormats() const;
};

AllFormatFlagsArray::AllFormatFlagsArray(FormatFlags lowMask, FormatFlags highMask, UINT multibitFieldsFilter) {
  if((highMask | lowMask) != highMask) { // (BitSet)lowMask should be a subset of (BitSet)highmask, <=> 1-bits in lowMask must be 1-bits in highMask too
    throwInvalidArgumentException(__TFUNCTION__, _T("lowMask=%08X, highMask=%08X"), lowMask, highMask);
  }
  for(UINT flags = 0; flags <= ios::_Fmtmask; flags++) {
    if((((flags|lowMask)&highMask)==flags) && isMultiBitFieldAllowed(flags, multibitFieldsFilter)) {
      add(flags);
    }
  }
}

void AllFormatFlagsArray::dumpAllFormats() const {
  const size_t n = size();
  debugLog(_T("%s\n"), __TFUNCTION__);
  for(size_t i = 0; i < n; i++) {
    debugLog(_T("%5zu:[%s]\n"), i, StreamParameters::flagsToString((*this)[i]).cstr());
  }
}

class AbstractStreamParametersIterator : public AbstractIterator {
private:
  DECLARECLASSNAME;
  const StreamSizeInterval         m_widthInterval, m_precisionInterval;
  const AllFormatFlagsArray        m_formatFlagsArray;
  const FormatFlags               *m_firstFormatFlags, *m_lastformatFlags;
  const String                     m_fillerString;
  const TCHAR                     *m_firstFillChar   , *m_lastFillChar;
  StreamSize                       m_width           ,  m_precision;
  const FormatFlags               *m_formatFlags;
  const TCHAR                     *m_fillChar;
  bool                             m_hasNext;
  StreamParameters                 m_value;

  inline void resetWidth()       { m_width       = m_widthInterval.getFrom();     }
  inline void resetPrecision()   { m_precision   = m_precisionInterval.getFrom(); }
  inline void resetFormatFlags() { m_formatFlags = m_firstFormatFlags;            }
  inline void resetFillChar()    { m_fillChar    = m_firstFillChar;               }

  inline void nextWidth()        { m_width++;       }
  inline void nextPrecision()    { m_precision++;   }
  inline void nextFormatFlags()  { m_formatFlags++; }
  inline void nextFillChar()     { m_fillChar++;    }

  inline bool hasNextWidth()       const { return m_width       < m_widthInterval.getTo();     }
  inline bool hasNextPrecision()   const { return m_precision   < m_precisionInterval.getTo(); }
  inline bool hasNextFormatFlags() const { return m_formatFlags < m_lastformatFlags;           }
  inline bool hasNextFillChar()    const { return m_fillChar    < m_lastFillChar;              }
  AbstractStreamParametersIterator(const AbstractStreamParametersIterator &src);
  AbstractStreamParametersIterator &operator=(const AbstractStreamParametersIterator&); // not implemented
  inline void init() {
    m_firstFormatFlags = &m_formatFlagsArray.first();
    m_lastformatFlags  = &m_formatFlagsArray.last();
    m_firstFillChar    = m_fillerString.cstr();
    m_lastFillChar     = m_firstFillChar + m_fillerString.length() - 1;
  }
  void copyCurrent(const AbstractStreamParametersIterator &it);
  static inline String getMinimalFillerString(const TCHAR *s) { // will return string with length >= 1. if s==NULL or empty string, return " " (1 space)
    const String result = s ? s : _T(" ");
    return result.isEmpty() ? _T(" ") : result;
  }

public:
  AbstractStreamParametersIterator(const StreamSizeInterval &widthInterval, const StreamSizeInterval &precisionInterval, FormatFlags lowMask, FormatFlags highMask, UINT multibitFieldsFilter, const TCHAR *fillers);
  void reset();
  AbstractIterator *clone()   override {
    return new AbstractStreamParametersIterator(*this);
  }
  inline bool hasNext() const override {
    return m_hasNext;
  }
  void *next()                override;
  void remove()               override {
    unsupportedOperationError(__TFUNCTION__);
  }
  inline size_t getMaxIterationCount() const {
    return ((intptr_t)m_widthInterval.getLength()+1) * ((intptr_t)m_precisionInterval.getLength()+1) * m_formatFlagsArray.size() * m_fillerString.length();
  }
  void dumpAllFormats() const {
    m_formatFlagsArray.dumpAllFormats();
  }
};

DEFINECLASSNAME(AbstractStreamParametersIterator);

AbstractStreamParametersIterator::AbstractStreamParametersIterator(const AbstractStreamParametersIterator &src)
: m_widthInterval(    src.m_widthInterval    )
, m_precisionInterval(src.m_precisionInterval)
, m_formatFlagsArray( src.m_formatFlagsArray )
, m_fillerString(     src.m_fillerString     )
{
  init();
  copyCurrent(src);
}

void AbstractStreamParametersIterator::copyCurrent(const AbstractStreamParametersIterator &it) {
  m_width       = it.m_width;
  m_precision   = it.m_precision;
  m_formatFlags = m_firstFormatFlags + (it.m_formatFlags - it.m_firstFormatFlags);
  m_fillChar    = m_firstFillChar    + (it.m_fillChar    - it.m_firstFillChar   );
  m_hasNext     = it.m_hasNext;
}

AbstractStreamParametersIterator::AbstractStreamParametersIterator(const StreamSizeInterval &widthInterval, const StreamSizeInterval &precisionInterval, FormatFlags lowMask, FormatFlags highMask, UINT multibitFieldsFilter, const TCHAR *fillers)
: m_widthInterval(    widthInterval    )
, m_precisionInterval(precisionInterval)
, m_formatFlagsArray( lowMask, highMask, multibitFieldsFilter)
, m_fillerString(     getMinimalFillerString(fillers))
{
  init();
  reset();
}

void AbstractStreamParametersIterator::reset() {
  resetWidth();
  resetPrecision();
  resetFormatFlags();
  resetFillChar();
  m_hasNext = true;
}

void *AbstractStreamParametersIterator::next() {
  if(!m_hasNext) noNextElementError(s_className);
  m_value = StreamParameters(m_precision, m_width, *m_formatFlags, *m_fillChar);
  if(hasNextFormatFlags()) {
    nextFormatFlags();
  } else {
    resetFormatFlags();
    if(hasNextFillChar()) {
      nextFillChar();
    } else {
      resetFillChar();
      if(hasNextPrecision()) {
        nextPrecision();
      } else {
        resetPrecision();
        if(hasNextWidth()) {
          nextWidth();
        } else {
          m_hasNext = false;
        }
      }
    }
  }
  return &m_value;
}

StreamParametersIterator::StreamParametersIterator(const StreamSizeInterval &widthInterval, const StreamSizeInterval &precisionInterval, FormatFlags lowMask, FormatFlags highMask, UINT multibitFieldsFilter, const TCHAR *fillers)
  : Iterator<StreamParameters>(new AbstractStreamParametersIterator(widthInterval, precisionInterval, lowMask, highMask, multibitFieldsFilter, fillers))
{
}

void StreamParametersIterator::reset() { // start all over
  ((AbstractStreamParametersIterator*)m_it)->reset();
}

size_t StreamParametersIterator::getMaxIterationCount() const {
  return ((AbstractStreamParametersIterator*)m_it)->getMaxIterationCount();
}

void StreamParametersIterator::dumpAllFormats() const {
  ((AbstractStreamParametersIterator*)m_it)->dumpAllFormats();
}

StreamParametersIterator StreamParameters::getIterator(const StreamSizeInterval &widthInterval, const StreamSizeInterval &precisionInterval, FormatFlags lowMask, FormatFlags highMask, UINT multibitFieldsFilter, const TCHAR *fillers) {
  return StreamParametersIterator(widthInterval, precisionInterval, lowMask, highMask, multibitFieldsFilter, fillers);
}

StreamParametersIterator StreamParameters::getFloatParamIterator(StreamSize maxWidth, const StreamSizeInterval &precisionInterval, FormatFlags lowMask, FormatFlags highMask, UINT multibitFieldsFilter, const TCHAR *fillers) {
  return getIterator(StreamSizeInterval(0, maxWidth), precisionInterval, lowMask, highMask, multibitFieldsFilter, fillers);
}

StreamParametersIterator StreamParameters::getFloatParamIterator(StreamSize maxWidth, StreamSize maxPrecision, FormatFlags lowMask, FormatFlags highMask, UINT multibitFieldsFilter, const TCHAR *fillers) {
  return getFloatParamIterator(maxWidth, StreamSizeInterval(0, maxPrecision), lowMask, highMask, multibitFieldsFilter, fillers);
}

StreamParametersIterator StreamParameters::getIntParamIterator(const StreamSizeInterval &widthInterval, FormatFlags lowMask, FormatFlags highMask, UINT multibitFieldsFilter, const TCHAR *fillers) {
  return getIterator(widthInterval, StreamSizeInterval(0, 0), lowMask, highMask, multibitFieldsFilter, fillers);
}

StreamParametersIterator StreamParameters::getIntParamIterator(StreamSize maxWidth, FormatFlags lowMask, FormatFlags highMask, UINT multibitFieldsFilter, const TCHAR *fillers) {
  return getIntParamIterator(StreamSizeInterval(0, maxWidth), lowMask, highMask, multibitFieldsFilter, fillers);
}
