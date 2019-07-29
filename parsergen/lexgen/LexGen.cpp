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
  void handleKeyword(TemplateWriter &writer, String &line) const;
};

ActionsWriter::ActionsWriter(const DFA &dfa) : m_dfa(dfa) {
}

void ActionsWriter::handleKeyword(TemplateWriter &writer, String &line) const {
  const SourcePositionWithName breakPos   = writer.getPos();
  const size_t                 stateCount = m_dfa.m_states.size();
  BitSet                       done(stateCount);
  for(size_t i = 0; i < stateCount; i++) {
    const DFAstate &s = m_dfa.m_states[i];
    if(s.m_accept && !done.contains(i)) {
      for(size_t j = i; j < stateCount; j++) {
        if(m_dfa.m_states[j].m_accept == s.m_accept && !done.contains(j)) {
          writer.printf(_T("case %d:\n"), (int)j);
          done.add(j);
        }
      }
      writer.incrLeftMargin(2);
      writer.writeSourceText(*s.m_accept);
      if(writer.getFlags().m_generateBreaks) {
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
  void handleKeyword(TemplateWriter &writer, String &line) const;
};

TablesWriter::TablesWriter(const DFA &dfa) : m_dfa(dfa) {
}

void TablesWriter::handleKeyword(TemplateWriter &writer, String &line) const {
  const SourcePositionWithName pos = writer.getPos();
  const String tmpFileName = TemplateWriter::createTempFileName(_T("txt"));
  MarginFile f(tmpFileName);
  if(writer.getFlags().m_verbose) {
    m_dfa.printStates(f);
  }
  m_dfa.printTables(f);
  f.close();
  const String text = readTextFile(tmpFileName);
  unlink(tmpFileName);
  writer.printf(_T("%s"), text.cstr());
  if(writer.getFlags().m_lineDirectives) {
    writer.writeLineDirective(pos.getName(),pos.getLineNumber()+1);
  }
}

static void printOutputFiles(const String    &templateName
                            ,const String    &lexFileName
                            ,const DFA       &dfa
                            ,const CodeFlags &flags
                            ,const String    &implOutputDir
                            ,const String    &headerOutputDir
                            ,const String    &nameSpace
                            ) {

  String lexName    = FileNameSplitter(lexFileName).getFileName();
  String sourceName = FileNameSplitter(lexFileName).getAbsolutePath();

  TemplateWriter   writer(templateName, implOutputDir, headerOutputDir, flags);
  SourceTextWriter headerWriter(    dfa.getHeader()    );
  SourceTextWriter driverHeadWriter(dfa.getDriverHead());
  SourceTextWriter driverTailWriter(dfa.getDriverTail());
  ActionsWriter    actionsWriter(   dfa);
  TablesWriter     tablesWriter(    dfa);
  NewFileHandler   newFileHandler;

  writer.addKeywordHandler(_T("FILEHEAD"     ), headerWriter     );
  writer.addKeywordHandler(_T("CLASSHEAD"    ), driverHeadWriter );
  writer.addKeywordHandler(_T("CLASSTAIL"    ), driverTailWriter );
  writer.addKeywordHandler(_T("TABLES"       ), tablesWriter     );
  writer.addKeywordHandler(_T("ACTIONS"      ), actionsWriter    );
  writer.addKeywordHandler(_T("NEWFILE"      ), newFileHandler   );
  writer.addKeywordHandler(_T("NEWHEADERFILE"), newFileHandler   );
  writer.addMacro(_T("LEXNAME"  ), lexName        );
  writer.addMacro(_T("OUTPUTDIR"), implOutputDir  );
  writer.addMacro(_T("HEADERDIR"), headerOutputDir);
  writer.addMacro(_T("NAMESPACE"), nameSpace      );
  if(nameSpace.length() > 0) {
    writer.addMacro(       _T("PUSHNAMESPACE"      ), format(_T("namespace %s {"    ), nameSpace.cstr()));
    writer.addMacro(       _T("POPNAMESPACE"       ), format(_T("}; // namespace %s"), nameSpace.cstr()));
  } else {
    writer.addMacro(       _T("PUSHNAMESPACE"      ), EMPTYSTRING);
    writer.addMacro(       _T("POPNAMESPACE"       ), EMPTYSTRING);
  }
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
  String   implOutputDir   = _T(".");
  String   headerOutputDir = implOutputDir;
  String   templateName    = EMPTYSTRING;
  String   nameSpace       = EMPTYSTRING;
  CodeFlags flags;
  TCHAR   *wizardName      = EMPTYSTRING;
  Language language        = CPP;

  try {
    for(argv++; *argv && *(cp = *argv) == '-'; argv++) {
      for(cp++;*cp;cp++) {
        switch(*cp) {
        case 'm':
          templateName = cp+1;
          if(templateName.length() == 0) usage();
          break;
        case 'N':
          nameSpace = cp+1;
          if(nameSpace.length() == 0) {
            usage();
          }
          break;
        case 'l':
          flags.m_lineDirectives = false;
          continue;
        case 'b':
          flags.m_generateBreaks = false;
          continue;
        case 'v':
          flags.m_verbose        = true;
          { int level;
            if(_stscanf(cp+1, _T("%d"), &level) != 1) {
              continue;
            }
            switch(level) {
            case 2: flags.m_DFAVerbose = true; // continue case
            case 1: flags.m_dumpStates = true; // continue case
            case 0: flags.m_verbose    = true;
                    break;
            default: usage();
            }
          }
          break;
        case 'w':
          flags.m_callWizard = true;
          wizardName = cp+1;
          break;
        case 'O':
          { const String a = cp+1;
            Tokenizer tok(a, _T(","));
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
            break;
          }
        case 'j':
          language = JAVA;
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
    switch(language) {
    case CPP :
      skeletonFileName = _T("lexgencpp.par");
      wizardTemplate   = _T("lexgencpp.wzr");
      break;
    case JAVA:
      skeletonFileName = _T("lexgenjava.par");
      wizardTemplate   = _T("lexgenjava.wzr");
      flags.m_lineDirectives   = false;
      break;
    default  :
      usage();
      break;
    }

    if(flags.m_callWizard) {
      String className = FileNameSplitter(wizardName).getFileName();
      wizard(stdout, wizardTemplate, className);
    } else {
      if(!*argv) {
        usage();
      }
      String lexFileName = *argv;
      if(templateName.length() == 0) { // template not specified in argv
        templateName = searchenv(skeletonFileName, _T("LIB"));
        if(templateName.length() == 0) {
          throwException(_T("Template <%s> not found in environment LIB-path\n"), skeletonFileName.cstr());
        }
      } else { // -mS options used. Check if templatefile S exist
        if(ACCESS(templateName, 0) < 0) {
          throwException(_T("Template <%s> not found"), templateName.cstr());
        }
      }

      NFA nfa;
      NFAparser parser(lexFileName, nfa);

      if(flags.m_verbose) {
        _tprintf(_T("Parsing %s\n"), lexFileName.cstr());
      }
      parser.thompsonConstruction();

      if(flags.m_dumpStates) {
        for(size_t i = 0; i < nfa.size(); i++) {
          nfa[i]->print(stdoutMarginFile);
        }
      }

      if(flags.m_verbose) {
        _tprintf(_T("Constructing DFA\n"));
      }

      DFA dfa(nfa, language, flags.m_DFAVerbose);
      if(!flags.m_DFAVerbose && flags.m_dumpStates) {
        const size_t stateCount = dfa.m_states.size();
        for(size_t i = 0; i < stateCount; i++) {
          dfa.m_states[i].print(stdoutMarginFile);
        }
      }
      printOutputFiles(templateName
                     , lexFileName
                     , dfa
                     , flags
                     , implOutputDir
                     , headerOutputDir
                     , nameSpace
                     );

    }
  } catch(Exception e) {
    _ftprintf(stderr, _T("%s\n"), e.what());
    return -1;
  }
  return 0;
}
