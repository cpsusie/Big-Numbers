#include "pch.h"

#include <Iterator.h>

void AbstractIterator::noNextElementError(const TCHAR *className) const {
  throwMethodException(className, _T("next"), _T("No such element"));
}

void AbstractIterator::concurrentModificationError(const TCHAR *className) const {
  throwMethodException(className, _T("next"), _T("Concurrent modification"));
}

void AbstractIterator::noCurrentElementError(const TCHAR *className) const {
  throwMethodException(className, _T("remove"), _T("No current element"));
}

void AbstractIterator::unsupportedOperationError(const TCHAR *method) const {
  throwUnsupportedOperationException(method);
}

const TCHAR *_compactArrayIteratorClassName = _T("CompactArrayIterator");
const TCHAR *_compactArrayClassName         = _T("CompactArray");
