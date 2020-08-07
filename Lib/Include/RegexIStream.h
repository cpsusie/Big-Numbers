#pragma once

#include "StringArray.h"

class DFARegex;

class RegexIStream {
private:
  DFARegex *m_regex;
  RegexIStream(const RegexIStream &);            // Not defined. Class not cloneable
  RegexIStream &operator=(const RegexIStream &); // Not defined. Class not cloneable
  void cleanup();
public:
  RegexIStream() : m_regex(NULL) {
  }
#if defined(_DEBUG)
  // See comment below for a description of pattern
  RegexIStream(const StringArray &pattern, bool ignoreCase = false, bool dumpStates = true) : m_regex(NULL) {
    compilePattern(pattern, ignoreCase,dumpStates);
  }
  // See comment below for a description of pattern
  void compilePattern(const StringArray &pattern, bool ignoreCase = false, bool dumpStates = true);
#else
  // See comment below for a description of pattern
  void compilePattern(const StringArray &pattern, bool ignoreCase = false);
  RegexIStream(const StringArray &pattern, bool ignoreCase = false) : m_regex(NULL) {
    compilePattern(pattern, ignoreCase);
  }
#endif // _DEBUG
  virtual ~RegexIStream() {
    cleanup();
  }
  inline bool isEmpty() const {
    return m_regex == NULL;
  }
  // Return index into StringArray &pattern, specified to compilePattern, which has the best (longest) match, if any,
  // and the next input character in stream is the character following this
  // If no match, -1 is returned and the stream is left unchanged.
  // If matchedString != NULL, and there is a match, it will contain the String read from the stream.
  int match(std::istream  &in, String *matchedString = NULL) const;
  int match(std::wistream &in, String *matchedString = NULL) const;
#if defined(_DEBUG)
  String toString() const;
#endif // _DEBUG
};

// Each String in StringArray pattern (argument to compilePattern) should comply to the following grammar
// An exception is thrown, if any syntax errors are found
//
// ExprList           -> ExprList Factor
//                     | Factor
//
// Factor             -> Term
//                     | Term ?
//                     | Term +
//                     | Term *
//
// Term               -> LPAR AlternativeList RPAR
//                     | DOT
//                     | LB OPTcomplement OPTcharclass RB
//                     | NORMALCHAR
//                     | WORDCHAR
//                     | NONWORDCHAR
//
// AlternativeList    -> AlternativeList OR ExprList
//                     | ExprList
//
// OPTcharclass       -> CharClass
//                     | epsilon
//
// OPTcomplement      -> COMPLEMENT
//                     | epsilon
//
// CharClass          -> ccl CharClass
//                     | ccl
//
// ccl                 | CHARACTER
//                     | CHARACTER TO CHARACTER
//
// LPAR               -> /(
// RPAR               -> /)
// OR                 -> /|
// WORDCHAR           -> /w
// NONWORDCHAR        -> /W
// LB                 -> [
// RB                 -> ]
// COMPLEMENT         -> ^
// TO                 -> -
// DOT                -> .
//
// CHARACTER (inside charclass) are treated different than outside charclass (NORMALCHAR).
// Outside charclass:
//   NORMALCHAR is /c or any, except .?+*[]/
//   If c is one of these, it is c, if c is one of ()|wW, it is described above. if c is anything else, then /c is /
//
// Inside charclass:
//   CHARACTER is /c or any, except ]-/     ^ is normal too, if not the first character following [
//   If c is one of these, it is c. if not, then /c is /
//
// Note: The characters .?+* are normal characters inside charclass
//       The characters ]-^  are normal characters outside charclass
//
// The [] and [^] is nonstandard. [] matches ' ', '\t', '\f', '\n', but not '\r'
// [^] matches anything except ' ', '\t', '\f', '\n' (for Windows also '\r')
// DOT (outside charclass) matches everything except '\n' (for Windows also '\r')
// WORDCHAR matches a-z,A-Z,0-9. NONWORDCHAR matches any other
