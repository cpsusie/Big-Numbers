#include "stdafx.h"
#include "Iterator.h"

void AbstractIterator::noNextElementError(const char *method) const {
  throwException("%s:No such element",method);
}

void AbstractIterator::concurrentModificationError(const char *method) const {
  throwException("%s:Concurrent modification", method);
}

void AbstractIterator::noCurrentElementError(const char *method) const {
  throwException("%s:No current element", method);
}

void AbstractIterator::unsupportedOperationError(const char *method) const {
  throwUnsupportedOperationException(method, method);
}
