#include "message.h"

/*
 * Args: buf; unfoldable
 * -buf: Pointer to a buffer of "bytes" (unsigned chars)
 * -unfoldable: Boolean, is this line unfoldable or not?
 * Description:
 * Reads from a buffer until encountering an RFC 5322 compliant line-ending (CRLF)
 * Allocates a new buffer and handles reallocation/resizing
 * Returns NULL on error
 * On success returns a line struct
 * Lines over 1k bytes will be truncated.
 * NUL (\0) byte will also terminate. This is for stability; presence indicates
 * a malformed (non-compliant) message.
*/
line * readline(byte *buf, bool unfoldable)
{
    size_t c = 0;
    size_t max = 100;
    line *line_s = (line *)malloc(sizeof(line));
    byte *linebuf = (byte *)calloc(max, sizeof(byte));
    if (line_s == NULL || linebuf == NULL) return NULL;
    byte *i = linebuf;
    byte *b = buf;
    for (;;)
    {
        if (c >= MAXLINELEN-1) break;
        if (c >= max-1)
        {
            max += 100;
            linebuf = realloc(linebuf, max);
            if (linebuf == NULL) return NULL;
            i = linebuf+c;
        }
        if (*b == '\0') break;
        if (*b == '\r' && (*(b+1) == '\0' || *(b+1) == '\n'))
        {
            if (*(b+1) == '\n' && unfoldable && *(b+2) && isspace(*(b+2)))
            {
                b += 2;
            } else
            {
                break;
            }
        }
        *i = *b;
        b++;
        i++;
        c++;
    }
    *(i+1) = '\0';
    if(c > 0)
    {
        linebuf = realloc(linebuf, c+1);
    }
    else 
    {
        linebuf = realloc(linebuf, sizeof(byte));
        *linebuf = '\0';
    }
    line_s->s = linebuf;
    line_s->len = c;
    return line_s;
}

void freeline(line *l)
{
    free(l->s);
    free(l);
}

/*
    * Convert a line struct into a header field struct
    * Split on ':' (colon) character
    * Will free the line struct afterwards
*/
field * parse_field(line *l) 
{
   if(l->len == 0) return NULL;
   field *f = (field *)malloc(sizeof(field));
   size_t max = l->len;
   size_t len = 0;
   f->name->s = (byte *)calloc(max, sizeof(byte));
   f->body->s = (byte *)calloc(max, sizeof(byte));
   if(f == NULL || f->name == NULL || f->body == NULL) return NULL;
   byte *cur = f->name->s;
   for(size_t i = 0; i < max; i++)
   {
    if((l->s)[i] == ':')
    {
        *(cur+1) = '\0';
        f->name->len = len;
        len = 0;
        cur = f->body->s;
        continue;
    }
    *cur = (l->s)[i];
    cur++;
    len++;
   }
   f->body->len = len;
   *(cur+1) = '\0';
   f->name->s = (byte *)realloc(f->name->s, f->name->len+1);
   f->body->s = (byte *)realloc(f->body->s, f->body->len+1);
   freeline(l);
}

void _freefield(field *f)
{
    free(f->name);
    free(f->body);
    free(f);
}