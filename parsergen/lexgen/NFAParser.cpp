#include "stdafx.h"
#include "NFA.h"

//#define DEBUGLEX

#ifdef ENTERFUNC
#undef ENTERFUNC
#endif
#ifdef LEAVEFUNC
#undef LEAVEFUNC
#endif

#ifdef DEBUGLEX

void NFAparser::enter(const TCHAR *function) {
  String input = m_scanner.getInput();
  input.replace(_T('\n'), _T(' ')).replace(_T('\r'), EMPTYSTRING);
  String next = String(_T("'")) + binToAscii(getLexeme()) + _T("'.");
  int i1 = m_recurseLevel;
  int i2 = max(30 - i1, 0);
  _tprintf(_T("%*.*senter %-17s%*.*s next=%-7s Follow=\")%s\"\n")
             ,i1, i1, EMPTYSTRING, function
             ,i2, i2, EMPTYSTRING, next.cstr(), input.cstr());
  fflush(stdout);
  m_recurseLevel++;
}

void NFAparser::leave(const TCHAR *function) {
  m_recurseLevel--;
  String input = m_scanner.getInput();
  input.replace(_T('\n'), _T(' ')).replace(_T('\r'), EMPTYSTRING);
  String next = String(_T("'")) + binToAscii(getLexeme()) + _T("'.");
  int i1 = m_recurseLevel;
  int i2 = max(30 - i1, 0);
  _tprintf(_T("%*.*sleave %-17s%*.*s next=%-7s Follow=\")%s\"\n")
          ,i1, i1, EMPTYSTRING, function
          ,i2, i2, EMPTYSTRING, next.cstr(), input.cstr());
  fflush(stdout);
}

#define ENTERFUNC enter(__TFUNCTION__)
#define LEAVEFUNC leave(__TFUNCTION__)

#define ENTER(label) enter(_T(label))
#define LEAVE(label) leave(_T(label))

#else

#define ENTERFUNC
#define LEAVEFUNC

#define ENTER(label)
#define LEAVE(label)

#endif


#ifdef _WIN32
#pragma message("NB -------------------- In Windows newline include \\r and \\n -------------------")
#else
#pragma message("NB -------------------- Newline include \\n -------------------")
#endif
// In Windows a newline is \r\n, In Unix \n,
// so charerterclasses involving newline depends on the platform

void NFAparser::expr(NFAstate *&startp, NFAstate *&endp) {
  // This recursive descent compiler can't handle left recursion.
  // The productions:
  //
  //  expr    => expr BAR catExpr
  //           | catExpr
  //           ;
  //
  // should be parsed as
  //
  //  expr    => catExpr expr'
  //           ;

  //  expr'   => BAR catExpr expr'
  //           | epsilon
  //           ;
  //
  // which will be done by this loop:
  //
  //  catExpr()
  //  while(match(BAR)) {
  //    nextToken();
  //    catExpr();
  //  }

  ENTERFUNC;

  catExpr(startp, endp);

  while(match(BAR)) {
    NFAstate *e2_start = NULL; // expression to right of |
    NFAstate *e2_end   = NULL;
    NFAstate *p;

    nextToken();
    catExpr(e2_start, e2_end);

    p               = new NFAstate;
    p->m_next2      = e2_start;
    p->m_next1      = startp;
    startp          = p;

    p               = new NFAstate;
    endp->m_next1   = p;
    e2_end->m_next1 = p;
    endp            = p;
  }

  LEAVEFUNC;
}

void NFAparser::catExpr(NFAstate *&startp, NFAstate *&endp) {
  // The same translations that were needed in the expr rules are needed here.
  //
  //  catExpr     => catExpr factor
  //               | factor
  //               ;
  //
  //  is translated to:
  //
  //  catExpr     => factor catExpr'
  //               ;
  //
  //  catExpr'    => factor catExpr'
  //               | epsilon
  //               ;
  //
  // which will be parsed by this loop:
  //
  //  factor();
  //  while(token in first1(catExpr') ) {
  //    factor();
  //  }

  ENTERFUNC;

  factor(startp, endp);

  while(firstInCatExpr(m_scanner.getToken())) {
    NFAstate *e2_start, *e2_end;

    factor(e2_start, e2_end);
    *endp = *e2_start;
    delete e2_start;

    endp = e2_end;
  }

  LEAVEFUNC;
}

bool NFAparser::firstInCatExpr(Token tok) {
  switch(tok) {
  case RPAR       : // )
  case DOLLAR     : // $
  case BAR        : // |
  case EOS        : // 0
    return false;

  case STAR       : // *
  case PLUS       : // +
  case QUEST      : // ?
    error(_T("+ ? or * must follow an expression."));
    return false;

  case RB         : // ]
    error(_T("Missing [ in character class."));
    return false;
  case CIRCUMFLEX : // ^
    error(_T("^ must be at start of expression or characterclass."));
    return false;
  }

  return true;
}

