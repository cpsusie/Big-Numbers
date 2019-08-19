#pragma once

#include <BasicIncludes.h>
#include <StreamParameters.h>
#include <CompactArray.h>
#include <Iterator.h>

class AllFormatFlagsArray : public CompactArray<FormatFlags> {
public:
  AllFormatFlagsArray();
};

class AbstractStreamParametersIterator : public AbstractIterator {
private:
  DECLARECLASSNAME;
  static const AllFormatFlagsArray s_flagArray;
  const size_t                     m_maxWidth, m_maxPrecision;
  static const size_t              s_maxFlagIndex;
  size_t                           m_nextWidth, m_nextPrecision, m_nextFlagIndex;
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
public:
  AbstractStreamParametersIterator(StreamSize maxWidth, StreamSize maxPrecision);
  void reset();
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
  inline size_t getMaxIterationCount() const {
    return (m_maxWidth+1) * m_maxPrecision * (s_maxFlagIndex+1);
  }
};

class StreamParametersIterator : public Iterator<StreamParameters> {
public:
  StreamParametersIterator(StreamSize maxWidth, StreamSize maxPrecision)
    : Iterator<StreamParameters>(new AbstractStreamParametersIterator(maxWidth, maxPrecision))
  {
  }
  inline void reset() { // start all over
    ((AbstractStreamParametersIterator*)m_it)->reset();
  }
  inline size_t getMaxIterationCount() const {
    return ((AbstractStreamParametersIterator*)m_it)->getMaxIterationCount();
  }
};

inline StreamParametersIterator createStreamParametersIterator(StreamSize maxWidth, StreamSize maxPrecision) {
  return StreamParametersIterator(maxWidth, maxPrecision);
}
