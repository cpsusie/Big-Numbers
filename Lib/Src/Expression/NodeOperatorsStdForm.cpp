#include "pch.h"
#include <Math/Expression/ParserTree.h>

namespace Expr {

// ---------------------------------------- Operators for StandardForm ----------------------------------------------------
// Not using SUM and PRODUCT nodes

class NodeOperatorsStdForm : public NodeOperators {
public:
  ExpressionNode *minus(     ExpressionNode *n) const;
  ExpressionNode *reciprocal(ExpressionNode *n) const;
  ExpressionNode *sqr(       ExpressionNode *n) const;
  ExpressionNode *sqrt(      ExpressionNode *n) const;
  ExpressionNode *exp(       ExpressionNode *n) const;
  ExpressionNode *exp10(     ExpressionNode *n) const;
  ExpressionNode *exp2(      ExpressionNode *n) const;
  ExpressionNode *cot(       ExpressionNode *n) const;
  ExpressionNode *csc(       ExpressionNode *n) const;
  ExpressionNode *sec(       ExpressionNode *n) const;

  ExpressionNode *sum(       ExpressionNode *n1, ExpressionNode *n2) const;
  ExpressionNode *diff(      ExpressionNode *n1, ExpressionNode *n2) const;
  ExpressionNode *prod(      ExpressionNode *n1, ExpressionNode *n2) const;
  ExpressionNode *quot(      ExpressionNode *n1, ExpressionNode *n2) const;
  // Return a division node without reduction to rational. to be used with exponents ie sqrt(x^2) != x
  ExpressionNode *quot(      ParserTree *tree, INT64 num, INT64 den) const;
  ExpressionNode *mod(       ExpressionNode *n1, ExpressionNode *n2) const;
  ExpressionNode *power(     ExpressionNode *n1, ExpressionNode *n2) const;
  ExpressionNode *root(      ExpressionNode *n1, ExpressionNode *n2) const;
  ParserTreeForm  getTreeForm() const {
    return TREEFORM_STANDARD;
  }
  void checkTreeFormConsistent(const ParserTree *tree) const {
    tree->checkIsStandardForm();
  }
};

class NodeOperatorsStdNumForm : public NodeOperatorsStdForm {
public:
  ParserTreeForm  getTreeForm() const {
    return TREEFORM_NUMERIC;
  }
};

ExpressionNode *NodeOperatorsStdForm::minus(ExpressionNode *n) const {
  switch(n->getSymbol()) {
  case NUMBER:
    if(n->isZero()) {
      return n;
    } else if(n->isOne()) {
      return getMinusOne(n);
    } else if(n->isMinusOne()) {
      return getOne(n);
    } else {
      return numberExpr(n,-n->getNumber());
    }
  case MINUS:
    if(n->isUnaryMinus()) {
      return n->left();
    } else {
      return diff(n->right(), n->left());
    }
  default:
    return unaryExpr(MINUS, n);
  }
}

ExpressionNode *NodeOperatorsStdForm::reciprocal(ExpressionNode *n) const {
  switch(n->getSymbol()) {
  case NUMBER:
    { const Number &v = n->getNumber();
      if(v.isRational()) {
        return numberExpr(n,::reciprocal(v.getRationalValue()));
      }
    }
    break;

  case QUOT: return quot(    n->right(), n->left());
  case EXP : return funcExpr(EXP       , minus(n->left()));
  case POW : return power(   n->left() , minus(n->right()));
  case ROOT: return root(    n->left() , minus(n->right()));
  case COS : return funcExpr(SEC, n->left());
  case SIN : return funcExpr(CSC, n->left());
  case TAN : return funcExpr(COT, n->left());
  case COT : return funcExpr(TAN, n->left());
  case SEC : return funcExpr(COS, n->left());
  case CSC : return funcExpr(SIN, n->left());
  }
  return quot(getOne(n), n);
}

ExpressionNode *NodeOperatorsStdForm::sum(ExpressionNode *n1, ExpressionNode *n2) const {
  if(n1->isRational() && n2->isRational()) {
    return numberExpr(n1, n1->getRational() + n2->getRational());
  }
  if(n1->isZero()) {
    return n2;
  } else if(n2->isZero()) {
    return n1;
  } else if(n1->isUnaryMinus()) {
    return diff(n2, n1->left());
  } else if(n2->isUnaryMinus()) {
    return diff(n1, n2->left());
  } else {
    return binaryExpr(PLUS, n1, n2);
  }
}

ExpressionNode *NodeOperatorsStdForm::diff(ExpressionNode *n1, ExpressionNode *n2) const {
  if(n1->isRational() && n2->isRational()) {
    return numberExpr(n1, n1->getRational() - n2->getRational());
  }
  if(n2->isZero()) {
    return n1;
  } else if(n1->isZero()) {
    return minus(n2);
  } else if(n2->isUnaryMinus()) {
    return sum(n1, n2->left());
  } else {
    return binaryExpr(MINUS, n1, n2);
  }
}

ExpressionNode *NodeOperatorsStdForm::prod(ExpressionNode *n1, ExpressionNode *n2) const {
  if(n1->isRational() && n2->isRational()) {
    return numberExpr(n1, n1->getRational() * n2->getRational());
  }
  if(n1->isZero() || n2->isZero()) {
    return getZero(n1);
  } else if(n1->isOne()) {
    return n2;
  } else if(n2->isOne()) {
    return n1;
  } else if(n1->isMinusOne()) {
    return minus(n2);
  } else if(n2->isMinusOne()) {
    return minus(n1);
  } else {
    return binaryExpr(PROD, n1, n2);
  }
}

ExpressionNode *NodeOperatorsStdForm::quot(ExpressionNode *n1, ExpressionNode *n2) const {
  if(n1->isRational() && n2->isRational()) {
    return numberExpr(n1, n1->getRational() / n2->getRational());
  }
  if(n2->isOne()) {
    return n1;
  } else if(n2->isMinusOne()) {
    return minus(n1);
  } else if(n1->isZero()) {
    return n1;
  } else {
    return binaryExpr(QUOT, n1, n2);
  }
}

ExpressionNode *NodeOperatorsStdForm::quot(ParserTree *tree, INT64 num, INT64 den) const {
  ExpressionNode *q = binaryExpr(QUOT, tree->numberExpr(num), tree->numberExpr(den));
  q->setReduced();
  return q;
}

ExpressionNode *NodeOperatorsStdForm::mod(ExpressionNode *n1, ExpressionNode *n2) const {
  return binaryExpr(MOD, n1, n2);
}

ExpressionNode *NodeOperatorsStdForm::sqr(ExpressionNode *n) const {
  return funcExpr(SQR, n);
}

ExpressionNode *NodeOperatorsStdForm::sqrt(ExpressionNode *n) const {
  return funcExpr(SQRT, n);
}

ExpressionNode *NodeOperatorsStdForm::power(ExpressionNode *n1, ExpressionNode *n2) const {
  if(n1->isRational() && n2->isInteger()) {
    return numberExpr(n1, pow(n1->getRational(), n2->getNumber().getIntValue()));
  }
  if(n1->isEulersConstant()) {
    if(n2->isZero()) {
      return getOne(n1);
    } else if(n2->isOne()) {
      return n1;
    }
    return exp(n2);
  } else if(n1->getSymbol() == POW) {
    ParserTree &tree = n1->getTree();
    return tree.powerExpr(n1->left(), tree.multiplyExponents(n1->right(), n2));
  }

  if(n2->isNumber()) {
    const Number &e = n2->getNumber();
    if(e.isRational()) {
      const Rational eR = e.getRationalValue();
      if(eR.isZero()) {
        return getOne(n1);
      } else if(eR == 1) {
        return n1;
      } else if(eR == -1) {
        return reciprocal(n1);
      } else if(eR.getNumerator() == 1) {
        return root(n1, numberExpr(n1,eR.getDenominator()));
      } else if(eR.getNumerator() == -1) {
        return reciprocal(root(n1, numberExpr(n1,eR.getDenominator())));
      }
    }
  }
  return n1->getTree().powerExpr(n1, n2);
}

ExpressionNode *NodeOperatorsStdForm::root(ExpressionNode *n1, ExpressionNode *n2) const {
  if(n2->isOne()) {
    return n1;
  } else if(n2->isMinusOne()) {
    return reciprocal(n1);
  }
  return binaryExpr(ROOT, n1, n2);
}

ExpressionNode *NodeOperatorsStdForm::exp(ExpressionNode *n) const {
  return funcExpr(EXP, n);
}

ExpressionNode *NodeOperatorsStdForm::exp10(ExpressionNode *n) const {
  return funcExpr(EXP10, n);
}

ExpressionNode *NodeOperatorsStdForm::exp2(ExpressionNode *n) const {
  return funcExpr(EXP2, n);
}

ExpressionNode *NodeOperatorsStdForm::cot(ExpressionNode *n) const {
  return funcExpr(COT, n);
}

ExpressionNode *NodeOperatorsStdForm::csc(ExpressionNode *n) const {
  return funcExpr(CSC, n);
}

ExpressionNode *NodeOperatorsStdForm::sec(ExpressionNode *n) const {
  return funcExpr(SEC, n);
}

static const NodeOperatorsStdForm    stdFormOps;
static const NodeOperatorsStdNumForm stdNumFormOps;

const NodeOperators *NodeOperators::s_stdForm    = &stdFormOps;
const NodeOperators *NodeOperators::s_stdNumForm = &stdNumFormOps;

// ------------------------------- tree conversion classes/functions ------------------
// ----------------------------------------- toStandardForm ------------------------------------------------------------------

class StdNode : public SNode {
private:
  StdNode toSForm()         const;
  StdNode toSFormTreeNode() const;
  StdNode toSFormBoolExpr() const;
  StdNode toSFormPoly()     const;
  StdNode toSFormAssign()   const;
  StdNode toSFormStmtList() const;
  StdNode toSFormSum()      const;
  StdNode toSFormProduct()  const;
  StdNode toSFormPow()      const;
  StdNode toSFormFactorArray(FactorArray &a, bool changeExponentSign) const;
  // assume !n.isEmpty()
  inline StdNode(const SNode &n) : SNode(n) {
    CHECKISCONSISTENT(n);
  }
public:
  // assume n != NULL
  inline StdNode(ExpressionNode *n) : SNode(n) {
    CHECKISCONSISTENT(*n);
  }
  SNode convert() const {
    ENTERMETHOD();
    RETURNNODE(toSForm());
  }
};

SNode ParserTree::toStandardForm(SNode n) {
  if(getTreeForm() == TREEFORM_STANDARD) {
    return n;
  }
  STARTREDUCTION();
  m_ops = NodeOperators::s_stdForm;
  if(n.isEmpty()) {
    return n;
  } else {
    return StdNode(n.node()).convert();
  }
}

// Eliminate all product-,addent- and Sum nodes
StdNode StdNode::toSForm() const {
  ENTERMETHOD();
  switch(getNodeType()) {
  case NT_NUMBER     :
  case NT_BOOLCONST  :
  case NT_VARIABLE   : RETURNTHIS;
  case NT_TREE       : RETURNNODE( toSFormTreeNode() );
  case NT_POWER      : RETURNNODE( toSFormPow()      );
  case NT_BOOLEXPR   : RETURNNODE( toSFormBoolExpr() );
  case NT_POLY       : RETURNNODE( toSFormPoly()     );
  case NT_ASSIGN     : RETURNNODE( toSFormAssign()   );
  case NT_STMTLIST   : RETURNNODE( toSFormStmtList() );
  case NT_SUM        : RETURNNODE( toSFormSum()      );
  case NT_PRODUCT    : RETURNNODE( toSFormProduct()  );
  default            : throwUnknownNodeTypeException(__TFUNCTION__);
                       return *this;
  }
}

#define S(n)  StdNode(n)

StdNode StdNode::toSFormTreeNode() const {
  ENTERMETHOD();
  const SNodeArray &a = getChildArray();
  SNodeArray        newChildArray(a.getTree(),a.size());
  for(size_t i = 0; i < a.size(); i++) {
    newChildArray.add(S(a[i]).toSForm());
  }
  RETURNNODE( treeExp(getSymbol(), newChildArray) );
}

StdNode StdNode::toSFormBoolExpr() const {
  ENTERMETHOD();
  const SNodeArray &a = getChildArray();
  SNodeArray        newChildArray(a.getTree(),a.size());
  for(size_t i = 0; i < a.size(); i++) {
    newChildArray.add(S(a[i]).toSForm());
  }
  RETURNNODE( boolExp(getSymbol(), newChildArray) );
}

StdNode StdNode::toSFormPoly() const {
  ENTERMETHOD();
  const SNodeArray &coefArray = getCoefArray();
  StdNode           arg       = getArgument();

  SNodeArray newCoefArray(coefArray.getTree(),coefArray.size());
  for(size_t i = 0; i < coefArray.size(); i++) {
    newCoefArray.add(S(coefArray[i]).toSForm());
  }
  RETURNNODE( polyExp(newCoefArray, arg.toSForm()) );
}

StdNode StdNode::toSFormAssign() const {
  ENTERMETHOD();
  RETURNNODE( assignStmt(left(), S(right()).toSForm()) );
};

StdNode StdNode::toSFormStmtList() const {
  ENTERMETHOD();
  const SNodeArray &a = getChildArray();
  SNodeArray newChildArray(a.getTree(),a.size());
  for(size_t i = 0; i < a.size(); i++) {
    newChildArray.add(S(a[i]).toSForm());
  }
  RETURNNODE( stmtList(newChildArray) );
}

StdNode StdNode::toSFormSum() const {
  ENTERMETHOD();
  AddentArray a = getAddentArray();
  if(a.size() == 0) {
    RETURNNODE( _0() );
  } else {
    a.sortStdForm();
    StdNode result = S(a[0].left()).toSForm(); // not createExpressionNode here. We'll get infinite recursion
    if(!a[0].isPositive()) result = -result;
    for(size_t i = 1; i < a.size(); i++) {
      SNode e  = a[i];
      SNode ne = S(e.left()).toSForm();
      if(e.isPositive()) result += ne; else result -= ne;
    }
    RETURNNODE( result );
  }
}

StdNode StdNode::toSFormProduct() const {
  ENTERMETHOD();
  Rational constant = 1;

  const FactorArray &a = getFactorArray();
  FactorArray newArray(a.getTree());
  for(size_t i = 0; i < a.size(); i++) {
    SNode f = a[i];
    Rational r;
    if(f.reducesToRationalConstant(&r)) {
      constant *= r;
    } else {
      newArray *= f;
    }
  }
  FactorArray p(getTree()),q(getTree());
  if(constant.getNumerator() != 1) {
    p *= constant.getNumerator();
  }
  if(constant.getDenominator() != 1) {
    q /= constant.getDenominator();
  }

  p *= newArray.selectConstantPositiveExponentFactors();
  p *= newArray.selectNonConstantExponentFactors();
  q *= newArray.selectConstantNegativeExponentFactors();

  StdNode result = toSFormFactorArray(p, false) / toSFormFactorArray(q, true);
  RETURNNODE( result );
}

StdNode StdNode::toSFormFactorArray(FactorArray &a, bool changeExponentSign) const {
  ENTERMETHOD();
  SNodeArray a1(a.getTree());
  for(size_t i = 0; i < a.size(); i++) {
    StdNode f = a[i];
    if(changeExponentSign) {
      a1.add(reciprocal(f.toSFormPow()));
    } else {
      a1.add(f.toSFormPow());
    }
  }
  if(a1.size() == 0) {
    RETURNNODE( _1() );
  } else {
    SNode result = a1[0];
    for(size_t i = 1; i < a1.size(); i++) {
      result *= a1[i];
    }
    RETURNNODE( result );
  }
}

StdNode StdNode::toSFormPow() const {
  ENTERMETHOD();
  const SNode base     = S(left()).toSForm();
  const SNode exponent = S(right()).toSForm();

  Rational expoR;
  if(exponent.reducesToRationalConstant(&expoR)) {
    const __int64 &num = expoR.getNumerator();
    if(expoR.isInteger()) {
      switch(num) {
      case  0 : RETURNNODE( _1() );
      case  1 : RETURNNODE( base );
      case -1:
        switch(base.getSymbol()) {
        case COS: RETURNNODE( sec(base.left()) );
        case SIN: RETURNNODE( csc(base.left()) );
        case TAN: RETURNNODE( cot(base.left()) );
        default : RETURNNODE( reciprocal(base) );
        }
      case  2: RETURNNODE( sqr(base) );
      case -2: RETURNNODE( reciprocal(sqr(base)) );
      default:
        if(num > 0) {
          RETURNNODE( pow(base, SNV(expoR.getNumerator())) );
        } else {
          RETURNNODE( reciprocal(pow(base, SNV(-num))) );
        }
      }
    } else { // r not integer
      const __int64 &den = expoR.getDenominator(); // > 0
      if(::abs(num) == 1) {
        if(den == 2) {
          if(num == 1) {
            RETURNNODE( sqrt(base) );
          } else { // num == -1
            RETURNNODE( reciprocal(sqrt(base)) );
          }
        } else {
          if(num == 1) {
            RETURNNODE( root(base, SNV(den)) );
          } else { // num == -1
            RETURNNODE( reciprocal(root(base, SNV(den))) );
          }
        }
      } else { // num != 1 and num != -1 and r not integer
        if(num > 0) {
          RETURNNODE( root(pow(base, SNV(num)), SNV(den)) );
        } else {
          RETURNNODE( reciprocal(root(pow(base, SNV(-num)), SNV(den))) );
        }
      }
    }
  } else { // exponent not rational constant
    if(exponent.getSymbol() == QUOT) {
      RETURNNODE( root(pow(base, exponent.left()), exponent.right()) );
    } else {
      RETURNNODE( pow(base, exponent) );
    }
  }
}

// -------------------------------------------------------------------------------------------------------

class StandardFormChecker : public ExpressionNodeHandler {
private:
  String m_error;
  bool   m_ok;
public:
  StandardFormChecker() : m_ok(true) {
  };
  bool handleNode(ExpressionNode *n, int level);
  bool isOk() const {
    return m_ok;
  }
  const String &getErrorMessage() const {
    return m_error;
  }
};

bool StandardFormChecker::handleNode(ExpressionNode *n, int level) {
  static const ExpressionSymbolSet illegalSymbolSet(
    PRODUCT, SUM, ADDENT, EOI
  );
  if(illegalSymbolSet.contains(n->getSymbol())) {
    m_error = format(_T("Illegal symbol in standard form:<%s>. node=<%s>")
                    ,n->getSymbolName().cstr(), n->toString().cstr());
    m_ok = false;
    return false;
  }
  return true;
}

void ParserTree::checkIsStandardForm() const {
  StandardFormChecker checker;
  ((ParserTree*)this)->traverseTree(checker);
  if(!checker.isOk()) {
    throwException(checker.getErrorMessage());
  }
}

}; // namespace Expr
