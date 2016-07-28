// lexgen.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <direct.h>
#include <io.h>
#include "DFA.h"
#include "Wizard.h"
#include <TemplateWriter.h>

class ActionsWriter : public KeywordHandler {
  const DFA &m_dfa;
  bool       m_lineDirectives;
  bool       m_generateBreaks;
private:
public:
  ActionsWriter(const DFA &dfa, bool lineDirectives, bool generateBreaks);
  void handleKeyword(TemplateWriter &writer, String &line) const;
};

ActionsWriter::ActionsWriter(const DFA &dfa, bool lineDirectives, bool generateBreaks) : m_dfa(dfa) {
  m_lineDirectives = lineDirectives;
  m_generateBreaks = generateBreaks;
}

void ActionsWriter::handleKeyword(TemplateWriter &writer, String &line) const {
  BitSet done(m_dfa.m_states.size());
  MarginFile &output = writer.getOutput();
  int indent = output.getLeftMargin();
  for(size_t i = 0; i < m_dfa.m_states.size(); i++) {
    const DFAstate &s = m_dfa.m_states[i];
    if(s.m_accept && !done.contains(i)) {
      output.setLeftMargin(indent);
      for(size_t j = i; j < m_dfa.m_states.size(); j++) {
        if(m_dfa.m_states[j].m_accept == s.m_accept && !done.contains(j)) {
          output.printf(_T("case %d:\n"), (int)j);
          done.add(j);
        }
      }
      output.setLeftMargin(indent+2);
      writeSourceText(output, *s.m_accept, writer.getPos(), m_lineDirectives);
      if(m_generateBreaks) {
        output.printf(_T("break;\n"));
      }
    }
  }
}

class TablesWriter : public KeywordHandler {
  const DFA &m_dfa;
  bool       m_verbose;
  bool       m_lineDirectives;
public:
  TablesWriter(const DFA &dfa, bool verbose, bool lineDirectives);
  void handleKeyword(TemplateWriter &writer, String &line) const;
};

TablesWriter::TablesWriter(const DFA &dfa, bool verbose, bool lineDirectives) : m_dfa(dfa) {
  m_verbose        = verbose;
  m_lineDirectives = lineDirectives;
}

void TablesWriter::handleKeyword(TemplateWriter &writer, String &line) const {
  if(m_verbose) {
    m_dfa.printStates(writer.getOutput());
  }
  m_dfa.printTables(writer.getOutput());
  if(m_lineDirectives) {
    writeLineDirective(writer.getOutput(), writer.getPos().getFileName(), writer.getPos().getLineNumber()+1);
  }
}


static void printOutputFiles(const String &templateName
                            ,const String &lexFileName
                            ,const DFA    &dfa
                            ,bool          verbose
                            ,bool          lineDirectives
                            ,bool          generateBreaks
                            ,const String &implOutputDir
                            ,const String &headerOutputDir
                            ) {

  String lexName    = FileNameSplitter(lexFileName).getFileName();
  String sourceName = FileNameSplitter(lexFileName).getAbsolutePath();

  TemplateWriter writer(templateName, implOutputDir, headerOutputDir, verbose);
  SourceTextWriter headerWriter(    dfa.getHeader()    , lineDirectives);
  SourceTextWriter driverHeadWriter(dfa.getDriverHead(), lineDirectives);
  SourceTextWriter driverTailWriter(dfa.getDriverTail(), lineDirectives);
  ActionsWriter    actionsWriter(   dfa, lineDirectives, generateBreaks);
  TablesWriter     tablesWriter(    dfa, verbose, lineDirectives);
  NewFileHandler   newFileHandler;

  writer.addKeywordHandler("FILEHEAD"     , headerWriter     );
  writer.addKeywordHandler("CLASSHEAD"    , driverHeadWriter );
  writer.addKeywordHandler("CLASSTAIL"    , driverTailWriter );
  writer.addKeywordHandler("TABLES"       , tablesWriter     );
  writer.addKeywordHandler("ACTIONS"      , actionsWriter    );
  writer.addKeywordHandler("NEWFILE"      , newFileHandler   );
  writer.addKeywordHandler("NEWHEADERFILE", newFileHandler   );
  writer.addMacro("LEXNAME"  , lexName);
  writer.addMacro("OUTPUTDIR", implOutputDir);
  writer.addMacro("HEADERDIR", headerOutputDir);
  writer.generateOutput();
}

