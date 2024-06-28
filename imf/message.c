#include "message.h"


/*
 * Reads from a buffer until encountering an RFC 5322 compliant line-ending (CRLF)
 * Allocates a new buffer and handles reallocation/resizing
 * Returns NULL on error
 * On success returns a line struct
 * Lines over 1k bytes will be truncated.
 * NUL (\0) byte will also terminate. This is for stability; presence indicates
 * a malformed (non-compliant) message.
*/
line * _readline(byte *buf, bool unfoldable)
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
        if (unfoldable)
        {
            if (isspace(*b))
            {
                if (*(b+1) == '\0' || *(b+2) == '\0') break;
                if (*(b+1) == '\r' && *(b+2) == '\n')
                {
                    *i = *b;
                    i++;
                    c++;
                    b += 3;
                    continue;
                }
            }
        }
        if (*b == '\r' && (*(b+1) == '\0' || *(b+1) == '\n')) break;
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

/*
 * RFC 5322 permits very long fields to be "folded" or broken in to multiple lines.
 * The definition states a fold is delimited by any whitespace character 
 * inserted before a CRLF. Therefore, an unfolding operation is to remove CRLF
 * characters wherever they are immediately followed by a CRLF.
*/
byte * unfold(byte *buf)
{

}

void _freeline(line *l)
{
    free(l->s);
    free(l);
}

message * create_message(void *buf)
{
    //TODO
    return NULL;
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
   _freeline(l);
}

void _freefield(field *f)
{
    free(f->name);
    free(f->body);
    free(f);
}