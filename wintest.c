#include "types.h"
#include "imf/message.h"

#define assert(A, B) ((A) == (B) ? TRUE : FALSE)
#define test(S, A, B) printf("%s: %s\r\n", S, ((assert(A, B)) ? "OK" : "FAIL"))

#ifndef printline
#define printline(L) printf("Line Length: %d\r\nContent: %s\r\n", L->len, L->s)
#endif
#ifndef printfield
#define printfield(F) printf("\tField Name: %s\r\n\tField Body: %s\r\n", F->name->s, F->body->s)
#endif

int main(void)
{
    printf("Beginning testing of IMF Functions\r\n");
    printf("Testing line reading...\r\n");
    byte *buf = (byte *)calloc(7, sizeof(byte));
    memcpy(buf, "Cat!\r\n", 6);
    line *l = readline(buf, TRUE);
    test("Testing read of valid line \"Cat!\"", l->len, strlen((char *)buf)-2);
    freeline(l);
    free(buf);
    buf = (byte *)calloc(10, sizeof(byte));
    memcpy(buf, "ca\r\n t!\r\n", 9);
    l = readline(buf, TRUE);
    test("Testing a folded valid line \"Ca<FOLD>t!\"", l->len, 5);
    freeline(l);
    free(buf);
    buf = (byte *)calloc(1000, sizeof(byte));
    memset(buf, 'a', 999);
    l = readline(buf, TRUE);
    test("Testing a line one larger than the valid limit", strlen(l->s), MAXLINELEN);
    freeline(l);
    free(buf);
    buf = (byte *)calloc(6, sizeof(byte));
    memcpy(buf, "Ca\0t!", 5);
    l = readline(buf, TRUE);
    test("Testing a non-compliant line that terminates early (Ca<NULL>t!)", strlen(l->s), 2);
    freeline(l);
    free(buf);
    buf = (byte *)calloc(7, sizeof(byte));
    memcpy(buf, "Ca\r\nt!", 6);
    l = readline(buf, TRUE);
    test("Testing a compliant line with extraneous data (Ca<CRLF>t!)", strlen(l->s), 2);
    freeline(l);
    free(buf);
    puts("Line test complete!\r\nBeginning field test...\r\n");
    l = readline("0123: 67\r\n 89\r\n", TRUE);
    field *f = parse_field(l);
    printfield(f);
    freefield(f);
    return 0;
}