void NFAparser::factor(NFAstate *&startp, NFAstate *&endp) {
  // factor  => term
  //          | term STAR
  //          | term PLUS
  //          | term QUEST
  //          ;

  ENTERFUNC;

  term(startp, endp);

  if(match(STAR) || match(PLUS) || match(QUEST)) {
    NFAstate *start = new NFAstate;
    NFAstate *end   = new NFAstate;
    start->m_next1  = startp;
    endp->m_next1   = end;

    if(match(STAR) || match(QUEST)) {      //   * or ?
      start->m_next2 = end;
    }

    if(match(STAR) || match(PLUS)) {       //   * or +
      endp->m_next2 = startp;
    }

    startp  = start;
    endp    = end;
    nextToken();
  }

  LEAVEFUNC;
}

void NFAparser::term(NFAstate *&startp, NFAstate *&endp) {
  //
  // term           => LB complement_opt charclass RB
  //                 | DOT
  //                 | LPAR expr RPAR
  //                 | character
  //                 ;
  //
  // charclass      => characterInterval
  //                 | epsilon
  //                 ;
  //
  // complement_opt => CIRCUMFLEX
  //                 | epsilon
  //                 ;
  //
  // The [] is nonstandard. It matches a space ' ', tab '\t', formfeed '\f', or newline '\n',
  // but not a carriage return '\r'.

  ENTERFUNC;

  if(match(LPAR)) { // ( expr )
    nextToken();
    expr( startp, endp );
    if(match(RPAR)) {
      nextToken();
    } else {
      error(_T("Missing ')'.")); // doesn't return
    }
  } else {
    if(!match(DOT) && !match(LB) ) {
      startp = new NFAstate(getLexeme());
      nextToken();
    } else { // DOT or [...]
      startp = new NFAstate(EDGE_CHCLASS);
      BitSet &chClass  = *startp->m_charClass;

      if(match(DOT)) { // . (DOT) matches everything except '\n' (for Windows also '\r')
        chClass.add('\n');
#ifdef _WIN32
        chClass.add('\r');
#endif
        chClass.invert();
      } else { // [...]
        nextToken(); // eat the '['
        bool complementCharClass = false;
        if(match(CIRCUMFLEX)) {            // Complement character class
          complementCharClass = true;
          nextToken();
        }
        if(match(RB)) {                     // [] or [^]
          chClass.add(' ').add('\t').add('\f').add('\n');
        } else {
          characterInterval(*startp->m_charClass);
        }
        if(complementCharClass) {
          chClass.add('\n'); // This will exclude '\n' if not specified in input
#ifdef _WIN32
          chClass.add('\r'); // and \r
#endif
          chClass.invert();
        }
        if(!match(RB)) {
          error(_T("Missing ] in character class."));
        }
      }
//      printcharclass(chClass);
      nextToken();
    }

    endp = startp->m_next1 = new NFAstate;
  }

  LEAVEFUNC;
}

void NFAparser::characterInterval(BitSet &set) {
  //
  // characterInterval  => ccl characterInterval'
  //                     ;
  //
  // characterInterval' => ccl characterInterval'
  //                     | epsilon
  //                     ;
  //
  // ccl                => DASH charater
  //                     | character DASH
  //                     | character DASH character
  //                     ;

  ENTERFUNC;

  if(match(DASH)) {       // Treat [-...] as a literal dash
    warning(_T("'-' at start of character class."));
    set.add(getLexeme());
    nextToken();
  }

  unsigned int first;
  while(!match(EOS) && !match(RB)) {

    ENTER("ccl");

    if(match(DASH)) {
      nextToken();
      if(match(RB)) {     // Treat [...-] as literal
        warning(_T("'-' at end of character class."));
        set.add('-');
      } else {
        set.add(first, getLexeme());
      }
    } else {
      first = getLexeme();
      set.add(first);
    }
    if(match(EOS) || match(RB)) {
      LEAVE("ccl");
      break;
    } else {
      nextToken();
      LEAVE("ccl");
    }
  }
  LEAVEFUNC;
}

AcceptAction *NFAparser::acceptString(int anchor) {
  if( *m_scanner.getInput() == '|' ) {
    if(m_lastAction == NULL) {
      m_lastAction = new AcceptAction;
    }
    return m_lastAction; // we will get the actual actionstring later
  }

  // now parse the actual action and save it
  AcceptAction *action;
  if(m_lastAction == NULL) {
    action = new AcceptAction;
  } else {
    action = m_lastAction;
    m_lastAction = NULL;
  }
  action->m_pos        = m_scanner.getRulePosition();
  action->m_sourceText = StringCollector::trimIndent(m_scanner.getInputPos(), (TCHAR*)m_scanner.getInput());
  action->m_anchor     = anchor;
//  printf(_T("action:#line %d <%s>\n"), action->m_lineno, action->m_sourcetext.cstr());
  return action;
}

