#include "pch.h"
#include <Math/Expression/ParserTree.h>
#include <Math/Expression/ExpressionFactor.h>
#include <Math/Expression/SumElement.h>

// -------------------------------------Operators for CanonicalForm-------------------------------------------------------------------------
// Not using PLUS,(binary) MINUS,PROD,QUOT,ROOT,SQR,SQRT,EXP,COT,CSC,SEC

class NodeOperatorsCanonForm : public NodeOperators {
private:
  static inline ExpressionNode   *getSum(         ExpressionNode *n, AddentArray &aa) {
    return n->getTree()->getSum(aa);
  }
  static inline ExpressionNode   *getProduct(     ExpressionNode *n, FactorArray &fa) {
    return n->getTree()->getProduct(fa);
  }
  static inline ExpressionNode   *unaryMinus(ExpressionNode *n) {
    return n->getTree()->unaryMinus(n);
  }
  static inline ExpressionNode   *getPoly(const ExpressionNodeArray &coefArray, ExpressionNode *arg) {
    return arg->getTree()->getPoly(coefArray, arg);
  }
  static inline ExpressionNode   *indexedSum(ExpressionNode *assign, ExpressionNode *end, ExpressionNode *expr) {
    return assign->getTree()->indexedSum(assign,end,expr);
  }
  static inline ExpressionNode   *indexedProduct(ExpressionNode *assign, ExpressionNode *end, ExpressionNode *expr) {
    return assign->getTree()->indexedProduct(assign,end,expr);
  }
  static inline ExpressionFactor *fetchFactorNode(ExpressionNode *base, ExpressionNode *exponent) {
    return base->getTree()->fetchFactorNode(base,exponent);
  }
  static inline ExpressionNode   *constExpression(ExpressionNode *n, const TCHAR *name) {
    return n->getTree()->constExpression(name);
  }
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
  ExpressionNode *mod(       ExpressionNode *n1, ExpressionNode *n2) const;
  ExpressionNode *power(     ExpressionNode *n1, ExpressionNode *n2) const;
  ExpressionNode *root(      ExpressionNode *n1, ExpressionNode *n2) const;
  ParserTreeForm  getTreeForm() const {
    return TREEFORM_CANONICAL;
  }
};

class NodeOperatorsCanonNumForm : public NodeOperatorsCanonForm {
public:
  ParserTreeForm  getTreeForm() const {
    return TREEFORM_NUMERIC;
  }
};

ExpressionNode *NodeOperatorsCanonForm::minus(ExpressionNode *n) const {
  switch(n->getSymbol()) {
  case NUMBER:
    return numberExpression(n,-n->getNumber());
  case MINUS:
    assert(n->isUnaryMinus());
    return n->left();

  case SUM       :
    { const AddentArray &a = n->getAddentArray();
      AddentArray newAddentArray(a.size());
      for(size_t i = 0; i < a.size(); i++) { // change sign for all elements in list
        SumElement *e = a[i];
        newAddentArray.add(e->getNode(),!e->isPositive());
      }
      return getSum(n,newAddentArray);
    }

  case PRODUCT   :
    { const FactorArray &factors = n->getFactorArray();
      const int          index   = factors.findFactorWithChangeableSign();
      FactorArray        newFactors(factors.size());
      for(size_t i = 0; i < factors.size(); i++) {
        ExpressionFactor *factor = factors[i];
        if(i != index) {
          newFactors.add(factor);
        } else {
          newFactors.add(minus(factor->base()),factor->exponent());
        }
      }
      if(index < 0) {
        newFactors.add(getMinusOne(n));
      }
      return getProduct(n,newFactors);
    }

  case POLY      :
    { const ExpressionNodeArray &coefArray = n->getCoefficientArray();
      ExpressionNodeArray       newCoefArray(coefArray.size());
      for(size_t i = 0; i < coefArray.size(); i++) {
        newCoefArray.add(minus(coefArray[i]));
      }
      ExpressionNode *arg = n->getArgument();
      return getPoly(newCoefArray, arg);
    }

  case INDEXEDSUM:
    return indexedSum(n->child(0), n->child(1), minus(n->child(2)));
  }

  return unaryMinus(n);
}

ExpressionNode *NodeOperatorsCanonForm::reciprocal(ExpressionNode *n) const {
  switch(n->getSymbol()) {
  case NUMBER :
    if(n->isRational()) {
      return numberExpression(n,::reciprocal(n->getRational()));
    }
    break;

  case PRODUCT       :
    { const FactorArray &factors = n->getFactorArray();
      FactorArray newFactors;
      for(size_t i = 0; i < factors.size(); i++) {
        ExpressionFactor *factor = factors[i];
        newFactors.add(factor->base(), minus(factor->exponent()));
      }
      return getProduct(n,newFactors);
    }
  case INDEXEDPRODUCT:
    return indexedProduct(n->child(0), n->child(1), reciprocal(n->child(2)));
  case POW    : // reciprocal(l^r) = l^-r
    return power(n->left(), minus(n->right()));
  }
  return quot(getOne(n), n);
}

