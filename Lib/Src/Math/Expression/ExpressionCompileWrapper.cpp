#include "pch.h"
#include <Math/Expression/Expression.h>
#include "ExpressionCompile.h"
#include "ExpressionCompileWrapper.h"

MachineCodeWrapper::~MachineCodeWrapper() {
  if(m_code) {
    delete (MachineCode*)m_code;
  }
}

Real MachineCodeWrapper::evaluateReal() const {
  return ((MachineCode*)m_code)->evaluateReal();
}

bool MachineCodeWrapper::evaluateBool() const {
  return ((MachineCode*)m_code)->evaluateBool();
}

CodeGeneratorWrapper::CodeGeneratorWrapper(ParserTree *tree, TrigonometricMode trigonometricMode) {
  m_code = new MachineCodeWrapper(CodeGenerator(tree, trigonometricMode).getCode());
}
