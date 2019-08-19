#pragma once

#include <BasicIncludes.h>
#include <StreamParameters.h>
#include <CompactArray.h>
#include <Iterator.h>

String toString1(FormatFlags flags);
String toString(const StreamParameters &param);

class AllFormatFlagsArray : public CompactArray<FormatFlags> {
public:
  AllFormatFlagsArray();
};

class AbstractStreamParametersIterator : public AbstractIterator {
private:
  DECLARECLASSNAME;
  static const AllFormatFlagsArray s_flagArray;
  static const size_t              s_maxFlagIndex;
  StreamSize                       m_nextWidth, m_nextPrecision;
  size_t                           m_nextFlagIndex;
  StreamSize                       m_maxWidth, m_maxPrecision;
  bool                             m_hasNext;
  StreamParameters                 m_value;

  inline void resetWidth()              { m_nextWidth     = 0; }
  inline void resetPrecision()          { m_nextPrecision = 1; }
  inline void resetFlagIndex()          { m_nextFlagIndex = 0; }

  inline void nextWidth()               { m_nextWidth++;       }
  inline void nextPrecision()           { m_nextPrecision++;   }
  inline void nextFlagIndex()           { m_nextFlagIndex++;   }

  inline bool hasNextWidth()      const { return m_nextWidth     < m_maxWidth;     }
  inline bool hasNextPrecision()  const { return m_nextPrecision < m_maxPrecision; }
  inline bool hasNextFlagIndex()  const { return m_nextFlagIndex < s_maxFlagIndex; }
  AbstractStreamParametersIterator(const AbstractStreamParametersIterator &src) {
    *this = src;
  }
public:
  AbstractStreamParametersIterator(StreamSize maxWidth, StreamSize maxPrecision);
  AbstractIterator *clone() {
    return new AbstractStreamParametersIterator(*this);
  }
  inline bool hasNext() const {
    return m_hasNext;
  }
  void *next();
  void remove() {
    unsupportedOperationError(__TFUNCTION__);
  }
};

inline Iterator<StreamParameters> createStreamParametersIterator(StreamSize maxWidth, StreamSize maxPrecision) {
  return Iterator<StreamParameters>(new AbstractStreamParametersIterator(maxWidth, maxPrecision));
};
