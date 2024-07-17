#include "types.h"
#include "imf/message.h"

#define assert(A, B) ((A) == (B) ? TRUE : FALSE)
#define test(S, A, B) printf("%s: %s\r\n", S, ((assert(A, B)) ? "OK" : "FAIL"))

#ifndef printline
#define printline(L) printf("Line Length: %d\r\nContent: %s\r\n", L->len, L->s)
#endif

int main(void)
{
    printf("Beginning testing of IMF Functions\r\n");
    printf("Testing line reading...\r\n");
    byte *buf = (byte *)calloc(7, sizeof(byte));
    memcpy(buf, "Cat!\r\n", 6);
    line *l = readline(buf, TRUE);
    test("Testing read of valid line \"Cat!\"", l->len, strlen((char *)buf)-2);
    printline(l);
    freeline(l);
    free(buf);
    buf = (byte *)calloc(10, sizeof(byte));
    memcpy(buf, "ca\r\n t!\r\n", 9);
    l = readline(buf, TRUE);
    test("Testing a folded valid line \"Ca<FOLD>t!\"", l->len, 5);
    printline(l);
    freeline(l);
    free(buf);
    return 0;
}