#pragma once

#include <PersistentDataTemplate.h>

typedef int FunctionDomainRangeType;

#define FUNCTIONDOMAINRANGE(DomainDim,RangeDim)  ((FunctionDomainRangeType)(((DomainDim)<<8) | (RangeDim)))

class FunctionImageParamPersistentData : public PersistentDataTemplate<FunctionDomainRangeType> {
};
