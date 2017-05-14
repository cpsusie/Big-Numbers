#pragma once

#include <BitSet.h>
#include <Scanner.h>
#include "LexScanner.h"

#define EDGE_EPSILON  -1                              // non-character values of NFAstate.m_edge
#define EDGE_CHCLASS  -2


#define MAX_CHARS   256                               // maximal character value

#define RMARGIN 118

class AcceptAction : public SourceText {
public:
  char m_anchor;            // is the pattern anchored to START, END, BOTH or none
  String dumpFormat() const;
};

class NFAstate {
public:
  int           m_id;        // The states id
  int           m_edge;      // Label for outgoing edge: character (>=0), EDGE_CHCLASS or EDGE_EPSILON
  BitSet       *m_charClass; // Characterclass when m_edge = EDGE_CHCLASS
  NFAstate     *m_next1;     // Next state (or NULL if none)
  NFAstate     *m_next2;     // Alternative next state if m_edge = EDGE_EPSILON. NULL if no alternative.
  AcceptAction *m_accept;    // AcceptAction if accepting state, else NULL
  NFAstate(int edge = EDGE_EPSILON);
  NFAstate *successor(int c) const; // Returns successor-state on transition c (character). NULL if none
  int getID() const {
    return m_id;
  }
  void print(MarginFile &f);
};

class NFA : public Array<NFAstate*> {
public:
  SourceText m_header, m_driverHead, m_driverTail;
  NFA() {};
  NFA(const NFA &src);      // not defined
  NFA &operator=(NFA &src); // not defined
};

class NFAparser {
private:
  LexScanner    m_scanner;
  AcceptAction *m_lastAction;
  int           m_idCounter;
  int           m_recurseLevel;
  NFA          &m_NFA;

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
  bool match(Token t) const {
    return m_scanner.getToken() == t;
  }

  void nextToken() {
    m_scanner.nextToken();
  }

  void error(TCHAR *message) {
    m_scanner.error(_T("%s"), message);
  }

  void warning(TCHAR *message)  {
    m_scanner.warning(_T("%s"), message);
  }

  unsigned int getLexeme() const {
    return m_scanner.getLexeme();
  }

public:
  NFAparser(const String &fname, NFA &nfa);
  NFAparser(NFAparser &src);                  // not defined
  NFAparser &operator=(const NFAparser &src); // not defined
  void thompsonConstruction();
};

String binToAscii(int c, bool use_hex = true); // Returns a String that represents c, using escape-sequences. ie opposite of escape
void printChar(     MarginFile &f, int c);     // f.printf(_T("%s"),binToAscii(c));
void printCharClass(MarginFile &f, BitSet &set);
void printSet(      MarginFile &f, BitSet &set);

