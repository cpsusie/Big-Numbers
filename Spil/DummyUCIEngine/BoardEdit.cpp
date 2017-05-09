#include "stdafx.h"

void Board::setup(const String &str) { // static 
  for(Tokenizer tok(str, _T(" ")); tok.hasNext();) {
    String command = tok.next();
    if(command == _T("fen")) {
      s_bc.setupBoard(tok);
    } else if(command == _T("startpos")) {
      s_bc.setupStartBoard();
    }
    initHistory();
    while(tok.hasNext()) {
      if(tok.next() == _T("moves")) {
        while(tok.hasNext()) {
          executeMove(Move(tok.next()));
        }
      }
    }
  }
}
