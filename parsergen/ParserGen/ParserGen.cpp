#include "stdafx.h"
#include <Tokenizer.h>
#include <io.h>
#include "GrammarParser.h"
#include "GrammarCode.h"
#include "Wizard.h"

static int verboseLevel = 1;
static void verbose(int level, TCHAR *format, ...) {
  if(level <= verboseLevel) {
    va_list argptr;
    va_start(argptr, format);
    _vtprintf(format, argptr);
    va_end(argptr);
  }
}

static void usage() {
  fprintf(stderr,
    "Usage:parsergen [options] file\n"
    " -mS: Use String S as template rather than parsergenXXX.par.\n"
    " -l : Suppress #line directives in output.\n"
    " -b : Suppress break-statements in output.\n"
    " -a : No reduceactions generated.\n"
    " -n : Generate nonterminalsymbols in XXXSymbol.h/XXXSymbol.java).\n"
    " -h : Write lookahead symbols in docfile.\n"
    " -c : Disable parser tables compression. (states where all actions are reduce with the same prod). Default is on\n"
    " -v[level]:verbose.\n"
    "     level = 0 -> silence.\n"
    "     level = 1 -> write main steps in process.\n"
    "     level = 2 -> write warnings to stdout.\n"
    "     default level is 1.\n"
    " -ffile :dump first1-sets to file.\n"
    " -wS: Parsergen-wizard. write template grammar-file with classname S to stdout.\n"
    " -Ooutputdir1[,outputdir2]: Output goes to outputdir1. If outputdir2 specified, .h-files will go here.\n"
    " -ttabsize:Tabulatorcharater will expand to this many spaces. tabsize >= 1. default tabsize=4.\n"
    " -j : Generate java-parser. Default is C++.\n"
  );
  exit(-1);
}

/*
void checkhas2reduce(Grammar &g) {
  for(unsigned int i = 0; i < g.m_stateactions.size(); i++) {
    actionlist &l = g.m_stateactions[i];
    for(unsigned int j = 0; j < l.size(); j++) {
      parseraction &p1 = l[j];
      if(p1.m_action < 0) {
        for(unsigned int k = j; k < l.size();  k++) {
          if(l[k].m_action < 0 && l[k].m_action != p1.m_action)
            printf("state:%d\n", i);
        }
      }
    }
  }
}
*/