NFAstate *NFAparser::rule() {
  //  rule       => anchor_bol expr anchor_eol EOS action
  //              ;
  //
  //  anchor_bol => ^
  //              | epsilon
  //              ;
  //
  //  anchor_eol => $
  //              | epsilon
  //              ;
  //
  //  action     => <tabs> <String of characters>
  //              | epsilon
  //              ;


  NFAstate *start  = NULL;
  NFAstate *end    = NULL;
  int       anchor = ANCHOR_NONE;

  ENTERFUNC;

  if(match(CIRCUMFLEX)) { // Anchor bol (beginning of line)
    start =  new NFAstate('\n');
    anchor |= ANCHOR_START ;
    nextToken();
    expr(start->m_next1, end);
  } else {
    expr(start, end);
  }

  if(match(DOLLAR)) { // Anchor eol
    // pattern followed by \r or \n (use a character class).

    nextToken();
    end->m_next1   = new NFAstate;
    end->m_edge    = EDGE_CHCLASS;
    end->m_charClass = new BitSet(MAX_CHARS);
    end->m_charClass->add('\n');
#ifdef _WIN32
    end->m_charClass->add('\r');
#endif
    end     = end->m_next1 ;
    anchor |= ANCHOR_END  ;
  }

  m_scanner.skipSpace();

  end->m_accept = acceptString(anchor);
  nextToken(); // skip past EOS

  LEAVEFUNC;

  return start;
}

NFAstate *NFAparser::machine() {
  NFAstate *start;
  NFAstate *p;

  ENTERFUNC;

  p = start  = new NFAstate;
  p->m_next1 = rule();

  while(!match(EOI)) {
    p->m_next2 = new NFAstate;
    p          = p->m_next2;
    p->m_next1 = rule();
  }

  LEAVEFUNC;

  return start;
}

void NFAparser::parseHeadBody(SourceText &source) {
  Token tok = m_scanner.hnext();
  m_scanner.collectBegin();
  while(tok != PERCENTRCURL && tok != EOI) {
    tok = m_scanner.hnext();
  }
  m_scanner.getCollected(source);
//  printf(_T("#line %d \")%s\"\n%s", lineno, m_scanner.getfname(), head.cstr());
}

void NFAparser::readHead() {
  Token tok = m_scanner.hnext();
  int section = 0;
  while(tok != PERCENTPERCENT && tok != EOI) {
    switch(tok) {
    case PERCENTLCURL:
      if(section > 1) {
        m_scanner.error(_T("Only 2 sections allowed."));
      }
      parseHeadBody((section == 0)?m_NFA.m_header:m_NFA.m_driverHead);
      section++;
      tok = m_scanner.hnext();
      break;
    case NAME:
      m_scanner.addMacro();
      tok = m_scanner.hnext();
      break;
    default:
      m_scanner.error(_T("Invalid text:%s"), m_scanner.getText().cstr());
      tok = m_scanner.hnext();
    }
  }
}

void NFAparser::readTail() {
  m_scanner.collectBegin();
  Token tok = m_scanner.hnext();
  while(tok != EOI) {
    tok = m_scanner.hnext();
  }
  m_scanner.getCollected(m_NFA.m_driverTail);
}

NFAstate *NFAparser::parse() {
  readHead();
  m_scanner.beginRuleSection();
  m_scanner.nextToken();
  NFAstate *start = machine();
  m_scanner.endRuleSection();
  readTail();
  return start;
}

void NFAparser::generateID(NFAstate *state) {
  if(state && state->m_id == -1) {
    state->m_id = m_idCounter++;
    m_NFA.add(state);
    generateID(state->m_next1);
    generateID(state->m_next2);
  }
}

static int NFAcmp(NFAstate * const &s1, NFAstate * const &s2) {
  return s1->m_id - s2->m_id;
}

void NFAparser::thompsonConstruction() {
  m_recurseLevel = 0;
  NFAstate *start = parse();
  m_idCounter = 0;
  generateID(start);
  m_NFA.sort(NFAcmp);

//  for(unsigned int i = 0; i < m_states.size(); i++)
//    m_states[i]->print();

}

NFAparser::NFAparser(const String &fname, NFA &nfa) : m_scanner(fname), m_NFA(nfa) {
  m_lastAction = NULL;
}
