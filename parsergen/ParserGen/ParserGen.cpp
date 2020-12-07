#include "stdafx.h"
#include <io.h>
#include <Tokenizer.h>
#include <ProcessTools.h>
#include <FileNameSplitter.h>
#include "GrammarParser.h"
#include "Grammar.h"
#include "GrammarResult.h"
#include "GrammarCode.h"
#include "Wizard.h"

static void usage() {
  _ftprintf(stderr,
    _T("Usage:parsergen [options] file\n"
       " -mS: Use String S as template rather than parsergenXXX.par.\n"
       " -l : Suppress #line directives in output.\n"
       " -b : Suppress break-statements in output.\n"
       " -a : No reduceactions generated.\n"
       " -Nnamespace: Wraps everything into a namespace with the given name.\n"
       "      Applies only to c++-code.\n"
       " -n : Generate nonterminalsymbols in XXXSymbol.h/XXXSymbol.java).\n"
       " -h : Write lookahead symbols in docfile.\n"
       " -C[comperess options...]"
       "  where compress option is a comma separated list, each beginning with a selector, 'R,S or N\n"
       "      R = Reduce Matrix, S = Shift Matrix, N = Next state matrix\n"
       "      and each matrix has 2 parameters:\n"
       "        r<recurseLevel> (recurselevel=[0..%u], default recurselevel=%u)\n"
       "        s<minBitSetSize>\n"
       " -F : Find optimal compression parameters (max recurse levels, minimal bitsetsize\n"
       "      Does not generate a parser or tables, but print memoryusage for parsertables to stdout for various combinations of compress-parameters\n"
       " -v[level]:verbose.\n"
       "     level = 0 -> silence.\n"
       "     level = 1 -> write main steps in process.\n"
       "     level = 2 -> write warnings to stdout.\n"
       "     level = 3 -> write compression node trees to stdout.\n"
       "     Default level is -v%u.\n"
       " -ffile :dump first1-sets to file.\n"
       " -wS: Parsergen-wizard. write template grammar-file with classname S to stdout.\n"
       " -Ooutputdir1[,outputdir2]: Output goes to outputdir1. If outputdir2 specified, .h-files will go here.\n"
       " -ttabsize:Tabulatorcharater will expand to this many spaces. tabsize=[1..%u]. Default tabsize=%u.\n"
       " -j : Generate java-parser. Default is C++.\n")
      ,Options::maxRecursiveCalls
      ,MatrixOptimizeParameters::defaultRecurseLevel
      ,Options::defaultVerboseLevel
      ,Options::maxTabSize
      ,Options::defaultTabSize
  );
  exit(-1);
}

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
      for(cp++; *cp; cp++) {
        switch(*cp) {
        case 'm':
          options.m_templateName = cp+1;
          if(options.m_templateName.length() == 0) {
            usage();
          }
          break;
        case 'C':
          { cp++;
            MatrixOptimizeParameters *optParam = nullptr;
            for(Tokenizer tok(cp, _T(",")); tok.hasNext();) {
              String cs = tok.next();
              for(TCHAR *cp1 = cs.cstr(); *cp1; cp1++) {
#define CHECKOPT() if(optParam == nullptr) usage(); (*optParam)
                switch(*cp1) {
                case 'R': optParam = &options.getOptimizeParameters(OPTPARAM_REDUCE); continue;
                case 'S': optParam = &options.getOptimizeParameters(OPTPARAM_SHIFT ); continue;
                case 'N': optParam = &options.getOptimizeParameters(OPTPARAM_SUCC  ); continue;
                case 'r': cp1++;  CHECKOPT().m_maxRecursion  = parseUINT(cp1);        break;
                case 's': cp1++;  CHECKOPT().m_minBitSetSize = parseUINT(cp1);        break;
                default : usage();
                }
                break;
              }
            }
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
        case 'F':
          options.m_findOptimalTableCompression = true;
          continue;
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

//      debugLog(_T("Options:\n%s"), options.toString().cstr());

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

        if(!grammar.getResult().allStatesConsistent()) {
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