int main(int argc, char **argv) {
  char     *cp;
  String    implOutputDir   = _T(".");
  String    headerOutputDir = implOutputDir;
  String    templateName    = _T("");
  CodeFlags flags;
  bool      callWizard      = false;
  Language  language        = CPP;
  char     *wizardName      = "";
  int       tabSize         = 4;
  String    first1File      = "";

  redirectDebugLog();
  try {
    for(argv++; *argv && *(cp = *argv) == '-'; argv++) {
      for(cp++; *cp; cp++) {
        switch(*cp) {
        case 'm':
          templateName = cp+1;
          if(templateName.length() == 0) {
            usage();
          }
          break;
        case 'v':
          if(sscanf(cp+1, "%d", &verboseLevel) != 1) {
            verboseLevel = 1; 
            continue;
          }
          if(verboseLevel < 0 || verboseLevel > 2) {
            usage();
          }
          break;
        case 'l':
          flags.m_lineDirectives       = false;
          continue;
        case 'b':
          flags.m_generateBreaks       = false;
          continue;
        case 'a':
          flags.m_generateActions      = false;
          continue;
        case 'h':
          flags.m_generateLookahead    = true;
          continue;
        case 'c':
          flags.m_useTableCompression  = false;
          continue;
        case 'n':
          flags.m_generateNonTerminals = true;
          continue;
        case 'f': 
          first1File = cp+1;
          if(first1File.length() == 0) {
            usage();
          }
          break;
        case 'O':
          { const String a = cp+1;
            Tokenizer tok(a, ",");
            if(tok.hasNext()) {
              implOutputDir = tok.next();
            } else {
              usage();
            }
            if(tok.hasNext()) {
              headerOutputDir = tok.next();
            } else {
              headerOutputDir = implOutputDir;
            }
            if(tok.hasNext()) {
              usage();
            }
          }
          break;
           
        case 'w':
          callWizard = true;
          wizardName = cp+1;
          break;
        case 't':
          if(sscanf(cp+1, "%d", &tabSize) != 1 || tabSize < 1) {
            usage();
          }
          break;
        case 'j':
          language = JAVA;
          continue;
        default :
          usage();   
        }
        break;
      }
    }

    String skeletonFileName = _T("");
    String wizardTemplate   = _T("");
    switch(language) {
	  case CPP :
      skeletonFileName = _T("parsergencpp.par");
      wizardTemplate   = _T("parsergencpp.wzr");
      break;
	  case JAVA:
      skeletonFileName = _T("parsergenjava.par");
      wizardTemplate   = _T("parsergenjava.wzr");
	    flags.m_lineDirectives   = false;
      break;
    default  :
      usage();
      break;
    }

    bool ok = true;
    if(callWizard) {
      wizard(stdout, wizardTemplate, wizardName);
    } else {
      if(!*argv) {
        usage();
      }

      if(templateName.length() == 0) { // template not specified in argv
        templateName = searchenv(skeletonFileName, "LIB");
        if(templateName.length() == 0) {
          throwException(_T("template <%s> not found in environment LIB-path\n"), skeletonFileName.cstr());
        }
      } else { // -mS options used. Check if templatefile S exist
        if(ACCESS(templateName, 0) < 0) {
          throwException(_T("Template <%s> not found"), templateName.cstr());
        }
      }

      String grammarFileName = FileNameSplitter(*argv).getAbsolutePath();
      Grammar grammar(language, verboseLevel);
      GrammarParser parser(grammarFileName, tabSize, grammar);
      parser.readGrammar();

      if(!parser.ok()) {
        ok = false;
      } else {
        double starttime = getProcessTime();

        grammar.generateStates();
  //      checkhas2reduce(g);

        GrammarCoder code(templateName
                         ,grammar
                         ,implOutputDir
                         ,headerOutputDir
                         ,flags);

        if(first1File.length() != 0) {
          FILE *f = MKFOPEN(first1File, "w");
          grammar.dumpFirst1Sets(f);
          fclose(f);
        }

        if(!grammar.allStatesConsistent()) {
          ok = false;
          code.generateDocFile();
          verbose(1, _T("Time:%.3lf\n"), (getProcessTime() - starttime)/1000000);
          verbose(1, _T("%4d\tterminals\n")     , grammar.getTerminalCount()   );
          verbose(1, _T("%4d\tnonterminals\n")  , grammar.getNonTerminalCount());
          verbose(1, _T("%4d\tproductions\n")   , grammar.getProductionCount() );
          _tprintf(_T("**** The grammar is NOT LALR(1) ****\n"));
        } else {
          verbose(2, _T("Generate skeletonparser\n"));
          code.generateParser();
          code.generateDocFile();
          verbose(1, _T("Time:%.3lf\n"), (getProcessTime() - starttime)/1000000);
          verbose(1, _T("%4d\tterminals\n")     , grammar.getTerminalCount()   );
          verbose(1, _T("%4d\tnonterminals\n")  , grammar.getNonTerminalCount());
          verbose(1, _T("%4d\tproductions\n")   , grammar.getProductionCount() );
          verbose(1, _T("%4d\tLALR(1) states\n"), grammar.getStateCount()      );
          verbose(1, _T("%4d\titems\n")         , grammar.getItemCount()       );
          verbose(1, _T("**** The grammar is LALR(1) ****\n"));
  //      grammar.dump();
        }
      }
    }
    return ok ? 0 : -1;
  } catch(Exception e) {
    _ftprintf(stderr, _T("%s\n"), e.what());
    return -1;
  }
}
