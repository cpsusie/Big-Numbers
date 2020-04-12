#include "pch.h"
#include <Math/Triangle/Triangle.h>

#ifndef TRILIBRARY

/*****************************************************************************/
/*                                                                           */
/*  readline()   Read a nonempty line from a file.                           */
/*                                                                           */
/*  A line is considered "nonempty" if it contains something that looks like */
/*  a number.  Comments (prefaced by `#') are ignored.                       */
/*                                                                           */
/*****************************************************************************/
char *readline(char *string, FILE *infile, char *infilename) {
  char *result;

  /* Search for something that looks like a number. */
  do {
    result = fgets(string, INPUTLINESIZE, infile);
    if(result == NULL) {
      triError(_T("Unexpected end of file in %s"), String(infilename).cstr());
    }
    __assume(result);
    /* Skip anything that doesn't look like a number, a comment, */
    /*   or the end of a line.                                   */
    while((*result != '\0') && (*result != '#')
        && (*result != '.') && (*result != '+') && (*result != '-')
        && ((*result < '0') || (*result > '9'))) {
      result++;
    }
  /* If it's a comment or end of line, read another line and try again. */
  } while((*result == '#') || (*result == '\0'));
  return result;
}

/*****************************************************************************/
/*  findfield()   Find the next field of a string.                           */
/*                                                                           */
/*  Jumps past the current field by searching for whitespace, then jumps     */
/*  past the whitespace to find the next field.                              */
/*****************************************************************************/
char *findfield(char *string) {
  char *result;

  result = string;
  /* Skip the current field.  Stop upon reaching whitespace. */
  while((*result != '\0') && (*result != '#') && (*result != ' ') && (*result != '\t')) {
    result++;
  }
  /* Now skip the whitespace and anything else that doesn't look like a */
  /*   number, a comment, or the end of a line.                         */
  while((*result != '\0') && (*result != '#')
         && (*result != '.') && (*result != '+') && (*result != '-')
         && ((*result < '0') || (*result > '9'))) {
    result++;
  }
  /* Check for a comment (prefixed with `#'). */
  if(*result == '#') {
    *result = '\0';
  }
  return result;
}

#endif
