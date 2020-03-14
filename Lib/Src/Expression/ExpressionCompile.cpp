#include "pch.h"
#include <Math/Expression/Expression.h>
#include <Math/Expression/ExpressionNode.h>
#include <Math/Expression/ParserTree.h>
#include "CodeGenerator.h"
#include "MachineCode.h"

namespace Expr {

bool Expression::compile(const String &expr, StringArray &errors, bool machineCode, bool optimize, FILE *listFile) {
  errors.clear();
  clear();
  m_ok = true;
  try {
    new ParserTree(this, expr); TRACE_NEW(m_tree);
    if(!m_tree->isOk()) {
      errors = m_tree->getErrors();
      if(listFile) {
        m_tree->listErrors(listFile);
      }
      clear();
      return m_ok;
    }
    setReturnType(m_tree->getRoot()->getReturnType());
    m_listFile = listFile;
    if(m_listFile) {
      ListFile::printComment(m_listFile, _T("%s\n\n"), expr.cstr());
      ListFile::printComment(m_listFile, _T("%s\n"  ), m_tree->toString().cstr());
    }
    if(optimize) {
      m_tree->reduce();
      if(m_listFile) {
        ListFile::printComment(m_listFile, _T("Reduced\n"));
        ListFile::printComment(m_listFile, _T("%s\n\n"), m_tree->toString().cstr());
      }
    }
    if(machineCode) {
/*
      setTreeForm(TREEFORM_NUMERIC );
      setTreeForm(TREEFORM_STANDARD);
      if(m_listFile) {
        ListFile::printComment(m_listFile, _T("Optimized\n"));
        ListFile::printComment(m_listFile, _T("%s\n"), toString().cstr());
        ListFile::printComment(m_listFile, _T("%s\n\n"), getSymbolTable().toString().cstr());
      }
*/
      setMachineCode(true);

      if(m_listFile) {
        ListFile::printComment(m_listFile, _T("%s\n"), ListFile::makeSkillLineString().cstr());
        m_listFile = NULL;
      }
    }
  } catch(Exception e) {
    m_ok = false;
    errors.add(e.what());
    if(listFile) {
      _ftprintf(listFile, _T("Exception:%s\n"), e.what());
    }
    m_listFile = NULL;
  } catch(...) {
    m_listFile = NULL;
    throw;
  }
  return m_ok;
}

void Expression::setMachineCode(bool machinecode) {
  if(machinecode != isMachineCode()) {
    if(machinecode) {
      genMachineCode();
    } else {
      clearMachineCode();
    }
    setProperty(EP_MACHINECODE, m_machineCode, machinecode);
  }
  updateEvalPointers();
}

Real Expression::evalRealFast() const {
  return m_code->evaluateReal();
}
bool Expression::evalBoolFast() const {
  return m_code->evaluateBool();
}

void Expression::genMachineCode() {
  clearMachineCode();
  m_code = CodeGenerator(m_tree, m_listFile).getCode();
}

void Expression::clearMachineCode() {
  SAFEDELETE(m_code);
}

}; // namespace Expr