static void usage() {
  fprintf(stderr, 
    "Usage:lexgen [options] input.\n"
    " -mS: Use String S as template rather than lexgenXXX.par.\n"
    " -l : Suppress #line directives in output.\n"
    " -b : Suppress break-statements in output.\n"
    " -v[level] :Verbose.\n"
    "    level: 0: print DFA-states to generated cpp-file.\n"
    "         : 1: dump NFA- and DFA-states to stdout.\n"
    "         : 2: dump DFA-construction steps stdout.\n"
    "    Default level = 0.\n"
    " -wS: lex-wizard. write template lex-file with classname Slex to stdout.\n"
	" -j : Generate java-lex. Default is C++.\n"
    " -Ooutputdir1[,outputdir2]: Output goes to outputdir1. If outputdir2 specified, .h-files will go here.\n"
  );
  exit(-1);
}

int main(int argc, char **argv) {
  char *cp;
  String implOutputDir   = _T(".");
  String headerOutputDir = implOutputDir;
  String templateName    = _T("");
  bool   verbose         = false;
  bool   generateBreaks  = true;
  bool   dumpStates      = false;
  bool   DFAVerbose      = false;
  bool   lineDirectives  = true;
  bool   callWizard      = false;
  char  *wizardName      = "";
  Language language      = CPP;

  try {
    for(argv++; *argv && *(cp = *argv) == '-'; argv++) {
      for(cp++;*cp;cp++) {
        switch(*cp) {
        case 'm': 
          templateName = cp+1;
          if(templateName.length() == 0) usage();
          break;
        case 'l':
          lineDirectives = false;
          continue;
        case 'b':
          generateBreaks = false;
          continue;
        case 'v':
          verbose        = true;
          { int level;
            if(sscanf(cp+1, "%d", &level) != 1)
              continue;
            switch(level) {
            case 2: DFAVerbose = true; // continue case
            case 1: dumpStates = true; // continue case
            case 0: verbose    = true;
                    break;
            default: usage();
            }
          }
          break;
        case 'w':
          callWizard = true; 
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
        }
        break;
      }
    }

    String skeletonFileName = _T("");
    String wizardTemplate   = _T("");
    switch(language) {
	case CPP :
      skeletonFileName = _T("lexgencpp.par");
      wizardTemplate   = _T("lexgencpp.wzr");
      break;
	case JAVA:
      skeletonFileName = _T("lexgenjava.par");
      wizardTemplate   = _T("lexgenjava.wzr");
	  lineDirectives   = false;
      break;
    default  :
      usage();
      break;
    }

    if(callWizard) {
      String className = FileNameSplitter(wizardName).getFileName();
      wizard(stdout, wizardTemplate, className);
    } else {
      if(!*argv) {
        usage();
      }
      String lexFileName = *argv;
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

      NFA nfa;
      NFAparser parser(lexFileName, nfa);

      if(verbose) {
        _tprintf(_T("Parsing %s\n"), lexFileName.cstr());
      }
      parser.thompsonConstruction();

      if(dumpStates) {
        for(size_t i = 0; i < nfa.size(); i++) {
          nfa[i]->print(stdoutMarginFile);
        }
      }

      if(verbose) {
        _tprintf(_T("Constructing DFA\n"));
      }

      DFA dfa(nfa, language, DFAVerbose);
      if(!DFAVerbose && dumpStates) {
        for(size_t i = 0; i < dfa.m_states.size(); i++) {
          dfa.m_states[i].print(stdoutMarginFile);
        }
      }

      printOutputFiles(templateName
                     , lexFileName
                     , dfa
                     , verbose
                     , lineDirectives
                     , generateBreaks
                     , implOutputDir
                     , headerOutputDir
                     );

    }
  } catch(Exception e) {
    _ftprintf(stderr, _T("%s\n"), e.what());
    return -1;
  }
  return 0;
}