ExpressionNode *NodeOperatorsCanonForm::sum(ExpressionNode *n1, ExpressionNode *n2) const {
  if(n1->isRational() && n2->isRational()) {
    return numberExpression(n1, n1->getRational() + n2->getRational());
  } else if(n1->isZero()) {
    return n2;
  } else if(n2->isZero()) {
    return n1;
  }
  AddentArray a;
  a.add(n1, true);
  a.add(n2, true);
  return getSum(n1,a);
}

ExpressionNode *NodeOperatorsCanonForm::diff(ExpressionNode *n1, ExpressionNode *n2) const {
  if(n1->isRational() && n2->isRational()) {
    return numberExpression(n1, n1->getRational() - n2->getRational());
  } else if(n2->isZero()) {
    return n1;
  } else if(n1->isZero()) {
    return minus(n2);
  }
  AddentArray a;
  a.add(n1, true );
  a.add(n2, false);
  return getSum(n1,a);
}

ExpressionNode *NodeOperatorsCanonForm::prod(ExpressionNode *n1, ExpressionNode *n2) const {
  if(n1->isZero() || n2->isZero()) {
    return getZero(n1);
  }
  if(n1->isRational() && n2->isRational()) {
    return numberExpression(n1, n1->getRational() * n2->getRational());
  }
  FactorArray a;
  a.add(n1);
  a.add(n2);
  return getProduct(n1,a);
}

ExpressionNode *NodeOperatorsCanonForm::quot(ExpressionNode *n1, ExpressionNode *n2) const {
  if(n1->isZero()) {
    return getZero(n1);
  }
  if(n1->isRational() && n2->isRational()) {
    return numberExpression(n1, n1->getRational() / n2->getRational());
  } else if(n2->isOne()) {
    return n1;
  }
  FactorArray a;
  a.add(n1);
  if(n2->getSymbol() != PRODUCT) {
    a.add((n2->getSymbol() == POW) ? fetchFactorNode(n2->left(), minus(n2->right())) : fetchFactorNode(n2, getMinusOne(n2)));
  } else {
    const FactorArray &a2 = n2->getFactorArray();
    for(size_t i = 0; i < a2.size(); i++) {
      ExpressionFactor *f = a2[i];
      a.add(f->base(), minus(f->exponent()));
    }
  }
  return getProduct(n1,a);
}

ExpressionNode *NodeOperatorsCanonForm::mod(ExpressionNode *n1, ExpressionNode *n2) const {
  return binaryExpr(MOD, n1, n2);
}

ExpressionNode *NodeOperatorsCanonForm::sqr(ExpressionNode *n) const {
  return power(n, getTwo(n));
}

ExpressionNode *NodeOperatorsCanonForm::sqrt(ExpressionNode *n) const {
  return root(n, getTwo(n));
}

ExpressionNode *NodeOperatorsCanonForm::power(ExpressionNode *n1, ExpressionNode *n2) const {
  if(n2->isZero()) {
    return getOne(n1);
  } else if(n2->isOne()) {
    return n1;
  } else if(n2->isMinusOne()) {
    return reciprocal(n1);
  } else if(n1->getSymbol() == POW) {
    return binaryExpr(POW, n1->left(), prod(n1->right(), n2));
  } else {
    return binaryExpr(POW, n1, n2);
  }
}

ExpressionNode *NodeOperatorsCanonForm::root(ExpressionNode *n1, ExpressionNode *n2) const {
  if(n2->isOne()) {
    return n1;
  } else if(n2->isMinusOne()) {
    return reciprocal(n1);
  }
  return power(n1, reciprocal(n2));
}

ExpressionNode *NodeOperatorsCanonForm::exp(ExpressionNode *n) const {
  return power(constExpression(n,_T("e")), n);
}

ExpressionNode *NodeOperatorsCanonForm::exp10(ExpressionNode *n) const {
  return power(getTen(n), n);
}

ExpressionNode *NodeOperatorsCanonForm::exp2(ExpressionNode *n) const {
  return power(getTwo(n), n);
}

ExpressionNode *NodeOperatorsCanonForm::cot(ExpressionNode *n) const {
  return reciprocal(functionExpr(TAN, n));
}

ExpressionNode *NodeOperatorsCanonForm::csc(ExpressionNode *n) const {
  return reciprocal(functionExpr(SIN, n));
}

