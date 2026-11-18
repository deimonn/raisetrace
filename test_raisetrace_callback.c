#include "raisetrace.h"

#include <stdlib.h>
#include <stdio.h>

static void segfault(volatile char *x)
{
    *x = 0;
}

static int custom_callback(int num, const char *msg,
                           struct backtrace_state *state)
{
    (void)num;
    (void)state;

    fputs(msg, stderr);
    fputc('\n', stderr);

    return RAISETRACE_DEFAULT;
}

int main(void)
{
    raisetrace_callback = custom_callback;
    raisetrace(raisetrace_faults);
    segfault(NULL);
}
