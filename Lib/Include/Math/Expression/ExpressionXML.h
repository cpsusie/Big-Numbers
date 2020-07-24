#pragma once

#include <XMLDoc.h>

namespace Expr {

class Expr3;

void setValue(XMLDoc &doc, XMLNodePtr n, const Expr3 &expr);
void getValue(XMLDoc &doc, XMLNodePtr n,       Expr3 &expr);

};
