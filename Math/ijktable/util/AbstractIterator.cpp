#include "stdafx.h"
#include "Iterator.h"

void AbstractIterator::noNextElementError(const char *className) const {
  throwMethodException(className, "next", "No such element");
}

void AbstractIterator::concurrentModificationError(const char *className) const {
  throwMethodException(className, "next", "Concurrent modification");
}

void AbstractIterator::noCurrentElementError(const char *className) const {
  throwMethodException(className, "remove", "No current element");
}

void AbstractIterator::unsupportedOperationError(const char *method) const {
  throwUnsupportedOperationException(method);
}

const char *_compactArrayIteratorClassName = "CompactArrayIterator";
const char *_compactArrayClassName         = "CompactArray";
