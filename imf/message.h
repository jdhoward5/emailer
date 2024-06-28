#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#include "../types.h"

/* Written by 0xEBFE
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

#define MAXLINELEN 1000

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
    size_t len;
} message;

/*
 * Attempt to create an RFC 5322 message from an arbitrary byte buffer
*/
message * create_message(void *buf);

/*
 * Convert a line into an RFC 5322 header field
*/
field * parse_field(line *l);

#endif /* message_h_ */