ExpressionNode *NodeOperatorsCanonForm::sec(ExpressionNode *n) const {
  return reciprocal(functionExpr(COS, n));
}

static const NodeOperatorsCanonForm    canonFormOps;
static const NodeOperatorsCanonNumForm canonNumFormOps;

const NodeOperators *NodeOperators::s_canonForm    = &canonFormOps;
const NodeOperators *NodeOperators::s_canonNumForm = &canonNumFormOps;


// ------------------------------- tree conversion classes/functions ------------------
// ----------------------------------------- toCanonicalForm ------------------------------------------------------------------

class CNode : public SNode {
private:
  CNode toCForm() const;
  CNode toCFormSum() const;
  CNode toCFormProduct() const;
  CNode toCFormPoly() const;
  CNode toCFormTreeNode() const;
  FactorArray &toCFormProduct(FactorArray &result, SNode &exponent) const;
  AddentArray &toCFormSum(    AddentArray &result, bool   positive) const;
  FactorArray &toCFormPower(  FactorArray &result, SNode &exponent) const;
  FactorArray &toCFormRoot(   FactorArray &result, SNode &exponent) const;

  inline CNode(const SNode &n) : SNode(n) {
  }
public:
  inline CNode(ExpressionNode *n) : SNode(n) {
  }
  ExpressionNode *convert() const {
    return toCForm();
  }
};

#define N(n)  CNode(n)
#define NV(v) SNode(getTree(),v)

// Replace PLUS,(binary) MINUS,PROD,QUOT,ROOT,SQRT,SQR,EXP,EXP10,EXP2,SEC,CSC,COT and negative constanst,
// with combinations of SUM, (unary MINUS), PRODUCT, POW, SIN,COS,TAN, and positive constants
CNode CNode::toCForm() const {
  switch(getSymbol()) {
  case NUMBER         :
  case NAME           : return *this;

  case MINUS          : if(isUnaryMinus()) return -N(left()).toCForm();
                        // NB continue case

  case PLUS           :
  case SUM            : return toCFormSum();

  case PRODUCT        :
  case PROD           :
  case QUOT           :
  case POW            :
  case SQR            :
  case ROOT           :
  case SQRT           :
  case EXP            :
  case EXP10          :
  case EXP2           : return toCFormProduct();

  case SEC            : return sec(N(left()).toCForm());
  case CSC            : return csc(N(left()).toCForm());
  case COT            : return cot(N(left()).toCForm());

  case POLY           : return toCFormPoly();

  default             : return toCFormTreeNode();
  }
}

CNode CNode::toCFormSum() const {
  AddentArray a;
  return getTree()->getSum(toCFormSum(a, true));
}

AddentArray &CNode::toCFormSum(AddentArray &result, bool positive) const {
  switch(getSymbol()) {
  case SUM:
    { const AddentArray &a = getAddentArray();
      for(size_t i = 0; i < a.size(); i++) {
        SumElement *e = a[i];
        N(a[i]->getNode()).toCFormSum(result, e->isPositive() == positive);
      }
    }
    break;
  case PLUS:
    N(right()).toCFormSum(N(left()).toCFormSum(result, positive), positive);
    break;
  case MINUS:
    if(isUnaryMinus()) {
      N(left()).toCFormSum(result, !positive);
    } else {
      N(right()).toCFormSum(N(left()).toCFormSum(result, positive), !positive);
    }
    break;
  default:
    result.add(toCForm(), positive);
    break;
  }
  return result;
}

CNode CNode::toCFormProduct() const {
  FactorArray a;
  return getTree()->getProduct(toCFormProduct(a, _1()));
}

FactorArray &CNode::toCFormProduct(FactorArray &result, SNode &exponent) const {
  switch(getSymbol()) {
  case PRODUCT:
    { const FactorArray &a = getFactorArray();
      if(exponent.isOne()) {
        result.addAll(a);
      } else {
        for(size_t i = 0; i < a.size(); i++) N(a[i]).toCFormProduct(result, exponent);
      }
    }
    break;
  case PROD:
    N(right()).toCFormProduct(N(left()).toCFormProduct(result, exponent), exponent);
    break;
  case QUOT:
    N(right()).toCFormProduct(N(left()).toCFormProduct(result, exponent), -exponent);
    break;
  case POW :
    toCFormPower(result, exponent);
    break;
  case SQR:
    N(sqr(N(left()).toCForm())).toCFormProduct(result, exponent);
    break;
  case EXP:
    N(exp(N(left()).toCForm())).toCFormProduct(result, exponent);
    break;
  case EXP10:
    N(exp10(N(left()).toCForm())).toCFormProduct(result, exponent);
    break;
  case EXP2:
    N(exp2(N(left()).toCForm())).toCFormProduct(result, exponent);
    break;
  case ROOT:
    toCFormRoot(result, exponent);
    break;
  case SQRT:
    N(sqrt(N(left()).toCForm())).toCFormProduct(result, exponent);
    break;
  case NUMBER:
    result.add((ExpressionNode*)node(), exponent);
    break;
  default:
    result.add(toCForm(), exponent);
    break;
  }
  return result;
}

