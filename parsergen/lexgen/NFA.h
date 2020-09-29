#pragma once

#include <BitSet.h>
#include <Scanner.h>
#include "LexScanner.h"

// non-character values of NFAstate.m_edge
#define EDGE_EPSILON  -1
#define EDGE_CHCLASS  -2

// maximal character value
#define MAX_CHARS   256

#define RMARGIN 118

class AcceptAction : public SourceText {
public:
  // is the pattern anchored to START, END, BOTH or none
  char m_anchor;
  String dumpFormat() const;
};

class NFAstate {
public:
  // The states id
  int           m_id;
  // Label for outgoing edge: character (>=0), EDGE_CHCLASS or EDGE_EPSILON
  int           m_edge;
  // Characterclass when m_edge = EDGE_CHCLASS
  BitSet       *m_charClass;
  // Next state (or NULL if none)
  NFAstate     *m_next1;
  // Alternative next state if m_edge = EDGE_EPSILON. NULL if no alternative.
  NFAstate     *m_next2;
  // AcceptAction if accepting state, else NULL
  AcceptAction *m_accept;
  NFAstate(int edge = EDGE_EPSILON);
  // Returns successor-state on transition c (character). NULL if none
  NFAstate *successor(int c) const;
  int getID() const {
    return m_id;
  }
  void print(MarginFile &f);
};

class NFA : public CompactArray<NFAstate*> {
private:
  NFA(const NFA &src);      // Not defined. Class not cloneable
  NFA &operator=(NFA &src); // Not defined. Class not cloneable

public:
  SourceText m_header, m_driverHead, m_driverTail;
  NFA() {
  }
};

class NFAparser {
private:
  LexScanner    m_scanner;
  AcceptAction *m_lastAction;
  int           m_idCounter;
  int           m_recurseLevel;
  NFA          &m_NFA;

  NFAparser(NFAparser &src);                  // Not defined. Class not cloneable
  NFAparser &operator=(const NFAparser &src); // Not defined. Class not cloneable

  void parseHeadBody(SourceText &source);
  void readHead();
  void readTail();
  void enter(const TCHAR *function);
  void leave(const TCHAR *function);
  AcceptAction *acceptString(int anchor);
  void generateID(NFAstate *state);
  NFAstate *parse();
  NFAstate *machine();
  NFAstate *rule();
  void expr(NFAstate *&startp, NFAstate *&endp);
  void catExpr(NFAstate *&startp, NFAstate *&endp);
  bool firstInCatExpr(Token tok);
  void factor(NFAstate *&startp, NFAstate *&endp);
  void term(NFAstate *&startp, NFAstate *&endp);
  void characterInterval(BitSet &set);
  inline bool match(Token t) const {
    return m_scanner.getToken() == t;
  }

  inline void nextToken() {
    m_scanner.nextToken();
  }

  inline void error(TCHAR *message) {
    m_scanner.error(_T("%s"), message);
  }

  inline void warning(TCHAR *message)  {
    m_scanner.warning(_T("%s"), message);
  }

  inline UINT getLexeme() const {
    return m_scanner.getLexeme();
  }

public:
  NFAparser(const String &fname, NFA &nfa);
  void thompsonConstruction();
};

// Returns a String that represents c. This will be the character itself for normal characters,
// and an escape sequence (\n, \t, \x00, ...), for most others. A ' is represented as \'.
// Returns a String that represents c, using escape-sequences. ie opposite of escape
// If useHex is true, then \xDD escape sequences are used. Otherwise, octal sequences (\DDD) are used.
String binToAscii(int c, bool use_hex = true);
// f.printf(_T("%s"),binToAscii(c));
void printChar(     MarginFile &f, int c);
void printCharClass(MarginFile &f, BitSet &set);
void printSet(      MarginFile &f, BitSet &set);

