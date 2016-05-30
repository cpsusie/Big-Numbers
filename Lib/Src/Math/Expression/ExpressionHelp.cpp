#include "pch.h"
#include <string.h>
#include <Math/Expression/Expression.h>

ExpressionDescription::ExpressionDescription(const char *syntax, const char *description) {
  m_syntax      = syntax;
  m_description = description;
}

const ExpressionDescription ExpressionDescription::expressionHelpList[] = {
    ExpressionDescription("x + y"                                        ,"x plus y.")
   ,ExpressionDescription("x - y"                                        ,"x minus y.")
   ,ExpressionDescription("x * y"                                        ,"x multiplied by y.")
   ,ExpressionDescription("x / y"                                        ,"x divided by y.")
   ,ExpressionDescription("x ^ y"                                        ,"x raised to the power of y. Special case:x^0 = 1 for all x >= 0.")
   ,ExpressionDescription("abs(x)"                                       ,"Absolute value of x. abs(x) = x for x >= 0, -x for x < 0.")
   ,ExpressionDescription("acos(x)"                                      ,"The inverse cosine function over the domain -1 <= x <= 1.")
   ,ExpressionDescription("acosh(x)"                                     ,"The inverse hyperbolic cosine function over the domain x >= 1.")
   ,ExpressionDescription("acot(x)"                                      ,"The inverse cotangent function.")
   ,ExpressionDescription("acsc(x)"                                      ,"The inverse cosecant function over the domain x <= -1 or x >= 1.")
   ,ExpressionDescription("a and b"                                      ,"Logical and. true iff a is true and b is true.")
   ,ExpressionDescription("asec(x)"                                      ,"The inverse secant function over the domain x <= -1 or x >= 1.")
   ,ExpressionDescription("asin(x)"                                      ,"The inverse sine function over the domain -1 <= x <= 1.")
   ,ExpressionDescription("asinh(x)"                                     ,"The inverse hyperbolic sine function.")
   ,ExpressionDescription("a = x*x;"                                     ,"Create the variable a and assign the value x*x to it.")
   ,ExpressionDescription("atan(x)"                                      ,"The inverse tangent function.")
   ,ExpressionDescription("atanh(x)"                                     ,"The inverse hyperbolic tangent function over the domain -1 <= x <= 1.")
   ,ExpressionDescription("binomial(x,y)"                                ,"The binomial coefficient of x and y.")
   ,ExpressionDescription("ceil(x)"                                      ,"The smallest integer greater than or equal to x. ceil(2.4) = 3. ceil(-2.1) = -2.")
   ,ExpressionDescription("cos(x)"                                       ,"The cosine function.")
   ,ExpressionDescription("cosh(x)"                                      ,"The hyperbolic cosine function.")
   ,ExpressionDescription("cot(x)"                                       ,"The cotangent function. cot(x) = 1/tan(x).")
   ,ExpressionDescription("csc(x)"                                       ,"The cosecant function. csc(x) = 1/sin(x).")
   ,ExpressionDescription("e"                                            ,"Base of the natural logarithm. e = 2.71828182...")
   ,ExpressionDescription("erf(x)"                                       ,"The error function. erf(x) = 2*norm(sqrt(2)*x)-1.")
   ,ExpressionDescription("exp(x)"                                       ,"The exponential function with base e.")
   ,ExpressionDescription("fac(x)"                                       ,"The factorial function. fac(x) = x! = x * (x-1) * (x-2)...* 1. fac(x) = gamma(x+1).")
   ,ExpressionDescription("floor(x)"                                     ,"The highest integer less than or equal to x. floor(2.4) = 2. floor(-2.1) = -3")
   ,ExpressionDescription("gamma(x)"                                     ,"The gamma function. gamma(1) = 1. gamma(x) = x * gamma(x-1).")
   ,ExpressionDescription("gauss(x)"                                     ,"The standard normal density. Gauss-density function. gauss(x) = norm'(x).")
   ,ExpressionDescription("if(condition,expr1,expr2)"                    ,"Conditional expression. if condition is true, the value is expr1, else the value is expr2.")
   ,ExpressionDescription("[condition]"                                  ,"Iverson bracket. If condition is true then 1, else 0. [condition] is equivalent to if(condition,1,0).")
   ,ExpressionDescription("inverf(x)"                                    ,"The inverse error function over the domain -1 < x < 1. inverf(erf(x)) = x.")
   ,ExpressionDescription("ln(x)"                                        ,"The natural logarithm with base e. ln'(x) = 1/x.")
   ,ExpressionDescription("log10(x)"                                     ,"The logarithm with base 10. log10(x) = ln(x) / ln(10).")
   ,ExpressionDescription("max(x,y)"                                     ,"The highest value of x and y.")
   ,ExpressionDescription("min(x,y)"                                     ,"The lowest value of x and y.")
   ,ExpressionDescription("norm(x)"                                      ,"The standard normal distribution function. norm'(x) = gauss(x).")
   ,ExpressionDescription("normrand(m,s)"                                ,"Normal distributed random value with mean value m, and standard deviation s.")
   ,ExpressionDescription("not a"                                        ,"Logical negation. true if a is false, false if a is true.")
   ,ExpressionDescription("a or b"                                       ,"Logical or.  true iff a is true or b is true or both is true.")
   ,ExpressionDescription("pi"                                           ,"pi = 3.14159265359...")
   ,ExpressionDescription("poly[a,b,c,...](x)"                           ,"The polynomial with coefficients a,b,c,.... poly[a,b,c](x) = ax^2+bx+c.")
   ,ExpressionDescription("probit(x)"                                    ,"The inverse normal distribution function over the domain 0 < x < 1. probit(norm(x)) = x.")
   ,ExpressionDescription("product(i = startExpr to endExpr) expression" ,"ex. 10! = product(i=1 to 10) i.")
   ,ExpressionDescription("rand(a,b)"                                    ,"Uniform distributed random value in the range [a;b[.")
   ,ExpressionDescription("root(x,y)"                                    ,"The y'th root of x. root(x,y) = x^(1/y). root(x,2) = square root of x.")
   ,ExpressionDescription("sign(x)"                                      ,"sign(x) = 1 for x > 0, 0 for x = 0, -1 for x < 0.")
   ,ExpressionDescription("sec(x)"                                       ,"The secant function. sec(x) = 1/cos(x).")
   ,ExpressionDescription("sin(x)"                                       ,"The sine function.")
   ,ExpressionDescription("sinh(x)"                                      ,"The hyperbolic sine function.")
   ,ExpressionDescription("sqr(x)"                                       ,"x squared. sqr(x) = x*x.")
   ,ExpressionDescription("sqrt(x)"                                      ,"The square root of x over the domain x = 0.")
   ,ExpressionDescription("sum(i = startExpr to endExpr) expression"     ,"Ex: sum(i=0 to n) x^i/fac(i) -> exp(x) for n -> infinity.")
   ,ExpressionDescription("tan(x)"                                       ,"The tangent function. tan(x) = sin(x)/cos(x).")
   ,ExpressionDescription("tanh(x)"                                      ,"The hyperbolic tangent function.")
};

const int ExpressionDescription::helpListSize = ARRAYSIZE(expressionHelpList);
