#include "pch.h"

#include <Iterator.h>

void AbstractIterator::noNextElementError(const TCHAR *method) {          // static
  throwException(_T("%s:No such element"), method);
}

void AbstractIterator::concurrentModificationError(const TCHAR *method) { // static
  throwException(_T("%s:Concurrent modification"),method);
}

void AbstractIterator::noCurrentElementError(const TCHAR *method) {       // static
  throwException(_T("%s:No current element"),method);
}

void AbstractIterator::unsupportedOperationError(const TCHAR *method) {   // static
  throwUnsupportedOperationException(method);
}
