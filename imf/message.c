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
    size_t linelen = 0;
    line *line_s = (line *)malloc(sizeof(line));
    byte *linebuf = (byte *)calloc(max, sizeof(byte));
    if (line_s == NULL || linebuf == NULL) return NULL;
    byte *i = linebuf;
    byte *b = buf;
    for (;;)
    {
        if (c >= max-1)
        {
            max += 100;
            linebuf = realloc(linebuf, max);
            if (linebuf == NULL) return NULL;
            i = linebuf+c;
        }
        if (linelen >= MAXLINELEN) break;
        if (*b == '\0') break;
        if (*b == '\r' && (*(b+1) == '\0' || *(b+1) == '\n'))
        {
            if (*(b+1) == '\n' && unfoldable && *(b+2) && isspace(*(b+2)))
            {
                b += 2;
                linelen = 0;
            } else
            {
                break;
            }
        }
        *i = *b;
        b++;
        i++;
        c++;
        linelen++;
    }
    *i = '\0';
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

/* 
 * Args: l
 * -l: pointer to a line structure
 * Description:
 * Free a line structure and all buffers
*/
void freeline(line *l)
{
    free(l->s);
    l->len = 0;
    free(l);
}

/*
 * Args: l
 * -l: pointer to a line structure containing a valid field.
 * Description:
 * Convert a line struct into a header field struct
 * Split on ':' (colon) character
 * Will free the line struct afterwards
 * Returns NULL on non-compliant field (no colon)
 * Do NOT try to reuse the line structure passed in, it is FREED.
*/
field * parse_field(line *l) 
{
    bool valid = FALSE;
    byte *b = l->s;
    if(l->len == 0) return NULL;
    field *f = (field *)malloc(sizeof(field));
    f->name = (line *)malloc(sizeof(line));
    f->body = (line *)malloc(sizeof(line));
    if (f == NULL || f->name == NULL || f->body == NULL) return NULL;
    for (size_t i = 0; i < l->len; i++)
    {
        b++;
        if ((l->s)[i] == ':')
        {
            valid = TRUE;
            (l->s)[i] = '\0';
            break;
        }
    }
    if (!valid)
    {
        free(f->name);
        free(f->body);
        free(f);
        return NULL;
    }
    if (isspace(*b)) b++;
    f->name = readline(l->s, TRUE);
    f->body = readline(b, TRUE);
    freeline(l);
    return f;
}

/*
 * Args: f
 * -f: Pointer to a field structure to free
 * Description:
 * Free the memory used by a field structure
*/
void freefield(field *f)
{
    freeline(f->name);
    freeline(f->body);
    free(f);
}