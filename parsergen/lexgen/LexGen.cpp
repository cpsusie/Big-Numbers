#include "stdafx.h"
#include <direct.h>
#include <io.h>
#include "DFA.h"
#include "Wizard.h"
#include <FileNameSplitter.h>
#include <TemplateWriter.h>

class ActionsWriter : public KeywordHandler {
  const DFA &m_dfa;
private:
public:
  ActionsWriter(const DFA &dfa);
  void handleKeyword(TemplateWriter &writer, String &line) const override;
};

ActionsWriter::ActionsWriter(const DFA &dfa) : m_dfa(dfa) {
}

void ActionsWriter::handleKeyword(TemplateWriter &writer, String &line) const {
  const Options               &options    = Options::getInstance();
  const SourcePositionWithName breakPos   = writer.getPos();
  const UINT                   stateCount = m_dfa.getStateCount();
  BitSet                       done(stateCount);
  for(UINT i = 0; i < stateCount; i++) {
    const DFAstate &s = m_dfa.m_states[i];
    if(s.m_accept && !done.contains(i)) {
      for(UINT j = i; j < stateCount; j++) {
        if((m_dfa.m_states[j].m_accept == s.m_accept) && !done.contains(j)) {
          writer.printf(_T("case %u:\n"), j);
          done.add(j);
        }
      }
      writer.incrLeftMargin(2);
      writer.writeSourceText(*s.m_accept);
      if(options.m_generateBreaks) {
        writer.printf(_T("break;\n"));
      }
      writer.decrLeftMargin(2);
    }
  }
}

class TablesWriter : public KeywordHandler {
  const DFA &m_dfa;
public:
  TablesWriter(const DFA &dfa);
  void handleKeyword(TemplateWriter &writer, String &line) const override;
};

TablesWriter::TablesWriter(const DFA &dfa) : m_dfa(dfa) {
}

void TablesWriter::handleKeyword(TemplateWriter &writer, String &line) const {
  const Options               &options     = Options::getInstance();
  const SourcePositionWithName pos         = writer.getPos();
  const String                 tmpFileName = TemplateWriter::createTempFileName(_T("txt"));
  MarginFile f(tmpFileName);
  if(options.m_verbose) {
    m_dfa.printStates(f);
  }
  m_dfa.printTables(f);
  f.close();
  const String text = readTextFile(tmpFileName);
  unlink(tmpFileName);
  writer.printf(_T("%s"), text.cstr());
  if(options.m_lineDirectives) {
    writer.writeLineDirective(pos.getName(),pos.getLineNumber()+1);
  }
}

static void printOutputFiles(const String    &lexFileName
                            ,const DFA       &dfa
                            ) {
  const Options &options    = Options::getInstance();
  const String   lexName    = FileNameSplitter(lexFileName).getFileName();
  const String   sourceName = FileNameSplitter(lexFileName).getAbsolutePath();

  TemplateWriter       writer;
  SourceTextWriter     headerWriter(    dfa.getHeader()    );
  SourceTextWriter     driverHeadWriter(dfa.getDriverHead());
  SourceTextWriter     driverTailWriter(dfa.getDriverTail());
  ActionsWriter        actionsWriter(   dfa);
  TablesWriter         tablesWriter(    dfa);

  writer.addKeywordHandler(_T("FILEHEAD"           ), headerWriter         );
  writer.addKeywordHandler(_T("CLASSHEAD"          ), driverHeadWriter     );
  writer.addKeywordHandler(_T("CLASSTAIL"          ), driverTailWriter     );
  writer.addKeywordHandler(_T("TABLES"             ), tablesWriter         );
  writer.addKeywordHandler(_T("ACTIONS"            ), actionsWriter        );

  writer.addMacro(_T("LEXNAME"   ), lexName         );
  writer.addMacro(_T("STATECOUNT"), toString(dfa.getStateCount()));

  writer.generateOutput();
}