// n.symbol == POW,
FactorArray &CNode::toCFormPower(FactorArray &result, SNode &exponent) const {
  DEFINEMETHODNAME;
  SNode base = N(left()).toCForm();
  SNode expo = N(right()).toCForm();

  switch(base.getSymbol()) {
  case POW :
    result.add(base.left(), base.right() * exponent * expo);
    break;
  case ROOT:
    throwInvalidSymbolForTreeMode(method);
    break;
  case PRODUCT:
    { const FactorArray &factors = base.getFactorArray();
      SNode              newExpo = exponent * expo;
      for(size_t i = 0; i < factors.size(); i++) N(factors[i]).toCFormPower(result, newExpo);
    }
    break;
  default                   :
    result.add(base, exponent * expo);
    break;
  }
  return result;
}

// n.symbol = ROOT
FactorArray &CNode::toCFormRoot(FactorArray &result, SNode &exponent) const {
  DEFINEMETHODNAME;
  SNode rad  = N(left()).toCForm();
  SNode root = N(right()).toCForm();

  switch(rad.getSymbol()) {
  case POW :
    result.add(rad.left(), rad.right() * exponent / root);
    break;
  case ROOT:
    throwInvalidSymbolForTreeMode(method);
    break;
  case PRODUCT:
    { const FactorArray &factors = rad.getFactorArray();
      SNode              newExpo = exponent / root;
      for(size_t i = 0; i < factors.size(); i++) N(factors[i]).toCFormPower(result, newExpo);
    }
    break;
  default                   :
    result.add(rad, exponent / root);
    break;
  }
  return result;
}

CNode CNode::toCFormPoly() const {
  const ExpressionNodeArray &coefArray = getCoefficientArray();
  ExpressionNodeArray newCoefArray(coefArray.size());
  for(size_t i = 0; i < coefArray.size(); i++) {
    newCoefArray.add(N(coefArray[i]).toCForm());
  }
  SNode newArg = N(getArgument()).toCForm();
  return polyExp(newCoefArray, newArg);
}

CNode CNode::toCFormTreeNode() const {
  const ExpressionNodeArray &a = getChildArray();
  ExpressionNodeArray newChildArray(a.size());
  for(size_t i = 0; i < a.size(); i++) {
    newChildArray.add(N(a[i]).toCForm());
  }
  return treeExp(getSymbol(), newChildArray);
}

// -------------------------------------------------------------------------------------------------------

class CanonicalFormChecker : public ExpressionNodeHandler {
private:
  BitSet m_illegalSymbolSet;
  String m_error;
  bool   m_ok;
public:
  CanonicalFormChecker();
  bool handleNode(ExpressionNode *n, int level);
  bool isOk() const {
    return m_ok;
  }
  const String &getErrorMessage() const {
    return m_error;
  }
};

CanonicalFormChecker::CanonicalFormChecker() : m_illegalSymbolSet(ParserTree::getTerminalCount() + 1) {
  static const ExpressionInputSymbol table[] = {
    PLUS
   ,PROD
   ,QUOT
   ,ROOT
   ,SQRT
   ,SQR
   ,EXP
   ,SEC
   ,CSC
   ,COT
  };
  for(int i = 0; i < ARRAYSIZE(table); i++) {
    m_illegalSymbolSet.add(table[i]);
  }
  m_ok = true;
}

bool CanonicalFormChecker::handleNode(ExpressionNode *n, int level) {
  if(m_illegalSymbolSet.contains(n->getSymbol()) || n->isBinaryMinus()) {
    m_error = format(_T("Illegal symbol in canonical form:<%s>. node=<%s>"), n->getSymbolName().cstr(), n->toString().cstr());
    m_ok = false;
    return false;
  }
  return true;
}

void ParserTree::checkIsCanonicalForm() {
  CanonicalFormChecker checker;
  traverseTree(checker);
  if(!checker.isOk()) {
    throwException(checker.getErrorMessage());
  }
}

ExpressionNode *ParserTree::toCanonicalForm(ExpressionNode *n) {
  if((getTreeForm() == TREEFORM_CANONICAL) || (n == NULL)) {
    return n;
  }
  m_ops = NodeOperators::s_canonForm;
  return CNode(n).convert();
}
