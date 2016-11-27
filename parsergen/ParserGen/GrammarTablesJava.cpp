#include "stdafx.h"
#include "GrammarCode.h"

static void printUncompressedActionArrayJava(MarginFile &output, const char *tableType, const int index, const ActionArray &actions) {
  const int actionCount = (int)actions.size();
  output.setLeftMargin(4);
  output.printf(_T("%s act%04d[] = { 0"), tableType, index);
  for(int a = 0; a < actionCount; a++) {
    const ParserAction &pa = actions[a];
    output.printf(_T(",%4d,%4d"), pa.m_token, pa.m_action);
    if(a % 5 == 4 && a != actionCount-1) {
      output.printf(_T("\n"));
      output.setLeftMargin(46);
    }
  }
  output.printf(_T("};\n"));
}

static void printCompressedActionArrayJava(MarginFile &output, const char *tableType, const int index, const ActionArray &actions) {
  const int actionCount = (int)actions.size();
  output.setLeftMargin(4);
  output.printf(_T("%s act%04d[] = { 1"), tableType, index);
  for(int a = 0; a < actionCount; a++) {
    const ParserAction &pa = actions[a];
    output.printf(_T(",%4d"), pa.m_token);
    if(a % 10 == 9) {
      output.printf(_T("\n"));
      output.setLeftMargin(46);
    }
  }
  output.printf(_T(",%4d};\n"), actions[0].m_action);
}

