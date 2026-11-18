#include "raisetrace.h"

#include <stdlib.h>

static void segfault(volatile char *x)
{
    *x = 0;
}

int main(void)
{
    raisetrace(raisetrace_faults);
    segfault(NULL);
}
