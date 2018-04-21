#pragma once

class MachineCodeWrapper {
public:
  const void *m_code;
  MachineCodeWrapper(const void *code) : m_code(code) {
  }
  ~MachineCodeWrapper();
  Real   evaluateReal() const;
  bool   evaluateBool() const;
};

class CodeGeneratorWrapper {
  MachineCodeWrapper *m_code;
public:
  CodeGeneratorWrapper(ParserTree *tree, TrigonometricMode trigonometricMode);
  const MachineCodeWrapper *getCode() const {
    return m_code;
  }
};