static void usage() {
  _ftprintf(stderr,
    _T("Usage:lexgen [options] input.\n"
       " -mS: Use String S as template rather than lexgenXXX.par.\n"
       " -l : Suppress #line directives in output.\n"
       " -b : Suppress break-statements in output.\n"
       " -Nnamespace: Wrappes everything into a namespace with the given name.\n"
       "      Applies only to c++-code.\n"
       " -v[level] :Verbose.\n"
       "    level: 0: print DFA-states to generated cpp-file.\n"
       "         : 1: dump NFA- and DFA-states to stdout.\n"
       "         : 2: dump DFA-construction steps stdout.\n"
       "    Default level = 0.\n"
       " -wS: lex-wizard. write template lex-file with classname Slex to stdout.\n"
       " -j : Generate java-lex. Default is C++.\n"
       " -Ooutputdir1[,outputdir2]: Output goes to outputdir1. If outputdir2 specified, .h-files will go here.\n")
  );
  exit(-1);
}

int _tmain(int argc, TCHAR **argv) {
  TCHAR   *cp;
  Options &options       = Options::getInstance();
  options.m_verboseLevel = 0;
  try {
    for(argv++; *argv && *(cp = *argv) == '-'; argv++) {
      for(cp++;*cp;cp++) {
        switch(*cp) {
        case 'm':
          options.m_templateName = cp+1;
          if(options.m_templateName.length() == 0) {
            usage();
          }
          break;
        case 'N':
          options.m_nameSpace = cp+1;
          if(options.m_nameSpace.length() == 0) {
            usage();
          }
          break;
        case 'l':
          options.m_lineDirectives = false;
          continue;
        case 'b':
          options.m_generateBreaks = false;
          continue;
        case 'v':
          { options.m_verbose      = true;
            options.m_verboseLevel = 0;
            UINT level;
            if((_stscanf(cp+1, _T("%u"), &level) != 1)) {
              continue;
            }
            if(level > Options::maxVerboseLevel) {
              usage();
            }
            options.m_verboseLevel = level;
          }
          break;
        case 'w':
          options.m_callWizard = true;
          options.m_wizardName = cp+1;
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
            break;
          }
        case 'j':
          options.m_language = JAVA;
          continue;
        default:
          usage();
          break;
        } // switch(..
        break;
      } // for(cp...)
    } // for(argv++;

    String skeletonFileName = EMPTYSTRING;
    String wizardTemplate   = EMPTYSTRING;
    switch(options.m_language) {
    case CPP :
      skeletonFileName = _T("lexgencpp.par");
      wizardTemplate   = _T("lexgencpp.wzr");
      break;
    case JAVA:
      skeletonFileName = _T("lexgenjava.par");
      wizardTemplate   = _T("lexgenjava.wzr");
      options.m_lineDirectives   = false;
      break;
    default  :
      usage();
      break;
    }

    if(options.m_callWizard) {
      String className = FileNameSplitter(options.m_wizardName).getFileName();
      wizard(stdout, wizardTemplate, className);
    } else {
      if(!*argv) {
        usage();
      }
      String lexFileName = *argv;

      options.checkTemplateExist(skeletonFileName);

      NFA nfa;
      NFAparser parser(lexFileName, nfa);

      if(options.m_verbose) {
        _tprintf(_T("Parsing %s\n"), lexFileName.cstr());
      }
      parser.thompsonConstruction();

      if(options.m_verboseLevel >= 1) {
        for(size_t i = 0; i < nfa.size(); i++) {
          nfa[i]->print(stdoutMarginFile);
        }
      }

      if(options.m_verbose) {
        _tprintf(_T("Constructing DFA\n"));
      }

      DFA dfa(nfa);
      if(options.m_verboseLevel == 1) {
        const UINT stateCount = dfa.getStateCount();
        for(UINT i = 0; i < stateCount; i++) {
          dfa.m_states[i].print(stdoutMarginFile);
        }
      }
      printOutputFiles(lexFileName, dfa);
    }
  } catch(Exception e) {
    _ftprintf(stderr, _T("%s\n"), e.what());
    return -1;
  }
  return 0;
}