void GrammarTables::printJava(MarginFile &output, bool useTableCompression) const {
  output.printf(
       _T("/*****************************************************************\\\n"
          "* The action matrix holds the parse action(state,terminal)        *\n"
          "* Used in LRParser::parserStep().                                 *\n"
          "* 2 different formats are used:Compressed and uncompressed.       *\n"
          "*                                                                 *\n"
          "* Uncompressed:                                                   *\n"
          "*                  act0001[] =           { 0, 1,2, 2,-2, 6,0 };   *\n"
          "*   Current state------+                   |  | |                 *\n"
          "*   0 indicates uncompressed format--------+  | |                 *\n"
          "*   Legal input symbol------------------------+ |                 *\n"
          "*   Action--------------------------------------+                 *\n"
          "*                                                                 *\n"
          "* Compressed:When all actions are reduce by the same production.  *\n"
          "*                  act0002[] =           { 1, 1, 2, 6, -3 };      *\n"
          "*   Current state------+                   |  |  |  |   |         *\n"
          "*   1 indicates compressed format----------+  |  |  |   |         *\n"
          "*   Legal input symbol------------------------+--+--+   |         *\n"
          "*   Action----------------------------------------------+         *\n"
          "*                                                                 *\n"
          "*   action = getAction(current state, current inputSymbol);       *\n"
          "*                                                                 *\n"
          "*   action <  0   - Reduce by production p, p == -action.         *\n"
          "*   action == 0   - Accept. Reduce by production 0.               *\n"
          "*   action >  0   - Go to state s (=action),                      *\n"
          "*                   and push [s,input,pos] to parser stack.       *\n"
          "*                   Then read next symbol from input.             *\n"
          "*   action == parserError - not found (=unexpected input).        *\n"
          "\\*****************************************************************/\n")
         );

  const int stateCount      = getStateCount();
  const int productionCount = getProductionCount();

  const char *tableType = "private static final short";

  for(int s = 0; s < stateCount; s++) {
    const ActionArray &actions = m_stateActions[s];
    if(useTableCompression && isCompressibleState(s)) {
      printCompressedActionArrayJava(output, tableType, s, actions);
    } else {
      printUncompressedActionArrayJava(output, tableType, s, actions);
    }
  }

  output.setLeftMargin(4);
  output.printf(_T("%s action[][] = {\n"), tableType);
  output.setLeftMargin(8);
  for(int s = 0; s < stateCount; s++) {
    if(s % 10 == 0) {
      output.printf(_T("/* %4d */ "), s);
    }
    output.printf(s > 0 ? _T(","):_T(" "));
    output.printf(_T("act%04d"), s);
    if(s % 10 == 9 || s == stateCount-1) {
      output.printf(_T("\n"));
    }
  }
  output.setLeftMargin(4);
  output.printf(_T("};\n\n"));

  output.setLeftMargin(4);
  output.printf(
       _T("/****************************************************************************\\\n"
          "* The successor matrix is used when the parser has reduced by prod A -> alfa *\n"
          "* Used by LRParser to compute newState = successor(state,A).                 *\n"
          "*                                                                            *\n"
          "*                  succ0007[] = { 5,3 , 6,2, 8,5 };                          *\n"
          "*   Current state-------+         | |                                        *\n"
          "*   NonTerminal A-----------------+ |                                        *\n"
          "*   Go to this state----------------+                                        *\n"
          "\\****************************************************************************/\n")
         );
  BitSet hasSuccessor(stateCount);
  for(int s = 0; s < stateCount; s++) {
    const ActionArray &succlist = m_stateSucc[s];
    const int count = (int)succlist.size();
    if(count > 0) {
      output.setLeftMargin(4);
      output.printf(_T("%s succ%04d[] = {"), tableType, s);
      char *delim = "";
      for(int a = 0; a < count; a++, delim=",") {
        const ParserAction &pa = succlist[a];
        output.printf(_T("%s%4d,%4d"), delim, pa.m_token, pa.m_action);
        if(a % 5 == 4 && a != count-1) {
          output.printf(_T("\n"));
          output.setLeftMargin(44);
        }
      }
      output.printf(_T("};\n"));
      hasSuccessor += s;
    }
  }

  output.setLeftMargin(4);
  output.printf(_T("%s successor[][] = {\n"), tableType);
  output.setLeftMargin(8);
  for(int s = 0; s < stateCount; s++) {
    if(s % 10 == 0) {
      output.printf(_T("/* %4d */ "), s);
    }
    output.printf(s > 0 ? _T(","):_T(" "));
    if(hasSuccessor.contains(s)) {
      output.printf(_T("succ%04d"), s);
    } else {
      output.printf(_T("null    "));
    }
    if(s % 10 == 9 || s == stateCount-1) {
      output.printf(_T("\n"));
    }
  }
  output.setLeftMargin(4);
  output.printf(_T("};\n\n"));

  output.setLeftMargin(4);
  output.printf(
       _T("/*********************************************************************\\\n"
          "* The leftSide[] array is indexed by production number, and holds the *\n"
          "* nonTerminal symbol on the left side of each production.             *\n"
          "\\*********************************************************************/\n")
         );
  output.printf(_T("%s leftSide[] = {\n"), tableType);
  output.setLeftMargin(8);
  for(int p = 0; p < productionCount; p++) {
    int l = m_left[p];
    if(p % 10 == 0) {
      output.printf(_T("/* %3d */ "), p);
    }
    output.printf(_T(" %3d"), l);
    if(p < productionCount-1) {
      output.printf(_T(","));
    }
    if(p % 10 == 9) {
      output.printf(_T("\n"));
    }
  }
  output.setLeftMargin(4);
  output.printf(_T("\n};\n\n"));

  output.setLeftMargin(4);
  output.printf(
       _T("/******************************************************************\\\n"
          "* The rightSide[] matrix is indexed by production number and holds *\n"
          "* the right side symbols of each production.                       *\n"
          "* Used for debugging.                                              *\n"
          "\\******************************************************************/\n")
         );
  for(int p = 0; p < productionCount; p++) {
    const CompactIntArray &r = m_rightSide[p];
    if(r.size() == 0) {
      continue;
    }
    output.printf(_T("%s right%04d[] = {"), tableType, p);
    for(size_t i = 0; i < r.size(); i++) {
      if(i > 0) {
        output.printf(_T(","));
      }
      output.printf(_T("%3d"), r[i]);
    }
    output.printf(_T("};\n"));
  }
  output.printf(_T("\n"));

  output.setLeftMargin(4);
  output.printf(_T("%s rightSide[][] = {\n"), tableType);
  output.setLeftMargin(8);
  for(int p = 0; p < productionCount; p++) {
    if(m_rightSide[p].size() == 0) {
      output.printf(_T("null     "));
    } else {
      output.printf(_T("right%04d"), p);
    }
    if(p < productionCount-1) {
      output.printf(_T(","));
    }
    if(p % 10 == 9 || p == productionCount-1) {
      output.printf(_T("\n"));
    }
  }
  output.setLeftMargin(4);
  output.printf(_T("};\n\n"));

  output.setLeftMargin(4);
  output.printf(
       _T("/*******************************************************\\\n"
          "* The symbolName[] Array is indexed by symbol and holds *\n"
          "* the name of each symbol (terminal or nonTerminal).    *\n"
          "* Used for debugging.                                   *\n"
          "\\*******************************************************/\n")
         );
  const int terminalCount = getTerminalCount();
  const int symbolCount   = getSymbolCount();
  output.printf(_T("private static final String symbolName[] = {\n"));
  output.setLeftMargin(8);
  for(int s = 0; s < symbolCount; s++) {
    output.printf(_T("/* %3d */ \"%s\""), s, m_symbolName[s].cstr());
    if(s < symbolCount-1) {
      output.printf(_T(","));
    }
    output.printf(_T("\n"));
  }
  output.setLeftMargin(4);
  output.printf(_T("};\n\n"));
}
