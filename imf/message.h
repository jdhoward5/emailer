/* Written by Eagle
 * C API for the implementation of the Internet Message Format (IMF)
 * Based on RFC 5322 (and compliant with said RFC)
*/

/*
 * From RFC 5322:
 * Messages are a series of ASCII characters (see MIME for anything fancier)
 * Messages are divided into lines of characters.
 * A line is a series of characters delimited by a carriage return (CR) and line-feed (LF)
 * A message consists of a header followed (optionally) by a body
 * The header is a sequence of lines with a special syntax.
 * The body is a series of lines that follows the header, and is separated by a blank line
 * 
*/

#ifndef message_h_
#define message_h_

#ifdef _WIN32
#include <Windows.h>
#include <stdio.h>
#else
#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#endif
#include "../types.h"

/* RFC 5322 mandates each line be no more than 998 characters (excluding CRLF) */
#define MAXLINELEN 998

typedef struct _line {
    byte *s;
    size_t len;
} line;

typedef struct _field {
    line *name;
    line *body;
} field;

typedef struct _header {
    field **fields;
    size_t len;
} header;

typedef struct _message {
    header *hdr;
    line **body;
    size_t n_lines;
} message;


line * readline(byte *buf, bool unfoldable);
void freeline(line *l);
field * parse_field(line *l);
void freefield(field *f);

#endif /* message_h_ */