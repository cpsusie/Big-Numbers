#include "stdafx.h"
#include <io.h>
#include <Tokenizer.h>
#include <ProcessTools.h>
#include <FileNameSplitter.h>
#include "GrammarParser.h"
#include "GrammarCode.h"
#include "Wizard.h"

static void usage() {
  _ftprintf(stderr,
    _T("Usage:parsergen [options] file\n"
       " -mS: Use String S as template rather than parsergenXXX.par.\n"
       " -l : Suppress #line directives in output.\n"
       " -b : Suppress break-statements in output.\n"
       " -a : No reduceactions generated.\n"
       " -Nnamespace: Wrappes everything into a namespace with the given name.\n"
       "      Applies only to c++-code.\n"
       " -n : Generate nonterminalsymbols in XXXSymbol.h/XXXSymbol.java).\n"
       " -h : Write lookahead symbols in docfile.\n"
       " [-c|+c<level>]: Disable or enable parser tables compression. If enabled, level specifies max number of recursive calls\n"
       "                 to determine parseraction. level=[0..%u]. Default is +c%u\n"
       " -T[,p][,r<maxlevel>][,m<minBitSetSize>]"
       "    : Compress successor matrix with same technique as action-matrix compression, but on the transposed successor-matrix.\n"
       "      p               : Prune matrix, if there only 1 newstate for all possible from-states to a given nterm. ie no check is done.\n"
       "      r<maxLevel>     : Max recurse level when following splitnodes, actually a linked list\n"
       "      m<minBitSetSize>: Make only stateBitSets which will contain at least the specified number of 1-bits. Must be >= 2. Default minbitsetsize=2\n"
       " -v[level]:verbose.\n"
       "     level = 0 -> silence.\n"
       "     level = 1 -> write main steps in process.\n"
       "     level = 2 -> write warnings to stdout.\n"
       "     Default level is -v%u.\n"
       " -ffile :dump first1-sets to file.\n"
       " -wS: Parsergen-wizard. write template grammar-file with classname S to stdout.\n"
       " -Ooutputdir1[,outputdir2]: Output goes to outputdir1. If outputdir2 specified, .h-files will go here.\n"
       " -ttabsize:Tabulatorcharater will expand to this many spaces. tabsize=[1..%u]. Default tabsize=%u.\n"
       " -j : Generate java-parser. Default is C++.\n")
      ,Options::maxRecursiveCalls
      ,Options::defaultRecurseLevel
      ,Options::defaultVerboseLevel
      ,Options::maxTabSize
      ,Options::defaultTabSize
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

static UINT parseUINT(TCHAR *&cp) {
  TCHAR *endp = nullptr;
  const UINT v = _tcstoul(cp, &endp, 10);
  if(v) cp = endp;
  return v;
}

int _tmain(int argc, TCHAR **argv) {
  TCHAR    *cp;
  Options  &options = Options::getInstance();
  try {
    for(argv++; *argv && _tcschr(_T("-+"), *(cp = *argv)); argv++) {
      if(*cp == '+') {
        cp++;
        switch(*cp) {
        case 'c':
          { options.m_useTableCompression  = true;
            cp++;
            const UINT maxRecurseLevel = parseUINT(cp);
            if(maxRecurseLevel > Options::maxRecursiveCalls) {
              usage();
            }
            options.m_maxRecursionAction = maxRecurseLevel;
          }
          break;
        default:
          usage();
        }
        continue;
      }
      // *cp = '-'
      for(cp++; *cp; cp++) {
        switch(*cp) {
        case 'm':
          options.m_templateName = cp+1;
          if(options.m_templateName.length() == 0) {
            usage();
          }
          break;
        case 'v':
          { UINT level;
            if((_stscanf(cp+1, _T("%u"), &level) != 1)) {
              options.m_verboseLevel = 1;
              continue;
            }
            if(level > Options::maxVerboseLevel) {
              usage();
            }
            options.m_verboseLevel = level;
          }
          break;
        case 'l':
          options.m_lineDirectives         = false;
          continue;
        case 'b':
          options.m_generateBreaks         = false;
          continue;
        case 'a':
          options.m_generateActions        = false;
          continue;
        case 'h':
          options.m_generateLookahead      = true;
          continue;
        case 'c':
          options.m_useTableCompression    = false;
          options.m_maxRecursionAction     = 0;
          continue;
        case 'T':
          { options.m_compressSuccTransposed = true;
            cp++;
            if(*cp) {
              for(Tokenizer tok(cp, _T(",")); tok.hasNext();) {
                String s = tok.next();
                for(TCHAR *cp1 = s.cstr(); *cp1;) {
                  switch(*cp1) {
                  case 'p':
                    options.m_pruneSuccTransBitSet = true;
                    cp1++;
                    break;
                  case 'm':
                    { cp1++;
                      const UINT v = parseUINT(cp1);
                      if((v < 2)) {
                        usage();
                      }
                      options.m_minStateBitSetSize = v;
                    }
                    break;
                  case 'r':
                    { cp1++;
                      const UINT v = parseUINT(cp1);
                      if(v > Options::maxRecursiveCalls) {
                        usage();
                      }
                      options.m_maxRecursionTransSucc = v;
                    }
                    break;
                  default:
                    usage();
                  }
                }
              }
            }
          }
          break;
        case 'n':
          options.m_generateNonTerminals   = true;
          continue;
        case 'f':
          options.m_first1File = cp+1;
          if(options.m_first1File.length() == 0) {
            usage();
          }
          break;
        case 'O':
          { const String a = cp+1;
            Tokenizer tok(a, _T(","));
            if(tok.hasNext()) {
              options.m_implOutputDir = tok.next();
            } else {
              usage();
            }
            if(tok.hasNext()) {
              options.m_headerOutputDir = tok.next();
            } else {
              options.m_headerOutputDir = options.m_implOutputDir;
            }
            if(tok.hasNext()) {
              usage();
            }
          }
          break;

        case 'w':
          options.m_callWizard = true;
          options.m_wizardName = cp+1;
          break;
        case 't':
          { cp++;
            const UINT tabSize = parseUINT(cp);
            if((tabSize < 1) || (tabSize > Options::maxTabSize)) {
              usage();
            }
            options.m_tabSize = (BYTE)tabSize;
          }
          break;
        case 'j':
          options.m_language = JAVA;
          continue;
        case 'N':
          options.m_nameSpace = cp+1;
          if(options.m_nameSpace.length() == 0) {
            usage();
          }
          break;
        default :
          usage();
        }
        break;
      }
    }

    String skeletonFileName = EMPTYSTRING;
    String wizardTemplate   = EMPTYSTRING;
    switch(options.m_language) {
	  case CPP :
      skeletonFileName = _T("parsergencpp.par");
      wizardTemplate   = _T("parsergencpp.wzr");
      break;
	  case JAVA:
      skeletonFileName = _T("parsergenjava.par");
      wizardTemplate   = _T("parsergenjava.wzr");
	  options.m_lineDirectives   = false;
      break;
    default  :
      usage();
      break;
    }

    bool ok = true;
    if(options.m_callWizard) {
      wizard(stdout, wizardTemplate, options.m_wizardName);
    } else {
      if(!*argv) {
        usage();
      }

      options.checkTemplateExist(skeletonFileName);

      String grammarFileName = FileNameSplitter(*argv).getAbsolutePath();
      Grammar grammar;
      GrammarParser parser(grammarFileName, grammar);
      parser.readGrammar();

      if(!parser.ok()) {
        ok = false;
      } else {
        double starttime = getProcessTime();

        grammar.generateStates();
  //      checkhas2reduce(g);

        GrammarCode code(grammar);

        if(options.m_first1File.length() != 0) {
          FILE *f = MKFOPEN(options.m_first1File, _T("w"));
          grammar.dumpFirst1Sets(f);
          fclose(f);
        }

        if(!grammar.allStatesConsistent()) {
          ok = false;
          code.generateDocFile();
          verbose(1, _T("Time:%.3lf\n"), (getProcessTime() - starttime)/1000000);
          verbose(1, _T("%4u\tterminals\n")     , grammar.getTermCount()       );
          verbose(1, _T("%4u\tnonterminals\n")  , grammar.getNTermCount()      );
          verbose(1, _T("%4u\tproductions\n")   , grammar.getProductionCount() );
          _tprintf(_T("**** The grammar is NOT LALR(1) ****\n"));
        } else {
          verbose(2, _T("Generate skeletonparser\n"));
          code.generateParser();
          code.generateDocFile();
          verbose(1, _T("Time:%.3lf\n"), (getProcessTime() - starttime)/1000000);
          verbose(1, _T("%4u\tterminals\n")     , grammar.getTermCount()       );
          verbose(1, _T("%4u\tnonterminals\n")  , grammar.getNTermCount()      );
          verbose(1, _T("%4u\tproductions\n")   , grammar.getProductionCount() );
          verbose(1, _T("%4u\tLALR(1) states\n"), grammar.getStateCount()      );
          verbose(1, _T("%4u\titems\n")         , grammar.getItemCount()       );
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
