#include "stdafx.h"
#include "StreamParametersIterator.h"

using namespace std;

DEFINECLASSNAME(AbstractStreamParametersIterator);

const AllFormatFlagsArray AbstractStreamParametersIterator::s_flagArray;
const size_t              AbstractStreamParametersIterator::s_maxFlagIndex = AbstractStreamParametersIterator::s_flagArray.size()-1;

AbstractStreamParametersIterator::AbstractStreamParametersIterator(StreamSize maxWidth, StreamSize maxPrecision)
: m_maxWidth(maxWidth)
, m_maxPrecision(maxPrecision)
{
  resetWidth();
  resetPrecision();
  resetFlagIndex();
  m_hasNext = true;
}

void *AbstractStreamParametersIterator::next() {
  if(!m_hasNext) noNextElementError(s_className);
  m_value = StreamParameters(m_nextPrecision, m_nextWidth, s_flagArray[m_nextFlagIndex]);
  if(hasNextFlagIndex()) {
    nextFlagIndex();
  } else {
    resetFlagIndex();
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
  return &m_value;
}

String toString1(FormatFlags flags) {
  String result;
  result =  (flags & ios::showpos   ) ? "+" : " ";
  result += (flags & ios::showpoint ) ? "." : " ";
  result += (flags & ios::left      ) ? "l" : " ";
  result += (flags & ios::right     ) ? "r" : " ";
  result += (flags & ios::scientific) ? (flags & ios::uppercase) ? "E" : "e" : " ";
  result += (flags & ios::fixed     ) ? "f" : " ";
  return result;
}

String toString(const StreamParameters &param) {
 return toString1(param.getFlags()) + format(_T(" %2d.%2d"), param.getWidth(), param.getPrecision());
}

AllFormatFlagsArray::AllFormatFlagsArray() {
  add(0);
  for(FormatFlags flags = 0;;) {
    if(!(flags & ios::uppercase)) {
      flags |= ios::uppercase;
    } else {
      flags &= ~ios::uppercase;
      if(!(flags & ios::floatfield)) {
        flags |= ios::fixed;
      } else {
        if(flags & ios::fixed) {
          flags &= ~ios::fixed;
          flags |= ios::scientific;
        } else {
          flags &= ~ios::scientific;
          if(!(flags & ios::left)) {
            flags |= ios::left;
          } else {
            flags &= ~ios::left;
            if(!(flags & ios::right)) {
              flags |= ios::right;
            } else {
              flags &= ~ios::right;
              if(!(flags & ios::showpoint)) {
                flags |= ios::showpoint;
              } else {
                flags &= ~ios::showpoint;
                if(!(flags & ios::showpos)) {
                  flags |= ios::showpos;
                } else {
                  break;
                }
              }
            }
          }
        }
      }
    }
    add(flags);
  }
}
